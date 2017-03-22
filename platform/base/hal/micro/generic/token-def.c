/** @file hal/micro/generic/token-def.c
 * @brief This file is for the code definitions (arrays) needed for the tokens.
 *
 * Copyright 2007-2011 by Ember Corporation. All rights reserved.           *80*
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"


//-- Build structure defines (these define all the data types of the tokens).
#define DEFINETYPES
  #include "stack/config/token-stack.h"
#undef DEFINETYPES

//-- Build parameter links
#define DEFINETOKENS
#define TOKEN_MFG(name,creator,iscnt,isidx,type,arraysize,...)

#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  creator,
  const uint16_t tokenCreators[] = {
    #include "stack/config/token-stack.h"
  };
#undef TOKEN_DEF

#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  iscnt,
  const bool tokenIsCnt[] = {
    #include "stack/config/token-stack.h"
  };
#undef TOKEN_DEF

#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  sizeof(type),
  const uint8_t tokenSize[] = {
    #include "stack/config/token-stack.h"
  };
#undef TOKEN_DEF

#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  arraysize,
  const uint8_t tokenArraySize[] = {
    #include "stack/config/token-stack.h"
  };
#undef TOKEN_DEF

//These set of DEFAULT token values are only used in the tokenDefaults array
//below.

// -------------------------------------------------------------------------
// MISRAC2012-Rule-20.7 doesn't allow for this use of concatenation within
// a macro expansion. Moreover, since TOKEN_DEF is only being used here as a
// static declaration of token default values, there's no risk that it will
// interfere with any other logic.
//cstat -MISRAC2012-Rule-20.7
// -------------------------------------------------------------------------
#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  const type TOKEN_##name##_DEFAULTS = __VA_ARGS__;
  #include "stack/config/token-stack.h"
#undef TOKEN_DEF
// -------------------------------------------------------------------------
// Re-enable enclosing parentheses CSTAT rule
//cstat +MISRAC2012-Rule-20.7
// -------------------------------------------------------------------------

#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  ((void *)&TOKEN_##name##_DEFAULTS),
  const void * const tokenDefaults[] = {
    #include "stack/config/token-stack.h"
  };
#undef TOKEN_DEF

#undef DEFINETOKENS

#undef TOKEN_MFG

