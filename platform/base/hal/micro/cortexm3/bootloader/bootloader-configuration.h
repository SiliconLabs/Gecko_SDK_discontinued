//=============================================================================
// FILE
//   bootloader-configuration.h - Various configuration options for the 
//       application and standalone bootloaders.
//
// DESCRIPTION
//   This file holds any customer accessible configuration options for the
//   Ember bootloaders.
//
//   Copyright 2012 by Ember Corporation. All rights reserved.             *80*
//=============================================================================


// The CUSTOMER_BOOTLOADER_VERSION allows you to track the version of your
// bootloader independent of the Ember release version. This value will be 
// stored at a fixed location in memory and be accessible by all Ember tools
// and through an API. The value can be any 32bit integer, however, the value of
// 0xFFFFFFFF is reserved to indicate an unset customer version number.
//
// #define CUSTOMER_BOOTLOADER_VERSION 1

