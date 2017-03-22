#ifndef __REGS_INTERNAL_H__
#define __REGS_INTERNAL_H__                                  1


/* FLASH_BASE block */
#define DATA_FLASH_BASE_BASE                                 (0x00000000u)
#define DATA_FLASH_BASE_END                                  (0x0007FFFFu)
#define DATA_FLASH_BASE_SIZE                                 (DATA_FLASH_BASE_END - DATA_FLASH_BASE_BASE + 1)

/* FLASH block */
#define DATA_FLASH_BASE                                      (0x08000000u)
#define DATA_FLASH_END                                       (0x0807FFFFu)
#define DATA_FLASH_SIZE                                      (DATA_FLASH_END - DATA_FLASH_BASE + 1)

/* BIG_INFO_BASE block */
#define DATA_BIG_INFO_BASE_BASE                              (0x00000000u)
#define DATA_BIG_INFO_BASE_END                               (0x000007FFu)
#define DATA_BIG_INFO_BASE_SIZE                              (DATA_BIG_INFO_BASE_END - DATA_BIG_INFO_BASE_BASE + 1)

/* BIG_INFO block */
#define DATA_BIG_INFO_BASE                                   (0x08080000u)
#define DATA_BIG_INFO_END                                    (0x080807FFu)
#define DATA_BIG_INFO_SIZE                                   (DATA_BIG_INFO_END - DATA_BIG_INFO_BASE + 1)

/* SMALL_INFO block */
#define DATA_SMALL_INFO_BASE                                 (0x08080800u)
#define DATA_SMALL_INFO_END                                  (0x08080FFFu)
#define DATA_SMALL_INFO_SIZE                                 (DATA_SMALL_INFO_END - DATA_SMALL_INFO_BASE + 1)

/* SRAM block */
#define DATA_SRAM_BASE                                       (0x20000000u)
#define DATA_SRAM_END                                        (0x2000FFFFu)
#define DATA_SRAM_SIZE                                       (DATA_SRAM_END - DATA_SRAM_BASE + 1)

/* CM_HV block */
#define BLOCK_CM_HV_BASE                                     (0x40000000u)
#define BLOCK_CM_HV_END                                      (0x40000048u)
#define BLOCK_CM_HV_SIZE                                     (BLOCK_CM_HV_END - BLOCK_CM_HV_BASE + 1)

#define HV_SPARE                                             *((volatile uint32_t *)0x40000000u)
#define HV_SPARE_REG                                         *((volatile uint32_t *)0x40000000u)
#define HV_SPARE_ADDR                                        (0x40000000u)
#define HV_SPARE_RESET                                       (0x00000000u)
        /* HV_SPARE field */
        #define HV_SPARE_HV_SPARE                            (0x000000FFu)
        #define HV_SPARE_HV_SPARE_MASK                       (0x000000FFu)
        #define HV_SPARE_HV_SPARE_BIT                        (0)
        #define HV_SPARE_HV_SPARE_BITS                       (8)

#define EVENT_CTRL                                           *((volatile uint32_t *)0x40000004u)
#define EVENT_CTRL_REG                                       *((volatile uint32_t *)0x40000004u)
#define EVENT_CTRL_ADDR                                      (0x40000004u)
#define EVENT_CTRL_RESET                                     (0x00000000u)
        /* LV_FREEZE field */
        #define LV_FREEZE                                    (0x00000002u)
        #define LV_FREEZE_MASK                               (0x00000002u)
        #define LV_FREEZE_BIT                                (1)
        #define LV_FREEZE_BITS                               (1)

#define SLEEPTMR_CLKEN                                       *((volatile uint32_t *)0x40000008u)
#define SLEEPTMR_CLKEN_REG                                   *((volatile uint32_t *)0x40000008u)
#define SLEEPTMR_CLKEN_ADDR                                  (0x40000008u)
#define SLEEPTMR_CLKEN_RESET                                 (0x00000002u)
        /* SLEEPTMR_CLK10KEN field */
        #define SLEEPTMR_CLK10KEN                            (0x00000002u)
        #define SLEEPTMR_CLK10KEN_MASK                       (0x00000002u)
        #define SLEEPTMR_CLK10KEN_BIT                        (1)
        #define SLEEPTMR_CLK10KEN_BITS                       (1)
        /* SLEEPTMR_CLK32KEN field */
        #define SLEEPTMR_CLK32KEN                            (0x00000001u)
        #define SLEEPTMR_CLK32KEN_MASK                       (0x00000001u)
        #define SLEEPTMR_CLK32KEN_BIT                        (0)
        #define SLEEPTMR_CLK32KEN_BITS                       (1)

#define CLKRC_TUNE                                           *((volatile uint32_t *)0x4000000Cu)
#define CLKRC_TUNE_REG                                       *((volatile uint32_t *)0x4000000Cu)
#define CLKRC_TUNE_ADDR                                      (0x4000000Cu)
#define CLKRC_TUNE_RESET                                     (0x00000000u)
        /* CLKRC_TUNE_FIELD field */
        #define CLKRC_TUNE_FIELD                             (0x0000000Fu)
        #define CLKRC_TUNE_FIELD_MASK                        (0x0000000Fu)
        #define CLKRC_TUNE_FIELD_BIT                         (0)
        #define CLKRC_TUNE_FIELD_BITS                        (4)

#define CLK1K_CAL                                            *((volatile uint32_t *)0x40000010u)
#define CLK1K_CAL_REG                                        *((volatile uint32_t *)0x40000010u)
#define CLK1K_CAL_ADDR                                       (0x40000010u)
#define CLK1K_CAL_RESET                                      (0x00005000u)
        /* CLK1K_INTEGER field */
        #define CLK1K_INTEGER                                (0x0000F800u)
        #define CLK1K_INTEGER_MASK                           (0x0000F800u)
        #define CLK1K_INTEGER_BIT                            (11)
        #define CLK1K_INTEGER_BITS                           (5)
        /* CLK1K_FRACTIONAL field */
        #define CLK1K_FRACTIONAL                             (0x000007FFu)
        #define CLK1K_FRACTIONAL_MASK                        (0x000007FFu)
        #define CLK1K_FRACTIONAL_BIT                         (0)
        #define CLK1K_FRACTIONAL_BITS                        (11)

#define REGEN_DSLEEP                                         *((volatile uint32_t *)0x40000014u)
#define REGEN_DSLEEP_REG                                     *((volatile uint32_t *)0x40000014u)
#define REGEN_DSLEEP_ADDR                                    (0x40000014u)
#define REGEN_DSLEEP_RESET                                   (0x00000001u)
        /* REGEN_DSLEEP_FIELD field */
        #define REGEN_DSLEEP_FIELD                           (0x00000001u)
        #define REGEN_DSLEEP_FIELD_MASK                      (0x00000001u)
        #define REGEN_DSLEEP_FIELD_BIT                       (0)
        #define REGEN_DSLEEP_FIELD_BITS                      (1)

#define VREG                                                 *((volatile uint32_t *)0x40000018u)
#define VREG_REG                                             *((volatile uint32_t *)0x40000018u)
#define VREG_ADDR                                            (0x40000018u)
#define VREG_RESET                                           (0x00000207u)
        /* VREF_EN field */
        #define VREG_VREF_EN                                 (0x00008000u)
        #define VREG_VREF_EN_MASK                            (0x00008000u)
        #define VREG_VREF_EN_BIT                             (15)
        #define VREG_VREF_EN_BITS                            (1)
        /* VREF_TEST field */
        #define VREG_VREF_TEST                               (0x00004000u)
        #define VREG_VREF_TEST_MASK                          (0x00004000u)
        #define VREG_VREF_TEST_BIT                           (14)
        #define VREG_VREF_TEST_BITS                          (1)
        /* VREG_1V8_EN field */
        #define VREG_VREG_1V8_EN                             (0x00000800u)
        #define VREG_VREG_1V8_EN_MASK                        (0x00000800u)
        #define VREG_VREG_1V8_EN_BIT                         (11)
        #define VREG_VREG_1V8_EN_BITS                        (1)
        /* VREG_1V8_TEST field */
        #define VREG_VREG_1V8_TEST                           (0x00000400u)
        #define VREG_VREG_1V8_TEST_MASK                      (0x00000400u)
        #define VREG_VREG_1V8_TEST_BIT                       (10)
        #define VREG_VREG_1V8_TEST_BITS                      (1)
        /* VREG_1V8_TRIM field */
        #define VREG_VREG_1V8_TRIM                           (0x00000380u)
        #define VREG_VREG_1V8_TRIM_MASK                      (0x00000380u)
        #define VREG_VREG_1V8_TRIM_BIT                       (7)
        #define VREG_VREG_1V8_TRIM_BITS                      (3)
        /* VREG_1V2_EN field */
        #define VREG_VREG_1V2_EN                             (0x00000010u)
        #define VREG_VREG_1V2_EN_MASK                        (0x00000010u)
        #define VREG_VREG_1V2_EN_BIT                         (4)
        #define VREG_VREG_1V2_EN_BITS                        (1)
        /* VREG_1V2_TEST field */
        #define VREG_VREG_1V2_TEST                           (0x00000008u)
        #define VREG_VREG_1V2_TEST_MASK                      (0x00000008u)
        #define VREG_VREG_1V2_TEST_BIT                       (3)
        #define VREG_VREG_1V2_TEST_BITS                      (1)
        /* VREG_1V2_TRIM field */
        #define VREG_VREG_1V2_TRIM                           (0x00000007u)
        #define VREG_VREG_1V2_TRIM_MASK                      (0x00000007u)
        #define VREG_VREG_1V2_TRIM_BIT                       (0)
        #define VREG_VREG_1V2_TRIM_BITS                      (3)

#define WAKE_SEL                                             *((volatile uint32_t *)0x40000020u)
#define WAKE_SEL_REG                                         *((volatile uint32_t *)0x40000020u)
#define WAKE_SEL_ADDR                                        (0x40000020u)
#define WAKE_SEL_RESET                                       (0x00000200u)
        /* WAKE_CSYSPWRUPREQ field */
        #define WAKE_CSYSPWRUPREQ                            (0x00000200u)
        #define WAKE_CSYSPWRUPREQ_MASK                       (0x00000200u)
        #define WAKE_CSYSPWRUPREQ_BIT                        (9)
        #define WAKE_CSYSPWRUPREQ_BITS                       (1)
        /* WAKE_CDBGPWRUPREQ field */
        #define WAKE_CDBGPWRUPREQ                            (0x00000100u)
        #define WAKE_CDBGPWRUPREQ_MASK                       (0x00000100u)
        #define WAKE_CDBGPWRUPREQ_BIT                        (8)
        #define WAKE_CDBGPWRUPREQ_BITS                       (1)
        /* WAKE_WAKE_CORE field */
        #define WAKE_WAKE_CORE                               (0x00000080u)
        #define WAKE_WAKE_CORE_MASK                          (0x00000080u)
        #define WAKE_WAKE_CORE_BIT                           (7)
        #define WAKE_WAKE_CORE_BITS                          (1)
        /* WAKE_SLEEPTMRWRAP field */
        #define WAKE_SLEEPTMRWRAP                            (0x00000040u)
        #define WAKE_SLEEPTMRWRAP_MASK                       (0x00000040u)
        #define WAKE_SLEEPTMRWRAP_BIT                        (6)
        #define WAKE_SLEEPTMRWRAP_BITS                       (1)
        /* WAKE_SLEEPTMRCMPB field */
        #define WAKE_SLEEPTMRCMPB                            (0x00000020u)
        #define WAKE_SLEEPTMRCMPB_MASK                       (0x00000020u)
        #define WAKE_SLEEPTMRCMPB_BIT                        (5)
        #define WAKE_SLEEPTMRCMPB_BITS                       (1)
        /* WAKE_SLEEPTMRCMPA field */
        #define WAKE_SLEEPTMRCMPA                            (0x00000010u)
        #define WAKE_SLEEPTMRCMPA_MASK                       (0x00000010u)
        #define WAKE_SLEEPTMRCMPA_BIT                        (4)
        #define WAKE_SLEEPTMRCMPA_BITS                       (1)
        /* WAKE_IRQD field */
        #define WAKE_IRQD                                    (0x00000008u)
        #define WAKE_IRQD_MASK                               (0x00000008u)
        #define WAKE_IRQD_BIT                                (3)
        #define WAKE_IRQD_BITS                               (1)
        /* WAKE_SC2 field */
        #define WAKE_SC2                                     (0x00000004u)
        #define WAKE_SC2_MASK                                (0x00000004u)
        #define WAKE_SC2_BIT                                 (2)
        #define WAKE_SC2_BITS                                (1)
        /* WAKE_SC1 field */
        #define WAKE_SC1                                     (0x00000002u)
        #define WAKE_SC1_MASK                                (0x00000002u)
        #define WAKE_SC1_BIT                                 (1)
        #define WAKE_SC1_BITS                                (1)
        /* GPIO_WAKE field */
        #define GPIO_WAKE                                    (0x00000001u)
        #define GPIO_WAKE_MASK                               (0x00000001u)
        #define GPIO_WAKE_BIT                                (0)
        #define GPIO_WAKE_BITS                               (1)

#define WAKE_CORE                                            *((volatile uint32_t *)0x40000024u)
#define WAKE_CORE_REG                                        *((volatile uint32_t *)0x40000024u)
#define WAKE_CORE_ADDR                                       (0x40000024u)
#define WAKE_CORE_RESET                                      (0x00000000u)
        /* WAKE_CORE_FIELD field */
        #define WAKE_CORE_FIELD                              (0x00000020u)
        #define WAKE_CORE_FIELD_MASK                         (0x00000020u)
        #define WAKE_CORE_FIELD_BIT                          (5)
        #define WAKE_CORE_FIELD_BITS                         (1)

#define PWRUP_EVENT                                          *((volatile uint32_t *)0x40000028u)
#define PWRUP_EVENT_REG                                      *((volatile uint32_t *)0x40000028u)
#define PWRUP_EVENT_ADDR                                     (0x40000028u)
#define PWRUP_EVENT_RESET                                    (0x00000000u)
        /* PWRUP_CSYSPWRUPREQ field */
        #define PWRUP_CSYSPWRUPREQ                           (0x00000200u)
        #define PWRUP_CSYSPWRUPREQ_MASK                      (0x00000200u)
        #define PWRUP_CSYSPWRUPREQ_BIT                       (9)
        #define PWRUP_CSYSPWRUPREQ_BITS                      (1)
        /* PWRUP_CDBGPWRUPREQ field */
        #define PWRUP_CDBGPWRUPREQ                           (0x00000100u)
        #define PWRUP_CDBGPWRUPREQ_MASK                      (0x00000100u)
        #define PWRUP_CDBGPWRUPREQ_BIT                       (8)
        #define PWRUP_CDBGPWRUPREQ_BITS                      (1)
        /* PWRUP_WAKECORE field */
        #define PWRUP_WAKECORE                               (0x00000080u)
        #define PWRUP_WAKECORE_MASK                          (0x00000080u)
        #define PWRUP_WAKECORE_BIT                           (7)
        #define PWRUP_WAKECORE_BITS                          (1)
        /* PWRUP_SLEEPTMRWRAP field */
        #define PWRUP_SLEEPTMRWRAP                           (0x00000040u)
        #define PWRUP_SLEEPTMRWRAP_MASK                      (0x00000040u)
        #define PWRUP_SLEEPTMRWRAP_BIT                       (6)
        #define PWRUP_SLEEPTMRWRAP_BITS                      (1)
        /* PWRUP_SLEEPTMRCOMPB field */
        #define PWRUP_SLEEPTMRCOMPB                          (0x00000020u)
        #define PWRUP_SLEEPTMRCOMPB_MASK                     (0x00000020u)
        #define PWRUP_SLEEPTMRCOMPB_BIT                      (5)
        #define PWRUP_SLEEPTMRCOMPB_BITS                     (1)
        /* PWRUP_SLEEPTMRCOMPA field */
        #define PWRUP_SLEEPTMRCOMPA                          (0x00000010u)
        #define PWRUP_SLEEPTMRCOMPA_MASK                     (0x00000010u)
        #define PWRUP_SLEEPTMRCOMPA_BIT                      (4)
        #define PWRUP_SLEEPTMRCOMPA_BITS                     (1)
        /* PWRUP_IRQD field */
        #define PWRUP_IRQD                                   (0x00000008u)
        #define PWRUP_IRQD_MASK                              (0x00000008u)
        #define PWRUP_IRQD_BIT                               (3)
        #define PWRUP_IRQD_BITS                              (1)
        /* PWRUP_SC2 field */
        #define PWRUP_SC2                                    (0x00000004u)
        #define PWRUP_SC2_MASK                               (0x00000004u)
        #define PWRUP_SC2_BIT                                (2)
        #define PWRUP_SC2_BITS                               (1)
        /* PWRUP_SC1 field */
        #define PWRUP_SC1                                    (0x00000002u)
        #define PWRUP_SC1_MASK                               (0x00000002u)
        #define PWRUP_SC1_BIT                                (1)
        #define PWRUP_SC1_BITS                               (1)
        /* PWRUP_GPIO field */
        #define PWRUP_GPIO                                   (0x00000001u)
        #define PWRUP_GPIO_MASK                              (0x00000001u)
        #define PWRUP_GPIO_BIT                               (0)
        #define PWRUP_GPIO_BITS                              (1)

#define RESET_EVENT                                          *((volatile uint32_t *)0x4000002Cu)
#define RESET_EVENT_REG                                      *((volatile uint32_t *)0x4000002Cu)
#define RESET_EVENT_ADDR                                     (0x4000002Cu)
#define RESET_EVENT_RESET                                    (0x00000001u)
        /* RESET_CPULOCKUP field */
        #define RESET_CPULOCKUP                              (0x00000080u)
        #define RESET_CPULOCKUP_MASK                         (0x00000080u)
        #define RESET_CPULOCKUP_BIT                          (7)
        #define RESET_CPULOCKUP_BITS                         (1)
        /* RESET_OPTBYTEFAIL field */
        #define RESET_OPTBYTEFAIL                            (0x00000040u)
        #define RESET_OPTBYTEFAIL_MASK                       (0x00000040u)
        #define RESET_OPTBYTEFAIL_BIT                        (6)
        #define RESET_OPTBYTEFAIL_BITS                       (1)
        /* RESET_DSLEEP field */
        #define RESET_DSLEEP                                 (0x00000020u)
        #define RESET_DSLEEP_MASK                            (0x00000020u)
        #define RESET_DSLEEP_BIT                             (5)
        #define RESET_DSLEEP_BITS                            (1)
        /* RESET_SW field */
        #define RESET_SW                                     (0x00000010u)
        #define RESET_SW_MASK                                (0x00000010u)
        #define RESET_SW_BIT                                 (4)
        #define RESET_SW_BITS                                (1)
        /* RESET_WDOG field */
        #define RESET_WDOG                                   (0x00000008u)
        #define RESET_WDOG_MASK                              (0x00000008u)
        #define RESET_WDOG_BIT                               (3)
        #define RESET_WDOG_BITS                              (1)
        /* RESET_NRESET field */
        #define RESET_NRESET                                 (0x00000004u)
        #define RESET_NRESET_MASK                            (0x00000004u)
        #define RESET_NRESET_BIT                             (2)
        #define RESET_NRESET_BITS                            (1)
        /* RESET_PWRLV field */
        #define RESET_PWRLV                                  (0x00000002u)
        #define RESET_PWRLV_MASK                             (0x00000002u)
        #define RESET_PWRLV_BIT                              (1)
        #define RESET_PWRLV_BITS                             (1)
        /* RESET_PWRHV field */
        #define RESET_PWRHV                                  (0x00000001u)
        #define RESET_PWRHV_MASK                             (0x00000001u)
        #define RESET_PWRHV_BIT                              (0)
        #define RESET_PWRHV_BITS                             (1)

#define DBG_MBOX                                             *((volatile uint32_t *)0x40000030u)
#define DBG_MBOX_REG                                         *((volatile uint32_t *)0x40000030u)
#define DBG_MBOX_ADDR                                        (0x40000030u)
#define DBG_MBOX_RESET                                       (0x00000000u)
        /* DBG_MBOX field */
        #define DBG_MBOX_DBG_MBOX                            (0x0000FFFFu)
        #define DBG_MBOX_DBG_MBOX_MASK                       (0x0000FFFFu)
        #define DBG_MBOX_DBG_MBOX_BIT                        (0)
        #define DBG_MBOX_DBG_MBOX_BITS                       (16)

#define CPWRUPREQ_STATUS                                     *((volatile uint32_t *)0x40000034u)
#define CPWRUPREQ_STATUS_REG                                 *((volatile uint32_t *)0x40000034u)
#define CPWRUPREQ_STATUS_ADDR                                (0x40000034u)
#define CPWRUPREQ_STATUS_RESET                               (0x00000000u)
        /* CPWRUPREQ field */
        #define CPWRUPREQ_STATUS_CPWRUPREQ                   (0x00000001u)
        #define CPWRUPREQ_STATUS_CPWRUPREQ_MASK              (0x00000001u)
        #define CPWRUPREQ_STATUS_CPWRUPREQ_BIT               (0)
        #define CPWRUPREQ_STATUS_CPWRUPREQ_BITS              (1)

#define CSYSPWRUPREQ_STATUS                                  *((volatile uint32_t *)0x40000038u)
#define CSYSPWRUPREQ_STATUS_REG                              *((volatile uint32_t *)0x40000038u)
#define CSYSPWRUPREQ_STATUS_ADDR                             (0x40000038u)
#define CSYSPWRUPREQ_STATUS_RESET                            (0x00000000u)
        /* CSYSPWRUPREQ field */
        #define CSYSPWRUPREQ_STATUS_CSYSPWRUPREQ             (0x00000001u)
        #define CSYSPWRUPREQ_STATUS_CSYSPWRUPREQ_MASK        (0x00000001u)
        #define CSYSPWRUPREQ_STATUS_CSYSPWRUPREQ_BIT         (0)
        #define CSYSPWRUPREQ_STATUS_CSYSPWRUPREQ_BITS        (1)

#define CSYSPWRUPACK_STATUS                                  *((volatile uint32_t *)0x4000003Cu)
#define CSYSPWRUPACK_STATUS_REG                              *((volatile uint32_t *)0x4000003Cu)
#define CSYSPWRUPACK_STATUS_ADDR                             (0x4000003Cu)
#define CSYSPWRUPACK_STATUS_RESET                            (0x00000000u)
        /* CSYSPWRUPACK field */
        #define CSYSPWRUPACK_STATUS_CSYSPWRUPACK             (0x00000001u)
        #define CSYSPWRUPACK_STATUS_CSYSPWRUPACK_MASK        (0x00000001u)
        #define CSYSPWRUPACK_STATUS_CSYSPWRUPACK_BIT         (0)
        #define CSYSPWRUPACK_STATUS_CSYSPWRUPACK_BITS        (1)

#define CSYSPWRUPACK_INHIBIT                                 *((volatile uint32_t *)0x40000040u)
#define CSYSPWRUPACK_INHIBIT_REG                             *((volatile uint32_t *)0x40000040u)
#define CSYSPWRUPACK_INHIBIT_ADDR                            (0x40000040u)
#define CSYSPWRUPACK_INHIBIT_RESET                           (0x00000000u)
        /* CSYSPWRUPACK_INHIBIT field */
        #define CSYSPWRUPACK_INHIBIT_CSYSPWRUPACK_INHIBIT    (0x00000001u)
        #define CSYSPWRUPACK_INHIBIT_CSYSPWRUPACK_INHIBIT_MASK (0x00000001u)
        #define CSYSPWRUPACK_INHIBIT_CSYSPWRUPACK_INHIBIT_BIT (0)
        #define CSYSPWRUPACK_INHIBIT_CSYSPWRUPACK_INHIBIT_BITS (1)

#define OPT_ERR_MAINTAIN_WAKE                                *((volatile uint32_t *)0x40000044u)
#define OPT_ERR_MAINTAIN_WAKE_REG                            *((volatile uint32_t *)0x40000044u)
#define OPT_ERR_MAINTAIN_WAKE_ADDR                           (0x40000044u)
#define OPT_ERR_MAINTAIN_WAKE_RESET                          (0x00000000u)
        /* OPT_ERR_MAINTAIN_WAKE field */
        #define OPT_ERR_MAINTAIN_WAKE_OPT_ERR_MAINTAIN_WAKE  (0x00000001u)
        #define OPT_ERR_MAINTAIN_WAKE_OPT_ERR_MAINTAIN_WAKE_MASK (0x00000001u)
        #define OPT_ERR_MAINTAIN_WAKE_OPT_ERR_MAINTAIN_WAKE_BIT (0)
        #define OPT_ERR_MAINTAIN_WAKE_OPT_ERR_MAINTAIN_WAKE_BITS (1)

#define RAM_TEST                                             *((volatile uint32_t *)0x40000048u)
#define RAM_TEST_REG                                         *((volatile uint32_t *)0x40000048u)
#define RAM_TEST_ADDR                                        (0x40000048u)
#define RAM_TEST_RESET                                       (0x0000000Eu)
        /* RAM_REGB field */
        #define RAM_TEST_RAM_REGB                            (0x00000020u)
        #define RAM_TEST_RAM_REGB_MASK                       (0x00000020u)
        #define RAM_TEST_RAM_REGB_BIT                        (5)
        #define RAM_TEST_RAM_REGB_BITS                       (1)
        /* RAM_TIMING field */
        #define RAM_TEST_RAM_TIMING                          (0x0000001Cu)
        #define RAM_TEST_RAM_TIMING_MASK                     (0x0000001Cu)
        #define RAM_TEST_RAM_TIMING_BIT                      (2)
        #define RAM_TEST_RAM_TIMING_BITS                     (3)
        /* RAM_LEAKAGE field */
        #define RAM_TEST_RAM_LEAKAGE                         (0x00000003u)
        #define RAM_TEST_RAM_LEAKAGE_MASK                    (0x00000003u)
        #define RAM_TEST_RAM_LEAKAGE_BIT                     (0)
        #define RAM_TEST_RAM_LEAKAGE_BITS                    (2)

/* BASEBAND block */
#define BLOCK_BASEBAND_BASE                                  (0x40001000u)
#define BLOCK_BASEBAND_END                                   (0x40001114u)
#define BLOCK_BASEBAND_SIZE                                  (BLOCK_BASEBAND_END - BLOCK_BASEBAND_BASE + 1)

#define MOD_CAL_CTRL                                         *((volatile uint32_t *)0x40001000u)
#define MOD_CAL_CTRL_REG                                     *((volatile uint32_t *)0x40001000u)
#define MOD_CAL_CTRL_ADDR                                    (0x40001000u)
#define MOD_CAL_CTRL_RESET                                   (0x00000000u)
        /* MOD_CAL_GO field */
        #define MOD_CAL_CTRL_MOD_CAL_GO                      (0x00008000u)
        #define MOD_CAL_CTRL_MOD_CAL_GO_MASK                 (0x00008000u)
        #define MOD_CAL_CTRL_MOD_CAL_GO_BIT                  (15)
        #define MOD_CAL_CTRL_MOD_CAL_GO_BITS                 (1)
        /* MOD_CAL_DONE field */
        #define MOD_CAL_CTRL_MOD_CAL_DONE                    (0x00000010u)
        #define MOD_CAL_CTRL_MOD_CAL_DONE_MASK               (0x00000010u)
        #define MOD_CAL_CTRL_MOD_CAL_DONE_BIT                (4)
        #define MOD_CAL_CTRL_MOD_CAL_DONE_BITS               (1)
        /* MOD_CAL_CYCLES field */
        #define MOD_CAL_CTRL_MOD_CAL_CYCLES                  (0x00000003u)
        #define MOD_CAL_CTRL_MOD_CAL_CYCLES_MASK             (0x00000003u)
        #define MOD_CAL_CTRL_MOD_CAL_CYCLES_BIT              (0)
        #define MOD_CAL_CTRL_MOD_CAL_CYCLES_BITS             (2)

#define MOD_CAL_COUNT_H                                      *((volatile uint32_t *)0x40001004u)
#define MOD_CAL_COUNT_H_REG                                  *((volatile uint32_t *)0x40001004u)
#define MOD_CAL_COUNT_H_ADDR                                 (0x40001004u)
#define MOD_CAL_COUNT_H_RESET                                (0x00000000u)
        /* MOD_CAL_COUNT_H field */
        #define MOD_CAL_COUNT_H_MOD_CAL_COUNT_H              (0x000000FFu)
        #define MOD_CAL_COUNT_H_MOD_CAL_COUNT_H_MASK         (0x000000FFu)
        #define MOD_CAL_COUNT_H_MOD_CAL_COUNT_H_BIT          (0)
        #define MOD_CAL_COUNT_H_MOD_CAL_COUNT_H_BITS         (8)

#define MOD_CAL_COUNT_L                                      *((volatile uint32_t *)0x40001008u)
#define MOD_CAL_COUNT_L_REG                                  *((volatile uint32_t *)0x40001008u)
#define MOD_CAL_COUNT_L_ADDR                                 (0x40001008u)
#define MOD_CAL_COUNT_L_RESET                                (0x00000000u)
        /* MOD_CAL_COUNT_L field */
        #define MOD_CAL_COUNT_L_MOD_CAL_COUNT_L              (0x0000FFFFu)
        #define MOD_CAL_COUNT_L_MOD_CAL_COUNT_L_MASK         (0x0000FFFFu)
        #define MOD_CAL_COUNT_L_MOD_CAL_COUNT_L_BIT          (0)
        #define MOD_CAL_COUNT_L_MOD_CAL_COUNT_L_BITS         (16)

#define RSSI_ROLLING                                         *((volatile uint32_t *)0x4000100Cu)
#define RSSI_ROLLING_REG                                     *((volatile uint32_t *)0x4000100Cu)
#define RSSI_ROLLING_ADDR                                    (0x4000100Cu)
#define RSSI_ROLLING_RESET                                   (0x00000000u)
        /* RSSI_ROLLING field */
        #define RSSI_ROLLING_RSSI_ROLLING                    (0x00003FFFu)
        #define RSSI_ROLLING_RSSI_ROLLING_MASK               (0x00003FFFu)
        #define RSSI_ROLLING_RSSI_ROLLING_BIT                (0)
        #define RSSI_ROLLING_RSSI_ROLLING_BITS               (14)

#define RSSI_PKT                                             *((volatile uint32_t *)0x40001010u)
#define RSSI_PKT_REG                                         *((volatile uint32_t *)0x40001010u)
#define RSSI_PKT_ADDR                                        (0x40001010u)
#define RSSI_PKT_RESET                                       (0x00000000u)
        /* RSSI_PKT field */
        #define RSSI_PKT_RSSI_PKT                            (0x000000FFu)
        #define RSSI_PKT_RSSI_PKT_MASK                       (0x000000FFu)
        #define RSSI_PKT_RSSI_PKT_BIT                        (0)
        #define RSSI_PKT_RSSI_PKT_BITS                       (8)

#define RX_ADC                                               *((volatile uint32_t *)0x40001014u)
#define RX_ADC_REG                                           *((volatile uint32_t *)0x40001014u)
#define RX_ADC_ADDR                                          (0x40001014u)
#define RX_ADC_RESET                                         (0x00000024u)
        /* RX_ADC field */
        #define RX_ADC_RX_ADC                                (0x0000007Fu)
        #define RX_ADC_RX_ADC_MASK                           (0x0000007Fu)
        #define RX_ADC_RX_ADC_BIT                            (0)
        #define RX_ADC_RX_ADC_BITS                           (7)

#define DEBUG_BB_MODE                                        *((volatile uint32_t *)0x40001018u)
#define DEBUG_BB_MODE_REG                                    *((volatile uint32_t *)0x40001018u)
#define DEBUG_BB_MODE_ADDR                                   (0x40001018u)
#define DEBUG_BB_MODE_RESET                                  (0x00000000u)
        /* DEBUG_BB_MODE_EN field */
        #define DEBUG_BB_MODE_DEBUG_BB_MODE_EN               (0x00008000u)
        #define DEBUG_BB_MODE_DEBUG_BB_MODE_EN_MASK          (0x00008000u)
        #define DEBUG_BB_MODE_DEBUG_BB_MODE_EN_BIT           (15)
        #define DEBUG_BB_MODE_DEBUG_BB_MODE_EN_BITS          (1)
        /* DEBUG_BB_MODE field */
        #define DEBUG_BB_MODE_DEBUG_BB_MODE                  (0x00000003u)
        #define DEBUG_BB_MODE_DEBUG_BB_MODE_MASK             (0x00000003u)
        #define DEBUG_BB_MODE_DEBUG_BB_MODE_BIT              (0)
        #define DEBUG_BB_MODE_DEBUG_BB_MODE_BITS             (2)

#define BB_DEBUG                                             *((volatile uint32_t *)0x4000101Cu)
#define BB_DEBUG_REG                                         *((volatile uint32_t *)0x4000101Cu)
#define BB_DEBUG_ADDR                                        (0x4000101Cu)
#define BB_DEBUG_RESET                                       (0x00000002u)
        /* SYNC_REG_EN field */
        #define BB_DEBUG_SYNC_REG_EN                         (0x00008000u)
        #define BB_DEBUG_SYNC_REG_EN_MASK                    (0x00008000u)
        #define BB_DEBUG_SYNC_REG_EN_BIT                     (15)
        #define BB_DEBUG_SYNC_REG_EN_BITS                    (1)
        /* DEBUG_MUX_ADDR field */
        #define BB_DEBUG_DEBUG_MUX_ADDR                      (0x000000F0u)
        #define BB_DEBUG_DEBUG_MUX_ADDR_MASK                 (0x000000F0u)
        #define BB_DEBUG_DEBUG_MUX_ADDR_BIT                  (4)
        #define BB_DEBUG_DEBUG_MUX_ADDR_BITS                 (4)
        /* BB_DEBUG_SEL field */
        #define BB_DEBUG_BB_DEBUG_SEL                        (0x00000003u)
        #define BB_DEBUG_BB_DEBUG_SEL_MASK                   (0x00000003u)
        #define BB_DEBUG_BB_DEBUG_SEL_BIT                    (0)
        #define BB_DEBUG_BB_DEBUG_SEL_BITS                   (2)

#define BB_DEBUG_VIEW                                        *((volatile uint32_t *)0x40001020u)
#define BB_DEBUG_VIEW_REG                                    *((volatile uint32_t *)0x40001020u)
#define BB_DEBUG_VIEW_ADDR                                   (0x40001020u)
#define BB_DEBUG_VIEW_RESET                                  (0x00000000u)
        /* BB_DEBUG_VIEW field */
        #define BB_DEBUG_VIEW_BB_DEBUG_VIEW                  (0x0000FFFFu)
        #define BB_DEBUG_VIEW_BB_DEBUG_VIEW_MASK             (0x0000FFFFu)
        #define BB_DEBUG_VIEW_BB_DEBUG_VIEW_BIT              (0)
        #define BB_DEBUG_VIEW_BB_DEBUG_VIEW_BITS             (16)

#define IF_FREQ                                              *((volatile uint32_t *)0x40001024u)
#define IF_FREQ_REG                                          *((volatile uint32_t *)0x40001024u)
#define IF_FREQ_ADDR                                         (0x40001024u)
#define IF_FREQ_RESET                                        (0x00000155u)
        /* TIMING_CORR_EN field */
        #define IF_FREQ_TIMING_CORR_EN                       (0x00008000u)
        #define IF_FREQ_TIMING_CORR_EN_MASK                  (0x00008000u)
        #define IF_FREQ_TIMING_CORR_EN_BIT                   (15)
        #define IF_FREQ_TIMING_CORR_EN_BITS                  (1)
        /* IF_FREQ field */
        #define IF_FREQ_IF_FREQ                              (0x000001FFu)
        #define IF_FREQ_IF_FREQ_MASK                         (0x000001FFu)
        #define IF_FREQ_IF_FREQ_BIT                          (0)
        #define IF_FREQ_IF_FREQ_BITS                         (9)

#define MOD_EN                                               *((volatile uint32_t *)0x40001028u)
#define MOD_EN_REG                                           *((volatile uint32_t *)0x40001028u)
#define MOD_EN_ADDR                                          (0x40001028u)
#define MOD_EN_RESET                                         (0x00000001u)
        /* MOD_EN field */
        #define MOD_EN_MOD_EN                                (0x00000001u)
        #define MOD_EN_MOD_EN_MASK                           (0x00000001u)
        #define MOD_EN_MOD_EN_BIT                            (0)
        #define MOD_EN_MOD_EN_BITS                           (1)

#define PRESCALE_CTRL                                        *((volatile uint32_t *)0x4000102Cu)
#define PRESCALE_CTRL_REG                                    *((volatile uint32_t *)0x4000102Cu)
#define PRESCALE_CTRL_ADDR                                   (0x4000102Cu)
#define PRESCALE_CTRL_RESET                                  (0x00000000u)
        /* PRESCALE_SET field */
        #define PRESCALE_CTRL_PRESCALE_SET                   (0x00008000u)
        #define PRESCALE_CTRL_PRESCALE_SET_MASK              (0x00008000u)
        #define PRESCALE_CTRL_PRESCALE_SET_BIT               (15)
        #define PRESCALE_CTRL_PRESCALE_SET_BITS              (1)
        /* PRESCALE_VAL field */
        #define PRESCALE_CTRL_PRESCALE_VAL                   (0x00000007u)
        #define PRESCALE_CTRL_PRESCALE_VAL_MASK              (0x00000007u)
        #define PRESCALE_CTRL_PRESCALE_VAL_BIT               (0)
        #define PRESCALE_CTRL_PRESCALE_VAL_BITS              (3)

#define ADC_BYPASS_EN                                        *((volatile uint32_t *)0x40001030u)
#define ADC_BYPASS_EN_REG                                    *((volatile uint32_t *)0x40001030u)
#define ADC_BYPASS_EN_ADDR                                   (0x40001030u)
#define ADC_BYPASS_EN_RESET                                  (0x00000000u)
        /* ADC_BYPASS_EN field */
        #define ADC_BYPASS_EN_ADC_BYPASS_EN                  (0x00000001u)
        #define ADC_BYPASS_EN_ADC_BYPASS_EN_MASK             (0x00000001u)
        #define ADC_BYPASS_EN_ADC_BYPASS_EN_BIT              (0)
        #define ADC_BYPASS_EN_ADC_BYPASS_EN_BITS             (1)

#define FIXED_CODE_EN                                        *((volatile uint32_t *)0x40001034u)
#define FIXED_CODE_EN_REG                                    *((volatile uint32_t *)0x40001034u)
#define FIXED_CODE_EN_ADDR                                   (0x40001034u)
#define FIXED_CODE_EN_RESET                                  (0x00000000u)
        /* FIXED_CODE_EN field */
        #define FIXED_CODE_EN_FIXED_CODE_EN                  (0x00000001u)
        #define FIXED_CODE_EN_FIXED_CODE_EN_MASK             (0x00000001u)
        #define FIXED_CODE_EN_FIXED_CODE_EN_BIT              (0)
        #define FIXED_CODE_EN_FIXED_CODE_EN_BITS             (1)

#define FIXED_CODE_H                                         *((volatile uint32_t *)0x40001038u)
#define FIXED_CODE_H_REG                                     *((volatile uint32_t *)0x40001038u)
#define FIXED_CODE_H_ADDR                                    (0x40001038u)
#define FIXED_CODE_H_RESET                                   (0x00000000u)
        /* FIXED_CODE_H field */
        #define FIXED_CODE_H_FIXED_CODE_H                    (0x0000FFFFu)
        #define FIXED_CODE_H_FIXED_CODE_H_MASK               (0x0000FFFFu)
        #define FIXED_CODE_H_FIXED_CODE_H_BIT                (0)
        #define FIXED_CODE_H_FIXED_CODE_H_BITS               (16)

#define FIXED_CODE_L                                         *((volatile uint32_t *)0x4000103Cu)
#define FIXED_CODE_L_REG                                     *((volatile uint32_t *)0x4000103Cu)
#define FIXED_CODE_L_ADDR                                    (0x4000103Cu)
#define FIXED_CODE_L_RESET                                   (0x00000000u)
        /* FIXED_CODE_L field */
        #define FIXED_CODE_L_FIXED_CODE_L                    (0x0000FFFFu)
        #define FIXED_CODE_L_FIXED_CODE_L_MASK               (0x0000FFFFu)
        #define FIXED_CODE_L_FIXED_CODE_L_BIT                (0)
        #define FIXED_CODE_L_FIXED_CODE_L_BITS               (16)

#define FIXED_CODE_L_SHADOW                                  *((volatile uint32_t *)0x40001040u)
#define FIXED_CODE_L_SHADOW_REG                              *((volatile uint32_t *)0x40001040u)
#define FIXED_CODE_L_SHADOW_ADDR                             (0x40001040u)
#define FIXED_CODE_L_SHADOW_RESET                            (0x00000000u)
        /* FIXED_CODE_L_SHADOW field */
        #define FIXED_CODE_L_SHADOW_FIXED_CODE_L_SHADOW      (0x0000FFFFu)
        #define FIXED_CODE_L_SHADOW_FIXED_CODE_L_SHADOW_MASK (0x0000FFFFu)
        #define FIXED_CODE_L_SHADOW_FIXED_CODE_L_SHADOW_BIT  (0)
        #define FIXED_CODE_L_SHADOW_FIXED_CODE_L_SHADOW_BITS (16)

#define RX_GAIN_CTRL                                         *((volatile uint32_t *)0x40001044u)
#define RX_GAIN_CTRL_REG                                     *((volatile uint32_t *)0x40001044u)
#define RX_GAIN_CTRL_ADDR                                    (0x40001044u)
#define RX_GAIN_CTRL_RESET                                   (0x00000000u)
        /* RX_GAIN_MUX field */
        #define RX_GAIN_CTRL_RX_GAIN_MUX                     (0x00008000u)
        #define RX_GAIN_CTRL_RX_GAIN_MUX_MASK                (0x00008000u)
        #define RX_GAIN_CTRL_RX_GAIN_MUX_BIT                 (15)
        #define RX_GAIN_CTRL_RX_GAIN_MUX_BITS                (1)
        /* RX_RF_GAIN_TEST field */
        #define RX_GAIN_CTRL_RX_RF_GAIN_TEST                 (0x00000080u)
        #define RX_GAIN_CTRL_RX_RF_GAIN_TEST_MASK            (0x00000080u)
        #define RX_GAIN_CTRL_RX_RF_GAIN_TEST_BIT             (7)
        #define RX_GAIN_CTRL_RX_RF_GAIN_TEST_BITS            (1)
        /* RX_MIXER_GAIN_TEST field */
        #define RX_GAIN_CTRL_RX_MIXER_GAIN_TEST              (0x00000040u)
        #define RX_GAIN_CTRL_RX_MIXER_GAIN_TEST_MASK         (0x00000040u)
        #define RX_GAIN_CTRL_RX_MIXER_GAIN_TEST_BIT          (6)
        #define RX_GAIN_CTRL_RX_MIXER_GAIN_TEST_BITS         (1)
        /* RX_FILTER_GAIN_TEST field */
        #define RX_GAIN_CTRL_RX_FILTER_GAIN_TEST             (0x00000030u)
        #define RX_GAIN_CTRL_RX_FILTER_GAIN_TEST_MASK        (0x00000030u)
        #define RX_GAIN_CTRL_RX_FILTER_GAIN_TEST_BIT         (4)
        #define RX_GAIN_CTRL_RX_FILTER_GAIN_TEST_BITS        (2)
        /* RX_IF_GAIN_TEST field */
        #define RX_GAIN_CTRL_RX_IF_GAIN_TEST                 (0x0000000Fu)
        #define RX_GAIN_CTRL_RX_IF_GAIN_TEST_MASK            (0x0000000Fu)
        #define RX_GAIN_CTRL_RX_IF_GAIN_TEST_BIT             (0)
        #define RX_GAIN_CTRL_RX_IF_GAIN_TEST_BITS            (4)

#define PD_DITHER_EN                                         *((volatile uint32_t *)0x40001048u)
#define PD_DITHER_EN_REG                                     *((volatile uint32_t *)0x40001048u)
#define PD_DITHER_EN_ADDR                                    (0x40001048u)
#define PD_DITHER_EN_RESET                                   (0x00000001u)
        /* PD_DITHER_EN field */
        #define PD_DITHER_EN_PD_DITHER_EN                    (0x00000001u)
        #define PD_DITHER_EN_PD_DITHER_EN_MASK               (0x00000001u)
        #define PD_DITHER_EN_PD_DITHER_EN_BIT                (0)
        #define PD_DITHER_EN_PD_DITHER_EN_BITS               (1)

#define RX_ERR_THRESH                                        *((volatile uint32_t *)0x4000104Cu)
#define RX_ERR_THRESH_REG                                    *((volatile uint32_t *)0x4000104Cu)
#define RX_ERR_THRESH_ADDR                                   (0x4000104Cu)
#define RX_ERR_THRESH_RESET                                  (0x00004608u)
        /* LPF_RX_ERR_COEFF field */
        #define RX_ERR_THRESH_LPF_RX_ERR_COEFF               (0x0000E000u)
        #define RX_ERR_THRESH_LPF_RX_ERR_COEFF_MASK          (0x0000E000u)
        #define RX_ERR_THRESH_LPF_RX_ERR_COEFF_BIT           (13)
        #define RX_ERR_THRESH_LPF_RX_ERR_COEFF_BITS          (3)
        /* LPF_RX_ERR_THRESH field */
        #define RX_ERR_THRESH_LPF_RX_ERR_THRESH              (0x00001F00u)
        #define RX_ERR_THRESH_LPF_RX_ERR_THRESH_MASK         (0x00001F00u)
        #define RX_ERR_THRESH_LPF_RX_ERR_THRESH_BIT          (8)
        #define RX_ERR_THRESH_LPF_RX_ERR_THRESH_BITS         (5)
        /* RX_ERR_THRESH field */
        #define RX_ERR_THRESH_RX_ERR_THRESH                  (0x0000001Fu)
        #define RX_ERR_THRESH_RX_ERR_THRESH_MASK             (0x0000001Fu)
        #define RX_ERR_THRESH_RX_ERR_THRESH_BIT              (0)
        #define RX_ERR_THRESH_RX_ERR_THRESH_BITS             (5)

#define CARRIER_THRESH                                       *((volatile uint32_t *)0x40001050u)
#define CARRIER_THRESH_REG                                   *((volatile uint32_t *)0x40001050u)
#define CARRIER_THRESH_ADDR                                  (0x40001050u)
#define CARRIER_THRESH_RESET                                 (0x00002332u)
        /* CARRIER_SPIKE_THRESH field */
        #define CARRIER_THRESH_CARRIER_SPIKE_THRESH          (0x0000FF00u)
        #define CARRIER_THRESH_CARRIER_SPIKE_THRESH_MASK     (0x0000FF00u)
        #define CARRIER_THRESH_CARRIER_SPIKE_THRESH_BIT      (8)
        #define CARRIER_THRESH_CARRIER_SPIKE_THRESH_BITS     (8)
        /* CARRIER_THRESH field */
        #define CARRIER_THRESH_CARRIER_THRESH                (0x000000FFu)
        #define CARRIER_THRESH_CARRIER_THRESH_MASK           (0x000000FFu)
        #define CARRIER_THRESH_CARRIER_THRESH_BIT            (0)
        #define CARRIER_THRESH_CARRIER_THRESH_BITS           (8)

#define RSSI_THRESH                                          *((volatile uint32_t *)0x40001054u)
#define RSSI_THRESH_REG                                      *((volatile uint32_t *)0x40001054u)
#define RSSI_THRESH_ADDR                                     (0x40001054u)
#define RSSI_THRESH_RESET                                    (0x00000100u)
        /* RSSI_THRESH field */
        #define RSSI_THRESH_RSSI_THRESH                      (0x0000FFFFu)
        #define RSSI_THRESH_RSSI_THRESH_MASK                 (0x0000FFFFu)
        #define RSSI_THRESH_RSSI_THRESH_BIT                  (0)
        #define RSSI_THRESH_RSSI_THRESH_BITS                 (16)

#define SYNTH_START                                          *((volatile uint32_t *)0x40001058u)
#define SYNTH_START_REG                                      *((volatile uint32_t *)0x40001058u)
#define SYNTH_START_ADDR                                     (0x40001058u)
#define SYNTH_START_RESET                                    (0x00006464u)
        /* SYNTH_WARM_START field */
        #define SYNTH_START_SYNTH_WARM_START                 (0x0000FF00u)
        #define SYNTH_START_SYNTH_WARM_START_MASK            (0x0000FF00u)
        #define SYNTH_START_SYNTH_WARM_START_BIT             (8)
        #define SYNTH_START_SYNTH_WARM_START_BITS            (8)
        /* SYNTH_COLD_START field */
        #define SYNTH_START_SYNTH_COLD_START                 (0x000000FFu)
        #define SYNTH_START_SYNTH_COLD_START_MASK            (0x000000FFu)
        #define SYNTH_START_SYNTH_COLD_START_BIT             (0)
        #define SYNTH_START_SYNTH_COLD_START_BITS            (8)

#define IN_LOCK_EN                                           *((volatile uint32_t *)0x4000105Cu)
#define IN_LOCK_EN_REG                                       *((volatile uint32_t *)0x4000105Cu)
#define IN_LOCK_EN_ADDR                                      (0x4000105Cu)
#define IN_LOCK_EN_RESET                                     (0x00000001u)
        /* IN_LOCK_EN field */
        #define IN_LOCK_EN_IN_LOCK_EN                        (0x00000001u)
        #define IN_LOCK_EN_IN_LOCK_EN_MASK                   (0x00000001u)
        #define IN_LOCK_EN_IN_LOCK_EN_BIT                    (0)
        #define IN_LOCK_EN_IN_LOCK_EN_BITS                   (1)

#define DITHER_AMPLITUDE                                     *((volatile uint32_t *)0x40001060u)
#define DITHER_AMPLITUDE_REG                                 *((volatile uint32_t *)0x40001060u)
#define DITHER_AMPLITUDE_ADDR                                (0x40001060u)
#define DITHER_AMPLITUDE_RESET                               (0x0000003Fu)
        /* DITHER_AMP field */
        #define DITHER_AMPLITUDE_DITHER_AMP                  (0x0000003Fu)
        #define DITHER_AMPLITUDE_DITHER_AMP_MASK             (0x0000003Fu)
        #define DITHER_AMPLITUDE_DITHER_AMP_BIT              (0)
        #define DITHER_AMPLITUDE_DITHER_AMP_BITS             (6)

#define TX_STEP_TIME                                         *((volatile uint32_t *)0x40001064u)
#define TX_STEP_TIME_REG                                     *((volatile uint32_t *)0x40001064u)
#define TX_STEP_TIME_ADDR                                    (0x40001064u)
#define TX_STEP_TIME_RESET                                   (0x00000000u)
        /* TX_STEP_TIME field */
        #define TX_STEP_TIME_TX_STEP_TIME                    (0x000000FFu)
        #define TX_STEP_TIME_TX_STEP_TIME_MASK               (0x000000FFu)
        #define TX_STEP_TIME_TX_STEP_TIME_BIT                (0)
        #define TX_STEP_TIME_TX_STEP_TIME_BITS               (8)

#define GAIN_THRESH_MAX                                      *((volatile uint32_t *)0x40001068u)
#define GAIN_THRESH_MAX_REG                                  *((volatile uint32_t *)0x40001068u)
#define GAIN_THRESH_MAX_ADDR                                 (0x40001068u)
#define GAIN_THRESH_MAX_RESET                                (0x00000060u)
        /* GAIN_THRESH_MAX field */
        #define GAIN_THRESH_MAX_GAIN_THRESH_MAX              (0x000000FFu)
        #define GAIN_THRESH_MAX_GAIN_THRESH_MAX_MASK         (0x000000FFu)
        #define GAIN_THRESH_MAX_GAIN_THRESH_MAX_BIT          (0)
        #define GAIN_THRESH_MAX_GAIN_THRESH_MAX_BITS         (8)

#define GAIN_THRESH_MID                                      *((volatile uint32_t *)0x4000106Cu)
#define GAIN_THRESH_MID_REG                                  *((volatile uint32_t *)0x4000106Cu)
#define GAIN_THRESH_MID_ADDR                                 (0x4000106Cu)
#define GAIN_THRESH_MID_RESET                                (0x00000030u)
        /* GAIN_THRESH_MID field */
        #define GAIN_THRESH_MID_GAIN_THRESH_MID              (0x000000FFu)
        #define GAIN_THRESH_MID_GAIN_THRESH_MID_MASK         (0x000000FFu)
        #define GAIN_THRESH_MID_GAIN_THRESH_MID_BIT          (0)
        #define GAIN_THRESH_MID_GAIN_THRESH_MID_BITS         (8)

#define GAIN_THRESH_MIN                                      *((volatile uint32_t *)0x40001070u)
#define GAIN_THRESH_MIN_REG                                  *((volatile uint32_t *)0x40001070u)
#define GAIN_THRESH_MIN_ADDR                                 (0x40001070u)
#define GAIN_THRESH_MIN_RESET                                (0x00000018u)
        /* GAIN_THRESH_MIN field */
        #define GAIN_THRESH_MIN_GAIN_THRESH_MIN              (0x000000FFu)
        #define GAIN_THRESH_MIN_GAIN_THRESH_MIN_MASK         (0x000000FFu)
        #define GAIN_THRESH_MIN_GAIN_THRESH_MIN_BIT          (0)
        #define GAIN_THRESH_MIN_GAIN_THRESH_MIN_BITS         (8)

#define GAIN_SETTING_0                                       *((volatile uint32_t *)0x40001074u)
#define GAIN_SETTING_0_REG                                   *((volatile uint32_t *)0x40001074u)
#define GAIN_SETTING_0_ADDR                                  (0x40001074u)
#define GAIN_SETTING_0_RESET                                 (0x00000000u)
        /* RX_MIXER_GAIN_0 field */
        #define GAIN_SETTING_0_RX_MIXER_GAIN_0               (0x00000040u)
        #define GAIN_SETTING_0_RX_MIXER_GAIN_0_MASK          (0x00000040u)
        #define GAIN_SETTING_0_RX_MIXER_GAIN_0_BIT           (6)
        #define GAIN_SETTING_0_RX_MIXER_GAIN_0_BITS          (1)
        /* RX_FILTER_GAIN_0 field */
        #define GAIN_SETTING_0_RX_FILTER_GAIN_0              (0x00000030u)
        #define GAIN_SETTING_0_RX_FILTER_GAIN_0_MASK         (0x00000030u)
        #define GAIN_SETTING_0_RX_FILTER_GAIN_0_BIT          (4)
        #define GAIN_SETTING_0_RX_FILTER_GAIN_0_BITS         (2)
        /* RX_IF_GAIN_0 field */
        #define GAIN_SETTING_0_RX_IF_GAIN_0                  (0x0000000Fu)
        #define GAIN_SETTING_0_RX_IF_GAIN_0_MASK             (0x0000000Fu)
        #define GAIN_SETTING_0_RX_IF_GAIN_0_BIT              (0)
        #define GAIN_SETTING_0_RX_IF_GAIN_0_BITS             (4)

#define GAIN_SETTING_1                                       *((volatile uint32_t *)0x40001078u)
#define GAIN_SETTING_1_REG                                   *((volatile uint32_t *)0x40001078u)
#define GAIN_SETTING_1_ADDR                                  (0x40001078u)
#define GAIN_SETTING_1_RESET                                 (0x00000010u)
        /* RX_MIXER_GAIN_1 field */
        #define GAIN_SETTING_1_RX_MIXER_GAIN_1               (0x00000040u)
        #define GAIN_SETTING_1_RX_MIXER_GAIN_1_MASK          (0x00000040u)
        #define GAIN_SETTING_1_RX_MIXER_GAIN_1_BIT           (6)
        #define GAIN_SETTING_1_RX_MIXER_GAIN_1_BITS          (1)
        /* RX_FILTER_GAIN_1 field */
        #define GAIN_SETTING_1_RX_FILTER_GAIN_1              (0x00000030u)
        #define GAIN_SETTING_1_RX_FILTER_GAIN_1_MASK         (0x00000030u)
        #define GAIN_SETTING_1_RX_FILTER_GAIN_1_BIT          (4)
        #define GAIN_SETTING_1_RX_FILTER_GAIN_1_BITS         (2)
        /* RX_IF_GAIN_1 field */
        #define GAIN_SETTING_1_RX_IF_GAIN_1                  (0x0000000Fu)
        #define GAIN_SETTING_1_RX_IF_GAIN_1_MASK             (0x0000000Fu)
        #define GAIN_SETTING_1_RX_IF_GAIN_1_BIT              (0)
        #define GAIN_SETTING_1_RX_IF_GAIN_1_BITS             (4)

#define GAIN_SETTING_2                                       *((volatile uint32_t *)0x4000107Cu)
#define GAIN_SETTING_2_REG                                   *((volatile uint32_t *)0x4000107Cu)
#define GAIN_SETTING_2_ADDR                                  (0x4000107Cu)
#define GAIN_SETTING_2_RESET                                 (0x00000030u)
        /* RX_MIXER_GAIN_2 field */
        #define GAIN_SETTING_2_RX_MIXER_GAIN_2               (0x00000040u)
        #define GAIN_SETTING_2_RX_MIXER_GAIN_2_MASK          (0x00000040u)
        #define GAIN_SETTING_2_RX_MIXER_GAIN_2_BIT           (6)
        #define GAIN_SETTING_2_RX_MIXER_GAIN_2_BITS          (1)
        /* RX_FILTER_GAIN_2 field */
        #define GAIN_SETTING_2_RX_FILTER_GAIN_2              (0x00000030u)
        #define GAIN_SETTING_2_RX_FILTER_GAIN_2_MASK         (0x00000030u)
        #define GAIN_SETTING_2_RX_FILTER_GAIN_2_BIT          (4)
        #define GAIN_SETTING_2_RX_FILTER_GAIN_2_BITS         (2)
        /* RX_IF_GAIN_2 field */
        #define GAIN_SETTING_2_RX_IF_GAIN_2                  (0x0000000Fu)
        #define GAIN_SETTING_2_RX_IF_GAIN_2_MASK             (0x0000000Fu)
        #define GAIN_SETTING_2_RX_IF_GAIN_2_BIT              (0)
        #define GAIN_SETTING_2_RX_IF_GAIN_2_BITS             (4)

#define GAIN_SETTING_3                                       *((volatile uint32_t *)0x40001080u)
#define GAIN_SETTING_3_REG                                   *((volatile uint32_t *)0x40001080u)
#define GAIN_SETTING_3_ADDR                                  (0x40001080u)
#define GAIN_SETTING_3_RESET                                 (0x00000031u)
        /* RX_MIXER_GAIN_3 field */
        #define GAIN_SETTING_3_RX_MIXER_GAIN_3               (0x00000040u)
        #define GAIN_SETTING_3_RX_MIXER_GAIN_3_MASK          (0x00000040u)
        #define GAIN_SETTING_3_RX_MIXER_GAIN_3_BIT           (6)
        #define GAIN_SETTING_3_RX_MIXER_GAIN_3_BITS          (1)
        /* RX_FILTER_GAIN_3 field */
        #define GAIN_SETTING_3_RX_FILTER_GAIN_3              (0x00000030u)
        #define GAIN_SETTING_3_RX_FILTER_GAIN_3_MASK         (0x00000030u)
        #define GAIN_SETTING_3_RX_FILTER_GAIN_3_BIT          (4)
        #define GAIN_SETTING_3_RX_FILTER_GAIN_3_BITS         (2)
        /* RX_IF_GAIN_3 field */
        #define GAIN_SETTING_3_RX_IF_GAIN_3                  (0x0000000Fu)
        #define GAIN_SETTING_3_RX_IF_GAIN_3_MASK             (0x0000000Fu)
        #define GAIN_SETTING_3_RX_IF_GAIN_3_BIT              (0)
        #define GAIN_SETTING_3_RX_IF_GAIN_3_BITS             (4)

#define GAIN_SETTING_4                                       *((volatile uint32_t *)0x40001084u)
#define GAIN_SETTING_4_REG                                   *((volatile uint32_t *)0x40001084u)
#define GAIN_SETTING_4_ADDR                                  (0x40001084u)
#define GAIN_SETTING_4_RESET                                 (0x00000032u)
        /* RX_MIXER_GAIN_4 field */
        #define GAIN_SETTING_4_RX_MIXER_GAIN_4               (0x00000040u)
        #define GAIN_SETTING_4_RX_MIXER_GAIN_4_MASK          (0x00000040u)
        #define GAIN_SETTING_4_RX_MIXER_GAIN_4_BIT           (6)
        #define GAIN_SETTING_4_RX_MIXER_GAIN_4_BITS          (1)
        /* RX_FILTER_GAIN_4 field */
        #define GAIN_SETTING_4_RX_FILTER_GAIN_4              (0x00000030u)
        #define GAIN_SETTING_4_RX_FILTER_GAIN_4_MASK         (0x00000030u)
        #define GAIN_SETTING_4_RX_FILTER_GAIN_4_BIT          (4)
        #define GAIN_SETTING_4_RX_FILTER_GAIN_4_BITS         (2)
        /* RX_IF_GAIN_4 field */
        #define GAIN_SETTING_4_RX_IF_GAIN_4                  (0x0000000Fu)
        #define GAIN_SETTING_4_RX_IF_GAIN_4_MASK             (0x0000000Fu)
        #define GAIN_SETTING_4_RX_IF_GAIN_4_BIT              (0)
        #define GAIN_SETTING_4_RX_IF_GAIN_4_BITS             (4)

#define GAIN_SETTING_5                                       *((volatile uint32_t *)0x40001088u)
#define GAIN_SETTING_5_REG                                   *((volatile uint32_t *)0x40001088u)
#define GAIN_SETTING_5_ADDR                                  (0x40001088u)
#define GAIN_SETTING_5_RESET                                 (0x00000033u)
        /* RX_MIXER_GAIN_5 field */
        #define GAIN_SETTING_5_RX_MIXER_GAIN_5               (0x00000040u)
        #define GAIN_SETTING_5_RX_MIXER_GAIN_5_MASK          (0x00000040u)
        #define GAIN_SETTING_5_RX_MIXER_GAIN_5_BIT           (6)
        #define GAIN_SETTING_5_RX_MIXER_GAIN_5_BITS          (1)
        /* RX_FILTER_GAIN_5 field */
        #define GAIN_SETTING_5_RX_FILTER_GAIN_5              (0x00000030u)
        #define GAIN_SETTING_5_RX_FILTER_GAIN_5_MASK         (0x00000030u)
        #define GAIN_SETTING_5_RX_FILTER_GAIN_5_BIT          (4)
        #define GAIN_SETTING_5_RX_FILTER_GAIN_5_BITS         (2)
        /* RX_IF_GAIN_5 field */
        #define GAIN_SETTING_5_RX_IF_GAIN_5                  (0x0000000Fu)
        #define GAIN_SETTING_5_RX_IF_GAIN_5_MASK             (0x0000000Fu)
        #define GAIN_SETTING_5_RX_IF_GAIN_5_BIT              (0)
        #define GAIN_SETTING_5_RX_IF_GAIN_5_BITS             (4)

#define GAIN_SETTING_6                                       *((volatile uint32_t *)0x4000108Cu)
#define GAIN_SETTING_6_REG                                   *((volatile uint32_t *)0x4000108Cu)
#define GAIN_SETTING_6_ADDR                                  (0x4000108Cu)
#define GAIN_SETTING_6_RESET                                 (0x00000034u)
        /* RX_MIXER_GAIN_6 field */
        #define GAIN_SETTING_6_RX_MIXER_GAIN_6               (0x00000040u)
        #define GAIN_SETTING_6_RX_MIXER_GAIN_6_MASK          (0x00000040u)
        #define GAIN_SETTING_6_RX_MIXER_GAIN_6_BIT           (6)
        #define GAIN_SETTING_6_RX_MIXER_GAIN_6_BITS          (1)
        /* RX_FILTER_GAIN_6 field */
        #define GAIN_SETTING_6_RX_FILTER_GAIN_6              (0x00000030u)
        #define GAIN_SETTING_6_RX_FILTER_GAIN_6_MASK         (0x00000030u)
        #define GAIN_SETTING_6_RX_FILTER_GAIN_6_BIT          (4)
        #define GAIN_SETTING_6_RX_FILTER_GAIN_6_BITS         (2)
        /* RX_IF_GAIN_6 field */
        #define GAIN_SETTING_6_RX_IF_GAIN_6                  (0x0000000Fu)
        #define GAIN_SETTING_6_RX_IF_GAIN_6_MASK             (0x0000000Fu)
        #define GAIN_SETTING_6_RX_IF_GAIN_6_BIT              (0)
        #define GAIN_SETTING_6_RX_IF_GAIN_6_BITS             (4)

#define GAIN_SETTING_7                                       *((volatile uint32_t *)0x40001090u)
#define GAIN_SETTING_7_REG                                   *((volatile uint32_t *)0x40001090u)
#define GAIN_SETTING_7_ADDR                                  (0x40001090u)
#define GAIN_SETTING_7_RESET                                 (0x00000035u)
        /* RX_MIXER_GAIN_7 field */
        #define GAIN_SETTING_7_RX_MIXER_GAIN_7               (0x00000040u)
        #define GAIN_SETTING_7_RX_MIXER_GAIN_7_MASK          (0x00000040u)
        #define GAIN_SETTING_7_RX_MIXER_GAIN_7_BIT           (6)
        #define GAIN_SETTING_7_RX_MIXER_GAIN_7_BITS          (1)
        /* RX_FILTER_GAIN_7 field */
        #define GAIN_SETTING_7_RX_FILTER_GAIN_7              (0x00000030u)
        #define GAIN_SETTING_7_RX_FILTER_GAIN_7_MASK         (0x00000030u)
        #define GAIN_SETTING_7_RX_FILTER_GAIN_7_BIT          (4)
        #define GAIN_SETTING_7_RX_FILTER_GAIN_7_BITS         (2)
        /* RX_IF_GAIN_7 field */
        #define GAIN_SETTING_7_RX_IF_GAIN_7                  (0x0000000Fu)
        #define GAIN_SETTING_7_RX_IF_GAIN_7_MASK             (0x0000000Fu)
        #define GAIN_SETTING_7_RX_IF_GAIN_7_BIT              (0)
        #define GAIN_SETTING_7_RX_IF_GAIN_7_BITS             (4)

#define GAIN_SETTING_8                                       *((volatile uint32_t *)0x40001094u)
#define GAIN_SETTING_8_REG                                   *((volatile uint32_t *)0x40001094u)
#define GAIN_SETTING_8_ADDR                                  (0x40001094u)
#define GAIN_SETTING_8_RESET                                 (0x00000036u)
        /* RX_MIXER_GAIN_8 field */
        #define GAIN_SETTING_8_RX_MIXER_GAIN_8               (0x00000040u)
        #define GAIN_SETTING_8_RX_MIXER_GAIN_8_MASK          (0x00000040u)
        #define GAIN_SETTING_8_RX_MIXER_GAIN_8_BIT           (6)
        #define GAIN_SETTING_8_RX_MIXER_GAIN_8_BITS          (1)
        /* RX_FILTER_GAIN_8 field */
        #define GAIN_SETTING_8_RX_FILTER_GAIN_8              (0x00000030u)
        #define GAIN_SETTING_8_RX_FILTER_GAIN_8_MASK         (0x00000030u)
        #define GAIN_SETTING_8_RX_FILTER_GAIN_8_BIT          (4)
        #define GAIN_SETTING_8_RX_FILTER_GAIN_8_BITS         (2)
        /* RX_IF_GAIN_8 field */
        #define GAIN_SETTING_8_RX_IF_GAIN_8                  (0x0000000Fu)
        #define GAIN_SETTING_8_RX_IF_GAIN_8_MASK             (0x0000000Fu)
        #define GAIN_SETTING_8_RX_IF_GAIN_8_BIT              (0)
        #define GAIN_SETTING_8_RX_IF_GAIN_8_BITS             (4)

#define GAIN_SETTING_9                                       *((volatile uint32_t *)0x40001098u)
#define GAIN_SETTING_9_REG                                   *((volatile uint32_t *)0x40001098u)
#define GAIN_SETTING_9_ADDR                                  (0x40001098u)
#define GAIN_SETTING_9_RESET                                 (0x00000076u)
        /* RX_MIXER_GAIN_9 field */
        #define GAIN_SETTING_9_RX_MIXER_GAIN_9               (0x00000040u)
        #define GAIN_SETTING_9_RX_MIXER_GAIN_9_MASK          (0x00000040u)
        #define GAIN_SETTING_9_RX_MIXER_GAIN_9_BIT           (6)
        #define GAIN_SETTING_9_RX_MIXER_GAIN_9_BITS          (1)
        /* RX_FILTER_GAIN_9 field */
        #define GAIN_SETTING_9_RX_FILTER_GAIN_9              (0x00000030u)
        #define GAIN_SETTING_9_RX_FILTER_GAIN_9_MASK         (0x00000030u)
        #define GAIN_SETTING_9_RX_FILTER_GAIN_9_BIT          (4)
        #define GAIN_SETTING_9_RX_FILTER_GAIN_9_BITS         (2)
        /* RX_IF_GAIN_9 field */
        #define GAIN_SETTING_9_RX_IF_GAIN_9                  (0x0000000Fu)
        #define GAIN_SETTING_9_RX_IF_GAIN_9_MASK             (0x0000000Fu)
        #define GAIN_SETTING_9_RX_IF_GAIN_9_BIT              (0)
        #define GAIN_SETTING_9_RX_IF_GAIN_9_BITS             (4)

#define GAIN_SETTING_10                                      *((volatile uint32_t *)0x4000109Cu)
#define GAIN_SETTING_10_REG                                  *((volatile uint32_t *)0x4000109Cu)
#define GAIN_SETTING_10_ADDR                                 (0x4000109Cu)
#define GAIN_SETTING_10_RESET                                (0x00000077u)
        /* RX_MIXER_GAIN_10 field */
        #define GAIN_SETTING_10_RX_MIXER_GAIN_10             (0x00000040u)
        #define GAIN_SETTING_10_RX_MIXER_GAIN_10_MASK        (0x00000040u)
        #define GAIN_SETTING_10_RX_MIXER_GAIN_10_BIT         (6)
        #define GAIN_SETTING_10_RX_MIXER_GAIN_10_BITS        (1)
        /* RX_FILTER_GAIN_10 field */
        #define GAIN_SETTING_10_RX_FILTER_GAIN_10            (0x00000030u)
        #define GAIN_SETTING_10_RX_FILTER_GAIN_10_MASK       (0x00000030u)
        #define GAIN_SETTING_10_RX_FILTER_GAIN_10_BIT        (4)
        #define GAIN_SETTING_10_RX_FILTER_GAIN_10_BITS       (2)
        /* RX_IF_GAIN_10 field */
        #define GAIN_SETTING_10_RX_IF_GAIN_10                (0x0000000Fu)
        #define GAIN_SETTING_10_RX_IF_GAIN_10_MASK           (0x0000000Fu)
        #define GAIN_SETTING_10_RX_IF_GAIN_10_BIT            (0)
        #define GAIN_SETTING_10_RX_IF_GAIN_10_BITS           (4)

#define GAIN_SETTING_11                                      *((volatile uint32_t *)0x400010A0u)
#define GAIN_SETTING_11_REG                                  *((volatile uint32_t *)0x400010A0u)
#define GAIN_SETTING_11_ADDR                                 (0x400010A0u)
#define GAIN_SETTING_11_RESET                                (0x00000078u)
        /* RX_MIXER_GAIN_11 field */
        #define GAIN_SETTING_11_RX_MIXER_GAIN_11             (0x00000040u)
        #define GAIN_SETTING_11_RX_MIXER_GAIN_11_MASK        (0x00000040u)
        #define GAIN_SETTING_11_RX_MIXER_GAIN_11_BIT         (6)
        #define GAIN_SETTING_11_RX_MIXER_GAIN_11_BITS        (1)
        /* RX_FILTER_GAIN_11 field */
        #define GAIN_SETTING_11_RX_FILTER_GAIN_11            (0x00000030u)
        #define GAIN_SETTING_11_RX_FILTER_GAIN_11_MASK       (0x00000030u)
        #define GAIN_SETTING_11_RX_FILTER_GAIN_11_BIT        (4)
        #define GAIN_SETTING_11_RX_FILTER_GAIN_11_BITS       (2)
        /* RX_IF_GAIN_11 field */
        #define GAIN_SETTING_11_RX_IF_GAIN_11                (0x0000000Fu)
        #define GAIN_SETTING_11_RX_IF_GAIN_11_MASK           (0x0000000Fu)
        #define GAIN_SETTING_11_RX_IF_GAIN_11_BIT            (0)
        #define GAIN_SETTING_11_RX_IF_GAIN_11_BITS           (4)

#define GAIN_CTRL_MIN_RF                                     *((volatile uint32_t *)0x400010A4u)
#define GAIN_CTRL_MIN_RF_REG                                 *((volatile uint32_t *)0x400010A4u)
#define GAIN_CTRL_MIN_RF_ADDR                                (0x400010A4u)
#define GAIN_CTRL_MIN_RF_RESET                               (0x000000F0u)
        /* GAIN_CTRL_MIN_RF field */
        #define GAIN_CTRL_MIN_RF_GAIN_CTRL_MIN_RF            (0x000001FFu)
        #define GAIN_CTRL_MIN_RF_GAIN_CTRL_MIN_RF_MASK       (0x000001FFu)
        #define GAIN_CTRL_MIN_RF_GAIN_CTRL_MIN_RF_BIT        (0)
        #define GAIN_CTRL_MIN_RF_GAIN_CTRL_MIN_RF_BITS       (9)

#define GAIN_CTRL_MAX_RF                                     *((volatile uint32_t *)0x400010A8u)
#define GAIN_CTRL_MAX_RF_REG                                 *((volatile uint32_t *)0x400010A8u)
#define GAIN_CTRL_MAX_RF_ADDR                                (0x400010A8u)
#define GAIN_CTRL_MAX_RF_RESET                               (0x000000FCu)
        /* GAIN_CTRL_MAX_RF field */
        #define GAIN_CTRL_MAX_RF_GAIN_CTRL_MAX_RF            (0x000001FFu)
        #define GAIN_CTRL_MAX_RF_GAIN_CTRL_MAX_RF_MASK       (0x000001FFu)
        #define GAIN_CTRL_MAX_RF_GAIN_CTRL_MAX_RF_BIT        (0)
        #define GAIN_CTRL_MAX_RF_GAIN_CTRL_MAX_RF_BITS       (9)

#define MIXER_GAIN_STEP                                      *((volatile uint32_t *)0x400010ACu)
#define MIXER_GAIN_STEP_REG                                  *((volatile uint32_t *)0x400010ACu)
#define MIXER_GAIN_STEP_ADDR                                 (0x400010ACu)
#define MIXER_GAIN_STEP_RESET                                (0x0000000Cu)
        /* MIXER_GAIN_STEP field */
        #define MIXER_GAIN_STEP_MIXER_GAIN_STEP              (0x0000000Fu)
        #define MIXER_GAIN_STEP_MIXER_GAIN_STEP_MASK         (0x0000000Fu)
        #define MIXER_GAIN_STEP_MIXER_GAIN_STEP_BIT          (0)
        #define MIXER_GAIN_STEP_MIXER_GAIN_STEP_BITS         (4)

#define PREAMBLE_EVENT                                       *((volatile uint32_t *)0x400010B0u)
#define PREAMBLE_EVENT_REG                                   *((volatile uint32_t *)0x400010B0u)
#define PREAMBLE_EVENT_ADDR                                  (0x400010B0u)
#define PREAMBLE_EVENT_RESET                                 (0x00005877u)
        /* PREAMBLE_CONFIRM_THRESH field */
        #define PREAMBLE_EVENT_PREAMBLE_CONFIRM_THRESH       (0x0000FF00u)
        #define PREAMBLE_EVENT_PREAMBLE_CONFIRM_THRESH_MASK  (0x0000FF00u)
        #define PREAMBLE_EVENT_PREAMBLE_CONFIRM_THRESH_BIT   (8)
        #define PREAMBLE_EVENT_PREAMBLE_CONFIRM_THRESH_BITS  (8)
        /* PREAMBLE_EVENT_THRESH field */
        #define PREAMBLE_EVENT_PREAMBLE_EVENT_THRESH         (0x000000FFu)
        #define PREAMBLE_EVENT_PREAMBLE_EVENT_THRESH_MASK    (0x000000FFu)
        #define PREAMBLE_EVENT_PREAMBLE_EVENT_THRESH_BIT     (0)
        #define PREAMBLE_EVENT_PREAMBLE_EVENT_THRESH_BITS    (8)

#define PREAMBLE_ABORT_THRESH                                *((volatile uint32_t *)0x400010B4u)
#define PREAMBLE_ABORT_THRESH_REG                            *((volatile uint32_t *)0x400010B4u)
#define PREAMBLE_ABORT_THRESH_ADDR                           (0x400010B4u)
#define PREAMBLE_ABORT_THRESH_RESET                          (0x00000071u)
        /* PREAMBLE_ABORT_THRESH field */
        #define PREAMBLE_ABORT_THRESH_PREAMBLE_ABORT_THRESH  (0x000000FFu)
        #define PREAMBLE_ABORT_THRESH_PREAMBLE_ABORT_THRESH_MASK (0x000000FFu)
        #define PREAMBLE_ABORT_THRESH_PREAMBLE_ABORT_THRESH_BIT (0)
        #define PREAMBLE_ABORT_THRESH_PREAMBLE_ABORT_THRESH_BITS (8)

#define PREAMBLE_ACCEPT_WINDOW                               *((volatile uint32_t *)0x400010B8u)
#define PREAMBLE_ACCEPT_WINDOW_REG                           *((volatile uint32_t *)0x400010B8u)
#define PREAMBLE_ACCEPT_WINDOW_ADDR                          (0x400010B8u)
#define PREAMBLE_ACCEPT_WINDOW_RESET                         (0x00000003u)
        /* PREAMBLE_ACCEPT_WINDOW field */
        #define PREAMBLE_ACCEPT_WINDOW_PREAMBLE_ACCEPT_WINDOW (0x0000007Fu)
        #define PREAMBLE_ACCEPT_WINDOW_PREAMBLE_ACCEPT_WINDOW_MASK (0x0000007Fu)
        #define PREAMBLE_ACCEPT_WINDOW_PREAMBLE_ACCEPT_WINDOW_BIT (0)
        #define PREAMBLE_ACCEPT_WINDOW_PREAMBLE_ACCEPT_WINDOW_BITS (7)

#define CCA_MODE                                             *((volatile uint32_t *)0x400010BCu)
#define CCA_MODE_REG                                         *((volatile uint32_t *)0x400010BCu)
#define CCA_MODE_ADDR                                        (0x400010BCu)
#define CCA_MODE_RESET                                       (0x00000000u)
        /* CCA_MODE field */
        #define CCA_MODE_CCA_MODE                            (0x00000003u)
        #define CCA_MODE_CCA_MODE_MASK                       (0x00000003u)
        #define CCA_MODE_CCA_MODE_BIT                        (0)
        #define CCA_MODE_CCA_MODE_BITS                       (2)

#define TX_POWER_MAX                                         *((volatile uint32_t *)0x400010C0u)
#define TX_POWER_MAX_REG                                     *((volatile uint32_t *)0x400010C0u)
#define TX_POWER_MAX_ADDR                                    (0x400010C0u)
#define TX_POWER_MAX_RESET                                   (0x00000000u)
        /* MANUAL_POWER field */
        #define TX_POWER_MAX_MANUAL_POWER                    (0x00008000u)
        #define TX_POWER_MAX_MANUAL_POWER_MASK               (0x00008000u)
        #define TX_POWER_MAX_MANUAL_POWER_BIT                (15)
        #define TX_POWER_MAX_MANUAL_POWER_BITS               (1)
        /* TX_POWER_MAX field */
        #define TX_POWER_MAX_TX_POWER_MAX                    (0x0000001Fu)
        #define TX_POWER_MAX_TX_POWER_MAX_MASK               (0x0000001Fu)
        #define TX_POWER_MAX_TX_POWER_MAX_BIT                (0)
        #define TX_POWER_MAX_TX_POWER_MAX_BITS               (5)

#define SYNTH_FREQ_H                                         *((volatile uint32_t *)0x400010C4u)
#define SYNTH_FREQ_H_REG                                     *((volatile uint32_t *)0x400010C4u)
#define SYNTH_FREQ_H_ADDR                                    (0x400010C4u)
#define SYNTH_FREQ_H_RESET                                   (0x00000003u)
        /* SYNTH_FREQ_H field */
        #define SYNTH_FREQ_H_SYNTH_FREQ_H                    (0x00000003u)
        #define SYNTH_FREQ_H_SYNTH_FREQ_H_MASK               (0x00000003u)
        #define SYNTH_FREQ_H_SYNTH_FREQ_H_BIT                (0)
        #define SYNTH_FREQ_H_SYNTH_FREQ_H_BITS               (2)

#define SYNTH_FREQ_L                                         *((volatile uint32_t *)0x400010C8u)
#define SYNTH_FREQ_L_REG                                     *((volatile uint32_t *)0x400010C8u)
#define SYNTH_FREQ_L_ADDR                                    (0x400010C8u)
#define SYNTH_FREQ_L_RESET                                   (0x00003800u)
        /* SYNTH_FREQ_L field */
        #define SYNTH_FREQ_L_SYNTH_FREQ_L                    (0x0000FFFFu)
        #define SYNTH_FREQ_L_SYNTH_FREQ_L_MASK               (0x0000FFFFu)
        #define SYNTH_FREQ_L_SYNTH_FREQ_L_BIT                (0)
        #define SYNTH_FREQ_L_SYNTH_FREQ_L_BITS               (16)

#define RSSI_INST                                            *((volatile uint32_t *)0x400010CCu)
#define RSSI_INST_REG                                        *((volatile uint32_t *)0x400010CCu)
#define RSSI_INST_ADDR                                       (0x400010CCu)
#define RSSI_INST_RESET                                      (0x00000000u)
        /* NEW_RSSI_INST field */
        #define RSSI_INST_NEW_RSSI_INST                      (0x00000200u)
        #define RSSI_INST_NEW_RSSI_INST_MASK                 (0x00000200u)
        #define RSSI_INST_NEW_RSSI_INST_BIT                  (9)
        #define RSSI_INST_NEW_RSSI_INST_BITS                 (1)
        /* RSSI_INST field */
        #define RSSI_INST_RSSI_INST                          (0x000001FFu)
        #define RSSI_INST_RSSI_INST_MASK                     (0x000001FFu)
        #define RSSI_INST_RSSI_INST_BIT                      (0)
        #define RSSI_INST_RSSI_INST_BITS                     (9)

#define FREQ_MEAS_CTRL1                                      *((volatile uint32_t *)0x400010D0u)
#define FREQ_MEAS_CTRL1_REG                                  *((volatile uint32_t *)0x400010D0u)
#define FREQ_MEAS_CTRL1_ADDR                                 (0x400010D0u)
#define FREQ_MEAS_CTRL1_RESET                                (0x00000160u)
        /* AUTO_TUNE_EN field */
        #define FREQ_MEAS_CTRL1_AUTO_TUNE_EN                 (0x00008000u)
        #define FREQ_MEAS_CTRL1_AUTO_TUNE_EN_MASK            (0x00008000u)
        #define FREQ_MEAS_CTRL1_AUTO_TUNE_EN_BIT             (15)
        #define FREQ_MEAS_CTRL1_AUTO_TUNE_EN_BITS            (1)
        /* FREQ_MEAS_EN field */
        #define FREQ_MEAS_CTRL1_FREQ_MEAS_EN                 (0x00004000u)
        #define FREQ_MEAS_CTRL1_FREQ_MEAS_EN_MASK            (0x00004000u)
        #define FREQ_MEAS_CTRL1_FREQ_MEAS_EN_BIT             (14)
        #define FREQ_MEAS_CTRL1_FREQ_MEAS_EN_BITS            (1)
        /* OPEN_LOOP_MANUAL field */
        #define FREQ_MEAS_CTRL1_OPEN_LOOP_MANUAL             (0x00002000u)
        #define FREQ_MEAS_CTRL1_OPEN_LOOP_MANUAL_MASK        (0x00002000u)
        #define FREQ_MEAS_CTRL1_OPEN_LOOP_MANUAL_BIT         (13)
        #define FREQ_MEAS_CTRL1_OPEN_LOOP_MANUAL_BITS        (1)
        /* OPEN_LOOP field */
        #define FREQ_MEAS_CTRL1_OPEN_LOOP                    (0x00001000u)
        #define FREQ_MEAS_CTRL1_OPEN_LOOP_MASK               (0x00001000u)
        #define FREQ_MEAS_CTRL1_OPEN_LOOP_BIT                (12)
        #define FREQ_MEAS_CTRL1_OPEN_LOOP_BITS               (1)
        /* DELAY_FIRST_MEAS field */
        #define FREQ_MEAS_CTRL1_DELAY_FIRST_MEAS             (0x00000400u)
        #define FREQ_MEAS_CTRL1_DELAY_FIRST_MEAS_MASK        (0x00000400u)
        #define FREQ_MEAS_CTRL1_DELAY_FIRST_MEAS_BIT         (10)
        #define FREQ_MEAS_CTRL1_DELAY_FIRST_MEAS_BITS        (1)
        /* DELAY_ALL_MEAS field */
        #define FREQ_MEAS_CTRL1_DELAY_ALL_MEAS               (0x00000200u)
        #define FREQ_MEAS_CTRL1_DELAY_ALL_MEAS_MASK          (0x00000200u)
        #define FREQ_MEAS_CTRL1_DELAY_ALL_MEAS_BIT           (9)
        #define FREQ_MEAS_CTRL1_DELAY_ALL_MEAS_BITS          (1)
        /* BIN_SEARCH_MSB field */
        #define FREQ_MEAS_CTRL1_BIN_SEARCH_MSB               (0x000001C0u)
        #define FREQ_MEAS_CTRL1_BIN_SEARCH_MSB_MASK          (0x000001C0u)
        #define FREQ_MEAS_CTRL1_BIN_SEARCH_MSB_BIT           (6)
        #define FREQ_MEAS_CTRL1_BIN_SEARCH_MSB_BITS          (3)
        /* TUNE_VCO_INIT field */
        #define FREQ_MEAS_CTRL1_TUNE_VCO_INIT                (0x0000003Fu)
        #define FREQ_MEAS_CTRL1_TUNE_VCO_INIT_MASK           (0x0000003Fu)
        #define FREQ_MEAS_CTRL1_TUNE_VCO_INIT_BIT            (0)
        #define FREQ_MEAS_CTRL1_TUNE_VCO_INIT_BITS           (6)

#define FREQ_MEAS_CTRL2                                      *((volatile uint32_t *)0x400010D4u)
#define FREQ_MEAS_CTRL2_REG                                  *((volatile uint32_t *)0x400010D4u)
#define FREQ_MEAS_CTRL2_ADDR                                 (0x400010D4u)
#define FREQ_MEAS_CTRL2_RESET                                (0x0000201Eu)
        /* FREQ_MEAS_TIMER field */
        #define FREQ_MEAS_CTRL2_FREQ_MEAS_TIMER              (0x0000FF00u)
        #define FREQ_MEAS_CTRL2_FREQ_MEAS_TIMER_MASK         (0x0000FF00u)
        #define FREQ_MEAS_CTRL2_FREQ_MEAS_TIMER_BIT          (8)
        #define FREQ_MEAS_CTRL2_FREQ_MEAS_TIMER_BITS         (8)
        /* TARGET_PERIOD field */
        #define FREQ_MEAS_CTRL2_TARGET_PERIOD                (0x000000FFu)
        #define FREQ_MEAS_CTRL2_TARGET_PERIOD_MASK           (0x000000FFu)
        #define FREQ_MEAS_CTRL2_TARGET_PERIOD_BIT            (0)
        #define FREQ_MEAS_CTRL2_TARGET_PERIOD_BITS           (8)

#define FREQ_MEAS_SHIFT                                      *((volatile uint32_t *)0x400010D8u)
#define FREQ_MEAS_SHIFT_REG                                  *((volatile uint32_t *)0x400010D8u)
#define FREQ_MEAS_SHIFT_ADDR                                 (0x400010D8u)
#define FREQ_MEAS_SHIFT_RESET                                (0x00000035u)
        /* FREQ_MEAS_SHIFT field */
        #define FREQ_MEAS_SHIFT_FREQ_MEAS_SHIFT              (0x000000FFu)
        #define FREQ_MEAS_SHIFT_FREQ_MEAS_SHIFT_MASK         (0x000000FFu)
        #define FREQ_MEAS_SHIFT_FREQ_MEAS_SHIFT_BIT          (0)
        #define FREQ_MEAS_SHIFT_FREQ_MEAS_SHIFT_BITS         (8)

#define FREQ_MEAS_STATUS1                                    *((volatile uint32_t *)0x400010DCu)
#define FREQ_MEAS_STATUS1_REG                                *((volatile uint32_t *)0x400010DCu)
#define FREQ_MEAS_STATUS1_ADDR                               (0x400010DCu)
#define FREQ_MEAS_STATUS1_RESET                              (0x00000000u)
        /* INVALID_EDGE field */
        #define FREQ_MEAS_STATUS1_INVALID_EDGE               (0x00008000u)
        #define FREQ_MEAS_STATUS1_INVALID_EDGE_MASK          (0x00008000u)
        #define FREQ_MEAS_STATUS1_INVALID_EDGE_BIT           (15)
        #define FREQ_MEAS_STATUS1_INVALID_EDGE_BITS          (1)
        /* SIGN_FOUND field */
        #define FREQ_MEAS_STATUS1_SIGN_FOUND                 (0x00004000u)
        #define FREQ_MEAS_STATUS1_SIGN_FOUND_MASK            (0x00004000u)
        #define FREQ_MEAS_STATUS1_SIGN_FOUND_BIT             (14)
        #define FREQ_MEAS_STATUS1_SIGN_FOUND_BITS            (1)
        /* FREQ_SIGN field */
        #define FREQ_MEAS_STATUS1_FREQ_SIGN                  (0x00002000u)
        #define FREQ_MEAS_STATUS1_FREQ_SIGN_MASK             (0x00002000u)
        #define FREQ_MEAS_STATUS1_FREQ_SIGN_BIT              (13)
        #define FREQ_MEAS_STATUS1_FREQ_SIGN_BITS             (1)
        /* PERIOD_FOUND field */
        #define FREQ_MEAS_STATUS1_PERIOD_FOUND               (0x00001000u)
        #define FREQ_MEAS_STATUS1_PERIOD_FOUND_MASK          (0x00001000u)
        #define FREQ_MEAS_STATUS1_PERIOD_FOUND_BIT           (12)
        #define FREQ_MEAS_STATUS1_PERIOD_FOUND_BITS          (1)
        /* NEAREST_DIFF field */
        #define FREQ_MEAS_STATUS1_NEAREST_DIFF               (0x000003FFu)
        #define FREQ_MEAS_STATUS1_NEAREST_DIFF_MASK          (0x000003FFu)
        #define FREQ_MEAS_STATUS1_NEAREST_DIFF_BIT           (0)
        #define FREQ_MEAS_STATUS1_NEAREST_DIFF_BITS          (10)

#define FREQ_MEAS_STATUS2                                    *((volatile uint32_t *)0x400010E0u)
#define FREQ_MEAS_STATUS2_REG                                *((volatile uint32_t *)0x400010E0u)
#define FREQ_MEAS_STATUS2_ADDR                               (0x400010E0u)
#define FREQ_MEAS_STATUS2_RESET                              (0x00000000u)
        /* BEAT_TIMER field */
        #define FREQ_MEAS_STATUS2_BEAT_TIMER                 (0x0000FFC0u)
        #define FREQ_MEAS_STATUS2_BEAT_TIMER_MASK            (0x0000FFC0u)
        #define FREQ_MEAS_STATUS2_BEAT_TIMER_BIT             (6)
        #define FREQ_MEAS_STATUS2_BEAT_TIMER_BITS            (10)
        /* BEATS field */
        #define FREQ_MEAS_STATUS2_BEATS                      (0x0000003Fu)
        #define FREQ_MEAS_STATUS2_BEATS_MASK                 (0x0000003Fu)
        #define FREQ_MEAS_STATUS2_BEATS_BIT                  (0)
        #define FREQ_MEAS_STATUS2_BEATS_BITS                 (6)

#define FREQ_MEAS_STATUS3                                    *((volatile uint32_t *)0x400010E4u)
#define FREQ_MEAS_STATUS3_REG                                *((volatile uint32_t *)0x400010E4u)
#define FREQ_MEAS_STATUS3_ADDR                               (0x400010E4u)
#define FREQ_MEAS_STATUS3_RESET                              (0x00000020u)
        /* TUNE_VCO field */
        #define FREQ_MEAS_STATUS3_TUNE_VCO                   (0x0000003Fu)
        #define FREQ_MEAS_STATUS3_TUNE_VCO_MASK              (0x0000003Fu)
        #define FREQ_MEAS_STATUS3_TUNE_VCO_BIT               (0)
        #define FREQ_MEAS_STATUS3_TUNE_VCO_BITS              (6)

#define SCR_CTRL                                             *((volatile uint32_t *)0x400010E8u)
#define SCR_CTRL_REG                                         *((volatile uint32_t *)0x400010E8u)
#define SCR_CTRL_ADDR                                        (0x400010E8u)
#define SCR_CTRL_RESET                                       (0x00000004u)
        /* SCR_RESET field */
        #define SCR_CTRL_SCR_RESET                           (0x00000004u)
        #define SCR_CTRL_SCR_RESET_MASK                      (0x00000004u)
        #define SCR_CTRL_SCR_RESET_BIT                       (2)
        #define SCR_CTRL_SCR_RESET_BITS                      (1)
        /* SCR_WRITE field */
        #define SCR_CTRL_SCR_WRITE                           (0x00000002u)
        #define SCR_CTRL_SCR_WRITE_MASK                      (0x00000002u)
        #define SCR_CTRL_SCR_WRITE_BIT                       (1)
        #define SCR_CTRL_SCR_WRITE_BITS                      (1)
        /* SCR_READ field */
        #define SCR_CTRL_SCR_READ                            (0x00000001u)
        #define SCR_CTRL_SCR_READ_MASK                       (0x00000001u)
        #define SCR_CTRL_SCR_READ_BIT                        (0)
        #define SCR_CTRL_SCR_READ_BITS                       (1)

#define SCR_BUSY                                             *((volatile uint32_t *)0x400010ECu)
#define SCR_BUSY_REG                                         *((volatile uint32_t *)0x400010ECu)
#define SCR_BUSY_ADDR                                        (0x400010ECu)
#define SCR_BUSY_RESET                                       (0x00000000u)
        /* SCR_BUSY field */
        #define SCR_BUSY_SCR_BUSY                            (0x00000001u)
        #define SCR_BUSY_SCR_BUSY_MASK                       (0x00000001u)
        #define SCR_BUSY_SCR_BUSY_BIT                        (0)
        #define SCR_BUSY_SCR_BUSY_BITS                       (1)

#define SCR_ADDR                                             *((volatile uint32_t *)0x400010F0u)
#define SCR_ADDR_REG                                         *((volatile uint32_t *)0x400010F0u)
#define SCR_ADDR_ADDR                                        (0x400010F0u)
#define SCR_ADDR_RESET                                       (0x00000000u)
        /* SCR_ADDR field */
        #define SCR_ADDR_SCR_ADDR                            (0x000000FFu)
        #define SCR_ADDR_SCR_ADDR_MASK                       (0x000000FFu)
        #define SCR_ADDR_SCR_ADDR_BIT                        (0)
        #define SCR_ADDR_SCR_ADDR_BITS                       (8)

#define SCR_WRITE                                            *((volatile uint32_t *)0x400010F4u)
#define SCR_WRITE_REG                                        *((volatile uint32_t *)0x400010F4u)
#define SCR_WRITE_ADDR                                       (0x400010F4u)
#define SCR_WRITE_RESET                                      (0x00000000u)
        /* SCR_WRITE field */
        #define SCR_WRITE_SCR_WRITE                          (0x0000FFFFu)
        #define SCR_WRITE_SCR_WRITE_MASK                     (0x0000FFFFu)
        #define SCR_WRITE_SCR_WRITE_BIT                      (0)
        #define SCR_WRITE_SCR_WRITE_BITS                     (16)

#define SCR_READ                                             *((volatile uint32_t *)0x400010F8u)
#define SCR_READ_REG                                         *((volatile uint32_t *)0x400010F8u)
#define SCR_READ_ADDR                                        (0x400010F8u)
#define SCR_READ_RESET                                       (0x00000000u)
        /* SCR_READ field */
        #define SCR_READ_SCR_READ                            (0x0000FFFFu)
        #define SCR_READ_SCR_READ_MASK                       (0x0000FFFFu)
        #define SCR_READ_SCR_READ_BIT                        (0)
        #define SCR_READ_SCR_READ_BITS                       (16)

#define SYNTH_LOCK                                           *((volatile uint32_t *)0x400010FCu)
#define SYNTH_LOCK_REG                                       *((volatile uint32_t *)0x400010FCu)
#define SYNTH_LOCK_ADDR                                      (0x400010FCu)
#define SYNTH_LOCK_RESET                                     (0x00000000u)
        /* IN_LOCK field */
        #define SYNTH_LOCK_IN_LOCK                           (0x00000001u)
        #define SYNTH_LOCK_IN_LOCK_MASK                      (0x00000001u)
        #define SYNTH_LOCK_IN_LOCK_BIT                       (0)
        #define SYNTH_LOCK_IN_LOCK_BITS                      (1)

#define AN_CAL_STATUS                                        *((volatile uint32_t *)0x40001100u)
#define AN_CAL_STATUS_REG                                    *((volatile uint32_t *)0x40001100u)
#define AN_CAL_STATUS_ADDR                                   (0x40001100u)
#define AN_CAL_STATUS_RESET                                  (0x00000000u)
        /* VCO_CTRL field */
        #define AN_CAL_STATUS_VCO_CTRL                       (0x0000000Cu)
        #define AN_CAL_STATUS_VCO_CTRL_MASK                  (0x0000000Cu)
        #define AN_CAL_STATUS_VCO_CTRL_BIT                   (2)
        #define AN_CAL_STATUS_VCO_CTRL_BITS                  (2)

#define BIAS_CAL_STATUS                                      *((volatile uint32_t *)0x40001104u)
#define BIAS_CAL_STATUS_REG                                  *((volatile uint32_t *)0x40001104u)
#define BIAS_CAL_STATUS_ADDR                                 (0x40001104u)
#define BIAS_CAL_STATUS_RESET                                (0x00000000u)
        /* VCOMP field */
        #define BIAS_CAL_STATUS_VCOMP                        (0x00000002u)
        #define BIAS_CAL_STATUS_VCOMP_MASK                   (0x00000002u)
        #define BIAS_CAL_STATUS_VCOMP_BIT                    (1)
        #define BIAS_CAL_STATUS_VCOMP_BITS                   (1)
        /* ICOMP field */
        #define BIAS_CAL_STATUS_ICOMP                        (0x00000001u)
        #define BIAS_CAL_STATUS_ICOMP_MASK                   (0x00000001u)
        #define BIAS_CAL_STATUS_ICOMP_BIT                    (0)
        #define BIAS_CAL_STATUS_ICOMP_BITS                   (1)

#define ATEST_SEL                                            *((volatile uint32_t *)0x40001108u)
#define ATEST_SEL_REG                                        *((volatile uint32_t *)0x40001108u)
#define ATEST_SEL_ADDR                                       (0x40001108u)
#define ATEST_SEL_RESET                                      (0x00000000u)
        /* ATEST_CTRL field */
        #define ATEST_SEL_ATEST_CTRL                         (0x0000FF00u)
        #define ATEST_SEL_ATEST_CTRL_MASK                    (0x0000FF00u)
        #define ATEST_SEL_ATEST_CTRL_BIT                     (8)
        #define ATEST_SEL_ATEST_CTRL_BITS                    (8)
        /* ATEST_SEL field */
        #define ATEST_SEL_ATEST_SEL                          (0x0000001Fu)
        #define ATEST_SEL_ATEST_SEL_MASK                     (0x0000001Fu)
        #define ATEST_SEL_ATEST_SEL_BIT                      (0)
        #define ATEST_SEL_ATEST_SEL_BITS                     (5)

#define AN_EN_TEST                                           *((volatile uint32_t *)0x4000110Cu)
#define AN_EN_TEST_REG                                       *((volatile uint32_t *)0x4000110Cu)
#define AN_EN_TEST_ADDR                                      (0x4000110Cu)
#define AN_EN_TEST_RESET                                     (0x00000000u)
        /* AN_TEST_MODE field */
        #define AN_EN_TEST_AN_TEST_MODE                      (0x00008000u)
        #define AN_EN_TEST_AN_TEST_MODE_MASK                 (0x00008000u)
        #define AN_EN_TEST_AN_TEST_MODE_BIT                  (15)
        #define AN_EN_TEST_AN_TEST_MODE_BITS                 (1)
        /* PFD_EN field */
        #define AN_EN_TEST_PFD_EN                            (0x00004000u)
        #define AN_EN_TEST_PFD_EN_MASK                       (0x00004000u)
        #define AN_EN_TEST_PFD_EN_BIT                        (14)
        #define AN_EN_TEST_PFD_EN_BITS                       (1)
        /* ADC_EN field */
        #define AN_EN_TEST_ADC_EN                            (0x00002000u)
        #define AN_EN_TEST_ADC_EN_MASK                       (0x00002000u)
        #define AN_EN_TEST_ADC_EN_BIT                        (13)
        #define AN_EN_TEST_ADC_EN_BITS                       (1)
        /* UNUSED field */
        #define AN_EN_TEST_UNUSED                            (0x00001000u)
        #define AN_EN_TEST_UNUSED_MASK                       (0x00001000u)
        #define AN_EN_TEST_UNUSED_BIT                        (12)
        #define AN_EN_TEST_UNUSED_BITS                       (1)
        /* PRE_FILT_EN field */
        #define AN_EN_TEST_PRE_FILT_EN                       (0x00000800u)
        #define AN_EN_TEST_PRE_FILT_EN_MASK                  (0x00000800u)
        #define AN_EN_TEST_PRE_FILT_EN_BIT                   (11)
        #define AN_EN_TEST_PRE_FILT_EN_BITS                  (1)
        /* IF_AMP_EN field */
        #define AN_EN_TEST_IF_AMP_EN                         (0x00000400u)
        #define AN_EN_TEST_IF_AMP_EN_MASK                    (0x00000400u)
        #define AN_EN_TEST_IF_AMP_EN_BIT                     (10)
        #define AN_EN_TEST_IF_AMP_EN_BITS                    (1)
        /* LNA_EN field */
        #define AN_EN_TEST_LNA_EN                            (0x00000200u)
        #define AN_EN_TEST_LNA_EN_MASK                       (0x00000200u)
        #define AN_EN_TEST_LNA_EN_BIT                        (9)
        #define AN_EN_TEST_LNA_EN_BITS                       (1)
        /* MIXER_EN field */
        #define AN_EN_TEST_MIXER_EN                          (0x00000100u)
        #define AN_EN_TEST_MIXER_EN_MASK                     (0x00000100u)
        #define AN_EN_TEST_MIXER_EN_BIT                      (8)
        #define AN_EN_TEST_MIXER_EN_BITS                     (1)
        /* CH_FILT_EN field */
        #define AN_EN_TEST_CH_FILT_EN                        (0x00000080u)
        #define AN_EN_TEST_CH_FILT_EN_MASK                   (0x00000080u)
        #define AN_EN_TEST_CH_FILT_EN_BIT                    (7)
        #define AN_EN_TEST_CH_FILT_EN_BITS                   (1)
        /* MOD_DAC_EN field */
        #define AN_EN_TEST_MOD_DAC_EN                        (0x00000040u)
        #define AN_EN_TEST_MOD_DAC_EN_MASK                   (0x00000040u)
        #define AN_EN_TEST_MOD_DAC_EN_BIT                    (6)
        #define AN_EN_TEST_MOD_DAC_EN_BITS                   (1)
        /* PA_EN field */
        #define AN_EN_TEST_PA_EN                             (0x00000010u)
        #define AN_EN_TEST_PA_EN_MASK                        (0x00000010u)
        #define AN_EN_TEST_PA_EN_BIT                         (4)
        #define AN_EN_TEST_PA_EN_BITS                        (1)
        /* PRESCALER_EN field */
        #define AN_EN_TEST_PRESCALER_EN                      (0x00000008u)
        #define AN_EN_TEST_PRESCALER_EN_MASK                 (0x00000008u)
        #define AN_EN_TEST_PRESCALER_EN_BIT                  (3)
        #define AN_EN_TEST_PRESCALER_EN_BITS                 (1)
        /* VCO_EN field */
        #define AN_EN_TEST_VCO_EN                            (0x00000004u)
        #define AN_EN_TEST_VCO_EN_MASK                       (0x00000004u)
        #define AN_EN_TEST_VCO_EN_BIT                        (2)
        #define AN_EN_TEST_VCO_EN_BITS                       (1)
        /* BIAS_EN field */
        #define AN_EN_TEST_BIAS_EN                           (0x00000001u)
        #define AN_EN_TEST_BIAS_EN_MASK                      (0x00000001u)
        #define AN_EN_TEST_BIAS_EN_BIT                       (0)
        #define AN_EN_TEST_BIAS_EN_BITS                      (1)

#define TUNE_FILTER_CTRL                                     *((volatile uint32_t *)0x40001110u)
#define TUNE_FILTER_CTRL_REG                                 *((volatile uint32_t *)0x40001110u)
#define TUNE_FILTER_CTRL_ADDR                                (0x40001110u)
#define TUNE_FILTER_CTRL_RESET                               (0x00000000u)
        /* TUNE_FILTER_EN field */
        #define TUNE_FILTER_CTRL_TUNE_FILTER_EN              (0x00000002u)
        #define TUNE_FILTER_CTRL_TUNE_FILTER_EN_MASK         (0x00000002u)
        #define TUNE_FILTER_CTRL_TUNE_FILTER_EN_BIT          (1)
        #define TUNE_FILTER_CTRL_TUNE_FILTER_EN_BITS         (1)
        /* TUNE_FILTER_RESET field */
        #define TUNE_FILTER_CTRL_TUNE_FILTER_RESET           (0x00000001u)
        #define TUNE_FILTER_CTRL_TUNE_FILTER_RESET_MASK      (0x00000001u)
        #define TUNE_FILTER_CTRL_TUNE_FILTER_RESET_BIT       (0)
        #define TUNE_FILTER_CTRL_TUNE_FILTER_RESET_BITS      (1)

#define NOISE_EN                                             *((volatile uint32_t *)0x40001114u)
#define NOISE_EN_REG                                         *((volatile uint32_t *)0x40001114u)
#define NOISE_EN_ADDR                                        (0x40001114u)
#define NOISE_EN_RESET                                       (0x00000000u)
        /* NOISE_EN field */
        #define NOISE_EN_NOISE_EN                            (0x00000001u)
        #define NOISE_EN_NOISE_EN_MASK                       (0x00000001u)
        #define NOISE_EN_NOISE_EN_BIT                        (0)
        #define NOISE_EN_NOISE_EN_BITS                       (1)

/* MAC block */
#define BLOCK_MAC_BASE                                       (0x40002000u)
#define BLOCK_MAC_END                                        (0x400020C8u)
#define BLOCK_MAC_SIZE                                       (BLOCK_MAC_END - BLOCK_MAC_BASE + 1)

#define MAC_RX_ST_ADDR_A                                     *((volatile uint32_t *)0x40002000u)
#define MAC_RX_ST_ADDR_A_REG                                 *((volatile uint32_t *)0x40002000u)
#define MAC_RX_ST_ADDR_A_ADDR                                (0x40002000u)
#define MAC_RX_ST_ADDR_A_RESET                               (0x20000000u)
        /* MAC_RAM_OFFS field */
        #define MAC_RX_ST_ADDR_A_MAC_RAM_OFFS                (0xFFFF0000u)
        #define MAC_RX_ST_ADDR_A_MAC_RAM_OFFS_MASK           (0xFFFF0000u)
        #define MAC_RX_ST_ADDR_A_MAC_RAM_OFFS_BIT            (16)
        #define MAC_RX_ST_ADDR_A_MAC_RAM_OFFS_BITS           (16)
        /* MAC_RX_ST_ADDR_A field */
        #define MAC_RX_ST_ADDR_A_MAC_RX_ST_ADDR_A            (0x0000FFFEu)
        #define MAC_RX_ST_ADDR_A_MAC_RX_ST_ADDR_A_MASK       (0x0000FFFEu)
        #define MAC_RX_ST_ADDR_A_MAC_RX_ST_ADDR_A_BIT        (1)
        #define MAC_RX_ST_ADDR_A_MAC_RX_ST_ADDR_A_BITS       (15)

#define MAC_RX_END_ADDR_A                                    *((volatile uint32_t *)0x40002004u)
#define MAC_RX_END_ADDR_A_REG                                *((volatile uint32_t *)0x40002004u)
#define MAC_RX_END_ADDR_A_ADDR                               (0x40002004u)
#define MAC_RX_END_ADDR_A_RESET                              (0x20000088u)
        /* MAC_RAM_OFFS field */
        #define MAC_RX_END_ADDR_A_MAC_RAM_OFFS               (0xFFFF0000u)
        #define MAC_RX_END_ADDR_A_MAC_RAM_OFFS_MASK          (0xFFFF0000u)
        #define MAC_RX_END_ADDR_A_MAC_RAM_OFFS_BIT           (16)
        #define MAC_RX_END_ADDR_A_MAC_RAM_OFFS_BITS          (16)
        /* MAC_RX_END_ADDR_A field */
        #define MAC_RX_END_ADDR_A_MAC_RX_END_ADDR_A          (0x0000FFFEu)
        #define MAC_RX_END_ADDR_A_MAC_RX_END_ADDR_A_MASK     (0x0000FFFEu)
        #define MAC_RX_END_ADDR_A_MAC_RX_END_ADDR_A_BIT      (1)
        #define MAC_RX_END_ADDR_A_MAC_RX_END_ADDR_A_BITS     (15)

#define MAC_RX_ST_ADDR_B                                     *((volatile uint32_t *)0x40002008u)
#define MAC_RX_ST_ADDR_B_REG                                 *((volatile uint32_t *)0x40002008u)
#define MAC_RX_ST_ADDR_B_ADDR                                (0x40002008u)
#define MAC_RX_ST_ADDR_B_RESET                               (0x20000000u)
        /* MAC_RAM_OFFS field */
        #define MAC_RX_ST_ADDR_B_MAC_RAM_OFFS                (0xFFFF0000u)
        #define MAC_RX_ST_ADDR_B_MAC_RAM_OFFS_MASK           (0xFFFF0000u)
        #define MAC_RX_ST_ADDR_B_MAC_RAM_OFFS_BIT            (16)
        #define MAC_RX_ST_ADDR_B_MAC_RAM_OFFS_BITS           (16)
        /* MAC_RX_ST_ADDR_B field */
        #define MAC_RX_ST_ADDR_B_MAC_RX_ST_ADDR_B            (0x0000FFFEu)
        #define MAC_RX_ST_ADDR_B_MAC_RX_ST_ADDR_B_MASK       (0x0000FFFEu)
        #define MAC_RX_ST_ADDR_B_MAC_RX_ST_ADDR_B_BIT        (1)
        #define MAC_RX_ST_ADDR_B_MAC_RX_ST_ADDR_B_BITS       (15)

#define MAC_RX_END_ADDR_B                                    *((volatile uint32_t *)0x4000200Cu)
#define MAC_RX_END_ADDR_B_REG                                *((volatile uint32_t *)0x4000200Cu)
#define MAC_RX_END_ADDR_B_ADDR                               (0x4000200Cu)
#define MAC_RX_END_ADDR_B_RESET                              (0x20000088u)
        /* MAC_RAM_OFFS field */
        #define MAC_RX_END_ADDR_B_MAC_RAM_OFFS               (0xFFFF0000u)
        #define MAC_RX_END_ADDR_B_MAC_RAM_OFFS_MASK          (0xFFFF0000u)
        #define MAC_RX_END_ADDR_B_MAC_RAM_OFFS_BIT           (16)
        #define MAC_RX_END_ADDR_B_MAC_RAM_OFFS_BITS          (16)
        /* MAC_RX_END_ADDR_B field */
        #define MAC_RX_END_ADDR_B_MAC_RX_END_ADDR_B          (0x0000FFFEu)
        #define MAC_RX_END_ADDR_B_MAC_RX_END_ADDR_B_MASK     (0x0000FFFEu)
        #define MAC_RX_END_ADDR_B_MAC_RX_END_ADDR_B_BIT      (1)
        #define MAC_RX_END_ADDR_B_MAC_RX_END_ADDR_B_BITS     (15)

#define MAC_TX_ST_ADDR_A                                     *((volatile uint32_t *)0x40002010u)
#define MAC_TX_ST_ADDR_A_REG                                 *((volatile uint32_t *)0x40002010u)
#define MAC_TX_ST_ADDR_A_ADDR                                (0x40002010u)
#define MAC_TX_ST_ADDR_A_RESET                               (0x20000000u)
        /* MAC_RAM_OFFS field */
        #define MAC_TX_ST_ADDR_A_MAC_RAM_OFFS                (0xFFFF0000u)
        #define MAC_TX_ST_ADDR_A_MAC_RAM_OFFS_MASK           (0xFFFF0000u)
        #define MAC_TX_ST_ADDR_A_MAC_RAM_OFFS_BIT            (16)
        #define MAC_TX_ST_ADDR_A_MAC_RAM_OFFS_BITS           (16)
        /* MAC_TX_ST_ADDR_A field */
        #define MAC_TX_ST_ADDR_A_MAC_TX_ST_ADDR_A            (0x0000FFFEu)
        #define MAC_TX_ST_ADDR_A_MAC_TX_ST_ADDR_A_MASK       (0x0000FFFEu)
        #define MAC_TX_ST_ADDR_A_MAC_TX_ST_ADDR_A_BIT        (1)
        #define MAC_TX_ST_ADDR_A_MAC_TX_ST_ADDR_A_BITS       (15)

#define MAC_TX_END_ADDR_A                                    *((volatile uint32_t *)0x40002014u)
#define MAC_TX_END_ADDR_A_REG                                *((volatile uint32_t *)0x40002014u)
#define MAC_TX_END_ADDR_A_ADDR                               (0x40002014u)
#define MAC_TX_END_ADDR_A_RESET                              (0x20000000u)
        /* MAC_RAM_OFFS field */
        #define MAC_TX_END_ADDR_A_MAC_RAM_OFFS               (0xFFFF0000u)
        #define MAC_TX_END_ADDR_A_MAC_RAM_OFFS_MASK          (0xFFFF0000u)
        #define MAC_TX_END_ADDR_A_MAC_RAM_OFFS_BIT           (16)
        #define MAC_TX_END_ADDR_A_MAC_RAM_OFFS_BITS          (16)
        /* MAC_TX_END_ADDR_A field */
        #define MAC_TX_END_ADDR_A_MAC_TX_END_ADDR_A          (0x0000FFFEu)
        #define MAC_TX_END_ADDR_A_MAC_TX_END_ADDR_A_MASK     (0x0000FFFEu)
        #define MAC_TX_END_ADDR_A_MAC_TX_END_ADDR_A_BIT      (1)
        #define MAC_TX_END_ADDR_A_MAC_TX_END_ADDR_A_BITS     (15)

#define MAC_TX_ST_ADDR_B                                     *((volatile uint32_t *)0x40002018u)
#define MAC_TX_ST_ADDR_B_REG                                 *((volatile uint32_t *)0x40002018u)
#define MAC_TX_ST_ADDR_B_ADDR                                (0x40002018u)
#define MAC_TX_ST_ADDR_B_RESET                               (0x20000000u)
        /* MAC_RAM_OFFS field */
        #define MAC_TX_ST_ADDR_B_MAC_RAM_OFFS                (0xFFFF0000u)
        #define MAC_TX_ST_ADDR_B_MAC_RAM_OFFS_MASK           (0xFFFF0000u)
        #define MAC_TX_ST_ADDR_B_MAC_RAM_OFFS_BIT            (16)
        #define MAC_TX_ST_ADDR_B_MAC_RAM_OFFS_BITS           (16)
        /* MAC_TX_ST_ADDR_B field */
        #define MAC_TX_ST_ADDR_B_MAC_TX_ST_ADDR_B            (0x0000FFFEu)
        #define MAC_TX_ST_ADDR_B_MAC_TX_ST_ADDR_B_MASK       (0x0000FFFEu)
        #define MAC_TX_ST_ADDR_B_MAC_TX_ST_ADDR_B_BIT        (1)
        #define MAC_TX_ST_ADDR_B_MAC_TX_ST_ADDR_B_BITS       (15)

#define MAC_TX_END_ADDR_B                                    *((volatile uint32_t *)0x4000201Cu)
#define MAC_TX_END_ADDR_B_REG                                *((volatile uint32_t *)0x4000201Cu)
#define MAC_TX_END_ADDR_B_ADDR                               (0x4000201Cu)
#define MAC_TX_END_ADDR_B_RESET                              (0x20000000u)
        /* MAC_RAM_OFFS field */
        #define MAC_TX_END_ADDR_B_MAC_RAM_OFFS               (0xFFFF0000u)
        #define MAC_TX_END_ADDR_B_MAC_RAM_OFFS_MASK          (0xFFFF0000u)
        #define MAC_TX_END_ADDR_B_MAC_RAM_OFFS_BIT           (16)
        #define MAC_TX_END_ADDR_B_MAC_RAM_OFFS_BITS          (16)
        /* MAC_TX_END_ADDR_B field */
        #define MAC_TX_END_ADDR_B_MAC_TX_END_ADDR_B          (0x0000FFFEu)
        #define MAC_TX_END_ADDR_B_MAC_TX_END_ADDR_B_MASK     (0x0000FFFEu)
        #define MAC_TX_END_ADDR_B_MAC_TX_END_ADDR_B_BIT      (1)
        #define MAC_TX_END_ADDR_B_MAC_TX_END_ADDR_B_BITS     (15)

#define RX_A_COUNT                                           *((volatile uint32_t *)0x40002020u)
#define RX_A_COUNT_REG                                       *((volatile uint32_t *)0x40002020u)
#define RX_A_COUNT_ADDR                                      (0x40002020u)
#define RX_A_COUNT_RESET                                     (0x00000000u)
        /* RX_A_COUNT field */
        #define RX_A_COUNT_RX_A_COUNT                        (0x000007FFu)
        #define RX_A_COUNT_RX_A_COUNT_MASK                   (0x000007FFu)
        #define RX_A_COUNT_RX_A_COUNT_BIT                    (0)
        #define RX_A_COUNT_RX_A_COUNT_BITS                   (11)

#define RX_B_COUNT                                           *((volatile uint32_t *)0x40002024u)
#define RX_B_COUNT_REG                                       *((volatile uint32_t *)0x40002024u)
#define RX_B_COUNT_ADDR                                      (0x40002024u)
#define RX_B_COUNT_RESET                                     (0x00000000u)
        /* RX_B_COUNT field */
        #define RX_B_COUNT_RX_B_COUNT                        (0x000007FFu)
        #define RX_B_COUNT_RX_B_COUNT_MASK                   (0x000007FFu)
        #define RX_B_COUNT_RX_B_COUNT_BIT                    (0)
        #define RX_B_COUNT_RX_B_COUNT_BITS                   (11)

#define TX_COUNT                                             *((volatile uint32_t *)0x40002028u)
#define TX_COUNT_REG                                         *((volatile uint32_t *)0x40002028u)
#define TX_COUNT_ADDR                                        (0x40002028u)
#define TX_COUNT_RESET                                       (0x00000000u)
        /* TX_COUNT field */
        #define TX_COUNT_TX_COUNT                            (0x000007FFu)
        #define TX_COUNT_TX_COUNT_MASK                       (0x000007FFu)
        #define TX_COUNT_TX_COUNT_BIT                        (0)
        #define TX_COUNT_TX_COUNT_BITS                       (11)

#define MAC_DMA_STATUS                                       *((volatile uint32_t *)0x4000202Cu)
#define MAC_DMA_STATUS_REG                                   *((volatile uint32_t *)0x4000202Cu)
#define MAC_DMA_STATUS_ADDR                                  (0x4000202Cu)
#define MAC_DMA_STATUS_RESET                                 (0x00000000u)
        /* TX_ACTIVE_B field */
        #define MAC_DMA_STATUS_TX_ACTIVE_B                   (0x00000008u)
        #define MAC_DMA_STATUS_TX_ACTIVE_B_MASK              (0x00000008u)
        #define MAC_DMA_STATUS_TX_ACTIVE_B_BIT               (3)
        #define MAC_DMA_STATUS_TX_ACTIVE_B_BITS              (1)
        /* TX_ACTIVE_A field */
        #define MAC_DMA_STATUS_TX_ACTIVE_A                   (0x00000004u)
        #define MAC_DMA_STATUS_TX_ACTIVE_A_MASK              (0x00000004u)
        #define MAC_DMA_STATUS_TX_ACTIVE_A_BIT               (2)
        #define MAC_DMA_STATUS_TX_ACTIVE_A_BITS              (1)
        /* RX_ACTIVE_B field */
        #define MAC_DMA_STATUS_RX_ACTIVE_B                   (0x00000002u)
        #define MAC_DMA_STATUS_RX_ACTIVE_B_MASK              (0x00000002u)
        #define MAC_DMA_STATUS_RX_ACTIVE_B_BIT               (1)
        #define MAC_DMA_STATUS_RX_ACTIVE_B_BITS              (1)
        /* RX_ACTIVE_A field */
        #define MAC_DMA_STATUS_RX_ACTIVE_A                   (0x00000001u)
        #define MAC_DMA_STATUS_RX_ACTIVE_A_MASK              (0x00000001u)
        #define MAC_DMA_STATUS_RX_ACTIVE_A_BIT               (0)
        #define MAC_DMA_STATUS_RX_ACTIVE_A_BITS              (1)

#define MAC_DMA_CONFIG                                       *((volatile uint32_t *)0x40002030u)
#define MAC_DMA_CONFIG_REG                                   *((volatile uint32_t *)0x40002030u)
#define MAC_DMA_CONFIG_ADDR                                  (0x40002030u)
#define MAC_DMA_CONFIG_RESET                                 (0x00000000u)
        /* TX_DMA_RESET field */
        #define MAC_DMA_CONFIG_TX_DMA_RESET                  (0x00000020u)
        #define MAC_DMA_CONFIG_TX_DMA_RESET_MASK             (0x00000020u)
        #define MAC_DMA_CONFIG_TX_DMA_RESET_BIT              (5)
        #define MAC_DMA_CONFIG_TX_DMA_RESET_BITS             (1)
        /* RX_DMA_RESET field */
        #define MAC_DMA_CONFIG_RX_DMA_RESET                  (0x00000010u)
        #define MAC_DMA_CONFIG_RX_DMA_RESET_MASK             (0x00000010u)
        #define MAC_DMA_CONFIG_RX_DMA_RESET_BIT              (4)
        #define MAC_DMA_CONFIG_RX_DMA_RESET_BITS             (1)
        /* TX_LOAD_B field */
        #define MAC_DMA_CONFIG_TX_LOAD_B                     (0x00000008u)
        #define MAC_DMA_CONFIG_TX_LOAD_B_MASK                (0x00000008u)
        #define MAC_DMA_CONFIG_TX_LOAD_B_BIT                 (3)
        #define MAC_DMA_CONFIG_TX_LOAD_B_BITS                (1)
        /* TX_LOAD_A field */
        #define MAC_DMA_CONFIG_TX_LOAD_A                     (0x00000004u)
        #define MAC_DMA_CONFIG_TX_LOAD_A_MASK                (0x00000004u)
        #define MAC_DMA_CONFIG_TX_LOAD_A_BIT                 (2)
        #define MAC_DMA_CONFIG_TX_LOAD_A_BITS                (1)
        /* RX_LOAD_B field */
        #define MAC_DMA_CONFIG_RX_LOAD_B                     (0x00000002u)
        #define MAC_DMA_CONFIG_RX_LOAD_B_MASK                (0x00000002u)
        #define MAC_DMA_CONFIG_RX_LOAD_B_BIT                 (1)
        #define MAC_DMA_CONFIG_RX_LOAD_B_BITS                (1)
        /* RX_LOAD_A field */
        #define MAC_DMA_CONFIG_RX_LOAD_A                     (0x00000001u)
        #define MAC_DMA_CONFIG_RX_LOAD_A_MASK                (0x00000001u)
        #define MAC_DMA_CONFIG_RX_LOAD_A_BIT                 (0)
        #define MAC_DMA_CONFIG_RX_LOAD_A_BITS                (1)

#define MAC_TIMER                                            *((volatile uint32_t *)0x40002038u)
#define MAC_TIMER_REG                                        *((volatile uint32_t *)0x40002038u)
#define MAC_TIMER_ADDR                                       (0x40002038u)
#define MAC_TIMER_RESET                                      (0x00000000u)
        /* MAC_TIMER field */
        #define MAC_TIMER_MAC_TIMER                          (0x000FFFFFu)
        #define MAC_TIMER_MAC_TIMER_MASK                     (0x000FFFFFu)
        #define MAC_TIMER_MAC_TIMER_BIT                      (0)
        #define MAC_TIMER_MAC_TIMER_BITS                     (20)

#define MAC_TIMER_COMPARE_A_H                                *((volatile uint32_t *)0x40002040u)
#define MAC_TIMER_COMPARE_A_H_REG                            *((volatile uint32_t *)0x40002040u)
#define MAC_TIMER_COMPARE_A_H_ADDR                           (0x40002040u)
#define MAC_TIMER_COMPARE_A_H_RESET                          (0x00000000u)
        /* MAC_COMPARE_A_H field */
        #define MAC_TIMER_COMPARE_A_H_MAC_COMPARE_A_H        (0x0000000Fu)
        #define MAC_TIMER_COMPARE_A_H_MAC_COMPARE_A_H_MASK   (0x0000000Fu)
        #define MAC_TIMER_COMPARE_A_H_MAC_COMPARE_A_H_BIT    (0)
        #define MAC_TIMER_COMPARE_A_H_MAC_COMPARE_A_H_BITS   (4)

#define MAC_TIMER_COMPARE_A_L                                *((volatile uint32_t *)0x40002044u)
#define MAC_TIMER_COMPARE_A_L_REG                            *((volatile uint32_t *)0x40002044u)
#define MAC_TIMER_COMPARE_A_L_ADDR                           (0x40002044u)
#define MAC_TIMER_COMPARE_A_L_RESET                          (0x00000000u)
        /* MAC_COMPARE_A_L field */
        #define MAC_TIMER_COMPARE_A_L_MAC_COMPARE_A_L        (0x0000FFFFu)
        #define MAC_TIMER_COMPARE_A_L_MAC_COMPARE_A_L_MASK   (0x0000FFFFu)
        #define MAC_TIMER_COMPARE_A_L_MAC_COMPARE_A_L_BIT    (0)
        #define MAC_TIMER_COMPARE_A_L_MAC_COMPARE_A_L_BITS   (16)

#define MAC_TIMER_COMPARE_B_H                                *((volatile uint32_t *)0x40002048u)
#define MAC_TIMER_COMPARE_B_H_REG                            *((volatile uint32_t *)0x40002048u)
#define MAC_TIMER_COMPARE_B_H_ADDR                           (0x40002048u)
#define MAC_TIMER_COMPARE_B_H_RESET                          (0x00000000u)
        /* MAC_COMPARE_B_H field */
        #define MAC_TIMER_COMPARE_B_H_MAC_COMPARE_B_H        (0x0000000Fu)
        #define MAC_TIMER_COMPARE_B_H_MAC_COMPARE_B_H_MASK   (0x0000000Fu)
        #define MAC_TIMER_COMPARE_B_H_MAC_COMPARE_B_H_BIT    (0)
        #define MAC_TIMER_COMPARE_B_H_MAC_COMPARE_B_H_BITS   (4)

#define MAC_TIMER_COMPARE_B_L                                *((volatile uint32_t *)0x4000204Cu)
#define MAC_TIMER_COMPARE_B_L_REG                            *((volatile uint32_t *)0x4000204Cu)
#define MAC_TIMER_COMPARE_B_L_ADDR                           (0x4000204Cu)
#define MAC_TIMER_COMPARE_B_L_RESET                          (0x00000000u)
        /* MAC_COMPARE_B_L field */
        #define MAC_TIMER_COMPARE_B_L_MAC_COMPARE_B_L        (0x0000FFFFu)
        #define MAC_TIMER_COMPARE_B_L_MAC_COMPARE_B_L_MASK   (0x0000FFFFu)
        #define MAC_TIMER_COMPARE_B_L_MAC_COMPARE_B_L_BIT    (0)
        #define MAC_TIMER_COMPARE_B_L_MAC_COMPARE_B_L_BITS   (16)

#define MAC_TIMER_CAPTURE_H                                  *((volatile uint32_t *)0x40002050u)
#define MAC_TIMER_CAPTURE_H_REG                              *((volatile uint32_t *)0x40002050u)
#define MAC_TIMER_CAPTURE_H_ADDR                             (0x40002050u)
#define MAC_TIMER_CAPTURE_H_RESET                            (0x00000000u)
        /* MAC_SFD_CAPTURE_HIGH field */
        #define MAC_TIMER_CAPTURE_H_MAC_SFD_CAPTURE_HIGH     (0x0000000Fu)
        #define MAC_TIMER_CAPTURE_H_MAC_SFD_CAPTURE_HIGH_MASK (0x0000000Fu)
        #define MAC_TIMER_CAPTURE_H_MAC_SFD_CAPTURE_HIGH_BIT (0)
        #define MAC_TIMER_CAPTURE_H_MAC_SFD_CAPTURE_HIGH_BITS (4)

#define MAC_TIMER_CAPTURE_L                                  *((volatile uint32_t *)0x40002054u)
#define MAC_TIMER_CAPTURE_L_REG                              *((volatile uint32_t *)0x40002054u)
#define MAC_TIMER_CAPTURE_L_ADDR                             (0x40002054u)
#define MAC_TIMER_CAPTURE_L_RESET                            (0x00000000u)
        /* MAC_SFD_CAPTURE_LOW field */
        #define MAC_TIMER_CAPTURE_L_MAC_SFD_CAPTURE_LOW      (0x0000FFFFu)
        #define MAC_TIMER_CAPTURE_L_MAC_SFD_CAPTURE_LOW_MASK (0x0000FFFFu)
        #define MAC_TIMER_CAPTURE_L_MAC_SFD_CAPTURE_LOW_BIT  (0)
        #define MAC_TIMER_CAPTURE_L_MAC_SFD_CAPTURE_LOW_BITS (16)

#define MAC_BO_TIMER                                         *((volatile uint32_t *)0x40002058u)
#define MAC_BO_TIMER_REG                                     *((volatile uint32_t *)0x40002058u)
#define MAC_BO_TIMER_ADDR                                    (0x40002058u)
#define MAC_BO_TIMER_RESET                                   (0x00000000u)
        /* MAC_BO_TIMER field */
        #define MAC_BO_TIMER_MAC_BO_TIMER                    (0x00000FFFu)
        #define MAC_BO_TIMER_MAC_BO_TIMER_MASK               (0x00000FFFu)
        #define MAC_BO_TIMER_MAC_BO_TIMER_BIT                (0)
        #define MAC_BO_TIMER_MAC_BO_TIMER_BITS               (12)

#define MAC_BOP_TIMER                                        *((volatile uint32_t *)0x4000205Cu)
#define MAC_BOP_TIMER_REG                                    *((volatile uint32_t *)0x4000205Cu)
#define MAC_BOP_TIMER_ADDR                                   (0x4000205Cu)
#define MAC_BOP_TIMER_RESET                                  (0x00000000u)
        /* MAC_BOP_TIMER field */
        #define MAC_BOP_TIMER_MAC_BOP_TIMER                  (0x0000007Fu)
        #define MAC_BOP_TIMER_MAC_BOP_TIMER_MASK             (0x0000007Fu)
        #define MAC_BOP_TIMER_MAC_BOP_TIMER_BIT              (0)
        #define MAC_BOP_TIMER_MAC_BOP_TIMER_BITS             (7)

#define MAC_TX_STROBE                                        *((volatile uint32_t *)0x40002060u)
#define MAC_TX_STROBE_REG                                    *((volatile uint32_t *)0x40002060u)
#define MAC_TX_STROBE_ADDR                                   (0x40002060u)
#define MAC_TX_STROBE_RESET                                  (0x00000000u)
        /* AUTO_CRC_TX field */
        #define MAC_TX_STROBE_AUTO_CRC_TX                    (0x00000008u)
        #define MAC_TX_STROBE_AUTO_CRC_TX_MASK               (0x00000008u)
        #define MAC_TX_STROBE_AUTO_CRC_TX_BIT                (3)
        #define MAC_TX_STROBE_AUTO_CRC_TX_BITS               (1)
        /* CCA_ON field */
        #define MAC_TX_STROBE_CCA_ON                         (0x00000004u)
        #define MAC_TX_STROBE_CCA_ON_MASK                    (0x00000004u)
        #define MAC_TX_STROBE_CCA_ON_BIT                     (2)
        #define MAC_TX_STROBE_CCA_ON_BITS                    (1)
        /* MAC_TX_RST field */
        #define MAC_TX_STROBE_MAC_TX_RST                     (0x00000002u)
        #define MAC_TX_STROBE_MAC_TX_RST_MASK                (0x00000002u)
        #define MAC_TX_STROBE_MAC_TX_RST_BIT                 (1)
        #define MAC_TX_STROBE_MAC_TX_RST_BITS                (1)
        /* START_TX field */
        #define MAC_TX_STROBE_START_TX                       (0x00000001u)
        #define MAC_TX_STROBE_START_TX_MASK                  (0x00000001u)
        #define MAC_TX_STROBE_START_TX_BIT                   (0)
        #define MAC_TX_STROBE_START_TX_BITS                  (1)

#define MAC_ACK_STROBE                                       *((volatile uint32_t *)0x40002064u)
#define MAC_ACK_STROBE_REG                                   *((volatile uint32_t *)0x40002064u)
#define MAC_ACK_STROBE_ADDR                                  (0x40002064u)
#define MAC_ACK_STROBE_RESET                                 (0x00000000u)
        /* MANUAL_ACK field */
        #define MAC_ACK_STROBE_MANUAL_ACK                    (0x00000002u)
        #define MAC_ACK_STROBE_MANUAL_ACK_MASK               (0x00000002u)
        #define MAC_ACK_STROBE_MANUAL_ACK_BIT                (1)
        #define MAC_ACK_STROBE_MANUAL_ACK_BITS               (1)
        /* FRAME_PENDING field */
        #define MAC_ACK_STROBE_FRAME_PENDING                 (0x00000001u)
        #define MAC_ACK_STROBE_FRAME_PENDING_MASK            (0x00000001u)
        #define MAC_ACK_STROBE_FRAME_PENDING_BIT             (0)
        #define MAC_ACK_STROBE_FRAME_PENDING_BITS            (1)

#define MAC_STATUS                                           *((volatile uint32_t *)0x40002068u)
#define MAC_STATUS_REG                                       *((volatile uint32_t *)0x40002068u)
#define MAC_STATUS_ADDR                                      (0x40002068u)
#define MAC_STATUS_RESET                                     (0x00000000u)
        /* RX_B_PEND_TX_ACK field */
        #define MAC_STATUS_RX_B_PEND_TX_ACK                  (0x00000800u)
        #define MAC_STATUS_RX_B_PEND_TX_ACK_MASK             (0x00000800u)
        #define MAC_STATUS_RX_B_PEND_TX_ACK_BIT              (11)
        #define MAC_STATUS_RX_B_PEND_TX_ACK_BITS             (1)
        /* RX_A_PEND_TX_ACK field */
        #define MAC_STATUS_RX_A_PEND_TX_ACK                  (0x00000400u)
        #define MAC_STATUS_RX_A_PEND_TX_ACK_MASK             (0x00000400u)
        #define MAC_STATUS_RX_A_PEND_TX_ACK_BIT              (10)
        #define MAC_STATUS_RX_A_PEND_TX_ACK_BITS             (1)
        /* RX_B_LAST_UNLOAD field */
        #define MAC_STATUS_RX_B_LAST_UNLOAD                  (0x00000200u)
        #define MAC_STATUS_RX_B_LAST_UNLOAD_MASK             (0x00000200u)
        #define MAC_STATUS_RX_B_LAST_UNLOAD_BIT              (9)
        #define MAC_STATUS_RX_B_LAST_UNLOAD_BITS             (1)
        /* RX_A_LAST_UNLOAD field */
        #define MAC_STATUS_RX_A_LAST_UNLOAD                  (0x00000100u)
        #define MAC_STATUS_RX_A_LAST_UNLOAD_MASK             (0x00000100u)
        #define MAC_STATUS_RX_A_LAST_UNLOAD_BIT              (8)
        #define MAC_STATUS_RX_A_LAST_UNLOAD_BITS             (1)
        /* WRONG_FORMAT field */
        #define MAC_STATUS_WRONG_FORMAT                      (0x00000080u)
        #define MAC_STATUS_WRONG_FORMAT_MASK                 (0x00000080u)
        #define MAC_STATUS_WRONG_FORMAT_BIT                  (7)
        #define MAC_STATUS_WRONG_FORMAT_BITS                 (1)
        /* WRONG_ADDRESS field */
        #define MAC_STATUS_WRONG_ADDRESS                     (0x00000040u)
        #define MAC_STATUS_WRONG_ADDRESS_MASK                (0x00000040u)
        #define MAC_STATUS_WRONG_ADDRESS_BIT                 (6)
        #define MAC_STATUS_WRONG_ADDRESS_BITS                (1)
        /* RX_ACK_REC field */
        #define MAC_STATUS_RX_ACK_REC                        (0x00000020u)
        #define MAC_STATUS_RX_ACK_REC_MASK                   (0x00000020u)
        #define MAC_STATUS_RX_ACK_REC_BIT                    (5)
        #define MAC_STATUS_RX_ACK_REC_BITS                   (1)
        /* SENDING_ACK field */
        #define MAC_STATUS_SENDING_ACK                       (0x00000010u)
        #define MAC_STATUS_SENDING_ACK_MASK                  (0x00000010u)
        #define MAC_STATUS_SENDING_ACK_BIT                   (4)
        #define MAC_STATUS_SENDING_ACK_BITS                  (1)
        /* RUN_BO field */
        #define MAC_STATUS_RUN_BO                            (0x00000008u)
        #define MAC_STATUS_RUN_BO_MASK                       (0x00000008u)
        #define MAC_STATUS_RUN_BO_BIT                        (3)
        #define MAC_STATUS_RUN_BO_BITS                       (1)
        /* TX_FRAME field */
        #define MAC_STATUS_TX_FRAME                          (0x00000004u)
        #define MAC_STATUS_TX_FRAME_MASK                     (0x00000004u)
        #define MAC_STATUS_TX_FRAME_BIT                      (2)
        #define MAC_STATUS_TX_FRAME_BITS                     (1)
        /* RX_FRAME field */
        #define MAC_STATUS_RX_FRAME                          (0x00000002u)
        #define MAC_STATUS_RX_FRAME_MASK                     (0x00000002u)
        #define MAC_STATUS_RX_FRAME_BIT                      (1)
        #define MAC_STATUS_RX_FRAME_BITS                     (1)
        /* RX_CRC_PASS field */
        #define MAC_STATUS_RX_CRC_PASS                       (0x00000001u)
        #define MAC_STATUS_RX_CRC_PASS_MASK                  (0x00000001u)
        #define MAC_STATUS_RX_CRC_PASS_BIT                   (0)
        #define MAC_STATUS_RX_CRC_PASS_BITS                  (1)

#define TX_CRC                                               *((volatile uint32_t *)0x4000206Cu)
#define TX_CRC_REG                                           *((volatile uint32_t *)0x4000206Cu)
#define TX_CRC_ADDR                                          (0x4000206Cu)
#define TX_CRC_RESET                                         (0x00000000u)
        /* TX_CRC field */
        #define TX_CRC_TX_CRC                                (0x0000FFFFu)
        #define TX_CRC_TX_CRC_MASK                           (0x0000FFFFu)
        #define TX_CRC_TX_CRC_BIT                            (0)
        #define TX_CRC_TX_CRC_BITS                           (16)

#define RX_CRC                                               *((volatile uint32_t *)0x40002070u)
#define RX_CRC_REG                                           *((volatile uint32_t *)0x40002070u)
#define RX_CRC_ADDR                                          (0x40002070u)
#define RX_CRC_RESET                                         (0x00000000u)
        /* RX_CRC field */
        #define RX_CRC_RX_CRC                                (0x0000FFFFu)
        #define RX_CRC_RX_CRC_MASK                           (0x0000FFFFu)
        #define RX_CRC_RX_CRC_BIT                            (0)
        #define RX_CRC_RX_CRC_BITS                           (16)

#define MAC_ACK_TO                                           *((volatile uint32_t *)0x40002074u)
#define MAC_ACK_TO_REG                                       *((volatile uint32_t *)0x40002074u)
#define MAC_ACK_TO_ADDR                                      (0x40002074u)
#define MAC_ACK_TO_RESET                                     (0x00000300u)
        /* ACK_TO field */
        #define MAC_ACK_TO_ACK_TO                            (0x00003FFFu)
        #define MAC_ACK_TO_ACK_TO_MASK                       (0x00003FFFu)
        #define MAC_ACK_TO_ACK_TO_BIT                        (0)
        #define MAC_ACK_TO_ACK_TO_BITS                       (14)

#define MAC_BOP_COMPARE                                      *((volatile uint32_t *)0x40002078u)
#define MAC_BOP_COMPARE_REG                                  *((volatile uint32_t *)0x40002078u)
#define MAC_BOP_COMPARE_ADDR                                 (0x40002078u)
#define MAC_BOP_COMPARE_RESET                                (0x00000014u)
        /* MAC_BOP_COMPARE field */
        #define MAC_BOP_COMPARE_MAC_BOP_COMPARE              (0x0000007Fu)
        #define MAC_BOP_COMPARE_MAC_BOP_COMPARE_MASK         (0x0000007Fu)
        #define MAC_BOP_COMPARE_MAC_BOP_COMPARE_BIT          (0)
        #define MAC_BOP_COMPARE_MAC_BOP_COMPARE_BITS         (7)

#define MAC_TX_ACK_FRAME                                     *((volatile uint32_t *)0x4000207Cu)
#define MAC_TX_ACK_FRAME_REG                                 *((volatile uint32_t *)0x4000207Cu)
#define MAC_TX_ACK_FRAME_ADDR                                (0x4000207Cu)
#define MAC_TX_ACK_FRAME_RESET                               (0x00000002u)
        /* ACK_SRC_AM field */
        #define MAC_TX_ACK_FRAME_ACK_SRC_AM                  (0x0000C000u)
        #define MAC_TX_ACK_FRAME_ACK_SRC_AM_MASK             (0x0000C000u)
        #define MAC_TX_ACK_FRAME_ACK_SRC_AM_BIT              (14)
        #define MAC_TX_ACK_FRAME_ACK_SRC_AM_BITS             (2)
        /* RES1213 field */
        #define MAC_TX_ACK_FRAME_RES1213                     (0x00003000u)
        #define MAC_TX_ACK_FRAME_RES1213_MASK                (0x00003000u)
        #define MAC_TX_ACK_FRAME_RES1213_BIT                 (12)
        #define MAC_TX_ACK_FRAME_RES1213_BITS                (2)
        /* ACK_DST_AM field */
        #define MAC_TX_ACK_FRAME_ACK_DST_AM                  (0x00000C00u)
        #define MAC_TX_ACK_FRAME_ACK_DST_AM_MASK             (0x00000C00u)
        #define MAC_TX_ACK_FRAME_ACK_DST_AM_BIT              (10)
        #define MAC_TX_ACK_FRAME_ACK_DST_AM_BITS             (2)
        /* RES789 field */
        #define MAC_TX_ACK_FRAME_RES789                      (0x00000380u)
        #define MAC_TX_ACK_FRAME_RES789_MASK                 (0x00000380u)
        #define MAC_TX_ACK_FRAME_RES789_BIT                  (7)
        #define MAC_TX_ACK_FRAME_RES789_BITS                 (3)
        /* ACK_IP field */
        #define MAC_TX_ACK_FRAME_ACK_IP                      (0x00000040u)
        #define MAC_TX_ACK_FRAME_ACK_IP_MASK                 (0x00000040u)
        #define MAC_TX_ACK_FRAME_ACK_IP_BIT                  (6)
        #define MAC_TX_ACK_FRAME_ACK_IP_BITS                 (1)
        /* ACK_ACK_REQ field */
        #define MAC_TX_ACK_FRAME_ACK_ACK_REQ                 (0x00000020u)
        #define MAC_TX_ACK_FRAME_ACK_ACK_REQ_MASK            (0x00000020u)
        #define MAC_TX_ACK_FRAME_ACK_ACK_REQ_BIT             (5)
        #define MAC_TX_ACK_FRAME_ACK_ACK_REQ_BITS            (1)
        /* ACK_FRAME_P field */
        #define MAC_TX_ACK_FRAME_ACK_FRAME_P                 (0x00000010u)
        #define MAC_TX_ACK_FRAME_ACK_FRAME_P_MASK            (0x00000010u)
        #define MAC_TX_ACK_FRAME_ACK_FRAME_P_BIT             (4)
        #define MAC_TX_ACK_FRAME_ACK_FRAME_P_BITS            (1)
        /* ACK_SEC_EN field */
        #define MAC_TX_ACK_FRAME_ACK_SEC_EN                  (0x00000008u)
        #define MAC_TX_ACK_FRAME_ACK_SEC_EN_MASK             (0x00000008u)
        #define MAC_TX_ACK_FRAME_ACK_SEC_EN_BIT              (3)
        #define MAC_TX_ACK_FRAME_ACK_SEC_EN_BITS             (1)
        /* ACK_FRAME_T field */
        #define MAC_TX_ACK_FRAME_ACK_FRAME_T                 (0x00000007u)
        #define MAC_TX_ACK_FRAME_ACK_FRAME_T_MASK            (0x00000007u)
        #define MAC_TX_ACK_FRAME_ACK_FRAME_T_BIT             (0)
        #define MAC_TX_ACK_FRAME_ACK_FRAME_T_BITS            (3)

#define MAC_CONFIG                                           *((volatile uint32_t *)0x40002080u)
#define MAC_CONFIG_REG                                       *((volatile uint32_t *)0x40002080u)
#define MAC_CONFIG_ADDR                                      (0x40002080u)
#define MAC_CONFIG_RESET                                     (0x00000000u)
        /* RSSI_INST_EN field */
        #define MAC_CONFIG_RSSI_INST_EN                      (0x00000004u)
        #define MAC_CONFIG_RSSI_INST_EN_MASK                 (0x00000004u)
        #define MAC_CONFIG_RSSI_INST_EN_BIT                  (2)
        #define MAC_CONFIG_RSSI_INST_EN_BITS                 (1)
        /* SPI_SPY_EN field */
        #define MAC_CONFIG_SPI_SPY_EN                        (0x00000002u)
        #define MAC_CONFIG_SPI_SPY_EN_MASK                   (0x00000002u)
        #define MAC_CONFIG_SPI_SPY_EN_BIT                    (1)
        #define MAC_CONFIG_SPI_SPY_EN_BITS                   (1)
        /* MAC_MODE field */
        #define MAC_CONFIG_MAC_MODE                          (0x00000001u)
        #define MAC_CONFIG_MAC_MODE_MASK                     (0x00000001u)
        #define MAC_CONFIG_MAC_MODE_BIT                      (0)
        #define MAC_CONFIG_MAC_MODE_BITS                     (1)

#define MAC_RX_CONFIG                                        *((volatile uint32_t *)0x40002084u)
#define MAC_RX_CONFIG_REG                                    *((volatile uint32_t *)0x40002084u)
#define MAC_RX_CONFIG_ADDR                                   (0x40002084u)
#define MAC_RX_CONFIG_RESET                                  (0x00000000u)
        /* AUTO_ACK field */
        #define MAC_RX_CONFIG_AUTO_ACK                       (0x00000080u)
        #define MAC_RX_CONFIG_AUTO_ACK_MASK                  (0x00000080u)
        #define MAC_RX_CONFIG_AUTO_ACK_BIT                   (7)
        #define MAC_RX_CONFIG_AUTO_ACK_BITS                  (1)
        /* APPEND_INFO field */
        #define MAC_RX_CONFIG_APPEND_INFO                    (0x00000040u)
        #define MAC_RX_CONFIG_APPEND_INFO_MASK               (0x00000040u)
        #define MAC_RX_CONFIG_APPEND_INFO_BIT                (6)
        #define MAC_RX_CONFIG_APPEND_INFO_BITS               (1)
        /* COORDINATOR field */
        #define MAC_RX_CONFIG_COORDINATOR                    (0x00000020u)
        #define MAC_RX_CONFIG_COORDINATOR_MASK               (0x00000020u)
        #define MAC_RX_CONFIG_COORDINATOR_BIT                (5)
        #define MAC_RX_CONFIG_COORDINATOR_BITS               (1)
        /* FILT_ADDR_ON field */
        #define MAC_RX_CONFIG_FILT_ADDR_ON                   (0x00000010u)
        #define MAC_RX_CONFIG_FILT_ADDR_ON_MASK              (0x00000010u)
        #define MAC_RX_CONFIG_FILT_ADDR_ON_BIT               (4)
        #define MAC_RX_CONFIG_FILT_ADDR_ON_BITS              (1)
        /* RES_FILT_PASS_ADDR field */
        #define MAC_RX_CONFIG_RES_FILT_PASS_ADDR             (0x00000008u)
        #define MAC_RX_CONFIG_RES_FILT_PASS_ADDR_MASK        (0x00000008u)
        #define MAC_RX_CONFIG_RES_FILT_PASS_ADDR_BIT         (3)
        #define MAC_RX_CONFIG_RES_FILT_PASS_ADDR_BITS        (1)
        /* RES_FILT_PASS field */
        #define MAC_RX_CONFIG_RES_FILT_PASS                  (0x00000004u)
        #define MAC_RX_CONFIG_RES_FILT_PASS_MASK             (0x00000004u)
        #define MAC_RX_CONFIG_RES_FILT_PASS_BIT              (2)
        #define MAC_RX_CONFIG_RES_FILT_PASS_BITS             (1)
        /* FILT_FORMAT_ON field */
        #define MAC_RX_CONFIG_FILT_FORMAT_ON                 (0x00000002u)
        #define MAC_RX_CONFIG_FILT_FORMAT_ON_MASK            (0x00000002u)
        #define MAC_RX_CONFIG_FILT_FORMAT_ON_BIT             (1)
        #define MAC_RX_CONFIG_FILT_FORMAT_ON_BITS            (1)
        /* MAC_RX_RST field */
        #define MAC_RX_CONFIG_MAC_RX_RST                     (0x00000001u)
        #define MAC_RX_CONFIG_MAC_RX_RST_MASK                (0x00000001u)
        #define MAC_RX_CONFIG_MAC_RX_RST_BIT                 (0)
        #define MAC_RX_CONFIG_MAC_RX_RST_BITS                (1)

#define MAC_TX_CONFIG                                        *((volatile uint32_t *)0x40002088u)
#define MAC_TX_CONFIG_REG                                    *((volatile uint32_t *)0x40002088u)
#define MAC_TX_CONFIG_ADDR                                   (0x40002088u)
#define MAC_TX_CONFIG_RESET                                  (0x00000008u)
        /* SLOTTED field */
        #define MAC_TX_CONFIG_SLOTTED                        (0x00000010u)
        #define MAC_TX_CONFIG_SLOTTED_MASK                   (0x00000010u)
        #define MAC_TX_CONFIG_SLOTTED_BIT                    (4)
        #define MAC_TX_CONFIG_SLOTTED_BITS                   (1)
        /* CCA_DELAY field */
        #define MAC_TX_CONFIG_CCA_DELAY                      (0x00000008u)
        #define MAC_TX_CONFIG_CCA_DELAY_MASK                 (0x00000008u)
        #define MAC_TX_CONFIG_CCA_DELAY_BIT                  (3)
        #define MAC_TX_CONFIG_CCA_DELAY_BITS                 (1)
        /* SLOTTED_ACK field */
        #define MAC_TX_CONFIG_SLOTTED_ACK                    (0x00000004u)
        #define MAC_TX_CONFIG_SLOTTED_ACK_MASK               (0x00000004u)
        #define MAC_TX_CONFIG_SLOTTED_ACK_BIT                (2)
        #define MAC_TX_CONFIG_SLOTTED_ACK_BITS               (1)
        /* INFINITE_CRC field */
        #define MAC_TX_CONFIG_INFINITE_CRC                   (0x00000002u)
        #define MAC_TX_CONFIG_INFINITE_CRC_MASK              (0x00000002u)
        #define MAC_TX_CONFIG_INFINITE_CRC_BIT               (1)
        #define MAC_TX_CONFIG_INFINITE_CRC_BITS              (1)
        /* WAIT_ACK field */
        #define MAC_TX_CONFIG_WAIT_ACK                       (0x00000001u)
        #define MAC_TX_CONFIG_WAIT_ACK_MASK                  (0x00000001u)
        #define MAC_TX_CONFIG_WAIT_ACK_BIT                   (0)
        #define MAC_TX_CONFIG_WAIT_ACK_BITS                  (1)

#define MAC_TIMER_CTRL                                       *((volatile uint32_t *)0x4000208Cu)
#define MAC_TIMER_CTRL_REG                                   *((volatile uint32_t *)0x4000208Cu)
#define MAC_TIMER_CTRL_ADDR                                  (0x4000208Cu)
#define MAC_TIMER_CTRL_RESET                                 (0x00000000u)
        /* COMP_A_SYNC field */
        #define MAC_TIMER_CTRL_COMP_A_SYNC                   (0x00000040u)
        #define MAC_TIMER_CTRL_COMP_A_SYNC_MASK              (0x00000040u)
        #define MAC_TIMER_CTRL_COMP_A_SYNC_BIT               (6)
        #define MAC_TIMER_CTRL_COMP_A_SYNC_BITS              (1)
        /* BOP_TIMER_RST field */
        #define MAC_TIMER_CTRL_BOP_TIMER_RST                 (0x00000020u)
        #define MAC_TIMER_CTRL_BOP_TIMER_RST_MASK            (0x00000020u)
        #define MAC_TIMER_CTRL_BOP_TIMER_RST_BIT             (5)
        #define MAC_TIMER_CTRL_BOP_TIMER_RST_BITS            (1)
        /* BOP_TIMER_EN field */
        #define MAC_TIMER_CTRL_BOP_TIMER_EN                  (0x00000010u)
        #define MAC_TIMER_CTRL_BOP_TIMER_EN_MASK             (0x00000010u)
        #define MAC_TIMER_CTRL_BOP_TIMER_EN_BIT              (4)
        #define MAC_TIMER_CTRL_BOP_TIMER_EN_BITS             (1)
        /* BO_TIMER_RST field */
        #define MAC_TIMER_CTRL_BO_TIMER_RST                  (0x00000008u)
        #define MAC_TIMER_CTRL_BO_TIMER_RST_MASK             (0x00000008u)
        #define MAC_TIMER_CTRL_BO_TIMER_RST_BIT              (3)
        #define MAC_TIMER_CTRL_BO_TIMER_RST_BITS             (1)
        /* BO_TIMER_EN field */
        #define MAC_TIMER_CTRL_BO_TIMER_EN                   (0x00000004u)
        #define MAC_TIMER_CTRL_BO_TIMER_EN_MASK              (0x00000004u)
        #define MAC_TIMER_CTRL_BO_TIMER_EN_BIT               (2)
        #define MAC_TIMER_CTRL_BO_TIMER_EN_BITS              (1)
        /* MAC_TIMER_RST field */
        #define MAC_TIMER_CTRL_MAC_TIMER_RST                 (0x00000002u)
        #define MAC_TIMER_CTRL_MAC_TIMER_RST_MASK            (0x00000002u)
        #define MAC_TIMER_CTRL_MAC_TIMER_RST_BIT             (1)
        #define MAC_TIMER_CTRL_MAC_TIMER_RST_BITS            (1)
        /* MAC_TIMER_EN field */
        #define MAC_TIMER_CTRL_MAC_TIMER_EN                  (0x00000001u)
        #define MAC_TIMER_CTRL_MAC_TIMER_EN_MASK             (0x00000001u)
        #define MAC_TIMER_CTRL_MAC_TIMER_EN_BIT              (0)
        #define MAC_TIMER_CTRL_MAC_TIMER_EN_BITS             (1)

#define PAN_ID                                               *((volatile uint32_t *)0x40002090u)
#define PAN_ID_REG                                           *((volatile uint32_t *)0x40002090u)
#define PAN_ID_ADDR                                          (0x40002090u)
#define PAN_ID_RESET                                         (0x00000000u)
        /* PAN_ID field */
        #define PAN_ID_PAN_ID                                (0x0000FFFFu)
        #define PAN_ID_PAN_ID_MASK                           (0x0000FFFFu)
        #define PAN_ID_PAN_ID_BIT                            (0)
        #define PAN_ID_PAN_ID_BITS                           (16)

#define SHORT_ADDR                                           *((volatile uint32_t *)0x40002094u)
#define SHORT_ADDR_REG                                       *((volatile uint32_t *)0x40002094u)
#define SHORT_ADDR_ADDR                                      (0x40002094u)
#define SHORT_ADDR_RESET                                     (0x0000FFFEu)
        /* SHORT_ADDR field */
        #define SHORT_ADDR_SHORT_ADDR                        (0x0000FFFFu)
        #define SHORT_ADDR_SHORT_ADDR_MASK                   (0x0000FFFFu)
        #define SHORT_ADDR_SHORT_ADDR_BIT                    (0)
        #define SHORT_ADDR_SHORT_ADDR_BITS                   (16)

#define EXT_ADDR_0                                           *((volatile uint32_t *)0x40002098u)
#define EXT_ADDR_0_REG                                       *((volatile uint32_t *)0x40002098u)
#define EXT_ADDR_0_ADDR                                      (0x40002098u)
#define EXT_ADDR_0_RESET                                     (0x00000000u)
        /* EXT_ADDR_0 field */
        #define EXT_ADDR_0_EXT_ADDR_0                        (0x0000FFFFu)
        #define EXT_ADDR_0_EXT_ADDR_0_MASK                   (0x0000FFFFu)
        #define EXT_ADDR_0_EXT_ADDR_0_BIT                    (0)
        #define EXT_ADDR_0_EXT_ADDR_0_BITS                   (16)

#define EXT_ADDR_1                                           *((volatile uint32_t *)0x4000209Cu)
#define EXT_ADDR_1_REG                                       *((volatile uint32_t *)0x4000209Cu)
#define EXT_ADDR_1_ADDR                                      (0x4000209Cu)
#define EXT_ADDR_1_RESET                                     (0x00000000u)
        /* EXT_ADDR_1 field */
        #define EXT_ADDR_1_EXT_ADDR_1                        (0x0000FFFFu)
        #define EXT_ADDR_1_EXT_ADDR_1_MASK                   (0x0000FFFFu)
        #define EXT_ADDR_1_EXT_ADDR_1_BIT                    (0)
        #define EXT_ADDR_1_EXT_ADDR_1_BITS                   (16)

#define EXT_ADDR_2                                           *((volatile uint32_t *)0x400020A0u)
#define EXT_ADDR_2_REG                                       *((volatile uint32_t *)0x400020A0u)
#define EXT_ADDR_2_ADDR                                      (0x400020A0u)
#define EXT_ADDR_2_RESET                                     (0x00000000u)
        /* EXT_ADDR_2 field */
        #define EXT_ADDR_2_EXT_ADDR_2                        (0x0000FFFFu)
        #define EXT_ADDR_2_EXT_ADDR_2_MASK                   (0x0000FFFFu)
        #define EXT_ADDR_2_EXT_ADDR_2_BIT                    (0)
        #define EXT_ADDR_2_EXT_ADDR_2_BITS                   (16)

#define EXT_ADDR_3                                           *((volatile uint32_t *)0x400020A4u)
#define EXT_ADDR_3_REG                                       *((volatile uint32_t *)0x400020A4u)
#define EXT_ADDR_3_ADDR                                      (0x400020A4u)
#define EXT_ADDR_3_RESET                                     (0x00000000u)
        /* EXT_ADDR_3 field */
        #define EXT_ADDR_3_EXT_ADDR_3                        (0x0000FFFFu)
        #define EXT_ADDR_3_EXT_ADDR_3_MASK                   (0x0000FFFFu)
        #define EXT_ADDR_3_EXT_ADDR_3_BIT                    (0)
        #define EXT_ADDR_3_EXT_ADDR_3_BITS                   (16)

#define MAC_STATE                                            *((volatile uint32_t *)0x400020A8u)
#define MAC_STATE_REG                                        *((volatile uint32_t *)0x400020A8u)
#define MAC_STATE_ADDR                                       (0x400020A8u)
#define MAC_STATE_RESET                                      (0x00000000u)
        /* SPY_STATE field */
        #define MAC_STATE_SPY_STATE                          (0x00000700u)
        #define MAC_STATE_SPY_STATE_MASK                     (0x00000700u)
        #define MAC_STATE_SPY_STATE_BIT                      (8)
        #define MAC_STATE_SPY_STATE_BITS                     (3)
        /* ACK_STATE field */
        #define MAC_STATE_ACK_STATE                          (0x000000C0u)
        #define MAC_STATE_ACK_STATE_MASK                     (0x000000C0u)
        #define MAC_STATE_ACK_STATE_BIT                      (6)
        #define MAC_STATE_ACK_STATE_BITS                     (2)
        /* BO_STATE field */
        #define MAC_STATE_BO_STATE                           (0x0000003Cu)
        #define MAC_STATE_BO_STATE_MASK                      (0x0000003Cu)
        #define MAC_STATE_BO_STATE_BIT                       (2)
        #define MAC_STATE_BO_STATE_BITS                      (4)
        /* TOP_STATE field */
        #define MAC_STATE_TOP_STATE                          (0x00000003u)
        #define MAC_STATE_TOP_STATE_MASK                     (0x00000003u)
        #define MAC_STATE_TOP_STATE_BIT                      (0)
        #define MAC_STATE_TOP_STATE_BITS                     (2)

#define RX_STATE                                             *((volatile uint32_t *)0x400020ACu)
#define RX_STATE_REG                                         *((volatile uint32_t *)0x400020ACu)
#define RX_STATE_ADDR                                        (0x400020ACu)
#define RX_STATE_RESET                                       (0x00000000u)
        /* RX_BUFFER_STATE field */
        #define RX_STATE_RX_BUFFER_STATE                     (0x000001E0u)
        #define RX_STATE_RX_BUFFER_STATE_MASK                (0x000001E0u)
        #define RX_STATE_RX_BUFFER_STATE_BIT                 (5)
        #define RX_STATE_RX_BUFFER_STATE_BITS                (4)
        /* RX_TOP_STATE field */
        #define RX_STATE_RX_TOP_STATE                        (0x0000001Fu)
        #define RX_STATE_RX_TOP_STATE_MASK                   (0x0000001Fu)
        #define RX_STATE_RX_TOP_STATE_BIT                    (0)
        #define RX_STATE_RX_TOP_STATE_BITS                   (5)

#define TX_STATE                                             *((volatile uint32_t *)0x400020B0u)
#define TX_STATE_REG                                         *((volatile uint32_t *)0x400020B0u)
#define TX_STATE_ADDR                                        (0x400020B0u)
#define TX_STATE_RESET                                       (0x00000000u)
        /* TX_BUFFER_STATE field */
        #define TX_STATE_TX_BUFFER_STATE                     (0x000000F0u)
        #define TX_STATE_TX_BUFFER_STATE_MASK                (0x000000F0u)
        #define TX_STATE_TX_BUFFER_STATE_BIT                 (4)
        #define TX_STATE_TX_BUFFER_STATE_BITS                (4)
        /* TX_TOP_STATE field */
        #define TX_STATE_TX_TOP_STATE                        (0x0000000Fu)
        #define TX_STATE_TX_TOP_STATE_MASK                   (0x0000000Fu)
        #define TX_STATE_TX_TOP_STATE_BIT                    (0)
        #define TX_STATE_TX_TOP_STATE_BITS                   (4)

#define DMA_STATE                                            *((volatile uint32_t *)0x400020B4u)
#define DMA_STATE_REG                                        *((volatile uint32_t *)0x400020B4u)
#define DMA_STATE_ADDR                                       (0x400020B4u)
#define DMA_STATE_RESET                                      (0x00000000u)
        /* DMA_RX_STATE field */
        #define DMA_STATE_DMA_RX_STATE                       (0x00000038u)
        #define DMA_STATE_DMA_RX_STATE_MASK                  (0x00000038u)
        #define DMA_STATE_DMA_RX_STATE_BIT                   (3)
        #define DMA_STATE_DMA_RX_STATE_BITS                  (3)
        /* DMA_TX_STATE field */
        #define DMA_STATE_DMA_TX_STATE                       (0x00000007u)
        #define DMA_STATE_DMA_TX_STATE_MASK                  (0x00000007u)
        #define DMA_STATE_DMA_TX_STATE_BIT                   (0)
        #define DMA_STATE_DMA_TX_STATE_BITS                  (3)

#define MAC_DEBUG                                            *((volatile uint32_t *)0x400020B8u)
#define MAC_DEBUG_REG                                        *((volatile uint32_t *)0x400020B8u)
#define MAC_DEBUG_ADDR                                       (0x400020B8u)
#define MAC_DEBUG_RESET                                      (0x00000000u)
        /* SW_DEBUG_OUT field */
        #define MAC_DEBUG_SW_DEBUG_OUT                       (0x00000060u)
        #define MAC_DEBUG_SW_DEBUG_OUT_MASK                  (0x00000060u)
        #define MAC_DEBUG_SW_DEBUG_OUT_BIT                   (5)
        #define MAC_DEBUG_SW_DEBUG_OUT_BITS                  (2)
        /* MAC_DEBUG_MUX field */
        #define MAC_DEBUG_MAC_DEBUG_MUX                      (0x0000001Fu)
        #define MAC_DEBUG_MAC_DEBUG_MUX_MASK                 (0x0000001Fu)
        #define MAC_DEBUG_MAC_DEBUG_MUX_BIT                  (0)
        #define MAC_DEBUG_MAC_DEBUG_MUX_BITS                 (5)

#define MAC_DEBUG_VIEW                                       *((volatile uint32_t *)0x400020BCu)
#define MAC_DEBUG_VIEW_REG                                   *((volatile uint32_t *)0x400020BCu)
#define MAC_DEBUG_VIEW_ADDR                                  (0x400020BCu)
#define MAC_DEBUG_VIEW_RESET                                 (0x00000010u)
        /* MAC_DEBUG_VIEW field */
        #define MAC_DEBUG_VIEW_MAC_DEBUG_VIEW                (0x0000FFFFu)
        #define MAC_DEBUG_VIEW_MAC_DEBUG_VIEW_MASK           (0x0000FFFFu)
        #define MAC_DEBUG_VIEW_MAC_DEBUG_VIEW_BIT            (0)
        #define MAC_DEBUG_VIEW_MAC_DEBUG_VIEW_BITS           (16)

#define MAC_RSSI_DELAY                                       *((volatile uint32_t *)0x400020C0u)
#define MAC_RSSI_DELAY_REG                                   *((volatile uint32_t *)0x400020C0u)
#define MAC_RSSI_DELAY_ADDR                                  (0x400020C0u)
#define MAC_RSSI_DELAY_RESET                                 (0x00000000u)
        /* RSSI_INST_DELAY_OK field */
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY_OK            (0x00000FC0u)
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY_OK_MASK       (0x00000FC0u)
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY_OK_BIT        (6)
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY_OK_BITS       (6)
        /* RSSI_INST_DELAY field */
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY               (0x0000003Fu)
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY_MASK          (0x0000003Fu)
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY_BIT           (0)
        #define MAC_RSSI_DELAY_RSSI_INST_DELAY_BITS          (6)

#define PANID_COUNT                                          *((volatile uint32_t *)0x400020C4u)
#define PANID_COUNT_REG                                      *((volatile uint32_t *)0x400020C4u)
#define PANID_COUNT_ADDR                                     (0x400020C4u)
#define PANID_COUNT_RESET                                    (0x00000000u)
        /* PANID_COUNT field */
        #define PANID_COUNT_PANID_COUNT                      (0x0000FFFFu)
        #define PANID_COUNT_PANID_COUNT_MASK                 (0x0000FFFFu)
        #define PANID_COUNT_PANID_COUNT_BIT                  (0)
        #define PANID_COUNT_PANID_COUNT_BITS                 (16)

#define NONPAN_COUNT                                         *((volatile uint32_t *)0x400020C8u)
#define NONPAN_COUNT_REG                                     *((volatile uint32_t *)0x400020C8u)
#define NONPAN_COUNT_ADDR                                    (0x400020C8u)
#define NONPAN_COUNT_RESET                                   (0x00000000u)
        /* NONPAN_COUNT field */
        #define NONPAN_COUNT_NONPAN_COUNT                    (0x0000FFFFu)
        #define NONPAN_COUNT_NONPAN_COUNT_MASK               (0x0000FFFFu)
        #define NONPAN_COUNT_NONPAN_COUNT_BIT                (0)
        #define NONPAN_COUNT_NONPAN_COUNT_BITS               (16)

/* SECURITY block */
#define BLOCK_SECURITY_BASE                                  (0x40003000u)
#define BLOCK_SECURITY_END                                   (0x40003044u)
#define BLOCK_SECURITY_SIZE                                  (BLOCK_SECURITY_END - BLOCK_SECURITY_BASE + 1)

#define SECURITY_CONFIG                                      *((volatile uint32_t *)0x40003000u)
#define SECURITY_CONFIG_REG                                  *((volatile uint32_t *)0x40003000u)
#define SECURITY_CONFIG_ADDR                                 (0x40003000u)
#define SECURITY_CONFIG_RESET                                (0x00000000u)
        /* SEC_RST field */
        #define SECURITY_CONFIG_SEC_RST                      (0x00000080u)
        #define SECURITY_CONFIG_SEC_RST_MASK                 (0x00000080u)
        #define SECURITY_CONFIG_SEC_RST_BIT                  (7)
        #define SECURITY_CONFIG_SEC_RST_BITS                 (1)
        /* CTR_IN field */
        #define SECURITY_CONFIG_CTR_IN                       (0x00000040u)
        #define SECURITY_CONFIG_CTR_IN_MASK                  (0x00000040u)
        #define SECURITY_CONFIG_CTR_IN_BIT                   (6)
        #define SECURITY_CONFIG_CTR_IN_BITS                  (1)
        /* MIC_XOR_CT field */
        #define SECURITY_CONFIG_MIC_XOR_CT                   (0x00000020u)
        #define SECURITY_CONFIG_MIC_XOR_CT_MASK              (0x00000020u)
        #define SECURITY_CONFIG_MIC_XOR_CT_BIT               (5)
        #define SECURITY_CONFIG_MIC_XOR_CT_BITS              (1)
        /* CBC_XOR_PT field */
        #define SECURITY_CONFIG_CBC_XOR_PT                   (0x00000010u)
        #define SECURITY_CONFIG_CBC_XOR_PT_MASK              (0x00000010u)
        #define SECURITY_CONFIG_CBC_XOR_PT_BIT               (4)
        #define SECURITY_CONFIG_CBC_XOR_PT_BITS              (1)
        /* CT_TO_CBC_ST field */
        #define SECURITY_CONFIG_CT_TO_CBC_ST                 (0x00000008u)
        #define SECURITY_CONFIG_CT_TO_CBC_ST_MASK            (0x00000008u)
        #define SECURITY_CONFIG_CT_TO_CBC_ST_BIT             (3)
        #define SECURITY_CONFIG_CT_TO_CBC_ST_BITS            (1)
        /* WAIT_CT_READ field */
        #define SECURITY_CONFIG_WAIT_CT_READ                 (0x00000004u)
        #define SECURITY_CONFIG_WAIT_CT_READ_MASK            (0x00000004u)
        #define SECURITY_CONFIG_WAIT_CT_READ_BIT             (2)
        #define SECURITY_CONFIG_WAIT_CT_READ_BITS            (1)
        /* WAIT_PT_WRITE field */
        #define SECURITY_CONFIG_WAIT_PT_WRITE                (0x00000002u)
        #define SECURITY_CONFIG_WAIT_PT_WRITE_MASK           (0x00000002u)
        #define SECURITY_CONFIG_WAIT_PT_WRITE_BIT            (1)
        #define SECURITY_CONFIG_WAIT_PT_WRITE_BITS           (1)
        /* START_AES field */
        #define SECURITY_CONFIG_START_AES                    (0x00000001u)
        #define SECURITY_CONFIG_START_AES_MASK               (0x00000001u)
        #define SECURITY_CONFIG_START_AES_BIT                (0)
        #define SECURITY_CONFIG_START_AES_BITS               (1)

#define SECURITY_STATUS                                      *((volatile uint32_t *)0x40003004u)
#define SECURITY_STATUS_REG                                  *((volatile uint32_t *)0x40003004u)
#define SECURITY_STATUS_ADDR                                 (0x40003004u)
#define SECURITY_STATUS_RESET                                (0x00000000u)
        /* SEC_BUSY field */
        #define SECURITY_STATUS_SEC_BUSY                     (0x00000001u)
        #define SECURITY_STATUS_SEC_BUSY_MASK                (0x00000001u)
        #define SECURITY_STATUS_SEC_BUSY_BIT                 (0)
        #define SECURITY_STATUS_SEC_BUSY_BITS                (1)

#define CBC_STATE_0                                          *((volatile uint32_t *)0x40003008u)
#define CBC_STATE_0_REG                                      *((volatile uint32_t *)0x40003008u)
#define CBC_STATE_0_ADDR                                     (0x40003008u)
#define CBC_STATE_0_RESET                                    (0x00000000u)
        /* CBC_STATE field */
        #define CBC_STATE_0_CBC_STATE                        (0xFFFFFFFFu)
        #define CBC_STATE_0_CBC_STATE_MASK                   (0xFFFFFFFFu)
        #define CBC_STATE_0_CBC_STATE_BIT                    (0)
        #define CBC_STATE_0_CBC_STATE_BITS                   (32)

#define CBC_STATE_1                                          *((volatile uint32_t *)0x4000300Cu)
#define CBC_STATE_1_REG                                      *((volatile uint32_t *)0x4000300Cu)
#define CBC_STATE_1_ADDR                                     (0x4000300Cu)
#define CBC_STATE_1_RESET                                    (0x00000000u)
        /* CBC_STATE_1 field */
        #define CBC_STATE_1_CBC_STATE_1                      (0xFFFFFFFFu)
        #define CBC_STATE_1_CBC_STATE_1_MASK                 (0xFFFFFFFFu)
        #define CBC_STATE_1_CBC_STATE_1_BIT                  (0)
        #define CBC_STATE_1_CBC_STATE_1_BITS                 (32)

#define CBC_STATE_2                                          *((volatile uint32_t *)0x40003010u)
#define CBC_STATE_2_REG                                      *((volatile uint32_t *)0x40003010u)
#define CBC_STATE_2_ADDR                                     (0x40003010u)
#define CBC_STATE_2_RESET                                    (0x00000000u)
        /* CBC_STATE_2 field */
        #define CBC_STATE_2_CBC_STATE_2                      (0xFFFFFFFFu)
        #define CBC_STATE_2_CBC_STATE_2_MASK                 (0xFFFFFFFFu)
        #define CBC_STATE_2_CBC_STATE_2_BIT                  (0)
        #define CBC_STATE_2_CBC_STATE_2_BITS                 (32)

#define CBC_STATE_3                                          *((volatile uint32_t *)0x40003014u)
#define CBC_STATE_3_REG                                      *((volatile uint32_t *)0x40003014u)
#define CBC_STATE_3_ADDR                                     (0x40003014u)
#define CBC_STATE_3_RESET                                    (0x00000000u)
        /* CBC_STATE_3 field */
        #define CBC_STATE_3_CBC_STATE_3                      (0xFFFFFFFFu)
        #define CBC_STATE_3_CBC_STATE_3_MASK                 (0xFFFFFFFFu)
        #define CBC_STATE_3_CBC_STATE_3_BIT                  (0)
        #define CBC_STATE_3_CBC_STATE_3_BITS                 (32)

#define PT                                                   *((volatile uint32_t *)0x40003028u)
#define PT_REG                                               *((volatile uint32_t *)0x40003028u)
#define PT_ADDR                                              (0x40003028u)
#define PT_RESET                                             (0x00000000u)
        /* PT field */
        #define PT_PT                                        (0xFFFFFFFFu)
        #define PT_PT_MASK                                   (0xFFFFFFFFu)
        #define PT_PT_BIT                                    (0)
        #define PT_PT_BITS                                   (32)

#define CT                                                   *((volatile uint32_t *)0x40003030u)
#define CT_REG                                               *((volatile uint32_t *)0x40003030u)
#define CT_ADDR                                              (0x40003030u)
#define CT_RESET                                             (0x00000000u)
        /* CT field */
        #define CT_CT                                        (0xFFFFFFFFu)
        #define CT_CT_MASK                                   (0xFFFFFFFFu)
        #define CT_CT_BIT                                    (0)
        #define CT_CT_BITS                                   (32)

#define KEY_0                                                *((volatile uint32_t *)0x40003038u)
#define KEY_0_REG                                            *((volatile uint32_t *)0x40003038u)
#define KEY_0_ADDR                                           (0x40003038u)
#define KEY_0_RESET                                          (0x00000000u)
        /* KEY_O field */
        #define KEY_0_KEY_O                                  (0xFFFFFFFFu)
        #define KEY_0_KEY_O_MASK                             (0xFFFFFFFFu)
        #define KEY_0_KEY_O_BIT                              (0)
        #define KEY_0_KEY_O_BITS                             (32)

#define KEY_1                                                *((volatile uint32_t *)0x4000303Cu)
#define KEY_1_REG                                            *((volatile uint32_t *)0x4000303Cu)
#define KEY_1_ADDR                                           (0x4000303Cu)
#define KEY_1_RESET                                          (0x00000000u)
        /* KEY_1 field */
        #define KEY_1_KEY_1                                  (0xFFFFFFFFu)
        #define KEY_1_KEY_1_MASK                             (0xFFFFFFFFu)
        #define KEY_1_KEY_1_BIT                              (0)
        #define KEY_1_KEY_1_BITS                             (32)

#define KEY_2                                                *((volatile uint32_t *)0x40003040u)
#define KEY_2_REG                                            *((volatile uint32_t *)0x40003040u)
#define KEY_2_ADDR                                           (0x40003040u)
#define KEY_2_RESET                                          (0x00000000u)
        /* KEY_2 field */
        #define KEY_2_KEY_2                                  (0xFFFFFFFFu)
        #define KEY_2_KEY_2_MASK                             (0xFFFFFFFFu)
        #define KEY_2_KEY_2_BIT                              (0)
        #define KEY_2_KEY_2_BITS                             (32)

#define KEY_3                                                *((volatile uint32_t *)0x40003044u)
#define KEY_3_REG                                            *((volatile uint32_t *)0x40003044u)
#define KEY_3_ADDR                                           (0x40003044u)
#define KEY_3_RESET                                          (0x00000000u)
        /* KEY_3 field */
        #define KEY_3_KEY_3                                  (0xFFFFFFFFu)
        #define KEY_3_KEY_3_MASK                             (0xFFFFFFFFu)
        #define KEY_3_KEY_3_BIT                              (0)
        #define KEY_3_KEY_3_BITS                             (32)

/* CM_LV block */
#define BLOCK_CM_LV_BASE                                     (0x40004000u)
#define BLOCK_CM_LV_END                                      (0x4000403Cu)
#define BLOCK_CM_LV_SIZE                                     (BLOCK_CM_LV_END - BLOCK_CM_LV_BASE + 1)

#define SILICON_ID                                           *((volatile uint32_t *)0x40004000u)
#define SILICON_ID_REG                                       *((volatile uint32_t *)0x40004000u)
#define SILICON_ID_ADDR                                      (0x40004000u)
#define SILICON_ID_RESET                                     (0x069AA62Bu)
        /* HW_VERSION field */
        #define SILICON_ID_HW_VERSION                        (0xF0000000u)
        #define SILICON_ID_HW_VERSION_MASK                   (0xF0000000u)
        #define SILICON_ID_HW_VERSION_BIT                    (28)
        #define SILICON_ID_HW_VERSION_BITS                   (4)
        /* ST_DIVISION field */
        #define SILICON_ID_ST_DIVISION                       (0x0F000000u)
        #define SILICON_ID_ST_DIVISION_MASK                  (0x0F000000u)
        #define SILICON_ID_ST_DIVISION_BIT                   (24)
        #define SILICON_ID_ST_DIVISION_BITS                  (4)
        /* CHIP_TYPE field */
        #define SILICON_ID_CHIP_TYPE                         (0x00FF8000u)
        #define SILICON_ID_CHIP_TYPE_MASK                    (0x00FF8000u)
        #define SILICON_ID_CHIP_TYPE_BIT                     (15)
        #define SILICON_ID_CHIP_TYPE_BITS                    (9)
        /* SUB_TYPE field */
        #define SILICON_ID_SUB_TYPE                          (0x00007000u)
        #define SILICON_ID_SUB_TYPE_MASK                     (0x00007000u)
        #define SILICON_ID_SUB_TYPE_BIT                      (12)
        #define SILICON_ID_SUB_TYPE_BITS                     (3)
        /* JEDEC_MAN_ID field */
        #define SILICON_ID_JEDEC_MAN_ID                      (0x00000FFEu)
        #define SILICON_ID_JEDEC_MAN_ID_MASK                 (0x00000FFEu)
        #define SILICON_ID_JEDEC_MAN_ID_BIT                  (1)
        #define SILICON_ID_JEDEC_MAN_ID_BITS                 (11)
        /* ONE field */
        #define SILICON_ID_ONE                               (0x00000001u)
        #define SILICON_ID_ONE_MASK                          (0x00000001u)
        #define SILICON_ID_ONE_BIT                           (0)
        #define SILICON_ID_ONE_BITS                          (1)

#define OSC24M_BIASTRIM                                      *((volatile uint32_t *)0x40004004u)
#define OSC24M_BIASTRIM_REG                                  *((volatile uint32_t *)0x40004004u)
#define OSC24M_BIASTRIM_ADDR                                 (0x40004004u)
#define OSC24M_BIASTRIM_RESET                                (0x0000000Fu)
        /* OSC24M_BIAS_TRIM field */
        #define OSC24M_BIASTRIM_OSC24M_BIAS_TRIM             (0x0000000Fu)
        #define OSC24M_BIASTRIM_OSC24M_BIAS_TRIM_MASK        (0x0000000Fu)
        #define OSC24M_BIASTRIM_OSC24M_BIAS_TRIM_BIT         (0)
        #define OSC24M_BIASTRIM_OSC24M_BIAS_TRIM_BITS        (4)

#define OSCHF_TUNE                                           *((volatile uint32_t *)0x40004008u)
#define OSCHF_TUNE_REG                                       *((volatile uint32_t *)0x40004008u)
#define OSCHF_TUNE_ADDR                                      (0x40004008u)
#define OSCHF_TUNE_RESET                                     (0x00000017u)
        /* OSCHF_TUNE_FIELD field */
        #define OSCHF_TUNE_FIELD                             (0x0000001Fu)
        #define OSCHF_TUNE_FIELD_MASK                        (0x0000001Fu)
        #define OSCHF_TUNE_FIELD_BIT                         (0)
        #define OSCHF_TUNE_FIELD_BITS                        (5)

#define OSC24M_COMP                                          *((volatile uint32_t *)0x4000400Cu)
#define OSC24M_COMP_REG                                      *((volatile uint32_t *)0x4000400Cu)
#define OSC24M_COMP_ADDR                                     (0x4000400Cu)
#define OSC24M_COMP_RESET                                    (0x00000000u)
        /* OSC24M_HI field */
        #define OSC24M_HI                                    (0x00000002u)
        #define OSC24M_HI_MASK                               (0x00000002u)
        #define OSC24M_HI_BIT                                (1)
        #define OSC24M_HI_BITS                               (1)
        /* OSC24M_LO field */
        #define OSC24M_LO                                    (0x00000001u)
        #define OSC24M_LO_MASK                               (0x00000001u)
        #define OSC24M_LO_BIT                                (0)
        #define OSC24M_LO_BITS                               (1)

#define CLK_PERIODMODE                                       *((volatile uint32_t *)0x40004010u)
#define CLK_PERIODMODE_REG                                   *((volatile uint32_t *)0x40004010u)
#define CLK_PERIODMODE_ADDR                                  (0x40004010u)
#define CLK_PERIODMODE_RESET                                 (0x00000000u)
        /* CLK_PERIODMODE_FIELD field */
        #define CLK_PERIODMODE_FIELD                         (0x00000003u)
        #define CLK_PERIODMODE_FIELD_MASK                    (0x00000003u)
        #define CLK_PERIODMODE_FIELD_BIT                     (0)
        #define CLK_PERIODMODE_FIELD_BITS                    (2)

#define CLK_PERIOD                                           *((volatile uint32_t *)0x40004014u)
#define CLK_PERIOD_REG                                       *((volatile uint32_t *)0x40004014u)
#define CLK_PERIOD_ADDR                                      (0x40004014u)
#define CLK_PERIOD_RESET                                     (0x00000000u)
        /* CLK_PERIOD_FIELD field */
        #define CLK_PERIOD_FIELD                             (0x0000FFFFu)
        #define CLK_PERIOD_FIELD_MASK                        (0x0000FFFFu)
        #define CLK_PERIOD_FIELD_BIT                         (0)
        #define CLK_PERIOD_FIELD_BITS                        (16)

#define DITHER_DIS                                           *((volatile uint32_t *)0x40004018u)
#define DITHER_DIS_REG                                       *((volatile uint32_t *)0x40004018u)
#define DITHER_DIS_ADDR                                      (0x40004018u)
#define DITHER_DIS_RESET                                     (0x00000000u)
        /* DITHER_DIS field */
        #define DITHER_DIS_DITHER_DIS                        (0x00000001u)
        #define DITHER_DIS_DITHER_DIS_MASK                   (0x00000001u)
        #define DITHER_DIS_DITHER_DIS_BIT                    (0)
        #define DITHER_DIS_DITHER_DIS_BITS                   (1)

#define OSC24M_CTRL                                          *((volatile uint32_t *)0x4000401Cu)
#define OSC24M_CTRL_REG                                      *((volatile uint32_t *)0x4000401Cu)
#define OSC24M_CTRL_ADDR                                     (0x4000401Cu)
#define OSC24M_CTRL_RESET                                    (0x00000000u)
        /* OSC24M_EN field */
        #define OSC24M_CTRL_OSC24M_EN                        (0x00000002u)
        #define OSC24M_CTRL_OSC24M_EN_MASK                   (0x00000002u)
        #define OSC24M_CTRL_OSC24M_EN_BIT                    (1)
        #define OSC24M_CTRL_OSC24M_EN_BITS                   (1)
        /* OSC24M_SEL field */
        #define OSC24M_CTRL_OSC24M_SEL                       (0x00000001u)
        #define OSC24M_CTRL_OSC24M_SEL_MASK                  (0x00000001u)
        #define OSC24M_CTRL_OSC24M_SEL_BIT                   (0)
        #define OSC24M_CTRL_OSC24M_SEL_BITS                  (1)

#define BUS_FAULT                                            *((volatile uint32_t *)0x40004024u)
#define BUS_FAULT_REG                                        *((volatile uint32_t *)0x40004024u)
#define BUS_FAULT_ADDR                                       (0x40004024u)
#define BUS_FAULT_RESET                                      (0x00000000u)
        /* WRONGSIZE field */
        #define BUS_FAULT_WRONGSIZE                          (0x00000008u)
        #define BUS_FAULT_WRONGSIZE_MASK                     (0x00000008u)
        #define BUS_FAULT_WRONGSIZE_BIT                      (3)
        #define BUS_FAULT_WRONGSIZE_BITS                     (1)
        /* PROTECTED field */
        #define BUS_FAULT_PROTECTED                          (0x00000004u)
        #define BUS_FAULT_PROTECTED_MASK                     (0x00000004u)
        #define BUS_FAULT_PROTECTED_BIT                      (2)
        #define BUS_FAULT_PROTECTED_BITS                     (1)
        /* RESERVED field */
        #define BUS_FAULT_RESERVED                           (0x00000002u)
        #define BUS_FAULT_RESERVED_MASK                      (0x00000002u)
        #define BUS_FAULT_RESERVED_BIT                       (1)
        #define BUS_FAULT_RESERVED_BITS                      (1)
        /* MISSED field */
        #define BUS_FAULT_MISSED                             (0x00000001u)
        #define BUS_FAULT_MISSED_MASK                        (0x00000001u)
        #define BUS_FAULT_MISSED_BIT                         (0)
        #define BUS_FAULT_MISSED_BITS                        (1)

#define PCTRACE_SEL                                          *((volatile uint32_t *)0x40004028u)
#define PCTRACE_SEL_REG                                      *((volatile uint32_t *)0x40004028u)
#define PCTRACE_SEL_ADDR                                     (0x40004028u)
#define PCTRACE_SEL_RESET                                    (0x00000000u)
        /* PCTRACE_SEL_FIELD field */
        #define PCTRACE_SEL_FIELD                            (0x00000001u)
        #define PCTRACE_SEL_FIELD_MASK                       (0x00000001u)
        #define PCTRACE_SEL_FIELD_BIT                        (0)
        #define PCTRACE_SEL_FIELD_BITS                       (1)

#define FPEC_CLKREQ                                          *((volatile uint32_t *)0x4000402Cu)
#define FPEC_CLKREQ_REG                                      *((volatile uint32_t *)0x4000402Cu)
#define FPEC_CLKREQ_ADDR                                     (0x4000402Cu)
#define FPEC_CLKREQ_RESET                                    (0x00000000u)
        /* FPEC_CLKREQ_FIELD field */
        #define FPEC_CLKREQ_FIELD                            (0x00000001u)
        #define FPEC_CLKREQ_FIELD_MASK                       (0x00000001u)
        #define FPEC_CLKREQ_FIELD_BIT                        (0)
        #define FPEC_CLKREQ_FIELD_BITS                       (1)

#define FPEC_CLKSTAT                                         *((volatile uint32_t *)0x40004030u)
#define FPEC_CLKSTAT_REG                                     *((volatile uint32_t *)0x40004030u)
#define FPEC_CLKSTAT_ADDR                                    (0x40004030u)
#define FPEC_CLKSTAT_RESET                                   (0x00000000u)
        /* FPEC_CLKBSY field */
        #define FPEC_CLKBSY                                  (0x00000002u)
        #define FPEC_CLKBSY_MASK                             (0x00000002u)
        #define FPEC_CLKBSY_BIT                              (1)
        #define FPEC_CLKBSY_BITS                             (1)
        /* FPEC_CLKACK field */
        #define FPEC_CLKACK                                  (0x00000001u)
        #define FPEC_CLKACK_MASK                             (0x00000001u)
        #define FPEC_CLKACK_BIT                              (0)
        #define FPEC_CLKACK_BITS                             (1)

#define LV_SPARE                                             *((volatile uint32_t *)0x40004034u)
#define LV_SPARE_REG                                         *((volatile uint32_t *)0x40004034u)
#define LV_SPARE_ADDR                                        (0x40004034u)
#define LV_SPARE_RESET                                       (0x00000000u)
        /* LV_SPARE field */
        #define LV_SPARE_LV_SPARE                            (0x000000FFu)
        #define LV_SPARE_LV_SPARE_MASK                       (0x000000FFu)
        #define LV_SPARE_LV_SPARE_BIT                        (0)
        #define LV_SPARE_LV_SPARE_BITS                       (8)

/* RAM_CTRL block */
#define BLOCK_RAM_CTRL_BASE                                  (0x40005000u)
#define BLOCK_RAM_CTRL_END                                   (0x40005000u)
#define BLOCK_RAM_CTRL_SIZE                                  (BLOCK_RAM_CTRL_END - BLOCK_RAM_CTRL_BASE + 1)

/* SLOW_TIMERS block */
#define BLOCK_SLOW_TIMERS_BASE                               (0x40006000u)
#define BLOCK_SLOW_TIMERS_END                                (0x40006024u)
#define BLOCK_SLOW_TIMERS_SIZE                               (BLOCK_SLOW_TIMERS_END - BLOCK_SLOW_TIMERS_BASE + 1)

#define WDOG_CFG                                             *((volatile uint32_t *)0x40006000u)
#define WDOG_CFG_REG                                         *((volatile uint32_t *)0x40006000u)
#define WDOG_CFG_ADDR                                        (0x40006000u)
#define WDOG_CFG_RESET                                       (0x00000002u)
        /* WDOG_DISABLE field */
        #define WDOG_DISABLE                                 (0x00000002u)
        #define WDOG_DISABLE_MASK                            (0x00000002u)
        #define WDOG_DISABLE_BIT                             (1)
        #define WDOG_DISABLE_BITS                            (1)
        /* WDOG_ENABLE field */
        #define WDOG_ENABLE                                  (0x00000001u)
        #define WDOG_ENABLE_MASK                             (0x00000001u)
        #define WDOG_ENABLE_BIT                              (0)
        #define WDOG_ENABLE_BITS                             (1)

#define WDOG_KEY                                             *((volatile uint32_t *)0x40006004u)
#define WDOG_KEY_REG                                         *((volatile uint32_t *)0x40006004u)
#define WDOG_KEY_ADDR                                        (0x40006004u)
#define WDOG_KEY_RESET                                       (0x00000000u)
        /* WDOG_KEY_FIELD field */
        #define WDOG_KEY_FIELD                               (0x0000FFFFu)
        #define WDOG_KEY_FIELD_MASK                          (0x0000FFFFu)
        #define WDOG_KEY_FIELD_BIT                           (0)
        #define WDOG_KEY_FIELD_BITS                          (16)

#define WDOG_RESET                                           *((volatile uint32_t *)0x40006008u)
#define WDOG_RESET_REG                                       *((volatile uint32_t *)0x40006008u)
#define WDOG_RESET_ADDR                                      (0x40006008u)
#define WDOG_RESET_RESET                                     (0x00000000u)

#define SLEEPTMR_CFG                                         *((volatile uint32_t *)0x4000600Cu)
#define SLEEPTMR_CFG_REG                                     *((volatile uint32_t *)0x4000600Cu)
#define SLEEPTMR_CFG_ADDR                                    (0x4000600Cu)
#define SLEEPTMR_CFG_RESET                                   (0x00000400u)
        /* SLEEPTMR_REVERSE field */
        #define SLEEPTMR_REVERSE                             (0x00001000u)
        #define SLEEPTMR_REVERSE_MASK                        (0x00001000u)
        #define SLEEPTMR_REVERSE_BIT                         (12)
        #define SLEEPTMR_REVERSE_BITS                        (1)
        /* SLEEPTMR_ENABLE field */
        #define SLEEPTMR_ENABLE                              (0x00000800u)
        #define SLEEPTMR_ENABLE_MASK                         (0x00000800u)
        #define SLEEPTMR_ENABLE_BIT                          (11)
        #define SLEEPTMR_ENABLE_BITS                         (1)
        /* SLEEPTMR_DBGPAUSE field */
        #define SLEEPTMR_DBGPAUSE                            (0x00000400u)
        #define SLEEPTMR_DBGPAUSE_MASK                       (0x00000400u)
        #define SLEEPTMR_DBGPAUSE_BIT                        (10)
        #define SLEEPTMR_DBGPAUSE_BITS                       (1)
        /* SLEEPTMR_CLKDIV field */
        #define SLEEPTMR_CLKDIV                              (0x000000F0u)
        #define SLEEPTMR_CLKDIV_MASK                         (0x000000F0u)
        #define SLEEPTMR_CLKDIV_BIT                          (4)
        #define SLEEPTMR_CLKDIV_BITS                         (4)
        /* SLEEPTMR_CLKSEL field */
        #define SLEEPTMR_CLKSEL                              (0x00000001u)
        #define SLEEPTMR_CLKSEL_MASK                         (0x00000001u)
        #define SLEEPTMR_CLKSEL_BIT                          (0)
        #define SLEEPTMR_CLKSEL_BITS                         (1)

#define SLEEPTMR_CNTH                                        *((volatile uint32_t *)0x40006010u)
#define SLEEPTMR_CNTH_REG                                    *((volatile uint32_t *)0x40006010u)
#define SLEEPTMR_CNTH_ADDR                                   (0x40006010u)
#define SLEEPTMR_CNTH_RESET                                  (0x00000000u)
        /* SLEEPTMR_CNTH_FIELD field */
        #define SLEEPTMR_CNTH_FIELD                          (0x0000FFFFu)
        #define SLEEPTMR_CNTH_FIELD_MASK                     (0x0000FFFFu)
        #define SLEEPTMR_CNTH_FIELD_BIT                      (0)
        #define SLEEPTMR_CNTH_FIELD_BITS                     (16)

#define SLEEPTMR_CNTL                                        *((volatile uint32_t *)0x40006014u)
#define SLEEPTMR_CNTL_REG                                    *((volatile uint32_t *)0x40006014u)
#define SLEEPTMR_CNTL_ADDR                                   (0x40006014u)
#define SLEEPTMR_CNTL_RESET                                  (0x00000000u)
        /* SLEEPTMR_CNTL_FIELD field */
        #define SLEEPTMR_CNTL_FIELD                          (0x0000FFFFu)
        #define SLEEPTMR_CNTL_FIELD_MASK                     (0x0000FFFFu)
        #define SLEEPTMR_CNTL_FIELD_BIT                      (0)
        #define SLEEPTMR_CNTL_FIELD_BITS                     (16)

#define SLEEPTMR_CMPAH                                       *((volatile uint32_t *)0x40006018u)
#define SLEEPTMR_CMPAH_REG                                   *((volatile uint32_t *)0x40006018u)
#define SLEEPTMR_CMPAH_ADDR                                  (0x40006018u)
#define SLEEPTMR_CMPAH_RESET                                 (0x0000FFFFu)
        /* SLEEPTMR_CMPAH_FIELD field */
        #define SLEEPTMR_CMPAH_FIELD                         (0x0000FFFFu)
        #define SLEEPTMR_CMPAH_FIELD_MASK                    (0x0000FFFFu)
        #define SLEEPTMR_CMPAH_FIELD_BIT                     (0)
        #define SLEEPTMR_CMPAH_FIELD_BITS                    (16)

#define SLEEPTMR_CMPAL                                       *((volatile uint32_t *)0x4000601Cu)
#define SLEEPTMR_CMPAL_REG                                   *((volatile uint32_t *)0x4000601Cu)
#define SLEEPTMR_CMPAL_ADDR                                  (0x4000601Cu)
#define SLEEPTMR_CMPAL_RESET                                 (0x0000FFFFu)
        /* SLEEPTMR_CMPAL_FIELD field */
        #define SLEEPTMR_CMPAL_FIELD                         (0x0000FFFFu)
        #define SLEEPTMR_CMPAL_FIELD_MASK                    (0x0000FFFFu)
        #define SLEEPTMR_CMPAL_FIELD_BIT                     (0)
        #define SLEEPTMR_CMPAL_FIELD_BITS                    (16)

#define SLEEPTMR_CMPBH                                       *((volatile uint32_t *)0x40006020u)
#define SLEEPTMR_CMPBH_REG                                   *((volatile uint32_t *)0x40006020u)
#define SLEEPTMR_CMPBH_ADDR                                  (0x40006020u)
#define SLEEPTMR_CMPBH_RESET                                 (0x0000FFFFu)
        /* SLEEPTMR_CMPBH_FIELD field */
        #define SLEEPTMR_CMPBH_FIELD                         (0x0000FFFFu)
        #define SLEEPTMR_CMPBH_FIELD_MASK                    (0x0000FFFFu)
        #define SLEEPTMR_CMPBH_FIELD_BIT                     (0)
        #define SLEEPTMR_CMPBH_FIELD_BITS                    (16)

#define SLEEPTMR_CMPBL                                       *((volatile uint32_t *)0x40006024u)
#define SLEEPTMR_CMPBL_REG                                   *((volatile uint32_t *)0x40006024u)
#define SLEEPTMR_CMPBL_ADDR                                  (0x40006024u)
#define SLEEPTMR_CMPBL_RESET                                 (0x0000FFFFu)
        /* SLEEPTMR_CMPBL_FIELD field */
        #define SLEEPTMR_CMPBL_FIELD                         (0x0000FFFFu)
        #define SLEEPTMR_CMPBL_FIELD_MASK                    (0x0000FFFFu)
        #define SLEEPTMR_CMPBL_FIELD_BIT                     (0)
        #define SLEEPTMR_CMPBL_FIELD_BITS                    (16)

/* CAL_ADC block */
#define BLOCK_CAL_ADC_BASE                                   (0x40007000u)
#define BLOCK_CAL_ADC_END                                    (0x40007004u)
#define BLOCK_CAL_ADC_SIZE                                   (BLOCK_CAL_ADC_END - BLOCK_CAL_ADC_BASE + 1)

#define CAL_ADC_DATA                                         *((volatile uint32_t *)0x40007000u)
#define CAL_ADC_DATA_REG                                     *((volatile uint32_t *)0x40007000u)
#define CAL_ADC_DATA_ADDR                                    (0x40007000u)
#define CAL_ADC_DATA_RESET                                   (0x00000000u)
        /* CAL_ADC_DATA field */
        #define CAL_ADC_DATA_CAL_ADC_DATA                    (0x0000FFFFu)
        #define CAL_ADC_DATA_CAL_ADC_DATA_MASK               (0x0000FFFFu)
        #define CAL_ADC_DATA_CAL_ADC_DATA_BIT                (0)
        #define CAL_ADC_DATA_CAL_ADC_DATA_BITS               (16)

#define CAL_ADC_CONFIG                                       *((volatile uint32_t *)0x40007004u)
#define CAL_ADC_CONFIG_REG                                   *((volatile uint32_t *)0x40007004u)
#define CAL_ADC_CONFIG_ADDR                                  (0x40007004u)
#define CAL_ADC_CONFIG_RESET                                 (0x00000000u)
        /* CAL_ADC_RATE field */
        #define CAL_ADC_CONFIG_CAL_ADC_RATE                  (0x00007000u)
        #define CAL_ADC_CONFIG_CAL_ADC_RATE_MASK             (0x00007000u)
        #define CAL_ADC_CONFIG_CAL_ADC_RATE_BIT              (12)
        #define CAL_ADC_CONFIG_CAL_ADC_RATE_BITS             (3)
        /* CAL_ADC_MUX field */
        #define CAL_ADC_CONFIG_CAL_ADC_MUX                   (0x00000F80u)
        #define CAL_ADC_CONFIG_CAL_ADC_MUX_MASK              (0x00000F80u)
        #define CAL_ADC_CONFIG_CAL_ADC_MUX_BIT               (7)
        #define CAL_ADC_CONFIG_CAL_ADC_MUX_BITS              (5)
        /* CAL_ADC_CLKSEL field */
        #define CAL_ADC_CONFIG_CAL_ADC_CLKSEL                (0x00000004u)
        #define CAL_ADC_CONFIG_CAL_ADC_CLKSEL_MASK           (0x00000004u)
        #define CAL_ADC_CONFIG_CAL_ADC_CLKSEL_BIT            (2)
        #define CAL_ADC_CONFIG_CAL_ADC_CLKSEL_BITS           (1)
        /* CAL_ADC_DITHER_DIS field */
        #define CAL_ADC_CONFIG_CAL_ADC_DITHER_DIS            (0x00000002u)
        #define CAL_ADC_CONFIG_CAL_ADC_DITHER_DIS_MASK       (0x00000002u)
        #define CAL_ADC_CONFIG_CAL_ADC_DITHER_DIS_BIT        (1)
        #define CAL_ADC_CONFIG_CAL_ADC_DITHER_DIS_BITS       (1)
        /* CAL_ADC_EN field */
        #define CAL_ADC_CONFIG_CAL_ADC_EN                    (0x00000001u)
        #define CAL_ADC_CONFIG_CAL_ADC_EN_MASK               (0x00000001u)
        #define CAL_ADC_CONFIG_CAL_ADC_EN_BIT                (0)
        #define CAL_ADC_CONFIG_CAL_ADC_EN_BITS               (1)

/* FLASH_CONTROL block */
#define BLOCK_FLASH_CONTROL_BASE                             (0x40008000u)
#define BLOCK_FLASH_CONTROL_END                              (0x40008084u)
#define BLOCK_FLASH_CONTROL_SIZE                             (BLOCK_FLASH_CONTROL_END - BLOCK_FLASH_CONTROL_BASE + 1)

#define FLASH_ACCESS                                         *((volatile uint32_t *)0x40008000u)
#define FLASH_ACCESS_REG                                     *((volatile uint32_t *)0x40008000u)
#define FLASH_ACCESS_ADDR                                    (0x40008000u)
#define FLASH_ACCESS_RESET                                   (0x00000031u)
        /* FLASH_LP field */
        #define FLASH_ACCESS_FLASH_LP                        (0x00000040u)
        #define FLASH_ACCESS_FLASH_LP_MASK                   (0x00000040u)
        #define FLASH_ACCESS_FLASH_LP_BIT                    (6)
        #define FLASH_ACCESS_FLASH_LP_BITS                   (1)
        /* PREFETCH_STATUS field */
        #define FLASH_ACCESS_PREFETCH_STATUS                 (0x00000020u)
        #define FLASH_ACCESS_PREFETCH_STATUS_MASK            (0x00000020u)
        #define FLASH_ACCESS_PREFETCH_STATUS_BIT             (5)
        #define FLASH_ACCESS_PREFETCH_STATUS_BITS            (1)
        /* PREFETCH_EN field */
        #define FLASH_ACCESS_PREFETCH_EN                     (0x00000010u)
        #define FLASH_ACCESS_PREFETCH_EN_MASK                (0x00000010u)
        #define FLASH_ACCESS_PREFETCH_EN_BIT                 (4)
        #define FLASH_ACCESS_PREFETCH_EN_BITS                (1)
        /* HALFCYCLE_ACCESS field */
        #define FLASH_ACCESS_HALFCYCLE_ACCESS                (0x00000008u)
        #define FLASH_ACCESS_HALFCYCLE_ACCESS_MASK           (0x00000008u)
        #define FLASH_ACCESS_HALFCYCLE_ACCESS_BIT            (3)
        #define FLASH_ACCESS_HALFCYCLE_ACCESS_BITS           (1)
        /* CODE_LATENCY field */
        #define FLASH_ACCESS_CODE_LATENCY                    (0x00000007u)
        #define FLASH_ACCESS_CODE_LATENCY_MASK               (0x00000007u)
        #define FLASH_ACCESS_CODE_LATENCY_BIT                (0)
        #define FLASH_ACCESS_CODE_LATENCY_BITS               (3)

#define FPEC_KEY                                             *((volatile uint32_t *)0x40008004u)
#define FPEC_KEY_REG                                         *((volatile uint32_t *)0x40008004u)
#define FPEC_KEY_ADDR                                        (0x40008004u)
#define FPEC_KEY_RESET                                       (0x00000000u)
        /* FKEYR field */
        #define FPEC_KEY_FKEYR                               (0xFFFFFFFFu)
        #define FPEC_KEY_FKEYR_MASK                          (0xFFFFFFFFu)
        #define FPEC_KEY_FKEYR_BIT                           (0)
        #define FPEC_KEY_FKEYR_BITS                          (32)

#define OPT_KEY                                              *((volatile uint32_t *)0x40008008u)
#define OPT_KEY_REG                                          *((volatile uint32_t *)0x40008008u)
#define OPT_KEY_ADDR                                         (0x40008008u)
#define OPT_KEY_RESET                                        (0x00000000u)
        /* OPTKEYR field */
        #define OPT_KEY_OPTKEYR                              (0xFFFFFFFFu)
        #define OPT_KEY_OPTKEYR_MASK                         (0xFFFFFFFFu)
        #define OPT_KEY_OPTKEYR_BIT                          (0)
        #define OPT_KEY_OPTKEYR_BITS                         (32)

#define FLASH_STATUS                                         *((volatile uint32_t *)0x4000800Cu)
#define FLASH_STATUS_REG                                     *((volatile uint32_t *)0x4000800Cu)
#define FLASH_STATUS_ADDR                                    (0x4000800Cu)
#define FLASH_STATUS_RESET                                   (0x00000000u)
        /* EOP field */
        #define FLASH_STATUS_EOP                             (0x00000020u)
        #define FLASH_STATUS_EOP_MASK                        (0x00000020u)
        #define FLASH_STATUS_EOP_BIT                         (5)
        #define FLASH_STATUS_EOP_BITS                        (1)
        /* WRP_ERR field */
        #define FLASH_STATUS_WRP_ERR                         (0x00000010u)
        #define FLASH_STATUS_WRP_ERR_MASK                    (0x00000010u)
        #define FLASH_STATUS_WRP_ERR_BIT                     (4)
        #define FLASH_STATUS_WRP_ERR_BITS                    (1)
        /* PAGE_PROG_ERR field */
        #define FLASH_STATUS_PAGE_PROG_ERR                   (0x00000008u)
        #define FLASH_STATUS_PAGE_PROG_ERR_MASK              (0x00000008u)
        #define FLASH_STATUS_PAGE_PROG_ERR_BIT               (3)
        #define FLASH_STATUS_PAGE_PROG_ERR_BITS              (1)
        /* PROG_ERR field */
        #define FLASH_STATUS_PROG_ERR                        (0x00000004u)
        #define FLASH_STATUS_PROG_ERR_MASK                   (0x00000004u)
        #define FLASH_STATUS_PROG_ERR_BIT                    (2)
        #define FLASH_STATUS_PROG_ERR_BITS                   (1)
        /* EARLY_BSY field */
        #define FLASH_STATUS_EARLY_BSY                       (0x00000002u)
        #define FLASH_STATUS_EARLY_BSY_MASK                  (0x00000002u)
        #define FLASH_STATUS_EARLY_BSY_BIT                   (1)
        #define FLASH_STATUS_EARLY_BSY_BITS                  (1)
        /* FLA_BSY field */
        #define FLASH_STATUS_FLA_BSY                         (0x00000001u)
        #define FLASH_STATUS_FLA_BSY_MASK                    (0x00000001u)
        #define FLASH_STATUS_FLA_BSY_BIT                     (0)
        #define FLASH_STATUS_FLA_BSY_BITS                    (1)

#define FLASH_CTRL                                           *((volatile uint32_t *)0x40008010u)
#define FLASH_CTRL_REG                                       *((volatile uint32_t *)0x40008010u)
#define FLASH_CTRL_ADDR                                      (0x40008010u)
#define FLASH_CTRL_RESET                                     (0x00000080u)
        /* EOPIE field */
        #define FLASH_CTRL_EOPIE                             (0x00001000u)
        #define FLASH_CTRL_EOPIE_MASK                        (0x00001000u)
        #define FLASH_CTRL_EOPIE_BIT                         (12)
        #define FLASH_CTRL_EOPIE_BITS                        (1)
        /* EARLYBSYIE field */
        #define FLASH_CTRL_EARLYBSYIE                        (0x00000800u)
        #define FLASH_CTRL_EARLYBSYIE_MASK                   (0x00000800u)
        #define FLASH_CTRL_EARLYBSYIE_BIT                    (11)
        #define FLASH_CTRL_EARLYBSYIE_BITS                   (1)
        /* ERRIE field */
        #define FLASH_CTRL_ERRIE                             (0x00000400u)
        #define FLASH_CTRL_ERRIE_MASK                        (0x00000400u)
        #define FLASH_CTRL_ERRIE_BIT                         (10)
        #define FLASH_CTRL_ERRIE_BITS                        (1)
        /* OPTWREN field */
        #define FLASH_CTRL_OPTWREN                           (0x00000200u)
        #define FLASH_CTRL_OPTWREN_MASK                      (0x00000200u)
        #define FLASH_CTRL_OPTWREN_BIT                       (9)
        #define FLASH_CTRL_OPTWREN_BITS                      (1)
        /* FSTPROG field */
        #define FLASH_CTRL_FSTPROG                           (0x00000100u)
        #define FLASH_CTRL_FSTPROG_MASK                      (0x00000100u)
        #define FLASH_CTRL_FSTPROG_BIT                       (8)
        #define FLASH_CTRL_FSTPROG_BITS                      (1)
        /* LOCK field */
        #define FLASH_CTRL_LOCK                              (0x00000080u)
        #define FLASH_CTRL_LOCK_MASK                         (0x00000080u)
        #define FLASH_CTRL_LOCK_BIT                          (7)
        #define FLASH_CTRL_LOCK_BITS                         (1)
        /* FLA_START field */
        #define FLASH_CTRL_FLA_START                         (0x00000040u)
        #define FLASH_CTRL_FLA_START_MASK                    (0x00000040u)
        #define FLASH_CTRL_FLA_START_BIT                     (6)
        #define FLASH_CTRL_FLA_START_BITS                    (1)
        /* OPTERASE field */
        #define FLASH_CTRL_OPTERASE                          (0x00000020u)
        #define FLASH_CTRL_OPTERASE_MASK                     (0x00000020u)
        #define FLASH_CTRL_OPTERASE_BIT                      (5)
        #define FLASH_CTRL_OPTERASE_BITS                     (1)
        /* OPTPROG field */
        #define FLASH_CTRL_OPTPROG                           (0x00000010u)
        #define FLASH_CTRL_OPTPROG_MASK                      (0x00000010u)
        #define FLASH_CTRL_OPTPROG_BIT                       (4)
        #define FLASH_CTRL_OPTPROG_BITS                      (1)
        /* GLOBALERASE field */
        #define FLASH_CTRL_GLOBALERASE                       (0x00000008u)
        #define FLASH_CTRL_GLOBALERASE_MASK                  (0x00000008u)
        #define FLASH_CTRL_GLOBALERASE_BIT                   (3)
        #define FLASH_CTRL_GLOBALERASE_BITS                  (1)
        /* MASSERASE field */
        #define FLASH_CTRL_MASSERASE                         (0x00000004u)
        #define FLASH_CTRL_MASSERASE_MASK                    (0x00000004u)
        #define FLASH_CTRL_MASSERASE_BIT                     (2)
        #define FLASH_CTRL_MASSERASE_BITS                    (1)
        /* PAGEERASE field */
        #define FLASH_CTRL_PAGEERASE                         (0x00000002u)
        #define FLASH_CTRL_PAGEERASE_MASK                    (0x00000002u)
        #define FLASH_CTRL_PAGEERASE_BIT                     (1)
        #define FLASH_CTRL_PAGEERASE_BITS                    (1)
        /* PROG field */
        #define FLASH_CTRL_PROG                              (0x00000001u)
        #define FLASH_CTRL_PROG_MASK                         (0x00000001u)
        #define FLASH_CTRL_PROG_BIT                          (0)
        #define FLASH_CTRL_PROG_BITS                         (1)

#define FLASH_ADDR                                           *((volatile uint32_t *)0x40008014u)
#define FLASH_ADDR_REG                                       *((volatile uint32_t *)0x40008014u)
#define FLASH_ADDR_ADDR                                      (0x40008014u)
#define FLASH_ADDR_RESET                                     (0x00000000u)
        /* FAR field */
        #define FLASH_ADDR_FAR                               (0xFFFFFFFFu)
        #define FLASH_ADDR_FAR_MASK                          (0xFFFFFFFFu)
        #define FLASH_ADDR_FAR_BIT                           (0)
        #define FLASH_ADDR_FAR_BITS                          (32)

#define OPT_BYTE                                             *((volatile uint32_t *)0x4000801Cu)
#define OPT_BYTE_REG                                         *((volatile uint32_t *)0x4000801Cu)
#define OPT_BYTE_ADDR                                        (0x4000801Cu)
#define OPT_BYTE_RESET                                       (0xFBFFFFFEu)
        /* RSVD field */
        #define OPT_BYTE_RSVD                                (0xF8000000u)
        #define OPT_BYTE_RSVD_MASK                           (0xF8000000u)
        #define OPT_BYTE_RSVD_BIT                            (27)
        #define OPT_BYTE_RSVD_BITS                           (5)
        /* OBR field */
        #define OPT_BYTE_OBR                                 (0x07FFFFFCu)
        #define OPT_BYTE_OBR_MASK                            (0x07FFFFFCu)
        #define OPT_BYTE_OBR_BIT                             (2)
        #define OPT_BYTE_OBR_BITS                            (25)
        /* RDPROT field */
        #define OPT_BYTE_RDPROT                              (0x00000002u)
        #define OPT_BYTE_RDPROT_MASK                         (0x00000002u)
        #define OPT_BYTE_RDPROT_BIT                          (1)
        #define OPT_BYTE_RDPROT_BITS                         (1)
        /* OPT_ERR field */
        #define OPT_BYTE_OPT_ERR                             (0x00000001u)
        #define OPT_BYTE_OPT_ERR_MASK                        (0x00000001u)
        #define OPT_BYTE_OPT_ERR_BIT                         (0)
        #define OPT_BYTE_OPT_ERR_BITS                        (1)

#define WRPROT                                               *((volatile uint32_t *)0x40008020u)
#define WRPROT_REG                                           *((volatile uint32_t *)0x40008020u)
#define WRPROT_ADDR                                          (0x40008020u)
#define WRPROT_RESET                                         (0xFFFFFFFFu)
        /* WRP field */
        #define WRPROT_WRP                                   (0xFFFFFFFFu)
        #define WRPROT_WRP_MASK                              (0xFFFFFFFFu)
        #define WRPROT_WRP_BIT                               (0)
        #define WRPROT_WRP_BITS                              (32)

#define FLASH_TEST_CTRL                                      *((volatile uint32_t *)0x40008080u)
#define FLASH_TEST_CTRL_REG                                  *((volatile uint32_t *)0x40008080u)
#define FLASH_TEST_CTRL_ADDR                                 (0x40008080u)
#define FLASH_TEST_CTRL_RESET                                (0x00000000u)
        /* TMR field */
        #define FLASH_TEST_CTRL_TMR                          (0x00001000u)
        #define FLASH_TEST_CTRL_TMR_MASK                     (0x00001000u)
        #define FLASH_TEST_CTRL_TMR_BIT                      (12)
        #define FLASH_TEST_CTRL_TMR_BITS                     (1)
        /* ERASE field */
        #define FLASH_TEST_CTRL_ERASE                        (0x00000800u)
        #define FLASH_TEST_CTRL_ERASE_MASK                   (0x00000800u)
        #define FLASH_TEST_CTRL_ERASE_BIT                    (11)
        #define FLASH_TEST_CTRL_ERASE_BITS                   (1)
        /* MAS1 field */
        #define FLASH_TEST_CTRL_MAS1                         (0x00000400u)
        #define FLASH_TEST_CTRL_MAS1_MASK                    (0x00000400u)
        #define FLASH_TEST_CTRL_MAS1_BIT                     (10)
        #define FLASH_TEST_CTRL_MAS1_BITS                    (1)
        /* TEST_PROG field */
        #define FLASH_TEST_CTRL_TEST_PROG                    (0x00000200u)
        #define FLASH_TEST_CTRL_TEST_PROG_MASK               (0x00000200u)
        #define FLASH_TEST_CTRL_TEST_PROG_BIT                (9)
        #define FLASH_TEST_CTRL_TEST_PROG_BITS               (1)
        /* NVSTR field */
        #define FLASH_TEST_CTRL_NVSTR                        (0x00000100u)
        #define FLASH_TEST_CTRL_NVSTR_MASK                   (0x00000100u)
        #define FLASH_TEST_CTRL_NVSTR_BIT                    (8)
        #define FLASH_TEST_CTRL_NVSTR_BITS                   (1)
        /* SE field */
        #define FLASH_TEST_CTRL_SE                           (0x00000080u)
        #define FLASH_TEST_CTRL_SE_MASK                      (0x00000080u)
        #define FLASH_TEST_CTRL_SE_BIT                       (7)
        #define FLASH_TEST_CTRL_SE_BITS                      (1)
        /* IFREN field */
        #define FLASH_TEST_CTRL_IFREN                        (0x00000040u)
        #define FLASH_TEST_CTRL_IFREN_MASK                   (0x00000040u)
        #define FLASH_TEST_CTRL_IFREN_BIT                    (6)
        #define FLASH_TEST_CTRL_IFREN_BITS                   (1)
        /* YE field */
        #define FLASH_TEST_CTRL_YE                           (0x00000020u)
        #define FLASH_TEST_CTRL_YE_MASK                      (0x00000020u)
        #define FLASH_TEST_CTRL_YE_BIT                       (5)
        #define FLASH_TEST_CTRL_YE_BITS                      (1)
        /* XE field */
        #define FLASH_TEST_CTRL_XE                           (0x00000010u)
        #define FLASH_TEST_CTRL_XE_MASK                      (0x00000010u)
        #define FLASH_TEST_CTRL_XE_BIT                       (4)
        #define FLASH_TEST_CTRL_XE_BITS                      (1)
        /* SW_CTRL field */
        #define FLASH_TEST_CTRL_SW_CTRL                      (0x00000008u)
        #define FLASH_TEST_CTRL_SW_CTRL_MASK                 (0x00000008u)
        #define FLASH_TEST_CTRL_SW_CTRL_BIT                  (3)
        #define FLASH_TEST_CTRL_SW_CTRL_BITS                 (1)
        /* SW field */
        #define FLASH_TEST_CTRL_SW                           (0x00000006u)
        #define FLASH_TEST_CTRL_SW_MASK                      (0x00000006u)
        #define FLASH_TEST_CTRL_SW_BIT                       (1)
        #define FLASH_TEST_CTRL_SW_BITS                      (2)
        /* SW_EN field */
        #define FLASH_TEST_CTRL_SW_EN                        (0x00000001u)
        #define FLASH_TEST_CTRL_SW_EN_MASK                   (0x00000001u)
        #define FLASH_TEST_CTRL_SW_EN_BIT                    (0)
        #define FLASH_TEST_CTRL_SW_EN_BITS                   (1)

#define FLASH_DATA0                                          *((volatile uint32_t *)0x40008084u)
#define FLASH_DATA0_REG                                      *((volatile uint32_t *)0x40008084u)
#define FLASH_DATA0_ADDR                                     (0x40008084u)
#define FLASH_DATA0_RESET                                    (0xFFFFFFFFu)
        /* FDR0 field */
        #define FLASH_DATA0_FDR0                             (0xFFFFFFFFu)
        #define FLASH_DATA0_FDR0_MASK                        (0xFFFFFFFFu)
        #define FLASH_DATA0_FDR0_BIT                         (0)
        #define FLASH_DATA0_FDR0_BITS                        (32)

#define I_AM_AN_EMULATOR                                     *((volatile uint32_t *)0x40009000u)
#define I_AM_AN_EMULATOR_REG                                 *((volatile uint32_t *)0x40009000u)
#define I_AM_AN_EMULATOR_ADDR                                (0x40009000u)
#define I_AM_AN_EMULATOR_RESET                               (0x00000000u)
        /* I_AM_AN_EMULATOR field */
        #define I_AM_AN_EMULATOR_I_AM_AN_EMULATOR            (0x00000001u)
        #define I_AM_AN_EMULATOR_I_AM_AN_EMULATOR_MASK       (0x00000001u)
        #define I_AM_AN_EMULATOR_I_AM_AN_EMULATOR_BIT        (0)
        #define I_AM_AN_EMULATOR_I_AM_AN_EMULATOR_BITS       (1)

/* INTERRUPTS block */
#define BLOCK_INTERRUPTS_BASE                                (0x4000A000u)
#define BLOCK_INTERRUPTS_END                                 (0x4000A88Cu)
#define BLOCK_INTERRUPTS_SIZE                                (BLOCK_INTERRUPTS_END - BLOCK_INTERRUPTS_BASE + 1)

#define MAC_RX_INT_SRC                                       *((volatile uint32_t *)0x4000A000u)
#define MAC_RX_INT_SRC_REG                                   *((volatile uint32_t *)0x4000A000u)
#define MAC_RX_INT_SRC_ADDR                                  (0x4000A000u)
#define MAC_RX_INT_SRC_RESET                                 (0x00000000u)
        /* TX_B_ACK_ERR_SRC field */
        #define MAC_RX_INT_SRC_TX_B_ACK_ERR_SRC              (0x00008000u)
        #define MAC_RX_INT_SRC_TX_B_ACK_ERR_SRC_MASK         (0x00008000u)
        #define MAC_RX_INT_SRC_TX_B_ACK_ERR_SRC_BIT          (15)
        #define MAC_RX_INT_SRC_TX_B_ACK_ERR_SRC_BITS         (1)
        /* TX_A_ACK_ERR_SRC field */
        #define MAC_RX_INT_SRC_TX_A_ACK_ERR_SRC              (0x00004000u)
        #define MAC_RX_INT_SRC_TX_A_ACK_ERR_SRC_MASK         (0x00004000u)
        #define MAC_RX_INT_SRC_TX_A_ACK_ERR_SRC_BIT          (14)
        #define MAC_RX_INT_SRC_TX_A_ACK_ERR_SRC_BITS         (1)
        /* RX_OVFLW_SRC field */
        #define MAC_RX_INT_SRC_RX_OVFLW_SRC                  (0x00002000u)
        #define MAC_RX_INT_SRC_RX_OVFLW_SRC_MASK             (0x00002000u)
        #define MAC_RX_INT_SRC_RX_OVFLW_SRC_BIT              (13)
        #define MAC_RX_INT_SRC_RX_OVFLW_SRC_BITS             (1)
        /* RX_ERROR_SRC field */
        #define MAC_RX_INT_SRC_RX_ERROR_SRC                  (0x00001000u)
        #define MAC_RX_INT_SRC_RX_ERROR_SRC_MASK             (0x00001000u)
        #define MAC_RX_INT_SRC_RX_ERROR_SRC_BIT              (12)
        #define MAC_RX_INT_SRC_RX_ERROR_SRC_BITS             (1)
        /* BB_RX_LEN_ERR_SRC field */
        #define MAC_RX_INT_SRC_BB_RX_LEN_ERR_SRC             (0x00000800u)
        #define MAC_RX_INT_SRC_BB_RX_LEN_ERR_SRC_MASK        (0x00000800u)
        #define MAC_RX_INT_SRC_BB_RX_LEN_ERR_SRC_BIT         (11)
        #define MAC_RX_INT_SRC_BB_RX_LEN_ERR_SRC_BITS        (1)
        /* TX_COLL_RX_SRC field */
        #define MAC_RX_INT_SRC_TX_COLL_RX_SRC                (0x00000400u)
        #define MAC_RX_INT_SRC_TX_COLL_RX_SRC_MASK           (0x00000400u)
        #define MAC_RX_INT_SRC_TX_COLL_RX_SRC_BIT            (10)
        #define MAC_RX_INT_SRC_TX_COLL_RX_SRC_BITS           (1)
        /* RSSI_INST_MEAS_SRC field */
        #define MAC_RX_INT_SRC_RSSI_INST_MEAS_SRC            (0x00000200u)
        #define MAC_RX_INT_SRC_RSSI_INST_MEAS_SRC_MASK       (0x00000200u)
        #define MAC_RX_INT_SRC_RSSI_INST_MEAS_SRC_BIT        (9)
        #define MAC_RX_INT_SRC_RSSI_INST_MEAS_SRC_BITS       (1)
        /* TX_B_ACK_SRC field */
        #define MAC_RX_INT_SRC_TX_B_ACK_SRC                  (0x00000100u)
        #define MAC_RX_INT_SRC_TX_B_ACK_SRC_MASK             (0x00000100u)
        #define MAC_RX_INT_SRC_TX_B_ACK_SRC_BIT              (8)
        #define MAC_RX_INT_SRC_TX_B_ACK_SRC_BITS             (1)
        /* TX_A_ACK_SRC field */
        #define MAC_RX_INT_SRC_TX_A_ACK_SRC                  (0x00000080u)
        #define MAC_RX_INT_SRC_TX_A_ACK_SRC_MASK             (0x00000080u)
        #define MAC_RX_INT_SRC_TX_A_ACK_SRC_BIT              (7)
        #define MAC_RX_INT_SRC_TX_A_ACK_SRC_BITS             (1)
        /* RX_B_UNLOAD_COMP_SRC field */
        #define MAC_RX_INT_SRC_RX_B_UNLOAD_COMP_SRC          (0x00000040u)
        #define MAC_RX_INT_SRC_RX_B_UNLOAD_COMP_SRC_MASK     (0x00000040u)
        #define MAC_RX_INT_SRC_RX_B_UNLOAD_COMP_SRC_BIT      (6)
        #define MAC_RX_INT_SRC_RX_B_UNLOAD_COMP_SRC_BITS     (1)
        /* RX_A_UNLOAD_COMP_SRC field */
        #define MAC_RX_INT_SRC_RX_A_UNLOAD_COMP_SRC          (0x00000020u)
        #define MAC_RX_INT_SRC_RX_A_UNLOAD_COMP_SRC_MASK     (0x00000020u)
        #define MAC_RX_INT_SRC_RX_A_UNLOAD_COMP_SRC_BIT      (5)
        #define MAC_RX_INT_SRC_RX_A_UNLOAD_COMP_SRC_BITS     (1)
        /* RX_B_ADDR_REC_SRC field */
        #define MAC_RX_INT_SRC_RX_B_ADDR_REC_SRC             (0x00000010u)
        #define MAC_RX_INT_SRC_RX_B_ADDR_REC_SRC_MASK        (0x00000010u)
        #define MAC_RX_INT_SRC_RX_B_ADDR_REC_SRC_BIT         (4)
        #define MAC_RX_INT_SRC_RX_B_ADDR_REC_SRC_BITS        (1)
        /* RX_A_ADDR_REC_SRC field */
        #define MAC_RX_INT_SRC_RX_A_ADDR_REC_SRC             (0x00000008u)
        #define MAC_RX_INT_SRC_RX_A_ADDR_REC_SRC_MASK        (0x00000008u)
        #define MAC_RX_INT_SRC_RX_A_ADDR_REC_SRC_BIT         (3)
        #define MAC_RX_INT_SRC_RX_A_ADDR_REC_SRC_BITS        (1)
        /* RX_B_FILT_COMP_SRC field */
        #define MAC_RX_INT_SRC_RX_B_FILT_COMP_SRC            (0x00000004u)
        #define MAC_RX_INT_SRC_RX_B_FILT_COMP_SRC_MASK       (0x00000004u)
        #define MAC_RX_INT_SRC_RX_B_FILT_COMP_SRC_BIT        (2)
        #define MAC_RX_INT_SRC_RX_B_FILT_COMP_SRC_BITS       (1)
        /* RX_A_FILT_COMP_SRC field */
        #define MAC_RX_INT_SRC_RX_A_FILT_COMP_SRC            (0x00000002u)
        #define MAC_RX_INT_SRC_RX_A_FILT_COMP_SRC_MASK       (0x00000002u)
        #define MAC_RX_INT_SRC_RX_A_FILT_COMP_SRC_BIT        (1)
        #define MAC_RX_INT_SRC_RX_A_FILT_COMP_SRC_BITS       (1)
        /* RX_FRAME_SRC field */
        #define MAC_RX_INT_SRC_RX_FRAME_SRC                  (0x00000001u)
        #define MAC_RX_INT_SRC_RX_FRAME_SRC_MASK             (0x00000001u)
        #define MAC_RX_INT_SRC_RX_FRAME_SRC_BIT              (0)
        #define MAC_RX_INT_SRC_RX_FRAME_SRC_BITS             (1)

#define MAC_TX_INT_SRC                                       *((volatile uint32_t *)0x4000A004u)
#define MAC_TX_INT_SRC_REG                                   *((volatile uint32_t *)0x4000A004u)
#define MAC_TX_INT_SRC_ADDR                                  (0x4000A004u)
#define MAC_TX_INT_SRC_RESET                                 (0x00000000u)
        /* RX_B_ACK_SRC field */
        #define MAC_TX_INT_SRC_RX_B_ACK_SRC                  (0x00000800u)
        #define MAC_TX_INT_SRC_RX_B_ACK_SRC_MASK             (0x00000800u)
        #define MAC_TX_INT_SRC_RX_B_ACK_SRC_BIT              (11)
        #define MAC_TX_INT_SRC_RX_B_ACK_SRC_BITS             (1)
        /* RX_A_ACK_SRC field */
        #define MAC_TX_INT_SRC_RX_A_ACK_SRC                  (0x00000400u)
        #define MAC_TX_INT_SRC_RX_A_ACK_SRC_MASK             (0x00000400u)
        #define MAC_TX_INT_SRC_RX_A_ACK_SRC_BIT              (10)
        #define MAC_TX_INT_SRC_RX_A_ACK_SRC_BITS             (1)
        /* TX_B_UNLOAD_SRC field */
        #define MAC_TX_INT_SRC_TX_B_UNLOAD_SRC               (0x00000200u)
        #define MAC_TX_INT_SRC_TX_B_UNLOAD_SRC_MASK          (0x00000200u)
        #define MAC_TX_INT_SRC_TX_B_UNLOAD_SRC_BIT           (9)
        #define MAC_TX_INT_SRC_TX_B_UNLOAD_SRC_BITS          (1)
        /* TX_A_UNLOAD_SRC field */
        #define MAC_TX_INT_SRC_TX_A_UNLOAD_SRC               (0x00000100u)
        #define MAC_TX_INT_SRC_TX_A_UNLOAD_SRC_MASK          (0x00000100u)
        #define MAC_TX_INT_SRC_TX_A_UNLOAD_SRC_BIT           (8)
        #define MAC_TX_INT_SRC_TX_A_UNLOAD_SRC_BITS          (1)
        /* ACK_EXPIRED_SRC field */
        #define MAC_TX_INT_SRC_ACK_EXPIRED_SRC               (0x00000080u)
        #define MAC_TX_INT_SRC_ACK_EXPIRED_SRC_MASK          (0x00000080u)
        #define MAC_TX_INT_SRC_ACK_EXPIRED_SRC_BIT           (7)
        #define MAC_TX_INT_SRC_ACK_EXPIRED_SRC_BITS          (1)
        /* TX_LOCK_FAIL_SRC field */
        #define MAC_TX_INT_SRC_TX_LOCK_FAIL_SRC              (0x00000040u)
        #define MAC_TX_INT_SRC_TX_LOCK_FAIL_SRC_MASK         (0x00000040u)
        #define MAC_TX_INT_SRC_TX_LOCK_FAIL_SRC_BIT          (6)
        #define MAC_TX_INT_SRC_TX_LOCK_FAIL_SRC_BITS         (1)
        /* TX_UNDERFLOW_SRC field */
        #define MAC_TX_INT_SRC_TX_UNDERFLOW_SRC              (0x00000020u)
        #define MAC_TX_INT_SRC_TX_UNDERFLOW_SRC_MASK         (0x00000020u)
        #define MAC_TX_INT_SRC_TX_UNDERFLOW_SRC_BIT          (5)
        #define MAC_TX_INT_SRC_TX_UNDERFLOW_SRC_BITS         (1)
        /* CCA_FAIL_SRC field */
        #define MAC_TX_INT_SRC_CCA_FAIL_SRC                  (0x00000010u)
        #define MAC_TX_INT_SRC_CCA_FAIL_SRC_MASK             (0x00000010u)
        #define MAC_TX_INT_SRC_CCA_FAIL_SRC_BIT              (4)
        #define MAC_TX_INT_SRC_CCA_FAIL_SRC_BITS             (1)
        /* SFD_SENT_SRC field */
        #define MAC_TX_INT_SRC_SFD_SENT_SRC                  (0x00000008u)
        #define MAC_TX_INT_SRC_SFD_SENT_SRC_MASK             (0x00000008u)
        #define MAC_TX_INT_SRC_SFD_SENT_SRC_BIT              (3)
        #define MAC_TX_INT_SRC_SFD_SENT_SRC_BITS             (1)
        /* BO_COMPLETE_SRC field */
        #define MAC_TX_INT_SRC_BO_COMPLETE_SRC               (0x00000004u)
        #define MAC_TX_INT_SRC_BO_COMPLETE_SRC_MASK          (0x00000004u)
        #define MAC_TX_INT_SRC_BO_COMPLETE_SRC_BIT           (2)
        #define MAC_TX_INT_SRC_BO_COMPLETE_SRC_BITS          (1)
        /* RX_ACK_SRC field */
        #define MAC_TX_INT_SRC_RX_ACK_SRC                    (0x00000002u)
        #define MAC_TX_INT_SRC_RX_ACK_SRC_MASK               (0x00000002u)
        #define MAC_TX_INT_SRC_RX_ACK_SRC_BIT                (1)
        #define MAC_TX_INT_SRC_RX_ACK_SRC_BITS               (1)
        /* TX_COMPLETE_SRC field */
        #define MAC_TX_INT_SRC_TX_COMPLETE_SRC               (0x00000001u)
        #define MAC_TX_INT_SRC_TX_COMPLETE_SRC_MASK          (0x00000001u)
        #define MAC_TX_INT_SRC_TX_COMPLETE_SRC_BIT           (0)
        #define MAC_TX_INT_SRC_TX_COMPLETE_SRC_BITS          (1)

#define MAC_TIMER_INT_SRC                                    *((volatile uint32_t *)0x4000A008u)
#define MAC_TIMER_INT_SRC_REG                                *((volatile uint32_t *)0x4000A008u)
#define MAC_TIMER_INT_SRC_ADDR                               (0x4000A008u)
#define MAC_TIMER_INT_SRC_RESET                              (0x00000000u)
        /* TIMER_COMP_B_SRC field */
        #define MAC_TIMER_INT_SRC_TIMER_COMP_B_SRC           (0x00000004u)
        #define MAC_TIMER_INT_SRC_TIMER_COMP_B_SRC_MASK      (0x00000004u)
        #define MAC_TIMER_INT_SRC_TIMER_COMP_B_SRC_BIT       (2)
        #define MAC_TIMER_INT_SRC_TIMER_COMP_B_SRC_BITS      (1)
        /* TIMER_COMP_A_SRC field */
        #define MAC_TIMER_INT_SRC_TIMER_COMP_A_SRC           (0x00000002u)
        #define MAC_TIMER_INT_SRC_TIMER_COMP_A_SRC_MASK      (0x00000002u)
        #define MAC_TIMER_INT_SRC_TIMER_COMP_A_SRC_BIT       (1)
        #define MAC_TIMER_INT_SRC_TIMER_COMP_A_SRC_BITS      (1)
        /* TIMER_WRAP_SRC field */
        #define MAC_TIMER_INT_SRC_TIMER_WRAP_SRC             (0x00000001u)
        #define MAC_TIMER_INT_SRC_TIMER_WRAP_SRC_MASK        (0x00000001u)
        #define MAC_TIMER_INT_SRC_TIMER_WRAP_SRC_BIT         (0)
        #define MAC_TIMER_INT_SRC_TIMER_WRAP_SRC_BITS        (1)

#define BB_INT_SRC                                           *((volatile uint32_t *)0x4000A00Cu)
#define BB_INT_SRC_REG                                       *((volatile uint32_t *)0x4000A00Cu)
#define BB_INT_SRC_ADDR                                      (0x4000A00Cu)
#define BB_INT_SRC_RESET                                     (0x00000000u)
        /* RSSI_INT_SRC field */
        #define BB_INT_SRC_RSSI_INT_SRC                      (0x00000002u)
        #define BB_INT_SRC_RSSI_INT_SRC_MASK                 (0x00000002u)
        #define BB_INT_SRC_RSSI_INT_SRC_BIT                  (1)
        #define BB_INT_SRC_RSSI_INT_SRC_BITS                 (1)
        /* BASEBAND_INT_SRC field */
        #define BB_INT_SRC_BASEBAND_INT_SRC                  (0x00000001u)
        #define BB_INT_SRC_BASEBAND_INT_SRC_MASK             (0x00000001u)
        #define BB_INT_SRC_BASEBAND_INT_SRC_BIT              (0)
        #define BB_INT_SRC_BASEBAND_INT_SRC_BITS             (1)

#define SEC_INT_SRC                                          *((volatile uint32_t *)0x4000A010u)
#define SEC_INT_SRC_REG                                      *((volatile uint32_t *)0x4000A010u)
#define SEC_INT_SRC_ADDR                                     (0x4000A010u)
#define SEC_INT_SRC_RESET                                    (0x00000000u)
        /* CT_WORD_VALID_SRC field */
        #define SEC_INT_SRC_CT_WORD_VALID_SRC                (0x00000004u)
        #define SEC_INT_SRC_CT_WORD_VALID_SRC_MASK           (0x00000004u)
        #define SEC_INT_SRC_CT_WORD_VALID_SRC_BIT            (2)
        #define SEC_INT_SRC_CT_WORD_VALID_SRC_BITS           (1)
        /* PT_WORD_REQ_SRC field */
        #define SEC_INT_SRC_PT_WORD_REQ_SRC                  (0x00000002u)
        #define SEC_INT_SRC_PT_WORD_REQ_SRC_MASK             (0x00000002u)
        #define SEC_INT_SRC_PT_WORD_REQ_SRC_BIT              (1)
        #define SEC_INT_SRC_PT_WORD_REQ_SRC_BITS             (1)
        /* ENC_COMPLETE_SRC field */
        #define SEC_INT_SRC_ENC_COMPLETE_SRC                 (0x00000001u)
        #define SEC_INT_SRC_ENC_COMPLETE_SRC_MASK            (0x00000001u)
        #define SEC_INT_SRC_ENC_COMPLETE_SRC_BIT             (0)
        #define SEC_INT_SRC_ENC_COMPLETE_SRC_BITS            (1)

#define INT_SLEEPTMRFLAG                                     *((volatile uint32_t *)0x4000A014u)
#define INT_SLEEPTMRFLAG_REG                                 *((volatile uint32_t *)0x4000A014u)
#define INT_SLEEPTMRFLAG_ADDR                                (0x4000A014u)
#define INT_SLEEPTMRFLAG_RESET                               (0x00000000u)
        /* INT_SLEEPTMRCMPB field */
        #define INT_SLEEPTMRCMPB                             (0x00000004u)
        #define INT_SLEEPTMRCMPB_MASK                        (0x00000004u)
        #define INT_SLEEPTMRCMPB_BIT                         (2)
        #define INT_SLEEPTMRCMPB_BITS                        (1)
        /* INT_SLEEPTMRCMPA field */
        #define INT_SLEEPTMRCMPA                             (0x00000002u)
        #define INT_SLEEPTMRCMPA_MASK                        (0x00000002u)
        #define INT_SLEEPTMRCMPA_BIT                         (1)
        #define INT_SLEEPTMRCMPA_BITS                        (1)
        /* INT_SLEEPTMRWRAP field */
        #define INT_SLEEPTMRWRAP                             (0x00000001u)
        #define INT_SLEEPTMRWRAP_MASK                        (0x00000001u)
        #define INT_SLEEPTMRWRAP_BIT                         (0)
        #define INT_SLEEPTMRWRAP_BITS                        (1)

#define INT_MGMTFLAG                                         *((volatile uint32_t *)0x4000A018u)
#define INT_MGMTFLAG_REG                                     *((volatile uint32_t *)0x4000A018u)
#define INT_MGMTFLAG_ADDR                                    (0x4000A018u)
#define INT_MGMTFLAG_RESET                                   (0x00000000u)
        /* INT_MGMTCALADC field */
        #define INT_MGMTCALADC                               (0x00000008u)
        #define INT_MGMTCALADC_MASK                          (0x00000008u)
        #define INT_MGMTCALADC_BIT                           (3)
        #define INT_MGMTCALADC_BITS                          (1)
        /* INT_MGMTFPEC field */
        #define INT_MGMTFPEC                                 (0x00000004u)
        #define INT_MGMTFPEC_MASK                            (0x00000004u)
        #define INT_MGMTFPEC_BIT                             (2)
        #define INT_MGMTFPEC_BITS                            (1)
        /* INT_MGMTOSC24MHI field */
        #define INT_MGMTOSC24MHI                             (0x00000002u)
        #define INT_MGMTOSC24MHI_MASK                        (0x00000002u)
        #define INT_MGMTOSC24MHI_BIT                         (1)
        #define INT_MGMTOSC24MHI_BITS                        (1)
        /* INT_MGMTOSC24MLO field */
        #define INT_MGMTOSC24MLO                             (0x00000001u)
        #define INT_MGMTOSC24MLO_MASK                        (0x00000001u)
        #define INT_MGMTOSC24MLO_BIT                         (0)
        #define INT_MGMTOSC24MLO_BITS                        (1)

#define INT_NMIFLAG                                          *((volatile uint32_t *)0x4000A01Cu)
#define INT_NMIFLAG_REG                                      *((volatile uint32_t *)0x4000A01Cu)
#define INT_NMIFLAG_ADDR                                     (0x4000A01Cu)
#define INT_NMIFLAG_RESET                                    (0x00000000u)
        /* INT_NMICLK24M field */
        #define INT_NMICLK24M                                (0x00000002u)
        #define INT_NMICLK24M_MASK                           (0x00000002u)
        #define INT_NMICLK24M_BIT                            (1)
        #define INT_NMICLK24M_BITS                           (1)
        /* INT_NMIWDOG field */
        #define INT_NMIWDOG                                  (0x00000001u)
        #define INT_NMIWDOG_MASK                             (0x00000001u)
        #define INT_NMIWDOG_BIT                              (0)
        #define INT_NMIWDOG_BITS                             (1)

#define INT_SLEEPTMRFORCE                                    *((volatile uint32_t *)0x4000A020u)
#define INT_SLEEPTMRFORCE_REG                                *((volatile uint32_t *)0x4000A020u)
#define INT_SLEEPTMRFORCE_ADDR                               (0x4000A020u)
#define INT_SLEEPTMRFORCE_RESET                              (0x00000000u)
        /* INT_SLEEPTMRCMPB field */
        #define INT_SLEEPTMRCMPB                             (0x00000004u)
        #define INT_SLEEPTMRCMPB_MASK                        (0x00000004u)
        #define INT_SLEEPTMRCMPB_BIT                         (2)
        #define INT_SLEEPTMRCMPB_BITS                        (1)
        /* INT_SLEEPTMRCMPA field */
        #define INT_SLEEPTMRCMPA                             (0x00000002u)
        #define INT_SLEEPTMRCMPA_MASK                        (0x00000002u)
        #define INT_SLEEPTMRCMPA_BIT                         (1)
        #define INT_SLEEPTMRCMPA_BITS                        (1)
        /* INT_SLEEPTMRWRAP field */
        #define INT_SLEEPTMRWRAP                             (0x00000001u)
        #define INT_SLEEPTMRWRAP_MASK                        (0x00000001u)
        #define INT_SLEEPTMRWRAP_BIT                         (0)
        #define INT_SLEEPTMRWRAP_BITS                        (1)

#define TEST_FORCE_ALL_INT                                   *((volatile uint32_t *)0x4000A024u)
#define TEST_FORCE_ALL_INT_REG                               *((volatile uint32_t *)0x4000A024u)
#define TEST_FORCE_ALL_INT_ADDR                              (0x4000A024u)
#define TEST_FORCE_ALL_INT_RESET                             (0x00000000u)
        /* FORCE_ALL_INT field */
        #define TEST_FORCE_ALL_INT_FORCE_ALL_INT             (0x00000001u)
        #define TEST_FORCE_ALL_INT_FORCE_ALL_INT_MASK        (0x00000001u)
        #define TEST_FORCE_ALL_INT_FORCE_ALL_INT_BIT         (0)
        #define TEST_FORCE_ALL_INT_FORCE_ALL_INT_BITS        (1)

#define MAC_RX_INT_MASK                                      *((volatile uint32_t *)0x4000A040u)
#define MAC_RX_INT_MASK_REG                                  *((volatile uint32_t *)0x4000A040u)
#define MAC_RX_INT_MASK_ADDR                                 (0x4000A040u)
#define MAC_RX_INT_MASK_RESET                                (0x00000000u)
        /* TX_B_ACK_ERR_MSK field */
        #define MAC_RX_INT_MASK_TX_B_ACK_ERR_MSK             (0x00008000u)
        #define MAC_RX_INT_MASK_TX_B_ACK_ERR_MSK_MASK        (0x00008000u)
        #define MAC_RX_INT_MASK_TX_B_ACK_ERR_MSK_BIT         (15)
        #define MAC_RX_INT_MASK_TX_B_ACK_ERR_MSK_BITS        (1)
        /* TX_A_ACK_ERR_MSK field */
        #define MAC_RX_INT_MASK_TX_A_ACK_ERR_MSK             (0x00004000u)
        #define MAC_RX_INT_MASK_TX_A_ACK_ERR_MSK_MASK        (0x00004000u)
        #define MAC_RX_INT_MASK_TX_A_ACK_ERR_MSK_BIT         (14)
        #define MAC_RX_INT_MASK_TX_A_ACK_ERR_MSK_BITS        (1)
        /* RX_OVFLW_MSK field */
        #define MAC_RX_INT_MASK_RX_OVFLW_MSK                 (0x00002000u)
        #define MAC_RX_INT_MASK_RX_OVFLW_MSK_MASK            (0x00002000u)
        #define MAC_RX_INT_MASK_RX_OVFLW_MSK_BIT             (13)
        #define MAC_RX_INT_MASK_RX_OVFLW_MSK_BITS            (1)
        /* RX_ERROR_MSK field */
        #define MAC_RX_INT_MASK_RX_ERROR_MSK                 (0x00001000u)
        #define MAC_RX_INT_MASK_RX_ERROR_MSK_MASK            (0x00001000u)
        #define MAC_RX_INT_MASK_RX_ERROR_MSK_BIT             (12)
        #define MAC_RX_INT_MASK_RX_ERROR_MSK_BITS            (1)
        /* BB_RX_LEN_ERR_MSK field */
        #define MAC_RX_INT_MASK_BB_RX_LEN_ERR_MSK            (0x00000800u)
        #define MAC_RX_INT_MASK_BB_RX_LEN_ERR_MSK_MASK       (0x00000800u)
        #define MAC_RX_INT_MASK_BB_RX_LEN_ERR_MSK_BIT        (11)
        #define MAC_RX_INT_MASK_BB_RX_LEN_ERR_MSK_BITS       (1)
        /* TX_COLL_RX_MSK field */
        #define MAC_RX_INT_MASK_TX_COLL_RX_MSK               (0x00000400u)
        #define MAC_RX_INT_MASK_TX_COLL_RX_MSK_MASK          (0x00000400u)
        #define MAC_RX_INT_MASK_TX_COLL_RX_MSK_BIT           (10)
        #define MAC_RX_INT_MASK_TX_COLL_RX_MSK_BITS          (1)
        /* RSSI_INST_MEAS_MSK field */
        #define MAC_RX_INT_MASK_RSSI_INST_MEAS_MSK           (0x00000200u)
        #define MAC_RX_INT_MASK_RSSI_INST_MEAS_MSK_MASK      (0x00000200u)
        #define MAC_RX_INT_MASK_RSSI_INST_MEAS_MSK_BIT       (9)
        #define MAC_RX_INT_MASK_RSSI_INST_MEAS_MSK_BITS      (1)
        /* TX_B_ACK_MSK field */
        #define MAC_RX_INT_MASK_TX_B_ACK_MSK                 (0x00000100u)
        #define MAC_RX_INT_MASK_TX_B_ACK_MSK_MASK            (0x00000100u)
        #define MAC_RX_INT_MASK_TX_B_ACK_MSK_BIT             (8)
        #define MAC_RX_INT_MASK_TX_B_ACK_MSK_BITS            (1)
        /* TX_A_ACK_MSK field */
        #define MAC_RX_INT_MASK_TX_A_ACK_MSK                 (0x00000080u)
        #define MAC_RX_INT_MASK_TX_A_ACK_MSK_MASK            (0x00000080u)
        #define MAC_RX_INT_MASK_TX_A_ACK_MSK_BIT             (7)
        #define MAC_RX_INT_MASK_TX_A_ACK_MSK_BITS            (1)
        /* RX_B_UNLOAD_COMP_MSK field */
        #define MAC_RX_INT_MASK_RX_B_UNLOAD_COMP_MSK         (0x00000040u)
        #define MAC_RX_INT_MASK_RX_B_UNLOAD_COMP_MSK_MASK    (0x00000040u)
        #define MAC_RX_INT_MASK_RX_B_UNLOAD_COMP_MSK_BIT     (6)
        #define MAC_RX_INT_MASK_RX_B_UNLOAD_COMP_MSK_BITS    (1)
        /* RX_A_UNLOAD_COMP_MSK field */
        #define MAC_RX_INT_MASK_RX_A_UNLOAD_COMP_MSK         (0x00000020u)
        #define MAC_RX_INT_MASK_RX_A_UNLOAD_COMP_MSK_MASK    (0x00000020u)
        #define MAC_RX_INT_MASK_RX_A_UNLOAD_COMP_MSK_BIT     (5)
        #define MAC_RX_INT_MASK_RX_A_UNLOAD_COMP_MSK_BITS    (1)
        /* RX_B_ADDR_REC_MSK field */
        #define MAC_RX_INT_MASK_RX_B_ADDR_REC_MSK            (0x00000010u)
        #define MAC_RX_INT_MASK_RX_B_ADDR_REC_MSK_MASK       (0x00000010u)
        #define MAC_RX_INT_MASK_RX_B_ADDR_REC_MSK_BIT        (4)
        #define MAC_RX_INT_MASK_RX_B_ADDR_REC_MSK_BITS       (1)
        /* RX_A_ADDR_REC_MSK field */
        #define MAC_RX_INT_MASK_RX_A_ADDR_REC_MSK            (0x00000008u)
        #define MAC_RX_INT_MASK_RX_A_ADDR_REC_MSK_MASK       (0x00000008u)
        #define MAC_RX_INT_MASK_RX_A_ADDR_REC_MSK_BIT        (3)
        #define MAC_RX_INT_MASK_RX_A_ADDR_REC_MSK_BITS       (1)
        /* RX_B_FILT_COMP_MSK field */
        #define MAC_RX_INT_MASK_RX_B_FILT_COMP_MSK           (0x00000004u)
        #define MAC_RX_INT_MASK_RX_B_FILT_COMP_MSK_MASK      (0x00000004u)
        #define MAC_RX_INT_MASK_RX_B_FILT_COMP_MSK_BIT       (2)
        #define MAC_RX_INT_MASK_RX_B_FILT_COMP_MSK_BITS      (1)
        /* RX_A_FILT_COMP_MSK field */
        #define MAC_RX_INT_MASK_RX_A_FILT_COMP_MSK           (0x00000002u)
        #define MAC_RX_INT_MASK_RX_A_FILT_COMP_MSK_MASK      (0x00000002u)
        #define MAC_RX_INT_MASK_RX_A_FILT_COMP_MSK_BIT       (1)
        #define MAC_RX_INT_MASK_RX_A_FILT_COMP_MSK_BITS      (1)
        /* RX_FRAME_MSK field */
        #define MAC_RX_INT_MASK_RX_FRAME_MSK                 (0x00000001u)
        #define MAC_RX_INT_MASK_RX_FRAME_MSK_MASK            (0x00000001u)
        #define MAC_RX_INT_MASK_RX_FRAME_MSK_BIT             (0)
        #define MAC_RX_INT_MASK_RX_FRAME_MSK_BITS            (1)

#define MAC_TX_INT_MASK                                      *((volatile uint32_t *)0x4000A044u)
#define MAC_TX_INT_MASK_REG                                  *((volatile uint32_t *)0x4000A044u)
#define MAC_TX_INT_MASK_ADDR                                 (0x4000A044u)
#define MAC_TX_INT_MASK_RESET                                (0x00000000u)
        /* RX_B_ACK_MSK field */
        #define MAC_TX_INT_MASK_RX_B_ACK_MSK                 (0x00000800u)
        #define MAC_TX_INT_MASK_RX_B_ACK_MSK_MASK            (0x00000800u)
        #define MAC_TX_INT_MASK_RX_B_ACK_MSK_BIT             (11)
        #define MAC_TX_INT_MASK_RX_B_ACK_MSK_BITS            (1)
        /* RX_A_ACK_MSK field */
        #define MAC_TX_INT_MASK_RX_A_ACK_MSK                 (0x00000400u)
        #define MAC_TX_INT_MASK_RX_A_ACK_MSK_MASK            (0x00000400u)
        #define MAC_TX_INT_MASK_RX_A_ACK_MSK_BIT             (10)
        #define MAC_TX_INT_MASK_RX_A_ACK_MSK_BITS            (1)
        /* TX_B_UNLOAD_MSK field */
        #define MAC_TX_INT_MASK_TX_B_UNLOAD_MSK              (0x00000200u)
        #define MAC_TX_INT_MASK_TX_B_UNLOAD_MSK_MASK         (0x00000200u)
        #define MAC_TX_INT_MASK_TX_B_UNLOAD_MSK_BIT          (9)
        #define MAC_TX_INT_MASK_TX_B_UNLOAD_MSK_BITS         (1)
        /* TX_A_UNLOAD_MSK field */
        #define MAC_TX_INT_MASK_TX_A_UNLOAD_MSK              (0x00000100u)
        #define MAC_TX_INT_MASK_TX_A_UNLOAD_MSK_MASK         (0x00000100u)
        #define MAC_TX_INT_MASK_TX_A_UNLOAD_MSK_BIT          (8)
        #define MAC_TX_INT_MASK_TX_A_UNLOAD_MSK_BITS         (1)
        /* ACK_EXPIRED_MSK field */
        #define MAC_TX_INT_MASK_ACK_EXPIRED_MSK              (0x00000080u)
        #define MAC_TX_INT_MASK_ACK_EXPIRED_MSK_MASK         (0x00000080u)
        #define MAC_TX_INT_MASK_ACK_EXPIRED_MSK_BIT          (7)
        #define MAC_TX_INT_MASK_ACK_EXPIRED_MSK_BITS         (1)
        /* TX_LOCK_FAIL_MSK field */
        #define MAC_TX_INT_MASK_TX_LOCK_FAIL_MSK             (0x00000040u)
        #define MAC_TX_INT_MASK_TX_LOCK_FAIL_MSK_MASK        (0x00000040u)
        #define MAC_TX_INT_MASK_TX_LOCK_FAIL_MSK_BIT         (6)
        #define MAC_TX_INT_MASK_TX_LOCK_FAIL_MSK_BITS        (1)
        /* TX_UNDERFLOW_MSK field */
        #define MAC_TX_INT_MASK_TX_UNDERFLOW_MSK             (0x00000020u)
        #define MAC_TX_INT_MASK_TX_UNDERFLOW_MSK_MASK        (0x00000020u)
        #define MAC_TX_INT_MASK_TX_UNDERFLOW_MSK_BIT         (5)
        #define MAC_TX_INT_MASK_TX_UNDERFLOW_MSK_BITS        (1)
        /* CCA_FAIL_MSK field */
        #define MAC_TX_INT_MASK_CCA_FAIL_MSK                 (0x00000010u)
        #define MAC_TX_INT_MASK_CCA_FAIL_MSK_MASK            (0x00000010u)
        #define MAC_TX_INT_MASK_CCA_FAIL_MSK_BIT             (4)
        #define MAC_TX_INT_MASK_CCA_FAIL_MSK_BITS            (1)
        /* SFD_SENT_MSK field */
        #define MAC_TX_INT_MASK_SFD_SENT_MSK                 (0x00000008u)
        #define MAC_TX_INT_MASK_SFD_SENT_MSK_MASK            (0x00000008u)
        #define MAC_TX_INT_MASK_SFD_SENT_MSK_BIT             (3)
        #define MAC_TX_INT_MASK_SFD_SENT_MSK_BITS            (1)
        /* BO_COMPLETE_MSK field */
        #define MAC_TX_INT_MASK_BO_COMPLETE_MSK              (0x00000004u)
        #define MAC_TX_INT_MASK_BO_COMPLETE_MSK_MASK         (0x00000004u)
        #define MAC_TX_INT_MASK_BO_COMPLETE_MSK_BIT          (2)
        #define MAC_TX_INT_MASK_BO_COMPLETE_MSK_BITS         (1)
        /* RX_ACK_MSK field */
        #define MAC_TX_INT_MASK_RX_ACK_MSK                   (0x00000002u)
        #define MAC_TX_INT_MASK_RX_ACK_MSK_MASK              (0x00000002u)
        #define MAC_TX_INT_MASK_RX_ACK_MSK_BIT               (1)
        #define MAC_TX_INT_MASK_RX_ACK_MSK_BITS              (1)
        /* TX_COMPLETE_MSK field */
        #define MAC_TX_INT_MASK_TX_COMPLETE_MSK              (0x00000001u)
        #define MAC_TX_INT_MASK_TX_COMPLETE_MSK_MASK         (0x00000001u)
        #define MAC_TX_INT_MASK_TX_COMPLETE_MSK_BIT          (0)
        #define MAC_TX_INT_MASK_TX_COMPLETE_MSK_BITS         (1)

#define MAC_TIMER_INT_MASK                                   *((volatile uint32_t *)0x4000A048u)
#define MAC_TIMER_INT_MASK_REG                               *((volatile uint32_t *)0x4000A048u)
#define MAC_TIMER_INT_MASK_ADDR                              (0x4000A048u)
#define MAC_TIMER_INT_MASK_RESET                             (0x00000000u)
        /* TIMER_COMP_B_MSK field */
        #define MAC_TIMER_INT_MASK_TIMER_COMP_B_MSK          (0x00000004u)
        #define MAC_TIMER_INT_MASK_TIMER_COMP_B_MSK_MASK     (0x00000004u)
        #define MAC_TIMER_INT_MASK_TIMER_COMP_B_MSK_BIT      (2)
        #define MAC_TIMER_INT_MASK_TIMER_COMP_B_MSK_BITS     (1)
        /* TIMER_COMP_A_MSK field */
        #define MAC_TIMER_INT_MASK_TIMER_COMP_A_MSK          (0x00000002u)
        #define MAC_TIMER_INT_MASK_TIMER_COMP_A_MSK_MASK     (0x00000002u)
        #define MAC_TIMER_INT_MASK_TIMER_COMP_A_MSK_BIT      (1)
        #define MAC_TIMER_INT_MASK_TIMER_COMP_A_MSK_BITS     (1)
        /* TIMER_WRAP_MSK field */
        #define MAC_TIMER_INT_MASK_TIMER_WRAP_MSK            (0x00000001u)
        #define MAC_TIMER_INT_MASK_TIMER_WRAP_MSK_MASK       (0x00000001u)
        #define MAC_TIMER_INT_MASK_TIMER_WRAP_MSK_BIT        (0)
        #define MAC_TIMER_INT_MASK_TIMER_WRAP_MSK_BITS       (1)

#define BB_INT_MASK                                          *((volatile uint32_t *)0x4000A04Cu)
#define BB_INT_MASK_REG                                      *((volatile uint32_t *)0x4000A04Cu)
#define BB_INT_MASK_ADDR                                     (0x4000A04Cu)
#define BB_INT_MASK_RESET                                    (0x00000000u)
        /* RSSI_INT_MSK field */
        #define BB_INT_MASK_RSSI_INT_MSK                     (0x00000002u)
        #define BB_INT_MASK_RSSI_INT_MSK_MASK                (0x00000002u)
        #define BB_INT_MASK_RSSI_INT_MSK_BIT                 (1)
        #define BB_INT_MASK_RSSI_INT_MSK_BITS                (1)
        /* BASEBAND_INT_MSK field */
        #define BB_INT_MASK_BASEBAND_INT_MSK                 (0x00000001u)
        #define BB_INT_MASK_BASEBAND_INT_MSK_MASK            (0x00000001u)
        #define BB_INT_MASK_BASEBAND_INT_MSK_BIT             (0)
        #define BB_INT_MASK_BASEBAND_INT_MSK_BITS            (1)

#define SEC_INT_MASK                                         *((volatile uint32_t *)0x4000A050u)
#define SEC_INT_MASK_REG                                     *((volatile uint32_t *)0x4000A050u)
#define SEC_INT_MASK_ADDR                                    (0x4000A050u)
#define SEC_INT_MASK_RESET                                   (0x00000000u)
        /* CT_WORD_VALID_MSK field */
        #define SEC_INT_MASK_CT_WORD_VALID_MSK               (0x00000004u)
        #define SEC_INT_MASK_CT_WORD_VALID_MSK_MASK          (0x00000004u)
        #define SEC_INT_MASK_CT_WORD_VALID_MSK_BIT           (2)
        #define SEC_INT_MASK_CT_WORD_VALID_MSK_BITS          (1)
        /* PT_WORD_REQ_MSK field */
        #define SEC_INT_MASK_PT_WORD_REQ_MSK                 (0x00000002u)
        #define SEC_INT_MASK_PT_WORD_REQ_MSK_MASK            (0x00000002u)
        #define SEC_INT_MASK_PT_WORD_REQ_MSK_BIT             (1)
        #define SEC_INT_MASK_PT_WORD_REQ_MSK_BITS            (1)
        /* ENC_COMPLETE_MSK field */
        #define SEC_INT_MASK_ENC_COMPLETE_MSK                (0x00000001u)
        #define SEC_INT_MASK_ENC_COMPLETE_MSK_MASK           (0x00000001u)
        #define SEC_INT_MASK_ENC_COMPLETE_MSK_BIT            (0)
        #define SEC_INT_MASK_ENC_COMPLETE_MSK_BITS           (1)

#define INT_SLEEPTMRCFG                                      *((volatile uint32_t *)0x4000A054u)
#define INT_SLEEPTMRCFG_REG                                  *((volatile uint32_t *)0x4000A054u)
#define INT_SLEEPTMRCFG_ADDR                                 (0x4000A054u)
#define INT_SLEEPTMRCFG_RESET                                (0x00000000u)
        /* INT_SLEEPTMRCMPB field */
        #define INT_SLEEPTMRCMPB                             (0x00000004u)
        #define INT_SLEEPTMRCMPB_MASK                        (0x00000004u)
        #define INT_SLEEPTMRCMPB_BIT                         (2)
        #define INT_SLEEPTMRCMPB_BITS                        (1)
        /* INT_SLEEPTMRCMPA field */
        #define INT_SLEEPTMRCMPA                             (0x00000002u)
        #define INT_SLEEPTMRCMPA_MASK                        (0x00000002u)
        #define INT_SLEEPTMRCMPA_BIT                         (1)
        #define INT_SLEEPTMRCMPA_BITS                        (1)
        /* INT_SLEEPTMRWRAP field */
        #define INT_SLEEPTMRWRAP                             (0x00000001u)
        #define INT_SLEEPTMRWRAP_MASK                        (0x00000001u)
        #define INT_SLEEPTMRWRAP_BIT                         (0)
        #define INT_SLEEPTMRWRAP_BITS                        (1)

#define INT_MGMTCFG                                          *((volatile uint32_t *)0x4000A058u)
#define INT_MGMTCFG_REG                                      *((volatile uint32_t *)0x4000A058u)
#define INT_MGMTCFG_ADDR                                     (0x4000A058u)
#define INT_MGMTCFG_RESET                                    (0x00000000u)
        /* INT_MGMTCALADC field */
        #define INT_MGMTCALADC                               (0x00000008u)
        #define INT_MGMTCALADC_MASK                          (0x00000008u)
        #define INT_MGMTCALADC_BIT                           (3)
        #define INT_MGMTCALADC_BITS                          (1)
        /* INT_MGMTFPEC field */
        #define INT_MGMTFPEC                                 (0x00000004u)
        #define INT_MGMTFPEC_MASK                            (0x00000004u)
        #define INT_MGMTFPEC_BIT                             (2)
        #define INT_MGMTFPEC_BITS                            (1)
        /* INT_MGMTOSC24MHI field */
        #define INT_MGMTOSC24MHI                             (0x00000002u)
        #define INT_MGMTOSC24MHI_MASK                        (0x00000002u)
        #define INT_MGMTOSC24MHI_BIT                         (1)
        #define INT_MGMTOSC24MHI_BITS                        (1)
        /* INT_MGMTOSC24MLO field */
        #define INT_MGMTOSC24MLO                             (0x00000001u)
        #define INT_MGMTOSC24MLO_MASK                        (0x00000001u)
        #define INT_MGMTOSC24MLO_BIT                         (0)
        #define INT_MGMTOSC24MLO_BITS                        (1)

/* GPIO block */
#define BLOCK_GPIO_BASE                                      (0x4000B000u)
#define BLOCK_GPIO_END                                       (0x4000BC2Cu)
#define BLOCK_GPIO_SIZE                                      (BLOCK_GPIO_END - BLOCK_GPIO_BASE + 1)

#define GPIO_PDCFGL                                          *((volatile uint32_t *)0x4000B600u)
#define GPIO_PDCFGL_REG                                      *((volatile uint32_t *)0x4000B600u)
#define GPIO_PDCFGL_ADDR                                     (0x4000B600u)
#define GPIO_PDCFGL_RESET                                    (0x00004444u)
        /* PD3_CFG field */
        #define PD3_CFG                                      (0x0000F000u)
        #define PD3_CFG_MASK                                 (0x0000F000u)
        #define PD3_CFG_BIT                                  (12)
        #define PD3_CFG_BITS                                 (4)
        /* PD2_CFG field */
        #define PD2_CFG                                      (0x00000F00u)
        #define PD2_CFG_MASK                                 (0x00000F00u)
        #define PD2_CFG_BIT                                  (8)
        #define PD2_CFG_BITS                                 (4)
        /* PD1_CFG field */
        #define PD1_CFG                                      (0x000000F0u)
        #define PD1_CFG_MASK                                 (0x000000F0u)
        #define PD1_CFG_BIT                                  (4)
        #define PD1_CFG_BITS                                 (4)
        /* PD0_CFG field */
        #define PD0_CFG                                      (0x0000000Fu)
        #define PD0_CFG_MASK                                 (0x0000000Fu)
        #define PD0_CFG_BIT                                  (0)
        #define PD0_CFG_BITS                                 (4)

#define GPIO_PDCFGH                                          *((volatile uint32_t *)0x4000B604u)
#define GPIO_PDCFGH_REG                                      *((volatile uint32_t *)0x4000B604u)
#define GPIO_PDCFGH_ADDR                                     (0x4000B604u)
#define GPIO_PDCFGH_RESET                                    (0x00004444u)
        /* PD7_CFG field */
        #define PD7_CFG                                      (0x0000F000u)
        #define PD7_CFG_MASK                                 (0x0000F000u)
        #define PD7_CFG_BIT                                  (12)
        #define PD7_CFG_BITS                                 (4)
        /* PD6_CFG field */
        #define PD6_CFG                                      (0x00000F00u)
        #define PD6_CFG_MASK                                 (0x00000F00u)
        #define PD6_CFG_BIT                                  (8)
        #define PD6_CFG_BITS                                 (4)
        /* PD5_CFG field */
        #define PD5_CFG                                      (0x000000F0u)
        #define PD5_CFG_MASK                                 (0x000000F0u)
        #define PD5_CFG_BIT                                  (4)
        #define PD5_CFG_BITS                                 (4)
        /* PD4_CFG field */
        #define PD4_CFG                                      (0x0000000Fu)
        #define PD4_CFG_MASK                                 (0x0000000Fu)
        #define PD4_CFG_BIT                                  (0)
        #define PD4_CFG_BITS                                 (4)

#define GPIO_PDIN                                            *((volatile uint32_t *)0x4000B608u)
#define GPIO_PDIN_REG                                        *((volatile uint32_t *)0x4000B608u)
#define GPIO_PDIN_ADDR                                       (0x4000B608u)
#define GPIO_PDIN_RESET                                      (0x00000000u)
        /* PD7 field */
        #define PD7                                          (0x00000080u)
        #define PD7_MASK                                     (0x00000080u)
        #define PD7_BIT                                      (7)
        #define PD7_BITS                                     (1)
        /* PD6 field */
        #define PD6                                          (0x00000040u)
        #define PD6_MASK                                     (0x00000040u)
        #define PD6_BIT                                      (6)
        #define PD6_BITS                                     (1)
        /* PD5 field */
        #define PD5                                          (0x00000020u)
        #define PD5_MASK                                     (0x00000020u)
        #define PD5_BIT                                      (5)
        #define PD5_BITS                                     (1)
        /* PD4 field */
        #define PD4                                          (0x00000010u)
        #define PD4_MASK                                     (0x00000010u)
        #define PD4_BIT                                      (4)
        #define PD4_BITS                                     (1)
        /* PD3 field */
        #define PD3                                          (0x00000008u)
        #define PD3_MASK                                     (0x00000008u)
        #define PD3_BIT                                      (3)
        #define PD3_BITS                                     (1)
        /* PD2 field */
        #define PD2                                          (0x00000004u)
        #define PD2_MASK                                     (0x00000004u)
        #define PD2_BIT                                      (2)
        #define PD2_BITS                                     (1)
        /* PD1 field */
        #define PD1                                          (0x00000002u)
        #define PD1_MASK                                     (0x00000002u)
        #define PD1_BIT                                      (1)
        #define PD1_BITS                                     (1)
        /* PD0 field */
        #define PD0                                          (0x00000001u)
        #define PD0_MASK                                     (0x00000001u)
        #define PD0_BIT                                      (0)
        #define PD0_BITS                                     (1)

#define GPIO_PDOUT                                           *((volatile uint32_t *)0x4000B60Cu)
#define GPIO_PDOUT_REG                                       *((volatile uint32_t *)0x4000B60Cu)
#define GPIO_PDOUT_ADDR                                      (0x4000B60Cu)
#define GPIO_PDOUT_RESET                                     (0x00000000u)
        /* PD7 field */
        #define PD7                                          (0x00000080u)
        #define PD7_MASK                                     (0x00000080u)
        #define PD7_BIT                                      (7)
        #define PD7_BITS                                     (1)
        /* PD6 field */
        #define PD6                                          (0x00000040u)
        #define PD6_MASK                                     (0x00000040u)
        #define PD6_BIT                                      (6)
        #define PD6_BITS                                     (1)
        /* PD5 field */
        #define PD5                                          (0x00000020u)
        #define PD5_MASK                                     (0x00000020u)
        #define PD5_BIT                                      (5)
        #define PD5_BITS                                     (1)
        /* PD4 field */
        #define PD4                                          (0x00000010u)
        #define PD4_MASK                                     (0x00000010u)
        #define PD4_BIT                                      (4)
        #define PD4_BITS                                     (1)
        /* PD3 field */
        #define PD3                                          (0x00000008u)
        #define PD3_MASK                                     (0x00000008u)
        #define PD3_BIT                                      (3)
        #define PD3_BITS                                     (1)
        /* PD2 field */
        #define PD2                                          (0x00000004u)
        #define PD2_MASK                                     (0x00000004u)
        #define PD2_BIT                                      (2)
        #define PD2_BITS                                     (1)
        /* PD1 field */
        #define PD1                                          (0x00000002u)
        #define PD1_MASK                                     (0x00000002u)
        #define PD1_BIT                                      (1)
        #define PD1_BITS                                     (1)
        /* PD0 field */
        #define PD0                                          (0x00000001u)
        #define PD0_MASK                                     (0x00000001u)
        #define PD0_BIT                                      (0)
        #define PD0_BITS                                     (1)

#define GPIO_PDSET                                           *((volatile uint32_t *)0x4000B610u)
#define GPIO_PDSET_REG                                       *((volatile uint32_t *)0x4000B610u)
#define GPIO_PDSET_ADDR                                      (0x4000B610u)
#define GPIO_PDSET_RESET                                     (0x00000000u)
        /* GPIO_PXSETRSVD field */
        #define GPIO_PXSETRSVD                               (0x0000FF00u)
        #define GPIO_PXSETRSVD_MASK                          (0x0000FF00u)
        #define GPIO_PXSETRSVD_BIT                           (8)
        #define GPIO_PXSETRSVD_BITS                          (8)
        /* PD7 field */
        #define PD7                                          (0x00000080u)
        #define PD7_MASK                                     (0x00000080u)
        #define PD7_BIT                                      (7)
        #define PD7_BITS                                     (1)
        /* PD6 field */
        #define PD6                                          (0x00000040u)
        #define PD6_MASK                                     (0x00000040u)
        #define PD6_BIT                                      (6)
        #define PD6_BITS                                     (1)
        /* PD5 field */
        #define PD5                                          (0x00000020u)
        #define PD5_MASK                                     (0x00000020u)
        #define PD5_BIT                                      (5)
        #define PD5_BITS                                     (1)
        /* PD4 field */
        #define PD4                                          (0x00000010u)
        #define PD4_MASK                                     (0x00000010u)
        #define PD4_BIT                                      (4)
        #define PD4_BITS                                     (1)
        /* PD3 field */
        #define PD3                                          (0x00000008u)
        #define PD3_MASK                                     (0x00000008u)
        #define PD3_BIT                                      (3)
        #define PD3_BITS                                     (1)
        /* PD2 field */
        #define PD2                                          (0x00000004u)
        #define PD2_MASK                                     (0x00000004u)
        #define PD2_BIT                                      (2)
        #define PD2_BITS                                     (1)
        /* PD1 field */
        #define PD1                                          (0x00000002u)
        #define PD1_MASK                                     (0x00000002u)
        #define PD1_BIT                                      (1)
        #define PD1_BITS                                     (1)
        /* PD0 field */
        #define PD0                                          (0x00000001u)
        #define PD0_MASK                                     (0x00000001u)
        #define PD0_BIT                                      (0)
        #define PD0_BITS                                     (1)

#define GPIO_PDCLR                                           *((volatile uint32_t *)0x4000B614u)
#define GPIO_PDCLR_REG                                       *((volatile uint32_t *)0x4000B614u)
#define GPIO_PDCLR_ADDR                                      (0x4000B614u)
#define GPIO_PDCLR_RESET                                     (0x00000000u)
        /* PD7 field */
        #define PD7                                          (0x00000080u)
        #define PD7_MASK                                     (0x00000080u)
        #define PD7_BIT                                      (7)
        #define PD7_BITS                                     (1)
        /* PD6 field */
        #define PD6                                          (0x00000040u)
        #define PD6_MASK                                     (0x00000040u)
        #define PD6_BIT                                      (6)
        #define PD6_BITS                                     (1)
        /* PD5 field */
        #define PD5                                          (0x00000020u)
        #define PD5_MASK                                     (0x00000020u)
        #define PD5_BIT                                      (5)
        #define PD5_BITS                                     (1)
        /* PD4 field */
        #define PD4                                          (0x00000010u)
        #define PD4_MASK                                     (0x00000010u)
        #define PD4_BIT                                      (4)
        #define PD4_BITS                                     (1)
        /* PD3 field */
        #define PD3                                          (0x00000008u)
        #define PD3_MASK                                     (0x00000008u)
        #define PD3_BIT                                      (3)
        #define PD3_BITS                                     (1)
        /* PD2 field */
        #define PD2                                          (0x00000004u)
        #define PD2_MASK                                     (0x00000004u)
        #define PD2_BIT                                      (2)
        #define PD2_BITS                                     (1)
        /* PD1 field */
        #define PD1                                          (0x00000002u)
        #define PD1_MASK                                     (0x00000002u)
        #define PD1_BIT                                      (1)
        #define PD1_BITS                                     (1)
        /* PD0 field */
        #define PD0                                          (0x00000001u)
        #define PD0_MASK                                     (0x00000001u)
        #define PD0_BIT                                      (0)
        #define PD0_BITS                                     (1)

#define GPIO_PECFGL                                          *((volatile uint32_t *)0x4000B800u)
#define GPIO_PECFGL_REG                                      *((volatile uint32_t *)0x4000B800u)
#define GPIO_PECFGL_ADDR                                     (0x4000B800u)
#define GPIO_PECFGL_RESET                                    (0x00004444u)
        /* PE3_CFG field */
        #define PE3_CFG                                      (0x0000F000u)
        #define PE3_CFG_MASK                                 (0x0000F000u)
        #define PE3_CFG_BIT                                  (12)
        #define PE3_CFG_BITS                                 (4)
        /* PE2_CFG field */
        #define PE2_CFG                                      (0x00000F00u)
        #define PE2_CFG_MASK                                 (0x00000F00u)
        #define PE2_CFG_BIT                                  (8)
        #define PE2_CFG_BITS                                 (4)
        /* PE1_CFG field */
        #define PE1_CFG                                      (0x000000F0u)
        #define PE1_CFG_MASK                                 (0x000000F0u)
        #define PE1_CFG_BIT                                  (4)
        #define PE1_CFG_BITS                                 (4)
        /* PE0_CFG field */
        #define PE0_CFG                                      (0x0000000Fu)
        #define PE0_CFG_MASK                                 (0x0000000Fu)
        #define PE0_CFG_BIT                                  (0)
        #define PE0_CFG_BITS                                 (4)

#define GPIO_PECFGH                                          *((volatile uint32_t *)0x4000B804u)
#define GPIO_PECFGH_REG                                      *((volatile uint32_t *)0x4000B804u)
#define GPIO_PECFGH_ADDR                                     (0x4000B804u)
#define GPIO_PECFGH_RESET                                    (0x00004444u)
        /* PE7_CFG field */
        #define PE7_CFG                                      (0x0000F000u)
        #define PE7_CFG_MASK                                 (0x0000F000u)
        #define PE7_CFG_BIT                                  (12)
        #define PE7_CFG_BITS                                 (4)
        /* PE6_CFG field */
        #define PE6_CFG                                      (0x00000F00u)
        #define PE6_CFG_MASK                                 (0x00000F00u)
        #define PE6_CFG_BIT                                  (8)
        #define PE6_CFG_BITS                                 (4)
        /* PE5_CFG field */
        #define PE5_CFG                                      (0x000000F0u)
        #define PE5_CFG_MASK                                 (0x000000F0u)
        #define PE5_CFG_BIT                                  (4)
        #define PE5_CFG_BITS                                 (4)
        /* PE4_CFG field */
        #define PE4_CFG                                      (0x0000000Fu)
        #define PE4_CFG_MASK                                 (0x0000000Fu)
        #define PE4_CFG_BIT                                  (0)
        #define PE4_CFG_BITS                                 (4)

#define GPIO_PEIN                                            *((volatile uint32_t *)0x4000B808u)
#define GPIO_PEIN_REG                                        *((volatile uint32_t *)0x4000B808u)
#define GPIO_PEIN_ADDR                                       (0x4000B808u)
#define GPIO_PEIN_RESET                                      (0x00000000u)
        /* PE7 field */
        #define PE7                                          (0x00000080u)
        #define PE7_MASK                                     (0x00000080u)
        #define PE7_BIT                                      (7)
        #define PE7_BITS                                     (1)
        /* PE6 field */
        #define PE6                                          (0x00000040u)
        #define PE6_MASK                                     (0x00000040u)
        #define PE6_BIT                                      (6)
        #define PE6_BITS                                     (1)
        /* PE5 field */
        #define PE5                                          (0x00000020u)
        #define PE5_MASK                                     (0x00000020u)
        #define PE5_BIT                                      (5)
        #define PE5_BITS                                     (1)
        /* PE4 field */
        #define PE4                                          (0x00000010u)
        #define PE4_MASK                                     (0x00000010u)
        #define PE4_BIT                                      (4)
        #define PE4_BITS                                     (1)
        /* PE3 field */
        #define PE3                                          (0x00000008u)
        #define PE3_MASK                                     (0x00000008u)
        #define PE3_BIT                                      (3)
        #define PE3_BITS                                     (1)
        /* PE2 field */
        #define PE2                                          (0x00000004u)
        #define PE2_MASK                                     (0x00000004u)
        #define PE2_BIT                                      (2)
        #define PE2_BITS                                     (1)
        /* PE1 field */
        #define PE1                                          (0x00000002u)
        #define PE1_MASK                                     (0x00000002u)
        #define PE1_BIT                                      (1)
        #define PE1_BITS                                     (1)
        /* PE0 field */
        #define PE0                                          (0x00000001u)
        #define PE0_MASK                                     (0x00000001u)
        #define PE0_BIT                                      (0)
        #define PE0_BITS                                     (1)

#define GPIO_PEOUT                                           *((volatile uint32_t *)0x4000B80Cu)
#define GPIO_PEOUT_REG                                       *((volatile uint32_t *)0x4000B80Cu)
#define GPIO_PEOUT_ADDR                                      (0x4000B80Cu)
#define GPIO_PEOUT_RESET                                     (0x00000000u)
        /* PE7 field */
        #define PE7                                          (0x00000080u)
        #define PE7_MASK                                     (0x00000080u)
        #define PE7_BIT                                      (7)
        #define PE7_BITS                                     (1)
        /* PE6 field */
        #define PE6                                          (0x00000040u)
        #define PE6_MASK                                     (0x00000040u)
        #define PE6_BIT                                      (6)
        #define PE6_BITS                                     (1)
        /* PE5 field */
        #define PE5                                          (0x00000020u)
        #define PE5_MASK                                     (0x00000020u)
        #define PE5_BIT                                      (5)
        #define PE5_BITS                                     (1)
        /* PE4 field */
        #define PE4                                          (0x00000010u)
        #define PE4_MASK                                     (0x00000010u)
        #define PE4_BIT                                      (4)
        #define PE4_BITS                                     (1)
        /* PE3 field */
        #define PE3                                          (0x00000008u)
        #define PE3_MASK                                     (0x00000008u)
        #define PE3_BIT                                      (3)
        #define PE3_BITS                                     (1)
        /* PE2 field */
        #define PE2                                          (0x00000004u)
        #define PE2_MASK                                     (0x00000004u)
        #define PE2_BIT                                      (2)
        #define PE2_BITS                                     (1)
        /* PE1 field */
        #define PE1                                          (0x00000002u)
        #define PE1_MASK                                     (0x00000002u)
        #define PE1_BIT                                      (1)
        #define PE1_BITS                                     (1)
        /* PE0 field */
        #define PE0                                          (0x00000001u)
        #define PE0_MASK                                     (0x00000001u)
        #define PE0_BIT                                      (0)
        #define PE0_BITS                                     (1)

#define GPIO_PESET                                           *((volatile uint32_t *)0x4000B810u)
#define GPIO_PESET_REG                                       *((volatile uint32_t *)0x4000B810u)
#define GPIO_PESET_ADDR                                      (0x4000B810u)
#define GPIO_PESET_RESET                                     (0x00000000u)
        /* GPIO_PXSETRSVD field */
        #define GPIO_PXSETRSVD                               (0x0000FF00u)
        #define GPIO_PXSETRSVD_MASK                          (0x0000FF00u)
        #define GPIO_PXSETRSVD_BIT                           (8)
        #define GPIO_PXSETRSVD_BITS                          (8)
        /* PE7 field */
        #define PE7                                          (0x00000080u)
        #define PE7_MASK                                     (0x00000080u)
        #define PE7_BIT                                      (7)
        #define PE7_BITS                                     (1)
        /* PE6 field */
        #define PE6                                          (0x00000040u)
        #define PE6_MASK                                     (0x00000040u)
        #define PE6_BIT                                      (6)
        #define PE6_BITS                                     (1)
        /* PE5 field */
        #define PE5                                          (0x00000020u)
        #define PE5_MASK                                     (0x00000020u)
        #define PE5_BIT                                      (5)
        #define PE5_BITS                                     (1)
        /* PE4 field */
        #define PE4                                          (0x00000010u)
        #define PE4_MASK                                     (0x00000010u)
        #define PE4_BIT                                      (4)
        #define PE4_BITS                                     (1)
        /* PE3 field */
        #define PE3                                          (0x00000008u)
        #define PE3_MASK                                     (0x00000008u)
        #define PE3_BIT                                      (3)
        #define PE3_BITS                                     (1)
        /* PE2 field */
        #define PE2                                          (0x00000004u)
        #define PE2_MASK                                     (0x00000004u)
        #define PE2_BIT                                      (2)
        #define PE2_BITS                                     (1)
        /* PE1 field */
        #define PE1                                          (0x00000002u)
        #define PE1_MASK                                     (0x00000002u)
        #define PE1_BIT                                      (1)
        #define PE1_BITS                                     (1)
        /* PE0 field */
        #define PE0                                          (0x00000001u)
        #define PE0_MASK                                     (0x00000001u)
        #define PE0_BIT                                      (0)
        #define PE0_BITS                                     (1)

#define GPIO_PECLR                                           *((volatile uint32_t *)0x4000B814u)
#define GPIO_PECLR_REG                                       *((volatile uint32_t *)0x4000B814u)
#define GPIO_PECLR_ADDR                                      (0x4000B814u)
#define GPIO_PECLR_RESET                                     (0x00000000u)
        /* PE7 field */
        #define PE7                                          (0x00000080u)
        #define PE7_MASK                                     (0x00000080u)
        #define PE7_BIT                                      (7)
        #define PE7_BITS                                     (1)
        /* PE6 field */
        #define PE6                                          (0x00000040u)
        #define PE6_MASK                                     (0x00000040u)
        #define PE6_BIT                                      (6)
        #define PE6_BITS                                     (1)
        /* PE5 field */
        #define PE5                                          (0x00000020u)
        #define PE5_MASK                                     (0x00000020u)
        #define PE5_BIT                                      (5)
        #define PE5_BITS                                     (1)
        /* PE4 field */
        #define PE4                                          (0x00000010u)
        #define PE4_MASK                                     (0x00000010u)
        #define PE4_BIT                                      (4)
        #define PE4_BITS                                     (1)
        /* PE3 field */
        #define PE3                                          (0x00000008u)
        #define PE3_MASK                                     (0x00000008u)
        #define PE3_BIT                                      (3)
        #define PE3_BITS                                     (1)
        /* PE2 field */
        #define PE2                                          (0x00000004u)
        #define PE2_MASK                                     (0x00000004u)
        #define PE2_BIT                                      (2)
        #define PE2_BITS                                     (1)
        /* PE1 field */
        #define PE1                                          (0x00000002u)
        #define PE1_MASK                                     (0x00000002u)
        #define PE1_BIT                                      (1)
        #define PE1_BITS                                     (1)
        /* PE0 field */
        #define PE0                                          (0x00000001u)
        #define PE0_MASK                                     (0x00000001u)
        #define PE0_BIT                                      (0)
        #define PE0_BITS                                     (1)

#define GPIO_PFCFGL                                          *((volatile uint32_t *)0x4000BA00u)
#define GPIO_PFCFGL_REG                                      *((volatile uint32_t *)0x4000BA00u)
#define GPIO_PFCFGL_ADDR                                     (0x4000BA00u)
#define GPIO_PFCFGL_RESET                                    (0x00004444u)
        /* PF3_CFG field */
        #define PF3_CFG                                      (0x0000F000u)
        #define PF3_CFG_MASK                                 (0x0000F000u)
        #define PF3_CFG_BIT                                  (12)
        #define PF3_CFG_BITS                                 (4)
        /* PF2_CFG field */
        #define PF2_CFG                                      (0x00000F00u)
        #define PF2_CFG_MASK                                 (0x00000F00u)
        #define PF2_CFG_BIT                                  (8)
        #define PF2_CFG_BITS                                 (4)
        /* PF1_CFG field */
        #define PF1_CFG                                      (0x000000F0u)
        #define PF1_CFG_MASK                                 (0x000000F0u)
        #define PF1_CFG_BIT                                  (4)
        #define PF1_CFG_BITS                                 (4)
        /* PF0_CFG field */
        #define PF0_CFG                                      (0x0000000Fu)
        #define PF0_CFG_MASK                                 (0x0000000Fu)
        #define PF0_CFG_BIT                                  (0)
        #define PF0_CFG_BITS                                 (4)

#define GPIO_PFCFGH                                          *((volatile uint32_t *)0x4000BA04u)
#define GPIO_PFCFGH_REG                                      *((volatile uint32_t *)0x4000BA04u)
#define GPIO_PFCFGH_ADDR                                     (0x4000BA04u)
#define GPIO_PFCFGH_RESET                                    (0x00004444u)
        /* PF7_CFG field */
        #define PF7_CFG                                      (0x0000F000u)
        #define PF7_CFG_MASK                                 (0x0000F000u)
        #define PF7_CFG_BIT                                  (12)
        #define PF7_CFG_BITS                                 (4)
        /* PF6_CFG field */
        #define PF6_CFG                                      (0x00000F00u)
        #define PF6_CFG_MASK                                 (0x00000F00u)
        #define PF6_CFG_BIT                                  (8)
        #define PF6_CFG_BITS                                 (4)
        /* PF5_CFG field */
        #define PF5_CFG                                      (0x000000F0u)
        #define PF5_CFG_MASK                                 (0x000000F0u)
        #define PF5_CFG_BIT                                  (4)
        #define PF5_CFG_BITS                                 (4)
        /* PF4_CFG field */
        #define PF4_CFG                                      (0x0000000Fu)
        #define PF4_CFG_MASK                                 (0x0000000Fu)
        #define PF4_CFG_BIT                                  (0)
        #define PF4_CFG_BITS                                 (4)

#define GPIO_PFIN                                            *((volatile uint32_t *)0x4000BA08u)
#define GPIO_PFIN_REG                                        *((volatile uint32_t *)0x4000BA08u)
#define GPIO_PFIN_ADDR                                       (0x4000BA08u)
#define GPIO_PFIN_RESET                                      (0x00000000u)
        /* PF7 field */
        #define PF7                                          (0x00000080u)
        #define PF7_MASK                                     (0x00000080u)
        #define PF7_BIT                                      (7)
        #define PF7_BITS                                     (1)
        /* PF6 field */
        #define PF6                                          (0x00000040u)
        #define PF6_MASK                                     (0x00000040u)
        #define PF6_BIT                                      (6)
        #define PF6_BITS                                     (1)
        /* PF5 field */
        #define PF5                                          (0x00000020u)
        #define PF5_MASK                                     (0x00000020u)
        #define PF5_BIT                                      (5)
        #define PF5_BITS                                     (1)
        /* PF4 field */
        #define PF4                                          (0x00000010u)
        #define PF4_MASK                                     (0x00000010u)
        #define PF4_BIT                                      (4)
        #define PF4_BITS                                     (1)
        /* PF3 field */
        #define PF3                                          (0x00000008u)
        #define PF3_MASK                                     (0x00000008u)
        #define PF3_BIT                                      (3)
        #define PF3_BITS                                     (1)
        /* PF2 field */
        #define PF2                                          (0x00000004u)
        #define PF2_MASK                                     (0x00000004u)
        #define PF2_BIT                                      (2)
        #define PF2_BITS                                     (1)
        /* PF1 field */
        #define PF1                                          (0x00000002u)
        #define PF1_MASK                                     (0x00000002u)
        #define PF1_BIT                                      (1)
        #define PF1_BITS                                     (1)
        /* PF0 field */
        #define PF0                                          (0x00000001u)
        #define PF0_MASK                                     (0x00000001u)
        #define PF0_BIT                                      (0)
        #define PF0_BITS                                     (1)

#define GPIO_PFOUT                                           *((volatile uint32_t *)0x4000BA0Cu)
#define GPIO_PFOUT_REG                                       *((volatile uint32_t *)0x4000BA0Cu)
#define GPIO_PFOUT_ADDR                                      (0x4000BA0Cu)
#define GPIO_PFOUT_RESET                                     (0x00000000u)
        /* PF7 field */
        #define PF7                                          (0x00000080u)
        #define PF7_MASK                                     (0x00000080u)
        #define PF7_BIT                                      (7)
        #define PF7_BITS                                     (1)
        /* PF6 field */
        #define PF6                                          (0x00000040u)
        #define PF6_MASK                                     (0x00000040u)
        #define PF6_BIT                                      (6)
        #define PF6_BITS                                     (1)
        /* PF5 field */
        #define PF5                                          (0x00000020u)
        #define PF5_MASK                                     (0x00000020u)
        #define PF5_BIT                                      (5)
        #define PF5_BITS                                     (1)
        /* PF4 field */
        #define PF4                                          (0x00000010u)
        #define PF4_MASK                                     (0x00000010u)
        #define PF4_BIT                                      (4)
        #define PF4_BITS                                     (1)
        /* PF3 field */
        #define PF3                                          (0x00000008u)
        #define PF3_MASK                                     (0x00000008u)
        #define PF3_BIT                                      (3)
        #define PF3_BITS                                     (1)
        /* PF2 field */
        #define PF2                                          (0x00000004u)
        #define PF2_MASK                                     (0x00000004u)
        #define PF2_BIT                                      (2)
        #define PF2_BITS                                     (1)
        /* PF1 field */
        #define PF1                                          (0x00000002u)
        #define PF1_MASK                                     (0x00000002u)
        #define PF1_BIT                                      (1)
        #define PF1_BITS                                     (1)
        /* PF0 field */
        #define PF0                                          (0x00000001u)
        #define PF0_MASK                                     (0x00000001u)
        #define PF0_BIT                                      (0)
        #define PF0_BITS                                     (1)

#define GPIO_PFSET                                           *((volatile uint32_t *)0x4000BA10u)
#define GPIO_PFSET_REG                                       *((volatile uint32_t *)0x4000BA10u)
#define GPIO_PFSET_ADDR                                      (0x4000BA10u)
#define GPIO_PFSET_RESET                                     (0x00000000u)
        /* GPIO_PXSETRSVD field */
        #define GPIO_PXSETRSVD                               (0x0000FF00u)
        #define GPIO_PXSETRSVD_MASK                          (0x0000FF00u)
        #define GPIO_PXSETRSVD_BIT                           (8)
        #define GPIO_PXSETRSVD_BITS                          (8)
        /* PF7 field */
        #define PF7                                          (0x00000080u)
        #define PF7_MASK                                     (0x00000080u)
        #define PF7_BIT                                      (7)
        #define PF7_BITS                                     (1)
        /* PF6 field */
        #define PF6                                          (0x00000040u)
        #define PF6_MASK                                     (0x00000040u)
        #define PF6_BIT                                      (6)
        #define PF6_BITS                                     (1)
        /* PF5 field */
        #define PF5                                          (0x00000020u)
        #define PF5_MASK                                     (0x00000020u)
        #define PF5_BIT                                      (5)
        #define PF5_BITS                                     (1)
        /* PF4 field */
        #define PF4                                          (0x00000010u)
        #define PF4_MASK                                     (0x00000010u)
        #define PF4_BIT                                      (4)
        #define PF4_BITS                                     (1)
        /* PF3 field */
        #define PF3                                          (0x00000008u)
        #define PF3_MASK                                     (0x00000008u)
        #define PF3_BIT                                      (3)
        #define PF3_BITS                                     (1)
        /* PF2 field */
        #define PF2                                          (0x00000004u)
        #define PF2_MASK                                     (0x00000004u)
        #define PF2_BIT                                      (2)
        #define PF2_BITS                                     (1)
        /* PF1 field */
        #define PF1                                          (0x00000002u)
        #define PF1_MASK                                     (0x00000002u)
        #define PF1_BIT                                      (1)
        #define PF1_BITS                                     (1)
        /* PF0 field */
        #define PF0                                          (0x00000001u)
        #define PF0_MASK                                     (0x00000001u)
        #define PF0_BIT                                      (0)
        #define PF0_BITS                                     (1)

#define GPIO_PFCLR                                           *((volatile uint32_t *)0x4000BA14u)
#define GPIO_PFCLR_REG                                       *((volatile uint32_t *)0x4000BA14u)
#define GPIO_PFCLR_ADDR                                      (0x4000BA14u)
#define GPIO_PFCLR_RESET                                     (0x00000000u)
        /* PF7 field */
        #define PF7                                          (0x00000080u)
        #define PF7_MASK                                     (0x00000080u)
        #define PF7_BIT                                      (7)
        #define PF7_BITS                                     (1)
        /* PF6 field */
        #define PF6                                          (0x00000040u)
        #define PF6_MASK                                     (0x00000040u)
        #define PF6_BIT                                      (6)
        #define PF6_BITS                                     (1)
        /* PF5 field */
        #define PF5                                          (0x00000020u)
        #define PF5_MASK                                     (0x00000020u)
        #define PF5_BIT                                      (5)
        #define PF5_BITS                                     (1)
        /* PF4 field */
        #define PF4                                          (0x00000010u)
        #define PF4_MASK                                     (0x00000010u)
        #define PF4_BIT                                      (4)
        #define PF4_BITS                                     (1)
        /* PF3 field */
        #define PF3                                          (0x00000008u)
        #define PF3_MASK                                     (0x00000008u)
        #define PF3_BIT                                      (3)
        #define PF3_BITS                                     (1)
        /* PF2 field */
        #define PF2                                          (0x00000004u)
        #define PF2_MASK                                     (0x00000004u)
        #define PF2_BIT                                      (2)
        #define PF2_BITS                                     (1)
        /* PF1 field */
        #define PF1                                          (0x00000002u)
        #define PF1_MASK                                     (0x00000002u)
        #define PF1_BIT                                      (1)
        #define PF1_BITS                                     (1)
        /* PF0 field */
        #define PF0                                          (0x00000001u)
        #define PF0_MASK                                     (0x00000001u)
        #define PF0_BIT                                      (0)
        #define PF0_BITS                                     (1)

#define GPIO_PDWAKE                                          *((volatile uint32_t *)0x4000BC14u)
#define GPIO_PDWAKE_REG                                      *((volatile uint32_t *)0x4000BC14u)
#define GPIO_PDWAKE_ADDR                                     (0x4000BC14u)
#define GPIO_PDWAKE_RESET                                    (0x00000000u)
        /* PD7 field */
        #define PD7                                          (0x00000080u)
        #define PD7_MASK                                     (0x00000080u)
        #define PD7_BIT                                      (7)
        #define PD7_BITS                                     (1)
        /* PD6 field */
        #define PD6                                          (0x00000040u)
        #define PD6_MASK                                     (0x00000040u)
        #define PD6_BIT                                      (6)
        #define PD6_BITS                                     (1)
        /* PD5 field */
        #define PD5                                          (0x00000020u)
        #define PD5_MASK                                     (0x00000020u)
        #define PD5_BIT                                      (5)
        #define PD5_BITS                                     (1)
        /* PD4 field */
        #define PD4                                          (0x00000010u)
        #define PD4_MASK                                     (0x00000010u)
        #define PD4_BIT                                      (4)
        #define PD4_BITS                                     (1)
        /* PD3 field */
        #define PD3                                          (0x00000008u)
        #define PD3_MASK                                     (0x00000008u)
        #define PD3_BIT                                      (3)
        #define PD3_BITS                                     (1)
        /* PD2 field */
        #define PD2                                          (0x00000004u)
        #define PD2_MASK                                     (0x00000004u)
        #define PD2_BIT                                      (2)
        #define PD2_BITS                                     (1)
        /* PD1 field */
        #define PD1                                          (0x00000002u)
        #define PD1_MASK                                     (0x00000002u)
        #define PD1_BIT                                      (1)
        #define PD1_BITS                                     (1)
        /* PD0 field */
        #define PD0                                          (0x00000001u)
        #define PD0_MASK                                     (0x00000001u)
        #define PD0_BIT                                      (0)
        #define PD0_BITS                                     (1)

#define GPIO_PEWAKE                                          *((volatile uint32_t *)0x4000BC18u)
#define GPIO_PEWAKE_REG                                      *((volatile uint32_t *)0x4000BC18u)
#define GPIO_PEWAKE_ADDR                                     (0x4000BC18u)
#define GPIO_PEWAKE_RESET                                    (0x00000000u)
        /* PE7 field */
        #define PE7                                          (0x00000080u)
        #define PE7_MASK                                     (0x00000080u)
        #define PE7_BIT                                      (7)
        #define PE7_BITS                                     (1)
        /* PE6 field */
        #define PE6                                          (0x00000040u)
        #define PE6_MASK                                     (0x00000040u)
        #define PE6_BIT                                      (6)
        #define PE6_BITS                                     (1)
        /* PE5 field */
        #define PE5                                          (0x00000020u)
        #define PE5_MASK                                     (0x00000020u)
        #define PE5_BIT                                      (5)
        #define PE5_BITS                                     (1)
        /* PE4 field */
        #define PE4                                          (0x00000010u)
        #define PE4_MASK                                     (0x00000010u)
        #define PE4_BIT                                      (4)
        #define PE4_BITS                                     (1)
        /* PE3 field */
        #define PE3                                          (0x00000008u)
        #define PE3_MASK                                     (0x00000008u)
        #define PE3_BIT                                      (3)
        #define PE3_BITS                                     (1)
        /* PE2 field */
        #define PE2                                          (0x00000004u)
        #define PE2_MASK                                     (0x00000004u)
        #define PE2_BIT                                      (2)
        #define PE2_BITS                                     (1)
        /* PE1 field */
        #define PE1                                          (0x00000002u)
        #define PE1_MASK                                     (0x00000002u)
        #define PE1_BIT                                      (1)
        #define PE1_BITS                                     (1)
        /* PE0 field */
        #define PE0                                          (0x00000001u)
        #define PE0_MASK                                     (0x00000001u)
        #define PE0_BIT                                      (0)
        #define PE0_BITS                                     (1)

#define GPIO_PFWAKE                                          *((volatile uint32_t *)0x4000BC1Cu)
#define GPIO_PFWAKE_REG                                      *((volatile uint32_t *)0x4000BC1Cu)
#define GPIO_PFWAKE_ADDR                                     (0x4000BC1Cu)
#define GPIO_PFWAKE_RESET                                    (0x00000000u)
        /* PF7 field */
        #define PF7                                          (0x00000080u)
        #define PF7_MASK                                     (0x00000080u)
        #define PF7_BIT                                      (7)
        #define PF7_BITS                                     (1)
        /* PF6 field */
        #define PF6                                          (0x00000040u)
        #define PF6_MASK                                     (0x00000040u)
        #define PF6_BIT                                      (6)
        #define PF6_BITS                                     (1)
        /* PF5 field */
        #define PF5                                          (0x00000020u)
        #define PF5_MASK                                     (0x00000020u)
        #define PF5_BIT                                      (5)
        #define PF5_BITS                                     (1)
        /* PF4 field */
        #define PF4                                          (0x00000010u)
        #define PF4_MASK                                     (0x00000010u)
        #define PF4_BIT                                      (4)
        #define PF4_BITS                                     (1)
        /* PF3 field */
        #define PF3                                          (0x00000008u)
        #define PF3_MASK                                     (0x00000008u)
        #define PF3_BIT                                      (3)
        #define PF3_BITS                                     (1)
        /* PF2 field */
        #define PF2                                          (0x00000004u)
        #define PF2_MASK                                     (0x00000004u)
        #define PF2_BIT                                      (2)
        #define PF2_BITS                                     (1)
        /* PF1 field */
        #define PF1                                          (0x00000002u)
        #define PF1_MASK                                     (0x00000002u)
        #define PF1_BIT                                      (1)
        #define PF1_BITS                                     (1)
        /* PF0 field */
        #define PF0                                          (0x00000001u)
        #define PF0_MASK                                     (0x00000001u)
        #define PF0_BIT                                      (0)
        #define PF0_BITS                                     (1)

#define GPIO_DBGCFG2                                         *((volatile uint32_t *)0x4000BC2Cu)
#define GPIO_DBGCFG2_REG                                     *((volatile uint32_t *)0x4000BC2Cu)
#define GPIO_DBGCFG2_ADDR                                    (0x4000BC2Cu)
#define GPIO_DBGCFG2_RESET                                   (0x00000000u)
        /* USBMON_EN field */
        #define USBMON_EN                                    (0x00000002u)
        #define USBMON_EN_MASK                               (0x00000002u)
        #define USBMON_EN_BIT                                (1)
        #define USBMON_EN_BITS                               (1)
        /* USBSTIM_EN field */
        #define USBSTIM_EN                                   (0x00000001u)
        #define USBSTIM_EN_MASK                              (0x00000001u)
        #define USBSTIM_EN_BIT                               (0)
        #define USBSTIM_EN_BITS                              (1)

/* TIM1 block */
#define BLOCK_TIM1_BASE                                      (0x4000F000u)
#define BLOCK_TIM1_END                                       (0x4000F050u)
#define BLOCK_TIM1_SIZE                                      (BLOCK_TIM1_END - BLOCK_TIM1_BASE + 1)

#define TMR1_DIER                                            *((volatile uint32_t *)0x4000F00Cu)
#define TMR1_DIER_REG                                        *((volatile uint32_t *)0x4000F00Cu)
#define TMR1_DIER_ADDR                                       (0x4000F00Cu)
#define TMR1_DIER_RESET                                      (0x00000000u)
        /* TIE field */
        #define TMR1_DIER_TIE                                (0x00000040u)
        #define TMR1_DIER_TIE_MASK                           (0x00000040u)
        #define TMR1_DIER_TIE_BIT                            (6)
        #define TMR1_DIER_TIE_BITS                           (1)
        /* CC4IE field */
        #define TMR1_DIER_CC4IE                              (0x00000010u)
        #define TMR1_DIER_CC4IE_MASK                         (0x00000010u)
        #define TMR1_DIER_CC4IE_BIT                          (4)
        #define TMR1_DIER_CC4IE_BITS                         (1)
        /* CC3IE field */
        #define TMR1_DIER_CC3IE                              (0x00000008u)
        #define TMR1_DIER_CC3IE_MASK                         (0x00000008u)
        #define TMR1_DIER_CC3IE_BIT                          (3)
        #define TMR1_DIER_CC3IE_BITS                         (1)
        /* CC2IE field */
        #define TMR1_DIER_CC2IE                              (0x00000004u)
        #define TMR1_DIER_CC2IE_MASK                         (0x00000004u)
        #define TMR1_DIER_CC2IE_BIT                          (2)
        #define TMR1_DIER_CC2IE_BITS                         (1)
        /* CC1IE field */
        #define TMR1_DIER_CC1IE                              (0x00000002u)
        #define TMR1_DIER_CC1IE_MASK                         (0x00000002u)
        #define TMR1_DIER_CC1IE_BIT                          (1)
        #define TMR1_DIER_CC1IE_BITS                         (1)
        /* UIE field */
        #define TMR1_DIER_UIE                                (0x00000001u)
        #define TMR1_DIER_UIE_MASK                           (0x00000001u)
        #define TMR1_DIER_UIE_BIT                            (0)
        #define TMR1_DIER_UIE_BITS                           (1)

#define TMR1_SR                                              *((volatile uint32_t *)0x4000F010u)
#define TMR1_SR_REG                                          *((volatile uint32_t *)0x4000F010u)
#define TMR1_SR_ADDR                                         (0x4000F010u)
#define TMR1_SR_RESET                                        (0x00000000u)
        /* CC4OF field */
        #define TMR1_SR_CC4OF                                (0x00001000u)
        #define TMR1_SR_CC4OF_MASK                           (0x00001000u)
        #define TMR1_SR_CC4OF_BIT                            (12)
        #define TMR1_SR_CC4OF_BITS                           (1)
        /* CC3OF field */
        #define TMR1_SR_CC3OF                                (0x00000800u)
        #define TMR1_SR_CC3OF_MASK                           (0x00000800u)
        #define TMR1_SR_CC3OF_BIT                            (11)
        #define TMR1_SR_CC3OF_BITS                           (1)
        /* CC2OF field */
        #define TMR1_SR_CC2OF                                (0x00000400u)
        #define TMR1_SR_CC2OF_MASK                           (0x00000400u)
        #define TMR1_SR_CC2OF_BIT                            (10)
        #define TMR1_SR_CC2OF_BITS                           (1)
        /* CC1OF field */
        #define TMR1_SR_CC1OF                                (0x00000200u)
        #define TMR1_SR_CC1OF_MASK                           (0x00000200u)
        #define TMR1_SR_CC1OF_BIT                            (9)
        #define TMR1_SR_CC1OF_BITS                           (1)
        /* TIF field */
        #define TMR1_SR_TIF                                  (0x00000040u)
        #define TMR1_SR_TIF_MASK                             (0x00000040u)
        #define TMR1_SR_TIF_BIT                              (6)
        #define TMR1_SR_TIF_BITS                             (1)
        /* CC4IF field */
        #define TMR1_SR_CC4IF                                (0x00000010u)
        #define TMR1_SR_CC4IF_MASK                           (0x00000010u)
        #define TMR1_SR_CC4IF_BIT                            (4)
        #define TMR1_SR_CC4IF_BITS                           (1)
        /* CC3IF field */
        #define TMR1_SR_CC3IF                                (0x00000008u)
        #define TMR1_SR_CC3IF_MASK                           (0x00000008u)
        #define TMR1_SR_CC3IF_BIT                            (3)
        #define TMR1_SR_CC3IF_BITS                           (1)
        /* CC2IF field */
        #define TMR1_SR_CC2IF                                (0x00000004u)
        #define TMR1_SR_CC2IF_MASK                           (0x00000004u)
        #define TMR1_SR_CC2IF_BIT                            (2)
        #define TMR1_SR_CC2IF_BITS                           (1)
        /* CC1IF field */
        #define TMR1_SR_CC1IF                                (0x00000002u)
        #define TMR1_SR_CC1IF_MASK                           (0x00000002u)
        #define TMR1_SR_CC1IF_BIT                            (1)
        #define TMR1_SR_CC1IF_BITS                           (1)
        /* UIF field */
        #define TMR1_SR_UIF                                  (0x00000001u)
        #define TMR1_SR_UIF_MASK                             (0x00000001u)
        #define TMR1_SR_UIF_BIT                              (0)
        #define TMR1_SR_UIF_BITS                             (1)

/* TIM2 block */
#define BLOCK_TIM2_BASE                                      (0x40010000u)
#define BLOCK_TIM2_END                                       (0x40010050u)
#define BLOCK_TIM2_SIZE                                      (BLOCK_TIM2_END - BLOCK_TIM2_BASE + 1)

#define TMR2_DIER                                            *((volatile uint32_t *)0x4001000Cu)
#define TMR2_DIER_REG                                        *((volatile uint32_t *)0x4001000Cu)
#define TMR2_DIER_ADDR                                       (0x4001000Cu)
#define TMR2_DIER_RESET                                      (0x00000000u)
        /* TIE field */
        #define TMR2_DIER_TIE                                (0x00000040u)
        #define TMR2_DIER_TIE_MASK                           (0x00000040u)
        #define TMR2_DIER_TIE_BIT                            (6)
        #define TMR2_DIER_TIE_BITS                           (1)
        /* CC4IE field */
        #define TMR2_DIER_CC4IE                              (0x00000010u)
        #define TMR2_DIER_CC4IE_MASK                         (0x00000010u)
        #define TMR2_DIER_CC4IE_BIT                          (4)
        #define TMR2_DIER_CC4IE_BITS                         (1)
        /* CC3IE field */
        #define TMR2_DIER_CC3IE                              (0x00000008u)
        #define TMR2_DIER_CC3IE_MASK                         (0x00000008u)
        #define TMR2_DIER_CC3IE_BIT                          (3)
        #define TMR2_DIER_CC3IE_BITS                         (1)
        /* CC2IE field */
        #define TMR2_DIER_CC2IE                              (0x00000004u)
        #define TMR2_DIER_CC2IE_MASK                         (0x00000004u)
        #define TMR2_DIER_CC2IE_BIT                          (2)
        #define TMR2_DIER_CC2IE_BITS                         (1)
        /* CC1IE field */
        #define TMR2_DIER_CC1IE                              (0x00000002u)
        #define TMR2_DIER_CC1IE_MASK                         (0x00000002u)
        #define TMR2_DIER_CC1IE_BIT                          (1)
        #define TMR2_DIER_CC1IE_BITS                         (1)
        /* UIE field */
        #define TMR2_DIER_UIE                                (0x00000001u)
        #define TMR2_DIER_UIE_MASK                           (0x00000001u)
        #define TMR2_DIER_UIE_BIT                            (0)
        #define TMR2_DIER_UIE_BITS                           (1)

#define TMR2_SR                                              *((volatile uint32_t *)0x40010010u)
#define TMR2_SR_REG                                          *((volatile uint32_t *)0x40010010u)
#define TMR2_SR_ADDR                                         (0x40010010u)
#define TMR2_SR_RESET                                        (0x00000000u)
        /* CC4OF field */
        #define TMR2_SR_CC4OF                                (0x00001000u)
        #define TMR2_SR_CC4OF_MASK                           (0x00001000u)
        #define TMR2_SR_CC4OF_BIT                            (12)
        #define TMR2_SR_CC4OF_BITS                           (1)
        /* CC3OF field */
        #define TMR2_SR_CC3OF                                (0x00000800u)
        #define TMR2_SR_CC3OF_MASK                           (0x00000800u)
        #define TMR2_SR_CC3OF_BIT                            (11)
        #define TMR2_SR_CC3OF_BITS                           (1)
        /* CC2OF field */
        #define TMR2_SR_CC2OF                                (0x00000400u)
        #define TMR2_SR_CC2OF_MASK                           (0x00000400u)
        #define TMR2_SR_CC2OF_BIT                            (10)
        #define TMR2_SR_CC2OF_BITS                           (1)
        /* CC1OF field */
        #define TMR2_SR_CC1OF                                (0x00000200u)
        #define TMR2_SR_CC1OF_MASK                           (0x00000200u)
        #define TMR2_SR_CC1OF_BIT                            (9)
        #define TMR2_SR_CC1OF_BITS                           (1)
        /* TIF field */
        #define TMR2_SR_TIF                                  (0x00000040u)
        #define TMR2_SR_TIF_MASK                             (0x00000040u)
        #define TMR2_SR_TIF_BIT                              (6)
        #define TMR2_SR_TIF_BITS                             (1)
        /* CC4IF field */
        #define TMR2_SR_CC4IF                                (0x00000010u)
        #define TMR2_SR_CC4IF_MASK                           (0x00000010u)
        #define TMR2_SR_CC4IF_BIT                            (4)
        #define TMR2_SR_CC4IF_BITS                           (1)
        /* CC3IF field */
        #define TMR2_SR_CC3IF                                (0x00000008u)
        #define TMR2_SR_CC3IF_MASK                           (0x00000008u)
        #define TMR2_SR_CC3IF_BIT                            (3)
        #define TMR2_SR_CC3IF_BITS                           (1)
        /* CC2IF field */
        #define TMR2_SR_CC2IF                                (0x00000004u)
        #define TMR2_SR_CC2IF_MASK                           (0x00000004u)
        #define TMR2_SR_CC2IF_BIT                            (2)
        #define TMR2_SR_CC2IF_BITS                           (1)
        /* CC1IF field */
        #define TMR2_SR_CC1IF                                (0x00000002u)
        #define TMR2_SR_CC1IF_MASK                           (0x00000002u)
        #define TMR2_SR_CC1IF_BIT                            (1)
        #define TMR2_SR_CC1IF_BITS                           (1)
        /* UIF field */
        #define TMR2_SR_UIF                                  (0x00000001u)
        #define TMR2_SR_UIF_MASK                             (0x00000001u)
        #define TMR2_SR_UIF_BIT                              (0)
        #define TMR2_SR_UIF_BITS                             (1)

/* USB block */
#define BLOCK_USB_BASE                                       (0x40011000u)
#define BLOCK_USB_END                                        (0x400110ACu)
#define BLOCK_USB_SIZE                                       (BLOCK_USB_END - BLOCK_USB_BASE + 1)

#define USB_STATUS2                                          *((volatile uint32_t *)0x40011070u)
#define USB_STATUS2_REG                                      *((volatile uint32_t *)0x40011070u)
#define USB_STATUS2_ADDR                                     (0x40011070u)
#define USB_STATUS2_RESET                                    (0x00000000u)
        /* DP_RPU_EN_SW2 field */
        #define USB_STATUS2_DP_RPU_EN_SW2                    (0x00000001u)
        #define USB_STATUS2_DP_RPU_EN_SW2_MASK               (0x00000001u)
        #define USB_STATUS2_DP_RPU_EN_SW2_BIT                (0)
        #define USB_STATUS2_DP_RPU_EN_SW2_BITS               (1)

/* EXT_RAM block */
#define BLOCK_EXT_RAM_BASE                                   (0x60000000u)
#define BLOCK_EXT_RAM_END                                    (0x9FFFFFFFu)
#define BLOCK_EXT_RAM_SIZE                                   (BLOCK_EXT_RAM_END - BLOCK_EXT_RAM_BASE + 1)

/* EXT_DEVICE block */
#define BLOCK_EXT_DEVICE_BASE                                (0xA0000000u)
#define BLOCK_EXT_DEVICE_END                                 (0xDFFFFFFFu)
#define BLOCK_EXT_DEVICE_SIZE                                (BLOCK_EXT_DEVICE_END - BLOCK_EXT_DEVICE_BASE + 1)

/* ITM block */
#define BLOCK_ITM_BASE                                       (0xE0000000u)
#define BLOCK_ITM_END                                        (0xE0000FFFu)
#define BLOCK_ITM_SIZE                                       (BLOCK_ITM_END - BLOCK_ITM_BASE + 1)

#define ITM_SP0                                              *((volatile uint32_t *)0xE0000000u)
#define ITM_SP0_REG                                          *((volatile uint32_t *)0xE0000000u)
#define ITM_SP0_ADDR                                         (0xE0000000u)
#define ITM_SP0_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP0_FIFOREADY                            (0x00000001u)
        #define ITM_SP0_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP0_FIFOREADY_BIT                        (0)
        #define ITM_SP0_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP0_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP0_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP0_STIMULUS_BIT                         (0)
        #define ITM_SP0_STIMULUS_BITS                        (32)

#define ITM_SP1                                              *((volatile uint32_t *)0xE0000004u)
#define ITM_SP1_REG                                          *((volatile uint32_t *)0xE0000004u)
#define ITM_SP1_ADDR                                         (0xE0000004u)
#define ITM_SP1_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP1_FIFOREADY                            (0x00000001u)
        #define ITM_SP1_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP1_FIFOREADY_BIT                        (0)
        #define ITM_SP1_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP1_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP1_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP1_STIMULUS_BIT                         (0)
        #define ITM_SP1_STIMULUS_BITS                        (32)

#define ITM_SP2                                              *((volatile uint32_t *)0xE0000008u)
#define ITM_SP2_REG                                          *((volatile uint32_t *)0xE0000008u)
#define ITM_SP2_ADDR                                         (0xE0000008u)
#define ITM_SP2_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP2_FIFOREADY                            (0x00000001u)
        #define ITM_SP2_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP2_FIFOREADY_BIT                        (0)
        #define ITM_SP2_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP2_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP2_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP2_STIMULUS_BIT                         (0)
        #define ITM_SP2_STIMULUS_BITS                        (32)

#define ITM_SP3                                              *((volatile uint32_t *)0xE000000Cu)
#define ITM_SP3_REG                                          *((volatile uint32_t *)0xE000000Cu)
#define ITM_SP3_ADDR                                         (0xE000000Cu)
#define ITM_SP3_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP3_FIFOREADY                            (0x00000001u)
        #define ITM_SP3_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP3_FIFOREADY_BIT                        (0)
        #define ITM_SP3_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP3_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP3_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP3_STIMULUS_BIT                         (0)
        #define ITM_SP3_STIMULUS_BITS                        (32)

#define ITM_SP4                                              *((volatile uint32_t *)0xE0000010u)
#define ITM_SP4_REG                                          *((volatile uint32_t *)0xE0000010u)
#define ITM_SP4_ADDR                                         (0xE0000010u)
#define ITM_SP4_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP4_FIFOREADY                            (0x00000001u)
        #define ITM_SP4_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP4_FIFOREADY_BIT                        (0)
        #define ITM_SP4_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP4_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP4_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP4_STIMULUS_BIT                         (0)
        #define ITM_SP4_STIMULUS_BITS                        (32)

#define ITM_SP5                                              *((volatile uint32_t *)0xE0000014u)
#define ITM_SP5_REG                                          *((volatile uint32_t *)0xE0000014u)
#define ITM_SP5_ADDR                                         (0xE0000014u)
#define ITM_SP5_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP5_FIFOREADY                            (0x00000001u)
        #define ITM_SP5_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP5_FIFOREADY_BIT                        (0)
        #define ITM_SP5_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP5_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP5_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP5_STIMULUS_BIT                         (0)
        #define ITM_SP5_STIMULUS_BITS                        (32)

#define ITM_SP6                                              *((volatile uint32_t *)0xE0000018u)
#define ITM_SP6_REG                                          *((volatile uint32_t *)0xE0000018u)
#define ITM_SP6_ADDR                                         (0xE0000018u)
#define ITM_SP6_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP6_FIFOREADY                            (0x00000001u)
        #define ITM_SP6_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP6_FIFOREADY_BIT                        (0)
        #define ITM_SP6_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP6_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP6_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP6_STIMULUS_BIT                         (0)
        #define ITM_SP6_STIMULUS_BITS                        (32)

#define ITM_SP7                                              *((volatile uint32_t *)0xE000001Cu)
#define ITM_SP7_REG                                          *((volatile uint32_t *)0xE000001Cu)
#define ITM_SP7_ADDR                                         (0xE000001Cu)
#define ITM_SP7_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP7_FIFOREADY                            (0x00000001u)
        #define ITM_SP7_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP7_FIFOREADY_BIT                        (0)
        #define ITM_SP7_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP7_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP7_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP7_STIMULUS_BIT                         (0)
        #define ITM_SP7_STIMULUS_BITS                        (32)

#define ITM_SP8                                              *((volatile uint32_t *)0xE0000020u)
#define ITM_SP8_REG                                          *((volatile uint32_t *)0xE0000020u)
#define ITM_SP8_ADDR                                         (0xE0000020u)
#define ITM_SP8_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP8_FIFOREADY                            (0x00000001u)
        #define ITM_SP8_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP8_FIFOREADY_BIT                        (0)
        #define ITM_SP8_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP8_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP8_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP8_STIMULUS_BIT                         (0)
        #define ITM_SP8_STIMULUS_BITS                        (32)

#define ITM_SP9                                              *((volatile uint32_t *)0xE0000024u)
#define ITM_SP9_REG                                          *((volatile uint32_t *)0xE0000024u)
#define ITM_SP9_ADDR                                         (0xE0000024u)
#define ITM_SP9_RESET                                        (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP9_FIFOREADY                            (0x00000001u)
        #define ITM_SP9_FIFOREADY_MASK                       (0x00000001u)
        #define ITM_SP9_FIFOREADY_BIT                        (0)
        #define ITM_SP9_FIFOREADY_BITS                       (1)
        /* STIMULUS field */
        #define ITM_SP9_STIMULUS                             (0xFFFFFFFFu)
        #define ITM_SP9_STIMULUS_MASK                        (0xFFFFFFFFu)
        #define ITM_SP9_STIMULUS_BIT                         (0)
        #define ITM_SP9_STIMULUS_BITS                        (32)

#define ITM_SP10                                             *((volatile uint32_t *)0xE0000028u)
#define ITM_SP10_REG                                         *((volatile uint32_t *)0xE0000028u)
#define ITM_SP10_ADDR                                        (0xE0000028u)
#define ITM_SP10_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP10_FIFOREADY                           (0x00000001u)
        #define ITM_SP10_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP10_FIFOREADY_BIT                       (0)
        #define ITM_SP10_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP10_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP10_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP10_STIMULUS_BIT                        (0)
        #define ITM_SP10_STIMULUS_BITS                       (32)

#define ITM_SP11                                             *((volatile uint32_t *)0xE000002Cu)
#define ITM_SP11_REG                                         *((volatile uint32_t *)0xE000002Cu)
#define ITM_SP11_ADDR                                        (0xE000002Cu)
#define ITM_SP11_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP11_FIFOREADY                           (0x00000001u)
        #define ITM_SP11_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP11_FIFOREADY_BIT                       (0)
        #define ITM_SP11_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP11_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP11_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP11_STIMULUS_BIT                        (0)
        #define ITM_SP11_STIMULUS_BITS                       (32)

#define ITM_SP12                                             *((volatile uint32_t *)0xE0000030u)
#define ITM_SP12_REG                                         *((volatile uint32_t *)0xE0000030u)
#define ITM_SP12_ADDR                                        (0xE0000030u)
#define ITM_SP12_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP12_FIFOREADY                           (0x00000001u)
        #define ITM_SP12_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP12_FIFOREADY_BIT                       (0)
        #define ITM_SP12_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP12_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP12_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP12_STIMULUS_BIT                        (0)
        #define ITM_SP12_STIMULUS_BITS                       (32)

#define ITM_SP13                                             *((volatile uint32_t *)0xE0000034u)
#define ITM_SP13_REG                                         *((volatile uint32_t *)0xE0000034u)
#define ITM_SP13_ADDR                                        (0xE0000034u)
#define ITM_SP13_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP13_FIFOREADY                           (0x00000001u)
        #define ITM_SP13_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP13_FIFOREADY_BIT                       (0)
        #define ITM_SP13_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP13_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP13_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP13_STIMULUS_BIT                        (0)
        #define ITM_SP13_STIMULUS_BITS                       (32)

#define ITM_SP14                                             *((volatile uint32_t *)0xE0000038u)
#define ITM_SP14_REG                                         *((volatile uint32_t *)0xE0000038u)
#define ITM_SP14_ADDR                                        (0xE0000038u)
#define ITM_SP14_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP14_FIFOREADY                           (0x00000001u)
        #define ITM_SP14_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP14_FIFOREADY_BIT                       (0)
        #define ITM_SP14_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP14_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP14_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP14_STIMULUS_BIT                        (0)
        #define ITM_SP14_STIMULUS_BITS                       (32)

#define ITM_SP15                                             *((volatile uint32_t *)0xE000003Cu)
#define ITM_SP15_REG                                         *((volatile uint32_t *)0xE000003Cu)
#define ITM_SP15_ADDR                                        (0xE000003Cu)
#define ITM_SP15_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP15_FIFOREADY                           (0x00000001u)
        #define ITM_SP15_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP15_FIFOREADY_BIT                       (0)
        #define ITM_SP15_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP15_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP15_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP15_STIMULUS_BIT                        (0)
        #define ITM_SP15_STIMULUS_BITS                       (32)

#define ITM_SP16                                             *((volatile uint32_t *)0xE0000040u)
#define ITM_SP16_REG                                         *((volatile uint32_t *)0xE0000040u)
#define ITM_SP16_ADDR                                        (0xE0000040u)
#define ITM_SP16_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP16_FIFOREADY                           (0x00000001u)
        #define ITM_SP16_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP16_FIFOREADY_BIT                       (0)
        #define ITM_SP16_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP16_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP16_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP16_STIMULUS_BIT                        (0)
        #define ITM_SP16_STIMULUS_BITS                       (32)

#define ITM_SP17                                             *((volatile uint32_t *)0xE0000044u)
#define ITM_SP17_REG                                         *((volatile uint32_t *)0xE0000044u)
#define ITM_SP17_ADDR                                        (0xE0000044u)
#define ITM_SP17_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP17_FIFOREADY                           (0x00000001u)
        #define ITM_SP17_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP17_FIFOREADY_BIT                       (0)
        #define ITM_SP17_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP17_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP17_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP17_STIMULUS_BIT                        (0)
        #define ITM_SP17_STIMULUS_BITS                       (32)

#define ITM_SP18                                             *((volatile uint32_t *)0xE0000048u)
#define ITM_SP18_REG                                         *((volatile uint32_t *)0xE0000048u)
#define ITM_SP18_ADDR                                        (0xE0000048u)
#define ITM_SP18_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP18_FIFOREADY                           (0x00000001u)
        #define ITM_SP18_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP18_FIFOREADY_BIT                       (0)
        #define ITM_SP18_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP18_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP18_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP18_STIMULUS_BIT                        (0)
        #define ITM_SP18_STIMULUS_BITS                       (32)

#define ITM_SP19                                             *((volatile uint32_t *)0xE000004Cu)
#define ITM_SP19_REG                                         *((volatile uint32_t *)0xE000004Cu)
#define ITM_SP19_ADDR                                        (0xE000004Cu)
#define ITM_SP19_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP19_FIFOREADY                           (0x00000001u)
        #define ITM_SP19_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP19_FIFOREADY_BIT                       (0)
        #define ITM_SP19_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP19_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP19_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP19_STIMULUS_BIT                        (0)
        #define ITM_SP19_STIMULUS_BITS                       (32)

#define ITM_SP20                                             *((volatile uint32_t *)0xE0000050u)
#define ITM_SP20_REG                                         *((volatile uint32_t *)0xE0000050u)
#define ITM_SP20_ADDR                                        (0xE0000050u)
#define ITM_SP20_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP20_FIFOREADY                           (0x00000001u)
        #define ITM_SP20_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP20_FIFOREADY_BIT                       (0)
        #define ITM_SP20_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP20_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP20_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP20_STIMULUS_BIT                        (0)
        #define ITM_SP20_STIMULUS_BITS                       (32)

#define ITM_SP21                                             *((volatile uint32_t *)0xE0000054u)
#define ITM_SP21_REG                                         *((volatile uint32_t *)0xE0000054u)
#define ITM_SP21_ADDR                                        (0xE0000054u)
#define ITM_SP21_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP21_FIFOREADY                           (0x00000001u)
        #define ITM_SP21_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP21_FIFOREADY_BIT                       (0)
        #define ITM_SP21_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP21_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP21_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP21_STIMULUS_BIT                        (0)
        #define ITM_SP21_STIMULUS_BITS                       (32)

#define ITM_SP22                                             *((volatile uint32_t *)0xE0000058u)
#define ITM_SP22_REG                                         *((volatile uint32_t *)0xE0000058u)
#define ITM_SP22_ADDR                                        (0xE0000058u)
#define ITM_SP22_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP22_FIFOREADY                           (0x00000001u)
        #define ITM_SP22_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP22_FIFOREADY_BIT                       (0)
        #define ITM_SP22_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP22_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP22_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP22_STIMULUS_BIT                        (0)
        #define ITM_SP22_STIMULUS_BITS                       (32)

#define ITM_SP23                                             *((volatile uint32_t *)0xE000005Cu)
#define ITM_SP23_REG                                         *((volatile uint32_t *)0xE000005Cu)
#define ITM_SP23_ADDR                                        (0xE000005Cu)
#define ITM_SP23_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP23_FIFOREADY                           (0x00000001u)
        #define ITM_SP23_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP23_FIFOREADY_BIT                       (0)
        #define ITM_SP23_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP23_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP23_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP23_STIMULUS_BIT                        (0)
        #define ITM_SP23_STIMULUS_BITS                       (32)

#define ITM_SP24                                             *((volatile uint32_t *)0xE0000060u)
#define ITM_SP24_REG                                         *((volatile uint32_t *)0xE0000060u)
#define ITM_SP24_ADDR                                        (0xE0000060u)
#define ITM_SP24_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP24_FIFOREADY                           (0x00000001u)
        #define ITM_SP24_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP24_FIFOREADY_BIT                       (0)
        #define ITM_SP24_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP24_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP24_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP24_STIMULUS_BIT                        (0)
        #define ITM_SP24_STIMULUS_BITS                       (32)

#define ITM_SP25                                             *((volatile uint32_t *)0xE0000064u)
#define ITM_SP25_REG                                         *((volatile uint32_t *)0xE0000064u)
#define ITM_SP25_ADDR                                        (0xE0000064u)
#define ITM_SP25_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP25_FIFOREADY                           (0x00000001u)
        #define ITM_SP25_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP25_FIFOREADY_BIT                       (0)
        #define ITM_SP25_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP25_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP25_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP25_STIMULUS_BIT                        (0)
        #define ITM_SP25_STIMULUS_BITS                       (32)

#define ITM_SP26                                             *((volatile uint32_t *)0xE0000068u)
#define ITM_SP26_REG                                         *((volatile uint32_t *)0xE0000068u)
#define ITM_SP26_ADDR                                        (0xE0000068u)
#define ITM_SP26_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP26_FIFOREADY                           (0x00000001u)
        #define ITM_SP26_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP26_FIFOREADY_BIT                       (0)
        #define ITM_SP26_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP26_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP26_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP26_STIMULUS_BIT                        (0)
        #define ITM_SP26_STIMULUS_BITS                       (32)

#define ITM_SP27                                             *((volatile uint32_t *)0xE000006Cu)
#define ITM_SP27_REG                                         *((volatile uint32_t *)0xE000006Cu)
#define ITM_SP27_ADDR                                        (0xE000006Cu)
#define ITM_SP27_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP27_FIFOREADY                           (0x00000001u)
        #define ITM_SP27_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP27_FIFOREADY_BIT                       (0)
        #define ITM_SP27_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP27_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP27_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP27_STIMULUS_BIT                        (0)
        #define ITM_SP27_STIMULUS_BITS                       (32)

#define ITM_SP28                                             *((volatile uint32_t *)0xE0000070u)
#define ITM_SP28_REG                                         *((volatile uint32_t *)0xE0000070u)
#define ITM_SP28_ADDR                                        (0xE0000070u)
#define ITM_SP28_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP28_FIFOREADY                           (0x00000001u)
        #define ITM_SP28_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP28_FIFOREADY_BIT                       (0)
        #define ITM_SP28_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP28_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP28_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP28_STIMULUS_BIT                        (0)
        #define ITM_SP28_STIMULUS_BITS                       (32)

#define ITM_SP29                                             *((volatile uint32_t *)0xE0000074u)
#define ITM_SP29_REG                                         *((volatile uint32_t *)0xE0000074u)
#define ITM_SP29_ADDR                                        (0xE0000074u)
#define ITM_SP29_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP29_FIFOREADY                           (0x00000001u)
        #define ITM_SP29_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP29_FIFOREADY_BIT                       (0)
        #define ITM_SP29_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP29_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP29_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP29_STIMULUS_BIT                        (0)
        #define ITM_SP29_STIMULUS_BITS                       (32)

#define ITM_SP30                                             *((volatile uint32_t *)0xE0000078u)
#define ITM_SP30_REG                                         *((volatile uint32_t *)0xE0000078u)
#define ITM_SP30_ADDR                                        (0xE0000078u)
#define ITM_SP30_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP30_FIFOREADY                           (0x00000001u)
        #define ITM_SP30_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP30_FIFOREADY_BIT                       (0)
        #define ITM_SP30_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP30_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP30_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP30_STIMULUS_BIT                        (0)
        #define ITM_SP30_STIMULUS_BITS                       (32)

#define ITM_SP31                                             *((volatile uint32_t *)0xE000007Cu)
#define ITM_SP31_REG                                         *((volatile uint32_t *)0xE000007Cu)
#define ITM_SP31_ADDR                                        (0xE000007Cu)
#define ITM_SP31_RESET                                       (0x00000000u)
        /* FIFOREADY field */
        #define ITM_SP31_FIFOREADY                           (0x00000001u)
        #define ITM_SP31_FIFOREADY_MASK                      (0x00000001u)
        #define ITM_SP31_FIFOREADY_BIT                       (0)
        #define ITM_SP31_FIFOREADY_BITS                      (1)
        /* STIMULUS field */
        #define ITM_SP31_STIMULUS                            (0xFFFFFFFFu)
        #define ITM_SP31_STIMULUS_MASK                       (0xFFFFFFFFu)
        #define ITM_SP31_STIMULUS_BIT                        (0)
        #define ITM_SP31_STIMULUS_BITS                       (32)

#define ITM_TER                                              *((volatile uint32_t *)0xE0000E00u)
#define ITM_TER_REG                                          *((volatile uint32_t *)0xE0000E00u)
#define ITM_TER_ADDR                                         (0xE0000E00u)
#define ITM_TER_RESET                                        (0x00000000u)
        /* STIMENA field */
        #define ITM_TER_STIMENA                              (0xFFFFFFFFu)
        #define ITM_TER_STIMENA_MASK                         (0xFFFFFFFFu)
        #define ITM_TER_STIMENA_BIT                          (0)
        #define ITM_TER_STIMENA_BITS                         (32)

#define ITM_TPR                                              *((volatile uint32_t *)0xE0000E40u)
#define ITM_TPR_REG                                          *((volatile uint32_t *)0xE0000E40u)
#define ITM_TPR_ADDR                                         (0xE0000E40u)
#define ITM_TPR_RESET                                        (0x00000000u)
        /* PRIVMASK field */
        #define ITM_TPR_PRIVMASK                             (0x0000000Fu)
        #define ITM_TPR_PRIVMASK_MASK                        (0x0000000Fu)
        #define ITM_TPR_PRIVMASK_BIT                         (0)
        #define ITM_TPR_PRIVMASK_BITS                        (4)

#define ITM_TCR                                              *((volatile uint32_t *)0xE0000E80u)
#define ITM_TCR_REG                                          *((volatile uint32_t *)0xE0000E80u)
#define ITM_TCR_ADDR                                         (0xE0000E80u)
#define ITM_TCR_RESET                                        (0x00000000u)
        /* BUSY field */
        #define ITM_TCR_BUSY                                 (0x00800000u)
        #define ITM_TCR_BUSY_MASK                            (0x00800000u)
        #define ITM_TCR_BUSY_BIT                             (23)
        #define ITM_TCR_BUSY_BITS                            (1)
        /* ATBID field */
        #define ITM_TCR_ATBID                                (0x007F0000u)
        #define ITM_TCR_ATBID_MASK                           (0x007F0000u)
        #define ITM_TCR_ATBID_BIT                            (16)
        #define ITM_TCR_ATBID_BITS                           (7)
        /* TSPRESCALE field */
        #define ITM_TCR_TSPRESCALE                           (0x00000300u)
        #define ITM_TCR_TSPRESCALE_MASK                      (0x00000300u)
        #define ITM_TCR_TSPRESCALE_BIT                       (8)
        #define ITM_TCR_TSPRESCALE_BITS                      (2)
        /* SWOENA field */
        #define ITM_TCR_SWOENA                               (0x00000010u)
        #define ITM_TCR_SWOENA_MASK                          (0x00000010u)
        #define ITM_TCR_SWOENA_BIT                           (4)
        #define ITM_TCR_SWOENA_BITS                          (1)
        /* DWTENA field */
        #define ITM_TCR_DWTENA                               (0x00000008u)
        #define ITM_TCR_DWTENA_MASK                          (0x00000008u)
        #define ITM_TCR_DWTENA_BIT                           (3)
        #define ITM_TCR_DWTENA_BITS                          (1)
        /* SYNCENA field */
        #define ITM_TCR_SYNCENA                              (0x00000004u)
        #define ITM_TCR_SYNCENA_MASK                         (0x00000004u)
        #define ITM_TCR_SYNCENA_BIT                          (2)
        #define ITM_TCR_SYNCENA_BITS                         (1)
        /* TSENA field */
        #define ITM_TCR_TSENA                                (0x00000002u)
        #define ITM_TCR_TSENA_MASK                           (0x00000002u)
        #define ITM_TCR_TSENA_BIT                            (1)
        #define ITM_TCR_TSENA_BITS                           (1)
        /* ITMEN field */
        #define ITM_TCR_ITMEN                                (0x00000001u)
        #define ITM_TCR_ITMEN_MASK                           (0x00000001u)
        #define ITM_TCR_ITMEN_BIT                            (0)
        #define ITM_TCR_ITMEN_BITS                           (1)

#define ITM_IW                                               *((volatile uint32_t *)0xE0000EF8u)
#define ITM_IW_REG                                           *((volatile uint32_t *)0xE0000EF8u)
#define ITM_IW_ADDR                                          (0xE0000EF8u)
#define ITM_IW_RESET                                         (0x00000000u)
        /* ATVALIDM field */
        #define ITM_IW_ATVALIDM                              (0x00000001u)
        #define ITM_IW_ATVALIDM_MASK                         (0x00000001u)
        #define ITM_IW_ATVALIDM_BIT                          (0)
        #define ITM_IW_ATVALIDM_BITS                         (1)

#define ITM_IR                                               *((volatile uint32_t *)0xE0000EFCu)
#define ITM_IR_REG                                           *((volatile uint32_t *)0xE0000EFCu)
#define ITM_IR_ADDR                                          (0xE0000EFCu)
#define ITM_IR_RESET                                         (0x00000000u)
        /* ATREADYM field */
        #define ITM_IR_ATREADYM                              (0x00000001u)
        #define ITM_IR_ATREADYM_MASK                         (0x00000001u)
        #define ITM_IR_ATREADYM_BIT                          (0)
        #define ITM_IR_ATREADYM_BITS                         (1)

#define ITM_IMC                                              *((volatile uint32_t *)0xE0000F00u)
#define ITM_IMC_REG                                          *((volatile uint32_t *)0xE0000F00u)
#define ITM_IMC_ADDR                                         (0xE0000F00u)
#define ITM_IMC_RESET                                        (0x00000000u)
        /* INTEGRATION field */
        #define ITM_IMC_INTEGRATION                          (0x00000001u)
        #define ITM_IMC_INTEGRATION_MASK                     (0x00000001u)
        #define ITM_IMC_INTEGRATION_BIT                      (0)
        #define ITM_IMC_INTEGRATION_BITS                     (1)

#define ITM_LA                                               *((volatile uint32_t *)0xE0000FB0u)
#define ITM_LA_REG                                           *((volatile uint32_t *)0xE0000FB0u)
#define ITM_LA_ADDR                                          (0xE0000FB0u)
#define ITM_LA_RESET                                         (0x00000000u)
        /* LOCKACC field */
        #define ITM_LA_LOCKACC                               (0xFFFFFFFFu)
        #define ITM_LA_LOCKACC_MASK                          (0xFFFFFFFFu)
        #define ITM_LA_LOCKACC_BIT                           (0)
        #define ITM_LA_LOCKACC_BITS                          (32)

#define ITM_LS                                               *((volatile uint32_t *)0xE0000FB4u)
#define ITM_LS_REG                                           *((volatile uint32_t *)0xE0000FB4u)
#define ITM_LS_ADDR                                          (0xE0000FB4u)
#define ITM_LS_RESET                                         (0x00000000u)
        /* BYTEACC field */
        #define ITM_LS_BYTEACC                               (0x00000004u)
        #define ITM_LS_BYTEACC_MASK                          (0x00000004u)
        #define ITM_LS_BYTEACC_BIT                           (2)
        #define ITM_LS_BYTEACC_BITS                          (1)
        /* ACCESS field */
        #define ITM_LS_ACCESS                                (0x00000002u)
        #define ITM_LS_ACCESS_MASK                           (0x00000002u)
        #define ITM_LS_ACCESS_BIT                            (1)
        #define ITM_LS_ACCESS_BITS                           (1)
        /* PRESENT field */
        #define ITM_LS_PRESENT                               (0x00000001u)
        #define ITM_LS_PRESENT_MASK                          (0x00000001u)
        #define ITM_LS_PRESENT_BIT                           (0)
        #define ITM_LS_PRESENT_BITS                          (1)

#define ITM_PERIPHID4                                        *((volatile uint32_t *)0xE0000FD0u)
#define ITM_PERIPHID4_REG                                    *((volatile uint32_t *)0xE0000FD0u)
#define ITM_PERIPHID4_ADDR                                   (0xE0000FD0u)
#define ITM_PERIPHID4_RESET                                  (0x00000004u)
        /* PERIPHID field */
        #define ITM_PERIPHID4_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID4_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID4_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID4_PERIPHID_BITS                  (32)

#define ITM_PERIPHID5                                        *((volatile uint32_t *)0xE0000FD4u)
#define ITM_PERIPHID5_REG                                    *((volatile uint32_t *)0xE0000FD4u)
#define ITM_PERIPHID5_ADDR                                   (0xE0000FD4u)
#define ITM_PERIPHID5_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define ITM_PERIPHID5_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID5_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID5_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID5_PERIPHID_BITS                  (32)

#define ITM_PERIPHID6                                        *((volatile uint32_t *)0xE0000FD8u)
#define ITM_PERIPHID6_REG                                    *((volatile uint32_t *)0xE0000FD8u)
#define ITM_PERIPHID6_ADDR                                   (0xE0000FD8u)
#define ITM_PERIPHID6_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define ITM_PERIPHID6_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID6_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID6_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID6_PERIPHID_BITS                  (32)

#define ITM_PERIPHID7                                        *((volatile uint32_t *)0xE0000FDCu)
#define ITM_PERIPHID7_REG                                    *((volatile uint32_t *)0xE0000FDCu)
#define ITM_PERIPHID7_ADDR                                   (0xE0000FDCu)
#define ITM_PERIPHID7_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define ITM_PERIPHID7_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID7_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID7_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID7_PERIPHID_BITS                  (32)

#define ITM_PERIPHID0                                        *((volatile uint32_t *)0xE0000FE0u)
#define ITM_PERIPHID0_REG                                    *((volatile uint32_t *)0xE0000FE0u)
#define ITM_PERIPHID0_ADDR                                   (0xE0000FE0u)
#define ITM_PERIPHID0_RESET                                  (0x00000001u)
        /* PERIPHID field */
        #define ITM_PERIPHID0_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID0_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID0_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID0_PERIPHID_BITS                  (32)

#define ITM_PERIPHID1                                        *((volatile uint32_t *)0xE0000FE4u)
#define ITM_PERIPHID1_REG                                    *((volatile uint32_t *)0xE0000FE4u)
#define ITM_PERIPHID1_ADDR                                   (0xE0000FE4u)
#define ITM_PERIPHID1_RESET                                  (0x000000B0u)
        /* PERIPHID field */
        #define ITM_PERIPHID1_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID1_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID1_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID1_PERIPHID_BITS                  (32)

#define ITM_PERIPHID2                                        *((volatile uint32_t *)0xE0000FE8u)
#define ITM_PERIPHID2_REG                                    *((volatile uint32_t *)0xE0000FE8u)
#define ITM_PERIPHID2_ADDR                                   (0xE0000FE8u)
#define ITM_PERIPHID2_RESET                                  (0x0000001Bu)
        /* PERIPHID field */
        #define ITM_PERIPHID2_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID2_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID2_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID2_PERIPHID_BITS                  (32)

#define ITM_PERIPHID3                                        *((volatile uint32_t *)0xE0000FECu)
#define ITM_PERIPHID3_REG                                    *((volatile uint32_t *)0xE0000FECu)
#define ITM_PERIPHID3_ADDR                                   (0xE0000FECu)
#define ITM_PERIPHID3_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define ITM_PERIPHID3_PERIPHID                       (0xFFFFFFFFu)
        #define ITM_PERIPHID3_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ITM_PERIPHID3_PERIPHID_BIT                   (0)
        #define ITM_PERIPHID3_PERIPHID_BITS                  (32)

#define ITM_CELLID0                                          *((volatile uint32_t *)0xE0000FF0u)
#define ITM_CELLID0_REG                                      *((volatile uint32_t *)0xE0000FF0u)
#define ITM_CELLID0_ADDR                                     (0xE0000FF0u)
#define ITM_CELLID0_RESET                                    (0x0000000Du)
        /* PERIPHID field */
        #define ITM_CELLID0_PERIPHID                         (0xFFFFFFFFu)
        #define ITM_CELLID0_PERIPHID_MASK                    (0xFFFFFFFFu)
        #define ITM_CELLID0_PERIPHID_BIT                     (0)
        #define ITM_CELLID0_PERIPHID_BITS                    (32)

#define ITM_CELLID1                                          *((volatile uint32_t *)0xE0000FF4u)
#define ITM_CELLID1_REG                                      *((volatile uint32_t *)0xE0000FF4u)
#define ITM_CELLID1_ADDR                                     (0xE0000FF4u)
#define ITM_CELLID1_RESET                                    (0x000000E0u)
        /* PERIPHID field */
        #define ITM_CELLID1_PERIPHID                         (0xFFFFFFFFu)
        #define ITM_CELLID1_PERIPHID_MASK                    (0xFFFFFFFFu)
        #define ITM_CELLID1_PERIPHID_BIT                     (0)
        #define ITM_CELLID1_PERIPHID_BITS                    (32)

#define ITM_CELLID2                                          *((volatile uint32_t *)0xE0000FF8u)
#define ITM_CELLID2_REG                                      *((volatile uint32_t *)0xE0000FF8u)
#define ITM_CELLID2_ADDR                                     (0xE0000FF8u)
#define ITM_CELLID2_RESET                                    (0x00000005u)
        /* PERIPHID field */
        #define ITM_CELLID2_PERIPHID                         (0xFFFFFFFFu)
        #define ITM_CELLID2_PERIPHID_MASK                    (0xFFFFFFFFu)
        #define ITM_CELLID2_PERIPHID_BIT                     (0)
        #define ITM_CELLID2_PERIPHID_BITS                    (32)

#define ITM_CELLID3                                          *((volatile uint32_t *)0xE0000FFCu)
#define ITM_CELLID3_REG                                      *((volatile uint32_t *)0xE0000FFCu)
#define ITM_CELLID3_ADDR                                     (0xE0000FFCu)
#define ITM_CELLID3_RESET                                    (0x000000B1u)
        /* PERIPHID field */
        #define ITM_CELLID3_PERIPHID                         (0xFFFFFFFFu)
        #define ITM_CELLID3_PERIPHID_MASK                    (0xFFFFFFFFu)
        #define ITM_CELLID3_PERIPHID_BIT                     (0)
        #define ITM_CELLID3_PERIPHID_BITS                    (32)

/* DWT block */
#define BLOCK_DWT_BASE                                       (0xE0001000u)
#define BLOCK_DWT_END                                        (0xE0001FFFu)
#define BLOCK_DWT_SIZE                                       (BLOCK_DWT_END - BLOCK_DWT_BASE + 1)

#define DWT_CTRL                                             *((volatile uint32_t *)0xE0001000u)
#define DWT_CTRL_REG                                         *((volatile uint32_t *)0xE0001000u)
#define DWT_CTRL_ADDR                                        (0xE0001000u)
#define DWT_CTRL_RESET                                       (0x40000000u)
        /* NUMCOMP field */
        #define DWT_CTRL_NUMCOMP                             (0xF0000000u)
        #define DWT_CTRL_NUMCOMP_MASK                        (0xF0000000u)
        #define DWT_CTRL_NUMCOMP_BIT                         (28)
        #define DWT_CTRL_NUMCOMP_BITS                        (4)
        /* CYCEVTENA field */
        #define DWT_CTRL_CYCEVTENA                           (0x00400000u)
        #define DWT_CTRL_CYCEVTENA_MASK                      (0x00400000u)
        #define DWT_CTRL_CYCEVTENA_BIT                       (22)
        #define DWT_CTRL_CYCEVTENA_BITS                      (1)
        /* FOLDEVTENA field */
        #define DWT_CTRL_FOLDEVTENA                          (0x00200000u)
        #define DWT_CTRL_FOLDEVTENA_MASK                     (0x00200000u)
        #define DWT_CTRL_FOLDEVTENA_BIT                      (21)
        #define DWT_CTRL_FOLDEVTENA_BITS                     (1)
        /* LSUEVTENA field */
        #define DWT_CTRL_LSUEVTENA                           (0x00100000u)
        #define DWT_CTRL_LSUEVTENA_MASK                      (0x00100000u)
        #define DWT_CTRL_LSUEVTENA_BIT                       (20)
        #define DWT_CTRL_LSUEVTENA_BITS                      (1)
        /* SLEEPEVTENA field */
        #define DWT_CTRL_SLEEPEVTENA                         (0x00080000u)
        #define DWT_CTRL_SLEEPEVTENA_MASK                    (0x00080000u)
        #define DWT_CTRL_SLEEPEVTENA_BIT                     (19)
        #define DWT_CTRL_SLEEPEVTENA_BITS                    (1)
        /* EXCEVTENA field */
        #define DWT_CTRL_EXCEVTENA                           (0x00040000u)
        #define DWT_CTRL_EXCEVTENA_MASK                      (0x00040000u)
        #define DWT_CTRL_EXCEVTENA_BIT                       (18)
        #define DWT_CTRL_EXCEVTENA_BITS                      (1)
        /* CPIEVTENA field */
        #define DWT_CTRL_CPIEVTENA                           (0x00020000u)
        #define DWT_CTRL_CPIEVTENA_MASK                      (0x00020000u)
        #define DWT_CTRL_CPIEVTENA_BIT                       (17)
        #define DWT_CTRL_CPIEVTENA_BITS                      (1)
        /* EXCTRCENA field */
        #define DWT_CTRL_EXCTRCENA                           (0x00010000u)
        #define DWT_CTRL_EXCTRCENA_MASK                      (0x00010000u)
        #define DWT_CTRL_EXCTRCENA_BIT                       (16)
        #define DWT_CTRL_EXCTRCENA_BITS                      (1)
        /* PCSAMPLEENA field */
        #define DWT_CTRL_PCSAMPLEENA                         (0x00001000u)
        #define DWT_CTRL_PCSAMPLEENA_MASK                    (0x00001000u)
        #define DWT_CTRL_PCSAMPLEENA_BIT                     (12)
        #define DWT_CTRL_PCSAMPLEENA_BITS                    (1)
        /* SYNCTAP field */
        #define DWT_CTRL_SYNCTAP                             (0x00000C00u)
        #define DWT_CTRL_SYNCTAP_MASK                        (0x00000C00u)
        #define DWT_CTRL_SYNCTAP_BIT                         (10)
        #define DWT_CTRL_SYNCTAP_BITS                        (2)
        /* CYCTAP field */
        #define DWT_CTRL_CYCTAP                              (0x00000200u)
        #define DWT_CTRL_CYCTAP_MASK                         (0x00000200u)
        #define DWT_CTRL_CYCTAP_BIT                          (9)
        #define DWT_CTRL_CYCTAP_BITS                         (1)
        /* POSTCNT field */
        #define DWT_CTRL_POSTCNT                             (0x000001E0u)
        #define DWT_CTRL_POSTCNT_MASK                        (0x000001E0u)
        #define DWT_CTRL_POSTCNT_BIT                         (5)
        #define DWT_CTRL_POSTCNT_BITS                        (4)
        /* POSTPRESET field */
        #define DWT_CTRL_POSTPRESET                          (0x0000001Eu)
        #define DWT_CTRL_POSTPRESET_MASK                     (0x0000001Eu)
        #define DWT_CTRL_POSTPRESET_BIT                      (1)
        #define DWT_CTRL_POSTPRESET_BITS                     (4)
        /* CYCCNTENA field */
        #define DWT_CTRL_CYCCNTENA                           (0x00000001u)
        #define DWT_CTRL_CYCCNTENA_MASK                      (0x00000001u)
        #define DWT_CTRL_CYCCNTENA_BIT                       (0)
        #define DWT_CTRL_CYCCNTENA_BITS                      (1)

#define DWT_CYCCNT                                           *((volatile uint32_t *)0xE0001004u)
#define DWT_CYCCNT_REG                                       *((volatile uint32_t *)0xE0001004u)
#define DWT_CYCCNT_ADDR                                      (0xE0001004u)
#define DWT_CYCCNT_RESET                                     (0x00000000u)
        /* CYCCNT field */
        #define DWT_CYCCNT_CYCCNT                            (0xFFFFFFFFu)
        #define DWT_CYCCNT_CYCCNT_MASK                       (0xFFFFFFFFu)
        #define DWT_CYCCNT_CYCCNT_BIT                        (0)
        #define DWT_CYCCNT_CYCCNT_BITS                       (32)

#define DWT_CPICNT                                           *((volatile uint32_t *)0xE0001008u)
#define DWT_CPICNT_REG                                       *((volatile uint32_t *)0xE0001008u)
#define DWT_CPICNT_ADDR                                      (0xE0001008u)
#define DWT_CPICNT_RESET                                     (0x00000000u)
        /* CPICNT field */
        #define DWT_CPICNT_CPICNT                            (0x000000FFu)
        #define DWT_CPICNT_CPICNT_MASK                       (0x000000FFu)
        #define DWT_CPICNT_CPICNT_BIT                        (0)
        #define DWT_CPICNT_CPICNT_BITS                       (8)

#define DWT_EXCCNT                                           *((volatile uint32_t *)0xE000100Cu)
#define DWT_EXCCNT_REG                                       *((volatile uint32_t *)0xE000100Cu)
#define DWT_EXCCNT_ADDR                                      (0xE000100Cu)
#define DWT_EXCCNT_RESET                                     (0x00000000u)
        /* EXCCNT field */
        #define DWT_EXCCNT_EXCCNT                            (0x000000FFu)
        #define DWT_EXCCNT_EXCCNT_MASK                       (0x000000FFu)
        #define DWT_EXCCNT_EXCCNT_BIT                        (0)
        #define DWT_EXCCNT_EXCCNT_BITS                       (8)

#define DWT_SLEEPCNT                                         *((volatile uint32_t *)0xE0001010u)
#define DWT_SLEEPCNT_REG                                     *((volatile uint32_t *)0xE0001010u)
#define DWT_SLEEPCNT_ADDR                                    (0xE0001010u)
#define DWT_SLEEPCNT_RESET                                   (0x00000000u)
        /* SLEEPCNT field */
        #define DWT_SLEEPCNT_SLEEPCNT                        (0x000000FFu)
        #define DWT_SLEEPCNT_SLEEPCNT_MASK                   (0x000000FFu)
        #define DWT_SLEEPCNT_SLEEPCNT_BIT                    (0)
        #define DWT_SLEEPCNT_SLEEPCNT_BITS                   (8)

#define DWT_LSUCNT                                           *((volatile uint32_t *)0xE0001014u)
#define DWT_LSUCNT_REG                                       *((volatile uint32_t *)0xE0001014u)
#define DWT_LSUCNT_ADDR                                      (0xE0001014u)
#define DWT_LSUCNT_RESET                                     (0x00000000u)
        /* CPICNT field */
        #define DWT_LSUCNT_CPICNT                            (0x000000FFu)
        #define DWT_LSUCNT_CPICNT_MASK                       (0x000000FFu)
        #define DWT_LSUCNT_CPICNT_BIT                        (0)
        #define DWT_LSUCNT_CPICNT_BITS                       (8)

#define DWT_FOLDCNT                                          *((volatile uint32_t *)0xE0001018u)
#define DWT_FOLDCNT_REG                                      *((volatile uint32_t *)0xE0001018u)
#define DWT_FOLDCNT_ADDR                                     (0xE0001018u)
#define DWT_FOLDCNT_RESET                                    (0x00000000u)
        /* CPICNT field */
        #define DWT_FOLDCNT_CPICNT                           (0x000000FFu)
        #define DWT_FOLDCNT_CPICNT_MASK                      (0x000000FFu)
        #define DWT_FOLDCNT_CPICNT_BIT                       (0)
        #define DWT_FOLDCNT_CPICNT_BITS                      (8)

#define DWT_PCSR                                             *((volatile uint32_t *)0xE000101Cu)
#define DWT_PCSR_REG                                         *((volatile uint32_t *)0xE000101Cu)
#define DWT_PCSR_ADDR                                        (0xE000101Cu)
#define DWT_PCSR_RESET                                       (0x00000000u)
        /* EIASAMPLE field */
        #define DWT_PCSR_EIASAMPLE                           (0xFFFFFFFFu)
        #define DWT_PCSR_EIASAMPLE_MASK                      (0xFFFFFFFFu)
        #define DWT_PCSR_EIASAMPLE_BIT                       (0)
        #define DWT_PCSR_EIASAMPLE_BITS                      (32)

#define DWT_COMP0                                            *((volatile uint32_t *)0xE0001020u)
#define DWT_COMP0_REG                                        *((volatile uint32_t *)0xE0001020u)
#define DWT_COMP0_ADDR                                       (0xE0001020u)
#define DWT_COMP0_RESET                                      (0x00000000u)
        /* COMP0 field */
        #define DWT_COMP0_COMP0                              (0xFFFFFFFFu)
        #define DWT_COMP0_COMP0_MASK                         (0xFFFFFFFFu)
        #define DWT_COMP0_COMP0_BIT                          (0)
        #define DWT_COMP0_COMP0_BITS                         (32)

#define DWT_MASK0                                            *((volatile uint32_t *)0xE0001024u)
#define DWT_MASK0_REG                                        *((volatile uint32_t *)0xE0001024u)
#define DWT_MASK0_ADDR                                       (0xE0001024u)
#define DWT_MASK0_RESET                                      (0x00000000u)
        /* MASK0 field */
        #define DWT_MASK0_MASK0                              (0x0000001Fu)
        #define DWT_MASK0_MASK0_MASK                         (0x0000001Fu)
        #define DWT_MASK0_MASK0_BIT                          (0)
        #define DWT_MASK0_MASK0_BITS                         (5)

#define DWT_FUNCTION0                                        *((volatile uint32_t *)0xE0001028u)
#define DWT_FUNCTION0_REG                                    *((volatile uint32_t *)0xE0001028u)
#define DWT_FUNCTION0_ADDR                                   (0xE0001028u)
#define DWT_FUNCTION0_RESET                                  (0x00000000u)
        /* MATCHED field */
        #define DWT_FUNCTION0_MATCHED                        (0x01000000u)
        #define DWT_FUNCTION0_MATCHED_MASK                   (0x01000000u)
        #define DWT_FUNCTION0_MATCHED_BIT                    (24)
        #define DWT_FUNCTION0_MATCHED_BITS                   (1)
        /* CYCMATCH field */
        #define DWT_FUNCTION0_CYCMATCH                       (0x00000080u)
        #define DWT_FUNCTION0_CYCMATCH_MASK                  (0x00000080u)
        #define DWT_FUNCTION0_CYCMATCH_BIT                   (7)
        #define DWT_FUNCTION0_CYCMATCH_BITS                  (1)
        /* EMITRANGE field */
        #define DWT_FUNCTION0_EMITRANGE                      (0x00000020u)
        #define DWT_FUNCTION0_EMITRANGE_MASK                 (0x00000020u)
        #define DWT_FUNCTION0_EMITRANGE_BIT                  (5)
        #define DWT_FUNCTION0_EMITRANGE_BITS                 (1)
        /* FUNCTION field */
        #define DWT_FUNCTION0_FUNCTION                       (0x0000000Fu)
        #define DWT_FUNCTION0_FUNCTION_MASK                  (0x0000000Fu)
        #define DWT_FUNCTION0_FUNCTION_BIT                   (0)
        #define DWT_FUNCTION0_FUNCTION_BITS                  (4)

#define DWT_COMP1                                            *((volatile uint32_t *)0xE0001030u)
#define DWT_COMP1_REG                                        *((volatile uint32_t *)0xE0001030u)
#define DWT_COMP1_ADDR                                       (0xE0001030u)
#define DWT_COMP1_RESET                                      (0x00000000u)
        /* COMP1 field */
        #define DWT_COMP1_COMP1                              (0xFFFFFFFFu)
        #define DWT_COMP1_COMP1_MASK                         (0xFFFFFFFFu)
        #define DWT_COMP1_COMP1_BIT                          (0)
        #define DWT_COMP1_COMP1_BITS                         (32)

#define DWT_MASK1                                            *((volatile uint32_t *)0xE0001034u)
#define DWT_MASK1_REG                                        *((volatile uint32_t *)0xE0001034u)
#define DWT_MASK1_ADDR                                       (0xE0001034u)
#define DWT_MASK1_RESET                                      (0x00000000u)
        /* MASK1 field */
        #define DWT_MASK1_MASK1                              (0x0000001Fu)
        #define DWT_MASK1_MASK1_MASK                         (0x0000001Fu)
        #define DWT_MASK1_MASK1_BIT                          (0)
        #define DWT_MASK1_MASK1_BITS                         (5)

#define DWT_FUNCTION1                                        *((volatile uint32_t *)0xE0001038u)
#define DWT_FUNCTION1_REG                                    *((volatile uint32_t *)0xE0001038u)
#define DWT_FUNCTION1_ADDR                                   (0xE0001038u)
#define DWT_FUNCTION1_RESET                                  (0x00000200u)
        /* MATCHED field */
        #define DWT_FUNCTION1_MATCHED                        (0x01000000u)
        #define DWT_FUNCTION1_MATCHED_MASK                   (0x01000000u)
        #define DWT_FUNCTION1_MATCHED_BIT                    (24)
        #define DWT_FUNCTION1_MATCHED_BITS                   (1)
        /* DATAVADDR1 field */
        #define DWT_FUNCTION1_DATAVADDR1                     (0x000F0000u)
        #define DWT_FUNCTION1_DATAVADDR1_MASK                (0x000F0000u)
        #define DWT_FUNCTION1_DATAVADDR1_BIT                 (16)
        #define DWT_FUNCTION1_DATAVADDR1_BITS                (4)
        /* DATAVADDR0 field */
        #define DWT_FUNCTION1_DATAVADDR0                     (0x0000F000u)
        #define DWT_FUNCTION1_DATAVADDR0_MASK                (0x0000F000u)
        #define DWT_FUNCTION1_DATAVADDR0_BIT                 (12)
        #define DWT_FUNCTION1_DATAVADDR0_BITS                (4)
        /* DATAVSIZE field */
        #define DWT_FUNCTION1_DATAVSIZE                      (0x00000C00u)
        #define DWT_FUNCTION1_DATAVSIZE_MASK                 (0x00000C00u)
        #define DWT_FUNCTION1_DATAVSIZE_BIT                  (10)
        #define DWT_FUNCTION1_DATAVSIZE_BITS                 (2)
        /* LNK1ENA field */
        #define DWT_FUNCTION1_LNK1ENA                        (0x00000200u)
        #define DWT_FUNCTION1_LNK1ENA_MASK                   (0x00000200u)
        #define DWT_FUNCTION1_LNK1ENA_BIT                    (9)
        #define DWT_FUNCTION1_LNK1ENA_BITS                   (1)
        /* DATAVMATCH field */
        #define DWT_FUNCTION1_DATAVMATCH                     (0x00000100u)
        #define DWT_FUNCTION1_DATAVMATCH_MASK                (0x00000100u)
        #define DWT_FUNCTION1_DATAVMATCH_BIT                 (8)
        #define DWT_FUNCTION1_DATAVMATCH_BITS                (1)
        /* EMITRANGE field */
        #define DWT_FUNCTION1_EMITRANGE                      (0x00000020u)
        #define DWT_FUNCTION1_EMITRANGE_MASK                 (0x00000020u)
        #define DWT_FUNCTION1_EMITRANGE_BIT                  (5)
        #define DWT_FUNCTION1_EMITRANGE_BITS                 (1)
        /* FUNCTION field */
        #define DWT_FUNCTION1_FUNCTION                       (0x0000000Fu)
        #define DWT_FUNCTION1_FUNCTION_MASK                  (0x0000000Fu)
        #define DWT_FUNCTION1_FUNCTION_BIT                   (0)
        #define DWT_FUNCTION1_FUNCTION_BITS                  (4)

#define DWT_COMP2                                            *((volatile uint32_t *)0xE0001040u)
#define DWT_COMP2_REG                                        *((volatile uint32_t *)0xE0001040u)
#define DWT_COMP2_ADDR                                       (0xE0001040u)
#define DWT_COMP2_RESET                                      (0x00000000u)
        /* COMP2 field */
        #define DWT_COMP2_COMP2                              (0xFFFFFFFFu)
        #define DWT_COMP2_COMP2_MASK                         (0xFFFFFFFFu)
        #define DWT_COMP2_COMP2_BIT                          (0)
        #define DWT_COMP2_COMP2_BITS                         (32)

#define DWT_MASK2                                            *((volatile uint32_t *)0xE0001044u)
#define DWT_MASK2_REG                                        *((volatile uint32_t *)0xE0001044u)
#define DWT_MASK2_ADDR                                       (0xE0001044u)
#define DWT_MASK2_RESET                                      (0x00000000u)
        /* MASK2 field */
        #define DWT_MASK2_MASK2                              (0x0000001Fu)
        #define DWT_MASK2_MASK2_MASK                         (0x0000001Fu)
        #define DWT_MASK2_MASK2_BIT                          (0)
        #define DWT_MASK2_MASK2_BITS                         (5)

#define DWT_FUNCTION2                                        *((volatile uint32_t *)0xE0001048u)
#define DWT_FUNCTION2_REG                                    *((volatile uint32_t *)0xE0001048u)
#define DWT_FUNCTION2_ADDR                                   (0xE0001048u)
#define DWT_FUNCTION2_RESET                                  (0x00000000u)
        /* MATCHED field */
        #define DWT_FUNCTION2_MATCHED                        (0x01000000u)
        #define DWT_FUNCTION2_MATCHED_MASK                   (0x01000000u)
        #define DWT_FUNCTION2_MATCHED_BIT                    (24)
        #define DWT_FUNCTION2_MATCHED_BITS                   (1)
        /* EMITRANGE field */
        #define DWT_FUNCTION2_EMITRANGE                      (0x00000020u)
        #define DWT_FUNCTION2_EMITRANGE_MASK                 (0x00000020u)
        #define DWT_FUNCTION2_EMITRANGE_BIT                  (5)
        #define DWT_FUNCTION2_EMITRANGE_BITS                 (1)
        /* FUNCTION field */
        #define DWT_FUNCTION2_FUNCTION                       (0x0000000Fu)
        #define DWT_FUNCTION2_FUNCTION_MASK                  (0x0000000Fu)
        #define DWT_FUNCTION2_FUNCTION_BIT                   (0)
        #define DWT_FUNCTION2_FUNCTION_BITS                  (4)

#define DWT_COMP3                                            *((volatile uint32_t *)0xE0001050u)
#define DWT_COMP3_REG                                        *((volatile uint32_t *)0xE0001050u)
#define DWT_COMP3_ADDR                                       (0xE0001050u)
#define DWT_COMP3_RESET                                      (0x00000000u)
        /* COMP3 field */
        #define DWT_COMP3_COMP3                              (0xFFFFFFFFu)
        #define DWT_COMP3_COMP3_MASK                         (0xFFFFFFFFu)
        #define DWT_COMP3_COMP3_BIT                          (0)
        #define DWT_COMP3_COMP3_BITS                         (32)

#define DWT_MASK3                                            *((volatile uint32_t *)0xE0001054u)
#define DWT_MASK3_REG                                        *((volatile uint32_t *)0xE0001054u)
#define DWT_MASK3_ADDR                                       (0xE0001054u)
#define DWT_MASK3_RESET                                      (0x00000000u)
        /* MASK3 field */
        #define DWT_MASK3_MASK3                              (0x0000001Fu)
        #define DWT_MASK3_MASK3_MASK                         (0x0000001Fu)
        #define DWT_MASK3_MASK3_BIT                          (0)
        #define DWT_MASK3_MASK3_BITS                         (5)

#define DWT_FUNCTION3                                        *((volatile uint32_t *)0xE0001058u)
#define DWT_FUNCTION3_REG                                    *((volatile uint32_t *)0xE0001058u)
#define DWT_FUNCTION3_ADDR                                   (0xE0001058u)
#define DWT_FUNCTION3_RESET                                  (0x00000000u)
        /* MATCHED field */
        #define DWT_FUNCTION3_MATCHED                        (0x01000000u)
        #define DWT_FUNCTION3_MATCHED_MASK                   (0x01000000u)
        #define DWT_FUNCTION3_MATCHED_BIT                    (24)
        #define DWT_FUNCTION3_MATCHED_BITS                   (1)
        /* EMITRANGE field */
        #define DWT_FUNCTION3_EMITRANGE                      (0x00000020u)
        #define DWT_FUNCTION3_EMITRANGE_MASK                 (0x00000020u)
        #define DWT_FUNCTION3_EMITRANGE_BIT                  (5)
        #define DWT_FUNCTION3_EMITRANGE_BITS                 (1)
        /* FUNCTION field */
        #define DWT_FUNCTION3_FUNCTION                       (0x0000000Fu)
        #define DWT_FUNCTION3_FUNCTION_MASK                  (0x0000000Fu)
        #define DWT_FUNCTION3_FUNCTION_BIT                   (0)
        #define DWT_FUNCTION3_FUNCTION_BITS                  (4)

#define DWT_PERIPHID4                                        *((volatile uint32_t *)0xE0001FD0u)
#define DWT_PERIPHID4_REG                                    *((volatile uint32_t *)0xE0001FD0u)
#define DWT_PERIPHID4_ADDR                                   (0xE0001FD0u)
#define DWT_PERIPHID4_RESET                                  (0x00000004u)
        /* PERIPHID field */
        #define DWT_PERIPHID4_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID4_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID4_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID4_PERIPHID_BITS                  (32)

#define DWT_PERIPHID5                                        *((volatile uint32_t *)0xE0001FD4u)
#define DWT_PERIPHID5_REG                                    *((volatile uint32_t *)0xE0001FD4u)
#define DWT_PERIPHID5_ADDR                                   (0xE0001FD4u)
#define DWT_PERIPHID5_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define DWT_PERIPHID5_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID5_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID5_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID5_PERIPHID_BITS                  (32)

#define DWT_PERIPHID6                                        *((volatile uint32_t *)0xE0001FD8u)
#define DWT_PERIPHID6_REG                                    *((volatile uint32_t *)0xE0001FD8u)
#define DWT_PERIPHID6_ADDR                                   (0xE0001FD8u)
#define DWT_PERIPHID6_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define DWT_PERIPHID6_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID6_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID6_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID6_PERIPHID_BITS                  (32)

#define DWT_PERIPHID7                                        *((volatile uint32_t *)0xE0001FDCu)
#define DWT_PERIPHID7_REG                                    *((volatile uint32_t *)0xE0001FDCu)
#define DWT_PERIPHID7_ADDR                                   (0xE0001FDCu)
#define DWT_PERIPHID7_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define DWT_PERIPHID7_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID7_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID7_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID7_PERIPHID_BITS                  (32)

#define DWT_PERIPHID0                                        *((volatile uint32_t *)0xE0001FE0u)
#define DWT_PERIPHID0_REG                                    *((volatile uint32_t *)0xE0001FE0u)
#define DWT_PERIPHID0_ADDR                                   (0xE0001FE0u)
#define DWT_PERIPHID0_RESET                                  (0x00000002u)
        /* PERIPHID field */
        #define DWT_PERIPHID0_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID0_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID0_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID0_PERIPHID_BITS                  (32)

#define DWT_PERIPHID1                                        *((volatile uint32_t *)0xE0001FE4u)
#define DWT_PERIPHID1_REG                                    *((volatile uint32_t *)0xE0001FE4u)
#define DWT_PERIPHID1_ADDR                                   (0xE0001FE4u)
#define DWT_PERIPHID1_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define DWT_PERIPHID1_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID1_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID1_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID1_PERIPHID_BITS                  (32)

#define DWT_PERIPHID2                                        *((volatile uint32_t *)0xE0001FE8u)
#define DWT_PERIPHID2_REG                                    *((volatile uint32_t *)0xE0001FE8u)
#define DWT_PERIPHID2_ADDR                                   (0xE0001FE8u)
#define DWT_PERIPHID2_RESET                                  (0x0000001Bu)
        /* PERIPHID field */
        #define DWT_PERIPHID2_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID2_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID2_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID2_PERIPHID_BITS                  (32)

#define DWT_PERIPHID3                                        *((volatile uint32_t *)0xE0001FECu)
#define DWT_PERIPHID3_REG                                    *((volatile uint32_t *)0xE0001FECu)
#define DWT_PERIPHID3_ADDR                                   (0xE0001FECu)
#define DWT_PERIPHID3_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define DWT_PERIPHID3_PERIPHID                       (0xFFFFFFFFu)
        #define DWT_PERIPHID3_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define DWT_PERIPHID3_PERIPHID_BIT                   (0)
        #define DWT_PERIPHID3_PERIPHID_BITS                  (32)

#define DWT_CELLID0                                          *((volatile uint32_t *)0xE0001FF0u)
#define DWT_CELLID0_REG                                      *((volatile uint32_t *)0xE0001FF0u)
#define DWT_CELLID0_ADDR                                     (0xE0001FF0u)
#define DWT_CELLID0_RESET                                    (0x0000000Du)
        /* CELLID field */
        #define DWT_CELLID0_CELLID                           (0xFFFFFFFFu)
        #define DWT_CELLID0_CELLID_MASK                      (0xFFFFFFFFu)
        #define DWT_CELLID0_CELLID_BIT                       (0)
        #define DWT_CELLID0_CELLID_BITS                      (32)

#define DWT_CELLID1                                          *((volatile uint32_t *)0xE0001FF4u)
#define DWT_CELLID1_REG                                      *((volatile uint32_t *)0xE0001FF4u)
#define DWT_CELLID1_ADDR                                     (0xE0001FF4u)
#define DWT_CELLID1_RESET                                    (0x000000E0u)
        /* CELLID field */
        #define DWT_CELLID1_CELLID                           (0xFFFFFFFFu)
        #define DWT_CELLID1_CELLID_MASK                      (0xFFFFFFFFu)
        #define DWT_CELLID1_CELLID_BIT                       (0)
        #define DWT_CELLID1_CELLID_BITS                      (32)

#define DWT_CELLID2                                          *((volatile uint32_t *)0xE0001FF8u)
#define DWT_CELLID2_REG                                      *((volatile uint32_t *)0xE0001FF8u)
#define DWT_CELLID2_ADDR                                     (0xE0001FF8u)
#define DWT_CELLID2_RESET                                    (0x00000005u)
        /* CELLID field */
        #define DWT_CELLID2_CELLID                           (0xFFFFFFFFu)
        #define DWT_CELLID2_CELLID_MASK                      (0xFFFFFFFFu)
        #define DWT_CELLID2_CELLID_BIT                       (0)
        #define DWT_CELLID2_CELLID_BITS                      (32)

#define DWT_CELLID3                                          *((volatile uint32_t *)0xE0001FFCu)
#define DWT_CELLID3_REG                                      *((volatile uint32_t *)0xE0001FFCu)
#define DWT_CELLID3_ADDR                                     (0xE0001FFCu)
#define DWT_CELLID3_RESET                                    (0x000000B1u)
        /* CELLID field */
        #define DWT_CELLID3_CELLID                           (0xFFFFFFFFu)
        #define DWT_CELLID3_CELLID_MASK                      (0xFFFFFFFFu)
        #define DWT_CELLID3_CELLID_BIT                       (0)
        #define DWT_CELLID3_CELLID_BITS                      (32)

/* FPB block */
#define BLOCK_FPB_BASE                                       (0xE0002000u)
#define BLOCK_FPB_END                                        (0xE0002FFFu)
#define BLOCK_FPB_SIZE                                       (BLOCK_FPB_END - BLOCK_FPB_BASE + 1)

#define FPB_CTRL                                             *((volatile uint32_t *)0xE0002000u)
#define FPB_CTRL_REG                                         *((volatile uint32_t *)0xE0002000u)
#define FPB_CTRL_ADDR                                        (0xE0002000u)
#define FPB_CTRL_RESET                                       (0x00000000u)
        /* NUM_LIT field */
        #define FPB_CTRL_NUM_LIT                             (0x00000F00u)
        #define FPB_CTRL_NUM_LIT_MASK                        (0x00000F00u)
        #define FPB_CTRL_NUM_LIT_BIT                         (8)
        #define FPB_CTRL_NUM_LIT_BITS                        (4)
        /* NUM_CODE field */
        #define FPB_CTRL_NUM_CODE                            (0x000000F0u)
        #define FPB_CTRL_NUM_CODE_MASK                       (0x000000F0u)
        #define FPB_CTRL_NUM_CODE_BIT                        (4)
        #define FPB_CTRL_NUM_CODE_BITS                       (4)
        /* KEY field */
        #define FPB_CTRL_KEY                                 (0x00000002u)
        #define FPB_CTRL_KEY_MASK                            (0x00000002u)
        #define FPB_CTRL_KEY_BIT                             (1)
        #define FPB_CTRL_KEY_BITS                            (1)
        /* enable field */
        #define FPB_CTRL_enable                              (0x00000001u)
        #define FPB_CTRL_enable_MASK                         (0x00000001u)
        #define FPB_CTRL_enable_BIT                          (0)
        #define FPB_CTRL_enable_BITS                         (1)

#define FPB_REMAP                                            *((volatile uint32_t *)0xE0002004u)
#define FPB_REMAP_REG                                        *((volatile uint32_t *)0xE0002004u)
#define FPB_REMAP_ADDR                                       (0xE0002004u)
#define FPB_REMAP_RESET                                      (0x20000000u)
        /* REMAP field */
        #define FPB_REMAP_REMAP                              (0x1FFFFFE0u)
        #define FPB_REMAP_REMAP_MASK                         (0x1FFFFFE0u)
        #define FPB_REMAP_REMAP_BIT                          (5)
        #define FPB_REMAP_REMAP_BITS                         (24)

#define FPB_COMP0                                            *((volatile uint32_t *)0xE0002008u)
#define FPB_COMP0_REG                                        *((volatile uint32_t *)0xE0002008u)
#define FPB_COMP0_ADDR                                       (0xE0002008u)
#define FPB_COMP0_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP0_REPLACE                            (0xC0000000u)
        #define FPB_COMP0_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP0_REPLACE_BIT                        (30)
        #define FPB_COMP0_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP0_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP0_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP0_COMP_BIT                           (2)
        #define FPB_COMP0_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP0_enable                             (0x00000001u)
        #define FPB_COMP0_enable_MASK                        (0x00000001u)
        #define FPB_COMP0_enable_BIT                         (0)
        #define FPB_COMP0_enable_BITS                        (1)

#define FPB_COMP1                                            *((volatile uint32_t *)0xE000200Cu)
#define FPB_COMP1_REG                                        *((volatile uint32_t *)0xE000200Cu)
#define FPB_COMP1_ADDR                                       (0xE000200Cu)
#define FPB_COMP1_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP1_REPLACE                            (0xC0000000u)
        #define FPB_COMP1_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP1_REPLACE_BIT                        (30)
        #define FPB_COMP1_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP1_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP1_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP1_COMP_BIT                           (2)
        #define FPB_COMP1_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP1_enable                             (0x00000001u)
        #define FPB_COMP1_enable_MASK                        (0x00000001u)
        #define FPB_COMP1_enable_BIT                         (0)
        #define FPB_COMP1_enable_BITS                        (1)

#define FPB_COMP2                                            *((volatile uint32_t *)0xE0002010u)
#define FPB_COMP2_REG                                        *((volatile uint32_t *)0xE0002010u)
#define FPB_COMP2_ADDR                                       (0xE0002010u)
#define FPB_COMP2_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP2_REPLACE                            (0xC0000000u)
        #define FPB_COMP2_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP2_REPLACE_BIT                        (30)
        #define FPB_COMP2_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP2_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP2_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP2_COMP_BIT                           (2)
        #define FPB_COMP2_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP2_enable                             (0x00000001u)
        #define FPB_COMP2_enable_MASK                        (0x00000001u)
        #define FPB_COMP2_enable_BIT                         (0)
        #define FPB_COMP2_enable_BITS                        (1)

#define FPB_COMP3                                            *((volatile uint32_t *)0xE0002014u)
#define FPB_COMP3_REG                                        *((volatile uint32_t *)0xE0002014u)
#define FPB_COMP3_ADDR                                       (0xE0002014u)
#define FPB_COMP3_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP3_REPLACE                            (0xC0000000u)
        #define FPB_COMP3_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP3_REPLACE_BIT                        (30)
        #define FPB_COMP3_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP3_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP3_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP3_COMP_BIT                           (2)
        #define FPB_COMP3_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP3_enable                             (0x00000001u)
        #define FPB_COMP3_enable_MASK                        (0x00000001u)
        #define FPB_COMP3_enable_BIT                         (0)
        #define FPB_COMP3_enable_BITS                        (1)

#define FPB_COMP4                                            *((volatile uint32_t *)0xE0002018u)
#define FPB_COMP4_REG                                        *((volatile uint32_t *)0xE0002018u)
#define FPB_COMP4_ADDR                                       (0xE0002018u)
#define FPB_COMP4_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP4_REPLACE                            (0xC0000000u)
        #define FPB_COMP4_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP4_REPLACE_BIT                        (30)
        #define FPB_COMP4_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP4_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP4_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP4_COMP_BIT                           (2)
        #define FPB_COMP4_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP4_enable                             (0x00000001u)
        #define FPB_COMP4_enable_MASK                        (0x00000001u)
        #define FPB_COMP4_enable_BIT                         (0)
        #define FPB_COMP4_enable_BITS                        (1)

#define FPB_COMP5                                            *((volatile uint32_t *)0xE000201Cu)
#define FPB_COMP5_REG                                        *((volatile uint32_t *)0xE000201Cu)
#define FPB_COMP5_ADDR                                       (0xE000201Cu)
#define FPB_COMP5_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP5_REPLACE                            (0xC0000000u)
        #define FPB_COMP5_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP5_REPLACE_BIT                        (30)
        #define FPB_COMP5_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP5_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP5_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP5_COMP_BIT                           (2)
        #define FPB_COMP5_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP5_enable                             (0x00000001u)
        #define FPB_COMP5_enable_MASK                        (0x00000001u)
        #define FPB_COMP5_enable_BIT                         (0)
        #define FPB_COMP5_enable_BITS                        (1)

#define FPB_COMP6                                            *((volatile uint32_t *)0xE0002020u)
#define FPB_COMP6_REG                                        *((volatile uint32_t *)0xE0002020u)
#define FPB_COMP6_ADDR                                       (0xE0002020u)
#define FPB_COMP6_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP6_REPLACE                            (0xC0000000u)
        #define FPB_COMP6_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP6_REPLACE_BIT                        (30)
        #define FPB_COMP6_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP6_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP6_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP6_COMP_BIT                           (2)
        #define FPB_COMP6_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP6_enable                             (0x00000001u)
        #define FPB_COMP6_enable_MASK                        (0x00000001u)
        #define FPB_COMP6_enable_BIT                         (0)
        #define FPB_COMP6_enable_BITS                        (1)

#define FPB_COMP7                                            *((volatile uint32_t *)0xE0002024u)
#define FPB_COMP7_REG                                        *((volatile uint32_t *)0xE0002024u)
#define FPB_COMP7_ADDR                                       (0xE0002024u)
#define FPB_COMP7_RESET                                      (0x00000000u)
        /* REPLACE field */
        #define FPB_COMP7_REPLACE                            (0xC0000000u)
        #define FPB_COMP7_REPLACE_MASK                       (0xC0000000u)
        #define FPB_COMP7_REPLACE_BIT                        (30)
        #define FPB_COMP7_REPLACE_BITS                       (2)
        /* COMP field */
        #define FPB_COMP7_COMP                               (0x1FFFFFFCu)
        #define FPB_COMP7_COMP_MASK                          (0x1FFFFFFCu)
        #define FPB_COMP7_COMP_BIT                           (2)
        #define FPB_COMP7_COMP_BITS                          (27)
        /* enable field */
        #define FPB_COMP7_enable                             (0x00000001u)
        #define FPB_COMP7_enable_MASK                        (0x00000001u)
        #define FPB_COMP7_enable_BIT                         (0)
        #define FPB_COMP7_enable_BITS                        (1)

#define FPB_PERIPHID4                                        *((volatile uint32_t *)0xE0002FD0u)
#define FPB_PERIPHID4_REG                                    *((volatile uint32_t *)0xE0002FD0u)
#define FPB_PERIPHID4_ADDR                                   (0xE0002FD0u)
#define FPB_PERIPHID4_RESET                                  (0x00000004u)
        /* PERIPHID field */
        #define FPB_PERIPHID4_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID4_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID4_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID4_PERIPHID_BITS                  (32)

#define FPB_PERIPHID5                                        *((volatile uint32_t *)0xE0002FD4u)
#define FPB_PERIPHID5_REG                                    *((volatile uint32_t *)0xE0002FD4u)
#define FPB_PERIPHID5_ADDR                                   (0xE0002FD4u)
#define FPB_PERIPHID5_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define FPB_PERIPHID5_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID5_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID5_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID5_PERIPHID_BITS                  (32)

#define FPB_PERIPHID6                                        *((volatile uint32_t *)0xE0002FD8u)
#define FPB_PERIPHID6_REG                                    *((volatile uint32_t *)0xE0002FD8u)
#define FPB_PERIPHID6_ADDR                                   (0xE0002FD8u)
#define FPB_PERIPHID6_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define FPB_PERIPHID6_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID6_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID6_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID6_PERIPHID_BITS                  (32)

#define FPB_PERIPHID7                                        *((volatile uint32_t *)0xE0002FDCu)
#define FPB_PERIPHID7_REG                                    *((volatile uint32_t *)0xE0002FDCu)
#define FPB_PERIPHID7_ADDR                                   (0xE0002FDCu)
#define FPB_PERIPHID7_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define FPB_PERIPHID7_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID7_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID7_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID7_PERIPHID_BITS                  (32)

#define FPB_PERIPHID0                                        *((volatile uint32_t *)0xE0002FE0u)
#define FPB_PERIPHID0_REG                                    *((volatile uint32_t *)0xE0002FE0u)
#define FPB_PERIPHID0_ADDR                                   (0xE0002FE0u)
#define FPB_PERIPHID0_RESET                                  (0x00000003u)
        /* PERIPHID field */
        #define FPB_PERIPHID0_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID0_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID0_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID0_PERIPHID_BITS                  (32)

#define FPB_PERIPHID1                                        *((volatile uint32_t *)0xE0002FE4u)
#define FPB_PERIPHID1_REG                                    *((volatile uint32_t *)0xE0002FE4u)
#define FPB_PERIPHID1_ADDR                                   (0xE0002FE4u)
#define FPB_PERIPHID1_RESET                                  (0x000000B0u)
        /* PERIPHID field */
        #define FPB_PERIPHID1_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID1_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID1_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID1_PERIPHID_BITS                  (32)

#define FPB_PERIPHID2                                        *((volatile uint32_t *)0xE0002FE8u)
#define FPB_PERIPHID2_REG                                    *((volatile uint32_t *)0xE0002FE8u)
#define FPB_PERIPHID2_ADDR                                   (0xE0002FE8u)
#define FPB_PERIPHID2_RESET                                  (0x0000000Bu)
        /* PERIPHID field */
        #define FPB_PERIPHID2_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID2_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID2_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID2_PERIPHID_BITS                  (32)

#define FPB_PERIPHID3                                        *((volatile uint32_t *)0xE0002FECu)
#define FPB_PERIPHID3_REG                                    *((volatile uint32_t *)0xE0002FECu)
#define FPB_PERIPHID3_ADDR                                   (0xE0002FECu)
#define FPB_PERIPHID3_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define FPB_PERIPHID3_PERIPHID                       (0xFFFFFFFFu)
        #define FPB_PERIPHID3_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define FPB_PERIPHID3_PERIPHID_BIT                   (0)
        #define FPB_PERIPHID3_PERIPHID_BITS                  (32)

#define FPB_CELLID0                                          *((volatile uint32_t *)0xE0002FF0u)
#define FPB_CELLID0_REG                                      *((volatile uint32_t *)0xE0002FF0u)
#define FPB_CELLID0_ADDR                                     (0xE0002FF0u)
#define FPB_CELLID0_RESET                                    (0x0000000Du)
        /* CELLID field */
        #define FPB_CELLID0_CELLID                           (0xFFFFFFFFu)
        #define FPB_CELLID0_CELLID_MASK                      (0xFFFFFFFFu)
        #define FPB_CELLID0_CELLID_BIT                       (0)
        #define FPB_CELLID0_CELLID_BITS                      (32)

#define FPB_CELLID1                                          *((volatile uint32_t *)0xE0002FF4u)
#define FPB_CELLID1_REG                                      *((volatile uint32_t *)0xE0002FF4u)
#define FPB_CELLID1_ADDR                                     (0xE0002FF4u)
#define FPB_CELLID1_RESET                                    (0x000000E0u)
        /* CELLID field */
        #define FPB_CELLID1_CELLID                           (0xFFFFFFFFu)
        #define FPB_CELLID1_CELLID_MASK                      (0xFFFFFFFFu)
        #define FPB_CELLID1_CELLID_BIT                       (0)
        #define FPB_CELLID1_CELLID_BITS                      (32)

#define FPB_CELLID2                                          *((volatile uint32_t *)0xE0002FF8u)
#define FPB_CELLID2_REG                                      *((volatile uint32_t *)0xE0002FF8u)
#define FPB_CELLID2_ADDR                                     (0xE0002FF8u)
#define FPB_CELLID2_RESET                                    (0x00000005u)
        /* CELLID field */
        #define FPB_CELLID2_CELLID                           (0xFFFFFFFFu)
        #define FPB_CELLID2_CELLID_MASK                      (0xFFFFFFFFu)
        #define FPB_CELLID2_CELLID_BIT                       (0)
        #define FPB_CELLID2_CELLID_BITS                      (32)

#define FPB_CELLID3                                          *((volatile uint32_t *)0xE0002FFCu)
#define FPB_CELLID3_REG                                      *((volatile uint32_t *)0xE0002FFCu)
#define FPB_CELLID3_ADDR                                     (0xE0002FFCu)
#define FPB_CELLID3_RESET                                    (0x000000B1u)
        /* CELLID field */
        #define FPB_CELLID3_CELLID                           (0xFFFFFFFFu)
        #define FPB_CELLID3_CELLID_MASK                      (0xFFFFFFFFu)
        #define FPB_CELLID3_CELLID_BIT                       (0)
        #define FPB_CELLID3_CELLID_BITS                      (32)

/* NVIC block */
#define BLOCK_NVIC_BASE                                      (0xE000E000u)
#define BLOCK_NVIC_END                                       (0xE000EFFFu)
#define BLOCK_NVIC_SIZE                                      (BLOCK_NVIC_END - BLOCK_NVIC_BASE + 1)

#define NVIC_MCR                                             *((volatile uint32_t *)0xE000E000u)
#define NVIC_MCR_REG                                         *((volatile uint32_t *)0xE000E000u)
#define NVIC_MCR_ADDR                                        (0xE000E000u)
#define NVIC_MCR_RESET                                       (0x00000000u)

#define NVIC_ICTR                                            *((volatile uint32_t *)0xE000E004u)
#define NVIC_ICTR_REG                                        *((volatile uint32_t *)0xE000E004u)
#define NVIC_ICTR_ADDR                                       (0xE000E004u)
#define NVIC_ICTR_RESET                                      (0x00000000u)
        /* INTLINESNUM field */
        #define NVIC_ICTR_INTLINESNUM                        (0x0000001Fu)
        #define NVIC_ICTR_INTLINESNUM_MASK                   (0x0000001Fu)
        #define NVIC_ICTR_INTLINESNUM_BIT                    (0)
        #define NVIC_ICTR_INTLINESNUM_BITS                   (5)

#define ST_CSR                                               *((volatile uint32_t *)0xE000E010u)
#define ST_CSR_REG                                           *((volatile uint32_t *)0xE000E010u)
#define ST_CSR_ADDR                                          (0xE000E010u)
#define ST_CSR_RESET                                         (0x00000000u)
        /* COUNTFLAG field */
        #define ST_CSR_COUNTFLAG                             (0x00010000u)
        #define ST_CSR_COUNTFLAG_MASK                        (0x00010000u)
        #define ST_CSR_COUNTFLAG_BIT                         (16)
        #define ST_CSR_COUNTFLAG_BITS                        (1)
        /* CLKSOURCE field */
        #define ST_CSR_CLKSOURCE                             (0x00000004u)
        #define ST_CSR_CLKSOURCE_MASK                        (0x00000004u)
        #define ST_CSR_CLKSOURCE_BIT                         (2)
        #define ST_CSR_CLKSOURCE_BITS                        (1)
        /* TICKINT field */
        #define ST_CSR_TICKINT                               (0x00000002u)
        #define ST_CSR_TICKINT_MASK                          (0x00000002u)
        #define ST_CSR_TICKINT_BIT                           (1)
        #define ST_CSR_TICKINT_BITS                          (1)
        /* ENABLE field */
        #define ST_CSR_ENABLE                                (0x00000001u)
        #define ST_CSR_ENABLE_MASK                           (0x00000001u)
        #define ST_CSR_ENABLE_BIT                            (0)
        #define ST_CSR_ENABLE_BITS                           (1)

#define ST_RVR                                               *((volatile uint32_t *)0xE000E014u)
#define ST_RVR_REG                                           *((volatile uint32_t *)0xE000E014u)
#define ST_RVR_ADDR                                          (0xE000E014u)
#define ST_RVR_RESET                                         (0x00000000u)
        /* RELOAD field */
        #define ST_RVR_RELOAD                                (0x00FFFFFFu)
        #define ST_RVR_RELOAD_MASK                           (0x00FFFFFFu)
        #define ST_RVR_RELOAD_BIT                            (0)
        #define ST_RVR_RELOAD_BITS                           (24)

#define ST_CVR                                               *((volatile uint32_t *)0xE000E018u)
#define ST_CVR_REG                                           *((volatile uint32_t *)0xE000E018u)
#define ST_CVR_ADDR                                          (0xE000E018u)
#define ST_CVR_RESET                                         (0x00000000u)
        /* CURRENT field */
        #define ST_CVR_CURRENT                               (0xFFFFFFFFu)
        #define ST_CVR_CURRENT_MASK                          (0xFFFFFFFFu)
        #define ST_CVR_CURRENT_BIT                           (0)
        #define ST_CVR_CURRENT_BITS                          (32)

#define ST_CALVR                                             *((volatile uint32_t *)0xE000E01Cu)
#define ST_CALVR_REG                                         *((volatile uint32_t *)0xE000E01Cu)
#define ST_CALVR_ADDR                                        (0xE000E01Cu)
#define ST_CALVR_RESET                                       (0x00000000u)
        /* NOREF field */
        #define ST_CALVR_NOREF                               (0x80000000u)
        #define ST_CALVR_NOREF_MASK                          (0x80000000u)
        #define ST_CALVR_NOREF_BIT                           (31)
        #define ST_CALVR_NOREF_BITS                          (1)
        /* SKEW field */
        #define ST_CALVR_SKEW                                (0x40000000u)
        #define ST_CALVR_SKEW_MASK                           (0x40000000u)
        #define ST_CALVR_SKEW_BIT                            (30)
        #define ST_CALVR_SKEW_BITS                           (1)
        /* TENMS field */
        #define ST_CALVR_TENMS                               (0x00FFFFFFu)
        #define ST_CALVR_TENMS_MASK                          (0x00FFFFFFu)
        #define ST_CALVR_TENMS_BIT                           (0)
        #define ST_CALVR_TENMS_BITS                          (24)

#define NVIC_IPR_3to0                                        *((volatile uint32_t *)0xE000E400u)
#define NVIC_IPR_3to0_REG                                    *((volatile uint32_t *)0xE000E400u)
#define NVIC_IPR_3to0_ADDR                                   (0xE000E400u)
#define NVIC_IPR_3to0_RESET                                  (0x00000000u)
        /* PRI_3 field */
        #define NVIC_IPR_3to0_PRI_3                          (0xFF000000u)
        #define NVIC_IPR_3to0_PRI_3_MASK                     (0xFF000000u)
        #define NVIC_IPR_3to0_PRI_3_BIT                      (24)
        #define NVIC_IPR_3to0_PRI_3_BITS                     (8)
        /* PRI_2 field */
        #define NVIC_IPR_3to0_PRI_2                          (0x00FF0000u)
        #define NVIC_IPR_3to0_PRI_2_MASK                     (0x00FF0000u)
        #define NVIC_IPR_3to0_PRI_2_BIT                      (16)
        #define NVIC_IPR_3to0_PRI_2_BITS                     (8)
        /* PRI_1 field */
        #define NVIC_IPR_3to0_PRI_1                          (0x0000FF00u)
        #define NVIC_IPR_3to0_PRI_1_MASK                     (0x0000FF00u)
        #define NVIC_IPR_3to0_PRI_1_BIT                      (8)
        #define NVIC_IPR_3to0_PRI_1_BITS                     (8)
        /* PRI_0 field */
        #define NVIC_IPR_3to0_PRI_0                          (0x000000FFu)
        #define NVIC_IPR_3to0_PRI_0_MASK                     (0x000000FFu)
        #define NVIC_IPR_3to0_PRI_0_BIT                      (0)
        #define NVIC_IPR_3to0_PRI_0_BITS                     (8)

#define NVIC_IPR_7to4                                        *((volatile uint32_t *)0xE000E404u)
#define NVIC_IPR_7to4_REG                                    *((volatile uint32_t *)0xE000E404u)
#define NVIC_IPR_7to4_ADDR                                   (0xE000E404u)
#define NVIC_IPR_7to4_RESET                                  (0x00000000u)
        /* PRI_7 field */
        #define NVIC_IPR_7to4_PRI_7                          (0xFF000000u)
        #define NVIC_IPR_7to4_PRI_7_MASK                     (0xFF000000u)
        #define NVIC_IPR_7to4_PRI_7_BIT                      (24)
        #define NVIC_IPR_7to4_PRI_7_BITS                     (8)
        /* PRI_6 field */
        #define NVIC_IPR_7to4_PRI_6                          (0x00FF0000u)
        #define NVIC_IPR_7to4_PRI_6_MASK                     (0x00FF0000u)
        #define NVIC_IPR_7to4_PRI_6_BIT                      (16)
        #define NVIC_IPR_7to4_PRI_6_BITS                     (8)
        /* PRI_5 field */
        #define NVIC_IPR_7to4_PRI_5                          (0x0000FF00u)
        #define NVIC_IPR_7to4_PRI_5_MASK                     (0x0000FF00u)
        #define NVIC_IPR_7to4_PRI_5_BIT                      (8)
        #define NVIC_IPR_7to4_PRI_5_BITS                     (8)
        /* PRI_4 field */
        #define NVIC_IPR_7to4_PRI_4                          (0x000000FFu)
        #define NVIC_IPR_7to4_PRI_4_MASK                     (0x000000FFu)
        #define NVIC_IPR_7to4_PRI_4_BIT                      (0)
        #define NVIC_IPR_7to4_PRI_4_BITS                     (8)

#define NVIC_IPR_11to8                                       *((volatile uint32_t *)0xE000E408u)
#define NVIC_IPR_11to8_REG                                   *((volatile uint32_t *)0xE000E408u)
#define NVIC_IPR_11to8_ADDR                                  (0xE000E408u)
#define NVIC_IPR_11to8_RESET                                 (0x00000000u)
        /* PRI_11 field */
        #define NVIC_IPR_11to8_PRI_11                        (0xFF000000u)
        #define NVIC_IPR_11to8_PRI_11_MASK                   (0xFF000000u)
        #define NVIC_IPR_11to8_PRI_11_BIT                    (24)
        #define NVIC_IPR_11to8_PRI_11_BITS                   (8)
        /* PRI_10 field */
        #define NVIC_IPR_11to8_PRI_10                        (0x00FF0000u)
        #define NVIC_IPR_11to8_PRI_10_MASK                   (0x00FF0000u)
        #define NVIC_IPR_11to8_PRI_10_BIT                    (16)
        #define NVIC_IPR_11to8_PRI_10_BITS                   (8)
        /* PRI_9 field */
        #define NVIC_IPR_11to8_PRI_9                         (0x0000FF00u)
        #define NVIC_IPR_11to8_PRI_9_MASK                    (0x0000FF00u)
        #define NVIC_IPR_11to8_PRI_9_BIT                     (8)
        #define NVIC_IPR_11to8_PRI_9_BITS                    (8)
        /* PRI_8 field */
        #define NVIC_IPR_11to8_PRI_8                         (0x000000FFu)
        #define NVIC_IPR_11to8_PRI_8_MASK                    (0x000000FFu)
        #define NVIC_IPR_11to8_PRI_8_BIT                     (0)
        #define NVIC_IPR_11to8_PRI_8_BITS                    (8)

#define NVIC_IPR_15to12                                      *((volatile uint32_t *)0xE000E40Cu)
#define NVIC_IPR_15to12_REG                                  *((volatile uint32_t *)0xE000E40Cu)
#define NVIC_IPR_15to12_ADDR                                 (0xE000E40Cu)
#define NVIC_IPR_15to12_RESET                                (0x00000000u)
        /* PRI_15 field */
        #define NVIC_IPR_15to12_PRI_15                       (0xFF000000u)
        #define NVIC_IPR_15to12_PRI_15_MASK                  (0xFF000000u)
        #define NVIC_IPR_15to12_PRI_15_BIT                   (24)
        #define NVIC_IPR_15to12_PRI_15_BITS                  (8)
        /* PRI_14 field */
        #define NVIC_IPR_15to12_PRI_14                       (0x00FF0000u)
        #define NVIC_IPR_15to12_PRI_14_MASK                  (0x00FF0000u)
        #define NVIC_IPR_15to12_PRI_14_BIT                   (16)
        #define NVIC_IPR_15to12_PRI_14_BITS                  (8)
        /* PRI_13 field */
        #define NVIC_IPR_15to12_PRI_13                       (0x0000FF00u)
        #define NVIC_IPR_15to12_PRI_13_MASK                  (0x0000FF00u)
        #define NVIC_IPR_15to12_PRI_13_BIT                   (8)
        #define NVIC_IPR_15to12_PRI_13_BITS                  (8)
        /* PRI_12 field */
        #define NVIC_IPR_15to12_PRI_12                       (0x000000FFu)
        #define NVIC_IPR_15to12_PRI_12_MASK                  (0x000000FFu)
        #define NVIC_IPR_15to12_PRI_12_BIT                   (0)
        #define NVIC_IPR_15to12_PRI_12_BITS                  (8)

#define NVIC_IPR_19to16                                      *((volatile uint32_t *)0xE000E410u)
#define NVIC_IPR_19to16_REG                                  *((volatile uint32_t *)0xE000E410u)
#define NVIC_IPR_19to16_ADDR                                 (0xE000E410u)
#define NVIC_IPR_19to16_RESET                                (0x00000000u)
        /* PRI_19 field */
        #define NVIC_IPR_19to16_PRI_19                       (0xFF000000u)
        #define NVIC_IPR_19to16_PRI_19_MASK                  (0xFF000000u)
        #define NVIC_IPR_19to16_PRI_19_BIT                   (24)
        #define NVIC_IPR_19to16_PRI_19_BITS                  (8)
        /* PRI_18 field */
        #define NVIC_IPR_19to16_PRI_18                       (0x00FF0000u)
        #define NVIC_IPR_19to16_PRI_18_MASK                  (0x00FF0000u)
        #define NVIC_IPR_19to16_PRI_18_BIT                   (16)
        #define NVIC_IPR_19to16_PRI_18_BITS                  (8)
        /* PRI_17 field */
        #define NVIC_IPR_19to16_PRI_17                       (0x0000FF00u)
        #define NVIC_IPR_19to16_PRI_17_MASK                  (0x0000FF00u)
        #define NVIC_IPR_19to16_PRI_17_BIT                   (8)
        #define NVIC_IPR_19to16_PRI_17_BITS                  (8)
        /* PRI_16 field */
        #define NVIC_IPR_19to16_PRI_16                       (0x000000FFu)
        #define NVIC_IPR_19to16_PRI_16_MASK                  (0x000000FFu)
        #define NVIC_IPR_19to16_PRI_16_BIT                   (0)
        #define NVIC_IPR_19to16_PRI_16_BITS                  (8)

#define SCS_CPUID                                            *((volatile uint32_t *)0xE000ED00u)
#define SCS_CPUID_REG                                        *((volatile uint32_t *)0xE000ED00u)
#define SCS_CPUID_ADDR                                       (0xE000ED00u)
#define SCS_CPUID_RESET                                      (0x411FC231u)
        /* IMPLEMENTER field */
        #define SCS_CPUID_IMPLEMENTER                        (0xFF000000u)
        #define SCS_CPUID_IMPLEMENTER_MASK                   (0xFF000000u)
        #define SCS_CPUID_IMPLEMENTER_BIT                    (24)
        #define SCS_CPUID_IMPLEMENTER_BITS                   (8)
        /* VARIANT field */
        #define SCS_CPUID_VARIANT                            (0x00F00000u)
        #define SCS_CPUID_VARIANT_MASK                       (0x00F00000u)
        #define SCS_CPUID_VARIANT_BIT                        (20)
        #define SCS_CPUID_VARIANT_BITS                       (4)
        /* CONSTANT field */
        #define SCS_CPUID_CONSTANT                           (0x000F0000u)
        #define SCS_CPUID_CONSTANT_MASK                      (0x000F0000u)
        #define SCS_CPUID_CONSTANT_BIT                       (16)
        #define SCS_CPUID_CONSTANT_BITS                      (4)
        /* PARTNO field */
        #define SCS_CPUID_PARTNO                             (0x0000FFF0u)
        #define SCS_CPUID_PARTNO_MASK                        (0x0000FFF0u)
        #define SCS_CPUID_PARTNO_BIT                         (4)
        #define SCS_CPUID_PARTNO_BITS                        (12)
        /* REVISION field */
        #define SCS_CPUID_REVISION                           (0x0000000Fu)
        #define SCS_CPUID_REVISION_MASK                      (0x0000000Fu)
        #define SCS_CPUID_REVISION_BIT                       (0)
        #define SCS_CPUID_REVISION_BITS                      (4)

#define SCS_ICSR                                             *((volatile uint32_t *)0xE000ED04u)
#define SCS_ICSR_REG                                         *((volatile uint32_t *)0xE000ED04u)
#define SCS_ICSR_ADDR                                        (0xE000ED04u)
#define SCS_ICSR_RESET                                       (0x00000000u)
        /* NMIPENDSET field */
        #define SCS_ICSR_NMIPENDSET                          (0x80000000u)
        #define SCS_ICSR_NMIPENDSET_MASK                     (0x80000000u)
        #define SCS_ICSR_NMIPENDSET_BIT                      (31)
        #define SCS_ICSR_NMIPENDSET_BITS                     (1)
        /* PENDSVSET field */
        #define SCS_ICSR_PENDSVSET                           (0x10000000u)
        #define SCS_ICSR_PENDSVSET_MASK                      (0x10000000u)
        #define SCS_ICSR_PENDSVSET_BIT                       (28)
        #define SCS_ICSR_PENDSVSET_BITS                      (1)
        /* PENDSVCLR field */
        #define SCS_ICSR_PENDSVCLR                           (0x08000000u)
        #define SCS_ICSR_PENDSVCLR_MASK                      (0x08000000u)
        #define SCS_ICSR_PENDSVCLR_BIT                       (27)
        #define SCS_ICSR_PENDSVCLR_BITS                      (1)
        /* PENDSTSET field */
        #define SCS_ICSR_PENDSTSET                           (0x04000000u)
        #define SCS_ICSR_PENDSTSET_MASK                      (0x04000000u)
        #define SCS_ICSR_PENDSTSET_BIT                       (26)
        #define SCS_ICSR_PENDSTSET_BITS                      (1)
        /* PENDSTCLR field */
        #define SCS_ICSR_PENDSTCLR                           (0x02000000u)
        #define SCS_ICSR_PENDSTCLR_MASK                      (0x02000000u)
        #define SCS_ICSR_PENDSTCLR_BIT                       (25)
        #define SCS_ICSR_PENDSTCLR_BITS                      (1)
        /* ISRPREEMPT field */
        #define SCS_ICSR_ISRPREEMPT                          (0x00800000u)
        #define SCS_ICSR_ISRPREEMPT_MASK                     (0x00800000u)
        #define SCS_ICSR_ISRPREEMPT_BIT                      (23)
        #define SCS_ICSR_ISRPREEMPT_BITS                     (1)
        /* ISRPENDING field */
        #define SCS_ICSR_ISRPENDING                          (0x00400000u)
        #define SCS_ICSR_ISRPENDING_MASK                     (0x00400000u)
        #define SCS_ICSR_ISRPENDING_BIT                      (22)
        #define SCS_ICSR_ISRPENDING_BITS                     (1)
        /* VECTPENDING field */
        #define SCS_ICSR_VECTPENDING                         (0x001FF000u)
        #define SCS_ICSR_VECTPENDING_MASK                    (0x001FF000u)
        #define SCS_ICSR_VECTPENDING_BIT                     (12)
        #define SCS_ICSR_VECTPENDING_BITS                    (9)
        /* RETTOBASE field */
        #define SCS_ICSR_RETTOBASE                           (0x00000800u)
        #define SCS_ICSR_RETTOBASE_MASK                      (0x00000800u)
        #define SCS_ICSR_RETTOBASE_BIT                       (11)
        #define SCS_ICSR_RETTOBASE_BITS                      (1)
        /* VECACTIVE field */
        #define SCS_ICSR_VECACTIVE                           (0x000001FFu)
        #define SCS_ICSR_VECACTIVE_MASK                      (0x000001FFu)
        #define SCS_ICSR_VECACTIVE_BIT                       (0)
        #define SCS_ICSR_VECACTIVE_BITS                      (9)

#define SCS_VTOR                                             *((volatile uint32_t *)0xE000ED08u)
#define SCS_VTOR_REG                                         *((volatile uint32_t *)0xE000ED08u)
#define SCS_VTOR_ADDR                                        (0xE000ED08u)
#define SCS_VTOR_RESET                                       (0x00000000u)
        /* TBLBASE field */
        #define SCS_VTOR_TBLBASE                             (0x20000000u)
        #define SCS_VTOR_TBLBASE_MASK                        (0x20000000u)
        #define SCS_VTOR_TBLBASE_BIT                         (29)
        #define SCS_VTOR_TBLBASE_BITS                        (1)
        /* TBLOFF field */
        #define SCS_VTOR_TBLOFF                              (0x1FFFFF00u)
        #define SCS_VTOR_TBLOFF_MASK                         (0x1FFFFF00u)
        #define SCS_VTOR_TBLOFF_BIT                          (8)
        #define SCS_VTOR_TBLOFF_BITS                         (21)

#define SCS_AIRCR                                            *((volatile uint32_t *)0xE000ED0Cu)
#define SCS_AIRCR_REG                                        *((volatile uint32_t *)0xE000ED0Cu)
#define SCS_AIRCR_ADDR                                       (0xE000ED0Cu)
#define SCS_AIRCR_RESET                                      (0x00000000u)
        /* VECTKEYSTAT field */
        #define SCS_AIRCR_VECTKEYSTAT                        (0xFFFF0000u)
        #define SCS_AIRCR_VECTKEYSTAT_MASK                   (0xFFFF0000u)
        #define SCS_AIRCR_VECTKEYSTAT_BIT                    (16)
        #define SCS_AIRCR_VECTKEYSTAT_BITS                   (16)
        /* VECTKEY field */
        #define SCS_AIRCR_VECTKEY                            (0xFFFF0000u)
        #define SCS_AIRCR_VECTKEY_MASK                       (0xFFFF0000u)
        #define SCS_AIRCR_VECTKEY_BIT                        (16)
        #define SCS_AIRCR_VECTKEY_BITS                       (16)
        /* ENDIANESS field */
        #define SCS_AIRCR_ENDIANESS                          (0x00008000u)
        #define SCS_AIRCR_ENDIANESS_MASK                     (0x00008000u)
        #define SCS_AIRCR_ENDIANESS_BIT                      (15)
        #define SCS_AIRCR_ENDIANESS_BITS                     (1)
        /* PRIGROUP field */
        #define SCS_AIRCR_PRIGROUP                           (0x00000700u)
        #define SCS_AIRCR_PRIGROUP_MASK                      (0x00000700u)
        #define SCS_AIRCR_PRIGROUP_BIT                       (8)
        #define SCS_AIRCR_PRIGROUP_BITS                      (3)
        /* SYSRESETREQ field */
        #define SCS_AIRCR_SYSRESETREQ                        (0x00000004u)
        #define SCS_AIRCR_SYSRESETREQ_MASK                   (0x00000004u)
        #define SCS_AIRCR_SYSRESETREQ_BIT                    (2)
        #define SCS_AIRCR_SYSRESETREQ_BITS                   (1)
        /* VECTCLRACTIVE field */
        #define SCS_AIRCR_VECTCLRACTIVE                      (0x00000002u)
        #define SCS_AIRCR_VECTCLRACTIVE_MASK                 (0x00000002u)
        #define SCS_AIRCR_VECTCLRACTIVE_BIT                  (1)
        #define SCS_AIRCR_VECTCLRACTIVE_BITS                 (1)
        /* VECTRESET field */
        #define SCS_AIRCR_VECTRESET                          (0x00000001u)
        #define SCS_AIRCR_VECTRESET_MASK                     (0x00000001u)
        #define SCS_AIRCR_VECTRESET_BIT                      (0)
        #define SCS_AIRCR_VECTRESET_BITS                     (1)

#define SCS_SCR                                              *((volatile uint32_t *)0xE000ED10u)
#define SCS_SCR_REG                                          *((volatile uint32_t *)0xE000ED10u)
#define SCS_SCR_ADDR                                         (0xE000ED10u)
#define SCS_SCR_RESET                                        (0x00000000u)
        /* SEVONPEND field */
        #define SCS_SCR_SEVONPEND                            (0x00000010u)
        #define SCS_SCR_SEVONPEND_MASK                       (0x00000010u)
        #define SCS_SCR_SEVONPEND_BIT                        (4)
        #define SCS_SCR_SEVONPEND_BITS                       (1)
        /* SLEEPDEEP field */
        #define SCS_SCR_SLEEPDEEP                            (0x00000004u)
        #define SCS_SCR_SLEEPDEEP_MASK                       (0x00000004u)
        #define SCS_SCR_SLEEPDEEP_BIT                        (2)
        #define SCS_SCR_SLEEPDEEP_BITS                       (1)
        /* SLEEPONEXIT field */
        #define SCS_SCR_SLEEPONEXIT                          (0x00000002u)
        #define SCS_SCR_SLEEPONEXIT_MASK                     (0x00000002u)
        #define SCS_SCR_SLEEPONEXIT_BIT                      (1)
        #define SCS_SCR_SLEEPONEXIT_BITS                     (1)

#define SCS_CCR                                              *((volatile uint32_t *)0xE000ED14u)
#define SCS_CCR_REG                                          *((volatile uint32_t *)0xE000ED14u)
#define SCS_CCR_ADDR                                         (0xE000ED14u)
#define SCS_CCR_RESET                                        (0x00000000u)
        /* STKALIGN field */
        #define SCS_CCR_STKALIGN                             (0x00000200u)
        #define SCS_CCR_STKALIGN_MASK                        (0x00000200u)
        #define SCS_CCR_STKALIGN_BIT                         (9)
        #define SCS_CCR_STKALIGN_BITS                        (1)
        /* BFHFNMIGN field */
        #define SCS_CCR_BFHFNMIGN                            (0x00000100u)
        #define SCS_CCR_BFHFNMIGN_MASK                       (0x00000100u)
        #define SCS_CCR_BFHFNMIGN_BIT                        (8)
        #define SCS_CCR_BFHFNMIGN_BITS                       (1)
        /* DIV_0_TRP field */
        #define SCS_CCR_DIV_0_TRP                            (0x00000010u)
        #define SCS_CCR_DIV_0_TRP_MASK                       (0x00000010u)
        #define SCS_CCR_DIV_0_TRP_BIT                        (4)
        #define SCS_CCR_DIV_0_TRP_BITS                       (1)
        /* UNALIGN_TRP field */
        #define SCS_CCR_UNALIGN_TRP                          (0x00000008u)
        #define SCS_CCR_UNALIGN_TRP_MASK                     (0x00000008u)
        #define SCS_CCR_UNALIGN_TRP_BIT                      (3)
        #define SCS_CCR_UNALIGN_TRP_BITS                     (1)
        /* USERSETMPEND field */
        #define SCS_CCR_USERSETMPEND                         (0x00000002u)
        #define SCS_CCR_USERSETMPEND_MASK                    (0x00000002u)
        #define SCS_CCR_USERSETMPEND_BIT                     (1)
        #define SCS_CCR_USERSETMPEND_BITS                    (1)
        /* NONBASETHRDENA field */
        #define SCS_CCR_NONBASETHRDENA                       (0x00000001u)
        #define SCS_CCR_NONBASETHRDENA_MASK                  (0x00000001u)
        #define SCS_CCR_NONBASETHRDENA_BIT                   (0)
        #define SCS_CCR_NONBASETHRDENA_BITS                  (1)

#define SCS_SHPR_7to4                                        *((volatile uint32_t *)0xE000ED18u)
#define SCS_SHPR_7to4_REG                                    *((volatile uint32_t *)0xE000ED18u)
#define SCS_SHPR_7to4_ADDR                                   (0xE000ED18u)
#define SCS_SHPR_7to4_RESET                                  (0x00000000u)
        /* PRI_7 field */
        #define SCS_SHPR_7to4_PRI_7                          (0xFF000000u)
        #define SCS_SHPR_7to4_PRI_7_MASK                     (0xFF000000u)
        #define SCS_SHPR_7to4_PRI_7_BIT                      (24)
        #define SCS_SHPR_7to4_PRI_7_BITS                     (8)
        /* PRI_6 field */
        #define SCS_SHPR_7to4_PRI_6                          (0x00FF0000u)
        #define SCS_SHPR_7to4_PRI_6_MASK                     (0x00FF0000u)
        #define SCS_SHPR_7to4_PRI_6_BIT                      (16)
        #define SCS_SHPR_7to4_PRI_6_BITS                     (8)
        /* PRI_5 field */
        #define SCS_SHPR_7to4_PRI_5                          (0x0000FF00u)
        #define SCS_SHPR_7to4_PRI_5_MASK                     (0x0000FF00u)
        #define SCS_SHPR_7to4_PRI_5_BIT                      (8)
        #define SCS_SHPR_7to4_PRI_5_BITS                     (8)
        /* PRI_4 field */
        #define SCS_SHPR_7to4_PRI_4                          (0x000000FFu)
        #define SCS_SHPR_7to4_PRI_4_MASK                     (0x000000FFu)
        #define SCS_SHPR_7to4_PRI_4_BIT                      (0)
        #define SCS_SHPR_7to4_PRI_4_BITS                     (8)

#define SCS_SHPR_11to8                                       *((volatile uint32_t *)0xE000ED1Cu)
#define SCS_SHPR_11to8_REG                                   *((volatile uint32_t *)0xE000ED1Cu)
#define SCS_SHPR_11to8_ADDR                                  (0xE000ED1Cu)
#define SCS_SHPR_11to8_RESET                                 (0x00000000u)
        /* PRI_11 field */
        #define SCS_SHPR_11to8_PRI_11                        (0xFF000000u)
        #define SCS_SHPR_11to8_PRI_11_MASK                   (0xFF000000u)
        #define SCS_SHPR_11to8_PRI_11_BIT                    (24)
        #define SCS_SHPR_11to8_PRI_11_BITS                   (8)
        /* PRI_10 field */
        #define SCS_SHPR_11to8_PRI_10                        (0x00FF0000u)
        #define SCS_SHPR_11to8_PRI_10_MASK                   (0x00FF0000u)
        #define SCS_SHPR_11to8_PRI_10_BIT                    (16)
        #define SCS_SHPR_11to8_PRI_10_BITS                   (8)
        /* PRI_9 field */
        #define SCS_SHPR_11to8_PRI_9                         (0x0000FF00u)
        #define SCS_SHPR_11to8_PRI_9_MASK                    (0x0000FF00u)
        #define SCS_SHPR_11to8_PRI_9_BIT                     (8)
        #define SCS_SHPR_11to8_PRI_9_BITS                    (8)
        /* PRI_8 field */
        #define SCS_SHPR_11to8_PRI_8                         (0x000000FFu)
        #define SCS_SHPR_11to8_PRI_8_MASK                    (0x000000FFu)
        #define SCS_SHPR_11to8_PRI_8_BIT                     (0)
        #define SCS_SHPR_11to8_PRI_8_BITS                    (8)

#define SCS_SHPR_15to12                                      *((volatile uint32_t *)0xE000ED20u)
#define SCS_SHPR_15to12_REG                                  *((volatile uint32_t *)0xE000ED20u)
#define SCS_SHPR_15to12_ADDR                                 (0xE000ED20u)
#define SCS_SHPR_15to12_RESET                                (0x00000000u)
        /* PRI_15 field */
        #define SCS_SHPR_15to12_PRI_15                       (0xFF000000u)
        #define SCS_SHPR_15to12_PRI_15_MASK                  (0xFF000000u)
        #define SCS_SHPR_15to12_PRI_15_BIT                   (24)
        #define SCS_SHPR_15to12_PRI_15_BITS                  (8)
        /* PRI_14 field */
        #define SCS_SHPR_15to12_PRI_14                       (0x00FF0000u)
        #define SCS_SHPR_15to12_PRI_14_MASK                  (0x00FF0000u)
        #define SCS_SHPR_15to12_PRI_14_BIT                   (16)
        #define SCS_SHPR_15to12_PRI_14_BITS                  (8)
        /* PRI_13 field */
        #define SCS_SHPR_15to12_PRI_13                       (0x0000FF00u)
        #define SCS_SHPR_15to12_PRI_13_MASK                  (0x0000FF00u)
        #define SCS_SHPR_15to12_PRI_13_BIT                   (8)
        #define SCS_SHPR_15to12_PRI_13_BITS                  (8)
        /* PRI_12 field */
        #define SCS_SHPR_15to12_PRI_12                       (0x000000FFu)
        #define SCS_SHPR_15to12_PRI_12_MASK                  (0x000000FFu)
        #define SCS_SHPR_15to12_PRI_12_BIT                   (0)
        #define SCS_SHPR_15to12_PRI_12_BITS                  (8)

#define SCS_SHCSR                                            *((volatile uint32_t *)0xE000ED24u)
#define SCS_SHCSR_REG                                        *((volatile uint32_t *)0xE000ED24u)
#define SCS_SHCSR_ADDR                                       (0xE000ED24u)
#define SCS_SHCSR_RESET                                      (0x00000000u)
        /* USGFAULTENA field */
        #define SCS_SHCSR_USGFAULTENA                        (0x00040000u)
        #define SCS_SHCSR_USGFAULTENA_MASK                   (0x00040000u)
        #define SCS_SHCSR_USGFAULTENA_BIT                    (18)
        #define SCS_SHCSR_USGFAULTENA_BITS                   (1)
        /* BUSFAULTENA field */
        #define SCS_SHCSR_BUSFAULTENA                        (0x00020000u)
        #define SCS_SHCSR_BUSFAULTENA_MASK                   (0x00020000u)
        #define SCS_SHCSR_BUSFAULTENA_BIT                    (17)
        #define SCS_SHCSR_BUSFAULTENA_BITS                   (1)
        /* MEMFAULTENA field */
        #define SCS_SHCSR_MEMFAULTENA                        (0x00010000u)
        #define SCS_SHCSR_MEMFAULTENA_MASK                   (0x00010000u)
        #define SCS_SHCSR_MEMFAULTENA_BIT                    (16)
        #define SCS_SHCSR_MEMFAULTENA_BITS                   (1)
        /* SVCALLPENDED field */
        #define SCS_SHCSR_SVCALLPENDED                       (0x00008000u)
        #define SCS_SHCSR_SVCALLPENDED_MASK                  (0x00008000u)
        #define SCS_SHCSR_SVCALLPENDED_BIT                   (15)
        #define SCS_SHCSR_SVCALLPENDED_BITS                  (1)
        /* BUSFAULTPENDED field */
        #define SCS_SHCSR_BUSFAULTPENDED                     (0x00004000u)
        #define SCS_SHCSR_BUSFAULTPENDED_MASK                (0x00004000u)
        #define SCS_SHCSR_BUSFAULTPENDED_BIT                 (14)
        #define SCS_SHCSR_BUSFAULTPENDED_BITS                (1)
        /* MEMFAULTPENDED field */
        #define SCS_SHCSR_MEMFAULTPENDED                     (0x00002000u)
        #define SCS_SHCSR_MEMFAULTPENDED_MASK                (0x00002000u)
        #define SCS_SHCSR_MEMFAULTPENDED_BIT                 (13)
        #define SCS_SHCSR_MEMFAULTPENDED_BITS                (1)
        /* USGFAULTPENDED field */
        #define SCS_SHCSR_USGFAULTPENDED                     (0x00001000u)
        #define SCS_SHCSR_USGFAULTPENDED_MASK                (0x00001000u)
        #define SCS_SHCSR_USGFAULTPENDED_BIT                 (12)
        #define SCS_SHCSR_USGFAULTPENDED_BITS                (1)
        /* SYSTICKACT field */
        #define SCS_SHCSR_SYSTICKACT                         (0x00000800u)
        #define SCS_SHCSR_SYSTICKACT_MASK                    (0x00000800u)
        #define SCS_SHCSR_SYSTICKACT_BIT                     (11)
        #define SCS_SHCSR_SYSTICKACT_BITS                    (1)
        /* PENDSVACT field */
        #define SCS_SHCSR_PENDSVACT                          (0x00000400u)
        #define SCS_SHCSR_PENDSVACT_MASK                     (0x00000400u)
        #define SCS_SHCSR_PENDSVACT_BIT                      (10)
        #define SCS_SHCSR_PENDSVACT_BITS                     (1)
        /* MONITORACT field */
        #define SCS_SHCSR_MONITORACT                         (0x00000100u)
        #define SCS_SHCSR_MONITORACT_MASK                    (0x00000100u)
        #define SCS_SHCSR_MONITORACT_BIT                     (8)
        #define SCS_SHCSR_MONITORACT_BITS                    (1)
        /* SVCALLACT field */
        #define SCS_SHCSR_SVCALLACT                          (0x00000080u)
        #define SCS_SHCSR_SVCALLACT_MASK                     (0x00000080u)
        #define SCS_SHCSR_SVCALLACT_BIT                      (7)
        #define SCS_SHCSR_SVCALLACT_BITS                     (1)
        /* USGFAULTACT field */
        #define SCS_SHCSR_USGFAULTACT                        (0x00000008u)
        #define SCS_SHCSR_USGFAULTACT_MASK                   (0x00000008u)
        #define SCS_SHCSR_USGFAULTACT_BIT                    (3)
        #define SCS_SHCSR_USGFAULTACT_BITS                   (1)
        /* BUSFAULTACT field */
        #define SCS_SHCSR_BUSFAULTACT                        (0x00000002u)
        #define SCS_SHCSR_BUSFAULTACT_MASK                   (0x00000002u)
        #define SCS_SHCSR_BUSFAULTACT_BIT                    (1)
        #define SCS_SHCSR_BUSFAULTACT_BITS                   (1)
        /* MEMFAULTACT field */
        #define SCS_SHCSR_MEMFAULTACT                        (0x00000001u)
        #define SCS_SHCSR_MEMFAULTACT_MASK                   (0x00000001u)
        #define SCS_SHCSR_MEMFAULTACT_BIT                    (0)
        #define SCS_SHCSR_MEMFAULTACT_BITS                   (1)

#define SCS_CFSR                                             *((volatile uint32_t *)0xE000ED28u)
#define SCS_CFSR_REG                                         *((volatile uint32_t *)0xE000ED28u)
#define SCS_CFSR_ADDR                                        (0xE000ED28u)
#define SCS_CFSR_RESET                                       (0x00000000u)
        /* DIVBYZERO field */
        #define SCS_CFSR_DIVBYZERO                           (0x02000000u)
        #define SCS_CFSR_DIVBYZERO_MASK                      (0x02000000u)
        #define SCS_CFSR_DIVBYZERO_BIT                       (25)
        #define SCS_CFSR_DIVBYZERO_BITS                      (1)
        /* UNALIGNED field */
        #define SCS_CFSR_UNALIGNED                           (0x01000000u)
        #define SCS_CFSR_UNALIGNED_MASK                      (0x01000000u)
        #define SCS_CFSR_UNALIGNED_BIT                       (24)
        #define SCS_CFSR_UNALIGNED_BITS                      (1)
        /* NOCP field */
        #define SCS_CFSR_NOCP                                (0x00080000u)
        #define SCS_CFSR_NOCP_MASK                           (0x00080000u)
        #define SCS_CFSR_NOCP_BIT                            (19)
        #define SCS_CFSR_NOCP_BITS                           (1)
        /* INVPC field */
        #define SCS_CFSR_INVPC                               (0x00040000u)
        #define SCS_CFSR_INVPC_MASK                          (0x00040000u)
        #define SCS_CFSR_INVPC_BIT                           (18)
        #define SCS_CFSR_INVPC_BITS                          (1)
        /* INVSTATE field */
        #define SCS_CFSR_INVSTATE                            (0x00020000u)
        #define SCS_CFSR_INVSTATE_MASK                       (0x00020000u)
        #define SCS_CFSR_INVSTATE_BIT                        (17)
        #define SCS_CFSR_INVSTATE_BITS                       (1)
        /* UNDEFINSTR field */
        #define SCS_CFSR_UNDEFINSTR                          (0x00010000u)
        #define SCS_CFSR_UNDEFINSTR_MASK                     (0x00010000u)
        #define SCS_CFSR_UNDEFINSTR_BIT                      (16)
        #define SCS_CFSR_UNDEFINSTR_BITS                     (1)
        /* BFARVALID field */
        #define SCS_CFSR_BFARVALID                           (0x00008000u)
        #define SCS_CFSR_BFARVALID_MASK                      (0x00008000u)
        #define SCS_CFSR_BFARVALID_BIT                       (15)
        #define SCS_CFSR_BFARVALID_BITS                      (1)
        /* STKERR field */
        #define SCS_CFSR_STKERR                              (0x00001000u)
        #define SCS_CFSR_STKERR_MASK                         (0x00001000u)
        #define SCS_CFSR_STKERR_BIT                          (12)
        #define SCS_CFSR_STKERR_BITS                         (1)
        /* UNSTKERR field */
        #define SCS_CFSR_UNSTKERR                            (0x00000800u)
        #define SCS_CFSR_UNSTKERR_MASK                       (0x00000800u)
        #define SCS_CFSR_UNSTKERR_BIT                        (11)
        #define SCS_CFSR_UNSTKERR_BITS                       (1)
        /* IMPRECISERR field */
        #define SCS_CFSR_IMPRECISERR                         (0x00000400u)
        #define SCS_CFSR_IMPRECISERR_MASK                    (0x00000400u)
        #define SCS_CFSR_IMPRECISERR_BIT                     (10)
        #define SCS_CFSR_IMPRECISERR_BITS                    (1)
        /* PRECISERR field */
        #define SCS_CFSR_PRECISERR                           (0x00000200u)
        #define SCS_CFSR_PRECISERR_MASK                      (0x00000200u)
        #define SCS_CFSR_PRECISERR_BIT                       (9)
        #define SCS_CFSR_PRECISERR_BITS                      (1)
        /* IBUSERR field */
        #define SCS_CFSR_IBUSERR                             (0x00000100u)
        #define SCS_CFSR_IBUSERR_MASK                        (0x00000100u)
        #define SCS_CFSR_IBUSERR_BIT                         (8)
        #define SCS_CFSR_IBUSERR_BITS                        (1)
        /* MMARVALID field */
        #define SCS_CFSR_MMARVALID                           (0x00000080u)
        #define SCS_CFSR_MMARVALID_MASK                      (0x00000080u)
        #define SCS_CFSR_MMARVALID_BIT                       (7)
        #define SCS_CFSR_MMARVALID_BITS                      (1)
        /* MSTKERR field */
        #define SCS_CFSR_MSTKERR                             (0x00000010u)
        #define SCS_CFSR_MSTKERR_MASK                        (0x00000010u)
        #define SCS_CFSR_MSTKERR_BIT                         (4)
        #define SCS_CFSR_MSTKERR_BITS                        (1)
        /* MUNSTKERR field */
        #define SCS_CFSR_MUNSTKERR                           (0x00000008u)
        #define SCS_CFSR_MUNSTKERR_MASK                      (0x00000008u)
        #define SCS_CFSR_MUNSTKERR_BIT                       (3)
        #define SCS_CFSR_MUNSTKERR_BITS                      (1)
        /* DACCVIOL field */
        #define SCS_CFSR_DACCVIOL                            (0x00000002u)
        #define SCS_CFSR_DACCVIOL_MASK                       (0x00000002u)
        #define SCS_CFSR_DACCVIOL_BIT                        (1)
        #define SCS_CFSR_DACCVIOL_BITS                       (1)
        /* IACCVIOL field */
        #define SCS_CFSR_IACCVIOL                            (0x00000001u)
        #define SCS_CFSR_IACCVIOL_MASK                       (0x00000001u)
        #define SCS_CFSR_IACCVIOL_BIT                        (0)
        #define SCS_CFSR_IACCVIOL_BITS                       (1)

#define SCS_HFSR                                             *((volatile uint32_t *)0xE000ED2Cu)
#define SCS_HFSR_REG                                         *((volatile uint32_t *)0xE000ED2Cu)
#define SCS_HFSR_ADDR                                        (0xE000ED2Cu)
#define SCS_HFSR_RESET                                       (0x00000000u)
        /* DEBUGEVT field */
        #define SCS_HFSR_DEBUGEVT                            (0x80000000u)
        #define SCS_HFSR_DEBUGEVT_MASK                       (0x80000000u)
        #define SCS_HFSR_DEBUGEVT_BIT                        (31)
        #define SCS_HFSR_DEBUGEVT_BITS                       (1)
        /* FORCED field */
        #define SCS_HFSR_FORCED                              (0x40000000u)
        #define SCS_HFSR_FORCED_MASK                         (0x40000000u)
        #define SCS_HFSR_FORCED_BIT                          (30)
        #define SCS_HFSR_FORCED_BITS                         (1)
        /* VECTTBL field */
        #define SCS_HFSR_VECTTBL                             (0x00000002u)
        #define SCS_HFSR_VECTTBL_MASK                        (0x00000002u)
        #define SCS_HFSR_VECTTBL_BIT                         (1)
        #define SCS_HFSR_VECTTBL_BITS                        (1)

#define SCS_DFSR                                             *((volatile uint32_t *)0xE000ED30u)
#define SCS_DFSR_REG                                         *((volatile uint32_t *)0xE000ED30u)
#define SCS_DFSR_ADDR                                        (0xE000ED30u)
#define SCS_DFSR_RESET                                       (0x00000000u)
        /* EXTERNAL field */
        #define SCS_DFSR_EXTERNAL                            (0x00000010u)
        #define SCS_DFSR_EXTERNAL_MASK                       (0x00000010u)
        #define SCS_DFSR_EXTERNAL_BIT                        (4)
        #define SCS_DFSR_EXTERNAL_BITS                       (1)
        /* VCATCH field */
        #define SCS_DFSR_VCATCH                              (0x00000008u)
        #define SCS_DFSR_VCATCH_MASK                         (0x00000008u)
        #define SCS_DFSR_VCATCH_BIT                          (3)
        #define SCS_DFSR_VCATCH_BITS                         (1)
        /* DWTTRAP field */
        #define SCS_DFSR_DWTTRAP                             (0x00000004u)
        #define SCS_DFSR_DWTTRAP_MASK                        (0x00000004u)
        #define SCS_DFSR_DWTTRAP_BIT                         (2)
        #define SCS_DFSR_DWTTRAP_BITS                        (1)
        /* BKPT field */
        #define SCS_DFSR_BKPT                                (0x00000002u)
        #define SCS_DFSR_BKPT_MASK                           (0x00000002u)
        #define SCS_DFSR_BKPT_BIT                            (1)
        #define SCS_DFSR_BKPT_BITS                           (1)
        /* HALTED field */
        #define SCS_DFSR_HALTED                              (0x00000001u)
        #define SCS_DFSR_HALTED_MASK                         (0x00000001u)
        #define SCS_DFSR_HALTED_BIT                          (0)
        #define SCS_DFSR_HALTED_BITS                         (1)

#define SCS_MMAR                                             *((volatile uint32_t *)0xE000ED34u)
#define SCS_MMAR_REG                                         *((volatile uint32_t *)0xE000ED34u)
#define SCS_MMAR_ADDR                                        (0xE000ED34u)
#define SCS_MMAR_RESET                                       (0x00000000u)
        /* ADDRESS field */
        #define SCS_MMAR_ADDRESS                             (0xFFFFFFFFu)
        #define SCS_MMAR_ADDRESS_MASK                        (0xFFFFFFFFu)
        #define SCS_MMAR_ADDRESS_BIT                         (0)
        #define SCS_MMAR_ADDRESS_BITS                        (32)

#define SCS_BFAR                                             *((volatile uint32_t *)0xE000ED38u)
#define SCS_BFAR_REG                                         *((volatile uint32_t *)0xE000ED38u)
#define SCS_BFAR_ADDR                                        (0xE000ED38u)
#define SCS_BFAR_RESET                                       (0x00000000u)
        /* ADDRESS field */
        #define SCS_BFAR_ADDRESS                             (0xFFFFFFFFu)
        #define SCS_BFAR_ADDRESS_MASK                        (0xFFFFFFFFu)
        #define SCS_BFAR_ADDRESS_BIT                         (0)
        #define SCS_BFAR_ADDRESS_BITS                        (32)

#define SCS_PFR0                                             *((volatile uint32_t *)0xE000ED40u)
#define SCS_PFR0_REG                                         *((volatile uint32_t *)0xE000ED40u)
#define SCS_PFR0_ADDR                                        (0xE000ED40u)
#define SCS_PFR0_RESET                                       (0x00000030u)
        /* FEATURE field */
        #define SCS_PFR0_FEATURE                             (0xFFFFFFFFu)
        #define SCS_PFR0_FEATURE_MASK                        (0xFFFFFFFFu)
        #define SCS_PFR0_FEATURE_BIT                         (0)
        #define SCS_PFR0_FEATURE_BITS                        (32)

#define SCS_PFR1                                             *((volatile uint32_t *)0xE000ED44u)
#define SCS_PFR1_REG                                         *((volatile uint32_t *)0xE000ED44u)
#define SCS_PFR1_ADDR                                        (0xE000ED44u)
#define SCS_PFR1_RESET                                       (0x00000200u)
        /* FEATURE field */
        #define SCS_PFR1_FEATURE                             (0xFFFFFFFFu)
        #define SCS_PFR1_FEATURE_MASK                        (0xFFFFFFFFu)
        #define SCS_PFR1_FEATURE_BIT                         (0)
        #define SCS_PFR1_FEATURE_BITS                        (32)

#define SCS_DFR0                                             *((volatile uint32_t *)0xE000ED48u)
#define SCS_DFR0_REG                                         *((volatile uint32_t *)0xE000ED48u)
#define SCS_DFR0_ADDR                                        (0xE000ED48u)
#define SCS_DFR0_RESET                                       (0x00100000u)
        /* FEATURE field */
        #define SCS_DFR0_FEATURE                             (0xFFFFFFFFu)
        #define SCS_DFR0_FEATURE_MASK                        (0xFFFFFFFFu)
        #define SCS_DFR0_FEATURE_BIT                         (0)
        #define SCS_DFR0_FEATURE_BITS                        (32)

#define SCS_AFR0                                             *((volatile uint32_t *)0xE000ED4Cu)
#define SCS_AFR0_REG                                         *((volatile uint32_t *)0xE000ED4Cu)
#define SCS_AFR0_ADDR                                        (0xE000ED4Cu)
#define SCS_AFR0_RESET                                       (0x00000000u)
        /* FEATURE field */
        #define SCS_AFR0_FEATURE                             (0xFFFFFFFFu)
        #define SCS_AFR0_FEATURE_MASK                        (0xFFFFFFFFu)
        #define SCS_AFR0_FEATURE_BIT                         (0)
        #define SCS_AFR0_FEATURE_BITS                        (32)

#define SCS_MMFR0                                            *((volatile uint32_t *)0xE000ED50u)
#define SCS_MMFR0_REG                                        *((volatile uint32_t *)0xE000ED50u)
#define SCS_MMFR0_ADDR                                       (0xE000ED50u)
#define SCS_MMFR0_RESET                                      (0x00000030u)
        /* FEATURE field */
        #define SCS_MMFR0_FEATURE                            (0xFFFFFFFFu)
        #define SCS_MMFR0_FEATURE_MASK                       (0xFFFFFFFFu)
        #define SCS_MMFR0_FEATURE_BIT                        (0)
        #define SCS_MMFR0_FEATURE_BITS                       (32)

#define SCS_MMFR1                                            *((volatile uint32_t *)0xE000ED54u)
#define SCS_MMFR1_REG                                        *((volatile uint32_t *)0xE000ED54u)
#define SCS_MMFR1_ADDR                                       (0xE000ED54u)
#define SCS_MMFR1_RESET                                      (0x00000000u)
        /* FEATURE field */
        #define SCS_MMFR1_FEATURE                            (0xFFFFFFFFu)
        #define SCS_MMFR1_FEATURE_MASK                       (0xFFFFFFFFu)
        #define SCS_MMFR1_FEATURE_BIT                        (0)
        #define SCS_MMFR1_FEATURE_BITS                       (32)

#define SCS_MMFR2                                            *((volatile uint32_t *)0xE000ED58u)
#define SCS_MMFR2_REG                                        *((volatile uint32_t *)0xE000ED58u)
#define SCS_MMFR2_ADDR                                       (0xE000ED58u)
#define SCS_MMFR2_RESET                                      (0x00000000u)
        /* FEATURE field */
        #define SCS_MMFR2_FEATURE                            (0xFFFFFFFFu)
        #define SCS_MMFR2_FEATURE_MASK                       (0xFFFFFFFFu)
        #define SCS_MMFR2_FEATURE_BIT                        (0)
        #define SCS_MMFR2_FEATURE_BITS                       (32)

#define SCS_MMFR3                                            *((volatile uint32_t *)0xE000ED5Cu)
#define SCS_MMFR3_REG                                        *((volatile uint32_t *)0xE000ED5Cu)
#define SCS_MMFR3_ADDR                                       (0xE000ED5Cu)
#define SCS_MMFR3_RESET                                      (0x00000000u)
        /* FEATURE field */
        #define SCS_MMFR3_FEATURE                            (0xFFFFFFFFu)
        #define SCS_MMFR3_FEATURE_MASK                       (0xFFFFFFFFu)
        #define SCS_MMFR3_FEATURE_BIT                        (0)
        #define SCS_MMFR3_FEATURE_BITS                       (32)

#define SCS_ISAFR0                                           *((volatile uint32_t *)0xE000ED60u)
#define SCS_ISAFR0_REG                                       *((volatile uint32_t *)0xE000ED60u)
#define SCS_ISAFR0_ADDR                                      (0xE000ED60u)
#define SCS_ISAFR0_RESET                                     (0x01141110u)
        /* FEATURE field */
        #define SCS_ISAFR0_FEATURE                           (0xFFFFFFFFu)
        #define SCS_ISAFR0_FEATURE_MASK                      (0xFFFFFFFFu)
        #define SCS_ISAFR0_FEATURE_BIT                       (0)
        #define SCS_ISAFR0_FEATURE_BITS                      (32)

#define SCS_ISAFR1                                           *((volatile uint32_t *)0xE000ED64u)
#define SCS_ISAFR1_REG                                       *((volatile uint32_t *)0xE000ED64u)
#define SCS_ISAFR1_ADDR                                      (0xE000ED64u)
#define SCS_ISAFR1_RESET                                     (0x02111000u)
        /* FEATURE field */
        #define SCS_ISAFR1_FEATURE                           (0xFFFFFFFFu)
        #define SCS_ISAFR1_FEATURE_MASK                      (0xFFFFFFFFu)
        #define SCS_ISAFR1_FEATURE_BIT                       (0)
        #define SCS_ISAFR1_FEATURE_BITS                      (32)

#define SCS_ISAFR2                                           *((volatile uint32_t *)0xE000ED68u)
#define SCS_ISAFR2_REG                                       *((volatile uint32_t *)0xE000ED68u)
#define SCS_ISAFR2_ADDR                                      (0xE000ED68u)
#define SCS_ISAFR2_RESET                                     (0x21112231u)
        /* FEATURE field */
        #define SCS_ISAFR2_FEATURE                           (0xFFFFFFFFu)
        #define SCS_ISAFR2_FEATURE_MASK                      (0xFFFFFFFFu)
        #define SCS_ISAFR2_FEATURE_BIT                       (0)
        #define SCS_ISAFR2_FEATURE_BITS                      (32)

#define SCS_ISAFR3                                           *((volatile uint32_t *)0xE000ED6Cu)
#define SCS_ISAFR3_REG                                       *((volatile uint32_t *)0xE000ED6Cu)
#define SCS_ISAFR3_ADDR                                      (0xE000ED6Cu)
#define SCS_ISAFR3_RESET                                     (0x11111110u)
        /* FEATURE field */
        #define SCS_ISAFR3_FEATURE                           (0xFFFFFFFFu)
        #define SCS_ISAFR3_FEATURE_MASK                      (0xFFFFFFFFu)
        #define SCS_ISAFR3_FEATURE_BIT                       (0)
        #define SCS_ISAFR3_FEATURE_BITS                      (32)

#define SCS_ISAFR4                                           *((volatile uint32_t *)0xE000ED70u)
#define SCS_ISAFR4_REG                                       *((volatile uint32_t *)0xE000ED70u)
#define SCS_ISAFR4_ADDR                                      (0xE000ED70u)
#define SCS_ISAFR4_RESET                                     (0x01310102u)
        /* FEATURE field */
        #define SCS_ISAFR4_FEATURE                           (0xFFFFFFFFu)
        #define SCS_ISAFR4_FEATURE_MASK                      (0xFFFFFFFFu)
        #define SCS_ISAFR4_FEATURE_BIT                       (0)
        #define SCS_ISAFR4_FEATURE_BITS                      (32)

#define MPU_TYPE                                             *((volatile uint32_t *)0xE000ED90u)
#define MPU_TYPE_REG                                         *((volatile uint32_t *)0xE000ED90u)
#define MPU_TYPE_ADDR                                        (0xE000ED90u)
#define MPU_TYPE_RESET                                       (0x00000800u)
        /* IREGION field */
        #define MPU_TYPE_IREGION                             (0x00FF0000u)
        #define MPU_TYPE_IREGION_MASK                        (0x00FF0000u)
        #define MPU_TYPE_IREGION_BIT                         (16)
        #define MPU_TYPE_IREGION_BITS                        (8)
        /* DREGION field */
        #define MPU_TYPE_DREGION                             (0x0000FF00u)
        #define MPU_TYPE_DREGION_MASK                        (0x0000FF00u)
        #define MPU_TYPE_DREGION_BIT                         (8)
        #define MPU_TYPE_DREGION_BITS                        (8)

#define MPU_CTRL                                             *((volatile uint32_t *)0xE000ED94u)
#define MPU_CTRL_REG                                         *((volatile uint32_t *)0xE000ED94u)
#define MPU_CTRL_ADDR                                        (0xE000ED94u)
#define MPU_CTRL_RESET                                       (0x00000000u)
        /* PRIVDEFENA field */
        #define MPU_CTRL_PRIVDEFENA                          (0x00000004u)
        #define MPU_CTRL_PRIVDEFENA_MASK                     (0x00000004u)
        #define MPU_CTRL_PRIVDEFENA_BIT                      (2)
        #define MPU_CTRL_PRIVDEFENA_BITS                     (1)
        /* HFNMIENA field */
        #define MPU_CTRL_HFNMIENA                            (0x00000002u)
        #define MPU_CTRL_HFNMIENA_MASK                       (0x00000002u)
        #define MPU_CTRL_HFNMIENA_BIT                        (1)
        #define MPU_CTRL_HFNMIENA_BITS                       (1)
        /* ENABLE field */
        #define MPU_CTRL_ENABLE                              (0x00000001u)
        #define MPU_CTRL_ENABLE_MASK                         (0x00000001u)
        #define MPU_CTRL_ENABLE_BIT                          (0)
        #define MPU_CTRL_ENABLE_BITS                         (1)

#define MPU_REGION                                           *((volatile uint32_t *)0xE000ED98u)
#define MPU_REGION_REG                                       *((volatile uint32_t *)0xE000ED98u)
#define MPU_REGION_ADDR                                      (0xE000ED98u)
#define MPU_REGION_RESET                                     (0x00000000u)
        /* REGION field */
        #define MPU_REGION_REGION                            (0x000000FFu)
        #define MPU_REGION_REGION_MASK                       (0x000000FFu)
        #define MPU_REGION_REGION_BIT                        (0)
        #define MPU_REGION_REGION_BITS                       (8)

#define MPU_BASE                                             *((volatile uint32_t *)0xE000ED9Cu)
#define MPU_BASE_REG                                         *((volatile uint32_t *)0xE000ED9Cu)
#define MPU_BASE_ADDR                                        (0xE000ED9Cu)
#define MPU_BASE_RESET                                       (0x00000000u)
        /* ADDRESS field */
        #define MPU_BASE_ADDRESS                             (0xFFFFFFE0u)
        #define MPU_BASE_ADDRESS_MASK                        (0xFFFFFFE0u)
        #define MPU_BASE_ADDRESS_BIT                         (5)
        #define MPU_BASE_ADDRESS_BITS                        (27)
        /* VALID field */
        #define MPU_BASE_VALID                               (0x00000010u)
        #define MPU_BASE_VALID_MASK                          (0x00000010u)
        #define MPU_BASE_VALID_BIT                           (4)
        #define MPU_BASE_VALID_BITS                          (1)
        /* REGION field */
        #define MPU_BASE_REGION                              (0x0000000Fu)
        #define MPU_BASE_REGION_MASK                         (0x0000000Fu)
        #define MPU_BASE_REGION_BIT                          (0)
        #define MPU_BASE_REGION_BITS                         (4)

#define MPU_ATTR                                             *((volatile uint32_t *)0xE000EDA0u)
#define MPU_ATTR_REG                                         *((volatile uint32_t *)0xE000EDA0u)
#define MPU_ATTR_ADDR                                        (0xE000EDA0u)
#define MPU_ATTR_RESET                                       (0x00000000u)
        /* XN field */
        #define MPU_ATTR_XN                                  (0x10000000u)
        #define MPU_ATTR_XN_MASK                             (0x10000000u)
        #define MPU_ATTR_XN_BIT                              (28)
        #define MPU_ATTR_XN_BITS                             (1)
        /* AP field */
        #define MPU_ATTR_AP                                  (0x07000000u)
        #define MPU_ATTR_AP_MASK                             (0x07000000u)
        #define MPU_ATTR_AP_BIT                              (24)
        #define MPU_ATTR_AP_BITS                             (3)
        /* TEX field */
        #define MPU_ATTR_TEX                                 (0x00380000u)
        #define MPU_ATTR_TEX_MASK                            (0x00380000u)
        #define MPU_ATTR_TEX_BIT                             (19)
        #define MPU_ATTR_TEX_BITS                            (3)
        /* S field */
        #define MPU_ATTR_S                                   (0x00040000u)
        #define MPU_ATTR_S_MASK                              (0x00040000u)
        #define MPU_ATTR_S_BIT                               (18)
        #define MPU_ATTR_S_BITS                              (1)
        /* C field */
        #define MPU_ATTR_C                                   (0x00020000u)
        #define MPU_ATTR_C_MASK                              (0x00020000u)
        #define MPU_ATTR_C_BIT                               (17)
        #define MPU_ATTR_C_BITS                              (1)
        /* B field */
        #define MPU_ATTR_B                                   (0x00010000u)
        #define MPU_ATTR_B_MASK                              (0x00010000u)
        #define MPU_ATTR_B_BIT                               (16)
        #define MPU_ATTR_B_BITS                              (1)
        /* SRD field */
        #define MPU_ATTR_SRD                                 (0x0000FF00u)
        #define MPU_ATTR_SRD_MASK                            (0x0000FF00u)
        #define MPU_ATTR_SRD_BIT                             (8)
        #define MPU_ATTR_SRD_BITS                            (8)
        /* SIZE field */
        #define MPU_ATTR_SIZE                                (0x0000003Eu)
        #define MPU_ATTR_SIZE_MASK                           (0x0000003Eu)
        #define MPU_ATTR_SIZE_BIT                            (1)
        #define MPU_ATTR_SIZE_BITS                           (5)
        /* ENABLE field */
        #define MPU_ATTR_ENABLE                              (0x00000001u)
        #define MPU_ATTR_ENABLE_MASK                         (0x00000001u)
        #define MPU_ATTR_ENABLE_BIT                          (0)
        #define MPU_ATTR_ENABLE_BITS                         (1)

#define MPU_BASE1                                            *((volatile uint32_t *)0xE000EDA4u)
#define MPU_BASE1_REG                                        *((volatile uint32_t *)0xE000EDA4u)
#define MPU_BASE1_ADDR                                       (0xE000EDA4u)
#define MPU_BASE1_RESET                                      (0x00000000u)
        /* ADDRESS field */
        #define MPU_BASE1_ADDRESS                            (0xFFFFFFE0u)
        #define MPU_BASE1_ADDRESS_MASK                       (0xFFFFFFE0u)
        #define MPU_BASE1_ADDRESS_BIT                        (5)
        #define MPU_BASE1_ADDRESS_BITS                       (27)
        /* VALID field */
        #define MPU_BASE1_VALID                              (0x00000010u)
        #define MPU_BASE1_VALID_MASK                         (0x00000010u)
        #define MPU_BASE1_VALID_BIT                          (4)
        #define MPU_BASE1_VALID_BITS                         (1)
        /* REGION field */
        #define MPU_BASE1_REGION                             (0x0000000Fu)
        #define MPU_BASE1_REGION_MASK                        (0x0000000Fu)
        #define MPU_BASE1_REGION_BIT                         (0)
        #define MPU_BASE1_REGION_BITS                        (4)

#define MPU_ATTR1                                            *((volatile uint32_t *)0xE000EDA8u)
#define MPU_ATTR1_REG                                        *((volatile uint32_t *)0xE000EDA8u)
#define MPU_ATTR1_ADDR                                       (0xE000EDA8u)
#define MPU_ATTR1_RESET                                      (0x00000000u)
        /* XN field */
        #define MPU_ATTR1_XN                                 (0x10000000u)
        #define MPU_ATTR1_XN_MASK                            (0x10000000u)
        #define MPU_ATTR1_XN_BIT                             (28)
        #define MPU_ATTR1_XN_BITS                            (1)
        /* AP field */
        #define MPU_ATTR1_AP                                 (0x07000000u)
        #define MPU_ATTR1_AP_MASK                            (0x07000000u)
        #define MPU_ATTR1_AP_BIT                             (24)
        #define MPU_ATTR1_AP_BITS                            (3)
        /* TEX field */
        #define MPU_ATTR1_TEX                                (0x00380000u)
        #define MPU_ATTR1_TEX_MASK                           (0x00380000u)
        #define MPU_ATTR1_TEX_BIT                            (19)
        #define MPU_ATTR1_TEX_BITS                           (3)
        /* S field */
        #define MPU_ATTR1_S                                  (0x00040000u)
        #define MPU_ATTR1_S_MASK                             (0x00040000u)
        #define MPU_ATTR1_S_BIT                              (18)
        #define MPU_ATTR1_S_BITS                             (1)
        /* C field */
        #define MPU_ATTR1_C                                  (0x00020000u)
        #define MPU_ATTR1_C_MASK                             (0x00020000u)
        #define MPU_ATTR1_C_BIT                              (17)
        #define MPU_ATTR1_C_BITS                             (1)
        /* B field */
        #define MPU_ATTR1_B                                  (0x00010000u)
        #define MPU_ATTR1_B_MASK                             (0x00010000u)
        #define MPU_ATTR1_B_BIT                              (16)
        #define MPU_ATTR1_B_BITS                             (1)
        /* SRD field */
        #define MPU_ATTR1_SRD                                (0x0000FF00u)
        #define MPU_ATTR1_SRD_MASK                           (0x0000FF00u)
        #define MPU_ATTR1_SRD_BIT                            (8)
        #define MPU_ATTR1_SRD_BITS                           (8)
        /* SIZE field */
        #define MPU_ATTR1_SIZE                               (0x0000003Eu)
        #define MPU_ATTR1_SIZE_MASK                          (0x0000003Eu)
        #define MPU_ATTR1_SIZE_BIT                           (1)
        #define MPU_ATTR1_SIZE_BITS                          (5)
        /* ENABLE field */
        #define MPU_ATTR1_ENABLE                             (0x00000001u)
        #define MPU_ATTR1_ENABLE_MASK                        (0x00000001u)
        #define MPU_ATTR1_ENABLE_BIT                         (0)
        #define MPU_ATTR1_ENABLE_BITS                        (1)

#define MPU_BASE2                                            *((volatile uint32_t *)0xE000EDACu)
#define MPU_BASE2_REG                                        *((volatile uint32_t *)0xE000EDACu)
#define MPU_BASE2_ADDR                                       (0xE000EDACu)
#define MPU_BASE2_RESET                                      (0x00000000u)
        /* ADDRESS field */
        #define MPU_BASE2_ADDRESS                            (0xFFFFFFE0u)
        #define MPU_BASE2_ADDRESS_MASK                       (0xFFFFFFE0u)
        #define MPU_BASE2_ADDRESS_BIT                        (5)
        #define MPU_BASE2_ADDRESS_BITS                       (27)
        /* VALID field */
        #define MPU_BASE2_VALID                              (0x00000010u)
        #define MPU_BASE2_VALID_MASK                         (0x00000010u)
        #define MPU_BASE2_VALID_BIT                          (4)
        #define MPU_BASE2_VALID_BITS                         (1)
        /* REGION field */
        #define MPU_BASE2_REGION                             (0x0000000Fu)
        #define MPU_BASE2_REGION_MASK                        (0x0000000Fu)
        #define MPU_BASE2_REGION_BIT                         (0)
        #define MPU_BASE2_REGION_BITS                        (4)

#define MPU_ATTR2                                            *((volatile uint32_t *)0xE000EDB0u)
#define MPU_ATTR2_REG                                        *((volatile uint32_t *)0xE000EDB0u)
#define MPU_ATTR2_ADDR                                       (0xE000EDB0u)
#define MPU_ATTR2_RESET                                      (0x00000000u)
        /* XN field */
        #define MPU_ATTR2_XN                                 (0x10000000u)
        #define MPU_ATTR2_XN_MASK                            (0x10000000u)
        #define MPU_ATTR2_XN_BIT                             (28)
        #define MPU_ATTR2_XN_BITS                            (1)
        /* AP field */
        #define MPU_ATTR2_AP                                 (0x1F000000u)
        #define MPU_ATTR2_AP_MASK                            (0x1F000000u)
        #define MPU_ATTR2_AP_BIT                             (24)
        #define MPU_ATTR2_AP_BITS                            (5)
        /* TEX field */
        #define MPU_ATTR2_TEX                                (0x00380000u)
        #define MPU_ATTR2_TEX_MASK                           (0x00380000u)
        #define MPU_ATTR2_TEX_BIT                            (19)
        #define MPU_ATTR2_TEX_BITS                           (3)
        /* S field */
        #define MPU_ATTR2_S                                  (0x00040000u)
        #define MPU_ATTR2_S_MASK                             (0x00040000u)
        #define MPU_ATTR2_S_BIT                              (18)
        #define MPU_ATTR2_S_BITS                             (1)
        /* C field */
        #define MPU_ATTR2_C                                  (0x00020000u)
        #define MPU_ATTR2_C_MASK                             (0x00020000u)
        #define MPU_ATTR2_C_BIT                              (17)
        #define MPU_ATTR2_C_BITS                             (1)
        /* B field */
        #define MPU_ATTR2_B                                  (0x00010000u)
        #define MPU_ATTR2_B_MASK                             (0x00010000u)
        #define MPU_ATTR2_B_BIT                              (16)
        #define MPU_ATTR2_B_BITS                             (1)
        /* SRD field */
        #define MPU_ATTR2_SRD                                (0x0000FF00u)
        #define MPU_ATTR2_SRD_MASK                           (0x0000FF00u)
        #define MPU_ATTR2_SRD_BIT                            (8)
        #define MPU_ATTR2_SRD_BITS                           (8)
        /* SIZE field */
        #define MPU_ATTR2_SIZE                               (0x0000003Eu)
        #define MPU_ATTR2_SIZE_MASK                          (0x0000003Eu)
        #define MPU_ATTR2_SIZE_BIT                           (1)
        #define MPU_ATTR2_SIZE_BITS                          (5)
        /* ENABLE field */
        #define MPU_ATTR2_ENABLE                             (0x00000003u)
        #define MPU_ATTR2_ENABLE_MASK                        (0x00000003u)
        #define MPU_ATTR2_ENABLE_BIT                         (0)
        #define MPU_ATTR2_ENABLE_BITS                        (2)

#define MPU_BASE3                                            *((volatile uint32_t *)0xE000EDB4u)
#define MPU_BASE3_REG                                        *((volatile uint32_t *)0xE000EDB4u)
#define MPU_BASE3_ADDR                                       (0xE000EDB4u)
#define MPU_BASE3_RESET                                      (0x00000000u)
        /* ADDRESS field */
        #define MPU_BASE3_ADDRESS                            (0xFFFFFFE0u)
        #define MPU_BASE3_ADDRESS_MASK                       (0xFFFFFFE0u)
        #define MPU_BASE3_ADDRESS_BIT                        (5)
        #define MPU_BASE3_ADDRESS_BITS                       (27)
        /* VALID field */
        #define MPU_BASE3_VALID                              (0x00000010u)
        #define MPU_BASE3_VALID_MASK                         (0x00000010u)
        #define MPU_BASE3_VALID_BIT                          (4)
        #define MPU_BASE3_VALID_BITS                         (1)
        /* REGION field */
        #define MPU_BASE3_REGION                             (0x0000000Fu)
        #define MPU_BASE3_REGION_MASK                        (0x0000000Fu)
        #define MPU_BASE3_REGION_BIT                         (0)
        #define MPU_BASE3_REGION_BITS                        (4)

#define MPU_ATTR3                                            *((volatile uint32_t *)0xE000EDBCu)
#define MPU_ATTR3_REG                                        *((volatile uint32_t *)0xE000EDBCu)
#define MPU_ATTR3_ADDR                                       (0xE000EDBCu)
#define MPU_ATTR3_RESET                                      (0x00000000u)
        /* XN field */
        #define MPU_ATTR3_XN                                 (0x10000000u)
        #define MPU_ATTR3_XN_MASK                            (0x10000000u)
        #define MPU_ATTR3_XN_BIT                             (28)
        #define MPU_ATTR3_XN_BITS                            (1)
        /* AP field */
        #define MPU_ATTR3_AP                                 (0x1F000000u)
        #define MPU_ATTR3_AP_MASK                            (0x1F000000u)
        #define MPU_ATTR3_AP_BIT                             (24)
        #define MPU_ATTR3_AP_BITS                            (5)
        /* TEX field */
        #define MPU_ATTR3_TEX                                (0x00380000u)
        #define MPU_ATTR3_TEX_MASK                           (0x00380000u)
        #define MPU_ATTR3_TEX_BIT                            (19)
        #define MPU_ATTR3_TEX_BITS                           (3)
        /* S field */
        #define MPU_ATTR3_S                                  (0x00040000u)
        #define MPU_ATTR3_S_MASK                             (0x00040000u)
        #define MPU_ATTR3_S_BIT                              (18)
        #define MPU_ATTR3_S_BITS                             (1)
        /* C field */
        #define MPU_ATTR3_C                                  (0x00020000u)
        #define MPU_ATTR3_C_MASK                             (0x00020000u)
        #define MPU_ATTR3_C_BIT                              (17)
        #define MPU_ATTR3_C_BITS                             (1)
        /* B field */
        #define MPU_ATTR3_B                                  (0x00010000u)
        #define MPU_ATTR3_B_MASK                             (0x00010000u)
        #define MPU_ATTR3_B_BIT                              (16)
        #define MPU_ATTR3_B_BITS                             (1)
        /* SRD field */
        #define MPU_ATTR3_SRD                                (0x0000FF00u)
        #define MPU_ATTR3_SRD_MASK                           (0x0000FF00u)
        #define MPU_ATTR3_SRD_BIT                            (8)
        #define MPU_ATTR3_SRD_BITS                           (8)
        /* SIZE field */
        #define MPU_ATTR3_SIZE                               (0x0000003Eu)
        #define MPU_ATTR3_SIZE_MASK                          (0x0000003Eu)
        #define MPU_ATTR3_SIZE_BIT                           (1)
        #define MPU_ATTR3_SIZE_BITS                          (5)
        /* ENABLE field */
        #define MPU_ATTR3_ENABLE                             (0x00000003u)
        #define MPU_ATTR3_ENABLE_MASK                        (0x00000003u)
        #define MPU_ATTR3_ENABLE_BIT                         (0)
        #define MPU_ATTR3_ENABLE_BITS                        (2)

#define DEBUG_HCSR                                           *((volatile uint32_t *)0xE000EDF0u)
#define DEBUG_HCSR_REG                                       *((volatile uint32_t *)0xE000EDF0u)
#define DEBUG_HCSR_ADDR                                      (0xE000EDF0u)
#define DEBUG_HCSR_RESET                                     (0x00000000u)
        /* S_RESET_ST field */
        #define DEBUG_HCSR_S_RESET_ST                        (0x02000000u)
        #define DEBUG_HCSR_S_RESET_ST_MASK                   (0x02000000u)
        #define DEBUG_HCSR_S_RESET_ST_BIT                    (25)
        #define DEBUG_HCSR_S_RESET_ST_BITS                   (1)
        /* S_RETIRE_ST field */
        #define DEBUG_HCSR_S_RETIRE_ST                       (0x01000000u)
        #define DEBUG_HCSR_S_RETIRE_ST_MASK                  (0x01000000u)
        #define DEBUG_HCSR_S_RETIRE_ST_BIT                   (24)
        #define DEBUG_HCSR_S_RETIRE_ST_BITS                  (1)
        /* S_LOCKUP field */
        #define DEBUG_HCSR_S_LOCKUP                          (0x00080000u)
        #define DEBUG_HCSR_S_LOCKUP_MASK                     (0x00080000u)
        #define DEBUG_HCSR_S_LOCKUP_BIT                      (19)
        #define DEBUG_HCSR_S_LOCKUP_BITS                     (1)
        /* S_SLEEP field */
        #define DEBUG_HCSR_S_SLEEP                           (0x00040000u)
        #define DEBUG_HCSR_S_SLEEP_MASK                      (0x00040000u)
        #define DEBUG_HCSR_S_SLEEP_BIT                       (18)
        #define DEBUG_HCSR_S_SLEEP_BITS                      (1)
        /* S_HALT field */
        #define DEBUG_HCSR_S_HALT                            (0x00020000u)
        #define DEBUG_HCSR_S_HALT_MASK                       (0x00020000u)
        #define DEBUG_HCSR_S_HALT_BIT                        (17)
        #define DEBUG_HCSR_S_HALT_BITS                       (1)
        /* S_REGRDY field */
        #define DEBUG_HCSR_S_REGRDY                          (0x00010000u)
        #define DEBUG_HCSR_S_REGRDY_MASK                     (0x00010000u)
        #define DEBUG_HCSR_S_REGRDY_BIT                      (16)
        #define DEBUG_HCSR_S_REGRDY_BITS                     (1)
        /* DBGKEY field */
        #define DEBUG_HCSR_DBGKEY                            (0xFFFF0000u)
        #define DEBUG_HCSR_DBGKEY_MASK                       (0xFFFF0000u)
        #define DEBUG_HCSR_DBGKEY_BIT                        (16)
        #define DEBUG_HCSR_DBGKEY_BITS                       (16)
        /* C_SNAPSTALL field */
        #define DEBUG_HCSR_C_SNAPSTALL                       (0x00000020u)
        #define DEBUG_HCSR_C_SNAPSTALL_MASK                  (0x00000020u)
        #define DEBUG_HCSR_C_SNAPSTALL_BIT                   (5)
        #define DEBUG_HCSR_C_SNAPSTALL_BITS                  (1)
        /* C_MASKINTS field */
        #define DEBUG_HCSR_C_MASKINTS                        (0x00000008u)
        #define DEBUG_HCSR_C_MASKINTS_MASK                   (0x00000008u)
        #define DEBUG_HCSR_C_MASKINTS_BIT                    (3)
        #define DEBUG_HCSR_C_MASKINTS_BITS                   (1)
        /* C_STEP field */
        #define DEBUG_HCSR_C_STEP                            (0x00000004u)
        #define DEBUG_HCSR_C_STEP_MASK                       (0x00000004u)
        #define DEBUG_HCSR_C_STEP_BIT                        (2)
        #define DEBUG_HCSR_C_STEP_BITS                       (1)
        /* C_HALT field */
        #define DEBUG_HCSR_C_HALT                            (0x00000002u)
        #define DEBUG_HCSR_C_HALT_MASK                       (0x00000002u)
        #define DEBUG_HCSR_C_HALT_BIT                        (1)
        #define DEBUG_HCSR_C_HALT_BITS                       (1)
        /* C_DEBUGEN field */
        #define DEBUG_HCSR_C_DEBUGEN                         (0x00000001u)
        #define DEBUG_HCSR_C_DEBUGEN_MASK                    (0x00000001u)
        #define DEBUG_HCSR_C_DEBUGEN_BIT                     (0)
        #define DEBUG_HCSR_C_DEBUGEN_BITS                    (1)

#define DEBUG_CRSR                                           *((volatile uint32_t *)0xE000EDF4u)
#define DEBUG_CRSR_REG                                       *((volatile uint32_t *)0xE000EDF4u)
#define DEBUG_CRSR_ADDR                                      (0xE000EDF4u)
#define DEBUG_CRSR_RESET                                     (0x00000000u)
        /* REGWnR field */
        #define DEBUG_CRSR_REGWnR                            (0x00010000u)
        #define DEBUG_CRSR_REGWnR_MASK                       (0x00010000u)
        #define DEBUG_CRSR_REGWnR_BIT                        (16)
        #define DEBUG_CRSR_REGWnR_BITS                       (1)
        /* REGSEL field */
        #define DEBUG_CRSR_REGSEL                            (0x0000001Fu)
        #define DEBUG_CRSR_REGSEL_MASK                       (0x0000001Fu)
        #define DEBUG_CRSR_REGSEL_BIT                        (0)
        #define DEBUG_CRSR_REGSEL_BITS                       (5)

#define DEBUG_CRDR                                           *((volatile uint32_t *)0xE000EDF8u)
#define DEBUG_CRDR_REG                                       *((volatile uint32_t *)0xE000EDF8u)
#define DEBUG_CRDR_ADDR                                      (0xE000EDF8u)
#define DEBUG_CRDR_RESET                                     (0x00000000u)
        /* DBGTMP field */
        #define DEBUG_CRDR_DBGTMP                            (0xFFFFFFFFu)
        #define DEBUG_CRDR_DBGTMP_MASK                       (0xFFFFFFFFu)
        #define DEBUG_CRDR_DBGTMP_BIT                        (0)
        #define DEBUG_CRDR_DBGTMP_BITS                       (32)

#define DEBUG_EMCR                                           *((volatile uint32_t *)0xE000EDFCu)
#define DEBUG_EMCR_REG                                       *((volatile uint32_t *)0xE000EDFCu)
#define DEBUG_EMCR_ADDR                                      (0xE000EDFCu)
#define DEBUG_EMCR_RESET                                     (0x00000000u)
        /* TRCENA field */
        #define DEBUG_EMCR_TRCENA                            (0x01000000u)
        #define DEBUG_EMCR_TRCENA_MASK                       (0x01000000u)
        #define DEBUG_EMCR_TRCENA_BIT                        (24)
        #define DEBUG_EMCR_TRCENA_BITS                       (1)
        /* MON_REQ field */
        #define DEBUG_EMCR_MON_REQ                           (0x00080000u)
        #define DEBUG_EMCR_MON_REQ_MASK                      (0x00080000u)
        #define DEBUG_EMCR_MON_REQ_BIT                       (19)
        #define DEBUG_EMCR_MON_REQ_BITS                      (1)
        /* MON_STEP field */
        #define DEBUG_EMCR_MON_STEP                          (0x00040000u)
        #define DEBUG_EMCR_MON_STEP_MASK                     (0x00040000u)
        #define DEBUG_EMCR_MON_STEP_BIT                      (18)
        #define DEBUG_EMCR_MON_STEP_BITS                     (1)
        /* MON_PEND field */
        #define DEBUG_EMCR_MON_PEND                          (0x00020000u)
        #define DEBUG_EMCR_MON_PEND_MASK                     (0x00020000u)
        #define DEBUG_EMCR_MON_PEND_BIT                      (17)
        #define DEBUG_EMCR_MON_PEND_BITS                     (1)
        /* MON_EN field */
        #define DEBUG_EMCR_MON_EN                            (0x00010000u)
        #define DEBUG_EMCR_MON_EN_MASK                       (0x00010000u)
        #define DEBUG_EMCR_MON_EN_BIT                        (16)
        #define DEBUG_EMCR_MON_EN_BITS                       (1)
        /* VC_HARDERR field */
        #define DEBUG_EMCR_VC_HARDERR                        (0x00000400u)
        #define DEBUG_EMCR_VC_HARDERR_MASK                   (0x00000400u)
        #define DEBUG_EMCR_VC_HARDERR_BIT                    (10)
        #define DEBUG_EMCR_VC_HARDERR_BITS                   (1)
        /* VC_INTERR field */
        #define DEBUG_EMCR_VC_INTERR                         (0x00000200u)
        #define DEBUG_EMCR_VC_INTERR_MASK                    (0x00000200u)
        #define DEBUG_EMCR_VC_INTERR_BIT                     (9)
        #define DEBUG_EMCR_VC_INTERR_BITS                    (1)
        /* VC_BUSERR field */
        #define DEBUG_EMCR_VC_BUSERR                         (0x00000100u)
        #define DEBUG_EMCR_VC_BUSERR_MASK                    (0x00000100u)
        #define DEBUG_EMCR_VC_BUSERR_BIT                     (8)
        #define DEBUG_EMCR_VC_BUSERR_BITS                    (1)
        /* VC_STATERR field */
        #define DEBUG_EMCR_VC_STATERR                        (0x00000080u)
        #define DEBUG_EMCR_VC_STATERR_MASK                   (0x00000080u)
        #define DEBUG_EMCR_VC_STATERR_BIT                    (7)
        #define DEBUG_EMCR_VC_STATERR_BITS                   (1)
        /* VC_CHKERR field */
        #define DEBUG_EMCR_VC_CHKERR                         (0x00000040u)
        #define DEBUG_EMCR_VC_CHKERR_MASK                    (0x00000040u)
        #define DEBUG_EMCR_VC_CHKERR_BIT                     (6)
        #define DEBUG_EMCR_VC_CHKERR_BITS                    (1)
        /* VC_NOCPERR field */
        #define DEBUG_EMCR_VC_NOCPERR                        (0x00000020u)
        #define DEBUG_EMCR_VC_NOCPERR_MASK                   (0x00000020u)
        #define DEBUG_EMCR_VC_NOCPERR_BIT                    (5)
        #define DEBUG_EMCR_VC_NOCPERR_BITS                   (1)
        /* VC_MMERR field */
        #define DEBUG_EMCR_VC_MMERR                          (0x00000010u)
        #define DEBUG_EMCR_VC_MMERR_MASK                     (0x00000010u)
        #define DEBUG_EMCR_VC_MMERR_BIT                      (4)
        #define DEBUG_EMCR_VC_MMERR_BITS                     (1)
        /* VC_CORERESET field */
        #define DEBUG_EMCR_VC_CORERESET                      (0x00000001u)
        #define DEBUG_EMCR_VC_CORERESET_MASK                 (0x00000001u)
        #define DEBUG_EMCR_VC_CORERESET_BIT                  (0)
        #define DEBUG_EMCR_VC_CORERESET_BITS                 (1)

#define NVIC_STIR                                            *((volatile uint32_t *)0xE000EF00u)
#define NVIC_STIR_REG                                        *((volatile uint32_t *)0xE000EF00u)
#define NVIC_STIR_ADDR                                       (0xE000EF00u)
#define NVIC_STIR_RESET                                      (0x00000000u)
        /* INTID field */
        #define NVIC_STIR_INTID                              (0x000003FFu)
        #define NVIC_STIR_INTID_MASK                         (0x000003FFu)
        #define NVIC_STIR_INTID_BIT                          (0)
        #define NVIC_STIR_INTID_BITS                         (10)

#define NVIC_PERIPHID4                                       *((volatile uint32_t *)0xE000EFD0u)
#define NVIC_PERIPHID4_REG                                   *((volatile uint32_t *)0xE000EFD0u)
#define NVIC_PERIPHID4_ADDR                                  (0xE000EFD0u)
#define NVIC_PERIPHID4_RESET                                 (0x00000004u)
        /* PERIPHID field */
        #define NVIC_PERIPHID4_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID4_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID4_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID4_PERIPHID_BITS                 (32)

#define NVIC_PERIPHID5                                       *((volatile uint32_t *)0xE000EFD4u)
#define NVIC_PERIPHID5_REG                                   *((volatile uint32_t *)0xE000EFD4u)
#define NVIC_PERIPHID5_ADDR                                  (0xE000EFD4u)
#define NVIC_PERIPHID5_RESET                                 (0x00000000u)
        /* PERIPHID field */
        #define NVIC_PERIPHID5_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID5_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID5_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID5_PERIPHID_BITS                 (32)

#define NVIC_PERIPHID6                                       *((volatile uint32_t *)0xE000EFD8u)
#define NVIC_PERIPHID6_REG                                   *((volatile uint32_t *)0xE000EFD8u)
#define NVIC_PERIPHID6_ADDR                                  (0xE000EFD8u)
#define NVIC_PERIPHID6_RESET                                 (0x00000000u)
        /* PERIPHID field */
        #define NVIC_PERIPHID6_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID6_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID6_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID6_PERIPHID_BITS                 (32)

#define NVIC_PERIPHID7                                       *((volatile uint32_t *)0xE000EFDCu)
#define NVIC_PERIPHID7_REG                                   *((volatile uint32_t *)0xE000EFDCu)
#define NVIC_PERIPHID7_ADDR                                  (0xE000EFDCu)
#define NVIC_PERIPHID7_RESET                                 (0x00000000u)
        /* PERIPHID field */
        #define NVIC_PERIPHID7_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID7_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID7_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID7_PERIPHID_BITS                 (32)

#define NVIC_PERIPHID0                                       *((volatile uint32_t *)0xE000EFE0u)
#define NVIC_PERIPHID0_REG                                   *((volatile uint32_t *)0xE000EFE0u)
#define NVIC_PERIPHID0_ADDR                                  (0xE000EFE0u)
#define NVIC_PERIPHID0_RESET                                 (0x00000000u)
        /* PERIPHID field */
        #define NVIC_PERIPHID0_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID0_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID0_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID0_PERIPHID_BITS                 (32)

#define NVIC_PERIPHID1                                       *((volatile uint32_t *)0xE000EFE4u)
#define NVIC_PERIPHID1_REG                                   *((volatile uint32_t *)0xE000EFE4u)
#define NVIC_PERIPHID1_ADDR                                  (0xE000EFE4u)
#define NVIC_PERIPHID1_RESET                                 (0x000000B0u)
        /* PERIPHID field */
        #define NVIC_PERIPHID1_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID1_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID1_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID1_PERIPHID_BITS                 (32)

#define NVIC_PERIPHID2                                       *((volatile uint32_t *)0xE000EFE8u)
#define NVIC_PERIPHID2_REG                                   *((volatile uint32_t *)0xE000EFE8u)
#define NVIC_PERIPHID2_ADDR                                  (0xE000EFE8u)
#define NVIC_PERIPHID2_RESET                                 (0x0000001Bu)
        /* PERIPHID field */
        #define NVIC_PERIPHID2_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID2_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID2_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID2_PERIPHID_BITS                 (32)

#define NVIC_PERIPHID3                                       *((volatile uint32_t *)0xE000EFECu)
#define NVIC_PERIPHID3_REG                                   *((volatile uint32_t *)0xE000EFECu)
#define NVIC_PERIPHID3_ADDR                                  (0xE000EFECu)
#define NVIC_PERIPHID3_RESET                                 (0x00000000u)
        /* PERIPHID field */
        #define NVIC_PERIPHID3_PERIPHID                      (0xFFFFFFFFu)
        #define NVIC_PERIPHID3_PERIPHID_MASK                 (0xFFFFFFFFu)
        #define NVIC_PERIPHID3_PERIPHID_BIT                  (0)
        #define NVIC_PERIPHID3_PERIPHID_BITS                 (32)

#define NVIC_PCELLID0                                        *((volatile uint32_t *)0xE000EFF0u)
#define NVIC_PCELLID0_REG                                    *((volatile uint32_t *)0xE000EFF0u)
#define NVIC_PCELLID0_ADDR                                   (0xE000EFF0u)
#define NVIC_PCELLID0_RESET                                  (0x0000000Du)
        /* PCELLID field */
        #define NVIC_PCELLID0_PCELLID                        (0xFFFFFFFFu)
        #define NVIC_PCELLID0_PCELLID_MASK                   (0xFFFFFFFFu)
        #define NVIC_PCELLID0_PCELLID_BIT                    (0)
        #define NVIC_PCELLID0_PCELLID_BITS                   (32)

#define NVIC_PCELLID1                                        *((volatile uint32_t *)0xE000EFF4u)
#define NVIC_PCELLID1_REG                                    *((volatile uint32_t *)0xE000EFF4u)
#define NVIC_PCELLID1_ADDR                                   (0xE000EFF4u)
#define NVIC_PCELLID1_RESET                                  (0x000000E0u)
        /* PCELLID field */
        #define NVIC_PCELLID1_PCELLID                        (0xFFFFFFFFu)
        #define NVIC_PCELLID1_PCELLID_MASK                   (0xFFFFFFFFu)
        #define NVIC_PCELLID1_PCELLID_BIT                    (0)
        #define NVIC_PCELLID1_PCELLID_BITS                   (32)

#define NVIC_PCELLID2                                        *((volatile uint32_t *)0xE000EFF8u)
#define NVIC_PCELLID2_REG                                    *((volatile uint32_t *)0xE000EFF8u)
#define NVIC_PCELLID2_ADDR                                   (0xE000EFF8u)
#define NVIC_PCELLID2_RESET                                  (0x00000005u)
        /* PCELLID field */
        #define NVIC_PCELLID2_PCELLID                        (0xFFFFFFFFu)
        #define NVIC_PCELLID2_PCELLID_MASK                   (0xFFFFFFFFu)
        #define NVIC_PCELLID2_PCELLID_BIT                    (0)
        #define NVIC_PCELLID2_PCELLID_BITS                   (32)

#define NVIC_PCELLID3                                        *((volatile uint32_t *)0xE000EFFCu)
#define NVIC_PCELLID3_REG                                    *((volatile uint32_t *)0xE000EFFCu)
#define NVIC_PCELLID3_ADDR                                   (0xE000EFFCu)
#define NVIC_PCELLID3_RESET                                  (0x000000B1u)
        /* PCELLID field */
        #define NVIC_PCELLID3_PCELLID                        (0xFFFFFFFFu)
        #define NVIC_PCELLID3_PCELLID_MASK                   (0xFFFFFFFFu)
        #define NVIC_PCELLID3_PCELLID_BIT                    (0)
        #define NVIC_PCELLID3_PCELLID_BITS                   (32)

/* TPIU block */
#define BLOCK_TPIU_BASE                                      (0xE0040000u)
#define BLOCK_TPIU_END                                       (0xE0040EF8u)
#define BLOCK_TPIU_SIZE                                      (BLOCK_TPIU_END - BLOCK_TPIU_BASE + 1)

#define TPIU_SPS                                             *((volatile uint32_t *)0xE0040000u)
#define TPIU_SPS_REG                                         *((volatile uint32_t *)0xE0040000u)
#define TPIU_SPS_ADDR                                        (0xE0040000u)
#define TPIU_SPS_RESET                                       (0x00000000u)
        /* SPS_04 field */
        #define TPIU_SPS_SPS_04                              (0x00000008u)
        #define TPIU_SPS_SPS_04_MASK                         (0x00000008u)
        #define TPIU_SPS_SPS_04_BIT                          (3)
        #define TPIU_SPS_SPS_04_BITS                         (1)
        /* SPS_03 field */
        #define TPIU_SPS_SPS_03                              (0x00000004u)
        #define TPIU_SPS_SPS_03_MASK                         (0x00000004u)
        #define TPIU_SPS_SPS_03_BIT                          (2)
        #define TPIU_SPS_SPS_03_BITS                         (1)
        /* SPS_02 field */
        #define TPIU_SPS_SPS_02                              (0x00000002u)
        #define TPIU_SPS_SPS_02_MASK                         (0x00000002u)
        #define TPIU_SPS_SPS_02_BIT                          (1)
        #define TPIU_SPS_SPS_02_BITS                         (1)
        /* SPS_01 field */
        #define TPIU_SPS_SPS_01                              (0x00000001u)
        #define TPIU_SPS_SPS_01_MASK                         (0x00000001u)
        #define TPIU_SPS_SPS_01_BIT                          (0)
        #define TPIU_SPS_SPS_01_BITS                         (1)

#define TPIU_CPS                                             *((volatile uint32_t *)0xE0040004u)
#define TPIU_CPS_REG                                         *((volatile uint32_t *)0xE0040004u)
#define TPIU_CPS_ADDR                                        (0xE0040004u)
#define TPIU_CPS_RESET                                       (0x00000001u)
        /* CPS_04 field */
        #define TPIU_CPS_CPS_04                              (0x00000008u)
        #define TPIU_CPS_CPS_04_MASK                         (0x00000008u)
        #define TPIU_CPS_CPS_04_BIT                          (3)
        #define TPIU_CPS_CPS_04_BITS                         (1)
        /* CPS_03 field */
        #define TPIU_CPS_CPS_03                              (0x00000004u)
        #define TPIU_CPS_CPS_03_MASK                         (0x00000004u)
        #define TPIU_CPS_CPS_03_BIT                          (2)
        #define TPIU_CPS_CPS_03_BITS                         (1)
        /* CPS_02 field */
        #define TPIU_CPS_CPS_02                              (0x00000002u)
        #define TPIU_CPS_CPS_02_MASK                         (0x00000002u)
        #define TPIU_CPS_CPS_02_BIT                          (1)
        #define TPIU_CPS_CPS_02_BITS                         (1)
        /* CPS_01 field */
        #define TPIU_CPS_CPS_01                              (0x00000001u)
        #define TPIU_CPS_CPS_01_MASK                         (0x00000001u)
        #define TPIU_CPS_CPS_01_BIT                          (0)
        #define TPIU_CPS_CPS_01_BITS                         (1)

#define TPIU_COSD                                            *((volatile uint32_t *)0xE0040010u)
#define TPIU_COSD_REG                                        *((volatile uint32_t *)0xE0040010u)
#define TPIU_COSD_ADDR                                       (0xE0040010u)
#define TPIU_COSD_RESET                                      (0x00000000u)
        /* PRESCALER field */
        #define TPIU_COSD_PRESCALER                          (0x00001FFFu)
        #define TPIU_COSD_PRESCALER_MASK                     (0x00001FFFu)
        #define TPIU_COSD_PRESCALER_BIT                      (0)
        #define TPIU_COSD_PRESCALER_BITS                     (13)

#define TPIU_SPP                                             *((volatile uint32_t *)0xE00400F0u)
#define TPIU_SPP_REG                                         *((volatile uint32_t *)0xE00400F0u)
#define TPIU_SPP_ADDR                                        (0xE00400F0u)
#define TPIU_SPP_RESET                                       (0x00000001u)
        /* PROTOCOL field */
        #define TPIU_SPP_PROTOCOL                            (0x00000003u)
        #define TPIU_SPP_PROTOCOL_MASK                       (0x00000003u)
        #define TPIU_SPP_PROTOCOL_BIT                        (0)
        #define TPIU_SPP_PROTOCOL_BITS                       (2)

#define TPIU_FFS                                             *((volatile uint32_t *)0xE0040300u)
#define TPIU_FFS_REG                                         *((volatile uint32_t *)0xE0040300u)
#define TPIU_FFS_ADDR                                        (0xE0040300u)
#define TPIU_FFS_RESET                                       (0x00000008u)
        /* FTNONSTOP field */
        #define TPIU_FFS_FTNONSTOP                           (0x00000008u)
        #define TPIU_FFS_FTNONSTOP_MASK                      (0x00000008u)
        #define TPIU_FFS_FTNONSTOP_BIT                       (3)
        #define TPIU_FFS_FTNONSTOP_BITS                      (1)
        /* TCPRESENT field */
        #define TPIU_FFS_TCPRESENT                           (0x00000004u)
        #define TPIU_FFS_TCPRESENT_MASK                      (0x00000004u)
        #define TPIU_FFS_TCPRESENT_BIT                       (2)
        #define TPIU_FFS_TCPRESENT_BITS                      (1)
        /* FTSTOPPED field */
        #define TPIU_FFS_FTSTOPPED                           (0x00000002u)
        #define TPIU_FFS_FTSTOPPED_MASK                      (0x00000002u)
        #define TPIU_FFS_FTSTOPPED_BIT                       (1)
        #define TPIU_FFS_FTSTOPPED_BITS                      (1)
        /* FLINPROG field */
        #define TPIU_FFS_FLINPROG                            (0x00000001u)
        #define TPIU_FFS_FLINPROG_MASK                       (0x00000001u)
        #define TPIU_FFS_FLINPROG_BIT                        (0)
        #define TPIU_FFS_FLINPROG_BITS                       (1)

#define TPIU_FFC                                             *((volatile uint32_t *)0xE0040304u)
#define TPIU_FFC_REG                                         *((volatile uint32_t *)0xE0040304u)
#define TPIU_FFC_ADDR                                        (0xE0040304u)
#define TPIU_FFC_RESET                                       (0x00000102u)
        /* TRIGIN field */
        #define TPIU_FFC_TRIGIN                              (0x00000100u)
        #define TPIU_FFC_TRIGIN_MASK                         (0x00000100u)
        #define TPIU_FFC_TRIGIN_BIT                          (8)
        #define TPIU_FFC_TRIGIN_BITS                         (1)
        /* ENFCONT field */
        #define TPIU_FFC_ENFCONT                             (0x00000002u)
        #define TPIU_FFC_ENFCONT_MASK                        (0x00000002u)
        #define TPIU_FFC_ENFCONT_BIT                         (1)
        #define TPIU_FFC_ENFCONT_BITS                        (1)

#define TPIU_FSC                                             *((volatile uint32_t *)0xE0040308u)
#define TPIU_FSC_REG                                         *((volatile uint32_t *)0xE0040308u)
#define TPIU_FSC_ADDR                                        (0xE0040308u)
#define TPIU_FSC_RESET                                       (0x00000000u)
        /* FSC field */
        #define TPIU_FSC_FSC                                 (0xFFFFFFFFu)
        #define TPIU_FSC_FSC_MASK                            (0xFFFFFFFFu)
        #define TPIU_FSC_FSC_BIT                             (0)
        #define TPIU_FSC_FSC_BITS                            (32)

#define TPIU_ITATBCTR2                                       *((volatile uint32_t *)0xE0040EF0u)
#define TPIU_ITATBCTR2_REG                                   *((volatile uint32_t *)0xE0040EF0u)
#define TPIU_ITATBCTR2_ADDR                                  (0xE0040EF0u)
#define TPIU_ITATBCTR2_RESET                                 (0x00000000u)
        /* ATREADY1 field */
        #define TPIU_ITATBCTR2_ATREADY1                      (0x00000001u)
        #define TPIU_ITATBCTR2_ATREADY1_MASK                 (0x00000001u)
        #define TPIU_ITATBCTR2_ATREADY1_BIT                  (0)
        #define TPIU_ITATBCTR2_ATREADY1_BITS                 (1)

#define TPIU_ITATBCTR0                                       *((volatile uint32_t *)0xE0040EF8u)
#define TPIU_ITATBCTR0_REG                                   *((volatile uint32_t *)0xE0040EF8u)
#define TPIU_ITATBCTR0_ADDR                                  (0xE0040EF8u)
#define TPIU_ITATBCTR0_RESET                                 (0x00000000u)
        /* ATREADY1 field */
        #define TPIU_ITATBCTR0_ATREADY1                      (0x00000001u)
        #define TPIU_ITATBCTR0_ATREADY1_MASK                 (0x00000001u)
        #define TPIU_ITATBCTR0_ATREADY1_BIT                  (0)
        #define TPIU_ITATBCTR0_ATREADY1_BITS                 (1)

/* ETM block */
#define BLOCK_ETM_BASE                                       (0xE0041000u)
#define BLOCK_ETM_END                                        (0xE0041FFFu)
#define BLOCK_ETM_SIZE                                       (BLOCK_ETM_END - BLOCK_ETM_BASE + 1)

#define ETM_CR                                               *((volatile uint32_t *)0xE0041000u)
#define ETM_CR_REG                                           *((volatile uint32_t *)0xE0041000u)
#define ETM_CR_ADDR                                          (0xE0041000u)
#define ETM_CR_RESET                                         (0x00000411u)
        /* PORTSIZE3 field */
        #define ETM_CR_PORTSIZE3                             (0x00200000u)
        #define ETM_CR_PORTSIZE3_MASK                        (0x00200000u)
        #define ETM_CR_PORTSIZE3_BIT                         (21)
        #define ETM_CR_PORTSIZE3_BITS                        (1)
        /* PORTMODE10 field */
        #define ETM_CR_PORTMODE10                            (0x00030000u)
        #define ETM_CR_PORTMODE10_MASK                       (0x00030000u)
        #define ETM_CR_PORTMODE10_BIT                        (16)
        #define ETM_CR_PORTMODE10_BITS                       (2)
        /* PORTMODE2 field */
        #define ETM_CR_PORTMODE2                             (0x00002000u)
        #define ETM_CR_PORTMODE2_MASK                        (0x00002000u)
        #define ETM_CR_PORTMODE2_BIT                         (13)
        #define ETM_CR_PORTMODE2_BITS                        (1)
        /* ETMEN field */
        #define ETM_CR_ETMEN                                 (0x00000800u)
        #define ETM_CR_ETMEN_MASK                            (0x00000800u)
        #define ETM_CR_ETMEN_BIT                             (11)
        #define ETM_CR_ETMEN_BITS                            (1)
        /* ETMPROG field */
        #define ETM_CR_ETMPROG                               (0x00000400u)
        #define ETM_CR_ETMPROG_MASK                          (0x00000400u)
        #define ETM_CR_ETMPROG_BIT                           (10)
        #define ETM_CR_ETMPROG_BITS                          (1)
        /* DBGRQ field */
        #define ETM_CR_DBGRQ                                 (0x00000200u)
        #define ETM_CR_DBGRQ_MASK                            (0x00000200u)
        #define ETM_CR_DBGRQ_BIT                             (9)
        #define ETM_CR_DBGRQ_BITS                            (1)
        /* BRCHO field */
        #define ETM_CR_BRCHO                                 (0x00000100u)
        #define ETM_CR_BRCHO_MASK                            (0x00000100u)
        #define ETM_CR_BRCHO_BIT                             (8)
        #define ETM_CR_BRCHO_BITS                            (1)
        /* STALLPROC field */
        #define ETM_CR_STALLPROC                             (0x00000080u)
        #define ETM_CR_STALLPROC_MASK                        (0x00000080u)
        #define ETM_CR_STALLPROC_BIT                         (7)
        #define ETM_CR_STALLPROC_BITS                        (1)
        /* PORTSIZE210 field */
        #define ETM_CR_PORTSIZE210                           (0x00000070u)
        #define ETM_CR_PORTSIZE210_MASK                      (0x00000070u)
        #define ETM_CR_PORTSIZE210_BIT                       (4)
        #define ETM_CR_PORTSIZE210_BITS                      (3)
        /* PWRDN field */
        #define ETM_CR_PWRDN                                 (0x00000001u)
        #define ETM_CR_PWRDN_MASK                            (0x00000001u)
        #define ETM_CR_PWRDN_BIT                             (0)
        #define ETM_CR_PWRDN_BITS                            (1)

#define ETM_CCR                                              *((volatile uint32_t *)0xE0041004u)
#define ETM_CCR_REG                                          *((volatile uint32_t *)0xE0041004u)
#define ETM_CCR_ADDR                                         (0xE0041004u)
#define ETM_CCR_RESET                                        (0x8C800000u)
        /* CCR field */
        #define ETM_CCR_CCR                                  (0xFFFFFFFFu)
        #define ETM_CCR_CCR_MASK                             (0xFFFFFFFFu)
        #define ETM_CCR_CCR_BIT                              (0)
        #define ETM_CCR_CCR_BITS                             (32)

#define ETM_TRIGGER                                          *((volatile uint32_t *)0xE0041008u)
#define ETM_TRIGGER_REG                                      *((volatile uint32_t *)0xE0041008u)
#define ETM_TRIGGER_ADDR                                     (0xE0041008u)
#define ETM_TRIGGER_RESET                                    (0x00000000u)
        /* TRIGEVENT field */
        #define ETM_TRIGGER_TRIGEVENT                        (0x0001FFFFu)
        #define ETM_TRIGGER_TRIGEVENT_MASK                   (0x0001FFFFu)
        #define ETM_TRIGGER_TRIGEVENT_BIT                    (0)
        #define ETM_TRIGGER_TRIGEVENT_BITS                   (17)

#define ETM_SR                                               *((volatile uint32_t *)0xE0041010u)
#define ETM_SR_REG                                           *((volatile uint32_t *)0xE0041010u)
#define ETM_SR_ADDR                                          (0xE0041010u)
#define ETM_SR_RESET                                         (0x00000002u)
        /* TRIG field */
        #define ETM_SR_TRIG                                  (0x00000008u)
        #define ETM_SR_TRIG_MASK                             (0x00000008u)
        #define ETM_SR_TRIG_BIT                              (3)
        #define ETM_SR_TRIG_BITS                             (1)
        /* STARTSTOP field */
        #define ETM_SR_STARTSTOP                             (0x00000004u)
        #define ETM_SR_STARTSTOP_MASK                        (0x00000004u)
        #define ETM_SR_STARTSTOP_BIT                         (2)
        #define ETM_SR_STARTSTOP_BITS                        (1)
        /* PROGSTAT field */
        #define ETM_SR_PROGSTAT                              (0x00000002u)
        #define ETM_SR_PROGSTAT_MASK                         (0x00000002u)
        #define ETM_SR_PROGSTAT_BIT                          (1)
        #define ETM_SR_PROGSTAT_BITS                         (1)
        /* OVRFLW field */
        #define ETM_SR_OVRFLW                                (0x00000001u)
        #define ETM_SR_OVRFLW_MASK                           (0x00000001u)
        #define ETM_SR_OVRFLW_BIT                            (0)
        #define ETM_SR_OVRFLW_BITS                           (1)

#define ETM_SCR                                              *((volatile uint32_t *)0xE0041014u)
#define ETM_SCR_REG                                          *((volatile uint32_t *)0xE0041014u)
#define ETM_SCR_ADDR                                         (0xE0041014u)
#define ETM_SCR_RESET                                        (0x00020D09u)
        /* NOFETCHCOMP field */
        #define ETM_SCR_NOFETCHCOMP                          (0x00020000u)
        #define ETM_SCR_NOFETCHCOMP_MASK                     (0x00020000u)
        #define ETM_SCR_NOFETCHCOMP_BIT                      (17)
        #define ETM_SCR_NOFETCHCOMP_BITS                     (1)
        /* NUMPROC field */
        #define ETM_SCR_NUMPROC                              (0x00007000u)
        #define ETM_SCR_NUMPROC_MASK                         (0x00007000u)
        #define ETM_SCR_NUMPROC_BIT                          (12)
        #define ETM_SCR_NUMPROC_BITS                         (3)
        /* PORTMODESUPPORTED field */
        #define ETM_SCR_PORTMODESUPPORTED                    (0x00000800u)
        #define ETM_SCR_PORTMODESUPPORTED_MASK               (0x00000800u)
        #define ETM_SCR_PORTMODESUPPORTED_BIT                (11)
        #define ETM_SCR_PORTMODESUPPORTED_BITS               (1)
        /* PORTSIZESUPPORTED field */
        #define ETM_SCR_PORTSIZESUPPORTED                    (0x00000400u)
        #define ETM_SCR_PORTSIZESUPPORTED_MASK               (0x00000400u)
        #define ETM_SCR_PORTSIZESUPPORTED_BIT                (10)
        #define ETM_SCR_PORTSIZESUPPORTED_BITS               (1)
        /* MAXPORTSIZE3 field */
        #define ETM_SCR_MAXPORTSIZE3                         (0x00000200u)
        #define ETM_SCR_MAXPORTSIZE3_MASK                    (0x00000200u)
        #define ETM_SCR_MAXPORTSIZE3_BIT                     (9)
        #define ETM_SCR_MAXPORTSIZE3_BITS                    (1)
        /* FIFOFULL field */
        #define ETM_SCR_FIFOFULL                             (0x00000100u)
        #define ETM_SCR_FIFOFULL_MASK                        (0x00000100u)
        #define ETM_SCR_FIFOFULL_BIT                         (8)
        #define ETM_SCR_FIFOFULL_BITS                        (1)
        /* RESERVED field */
        #define ETM_SCR_RESERVED                             (0x00000008u)
        #define ETM_SCR_RESERVED_MASK                        (0x00000008u)
        #define ETM_SCR_RESERVED_BIT                         (3)
        #define ETM_SCR_RESERVED_BITS                        (1)
        /* MAXPORTSIZE20 field */
        #define ETM_SCR_MAXPORTSIZE20                        (0x00000007u)
        #define ETM_SCR_MAXPORTSIZE20_MASK                   (0x00000007u)
        #define ETM_SCR_MAXPORTSIZE20_BIT                    (0)
        #define ETM_SCR_MAXPORTSIZE20_BITS                   (3)

#define ETM_TEEVR                                            *((volatile uint32_t *)0xE0041020u)
#define ETM_TEEVR_REG                                        *((volatile uint32_t *)0xE0041020u)
#define ETM_TEEVR_ADDR                                       (0xE0041020u)
#define ETM_TEEVR_RESET                                      (0x00000000u)
        /* TRACEEVENT field */
        #define ETM_TEEVR_TRACEEVENT                         (0x0001FFFFu)
        #define ETM_TEEVR_TRACEEVENT_MASK                    (0x0001FFFFu)
        #define ETM_TEEVR_TRACEEVENT_BIT                     (0)
        #define ETM_TEEVR_TRACEEVENT_BITS                    (17)

#define ETM_TECR1                                            *((volatile uint32_t *)0xE0041024u)
#define ETM_TECR1_REG                                        *((volatile uint32_t *)0xE0041024u)
#define ETM_TECR1_ADDR                                       (0xE0041024u)
#define ETM_TECR1_RESET                                      (0x00000000u)
        /* TRACEEN field */
        #define ETM_TECR1_TRACEEN                            (0x02000000u)
        #define ETM_TECR1_TRACEEN_MASK                       (0x02000000u)
        #define ETM_TECR1_TRACEEN_BIT                        (25)
        #define ETM_TECR1_TRACEEN_BITS                       (1)

#define ETM_FFLR                                             *((volatile uint32_t *)0xE004102Cu)
#define ETM_FFLR_REG                                         *((volatile uint32_t *)0xE004102Cu)
#define ETM_FFLR_ADDR                                        (0xE004102Cu)
#define ETM_FFLR_RESET                                       (0x00000000u)
        /* LEVEL field */
        #define ETM_FFLR_LEVEL                               (0x000000FFu)
        #define ETM_FFLR_LEVEL_MASK                          (0x000000FFu)
        #define ETM_FFLR_LEVEL_BIT                           (0)
        #define ETM_FFLR_LEVEL_BITS                          (8)

#define ETM_SYNCFR                                           *((volatile uint32_t *)0xE00411E0u)
#define ETM_SYNCFR_REG                                       *((volatile uint32_t *)0xE00411E0u)
#define ETM_SYNCFR_ADDR                                      (0xE00411E0u)
#define ETM_SYNCFR_RESET                                     (0x00000400u)
        /* SYNCFREQ field */
        #define ETM_SYNCFR_SYNCFREQ                          (0x00000FFFu)
        #define ETM_SYNCFR_SYNCFREQ_MASK                     (0x00000FFFu)
        #define ETM_SYNCFR_SYNCFREQ_BIT                      (0)
        #define ETM_SYNCFR_SYNCFREQ_BITS                     (12)

#define ETM_IDR                                              *((volatile uint32_t *)0xE00411E4u)
#define ETM_IDR_REG                                          *((volatile uint32_t *)0xE00411E4u)
#define ETM_IDR_ADDR                                         (0xE00411E4u)
#define ETM_IDR_RESET                                        (0x4114F241u)
        /* ID field */
        #define ETM_IDR_ID                                   (0xFFFFFFFFu)
        #define ETM_IDR_ID_MASK                              (0xFFFFFFFFu)
        #define ETM_IDR_ID_BIT                               (0)
        #define ETM_IDR_ID_BITS                              (32)

#define ETM_CCER                                             *((volatile uint32_t *)0xE00411E8u)
#define ETM_CCER_REG                                         *((volatile uint32_t *)0xE00411E8u)
#define ETM_CCER_ADDR                                        (0xE00411E8u)
#define ETM_CCER_RESET                                       (0x00018800u)
        /* CCER field */
        #define ETM_CCER_CCER                                (0xFFFFFFFFu)
        #define ETM_CCER_CCER_MASK                           (0xFFFFFFFFu)
        #define ETM_CCER_CCER_BIT                            (0)
        #define ETM_CCER_CCER_BITS                           (32)

#define ETM_TESSEICR                                         *((volatile uint32_t *)0xE00411F0u)
#define ETM_TESSEICR_REG                                     *((volatile uint32_t *)0xE00411F0u)
#define ETM_TESSEICR_ADDR                                    (0xE00411F0u)
#define ETM_TESSEICR_RESET                                   (0x00000000u)
        /* STOPSEL field */
        #define ETM_TESSEICR_STOPSEL                         (0x000F0000u)
        #define ETM_TESSEICR_STOPSEL_MASK                    (0x000F0000u)
        #define ETM_TESSEICR_STOPSEL_BIT                     (16)
        #define ETM_TESSEICR_STOPSEL_BITS                    (4)
        /* STARTSEL field */
        #define ETM_TESSEICR_STARTSEL                        (0x0000000Fu)
        #define ETM_TESSEICR_STARTSEL_MASK                   (0x0000000Fu)
        #define ETM_TESSEICR_STARTSEL_BIT                    (0)
        #define ETM_TESSEICR_STARTSEL_BITS                   (4)

#define ETM_TRACEIDR                                         *((volatile uint32_t *)0xE0041200u)
#define ETM_TRACEIDR_REG                                     *((volatile uint32_t *)0xE0041200u)
#define ETM_TRACEIDR_ADDR                                    (0xE0041200u)
#define ETM_TRACEIDR_RESET                                   (0x00000000u)
        /* TRACEID field */
        #define ETM_TRACEIDR_TRACEID                         (0x0000007Fu)
        #define ETM_TRACEIDR_TRACEID_MASK                    (0x0000007Fu)
        #define ETM_TRACEIDR_TRACEID_BIT                     (0)
        #define ETM_TRACEIDR_TRACEID_BITS                    (7)

#define ETM_PDSR                                             *((volatile uint32_t *)0xE0041314u)
#define ETM_PDSR_REG                                         *((volatile uint32_t *)0xE0041314u)
#define ETM_PDSR_ADDR                                        (0xE0041314u)
#define ETM_PDSR_RESET                                       (0x00000001u)
        /* PDSTICK field */
        #define ETM_PDSR_PDSTICK                             (0x00000002u)
        #define ETM_PDSR_PDSTICK_MASK                        (0x00000002u)
        #define ETM_PDSR_PDSTICK_BIT                         (1)
        #define ETM_PDSR_PDSTICK_BITS                        (1)
        /* PDSTATUS field */
        #define ETM_PDSR_PDSTATUS                            (0x00000001u)
        #define ETM_PDSR_PDSTATUS_MASK                       (0x00000001u)
        #define ETM_PDSR_PDSTATUS_BIT                        (0)
        #define ETM_PDSR_PDSTATUS_BITS                       (1)

#define ETM_ITMISCIN                                         *((volatile uint32_t *)0xE0041EE0u)
#define ETM_ITMISCIN_REG                                     *((volatile uint32_t *)0xE0041EE0u)
#define ETM_ITMISCIN_ADDR                                    (0xE0041EE0u)
#define ETM_ITMISCIN_RESET                                   (0x00000000u)
        /* COREHALT field */
        #define ETM_ITMISCIN_COREHALT                        (0x00000010u)
        #define ETM_ITMISCIN_COREHALT_MASK                   (0x00000010u)
        #define ETM_ITMISCIN_COREHALT_BIT                    (4)
        #define ETM_ITMISCIN_COREHALT_BITS                   (1)
        /* EXTIN field */
        #define ETM_ITMISCIN_EXTIN                           (0x00000003u)
        #define ETM_ITMISCIN_EXTIN_MASK                      (0x00000003u)
        #define ETM_ITMISCIN_EXTIN_BIT                       (0)
        #define ETM_ITMISCIN_EXTIN_BITS                      (2)

#define ETM_ITTRIGOUT                                        *((volatile uint32_t *)0xE0041EE8u)
#define ETM_ITTRIGOUT_REG                                    *((volatile uint32_t *)0xE0041EE8u)
#define ETM_ITTRIGOUT_ADDR                                   (0xE0041EE8u)
#define ETM_ITTRIGOUT_RESET                                  (0x00000000u)
        /* TRIGOUT field */
        #define ETM_ITTRIGOUT_TRIGOUT                        (0x00000001u)
        #define ETM_ITTRIGOUT_TRIGOUT_MASK                   (0x00000001u)
        #define ETM_ITTRIGOUT_TRIGOUT_BIT                    (0)
        #define ETM_ITTRIGOUT_TRIGOUT_BITS                   (1)

#define ETM_ITBCTR2                                          *((volatile uint32_t *)0xE0041EF0u)
#define ETM_ITBCTR2_REG                                      *((volatile uint32_t *)0xE0041EF0u)
#define ETM_ITBCTR2_ADDR                                     (0xE0041EF0u)
#define ETM_ITBCTR2_RESET                                    (0x00000001u)
        /* ATREADY field */
        #define ETM_ITBCTR2_ATREADY                          (0x00000001u)
        #define ETM_ITBCTR2_ATREADY_MASK                     (0x00000001u)
        #define ETM_ITBCTR2_ATREADY_BIT                      (0)
        #define ETM_ITBCTR2_ATREADY_BITS                     (1)

#define ETM_ITBCTR0                                          *((volatile uint32_t *)0xE0041EF8u)
#define ETM_ITBCTR0_REG                                      *((volatile uint32_t *)0xE0041EF8u)
#define ETM_ITBCTR0_ADDR                                     (0xE0041EF8u)
#define ETM_ITBCTR0_RESET                                    (0x00000000u)
        /* ATVALID field */
        #define ETM_ITBCTR0_ATVALID                          (0x00000001u)
        #define ETM_ITBCTR0_ATVALID_MASK                     (0x00000001u)
        #define ETM_ITBCTR0_ATVALID_BIT                      (0)
        #define ETM_ITBCTR0_ATVALID_BITS                     (1)

#define ETM_ITCTRL                                           *((volatile uint32_t *)0xE0041F00u)
#define ETM_ITCTRL_REG                                       *((volatile uint32_t *)0xE0041F00u)
#define ETM_ITCTRL_ADDR                                      (0xE0041F00u)
#define ETM_ITCTRL_RESET                                     (0x00000000u)
        /* ENABLEITMODE field */
        #define ETM_ITCTRL_ENABLEITMODE                      (0x00000001u)
        #define ETM_ITCTRL_ENABLEITMODE_MASK                 (0x00000001u)
        #define ETM_ITCTRL_ENABLEITMODE_BIT                  (0)
        #define ETM_ITCTRL_ENABLEITMODE_BITS                 (1)

#define ETM_CLAIMSET                                         *((volatile uint32_t *)0xE0041FA0u)
#define ETM_CLAIMSET_REG                                     *((volatile uint32_t *)0xE0041FA0u)
#define ETM_CLAIMSET_ADDR                                    (0xE0041FA0u)
#define ETM_CLAIMSET_RESET                                   (0x0000000Fu)
        /* CLAIMSET field */
        #define ETM_CLAIMSET_CLAIMSET                        (0x000000FFu)
        #define ETM_CLAIMSET_CLAIMSET_MASK                   (0x000000FFu)
        #define ETM_CLAIMSET_CLAIMSET_BIT                    (0)
        #define ETM_CLAIMSET_CLAIMSET_BITS                   (8)

#define ETM_CLAIMCLR                                         *((volatile uint32_t *)0xE0041FA4u)
#define ETM_CLAIMCLR_REG                                     *((volatile uint32_t *)0xE0041FA4u)
#define ETM_CLAIMCLR_ADDR                                    (0xE0041FA4u)
#define ETM_CLAIMCLR_RESET                                   (0x00000000u)
        /* CLAIMCLR field */
        #define ETM_CLAIMCLR_CLAIMCLR                        (0x000000FFu)
        #define ETM_CLAIMCLR_CLAIMCLR_MASK                   (0x000000FFu)
        #define ETM_CLAIMCLR_CLAIMCLR_BIT                    (0)
        #define ETM_CLAIMCLR_CLAIMCLR_BITS                   (8)

#define ETM_LAR                                              *((volatile uint32_t *)0xE0041FB0u)
#define ETM_LAR_REG                                          *((volatile uint32_t *)0xE0041FB0u)
#define ETM_LAR_ADDR                                         (0xE0041FB0u)
#define ETM_LAR_RESET                                        (0x00000000u)
        /* LOCK field */
        #define ETM_LAR_LOCK                                 (0xFFFFFFFFu)
        #define ETM_LAR_LOCK_MASK                            (0xFFFFFFFFu)
        #define ETM_LAR_LOCK_BIT                             (0)
        #define ETM_LAR_LOCK_BITS                            (32)

#define ETM_LSR                                              *((volatile uint32_t *)0xE0041FB4u)
#define ETM_LSR_REG                                          *((volatile uint32_t *)0xE0041FB4u)
#define ETM_LSR_ADDR                                         (0xE0041FB4u)
#define ETM_LSR_RESET                                        (0x00000003u)
        /* LOCK field */
        #define ETM_LSR_LOCK                                 (0x00000002u)
        #define ETM_LSR_LOCK_MASK                            (0x00000002u)
        #define ETM_LSR_LOCK_BIT                             (1)
        #define ETM_LSR_LOCK_BITS                            (1)
        /* LOCKIMPL field */
        #define ETM_LSR_LOCKIMPL                             (0x00000001u)
        #define ETM_LSR_LOCKIMPL_MASK                        (0x00000001u)
        #define ETM_LSR_LOCKIMPL_BIT                         (0)
        #define ETM_LSR_LOCKIMPL_BITS                        (1)

#define ETM_AUTHSTATUS                                       *((volatile uint32_t *)0xE0041FB8u)
#define ETM_AUTHSTATUS_REG                                   *((volatile uint32_t *)0xE0041FB8u)
#define ETM_AUTHSTATUS_ADDR                                  (0xE0041FB8u)
#define ETM_AUTHSTATUS_RESET                                 (0x000000C0u)
        /* SECPERM field */
        #define ETM_AUTHSTATUS_SECPERM                       (0x000000C0u)
        #define ETM_AUTHSTATUS_SECPERM_MASK                  (0x000000C0u)
        #define ETM_AUTHSTATUS_SECPERM_BIT                   (6)
        #define ETM_AUTHSTATUS_SECPERM_BITS                  (2)
        /* SEC field */
        #define ETM_AUTHSTATUS_SEC                           (0x00000030u)
        #define ETM_AUTHSTATUS_SEC_MASK                      (0x00000030u)
        #define ETM_AUTHSTATUS_SEC_BIT                       (4)
        #define ETM_AUTHSTATUS_SEC_BITS                      (2)
        /* NONSECPERM field */
        #define ETM_AUTHSTATUS_NONSECPERM                    (0x0000000Cu)
        #define ETM_AUTHSTATUS_NONSECPERM_MASK               (0x0000000Cu)
        #define ETM_AUTHSTATUS_NONSECPERM_BIT                (2)
        #define ETM_AUTHSTATUS_NONSECPERM_BITS               (2)
        /* NONSEC field */
        #define ETM_AUTHSTATUS_NONSEC                        (0x00000003u)
        #define ETM_AUTHSTATUS_NONSEC_MASK                   (0x00000003u)
        #define ETM_AUTHSTATUS_NONSEC_BIT                    (0)
        #define ETM_AUTHSTATUS_NONSEC_BITS                   (2)

#define ETM_DEVTYPE                                          *((volatile uint32_t *)0xE0041FCCu)
#define ETM_DEVTYPE_REG                                      *((volatile uint32_t *)0xE0041FCCu)
#define ETM_DEVTYPE_ADDR                                     (0xE0041FCCu)
#define ETM_DEVTYPE_RESET                                    (0x00000013u)
        /* SUBTYPE field */
        #define ETM_DEVTYPE_SUBTYPE                          (0x000000F0u)
        #define ETM_DEVTYPE_SUBTYPE_MASK                     (0x000000F0u)
        #define ETM_DEVTYPE_SUBTYPE_BIT                      (4)
        #define ETM_DEVTYPE_SUBTYPE_BITS                     (4)
        /* MAJORTYPE field */
        #define ETM_DEVTYPE_MAJORTYPE                        (0x0000000Fu)
        #define ETM_DEVTYPE_MAJORTYPE_MASK                   (0x0000000Fu)
        #define ETM_DEVTYPE_MAJORTYPE_BIT                    (0)
        #define ETM_DEVTYPE_MAJORTYPE_BITS                   (4)

#define ETM_PERIPHID4                                        *((volatile uint32_t *)0xE0041FD0u)
#define ETM_PERIPHID4_REG                                    *((volatile uint32_t *)0xE0041FD0u)
#define ETM_PERIPHID4_ADDR                                   (0xE0041FD0u)
#define ETM_PERIPHID4_RESET                                  (0x00000004u)
        /* PERIPHID field */
        #define ETM_PERIPHID4_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID4_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID4_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID4_PERIPHID_BITS                  (32)

#define ETM_PERIPHID5                                        *((volatile uint32_t *)0xE0041FD4u)
#define ETM_PERIPHID5_REG                                    *((volatile uint32_t *)0xE0041FD4u)
#define ETM_PERIPHID5_ADDR                                   (0xE0041FD4u)
#define ETM_PERIPHID5_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define ETM_PERIPHID5_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID5_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID5_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID5_PERIPHID_BITS                  (32)

#define ETM_PERIPHID6                                        *((volatile uint32_t *)0xE0041FD8u)
#define ETM_PERIPHID6_REG                                    *((volatile uint32_t *)0xE0041FD8u)
#define ETM_PERIPHID6_ADDR                                   (0xE0041FD8u)
#define ETM_PERIPHID6_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define ETM_PERIPHID6_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID6_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID6_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID6_PERIPHID_BITS                  (32)

#define ETM_PERIPHID7                                        *((volatile uint32_t *)0xE0041FDCu)
#define ETM_PERIPHID7_REG                                    *((volatile uint32_t *)0xE0041FDCu)
#define ETM_PERIPHID7_ADDR                                   (0xE0041FDCu)
#define ETM_PERIPHID7_RESET                                  (0x00000000u)
        /* PERIPHID field */
        #define ETM_PERIPHID7_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID7_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID7_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID7_PERIPHID_BITS                  (32)

#define ETM_PERIPHID0                                        *((volatile uint32_t *)0xE0041FE0u)
#define ETM_PERIPHID0_REG                                    *((volatile uint32_t *)0xE0041FE0u)
#define ETM_PERIPHID0_ADDR                                   (0xE0041FE0u)
#define ETM_PERIPHID0_RESET                                  (0x00000024u)
        /* PERIPHID field */
        #define ETM_PERIPHID0_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID0_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID0_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID0_PERIPHID_BITS                  (32)

#define ETM_PERIPHID1                                        *((volatile uint32_t *)0xE0041FE4u)
#define ETM_PERIPHID1_REG                                    *((volatile uint32_t *)0xE0041FE4u)
#define ETM_PERIPHID1_ADDR                                   (0xE0041FE4u)
#define ETM_PERIPHID1_RESET                                  (0x000000B9u)
        /* PERIPHID field */
        #define ETM_PERIPHID1_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID1_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID1_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID1_PERIPHID_BITS                  (32)

#define ETM_PERIPHID2                                        *((volatile uint32_t *)0xE0041FE8u)
#define ETM_PERIPHID2_REG                                    *((volatile uint32_t *)0xE0041FE8u)
#define ETM_PERIPHID2_ADDR                                   (0xE0041FE8u)
#define ETM_PERIPHID2_RESET                                  (0x0000001Bu)
        /* PERIPHID field */
        #define ETM_PERIPHID2_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID2_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID2_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID2_PERIPHID_BITS                  (32)

#define ETM_PERIPHID3                                        *((volatile uint32_t *)0xE0041FECu)
#define ETM_PERIPHID3_REG                                    *((volatile uint32_t *)0xE0041FECu)
#define ETM_PERIPHID3_ADDR                                   (0xE0041FECu)
#define ETM_PERIPHID3_RESET                                  (0x00000010u)
        /* PERIPHID field */
        #define ETM_PERIPHID3_PERIPHID                       (0xFFFFFFFFu)
        #define ETM_PERIPHID3_PERIPHID_MASK                  (0xFFFFFFFFu)
        #define ETM_PERIPHID3_PERIPHID_BIT                   (0)
        #define ETM_PERIPHID3_PERIPHID_BITS                  (32)

#define ETM_PCELLID0                                         *((volatile uint32_t *)0xE0041FF0u)
#define ETM_PCELLID0_REG                                     *((volatile uint32_t *)0xE0041FF0u)
#define ETM_PCELLID0_ADDR                                    (0xE0041FF0u)
#define ETM_PCELLID0_RESET                                   (0x0000000Du)
        /* PCELLID field */
        #define ETM_PCELLID0_PCELLID                         (0xFFFFFFFFu)
        #define ETM_PCELLID0_PCELLID_MASK                    (0xFFFFFFFFu)
        #define ETM_PCELLID0_PCELLID_BIT                     (0)
        #define ETM_PCELLID0_PCELLID_BITS                    (32)

#define ETM_PCELLID1                                         *((volatile uint32_t *)0xE0041FF4u)
#define ETM_PCELLID1_REG                                     *((volatile uint32_t *)0xE0041FF4u)
#define ETM_PCELLID1_ADDR                                    (0xE0041FF4u)
#define ETM_PCELLID1_RESET                                   (0x00000090u)
        /* PCELLID field */
        #define ETM_PCELLID1_PCELLID                         (0xFFFFFFFFu)
        #define ETM_PCELLID1_PCELLID_MASK                    (0xFFFFFFFFu)
        #define ETM_PCELLID1_PCELLID_BIT                     (0)
        #define ETM_PCELLID1_PCELLID_BITS                    (32)

#define ETM_PCELLID2                                         *((volatile uint32_t *)0xE0041FF8u)
#define ETM_PCELLID2_REG                                     *((volatile uint32_t *)0xE0041FF8u)
#define ETM_PCELLID2_ADDR                                    (0xE0041FF8u)
#define ETM_PCELLID2_RESET                                   (0x00000005u)
        /* PCELLID field */
        #define ETM_PCELLID2_PCELLID                         (0xFFFFFFFFu)
        #define ETM_PCELLID2_PCELLID_MASK                    (0xFFFFFFFFu)
        #define ETM_PCELLID2_PCELLID_BIT                     (0)
        #define ETM_PCELLID2_PCELLID_BITS                    (32)

#define ETM_PCELLID3                                         *((volatile uint32_t *)0xE0041FFCu)
#define ETM_PCELLID3_REG                                     *((volatile uint32_t *)0xE0041FFCu)
#define ETM_PCELLID3_ADDR                                    (0xE0041FFCu)
#define ETM_PCELLID3_RESET                                   (0x000000B1u)
        /* PCELLID field */
        #define ETM_PCELLID3_PCELLID                         (0xFFFFFFFFu)
        #define ETM_PCELLID3_PCELLID_MASK                    (0xFFFFFFFFu)
        #define ETM_PCELLID3_PCELLID_BIT                     (0)
        #define ETM_PCELLID3_PCELLID_BITS                    (32)

/* ROM_TAB block */
#define BLOCK_ROM_TAB_BASE                                   (0xE00FF000u)
#define BLOCK_ROM_TAB_END                                    (0xE00FFFFFu)
#define BLOCK_ROM_TAB_SIZE                                   (BLOCK_ROM_TAB_END - BLOCK_ROM_TAB_BASE + 1)

#define ROM_SCS                                              *((volatile uint32_t *)0xE00FF000u)
#define ROM_SCS_REG                                          *((volatile uint32_t *)0xE00FF000u)
#define ROM_SCS_ADDR                                         (0xE00FF000u)
#define ROM_SCS_RESET                                        (0xFFF0F003u)
        /* ADDR_OFF field */
        #define ROM_SCS_ADDR_OFF                             (0xFFFFF000u)
        #define ROM_SCS_ADDR_OFF_MASK                        (0xFFFFF000u)
        #define ROM_SCS_ADDR_OFF_BIT                         (12)
        #define ROM_SCS_ADDR_OFF_BITS                        (20)
        /* FORMAT field */
        #define ROM_SCS_FORMAT                               (0x00000002u)
        #define ROM_SCS_FORMAT_MASK                          (0x00000002u)
        #define ROM_SCS_FORMAT_BIT                           (1)
        #define ROM_SCS_FORMAT_BITS                          (1)
        /* ENTRY_PRES field */
        #define ROM_SCS_ENTRY_PRES                           (0x00000001u)
        #define ROM_SCS_ENTRY_PRES_MASK                      (0x00000001u)
        #define ROM_SCS_ENTRY_PRES_BIT                       (0)
        #define ROM_SCS_ENTRY_PRES_BITS                      (1)

#define ROM_DWT                                              *((volatile uint32_t *)0xE00FF004u)
#define ROM_DWT_REG                                          *((volatile uint32_t *)0xE00FF004u)
#define ROM_DWT_ADDR                                         (0xE00FF004u)
#define ROM_DWT_RESET                                        (0xFFF02003u)
        /* ADDR_OFF field */
        #define ROM_DWT_ADDR_OFF                             (0xFFFFF000u)
        #define ROM_DWT_ADDR_OFF_MASK                        (0xFFFFF000u)
        #define ROM_DWT_ADDR_OFF_BIT                         (12)
        #define ROM_DWT_ADDR_OFF_BITS                        (20)
        /* FORMAT field */
        #define ROM_DWT_FORMAT                               (0x00000002u)
        #define ROM_DWT_FORMAT_MASK                          (0x00000002u)
        #define ROM_DWT_FORMAT_BIT                           (1)
        #define ROM_DWT_FORMAT_BITS                          (1)
        /* ENTRY_PRES field */
        #define ROM_DWT_ENTRY_PRES                           (0x00000001u)
        #define ROM_DWT_ENTRY_PRES_MASK                      (0x00000001u)
        #define ROM_DWT_ENTRY_PRES_BIT                       (0)
        #define ROM_DWT_ENTRY_PRES_BITS                      (1)

#define ROM_FPB                                              *((volatile uint32_t *)0xE00FF008u)
#define ROM_FPB_REG                                          *((volatile uint32_t *)0xE00FF008u)
#define ROM_FPB_ADDR                                         (0xE00FF008u)
#define ROM_FPB_RESET                                        (0xFFF03003u)
        /* ADDR_OFF field */
        #define ROM_FPB_ADDR_OFF                             (0xFFFFF000u)
        #define ROM_FPB_ADDR_OFF_MASK                        (0xFFFFF000u)
        #define ROM_FPB_ADDR_OFF_BIT                         (12)
        #define ROM_FPB_ADDR_OFF_BITS                        (20)
        /* FORMAT field */
        #define ROM_FPB_FORMAT                               (0x00000002u)
        #define ROM_FPB_FORMAT_MASK                          (0x00000002u)
        #define ROM_FPB_FORMAT_BIT                           (1)
        #define ROM_FPB_FORMAT_BITS                          (1)
        /* ENTRY_PRES field */
        #define ROM_FPB_ENTRY_PRES                           (0x00000001u)
        #define ROM_FPB_ENTRY_PRES_MASK                      (0x00000001u)
        #define ROM_FPB_ENTRY_PRES_BIT                       (0)
        #define ROM_FPB_ENTRY_PRES_BITS                      (1)

#define ROM_ITM                                              *((volatile uint32_t *)0xE00FF00Cu)
#define ROM_ITM_REG                                          *((volatile uint32_t *)0xE00FF00Cu)
#define ROM_ITM_ADDR                                         (0xE00FF00Cu)
#define ROM_ITM_RESET                                        (0xFFF01003u)
        /* ADDR_OFF field */
        #define ROM_ITM_ADDR_OFF                             (0xFFFFF000u)
        #define ROM_ITM_ADDR_OFF_MASK                        (0xFFFFF000u)
        #define ROM_ITM_ADDR_OFF_BIT                         (12)
        #define ROM_ITM_ADDR_OFF_BITS                        (20)
        /* FORMAT field */
        #define ROM_ITM_FORMAT                               (0x00000002u)
        #define ROM_ITM_FORMAT_MASK                          (0x00000002u)
        #define ROM_ITM_FORMAT_BIT                           (1)
        #define ROM_ITM_FORMAT_BITS                          (1)
        /* ENTRY_PRES field */
        #define ROM_ITM_ENTRY_PRES                           (0x00000001u)
        #define ROM_ITM_ENTRY_PRES_MASK                      (0x00000001u)
        #define ROM_ITM_ENTRY_PRES_BIT                       (0)
        #define ROM_ITM_ENTRY_PRES_BITS                      (1)

#define ROM_TPIU                                             *((volatile uint32_t *)0xE00FF010u)
#define ROM_TPIU_REG                                         *((volatile uint32_t *)0xE00FF010u)
#define ROM_TPIU_ADDR                                        (0xE00FF010u)
#define ROM_TPIU_RESET                                       (0xFFF0F003u)
        /* ADDR_OFF field */
        #define ROM_TPIU_ADDR_OFF                            (0xFFFFF000u)
        #define ROM_TPIU_ADDR_OFF_MASK                       (0xFFFFF000u)
        #define ROM_TPIU_ADDR_OFF_BIT                        (12)
        #define ROM_TPIU_ADDR_OFF_BITS                       (20)
        /* FORMAT field */
        #define ROM_TPIU_FORMAT                              (0x00000002u)
        #define ROM_TPIU_FORMAT_MASK                         (0x00000002u)
        #define ROM_TPIU_FORMAT_BIT                          (1)
        #define ROM_TPIU_FORMAT_BITS                         (1)
        /* ENTRY_PRES field */
        #define ROM_TPIU_ENTRY_PRES                          (0x00000001u)
        #define ROM_TPIU_ENTRY_PRES_MASK                     (0x00000001u)
        #define ROM_TPIU_ENTRY_PRES_BIT                      (0)
        #define ROM_TPIU_ENTRY_PRES_BITS                     (1)

#define ROM_ETM                                              *((volatile uint32_t *)0xE00FF014u)
#define ROM_ETM_REG                                          *((volatile uint32_t *)0xE00FF014u)
#define ROM_ETM_ADDR                                         (0xE00FF014u)
#define ROM_ETM_RESET                                        (0xFFF0F002u)
        /* ADDR_OFF field */
        #define ROM_ETM_ADDR_OFF                             (0xFFFFF000u)
        #define ROM_ETM_ADDR_OFF_MASK                        (0xFFFFF000u)
        #define ROM_ETM_ADDR_OFF_BIT                         (12)
        #define ROM_ETM_ADDR_OFF_BITS                        (20)
        /* FORMAT field */
        #define ROM_ETM_FORMAT                               (0x00000002u)
        #define ROM_ETM_FORMAT_MASK                          (0x00000002u)
        #define ROM_ETM_FORMAT_BIT                           (1)
        #define ROM_ETM_FORMAT_BITS                          (1)
        /* ENTRY_PRES field */
        #define ROM_ETM_ENTRY_PRES                           (0x00000001u)
        #define ROM_ETM_ENTRY_PRES_MASK                      (0x00000001u)
        #define ROM_ETM_ENTRY_PRES_BIT                       (0)
        #define ROM_ETM_ENTRY_PRES_BITS                      (1)

#define ROM_END                                              *((volatile uint32_t *)0xE00FF018u)
#define ROM_END_REG                                          *((volatile uint32_t *)0xE00FF018u)
#define ROM_END_ADDR                                         (0xE00FF018u)
#define ROM_END_RESET                                        (0x00000000u)
        /* END field */
        #define ROM_END_END                                  (0xFFFFFFFFu)
        #define ROM_END_END_MASK                             (0xFFFFFFFFu)
        #define ROM_END_END_BIT                              (0)
        #define ROM_END_END_BITS                             (32)

#define ROM_MEMTYPE                                          *((volatile uint32_t *)0xE00FFFCCu)
#define ROM_MEMTYPE_REG                                      *((volatile uint32_t *)0xE00FFFCCu)
#define ROM_MEMTYPE_ADDR                                     (0xE00FFFCCu)
#define ROM_MEMTYPE_RESET                                    (0x00000001u)
        /* MEMTYPE field */
        #define ROM_MEMTYPE_MEMTYPE                          (0x00000001u)
        #define ROM_MEMTYPE_MEMTYPE_MASK                     (0x00000001u)
        #define ROM_MEMTYPE_MEMTYPE_BIT                      (0)
        #define ROM_MEMTYPE_MEMTYPE_BITS                     (1)

#define ROM_PID4                                             *((volatile uint32_t *)0xE00FFFD0u)
#define ROM_PID4_REG                                         *((volatile uint32_t *)0xE00FFFD0u)
#define ROM_PID4_ADDR                                        (0xE00FFFD0u)
#define ROM_PID4_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID4_PID                                 (0x0000000Fu)
        #define ROM_PID4_PID_MASK                            (0x0000000Fu)
        #define ROM_PID4_PID_BIT                             (0)
        #define ROM_PID4_PID_BITS                            (4)

#define ROM_PID5                                             *((volatile uint32_t *)0xE00FFFD4u)
#define ROM_PID5_REG                                         *((volatile uint32_t *)0xE00FFFD4u)
#define ROM_PID5_ADDR                                        (0xE00FFFD4u)
#define ROM_PID5_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID5_PID                                 (0x0000000Fu)
        #define ROM_PID5_PID_MASK                            (0x0000000Fu)
        #define ROM_PID5_PID_BIT                             (0)
        #define ROM_PID5_PID_BITS                            (4)

#define ROM_PID6                                             *((volatile uint32_t *)0xE00FFFD8u)
#define ROM_PID6_REG                                         *((volatile uint32_t *)0xE00FFFD8u)
#define ROM_PID6_ADDR                                        (0xE00FFFD8u)
#define ROM_PID6_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID6_PID                                 (0x0000000Fu)
        #define ROM_PID6_PID_MASK                            (0x0000000Fu)
        #define ROM_PID6_PID_BIT                             (0)
        #define ROM_PID6_PID_BITS                            (4)

#define ROM_PID7                                             *((volatile uint32_t *)0xE00FFFDCu)
#define ROM_PID7_REG                                         *((volatile uint32_t *)0xE00FFFDCu)
#define ROM_PID7_ADDR                                        (0xE00FFFDCu)
#define ROM_PID7_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID7_PID                                 (0x0000000Fu)
        #define ROM_PID7_PID_MASK                            (0x0000000Fu)
        #define ROM_PID7_PID_BIT                             (0)
        #define ROM_PID7_PID_BITS                            (4)

#define ROM_PID0                                             *((volatile uint32_t *)0xE00FFFE0u)
#define ROM_PID0_REG                                         *((volatile uint32_t *)0xE00FFFE0u)
#define ROM_PID0_ADDR                                        (0xE00FFFE0u)
#define ROM_PID0_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID0_PID                                 (0x0000000Fu)
        #define ROM_PID0_PID_MASK                            (0x0000000Fu)
        #define ROM_PID0_PID_BIT                             (0)
        #define ROM_PID0_PID_BITS                            (4)

#define ROM_PID1                                             *((volatile uint32_t *)0xE00FFFE4u)
#define ROM_PID1_REG                                         *((volatile uint32_t *)0xE00FFFE4u)
#define ROM_PID1_ADDR                                        (0xE00FFFE4u)
#define ROM_PID1_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID1_PID                                 (0x0000000Fu)
        #define ROM_PID1_PID_MASK                            (0x0000000Fu)
        #define ROM_PID1_PID_BIT                             (0)
        #define ROM_PID1_PID_BITS                            (4)

#define ROM_PID2                                             *((volatile uint32_t *)0xE00FFFE8u)
#define ROM_PID2_REG                                         *((volatile uint32_t *)0xE00FFFE8u)
#define ROM_PID2_ADDR                                        (0xE00FFFE8u)
#define ROM_PID2_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID2_PID                                 (0x0000000Fu)
        #define ROM_PID2_PID_MASK                            (0x0000000Fu)
        #define ROM_PID2_PID_BIT                             (0)
        #define ROM_PID2_PID_BITS                            (4)

#define ROM_PID3                                             *((volatile uint32_t *)0xE00FFFECu)
#define ROM_PID3_REG                                         *((volatile uint32_t *)0xE00FFFECu)
#define ROM_PID3_ADDR                                        (0xE00FFFECu)
#define ROM_PID3_RESET                                       (0x00000000u)
        /* PID field */
        #define ROM_PID3_PID                                 (0x0000000Fu)
        #define ROM_PID3_PID_MASK                            (0x0000000Fu)
        #define ROM_PID3_PID_BIT                             (0)
        #define ROM_PID3_PID_BITS                            (4)

#define ROM_CID0                                             *((volatile uint32_t *)0xE00FFFF0u)
#define ROM_CID0_REG                                         *((volatile uint32_t *)0xE00FFFF0u)
#define ROM_CID0_ADDR                                        (0xE00FFFF0u)
#define ROM_CID0_RESET                                       (0x0000000Du)
        /* CID field */
        #define ROM_CID0_CID                                 (0x000000FFu)
        #define ROM_CID0_CID_MASK                            (0x000000FFu)
        #define ROM_CID0_CID_BIT                             (0)
        #define ROM_CID0_CID_BITS                            (8)

#define ROM_CID1                                             *((volatile uint32_t *)0xE00FFFF4u)
#define ROM_CID1_REG                                         *((volatile uint32_t *)0xE00FFFF4u)
#define ROM_CID1_ADDR                                        (0xE00FFFF4u)
#define ROM_CID1_RESET                                       (0x00000010u)
        /* CID field */
        #define ROM_CID1_CID                                 (0x000000FFu)
        #define ROM_CID1_CID_MASK                            (0x000000FFu)
        #define ROM_CID1_CID_BIT                             (0)
        #define ROM_CID1_CID_BITS                            (8)

#define ROM_CID2                                             *((volatile uint32_t *)0xE00FFFF8u)
#define ROM_CID2_REG                                         *((volatile uint32_t *)0xE00FFFF8u)
#define ROM_CID2_ADDR                                        (0xE00FFFF8u)
#define ROM_CID2_RESET                                       (0x00000005u)
        /* CID field */
        #define ROM_CID2_CID                                 (0x000000FFu)
        #define ROM_CID2_CID_MASK                            (0x000000FFu)
        #define ROM_CID2_CID_BIT                             (0)
        #define ROM_CID2_CID_BITS                            (8)

#define ROM_CID3                                             *((volatile uint32_t *)0xE00FFFFCu)
#define ROM_CID3_REG                                         *((volatile uint32_t *)0xE00FFFFCu)
#define ROM_CID3_ADDR                                        (0xE00FFFFCu)
#define ROM_CID3_RESET                                       (0x000000B1u)
        /* CID field */
        #define ROM_CID3_CID                                 (0x000000FFu)
        #define ROM_CID3_CID_MASK                            (0x000000FFu)
        #define ROM_CID3_CID_BIT                             (0)
        #define ROM_CID3_CID_BITS                            (8)

/* VENDOR block */
#define BLOCK_VENDOR_BASE                                    (0xE0100000u)
#define BLOCK_VENDOR_END                                     (0xFFFFFFFFu)
#define BLOCK_VENDOR_SIZE                                    (BLOCK_VENDOR_END - BLOCK_VENDOR_BASE + 1)


#endif /* __REGS_INTERNAL_H__ */

