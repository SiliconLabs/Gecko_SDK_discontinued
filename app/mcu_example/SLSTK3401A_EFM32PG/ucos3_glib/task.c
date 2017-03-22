/**************************************************************************//**
 * @file task.c
 * @brief This file contains all the application tasks
 * @version 5.1.2
 *
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <cpu.h>
#include <lib_mem.h>
#include <os.h>

#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"

#include "gpiointerrupt.h"
#include "hooks.h"
#include "task_cfg.h"
#include "bsp.h"
#include "bspconfig.h"
#include "bsp_os.h"
#include "glib.h"
#include "displayls013b7dh03config.h"
#include "image.h"

#include <string.h>

/********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 *******************************************************************************************************/

static OS_TCB   TASK_StartTCB;
static CPU_STK  TASK_StartStk[TASK_CFG_START_STK_SIZE];

static OS_TCB   TASK_ToggleTCB;
static CPU_STK  TASK_ToggleStk[TASK_CFG_TOGGLE_STK_SIZE];

static OS_TCB   TASK_DisplayTCB;
static CPU_STK  TASK_DisplayStk[TASK_CFG_DISPLAY_STK_SIZE];

static OS_TCB   TASK_ImageTCB;
static CPU_STK  TASK_ImageStk[TASK_CFG_IMAGE_STK_SIZE];

static OS_MUTEX     displayMutex;
static OS_FLAG_GRP  buttonsFlags;

static GLIB_Context_t glibContextTop;
static GLIB_Context_t glibContextBottom;

/********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 *******************************************************************************************************/

static void TASK_Start(void *p_arg);
static void TASK_Toggle(void *p_arg);
static void TASK_Display(void *p_arg);
static void TASK_Image(void *p_arg);

static void TASK_Create(void);
static void TASK_ObjCreate(void);

/**************************************************************************//**
 * @brief
 *   GPIO callback function.
 *
 * @details
 *   This function is called by the gpio interrupt driver in interrupt context.
 *****************************************************************************/
static void gpioCallback(uint8_t pin)
{
  OS_ERR err;
  OSFlagPost(&buttonsFlags, 0x1 << pin, OS_OPT_POST_FLAG_SET, &err);
}

/**************************************************************************//**
 * @brief
 *   Setup the gpio interrupt driver and enable interrupts for the two
 *   push buttons on the kit.
 *****************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO in CMU. */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Initialize GPIO interrupt dispatcher. */
  GPIOINT_Init();

  /* Configure PB0 and PB1 as input. */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInput, 0);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInput, 0);

  /* Reg callbacks before setting up and enabling ints.   */
  GPIOINT_CallbackRegister(BSP_GPIO_PB0_PIN, gpioCallback);
  GPIOINT_CallbackRegister(BSP_GPIO_PB1_PIN, gpioCallback);

  /* Set falling edge interrupt for both ports. */
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);
}

/**************************************************************************//**
 * @brief
 *   Initialize the operating system and create the initial task.
 *****************************************************************************/
void TASK_Init(void)
{
  OS_ERR  err;

  /* Setup GPIO interrupts for the buttons */
  gpioSetup();

  /* Initialize Memory Managment Module */
  Mem_Init();

  /* Disable all Interrupts */
  CPU_IntDis();

  /* Init uC/OS-III */
  OSInit(&err);

  App_OS_SetAllHooks();

  /* Create the initial task */
  OSTaskCreate(&TASK_StartTCB,
                "Start Task",
                TASK_Start,
                0u,
                TASK_CFG_START_PRIO,
               &TASK_StartStk[0u],
                TASK_CFG_START_STK_SIZE / 10u,
                TASK_CFG_START_STK_SIZE,
                0u,
                0u,
                0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);

  if (err != OS_ERR_NONE) {
    CPU_SW_EXCEPTION(;);
  }
}

/**************************************************************************//**
 * @brief
 *   This task is the startup task. It sets up the tick and the statistics
 *   task and has responsibility to start the rest of the application tasks.
 *****************************************************************************/
static void TASK_Start(void *p_arg)
{
  (void)p_arg;
  OS_ERR err;

  BSP_OSTickInit();
  BSP_LedsInit();

  /* Initialize the uC/CPU services */
  CPU_Init();

#if OS_CFG_STAT_TASK_EN == DEF_ENABLED
  /* Initialize the statistics task which monitors the task stack usage */
  OSStatTaskCPUUsageInit(&err);
  OSStatReset(&err);
#endif

  /* Create shared objects used by the tasks */
  TASK_ObjCreate();

  /* Create the rest of the tasks */
  TASK_Create();

  while (DEF_TRUE)
  {
    BSP_LedToggle(0);    
    OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
  }
}

/**************************************************************************//**
 * @brief
 *   This task runs at 60Hz and is responsible for toggling the LCD EXTCOMIN
 *   pin which is a requirement for driving the Sharp LCD memory display.
 *****************************************************************************/
static void TASK_Toggle(void *p_arg)
{
  (void) p_arg;
  OS_ERR err;

  GPIO_PinModeSet(LCD_PORT_EXTCOMIN, LCD_PIN_EXTCOMIN, gpioModePushPull, 0);
  while (DEF_TRUE)
  {
    GPIO_PinOutToggle(LCD_PORT_EXTCOMIN, LCD_PIN_EXTCOMIN);
    OSTimeDly(16, OS_OPT_TIME_PERIODIC, &err);
  }
}

/**************************************************************************//**
 * @brief
 *   This task displays a text on the screen and animates a bounching ball
 *   in the top region of the screen.
 *****************************************************************************/
static void TASK_Display(void *p_arg)
{
  OS_ERR err;
  static const char * msg = "uC/OS-III + GLIB";
  static GLIB_Rectangle_t view = {0, 0, 127, 63};
  int32_t x = 20;
  int32_t y = 20;
  int32_t dx = 1;
  int32_t dy = 1;
  static const int r = 4;
  GLIB_Context_t * ctx = &glibContextTop;

  (void) p_arg;
  /* Initialize the GLIB context. */
  GLIB_contextInit(ctx);
  ctx->backgroundColor = Black;
  ctx->foregroundColor = White;
    
  while (DEF_TRUE)
  {
    OSMutexPend(&displayMutex, 0, OS_OPT_PEND_BLOCKING, (CPU_TS *)0, &err);
    EFM_ASSERT(err == OS_ERR_NONE);
    
    /* Clear section of the screen */
    ctx->foregroundColor = Black;
    GLIB_drawRectFilled(ctx, &view);
    ctx->foregroundColor = White;
    /* Draw content */
    GLIB_drawString(ctx, (char *)msg, strlen(msg), 0, 0, true);
    GLIB_drawCircleFilled(ctx, x, y, r);
    DMD_updateDisplay();

    OSMutexPost(&displayMutex, OS_OPT_POST_NONE, &err);
    EFM_ASSERT(err == OS_ERR_NONE);

    /* Delay task until next frame should be drawn. */
    OSTimeDly(33, OS_OPT_TIME_PERIODIC, &err);

    /* Collision detection */
    int32_t xEdgeMin = x - r;
    int32_t xEdgeMax = x + r;
    int32_t yEdgeMin = y - r;
    int32_t yEdgeMax = y + r;
    /* Update position */
    if ((xEdgeMax >= view.xMax) || (xEdgeMin < view.xMin)) {
      dx = -dx;
    }
    if ((yEdgeMax >= view.yMax) || (yEdgeMin < view.yMin)) {
      dy = -dy;
    }
    x += dx;
    y += dy;
  }
}

/**************************************************************************//**
 * @brief
 *   This task displays an image in the bottom section of the display.
 *   The image can be changed by pressing the push buttons.
 *****************************************************************************/
static void TASK_Image(void *p_arg)
{
  OS_ERR err;
  EMSTATUS status;
  static GLIB_Rectangle_t view = {0, 64, 127, 127};
  GLIB_Context_t * ctx = &glibContextBottom;
  size_t i = 0;
  
  (void) p_arg;
  /* Initialize the GLIB context. */
  status = GLIB_contextInit(ctx);
  EFM_ASSERT(GLIB_OK == status);
  ctx->backgroundColor = White;
  ctx->foregroundColor = Black;
  
  while (DEF_TRUE)
  {
    OSMutexPend(&displayMutex, 0, OS_OPT_PEND_BLOCKING, (CPU_TS *)0, &err);
    EFM_ASSERT(err == OS_ERR_NONE);
    
    /* Clear bottom section of the screen */
    ctx->foregroundColor = White;
    GLIB_drawRectFilled(ctx, &view);
    ctx->foregroundColor = Black;
    GLIB_drawBitmap(ctx,
                    view.xMin,
                    view.yMin,
                    view.xMax - view.xMin + 1,
                    view.yMax - view.yMin + 1,
                    (uint8_t *)images[i]);
    DMD_updateDisplay();
    
    OSMutexPost(&displayMutex, OS_OPT_POST_NONE, &err);
    EFM_ASSERT(err == OS_ERR_NONE);

    /* Wait for any button to be pressed */
    OSFlagPend(&buttonsFlags,
               (0x1 << BSP_GPIO_PB0_PIN) | (0x1 << BSP_GPIO_PB1_PIN),
               0,
               OS_OPT_PEND_FLAG_SET_ANY + OS_OPT_PEND_FLAG_CONSUME,
               (CPU_TS *)0,
               &err);
    EFM_ASSERT(err == OS_ERR_NONE);
    
    /* Toggle image */
    i = (i + 1) % IMAGE_COUNT;
  }
}

/**************************************************************************//**
 * @brief
 *   Create the application tasks
 *****************************************************************************/
static void TASK_Create(void)
{
  OS_ERR err;
  
  /* Create the toggle task */
  OSTaskCreate(&TASK_ToggleTCB,
                "Toggle Task",
                TASK_Toggle,
                0u,
                TASK_CFG_TOGGLE_PRIO,
               &TASK_ToggleStk[0u],
                TASK_CFG_TOGGLE_STK_SIZE / 10u,
                TASK_CFG_TOGGLE_STK_SIZE,
                0u,
                0u,
                0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);

  if (err != OS_ERR_NONE) {
    CPU_SW_EXCEPTION(;);
  }
  
  /* Create the display task */
  OSTaskCreate(&TASK_DisplayTCB,
                "Display Task",
                TASK_Display,
                0u,
                TASK_CFG_DISPLAY_PRIO,
               &TASK_DisplayStk[0u],
                TASK_CFG_DISPLAY_STK_SIZE / 10u,
                TASK_CFG_DISPLAY_STK_SIZE,
                0u,
                0u,
                0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);

  if (err != OS_ERR_NONE) {
    CPU_SW_EXCEPTION(;);
  }

  /* Create the image task */
  OSTaskCreate(&TASK_ImageTCB,
                "Image Task",
                TASK_Image,
                0u,
                TASK_CFG_IMAGE_PRIO,
               &TASK_ImageStk[0u],
                TASK_CFG_IMAGE_STK_SIZE / 10u,
                TASK_CFG_IMAGE_STK_SIZE,
                0u,
                0u,
                0u,
               (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
               &err);

  if (err != OS_ERR_NONE) {
    CPU_SW_EXCEPTION(;);
  }
}

/**************************************************************************//**
 * @brief
 *   Create application kernel objects.
 *****************************************************************************/
static void TASK_ObjCreate(void)
{
  OS_ERR err;
  OSMutexCreate(&displayMutex, "Display Mutex", &err);
  if (err != OS_ERR_NONE) {
    CPU_SW_EXCEPTION(;);
  }

  OSFlagCreate(&buttonsFlags, "Push Buttons Status", (OS_FLAGS)0, &err);
  if (err != OS_ERR_NONE) {
    CPU_SW_EXCEPTION(;);
  }
}
