nvm_simple - non-volatile memory driver usage simple example

This example shows simple usage of NVM module.

It stores object (32bit word in this case) into flash region declared 
as non-volatile memory area. It uses wear leveling and demonstrates 
simple use of the NVM driver. There are two types of pages: "normal" 
dedicated for storing multiple objects which doesn't change often 
and "wear" which can store single object (but this could be structure
containing many fields) that can often change. In this example on two pages 
same object is stored but NVM could handle multiple pages with different objects.

PB0 - short press recalls data from "normal" page
PB0 - long press store data to "normal" page
PB1 - counts up, and after releasing stores data to "wear" page

RESET - resets CPU and if there were valid data in NVM recovers last data value.

Board:  Silicon Labs EFM32_Gxx_STK Development Kit
Device: EFM32G890F128
