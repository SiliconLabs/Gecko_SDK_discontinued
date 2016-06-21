/***************************************************************************//**
 * @file    iec60335_class_b_critical_data.h
 * @brief   header for c based secure data storage test
 *              POST and BIST secure data storage test for all compiler
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __IEC60335_CLASS_B_CRITCAL_DATA_H__
#define __IEC60335_CLASS_B_CRITCAL_DATA_H__

/*! @addtogroup IEC60335_CRITICAL_DATA
 * @{
 */

#include "iec60335_class_b_typedef.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*! @file iec60335_b_critical_data.h
 */

/*! \brief typedef for a critical floating point variable with 64 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_double_struct
{
  double inverse;       /*!< The complement of the data content of the critical variable */
  double data;          /*!< Data content of the critical variable */
} critical_double;

/*! \brief typedef for a critical floating point variable with 32 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_float_struct
{
  float inverse;    /*!< The complement of the data content of the critical variable */
  float data;       /*!< Data content of the critical variable */
} critical_float;

/*! \brief typedef for a critical unsigned integer with 64 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_uint64_t_struct
{
  uint64_t inverse;       /*!< The complement of the data content of the critical variable */
  uint64_t data;          /*!< Data content of the critical variable */
} critical_uint64_t;

/*! \brief typedef for a critical signed integer with 64 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_int64_t_struct
{
  int64_t inverse;        /*!< The complement of the data content of the critical variable */
  int64_t data;           /*!< Data content of the critical variable */
} critical_int64_t;

/*! \brief typedef for a critical unsigned integer with 32 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_uint32_t_struct
{
  uint32_t inverse;         /*!< The complement of the data content of the critical variable */
  uint32_t data;            /*!< Data content of the critical variable */
} critical_uint32_t;

/*! \brief typedef for a critical signed integer with 32 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_int32_t_struct
{
  int32_t inverse;          /*!< The complement of the data content of the critical variable */
  int32_t data;             /*!< Data content of the critical variable */
} critical_int32_t;

/*! \brief typedef for a critical unsigned integer with 16 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_uint16_t_struct
{
  uint16_t inverse;         /*!< The complement of the data content of the critical variable */
  uint16_t data;            /*!< Data content of the critical variable */
} critical_uint16_t;

/*! \brief typedef for a critical signed integer with 16 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_int16_t_struct
{
  int16_t inverse;          /*!< The complement of the data content of the critical variable */
  int16_t data;             /*!< Data content of the critical variable */
} critical_int16_t;

/*! \brief typedef for a critical unsigned integer with 8 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_uint8_t_struct
{
  uint8_t inverse;          /*!< The complement of the data content of the critical variable */
  uint8_t data;             /*!< Data content of the critical variable */
} critical_uint8_t;

/*! \brief typedef for a critical signed integer with 8 bits.
 *
 *  This variable has to be checked with the macro \a IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataValidate(criticalVar)
 *
 *  \attention Do only use the macros to read or write the variable.
 *  \see IEC60335_ClassB_CriticalDataPop(criticalVar)
 *  \see IEC60335_ClassB_CriticalDataPush(criticalVar, value)
 *
 *  \attention To instance such a critical variable, the macro \a IEC60335_ClassB_CriticalDataInit(value)
 *  has to be used to initialize the struct.
 */
typedef struct critical_int8_t_struct
{
  int8_t inverse;           /*!< The complement of the data content of the critical variable */
  int8_t data;              /*!< Data content of the critical variable */
} critical_int8_t;

/*! @brief Macro to check critical variables.
 *
 * @param criticalVar  target variable of critical type
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 */
#define IEC60335_ClassB_CriticalDataValidate(criticalVar) \
  ((criticalVar.data == ~(criticalVar.inverse)) ? (IEC60335_testPassed) : (IEC60335_testFailed))

/*! @brief Macro to read critical variables.
 *
 *  @param criticalVar  target variable of critical type
 */
#define IEC60335_ClassB_CriticalDataPop(criticalVar) \
  (criticalVar.data)

/*! @brief Macro to write critical variables.
 *
 *  @param criticalVar  target variable of critical type
 *  @param value        new value
 */
#define IEC60335_ClassB_CriticalDataPush(criticalVar, value)    { \
    criticalVar.inverse = ~value;                                 \
    criticalVar.data    = value;                                  \
}

/*! @brief Macro to initialize critical variables.
 *
 *  necessary to initialize every type of critical data with start value
 *  @param value
 */
#define IEC60335_ClassB_CriticalDataInit(value)                 { \
    ~value, value                                                 \
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* __IEC60335_CLASS_B_CRITCAL_DATA_H__ */

/************************************** EOF *********************************/
