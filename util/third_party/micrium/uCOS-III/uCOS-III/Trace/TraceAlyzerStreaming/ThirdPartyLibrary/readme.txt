Percepio TraceAlyzer for uC/OS-III (STREAMING MODE)

Download the embedded target code to support Percepio's TraceAlyzer for µC/OS-III (Streaming)
from the following website http://percepio.com/download and place the files in this folder.

In the end, this folder should look something like this:


    \---ThirdPartyLibrary
        |   readme.txt
        |   trcHardwarePort.h
        |   trcKernelPort.c
        |   trcKernelPort.h
        |   trcPagedEventBuffer.c
        |   trcPagedEventBuffer.h
        |   trcPagedEventBufferConfig.h
        |   trcRecorder.c
        |   trcRecorder.h
        |   trcStreamPort.h
        |   trcTCPIP.c
        |   trcTCPIP.h
        |   trcTCPIPConfig.h
        |
        +---ConfigurationTemplate
        |       trcConfig.h
        |
        \---Segger
            \---RTT
                    SEGGER_RTT.c
                    SEGGER_RTT.h
                    SEGGER_RTT_Conf.h
                    SEGGER_RTT_printf.c