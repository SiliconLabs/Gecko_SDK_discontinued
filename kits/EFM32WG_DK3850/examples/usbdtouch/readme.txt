USB Bitmap transfer using USB drive functionality.

This example demonstrate use several functionalities:
1. Creation of virtual drive in system with FAT FS,
2. Mounting the drive on PC and file transfer,
3. Bitmap file creation based on TFT frame buffer content,
4. Resistive touch panel interaction.

On system startup initial drive is created and 
formatted using FAT FS then simple readme.txt file 
is put on file system. Every time user press PB4 key
new file, containing TFT frame buffer in bitmap format
is added. All files could be retrieved after connecting
board to PC by means of USB. For this connection use
small USB socket located on Wonder Gecko CPU board, not
the big one on development kit.

If new files doesn't appear on drive after pressing PB4,
try to reconnect the board to PC.

Board:  Silicon Labs EFM32WG-DK3850 Development Kit
Device: EFM32WG990F256
