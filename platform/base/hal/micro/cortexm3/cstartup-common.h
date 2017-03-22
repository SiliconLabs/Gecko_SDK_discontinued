//=============================================================================
// FILE
//   cstartup.c - Startup and low-level utility code for the Cortex-M3 with the
//                IAR tool chain.
//
// DESCRIPTION
//   This file defines the basic information needed to go from reset up to
//   the main() found in C code.
//
//   Copyright 2009 by Ember Corporation. All rights reserved.             *80*
//=============================================================================


// Forward declaration for the debugging ISR shim
void halInternalIntDebuggingIsr(void);

// Determines reset cause
void halInternalClassifyReset(void);

//=============================================================================
// Provide a prototype for the real C code main() in the application.
//=============================================================================
extern int main(void);


extern const HalVectorTableType __vector_table[];

