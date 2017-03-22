var group__EblParser =
[
    [ "EBL Format", "group__EblParserFormat.html", "group__EblParserFormat" ],
    [ "EblBuffer_t", "structEblBuffer__t.html", [
      [ "buffer", "structEblBuffer__t.html#af9e9f36800efd76f7bedbd61ea321000", null ],
      [ "length", "structEblBuffer__t.html#a9c5d14ba3001c253327c3df7a376bc25", null ],
      [ "offset", "structEblBuffer__t.html#a7b77c6060c22ae263ab2e9d6b9508217", null ]
    ] ],
    [ "ParserContext_t", "structParserContext__t.html", [
      [ "internalState", "structParserContext__t.html#a7cbede58f15b409f755392f58dfa45cf", null ],
      [ "localBuffer", "structParserContext__t.html#ae2894d1260a68b61c737e827a4a20b86", null ],
      [ "aesContext", "structParserContext__t.html#ac762d4311c8eb741f02f1ef6e9d0dbe7", null ],
      [ "shaContext", "structParserContext__t.html#a07b4c3f549eb8d8830e9ad7b81b4465f", null ],
      [ "lengthOfTag", "structParserContext__t.html#a78bddcf2b38ed1e5fefecfbf1f79dbd6", null ],
      [ "offsetInTag", "structParserContext__t.html#a26b0a09c9c9eb84ea898a9876e76c078", null ],
      [ "lengthOfEncryptedTag", "structParserContext__t.html#aa182c87437a603ed4805ff19c60b468f", null ],
      [ "offsetInEncryptedTag", "structParserContext__t.html#aa9dc5322735f1e2beb6f863d4b55fcd3", null ],
      [ "programmingAddress", "structParserContext__t.html#a003d99da24dabe218be8bc456cf8915c", null ],
      [ "metadataAddress", "structParserContext__t.html#a617f8ec8cf520a0009f4758995f05cc0", null ],
      [ "bootloaderAddress", "structParserContext__t.html#adbdc2dc3f6d6220da54bf489f3b45027", null ],
      [ "withheldApplicationVectors", "structParserContext__t.html#a55450c300cd1dbc12b84080b4c000691", null ],
      [ "withheldUpgradeVectors", "structParserContext__t.html#ac3946d0c48559b9ef38a5a6d09445c12", null ],
      [ "withheldBootloaderVectors", "structParserContext__t.html#ac3fa96550fb18798d2b33480a0288204", null ],
      [ "fileCrc", "structParserContext__t.html#a0a1ad19b7d637d552def37a234e6dca8", null ],
      [ "encrypted", "structParserContext__t.html#a34664d6a7edad34a555ad7fe929cc42c", null ],
      [ "inEncryptedContainer", "structParserContext__t.html#a33bb143a04ceb5f60b41e373fc93059b", null ],
      [ "gotSignature", "structParserContext__t.html#ab17021c627e040c7600665cc15206e5b", null ],
      [ "isV2", "structParserContext__t.html#a89170cbd0e393622598815dd42ec06a5", null ]
    ] ],
    [ "EblParserState_t", "group__EblParser.html#ga7767157da0dffc9530af3da1f87b88a3", [
      [ "EblParserStateInit", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a6ae86128d96bcff1eb0017d2bea71c1b", null ],
      [ "EblParserStateIdle", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a94a7dd46ab93e4e70b8cf96fcf6b7fa3", null ],
      [ "EblParserStateHeader", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a5629e9a4519dcaae2fa38654ef6c8b24", null ],
      [ "EblParserStateHeaderV2", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a8b0a6f949317eda8ef29c9cd12ec039c", null ],
      [ "EblParserStateBootloader", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a947f880bf927004914280810d813d3f2", null ],
      [ "EblParserStateBootloaderData", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3afce00d21e18521729d8974de1c868e3a", null ],
      [ "EblParserStateApplication", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a923f1ea1d56cfbe30fa0b68f58b7fee5", null ],
      [ "EblParserStateMetadata", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a744e406c55a3f170ac83bea3ecc803a5", null ],
      [ "EblParserStateProg", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3ae0cc235fb4b57d95c5ce90276c94a3bb", null ],
      [ "EblParserStateProgData", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3ab6c71edfc4e557a9b1d62b4f1f179a24", null ],
      [ "EblParserStateEraseProg", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a7a77dfdf146c3ebd73d2975ce42f75c3", null ],
      [ "EblParserStateFinalize", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a816d08790eaf7c4f8b7747561a40bff4", null ],
      [ "EblParserStateDone", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3a0c0f2b2e1f5b87bad47adc5b69a2acba", null ],
      [ "EblParserStateEncryptionInit", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3ab8599d1ca1c8477e5a974cabb07f7835", null ],
      [ "EblParserStateEncryptionContainer", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3addbc4f88a9747e46d0a9fbe974d0f87f", null ],
      [ "EblParserStateSignature", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3ad058844f29e64ef44ce38ca43e1fdc17", null ],
      [ "EblParserStateError", "group__EblParser.html#gga7767157da0dffc9530af3da1f87b88a3aee0cd5896467124e1e3c072e0f720e97", null ]
    ] ]
];