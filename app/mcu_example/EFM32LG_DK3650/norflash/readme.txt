Norflash example.

This example demonstrates use of the NORFLASH driver.

Connect a terminal to the serialport of the DK (115200-N-8-1).
Operations on the flash are initiated by issuing commands on the terminal.
Command "h" will print a help screen on the terminal :

    Available commands:
        fi        : Show flash information
        h         : Show this help
        dump n    : Show first 16 bytes of sector n
        dumps n   : Show entire sector n content
        blc n     : Blankcheck sector n
        bld       : Blankcheck entire device
        erase n   : Erase sector n
        eraseall  : Erase entire device
        prog n    : Program first 16 bytes of sector n with byte pattern
        exec n    : Execute "blink" from sector n
        misc      : Test misc. NORFLASH API functions
        rspeed    : Measure flash read speed

Board:  Silicon Labs EFM32LG-DK3650 Development Kit
Device: EFM32LG990F256
