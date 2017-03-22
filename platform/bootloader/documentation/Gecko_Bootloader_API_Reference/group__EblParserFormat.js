var group__EblParserFormat =
[
    [ "EblTagHeader_t", "structEblTagHeader__t.html", [
      [ "tagId", "structEblTagHeader__t.html#acf43c82adff701734543f927bc56f2d5", null ],
      [ "length", "structEblTagHeader__t.html#aa3cabe7daaca7f97253ebae7cb33b214", null ]
    ] ],
    [ "EblHeader_t", "structEblHeader__t.html", [
      [ "header", "structEblHeader__t.html#af233133bf321c93097533aaa2a58be55", null ],
      [ "version", "structEblHeader__t.html#abfd94b9fae28516677c7134ebe20a2f3", null ],
      [ "type", "structEblHeader__t.html#af6b6f7fab9f416d8c62a00f4a436afb7", null ]
    ] ],
    [ "EblApplication_t", "structEblApplication__t.html", [
      [ "header", "structEblApplication__t.html#a99bb2bdd55121805c546de6422b4e740", null ],
      [ "appInfo", "structEblApplication__t.html#a09e45c5950868807bba0c28b09ecd7c5", null ]
    ] ],
    [ "EblBootloader_t", "structEblBootloader__t.html", [
      [ "header", "structEblBootloader__t.html#ad30a4f0b8d4e18d48d9a94637d3caaf2", null ],
      [ "bootloaderVersion", "structEblBootloader__t.html#aaeae90bcb63f0f2e16423ceb6edaf2fc", null ],
      [ "address", "structEblBootloader__t.html#aefb20210e675d7d6e3020bacbd353ce6", null ],
      [ "data", "structEblBootloader__t.html#a71a761efea555e6f06de411fcb8c389a", null ]
    ] ],
    [ "EblMetadata_t", "structEblMetadata__t.html", [
      [ "header", "structEblMetadata__t.html#aefd78f5b69fa534babb022a9449350a9", null ],
      [ "metaData", "structEblMetadata__t.html#a27f61c9773926d96fb5d73a17d33bb0b", null ]
    ] ],
    [ "EblProg_t", "structEblProg__t.html", [
      [ "header", "structEblProg__t.html#ac1b05431241dc925e941f216fa6820e2", null ],
      [ "flashStartAddress", "structEblProg__t.html#a7312dd5414f39ee67a33f374ee827440", null ],
      [ "data", "structEblProg__t.html#ac746a54816fac24dcfe17ccd21061e1a", null ]
    ] ],
    [ "EblEnd_t", "structEblEnd__t.html", [
      [ "header", "structEblEnd__t.html#af6ce4bba1345d2e50456ea516549b123", null ],
      [ "eblCrc", "structEblEnd__t.html#a052a7d9e438c35e6572c66c749cc8f11", null ]
    ] ],
    [ "EblEncryptionHeader_t", "structEblEncryptionHeader__t.html", [
      [ "header", "structEblEncryptionHeader__t.html#a8e6e5dcbb8b28888ef058cb210990ffc", null ],
      [ "version", "structEblEncryptionHeader__t.html#a2a8236c3f475ae6eb9076af9b67ff4ba", null ],
      [ "magicWord", "structEblEncryptionHeader__t.html#a264b7efe26c608fb0ca9febf7b334011", null ],
      [ "encryptionType", "structEblEncryptionHeader__t.html#a92e97126095b9793b14e8985828e6afa", null ]
    ] ],
    [ "EblEncryptionInitAesCcm_t", "structEblEncryptionInitAesCcm__t.html", [
      [ "header", "structEblEncryptionInitAesCcm__t.html#a10b419a320d8dfc79d69dc230520361c", null ],
      [ "msgLen", "structEblEncryptionInitAesCcm__t.html#a1bf7a2c1e0e0963bb9beabf1d225d86f", null ],
      [ "nonce", "structEblEncryptionInitAesCcm__t.html#af467741f1e5fc1a25354bc231e1a1fd5", null ]
    ] ],
    [ "EblEncryptionData_t", "structEblEncryptionData__t.html", [
      [ "header", "structEblEncryptionData__t.html#aae49cd2b847b9d75cbae498cd756cbab", null ],
      [ "encryptedEblData", "structEblEncryptionData__t.html#a5fae98495d4e6eddd0f17ba0b9a86ff1", null ]
    ] ],
    [ "EblEncryptionAesCcmSignature_t", "structEblEncryptionAesCcmSignature__t.html", [
      [ "header", "structEblEncryptionAesCcmSignature__t.html#aaf5e9a24143bcbb9f82ecb095e39ce4c", null ],
      [ "eblMac", "structEblEncryptionAesCcmSignature__t.html#a21b04543fd1ff2bfe9a94b5d5c6a660a", null ]
    ] ],
    [ "EblSignatureEcdsaP256_t", "structEblSignatureEcdsaP256__t.html", [
      [ "header", "structEblSignatureEcdsaP256__t.html#a8cbd1209b3e9549ebf50dd6aaf8c185a", null ],
      [ "r", "structEblSignatureEcdsaP256__t.html#add63ddeca52601d5958a1547460a8934", null ],
      [ "s", "structEblSignatureEcdsaP256__t.html#a27127726900609ad52143ed75627b985", null ]
    ] ],
    [ "EBL_IMAGE_MAGIC_WORD", "group__EblParserFormat.html#ga3d8fbe471970fba2d1ca38b77731dd49", null ],
    [ "EBL_COMPATIBILITY_MAJOR_VERSION", "group__EblParserFormat.html#gaf968f8d6a0564a7afd428bd3477a69ac", null ],
    [ "EBLV2_COMPATIBILITY_MAJOR_VERSION", "group__EblParserFormat.html#ga5da5de5822dac7c0872aec50f1f2a484", null ],
    [ "EBL_TAG_ID_HEADER_V3", "group__EblParserFormat.html#ga0f0dcb14477d875ba6d9b482aa1533cb", null ],
    [ "EBL_TAG_ID_BOOTLOADER", "group__EblParserFormat.html#ga0c2a6a5133a1d4e8619588f7de7ea113", null ],
    [ "EBL_TAG_ID_APPLICATION", "group__EblParserFormat.html#ga29e2fa92ffa6d1c4f6500cc3dce11eae", null ],
    [ "EBL_TAG_ID_METADATA", "group__EblParserFormat.html#gac61fa5d9a79733865a6d64cf55f95bbd", null ],
    [ "EBL_TAG_ID_PROG", "group__EblParserFormat.html#ga8dddb4f7be61eff95a3b45d9c5333147", null ],
    [ "EBL_TAG_ID_ERASEPROG", "group__EblParserFormat.html#ga126abf0c2a521b0d29e8d7561ce6c6ff", null ],
    [ "EBL_TAG_ID_END", "group__EblParserFormat.html#ga955a373b38135b45b796f1199568d0d9", null ],
    [ "EBL_TAG_ID_ENC_HEADER", "group__EblParserFormat.html#ga504d80f2c3cf47369769edc073c9f80a", null ],
    [ "EBL_TAG_ID_ENC_INIT", "group__EblParserFormat.html#ga9fe6ad99e196189ad07d99d6c905161a", null ],
    [ "EBL_TAG_ID_ENC_EBL_DATA", "group__EblParserFormat.html#ga8bf038cbbaf69054bd1784a27f619ee3", null ],
    [ "EBL_TAG_ID_ENC_MAC", "group__EblParserFormat.html#gade7d88b45a1a4d5a28dd6154b64f323a", null ],
    [ "EBL_TAG_ID_SIGNATURE_ECDSA_P256", "group__EblParserFormat.html#gae8bfb6989b0c9d5da8bfcbe60a089443", null ],
    [ "EBL_TYPE_NONE", "group__EblParserFormat.html#ga3dc7fbbe953bbec6993f0da01df96361", null ],
    [ "EBL_TYPE_ENCRYPTION_AESCCM", "group__EblParserFormat.html#gaca1359558b18137e8f258fe6c0c9530d", null ],
    [ "EBL_TYPE_SIGNATURE_ECDSA", "group__EblParserFormat.html#ga93442ceb65d268c4b9ef122368db3136", null ]
];