uC/OS-III + GLIB example using the Sharp Memory LCD on the SLSTK3402A.

This example is using uC/OS-III to run multiple tasks at the same time 
and two of the task are drawing to the Sharp Memory LCD using GLIB.
There are in total 4 uC/OS-III application tasks created and active in this 
example.

"Start Task": This task is the first to start and it will create
the other tasks along with the shared uC/OS-III objects used in the 
application. Once the initialization is done this task will start to blink
LED0 on the kit every second.

"Toggle Task": The Sharp Memory LCD requires the user to toggle a gpio
pin at 60 Hz, and this task has the responsibility to do this.

"Display Task": This task will display a text on the screen and will 
animate a ball bouncing on the top portion of the Sharp Memory LCD. This 
task will run regularly in order to acheive a smooth animation.

"Image Task": This task will draw an image on the bottom part of the 
Sharp Memory LCD. Once an image is drawn on the screen this task will wait
for one of the push buttons to be pressed until it displays another image.

All access to the display is being protected by a mutex provided by uC/OS-III.

The user can use the PB0 and PB1 buttons on the kit to change the image
that is being displayed on the bottom part of the Sharp Memory LCD.

Board:  Silicon Labs SLSTK3402A Starter Kit
Device: EFM32PG12B500F1024GL125
