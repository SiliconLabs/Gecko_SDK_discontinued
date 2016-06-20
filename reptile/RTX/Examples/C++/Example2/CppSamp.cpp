/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    CppSamp.cpp
 *      Purpose: C++ example with operator overloading and friend functions
 *----------------------------------------------------------------------------
 *
 * Copyright (c) 1999-2009 KEIL, 2009-2013 ARM Germany GmbH
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of ARM  nor the names of its contributors may be used 
 *    to endorse or promote products derived from this software without 
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "cmsis_os.h"

extern "C"  {
  extern void SER_Init (void);          // Initialize Serial Interface
}


class Complex {
  int re_part;
  int im_part;

public:
  Complex(int re, int im) { re_part = re; im_part = im; }
  Complex(int re)         { re_part = re; im_part = 0;  }
  Complex()               { re_part = 0;  im_part = 0;  }

  void   Print(int i1);
  void   Print();
  friend Complex operator+(Complex, Complex);
  friend Complex operator-(Complex, Complex);
  friend Complex operator-(Complex);

  void   SetKey(unsigned int n);

private:
  unsigned int m_KeyCode;
};


void Complex::SetKey(unsigned int n) {
  m_KeyCode = n;
}

void Complex::Print(int i1) {
  printf("i1 = %d\n", i1);
}

void Complex::Print(void) {
  printf("re = %d, im = %d\n", re_part, im_part);
}

Complex operator+(Complex c1, Complex c2) {
  return Complex (c1.re_part + c2.re_part, c1.im_part + c2.im_part);
}

Complex operator-(Complex c1, Complex c2) {
  return Complex (c1.re_part - c2.re_part, c1.im_part - c2.im_part);
}

Complex operator-(Complex c1) {
  return Complex(-c1.re_part, -c1.im_part);
}


int    Adj_i;
int  Adj_N_i;

typedef struct myt {
  short int  node;
  short int  type;
  struct myt   *L;
  struct myt   *R;
} MyType;

namespace ADJ {
  int    Adj_i;
  namespace ADJ_N {
    int Adj_N_i;
  }
  MyType   ex1;
  MyType   ex2;
}


/* Forward reference */
void threadX (void const *argument);

/* Thread IDs */
osThreadId main_id;
osThreadId threadX_id;

/* Thread definitions */
osThreadDef(threadX, osPriorityNormal, 1, 0);


/*
 * Thread X
 */
void threadX (void const *argument) {
  for (;;) {
    /* Wait for completion of do-this */
    osSignalWait(0x0001, osWaitForever);
    
    /* do-that */
    Adj_i   = 1000;
    Adj_N_i = 2000;
    
    ADJ::Adj_i          = 2;
    ADJ::ADJ_N::Adj_N_i = 2;

    ADJ::ex1.node = 41;
    ADJ::ex2.node = 42;
    ADJ::ex1.type = 10;
    ADJ::ex2.type = 11;

    /* Indicate to main thread completion of do-that */
    osSignalSet(main_id, 0x0001);
  }
}


/*
 * Main Thread
 */
int main (void) {

  SER_Init();  // Initialize Serial Interface

  printf("Example: CppSamp - Operator overloading / friend functions.\r\n"
         "===========================================================\r\n\r\n");

  /* Get main thread ID */
  main_id = osThreadGetId();
  
  /* Create thread X */
  threadX_id = osThreadCreate(osThread(threadX), NULL);

  /* Indicate to thread X completion of do-this */
  osSignalSet(threadX_id, 0x0001);
  
  /* Wait for completion of do-that */
  osSignalWait(0x0001, osWaitForever);

  Complex c1(1,2);
  Complex c2(2);
  Complex c3;

  c1.SetKey(1000);

  printf("Complex number c1: ");
  c1.Print();
  printf("Complex number c2: ");
  c2.Print();

  printf("Calculate c1 + c2: ");
  c3 = c1 + c2;
  c3.Print();

  printf("Calculate c2 - c1: ");
  c3 = c2 - c1;
  c3.Print();

  printf("Negate previous  : ");
  c3 = -c3;
  c3.Print();

  c3.Print(10);
  
  printf("\nEnd of Program.\r\n");

  while (1);
}
