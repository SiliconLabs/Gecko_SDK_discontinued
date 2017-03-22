/***************************************************************************//**
 * @file emlib_config.h
 * @brief EMLIB configuration file.
 * @version 5.0.0
 *******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/
#ifndef __SILICON_LABS_EMLIB_CONFIG_H__
#define __SILICON_LABS_EMLIB_CONFIG_H__

#if defined(CORTEXM3_EZR32HG_MICRO) || (CORTEXM3_EZR32HG)
#define CORE_ATOMIC_METHOD    CORE_ATOMIC_METHOD_PRIMASK
#else
#define CORE_ATOMIC_METHOD    CORE_ATOMIC_METHOD_BASEPRI
#endif

#define CORE_ATOMIC_BASE_PRIORITY_LEVEL 3

#endif /* __SILICON_LABS_EMLIB_CONFIG_H__ */
