/**************************************************************************//**
 * @file spaceinvaders.c
 * @brief Spaceinvaders game.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"

#include "bspconfig.h"

#include "display.h"
#define DISPLAY_DEVICE_NO       (0)

#include "drivers/capsense_spaceinvaders.h"

#include "spaceinvaders.h"

#include "img/qrcode.xbm"
#include "img/splash.xbm"


#include "game.h"
#include "render.h"

#define FRAME_UPDATE_PERIOD     20
#define CAPSENSE_UPDATE_PERIOD  5
#define GAME_START_TIMEOUT      1000
#define GAME_OVER_TIMEOUT       1500

typedef enum _gameState
{
   qrCode,
   qrCodeWait,
   welcomeScreen,
   welcomeScreenWait,
   gameStart,
   gameStartWait,
   gameRunning,
   gamePaused,
   gamePausedWait,
   gameOver,
   gameVictory
} GameState;


static bool gameInitiated = false;
volatile bool gameUpdateFlag, pb0Pressed, pb1Pressed;
volatile GameState state, nextState;
volatile int timeout = 0;
static DISPLAY_Device_t displayDevice;

static void gpioSetup(void);
static void showWelcomeScreen( void );
void GPIO_Unified_IRQ(void) ;

/**************************************************************************//**
 * @brief Timer Event Handler function for the Space Invaders game.
 *        This function must be called periodically, preferably
 *        from within the interrupt routine of a timer or the
 *        SysTick timer each millisecond.
 *****************************************************************************/
void SPACEINVADERS_TimerEventHandler(void)
{
   static int frameUpdateCount    = 0;
   static int capsenseUpdateCount = 0;

   /* Return at once if init has not been called */
   if (!gameInitiated)
   {
      return;
   }

   if (++capsenseUpdateCount == CAPSENSE_UPDATE_PERIOD)
   {
      capsenseUpdateCount = 0;

      /* Update capsense module */
      CAPSENSE_timerEventHandler();
   }

   if (++frameUpdateCount == FRAME_UPDATE_PERIOD)
   {
      frameUpdateCount = 0;

      /* Update state */
      state = nextState;

      /* Set update flag for game */
      gameUpdateFlag = true;
   }

    /* Update wait timer */
    if (timeout > 0)
    {
       timeout--;
    }

   return;
}

/**************************************************************************//**
* @brief Setup GPIO interrupt for pushbuttons.
*****************************************************************************/
static void gpioSetup(void)
{
  /* Enable GPIO clock */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PB0 as input and enable interrupt  */
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, false, true, true);

  /* Configure PB1 as input and enable interrupt */
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPull, 1);
  GPIO_IntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
  NVIC_EnableIRQ(GPIO_ODD_IRQn);
}

/**************************************************************************//**
* @brief Unified GPIO Interrupt handler (pushbuttons)
*        PB0 Starts selected test
*        PB1 Cycles through the available tests
*****************************************************************************/
void GPIO_Unified_IRQ(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Act on interrupts */
  if (interruptMask & (1 << BSP_GPIO_PB0_PIN))
  {
    /* PB0 */
    pb0Pressed = true;
  }

  if (interruptMask & (1 << BSP_GPIO_PB1_PIN))
  {
    /* PB1 */
    pb1Pressed = true;
  }
}

/**************************************************************************//**
* @brief GPIO Interrupt handler for even pins
*****************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}

/**************************************************************************//**
* @brief GPIO Interrupt handler for odd pins
*****************************************************************************/
void GPIO_ODD_IRQHandler(void)
{
  GPIO_Unified_IRQ();
}


/**************************************************************************//**
 * @brief Initiate the Space Invaders game
 *****************************************************************************/
void SPACEINVADERS_Init( void )
{
   EMSTATUS status;

   /* Initialize the DISPLAY driver. */
   DISPLAY_Init();

   /* Retrieve the properties of the DISPLAY. */
   status = DISPLAY_DeviceGet(DISPLAY_DEVICE_NO, &displayDevice);
   if (DISPLAY_EMSTATUS_OK != status)
     return;

   /* Configure push button interrupts */
   gpioSetup();

   /* Start capacitive sense buttons */
   CAPSENSE_Init();

   /* Set initial game state */
   state = qrCode;
   nextState = state;

   gameInitiated = true;
}

static void showWelcomeScreen( void )
{
   RENDER_SetFramebuffer( splash );
   RENDER_Write( 4, 96,  "PB0   : Play/Pause" );
   RENDER_Write( 4, 104, "PB1   : Fire missile" );
   RENDER_Write( 4, 112, "Touch : Move tank" );
}

/**************************************************************************//**
 * @brief The Space Invaders game loop
 *        Control will never return from this function.
 *        It implements the main state machine.
 *****************************************************************************/
void SPACEINVADERS_GameLoop( void )
{
   int level = 1;
   int status;

   while(1)
   {
      if (gameUpdateFlag)
      {
         gameUpdateFlag = false;

         switch(state)
         {
         case qrCode:
            RENDER_SetFramebuffer( qrcode );
            RENDER_UpdateDisplay( true, &displayDevice );
            nextState = qrCodeWait;
            break;

         case qrCodeWait:
            if (CAPSENSE_getPressed(3) || CAPSENSE_getPressed(4) || pb0Pressed || pb1Pressed)
            {
               pb0Pressed = false;
               pb1Pressed = false;
               nextState = welcomeScreen;
            }
            break;

         case welcomeScreen:
            /* Draw the splash screen and wait for button press */
            showWelcomeScreen();
            RENDER_UpdateDisplay( true, &displayDevice );
            nextState = welcomeScreenWait;
            break;

         case welcomeScreenWait:
            /* Start the game when PB0 is pressed */
            if (pb0Pressed)
            {
               pb0Pressed = false;
               nextState = gameStart;
            }
            break;

         case gameStart:
            /* Initiate the game, then wait 1 second before start */
            GAME_Init( level );
            GAME_Redraw();
            RENDER_UpdateDisplay( true, &displayDevice );
            timeout = GAME_START_TIMEOUT;
            nextState = gameStartWait;
            break;

         case gameStartWait:
            /* Wait for timeout before starting actual game */
            if (timeout == 0)
            {
               /* Prevent entering PAUSE state right after timeout */
               pb0Pressed = false;
               nextState = gameRunning;
            }
            break;

         case gameRunning:
            /* Pause the game if PB0 is pressed */
            if (pb0Pressed)
            {
               pb0Pressed = false;
               nextState = gamePaused;
               break;
            }

            /* Update tank position if capsense buttons pressed */
            if (CAPSENSE_getPressed(3) && !CAPSENSE_getPressed(4))
            {
               GAME_MoveTank(1);
            }
            else if (CAPSENSE_getPressed(4) && !CAPSENSE_getPressed(3))
            {
               GAME_MoveTank(-1);
            }

            /* Fire missile if PB1 is pressed */
            if (pb1Pressed)
            {
               pb1Pressed = false;
               GAME_FirePlayerMissile();
            }

            /* Update game */
            status = GAME_Update();
            GAME_Redraw();

            /* Check game status */
            if (status == GAME_OVER)
            {
               RENDER_ClearFramebufferArea( 35, 57, 93, 69, 0 );
               RENDER_Write( 37, 59, "GAME OVER" );
               timeout = GAME_OVER_TIMEOUT;
               nextState = gameOver;
            }
            else if (status == GAME_VICTORY)
            {
               RENDER_ClearFramebufferArea( 35, 57, 87, 69, 0 );
               RENDER_Write( 37, 59, "VICTORY!" );
               timeout = GAME_OVER_TIMEOUT;
               nextState = gameVictory;
            }
            else
            {
               nextState = gameRunning;
            }

            /* Update display */
            RENDER_UpdateDisplay( false, &displayDevice );
            break;


         case gamePaused:
            showWelcomeScreen();
            RENDER_Write( 46, 80, "PAUSED" );
            RENDER_UpdateDisplay( true, &displayDevice );
            nextState = gamePausedWait;
            break;

         case gamePausedWait:
            if (pb0Pressed)
            {
               pb0Pressed = false;
               GAME_Redraw();
               RENDER_UpdateDisplay( true, &displayDevice );
               nextState = gameRunning;
            }
            break;

         case gameOver:
            if (timeout == 0)
            {
               level = 1;
               pb0Pressed = false;
               pb1Pressed = false;
               nextState = qrCode;
            }
            break;

         case gameVictory:
            if (timeout == 0)
            {
               level++;
               pb0Pressed = false;
               pb1Pressed = false;
               nextState = gameStart;
            }
            break;

         }
      }
      else
      {
         EMU_EnterEM1();
      }
   }
}
