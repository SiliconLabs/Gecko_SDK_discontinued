#ifndef __REGS_INDIRECT_ANALOGUE_H__
#define __REGS_INDIRECT_ANALOGUE_H__                         1


/* ANALOGUE_CONTROL block */
#define BLOCK_ANALOGUE_CONTROL_BASE                          (0x00000000u)
#define BLOCK_ANALOGUE_CONTROL_END                           (0x000000FFu)
#define BLOCK_ANALOGUE_CONTROL_SIZE                          (BLOCK_ANALOGUE_CONTROL_END - BLOCK_ANALOGUE_CONTROL_BASE + 1)

#define IFFILTER_H                                           (0x00000010u)
#define IFFILTER_H_REG                                       (0x00000010u)
#define IFFILTER_H_ADDR                                      (0x00000010u)
#define IFFILTER_H_RESET                                     (0x00000000u)
        /* IFFILTER_IQ_TRIM field */
        #define IFFILTER_H_IFFILTER_IQ_TRIM                  (0x0000FC00u)
        #define IFFILTER_H_IFFILTER_IQ_TRIM_MASK             (0x0000FC00u)
        #define IFFILTER_H_IFFILTER_IQ_TRIM_BIT              (10)
        #define IFFILTER_H_IFFILTER_IQ_TRIM_BITS             (6)
        /* IFFILTER_CH_TRIM field */
        #define IFFILTER_H_IFFILTER_CH_TRIM                  (0x000003E0u)
        #define IFFILTER_H_IFFILTER_CH_TRIM_MASK             (0x000003E0u)
        #define IFFILTER_H_IFFILTER_CH_TRIM_BIT              (5)
        #define IFFILTER_H_IFFILTER_CH_TRIM_BITS             (5)
        /* IFFILTER_PRE_TRIM field */
        #define IFFILTER_H_IFFILTER_PRE_TRIM                 (0x0000001Fu)
        #define IFFILTER_H_IFFILTER_PRE_TRIM_MASK            (0x0000001Fu)
        #define IFFILTER_H_IFFILTER_PRE_TRIM_BIT             (0)
        #define IFFILTER_H_IFFILTER_PRE_TRIM_BITS            (5)

#define IFFILTER_L                                           (0x00000011u)
#define IFFILTER_L_REG                                       (0x00000011u)
#define IFFILTER_L_ADDR                                      (0x00000011u)
#define IFFILTER_L_RESET                                     (0x00000000u)
        /* IFFILTER_TM field */
        #define IFFILTER_L_IFFILTER_TM                       (0x0000C000u)
        #define IFFILTER_L_IFFILTER_TM_MASK                  (0x0000C000u)
        #define IFFILTER_L_IFFILTER_TM_BIT                   (14)
        #define IFFILTER_L_IFFILTER_TM_BITS                  (2)
        /* IQMIXER_PHASE_SW field */
        #define IFFILTER_L_IQMIXER_PHASE_SW                  (0x00002000u)
        #define IFFILTER_L_IQMIXER_PHASE_SW_MASK             (0x00002000u)
        #define IFFILTER_L_IQMIXER_PHASE_SW_BIT              (13)
        #define IFFILTER_L_IQMIXER_PHASE_SW_BITS             (1)
        /* IFFILTER_COMPLX_ENB field */
        #define IFFILTER_L_IFFILTER_COMPLX_ENB               (0x00001000u)
        #define IFFILTER_L_IFFILTER_COMPLX_ENB_MASK          (0x00001000u)
        #define IFFILTER_L_IFFILTER_COMPLX_ENB_BIT           (12)
        #define IFFILTER_L_IFFILTER_COMPLX_ENB_BITS          (1)
        /* IFFILTER_BIASV field */
        #define IFFILTER_L_IFFILTER_BIASV                    (0x00000FC0u)
        #define IFFILTER_L_IFFILTER_BIASV_MASK               (0x00000FC0u)
        #define IFFILTER_L_IFFILTER_BIASV_BIT                (6)
        #define IFFILTER_L_IFFILTER_BIASV_BITS               (6)
        /* IFFILTER_BIASI field */
        #define IFFILTER_L_IFFILTER_BIASI                    (0x0000003Fu)
        #define IFFILTER_L_IFFILTER_BIASI_MASK               (0x0000003Fu)
        #define IFFILTER_L_IFFILTER_BIASI_BIT                (0)
        #define IFFILTER_L_IFFILTER_BIASI_BITS               (6)

#define LNA                                                  (0x00000020u)
#define LNA_REG                                              (0x00000020u)
#define LNA_ADDR                                             (0x00000020u)
#define LNA_RESET                                            (0x00000000u)
        /* UNUSED field */
        #define LNA_UNUSED                                   (0x0000C000u)
        #define LNA_UNUSED_MASK                              (0x0000C000u)
        #define LNA_UNUSED_BIT                               (14)
        #define LNA_UNUSED_BITS                              (2)
        /* LNA_GAIN_TRIM field */
        #define LNA_LNA_GAIN_TRIM                            (0x00003000u)
        #define LNA_LNA_GAIN_TRIM_MASK                       (0x00003000u)
        #define LNA_LNA_GAIN_TRIM_BIT                        (12)
        #define LNA_LNA_GAIN_TRIM_BITS                       (2)
        /* LNA_BIASV field */
        #define LNA_LNA_BIASV                                (0x00000FC0u)
        #define LNA_LNA_BIASV_MASK                           (0x00000FC0u)
        #define LNA_LNA_BIASV_BIT                            (6)
        #define LNA_LNA_BIASV_BITS                           (6)
        /* LNA_TUNE field */
        #define LNA_LNA_TUNE                                 (0x0000003Fu)
        #define LNA_LNA_TUNE_MASK                            (0x0000003Fu)
        #define LNA_LNA_TUNE_BIT                             (0)
        #define LNA_LNA_TUNE_BITS                            (6)

#define IFAMP                                                (0x00000030u)
#define IFAMP_REG                                            (0x00000030u)
#define IFAMP_ADDR                                           (0x00000030u)
#define IFAMP_RESET                                          (0x00000000u)
        /* IFAMP_TM field */
        #define IFAMP_IFAMP_TM                               (0x0000C000u)
        #define IFAMP_IFAMP_TM_MASK                          (0x0000C000u)
        #define IFAMP_IFAMP_TM_BIT                           (14)
        #define IFAMP_IFAMP_TM_BITS                          (2)
        /* UNUSED field */
        #define IFAMP_UNUSED                                 (0x00003000u)
        #define IFAMP_UNUSED_MASK                            (0x00003000u)
        #define IFAMP_UNUSED_BIT                             (12)
        #define IFAMP_UNUSED_BITS                            (2)
        /* IFAMP_BIASV field */
        #define IFAMP_IFAMP_BIASV                            (0x00000FC0u)
        #define IFAMP_IFAMP_BIASV_MASK                       (0x00000FC0u)
        #define IFAMP_IFAMP_BIASV_BIT                        (6)
        #define IFAMP_IFAMP_BIASV_BITS                       (6)
        /* IFAMP_BIASI field */
        #define IFAMP_IFAMP_BIASI                            (0x0000003Fu)
        #define IFAMP_IFAMP_BIASI_MASK                       (0x0000003Fu)
        #define IFAMP_IFAMP_BIASI_BIT                        (0)
        #define IFAMP_IFAMP_BIASI_BITS                       (6)

#define RXADC_H                                              (0x00000040u)
#define RXADC_H_REG                                          (0x00000040u)
#define RXADC_H_ADDR                                         (0x00000040u)
#define RXADC_H_RESET                                        (0x00000000u)
        /* RXADC_TM field */
        #define RXADC_H_RXADC_TM                             (0x0000C000u)
        #define RXADC_H_RXADC_TM_MASK                        (0x0000C000u)
        #define RXADC_H_RXADC_TM_BIT                         (14)
        #define RXADC_H_RXADC_TM_BITS                        (2)
        /* RXADC_BIAS_TRIM field */
        #define RXADC_H_RXADC_BIAS_TRIM                      (0x00003000u)
        #define RXADC_H_RXADC_BIAS_TRIM_MASK                 (0x00003000u)
        #define RXADC_H_RXADC_BIAS_TRIM_BIT                  (12)
        #define RXADC_H_RXADC_BIAS_TRIM_BITS                 (2)
        /* RXADC_BIASV field */
        #define RXADC_H_RXADC_BIASV                          (0x00000FC0u)
        #define RXADC_H_RXADC_BIASV_MASK                     (0x00000FC0u)
        #define RXADC_H_RXADC_BIASV_BIT                      (6)
        #define RXADC_H_RXADC_BIASV_BITS                     (6)
        /* RXADC_BIASI field */
        #define RXADC_H_RXADC_BIASI                          (0x0000003Fu)
        #define RXADC_H_RXADC_BIASI_MASK                     (0x0000003Fu)
        #define RXADC_H_RXADC_BIASI_BIT                      (0)
        #define RXADC_H_RXADC_BIASI_BITS                     (6)

#define RXADC_L                                              (0x00000041u)
#define RXADC_L_REG                                          (0x00000041u)
#define RXADC_L_ADDR                                         (0x00000041u)
#define RXADC_L_RESET                                        (0x00000000u)
        /* UNUSED field */
        #define RXADC_L_UNUSED                               (0x0000F800u)
        #define RXADC_L_UNUSED_MASK                          (0x0000F800u)
        #define RXADC_L_UNUSED_BIT                           (11)
        #define RXADC_L_UNUSED_BITS                          (5)
        /* RXADC_GAIN field */
        #define RXADC_L_RXADC_GAIN                           (0x00000400u)
        #define RXADC_L_RXADC_GAIN_MASK                      (0x00000400u)
        #define RXADC_L_RXADC_GAIN_BIT                       (10)
        #define RXADC_L_RXADC_GAIN_BITS                      (1)
        /* RXADC_JAM field */
        #define RXADC_L_RXADC_JAM                            (0x000003FFu)
        #define RXADC_L_RXADC_JAM_MASK                       (0x000003FFu)
        #define RXADC_L_RXADC_JAM_BIT                        (0)
        #define RXADC_L_RXADC_JAM_BITS                       (10)

#define REVMARK                                              (0x00000050u)
#define REVMARK_REG                                          (0x00000050u)
#define REVMARK_ADDR                                         (0x00000050u)
#define REVMARK_RESET                                        (0x00000101u)
        /* REVMARK field */
        #define REVMARK_REVMARK                              (0x0000FFFFu)
        #define REVMARK_REVMARK_MASK                         (0x0000FFFFu)
        #define REVMARK_REVMARK_BIT                          (0)
        #define REVMARK_REVMARK_BITS                         (16)

#define PRESCALER                                            (0x00000060u)
#define PRESCALER_REG                                        (0x00000060u)
#define PRESCALER_ADDR                                       (0x00000060u)
#define PRESCALER_RESET                                      (0x00000000u)
        /* PRESCALER_TM field */
        #define PRESCALER_PRESCALER_TM                       (0x00008000u)
        #define PRESCALER_PRESCALER_TM_MASK                  (0x00008000u)
        #define PRESCALER_PRESCALER_TM_BIT                   (15)
        #define PRESCALER_PRESCALER_TM_BITS                  (1)
        /* UNUSED field */
        #define PRESCALER_UNUSED                             (0x00007800u)
        #define PRESCALER_UNUSED_MASK                        (0x00007800u)
        #define PRESCALER_UNUSED_BIT                         (11)
        #define PRESCALER_UNUSED_BITS                        (4)
        /* PRESCALER_MC_FORCE field */
        #define PRESCALER_PRESCALER_MC_FORCE                 (0x00000400u)
        #define PRESCALER_PRESCALER_MC_FORCE_MASK            (0x00000400u)
        #define PRESCALER_PRESCALER_MC_FORCE_BIT             (10)
        #define PRESCALER_PRESCALER_MC_FORCE_BITS            (1)
        /* PRESCALER_BOOST_5GDIV field */
        #define PRESCALER_PRESCALER_BOOST_5GDIV              (0x00000200u)
        #define PRESCALER_PRESCALER_BOOST_5GDIV_MASK         (0x00000200u)
        #define PRESCALER_PRESCALER_BOOST_5GDIV_BIT          (9)
        #define PRESCALER_PRESCALER_BOOST_5GDIV_BITS         (1)
        /* PRESCALER_BOOST_DIV2 field */
        #define PRESCALER_PRESCALER_BOOST_DIV2               (0x00000100u)
        #define PRESCALER_PRESCALER_BOOST_DIV2_MASK          (0x00000100u)
        #define PRESCALER_PRESCALER_BOOST_DIV2_BIT           (8)
        #define PRESCALER_PRESCALER_BOOST_DIV2_BITS          (1)
        /* PRESCALER_BOOST_MMDIVHI field */
        #define PRESCALER_PRESCALER_BOOST_MMDIVHI            (0x00000080u)
        #define PRESCALER_PRESCALER_BOOST_MMDIVHI_MASK       (0x00000080u)
        #define PRESCALER_PRESCALER_BOOST_MMDIVHI_BIT        (7)
        #define PRESCALER_PRESCALER_BOOST_MMDIVHI_BITS       (1)
        /* PRESCALER_BOOST_MMDIVLO field */
        #define PRESCALER_PRESCALER_BOOST_MMDIVLO            (0x00000040u)
        #define PRESCALER_PRESCALER_BOOST_MMDIVLO_MASK       (0x00000040u)
        #define PRESCALER_PRESCALER_BOOST_MMDIVLO_BIT        (6)
        #define PRESCALER_PRESCALER_BOOST_MMDIVLO_BITS       (1)
        /* PRESCALER_BIASI field */
        #define PRESCALER_PRESCALER_BIASI                    (0x0000003Fu)
        #define PRESCALER_PRESCALER_BIASI_MASK               (0x0000003Fu)
        #define PRESCALER_PRESCALER_BIASI_BIT                (0)
        #define PRESCALER_PRESCALER_BIASI_BITS               (6)

#define PHDET                                                (0x00000070u)
#define PHDET_REG                                            (0x00000070u)
#define PHDET_ADDR                                           (0x00000070u)
#define PHDET_RESET                                          (0x00000000u)
        /* PHDET_TM field */
        #define PHDET_PHDET_TM                               (0x00008000u)
        #define PHDET_PHDET_TM_MASK                          (0x00008000u)
        #define PHDET_PHDET_TM_BIT                           (15)
        #define PHDET_PHDET_TM_BITS                          (1)
        /* UNUSED field */
        #define PHDET_UNUSED                                 (0x00007F00u)
        #define PHDET_UNUSED_MASK                            (0x00007F00u)
        #define PHDET_UNUSED_BIT                             (8)
        #define PHDET_UNUSED_BITS                            (7)
        /* PHDET_BIAS_TRIM field */
        #define PHDET_PHDET_BIAS_TRIM                        (0x000000C0u)
        #define PHDET_PHDET_BIAS_TRIM_MASK                   (0x000000C0u)
        #define PHDET_PHDET_BIAS_TRIM_BIT                    (6)
        #define PHDET_PHDET_BIAS_TRIM_BITS                   (2)
        /* PHDET_BIASI field */
        #define PHDET_PHDET_BIASI                            (0x0000003Fu)
        #define PHDET_PHDET_BIASI_MASK                       (0x0000003Fu)
        #define PHDET_PHDET_BIASI_BIT                        (0)
        #define PHDET_PHDET_BIASI_BITS                       (6)

#define VCO                                                  (0x00000080u)
#define VCO_REG                                              (0x00000080u)
#define VCO_ADDR                                             (0x00000080u)
#define VCO_RESET                                            (0x00000000u)
        /* UNUSED field */
        #define VCO_UNUSED                                   (0x0000FC00u)
        #define VCO_UNUSED_MASK                              (0x0000FC00u)
        #define VCO_UNUSED_BIT                               (10)
        #define VCO_UNUSED_BITS                              (6)
        /* VCO_VAR_TRIM field */
        #define VCO_VCO_VAR_TRIM                             (0x00000380u)
        #define VCO_VCO_VAR_TRIM_MASK                        (0x00000380u)
        #define VCO_VCO_VAR_TRIM_BIT                         (7)
        #define VCO_VCO_VAR_TRIM_BITS                        (3)
        /* VCO_BIAS_BOOST field */
        #define VCO_VCO_BIAS_BOOST                           (0x00000040u)
        #define VCO_VCO_BIAS_BOOST_MASK                      (0x00000040u)
        #define VCO_VCO_BIAS_BOOST_BIT                       (6)
        #define VCO_VCO_BIAS_BOOST_BITS                      (1)
        /* VCO_BIASI field */
        #define VCO_VCO_BIASI                                (0x0000003Fu)
        #define VCO_VCO_BIASI_MASK                           (0x0000003Fu)
        #define VCO_VCO_BIASI_BIT                            (0)
        #define VCO_VCO_BIASI_BITS                           (6)

#define LOOPFILTER                                           (0x00000090u)
#define LOOPFILTER_REG                                       (0x00000090u)
#define LOOPFILTER_ADDR                                      (0x00000090u)
#define LOOPFILTER_RESET                                     (0x00000000u)
        /* LOOPFILTER_TM field */
        #define LOOPFILTER_LOOPFILTER_TM                     (0x0000C000u)
        #define LOOPFILTER_LOOPFILTER_TM_MASK                (0x0000C000u)
        #define LOOPFILTER_LOOPFILTER_TM_BIT                 (14)
        #define LOOPFILTER_LOOPFILTER_TM_BITS                (2)
        /* UNUSED field */
        #define LOOPFILTER_UNUSED                            (0x00002000u)
        #define LOOPFILTER_UNUSED_MASK                       (0x00002000u)
        #define LOOPFILTER_UNUSED_BIT                        (13)
        #define LOOPFILTER_UNUSED_BITS                       (1)
        /* LOOPFILTER_RESET field */
        #define LOOPFILTER_LOOPFILTER_RESET                  (0x00001000u)
        #define LOOPFILTER_LOOPFILTER_RESET_MASK             (0x00001000u)
        #define LOOPFILTER_LOOPFILTER_RESET_BIT              (12)
        #define LOOPFILTER_LOOPFILTER_RESET_BITS             (1)
        /* LOOPFILTER_BIASV field */
        #define LOOPFILTER_LOOPFILTER_BIASV                  (0x00000FC0u)
        #define LOOPFILTER_LOOPFILTER_BIASV_MASK             (0x00000FC0u)
        #define LOOPFILTER_LOOPFILTER_BIASV_BIT              (6)
        #define LOOPFILTER_LOOPFILTER_BIASV_BITS             (6)
        /* LOOPFILTER_BIASI field */
        #define LOOPFILTER_LOOPFILTER_BIASI                  (0x0000003Fu)
        #define LOOPFILTER_LOOPFILTER_BIASI_MASK             (0x0000003Fu)
        #define LOOPFILTER_LOOPFILTER_BIASI_BIT              (0)
        #define LOOPFILTER_LOOPFILTER_BIASI_BITS             (6)

#define PA                                                   (0x000000A0u)
#define PA_REG                                               (0x000000A0u)
#define PA_ADDR                                              (0x000000A0u)
#define PA_RESET                                             (0x00000000u)
        /* PA_TM field */
        #define PA_PA_TM                                     (0x00008000u)
        #define PA_PA_TM_MASK                                (0x00008000u)
        #define PA_PA_TM_BIT                                 (15)
        #define PA_PA_TM_BITS                                (1)
        /* PA_USE_FIB field */
        #define PA_PA_USE_FIB                                (0x00004000u)
        #define PA_PA_USE_FIB_MASK                           (0x00004000u)
        #define PA_PA_USE_FIB_BIT                            (14)
        #define PA_PA_USE_FIB_BITS                           (1)
        /* PA_BIAS field */
        #define PA_PA_BIAS                                   (0x00003800u)
        #define PA_PA_BIAS_MASK                              (0x00003800u)
        #define PA_PA_BIAS_BIT                               (11)
        #define PA_PA_BIAS_BITS                              (3)
        /* PA_BUFFER_BIAS field */
        #define PA_PA_BUFFER_BIAS                            (0x00000700u)
        #define PA_PA_BUFFER_BIAS_MASK                       (0x00000700u)
        #define PA_PA_BUFFER_BIAS_BIT                        (8)
        #define PA_PA_BUFFER_BIAS_BITS                       (3)
        /* PA_HIGH_BIAS field */
        #define PA_PA_HIGH_BIAS                              (0x00000080u)
        #define PA_PA_HIGH_BIAS_MASK                         (0x00000080u)
        #define PA_PA_HIGH_BIAS_BIT                          (7)
        #define PA_PA_HIGH_BIAS_BITS                         (1)
        /* PA_OUT_SEL field */
        #define PA_PA_OUT_SEL                                (0x00000040u)
        #define PA_PA_OUT_SEL_MASK                           (0x00000040u)
        #define PA_PA_OUT_SEL_BIT                            (6)
        #define PA_PA_OUT_SEL_BITS                           (1)
        /* PA_BIASI field */
        #define PA_PA_BIASI                                  (0x0000003Fu)
        #define PA_PA_BIASI_MASK                             (0x0000003Fu)
        #define PA_PA_BIASI_BIT                              (0)
        #define PA_PA_BIASI_BITS                             (6)

#define MODDAC                                               (0x000000B0u)
#define MODDAC_REG                                           (0x000000B0u)
#define MODDAC_ADDR                                          (0x000000B0u)
#define MODDAC_RESET                                         (0x00000000u)
        /* MODDAC_TM field */
        #define MODDAC_MODDAC_TM                             (0x00008000u)
        #define MODDAC_MODDAC_TM_MASK                        (0x00008000u)
        #define MODDAC_MODDAC_TM_BIT                         (15)
        #define MODDAC_MODDAC_TM_BITS                        (1)
        /* MODDAC_VREF_TRIM field */
        #define MODDAC_MODDAC_VREF_TRIM                      (0x00006000u)
        #define MODDAC_MODDAC_VREF_TRIM_MASK                 (0x00006000u)
        #define MODDAC_MODDAC_VREF_TRIM_BIT                  (13)
        #define MODDAC_MODDAC_VREF_TRIM_BITS                 (2)
        /* MODDAC_MODDATA field */
        #define MODDAC_MODDAC_MODDATA                        (0x00001FC0u)
        #define MODDAC_MODDAC_MODDATA_MASK                   (0x00001FC0u)
        #define MODDAC_MODDAC_MODDATA_BIT                    (6)
        #define MODDAC_MODDAC_MODDATA_BITS                   (7)
        /* MODDAC_BIASI field */
        #define MODDAC_MODDAC_BIASI                          (0x0000003Fu)
        #define MODDAC_MODDAC_BIASI_MASK                     (0x0000003Fu)
        #define MODDAC_MODDAC_BIASI_BIT                      (0)
        #define MODDAC_MODDAC_BIASI_BITS                     (6)

#define AUXADC                                               (0x000000C0u)
#define AUXADC_REG                                           (0x000000C0u)
#define AUXADC_ADDR                                          (0x000000C0u)
#define AUXADC_RESET                                         (0x00000000u)
        /* AUXADC_TM field */
        #define AUXADC_AUXADC_TM                             (0x0000C000u)
        #define AUXADC_AUXADC_TM_MASK                        (0x0000C000u)
        #define AUXADC_AUXADC_TM_BIT                         (14)
        #define AUXADC_AUXADC_TM_BITS                        (2)
        /* UNUSED field */
        #define AUXADC_UNUSED                                (0x00002000u)
        #define AUXADC_UNUSED_MASK                           (0x00002000u)
        #define AUXADC_UNUSED_BIT                            (13)
        #define AUXADC_UNUSED_BITS                           (1)
        /* AUXADC_EXTIN_EN field */
        #define AUXADC_AUXADC_EXTIN_EN                       (0x00001000u)
        #define AUXADC_AUXADC_EXTIN_EN_MASK                  (0x00001000u)
        #define AUXADC_AUXADC_EXTIN_EN_BIT                   (12)
        #define AUXADC_AUXADC_EXTIN_EN_BITS                  (1)
        /* AUXADC_BIASV field */
        #define AUXADC_AUXADC_BIASV                          (0x00000FC0u)
        #define AUXADC_AUXADC_BIASV_MASK                     (0x00000FC0u)
        #define AUXADC_AUXADC_BIASV_BIT                      (6)
        #define AUXADC_AUXADC_BIASV_BITS                     (6)
        /* AUXADC_BIASI field */
        #define AUXADC_AUXADC_BIASI                          (0x0000003Fu)
        #define AUXADC_AUXADC_BIASI_MASK                     (0x0000003Fu)
        #define AUXADC_AUXADC_BIASI_BIT                      (0)
        #define AUXADC_AUXADC_BIASI_BITS                     (6)

#define CALADC                                               (0x000000D0u)
#define CALADC_REG                                           (0x000000D0u)
#define CALADC_ADDR                                          (0x000000D0u)
#define CALADC_RESET                                         (0x00000000u)
        /* CALADC_TM field */
        #define CALADC_CALADC_TM                             (0x0000C000u)
        #define CALADC_CALADC_TM_MASK                        (0x0000C000u)
        #define CALADC_CALADC_TM_BIT                         (14)
        #define CALADC_CALADC_TM_BITS                        (2)
        /* UNUSED field */
        #define CALADC_UNUSED                                (0x00003000u)
        #define CALADC_UNUSED_MASK                           (0x00003000u)
        #define CALADC_UNUSED_BIT                            (12)
        #define CALADC_UNUSED_BITS                           (2)
        /* CALADC_BIASV field */
        #define CALADC_CALADC_BIASV                          (0x00000FC0u)
        #define CALADC_CALADC_BIASV_MASK                     (0x00000FC0u)
        #define CALADC_CALADC_BIASV_BIT                      (6)
        #define CALADC_CALADC_BIASV_BITS                     (6)
        /* CALADC_BIASI field */
        #define CALADC_CALADC_BIASI                          (0x0000003Fu)
        #define CALADC_CALADC_BIASI_MASK                     (0x0000003Fu)
        #define CALADC_CALADC_BIASI_BIT                      (0)
        #define CALADC_CALADC_BIASI_BITS                     (6)

#define IQMIXER                                              (0x000000E0u)
#define IQMIXER_REG                                          (0x000000E0u)
#define IQMIXER_ADDR                                         (0x000000E0u)
#define IQMIXER_RESET                                        (0x00000000u)
        /* IQMIXER_TM field */
        #define IQMIXER_IQMIXER_TM                           (0x00008000u)
        #define IQMIXER_IQMIXER_TM_MASK                      (0x00008000u)
        #define IQMIXER_IQMIXER_TM_BIT                       (15)
        #define IQMIXER_IQMIXER_TM_BITS                      (1)
        /* UNUSED field */
        #define IQMIXER_UNUSED                               (0x00004000u)
        #define IQMIXER_UNUSED_MASK                          (0x00004000u)
        #define IQMIXER_UNUSED_BIT                           (14)
        #define IQMIXER_UNUSED_BITS                          (1)
        /* IQMIXER_RFDAC_MSB field */
        #define IQMIXER_IQMIXER_RFDAC_MSB                    (0x00002000u)
        #define IQMIXER_IQMIXER_RFDAC_MSB_MASK               (0x00002000u)
        #define IQMIXER_IQMIXER_RFDAC_MSB_BIT                (13)
        #define IQMIXER_IQMIXER_RFDAC_MSB_BITS               (1)
        /* IQMIXER_DIVDAC field */
        #define IQMIXER_IQMIXER_DIVDAC                       (0x00001F00u)
        #define IQMIXER_IQMIXER_DIVDAC_MASK                  (0x00001F00u)
        #define IQMIXER_IQMIXER_DIVDAC_BIT                   (8)
        #define IQMIXER_IQMIXER_DIVDAC_BITS                  (5)
        /* IQMIXER_RFDAC field */
        #define IQMIXER_IQMIXER_RFDAC                        (0x000000C0u)
        #define IQMIXER_IQMIXER_RFDAC_MASK                   (0x000000C0u)
        #define IQMIXER_IQMIXER_RFDAC_BIT                    (6)
        #define IQMIXER_IQMIXER_RFDAC_BITS                   (2)
        /* IQMIXER_BIASI field */
        #define IQMIXER_IQMIXER_BIASI                        (0x0000003Fu)
        #define IQMIXER_IQMIXER_BIASI_MASK                   (0x0000003Fu)
        #define IQMIXER_IQMIXER_BIASI_BIT                    (0)
        #define IQMIXER_IQMIXER_BIASI_BITS                   (6)

#define BIAS_MASTER                                          (0x000000F0u)
#define BIAS_MASTER_REG                                      (0x000000F0u)
#define BIAS_MASTER_ADDR                                     (0x000000F0u)
#define BIAS_MASTER_RESET                                    (0x00000000u)
        /* BIAS_MASTER_TM field */
        #define BIAS_MASTER_BIAS_MASTER_TM                   (0x0000C000u)
        #define BIAS_MASTER_BIAS_MASTER_TM_MASK              (0x0000C000u)
        #define BIAS_MASTER_BIAS_MASTER_TM_BIT               (14)
        #define BIAS_MASTER_BIAS_MASTER_TM_BITS              (2)
        /* UNUSED field */
        #define BIAS_MASTER_UNUSED                           (0x00003000u)
        #define BIAS_MASTER_UNUSED_MASK                      (0x00003000u)
        #define BIAS_MASTER_UNUSED_BIT                       (12)
        #define BIAS_MASTER_UNUSED_BITS                      (2)
        /* BIAS_MASTER_BIASV field */
        #define BIAS_MASTER_BIAS_MASTER_BIASV                (0x00000FC0u)
        #define BIAS_MASTER_BIAS_MASTER_BIASV_MASK           (0x00000FC0u)
        #define BIAS_MASTER_BIAS_MASTER_BIASV_BIT            (6)
        #define BIAS_MASTER_BIAS_MASTER_BIASV_BITS           (6)
        /* BIAS_MASTER_BIASI field */
        #define BIAS_MASTER_BIAS_MASTER_BIASI                (0x0000003Fu)
        #define BIAS_MASTER_BIAS_MASTER_BIASI_MASK           (0x0000003Fu)
        #define BIAS_MASTER_BIAS_MASTER_BIASI_BIT            (0)
        #define BIAS_MASTER_BIAS_MASTER_BIASI_BITS           (6)


#endif /* __REGS_INDIRECT_ANALOGUE_H__ */

