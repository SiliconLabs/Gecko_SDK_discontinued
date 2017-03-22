var group__CommonInterface =
[
    [ "Reset Information", "group__ResetInterface.html", "group__ResetInterface" ],
    [ "BareBootTable_t", "structBareBootTable__t.html", [
      [ "stackTop", "structBareBootTable__t.html#acbe8d7a123f12c3bbc0d1aa681c7d034", null ],
      [ "resetVector", "structBareBootTable__t.html#a85ecf0e4b1d04592d385083a092fbf5e", null ],
      [ "reserved0", "structBareBootTable__t.html#ac2c6d899d06550ea358720a031e2a154", null ],
      [ "reserved1", "structBareBootTable__t.html#ad58b0b3191c0fb957648d967b47d9fa3", null ],
      [ "table", "structBareBootTable__t.html#a0d589c2879a4cf4cee6f76dcd078bf51", null ],
      [ "reserved2", "structBareBootTable__t.html#a2db22558941545c1f8585b010d4290a5", null ],
      [ "signature", "structBareBootTable__t.html#ac4faf0c9993b6f94600dd35965f99535", null ]
    ] ],
    [ "BootloaderInformation_t", "structBootloaderInformation__t.html", [
      [ "type", "structBootloaderInformation__t.html#a9e6653d8e7ae2516b077737c85708024", null ],
      [ "version", "structBootloaderInformation__t.html#a724b1e270117777a09dfd2181091faad", null ],
      [ "capabilities", "structBootloaderInformation__t.html#a36c2f75904a45bdcf2077d7c729230fc", null ]
    ] ],
    [ "BootloaderHeader_t", "structBootloaderHeader__t.html", [
      [ "type", "structBootloaderHeader__t.html#af3ec9de1768bd8c9a760bcb49fec24cb", null ],
      [ "layout", "structBootloaderHeader__t.html#ab9fa319bc78d576ed196dbcdf76a09b8", null ],
      [ "version", "structBootloaderHeader__t.html#a7ea2967014a4ccc4ef1f0f870574bd3a", null ]
    ] ],
    [ "FirstBootloaderTable_t", "structFirstBootloaderTable__t.html", [
      [ "header", "structFirstBootloaderTable__t.html#a7e285456b76019ff8f38e8e6526597fe", null ],
      [ "mainBootloader", "structFirstBootloaderTable__t.html#ae382ebb2a2eece6b2b5e34b7548fbebc", null ],
      [ "upgradeLocation", "structFirstBootloaderTable__t.html#aba568a3af0487ce0c8c8262e98c7d446", null ]
    ] ],
    [ "MainBootloaderTable_t", "structMainBootloaderTable__t.html", [
      [ "header", "structMainBootloaderTable__t.html#a343fe9a0e426484556e26634c2c01a0a", null ],
      [ "size", "structMainBootloaderTable__t.html#a743537601f19d5c4d913cb80d0d74c7f", null ],
      [ "startOfAppSpace", "structMainBootloaderTable__t.html#a7999ef70eb5dc2208ab070377f56b725", null ],
      [ "endOfAppSpace", "structMainBootloaderTable__t.html#ae1f61019694aea43b30681d55a723bd1", null ],
      [ "capabilities", "structMainBootloaderTable__t.html#a5f93bdb5b88b1494890f286d3715049b", null ],
      [ "init", "structMainBootloaderTable__t.html#af81dcff32c3ea48a5ad4b199e90cac0d", null ],
      [ "deinit", "structMainBootloaderTable__t.html#a332e2145a2d79d902cefacdb7992bb28", null ],
      [ "verifyApplication", "structMainBootloaderTable__t.html#a3a288ed03f59c8772e8ac50ff870a977", null ],
      [ "initParser", "structMainBootloaderTable__t.html#a3f8acf36b5b116840cbbd35d01a92645", null ],
      [ "parseBuffer", "structMainBootloaderTable__t.html#a64bd204ef6d0fe2c408791cec5174300", null ],
      [ "storage", "structMainBootloaderTable__t.html#ad528c2cd0416f2dc0dc17aa79ec8be78", null ]
    ] ],
    [ "BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_SIGNATURE", "group__CommonInterface.html#gafd9d598334a397c77f16839e10708ef6", null ],
    [ "BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_ENCRYPTION", "group__CommonInterface.html#ga326e4b629d5759fc4d866fa2b8569c05", null ],
    [ "BOOTLOADER_CAPABILITY_ENFORCE_SECURE_BOOT", "group__CommonInterface.html#ga994674cb52b865d7f4b263255e3f9b35", null ],
    [ "BOOTLOADER_CAPABILITY_BOOTLOADER_UPGRADE", "group__CommonInterface.html#gac838a652229ad038001d92ae7dee945d", null ],
    [ "BOOTLOADER_CAPABILITY_EBL", "group__CommonInterface.html#ga3c1b8ac013a6b9e58d14b9b4b45007da", null ],
    [ "BOOTLOADER_CAPABILITY_EBL_SIGNATURE", "group__CommonInterface.html#ga15bd96d662269c23b2081ffb6b260d17", null ],
    [ "BOOTLOADER_CAPABILITY_EBL_ENCRYPTION", "group__CommonInterface.html#ga7ca8f187eba8ecc7eacfa9c088dc2084", null ],
    [ "BOOTLOADER_CAPABILITY_STORAGE", "group__CommonInterface.html#ga23fd85788bda58ea6e4f28eab4b74539", null ],
    [ "BOOTLOADER_CAPABILITY_COMMUNICATION", "group__CommonInterface.html#ga8399dd23ea8f5e3232359386c37f4c63", null ],
    [ "BOOTLOADER_MAGIC_FIRST_STAGE", "group__CommonInterface.html#ga264cbf37f524c70a4e6824cbf0095ccb", null ],
    [ "BOOTLOADER_MAGIC_MAIN", "group__CommonInterface.html#ga8c1a1c805a33a61501fb14b5f8f2c2e3", null ],
    [ "firstBootloaderTable", "group__CommonInterface.html#gad388d4d1c5656d47cd70dde2d3bce6f2", null ],
    [ "mainBootloaderTable", "group__CommonInterface.html#ga7729c13bf7a5f98b29280a836b3d1a84", null ],
    [ "BootloaderType_t", "group__CommonInterface.html#ga8c6b9a36c4c310e50b0b3511cccbe078", [
      [ "NONE", "group__CommonInterface.html#gga8c6b9a36c4c310e50b0b3511cccbe078ac157bdf0b85a40d2619cbc8bc1ae5fe2", null ],
      [ "SL_BOOTLOADER", "group__CommonInterface.html#gga8c6b9a36c4c310e50b0b3511cccbe078ad8935a7f43596d88d93a7a5a15ed6a20", null ]
    ] ],
    [ "bootloader_getInfo", "group__CommonInterface.html#ga36c02adad76bd72c8564cae1418cdbad", null ],
    [ "bootloader_init", "group__CommonInterface.html#ga3c66502510a5302eb25149df070d898b", null ],
    [ "bootloader_deinit", "group__CommonInterface.html#ga645ab019546fe7d05e4f9599919a5363", null ],
    [ "bootloader_rebootAndInstall", "group__CommonInterface.html#ga100dba06fa6561f27e924e6eb56cfa47", null ],
    [ "bootloader_verifyApplication", "group__CommonInterface.html#ga11ebd318571b961b6266ebd8beffa767", null ]
];