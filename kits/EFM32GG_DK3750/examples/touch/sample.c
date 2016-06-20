/*
 *
 *   Copyright (c) 2001, Carlos E. Vidales. All rights reserved.
 * 
 *   This sample program was written and put in the public domain 
 *    by Carlos E. Vidales.  The program is provided "as is" 
 *    without warranty of any kind, either expressed or implied.
 *   If you choose to use the program within your own products
 *    you do so at your own risk, and assume the responsibility
 *    for servicing, repairing or correcting the program should
 *    it prove defective in any manner.
 *   You may copy and distribute the program's source code in any 
 *    medium, provided that you also include in each copy an
 *    appropriate copyright notice and disclaimer of warranty.
 *   You may also modify this program and distribute copies of
 *    it provided that you include prominent notices stating 
 *    that you changed the file(s) and the date of any change,
 *    and that you do not charge any royalties or licenses for 
 *    its use.
 *    
 *
 *   File Name:  sample.c
 *
 *
 */


#define _SAMPLE_C_



/****************************************************/
/*                                                  */
/* Included files                                   */
/*                                                  */
/****************************************************/

#include <stdio.h>

#include "calibrate.h"



/****************************************************/
/*                                                  */
/* Local Definitions                                */
/*                                                  */
/****************************************************/

#define		MAX_SAMPLES		    8


/****************************************************/
/*                                                  */
/* Local Variables                                  */
/*                                                  */
/****************************************************/

        /* NOTE: Even though the validity of the calibration/translation method  */
        /*        proposed has been verified with empirical data from several    */
        /*        actual touch screen enabled displays, for the convenience of   */
        /*        this exercise, the raw and expected data used and presented    */
        /*        below are artificial.  When used with actual data the          */
        /*        functions presented yield results that may be off by a larger  */
        /*        but still small percentage (~1-3%) due to electrical noise and */
        /*        human error (i.e., the hand touching a screen target and       */
        /*        missing by a small amount.)                                    */


		/* The array of input points.  The first three are used for calibration. */
        /* These set of points assume that the touchscreen has vertical and      */
        /*  horizontal resolutions of 1024 pixels (10-bit digitizer.)            */ 

POINT screenSample[MAX_SAMPLES] =	{
                                            {  73, 154 },
											{ 891, 516 },
											{ 512, 939 },
											{ 265, 414 },
											{ 606, 171 },
											{ 768, 700 },
											{ 111, 956 },
                                            { 448, 580 }
									} ;



		/* The array of expected "right answers."  The values selected assume a  */
        /*  vertical and horizontal display resolution of 240 pixels.            */

POINT displaySample[MAX_SAMPLES] =	{
                                            {  30,  30 },
											{ 210, 120 },
											{ 120, 210 },
                                            {  70,  90 },
											{ 150,  40 },
											{ 180, 160 },
                                            {  30, 210 },
											{ 110, 130 }
									} ;


		/* An array of perfect input screen points used to obtain a first pass   */
        /*  calibration matrix good enough to collect calibration samples.       */

POINT perfectScreenSample[3] =	{
                                            { 100, 100 },
											{ 900, 500 },
                                            { 500, 900 }
                                } ;



		/* An array of perfect display points used to obtain a first pass        */
        /*  calibration matrix good enough to collect calibration samples.       */

POINT perfectDisplaySample[3] = {
                                            { 100, 100 },
											{ 900, 500 },
                                            { 500, 900 }
	                            } ;


/****************************************************/
/*                                                  */
/* Forward Declaration of local functions           */
/*                                                  */
/****************************************************/

int   main( int argCount, char ** argValue ) ;
void  greeting( void ) ;





/**********************************************************************
 *
 *     Function: main()
 *
 *  Description: Entry point into console version of sample
 *                program that exercises the calibration 
 *                functions.
 * 
 *  Argument(s): argCount - the number of arguments provided
 *               argValue - pointer to the list of char strings 
 *                           representing the command line arguments.
 * 
 *       Return: void
 *
 */
int main( int argCount, char ** argValue )
{

    int retValue = OK ;

    MATRIX  matrix ;
    POINT   display ;
    
    
    int     n ;


      
    greeting() ;


        /* The following call calculates the translation matrix that   */
        /*  results when the three consecutive points in the sample    */
        /*  set are used.  Such points are assumed to be properly      */
        /*  spaced within the screen surface.                          */
        /* Note that we call the function twice as we would normally   */
        /*  do within a calibration routine.  The first time we call   */
        /*  it using a perfect set of display and screen arguments.    */
        /* Such a call is made to obtain a calibration matrix that is  */
        /*  just good enough to collect samples to do the real         */
        /*  calibration.                                               */
        /*                                                             */
        /*                                                             */
        /*                                                             */
        /*                 NOTE!    NOTE!    NOTE!                     */
        /*                                                             */
        /*  setCalibrationMatrix() and getDisplayPoint() will do fine  */
        /*  for you as they are, provided that your digitizer          */
        /*  resolution does not exceed 10 bits (1024 values).  Higher  */
        /*  resolutions may cause the integer operations to overflow   */
        /*  and return incorrect values.  If you wish to use these     */
        /*  functions with digitizer resolutions of 12 bits (4096      */
        /*  values) you will either have to a) use 64-bit signed       */
        /*  integer variables and math, or b) judiciously modify the   */
        /*  operations to scale results by a factor of 2 or even 4.    */
        /*                                                             */


    setCalibrationMatrix( &perfectDisplaySample[0], 
						  &perfectScreenSample[0], 
						  &matrix ) ;

        /* Look at the matrix values when we use a perfect sample set. */
        /* The result is a unity matrix.                               */
    printf("\n\nLook at the unity matrix:\n\n"
           "matrix.An = % 8ld  matrix.Bn = % 8ld  matrix.Cn = % 8ld\n"
           "matrix.Dn = % 8ld  matrix.En = % 8ld  matrix.Fn = % 8ld\n"
           "matrix.Divider = % 8ld\n",
           matrix.An,matrix.Bn,matrix.Cn,
           matrix.Dn,matrix.En,matrix.Fn,
           matrix.Divider ) ;





        /* Now is when we need to do the work to collect a real set of */
        /*  calibration data.                                          */

        /* Draw three targets on your display. Drawing one at time is  */
        /*  probably a simpler implementation. These targets should be */
        /*  widely separated but also avoid the areas too near the     */
        /*  edges where digitizer output tends to become non-linear.   */
        /*  The recommended set of points is (in display resolution    */
        /*   percentages):                                             */
        /*                                                             */
        /*                  ( 15, 15)                                  */
        /*                  ( 50, 85)                                  */
        /*                  ( 85, 50)                                  */
        /*                                                             */
        /* Each time save the display and screen set (returned by the  */
        /*  digitizer when the user touches each calibration target    */
        /*  into the corresponding array).                             */ 
        /* Since you normalized your calibration matrix above, you     */
        /*  should be able to use touch screen data as it would be     */
        /*  provided by the digitizer driver.  When the matrix equals  */
        /*  unity, getDisplayPoint() returns the same raw input data   */
        /*  as output.                                                 */



    


        /* Call the function once more to obtain the calibration       */
        /*  factors you will use until you calibrate again.            */
    setCalibrationMatrix( &displaySample[0], &screenSample[0], &matrix ) ;

        /* Let's see the matrix values for no particular reason.       */
    printf("\n\nThis is the actual calibration matrix that we will use\n"
           "for all points (until we calibrate again):\n\n"
           "matrix.An = % 8d  matrix.Bn = % 8d  matrix.Cn = % 8d\n"
           "matrix.Dn = % 8d  matrix.En = % 8d  matrix.Fn = % 8d\n"
           "matrix.Divider = % 8d\n",
           matrix.An,matrix.Bn,matrix.Cn,
           matrix.Dn,matrix.En,matrix.Fn,
           matrix.Divider ) ;



        /* Now, lets use the complete set of screen samples to verify  */
        /*  that the calculated calibration matrix does its job as     */
        /*  expected.                                                  */
    printf("\n\nShow the results of our work:\n\n"
           "  Screen Sample    Translated Sample    Display Sample\n\n" ) ;


        /* In a real application, your digitizer driver interrupt      */
        /*  would probably do the following:                           */
        /*      1) collect raw digitizer data,                         */
        /*      2) filter the raw data which would probably contain    */
        /*          position jitter,                                   */
        /*      3) filter out repeated values (a touch screen          */
        /*          controller normally continues causing interrupts   */
        /*          and collecting data as long as the user is         */
        /*          pressing on the screen), and                       */
        /*      4) call the function getDisplayPoint() to obtain       */
        /*          the display coordinates that the user meant to     */
        /*          input as he touched the screen.                    */
        /*                                                             */
        /* This code sample, of course, only uses sample data.  So we  */
        /*  simply run through all the available sample points.        */

    for( n = 0 ; n < MAX_SAMPLES ; ++n )
    {
        getDisplayPoint( &display, &screenSample[n], &matrix ) ;
        printf("  % 6d,%-6d      % 6d,%-6d       % 6d,%-6d\n", 
                screenSample[n].x,  screenSample[n].y,
                display.x,          display.y,
                displaySample[n].x, displaySample[n].y ) ;
    }


    return( retValue ) ;

} // end of main() 



/**********************************************************************
 *
 *     Function: greeting()
 *
 *  Description: Provides the sample program's welcome message.
 * 
 *  Argument(s): None
 * 
 *       Return: void
 *
 */
void greeting( void ) 
{
    printf("\n"
           "Touch Screen Calibration Sample Code,\n"
           " by Carlos E. Vidales, Copyright 2001.\n"
           "\n" ) ;
}


