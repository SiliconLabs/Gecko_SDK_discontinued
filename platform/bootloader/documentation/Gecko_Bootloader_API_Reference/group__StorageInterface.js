var group__StorageInterface =
[
    [ "BootloaderStorageSlot_t", "structBootloaderStorageSlot__t.html", [
      [ "address", "structBootloaderStorageSlot__t.html#a03509836fd15641a0b075a4a584d6e7d", null ],
      [ "length", "structBootloaderStorageSlot__t.html#aa68c7e64791be8fea596b2156ca246a8", null ]
    ] ],
    [ "BootloaderStorageImplementationInformation_t", "structBootloaderStorageImplementationInformation__t.html", [
      [ "version", "structBootloaderStorageImplementationInformation__t.html#a9248b4089672ec4e67edd756f56d5686", null ],
      [ "capabilitiesMask", "structBootloaderStorageImplementationInformation__t.html#a18a12a54fc8f2ec179155f9ade6b71da", null ],
      [ "pageEraseMs", "structBootloaderStorageImplementationInformation__t.html#aa8715dff6aeca2441dc990cc472fd0eb", null ],
      [ "partEraseMs", "structBootloaderStorageImplementationInformation__t.html#a22eae0646d1a0865e53225cef10df70b", null ],
      [ "pageSize", "structBootloaderStorageImplementationInformation__t.html#a23fd0b47d1dc4a481088fb65270f398d", null ],
      [ "partSize", "structBootloaderStorageImplementationInformation__t.html#a9db9d056b6765daf977c49d801c6882c", null ],
      [ "partDescription", "structBootloaderStorageImplementationInformation__t.html#a851962d9b34680c41377b5b7a2175488", null ],
      [ "wordSizeBytes", "structBootloaderStorageImplementationInformation__t.html#ad215620f489fe7ee1b99eff609379f2b", null ]
    ] ],
    [ "BootloaderStorageInformation_t", "structBootloaderStorageInformation__t.html", [
      [ "version", "structBootloaderStorageInformation__t.html#ab9dfc13cc54e4f1b5099f9960a305081", null ],
      [ "capabilities", "structBootloaderStorageInformation__t.html#a9ef2cd56fc5d7b60a5a282294070b5e5", null ],
      [ "storageType", "structBootloaderStorageInformation__t.html#aeb6e26312304de39a181e35e11f85ca3", null ],
      [ "numStorageSlots", "structBootloaderStorageInformation__t.html#abe55879639007a8da6fdf26a0b3d89b5", null ],
      [ "info", "structBootloaderStorageInformation__t.html#a33134338418ec2ce0787661dbfbb7797", null ]
    ] ],
    [ "BootloaderStorageFunctions", "structBootloaderStorageFunctions.html", [
      [ "version", "structBootloaderStorageFunctions.html#a786fd614d84ebdb1c67b250c74dea1ea", null ],
      [ "getInfo", "structBootloaderStorageFunctions.html#a2ca3b970aa114186ba621969e95c96c4", null ],
      [ "getSlotInfo", "structBootloaderStorageFunctions.html#ae23ccb78c7f96230e5c5c94ca8857009", null ],
      [ "read", "structBootloaderStorageFunctions.html#a1bb2a6599e786a5453bf5a75758dad87", null ],
      [ "write", "structBootloaderStorageFunctions.html#a585063441c2e259d5bba5c723586b987", null ],
      [ "erase", "structBootloaderStorageFunctions.html#a86f964227919fe24051dae7aad6704db", null ],
      [ "setImagesToBootload", "structBootloaderStorageFunctions.html#a51e198fdf3f21e1d0547a606d86d93fb", null ],
      [ "getImagesToBootload", "structBootloaderStorageFunctions.html#a285d8fdcf01c07c5851996de566f01f8", null ],
      [ "appendImageToBootloadList", "structBootloaderStorageFunctions.html#abf07512d110b275841f65877b5f61f25", null ],
      [ "initParseImage", "structBootloaderStorageFunctions.html#a8a723554fa924f53f871611c55d2683a", null ],
      [ "verifyImage", "structBootloaderStorageFunctions.html#adca4b94b84a873d8de842a6ce9078c26", null ],
      [ "getImageInfo", "structBootloaderStorageFunctions.html#ab877838a9080da2b104746e67a059391", null ],
      [ "isBusy", "structBootloaderStorageFunctions.html#afa0c4dde3082804252568bfdf0fa3de3", null ],
      [ "readRaw", "structBootloaderStorageFunctions.html#aa14110bf5b71cdf36265f6dc6f194bdd", null ],
      [ "writeRaw", "structBootloaderStorageFunctions.html#a5b70b9cfe35f905a6f53249bf5ffdcd1", null ],
      [ "eraseRaw", "structBootloaderStorageFunctions.html#a63c1e835773e5836b9a97febde402b70", null ]
    ] ],
    [ "BOOTLOADER_STORAGE_IMPL_INFO_VERSION", "group__StorageInterface.html#gaa3ec112096e9442dff6c12b3adf7b7ca", null ],
    [ "BOOTLOADER_STORAGE_IMPL_INFO_VERSION_MAJOR", "group__StorageInterface.html#ga1f4839ed2d057841b664d9b3a26f5246", null ],
    [ "BOOTLOADER_STORAGE_IMPL_INFO_VERSION_MAJOR_MASK", "group__StorageInterface.html#gac2d221e19a293930ec7b2e915c21708b", null ],
    [ "BOOTLOADER_STORAGE_IMPL_CAPABILITY_ERASE_SUPPORTED", "group__StorageInterface.html#ga329ca9f415fb1ba646d86eb67344f82a", null ],
    [ "BOOTLOADER_STORAGE_IMPL_CAPABILITY_PAGE_ERASE_REQUIRED", "group__StorageInterface.html#ga2c8c8994d789e65d91aa9c5e0eea7f94", null ],
    [ "BOOTLOADER_STORAGE_IMPL_CAPABILITY_BLOCKING_WRITE", "group__StorageInterface.html#ga1ec762d826e7358a9e099a0fec71fd09", null ],
    [ "BOOTLOADER_STORAGE_IMPL_CAPABILITY_BLOCKING_ERASE", "group__StorageInterface.html#gab1b14dc36ecc7926f556019332c02808", null ],
    [ "BootloaderStorageFunctions_t", "group__StorageInterface.html#gaa89e144ac8860e6438fa05de6ef4339c", null ],
    [ "BootloaderStorageType_t", "group__StorageInterface.html#gac5a52f72db78d655840177f0e034d37c", [
      [ "SPIFLASH", "group__StorageInterface.html#ggac5a52f72db78d655840177f0e034d37caaf1113ff5007b46ad4e50ef301ed299b", null ],
      [ "INTERNAL_FLASH", "group__StorageInterface.html#ggac5a52f72db78d655840177f0e034d37ca5e341ba74bb0afd21095eabe0f8eba9b", null ],
      [ "CUSTOM", "group__StorageInterface.html#ggac5a52f72db78d655840177f0e034d37ca945d6010d321d9fe75cbba7b6f37f3b5", null ]
    ] ],
    [ "bootloader_getStorageInfo", "group__StorageInterface.html#gafc4cc80fcbb0bfea595915b11a6b5828", null ],
    [ "bootloader_getStorageSlotInfo", "group__StorageInterface.html#ga08d0906c70cd68f8119b8f11319cc5ff", null ],
    [ "bootloader_readStorage", "group__StorageInterface.html#gacd7f4e8b6bba7fd8689a9a2da9511afd", null ],
    [ "bootloader_writeStorage", "group__StorageInterface.html#ga86dae5018d8726a68dd826240d8f2c52", null ],
    [ "bootloader_eraseStorageSlot", "group__StorageInterface.html#gae14880e1483f25bd67f1b96e5345862f", null ],
    [ "bootloader_setImagesToBootload", "group__StorageInterface.html#ga8e20baecb19424efaa667248b851d991", null ],
    [ "bootloader_getImagesToBootload", "group__StorageInterface.html#gafb9569405337cf6a8bec987bf96705c7", null ],
    [ "bootloader_appendImageToBootloadList", "group__StorageInterface.html#ga833f0a3ed32c054d48e564d2fd5082aa", null ],
    [ "bootloader_setImageToBootload", "group__StorageInterface.html#gaf4c61349c24dca22a52ba3de71ffe7ee", null ],
    [ "bootloader_initVerifyImage", "group__StorageInterface.html#gad375581d8a53e23db490a4bc0e9a9fb2", null ],
    [ "bootloader_continueVerifyImage", "group__StorageInterface.html#ga83dbc153b4d054dfe8ccfec6a0c7adbc", null ],
    [ "bootloader_verifyImage", "group__StorageInterface.html#gaa292326b7370d02f99702ce3b17b9cfc", null ],
    [ "bootloader_getImageInfo", "group__StorageInterface.html#ga67eb1e49eeb811780051441399b0cac0", null ],
    [ "bootloader_storageIsBusy", "group__StorageInterface.html#ga60cb9344aa4453edf0bc73f18029d389", null ],
    [ "bootloader_readRawStorage", "group__StorageInterface.html#gabbd1d9bfd1f9a87cc1599ce0354cb405", null ],
    [ "bootloader_writeRawStorage", "group__StorageInterface.html#gae70235e890e250a27b93c8ec00bc982e", null ],
    [ "bootloader_eraseRawStorage", "group__StorageInterface.html#gab2f888bce5340da23d5febfb94f65017", null ]
];