SEGGER SystemView for uC/OS-III

Download the embedded target code to support Segger's SystemView for uC/OS-III 
from the following website https://www.segger.com/downloads/free_tools#SystemView 
and place the files in this folder.

In the end, this folder should look something like this:

    \---ThirdPartyLibrary
        |   readme.txt
        |
        +---Config
        |       SEGGER_RTT_Conf.h
        |       SEGGER_SYSVIEW_Conf.h
        |
        +---Sample
        |   +---Config
        |   |       SEGGER_SYSVIEW_Config_uCOSIII.c
        |   |
        |   \---OS
        |           SEGGER_SYSVIEW_uCOSIII.c
        |
        \---SEGGER
                Global.h
                SEGGER.h
                SEGGER_RTT.c
                SEGGER_RTT.h
                SEGGER_SYSVIEW.c
                SEGGER_SYSVIEW.h
                SEGGER_SYSVIEW_ConfDefaults.h
                SEGGER_SYSVIEW_Int.h



