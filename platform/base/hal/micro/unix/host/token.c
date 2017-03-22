// Copyright 2016 Silicon Laboratories, Inc.

// For ftruncate(2) in glibc.
#define _XOPEN_SOURCE 500

#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#include EMBER_AF_API_HAL

#include <sys/mman.h>
#include <sys/stat.h>
#include <err.h>
#include <fcntl.h>
#include <sysexits.h>
#include <unistd.h>

#ifdef EMBER_AF_API_TOKEN

// TODO: This implementation includes token-stack.h for access to token
// definitions.  In a host/NCP configuration, the tokens defined directly in
// token-stack.h are used by the NCP, not the host.  token-stack.h should be
// set up to exclude stack tokens when compiling a host application.  The
// inclusion of NCP tokens on the host is harmless, although it does waste disk
// space and is potentially confusing.

// TODO: This implementation treats manufacturing tokens as read-only
// application tokens.  Manufacturing tokens should be initialized to FF and be
// writable at runtime only if the value is FF.

// TODO: This implementation does not handle upgrading older token files.  This
// is not currently an issue because there is only one version of the token
// file.  Future versions, however, should recognize old token files and
// migrate them to the new format.  Instead, this condition is treated as an
// incompatible token file, which results in all values getting reset to the
// defaults.

// TODO: This implementation does not handle reordering or token definitions in
// token-stack.h or APPLICATION_TOKEN_HEADER.  Instead of relying on fixed
// indexes and offsets, the code could search the entire token file for the
// appropriate entry for a given token.  Instead, this condition is treated as
// an incompatible token file, which results in all values getting reset to the
// defaults.

// TODO: This implementation does not handle resizing of tokens.  The code
// could move data around within the token file to accomodate resized tokens.
// This would be fairly easy to implement for tokens that grow.  For tokens
// that shrink, some care is required to avoid truncating the file before the
// data has been rearranged.  Instead, this condition is treated as an
// incompatible token file, which results in all values getting reset to the
// defaults.

// Version 1 format:
//   version:1 creator[0]:2 isCnt[0]:1 size[0]:1 arraySize[0]:1 data[0]:m_0 ...
//   creator[n]:2 isCnt[n]:1 size[n]:1 arraySize[n]:1 data[n]:m_n
#define VERSION 1

extern const uint16_t tokenCreators[];
extern const bool tokenIsCnt[];
extern const uint8_t tokenSize[];
extern const uint8_t tokenArraySize[];

// TODO: Don't include stack tokens on the host.
#define DEFINETOKENS
#define TOKEN_MFG TOKEN_DEF
#define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...) \
  TOKEN_##name##_ADDRESS,
  static const uint16_t addresses[] = {
    #include "stack/config/token-stack.h"
  };
#undef TOKEN_DEF
#undef TOKEN_MFG
#undef DEFINETOKENS

static void initializeTokenSystem(void);
static void resetTokenData(void);
static size_t getNvmOffset(uint16_t token, uint8_t index, uint8_t len);

#ifndef EMBER_AF_TOKEN_FILENAME
  #if defined(EMBER_AF_DEVICE_NAME)
    #define EMBER_AF_TOKEN_FILENAME EMBER_AF_DEVICE_NAME ".nvm"
  #elif defined(ZA_PROMPT)
    #define EMBER_AF_TOKEN_FILENAME ZA_PROMPT ".nvm"
  #else
    #error EMBER_AF_TOKEN_FILENAME must be defined.
  #endif
#endif

// mmap(2) returns MAP_FAILED on failure, which is not necessarily defined the
// same as NULL.
static uint8_t *nvm = MAP_FAILED;
#define isInitialized() (nvm != MAP_FAILED)

#define PER_TOKEN_OVERHEAD      \
  (sizeof(tokenCreators[0])     \
   + sizeof(tokenIsCnt[0])      \
   + sizeof(tokenSize[0])       \
   + sizeof(tokenArraySize[0]))
#define TOTAL_SIZE                    \
  (1 /* version overhead */           \
   + TOKEN_COUNT * PER_TOKEN_OVERHEAD \
   + TOKEN_MAXIMUM_SIZE)

void halInternalGetTokenData(void *data, uint16_t token, uint8_t index, uint8_t len)
{
  ATOMIC(
    size_t offset = getNvmOffset(token, index, len);
    MEMCOPY(data, nvm + offset, len);
  )
}

void halInternalSetTokenData(uint16_t token, uint8_t index, void *data, uint8_t len)
{
  ATOMIC(
    size_t offset = getNvmOffset(token, index, len);
    MEMCOPY(nvm + offset, data, len);
    if (msync(nvm, TOKEN_MAXIMUM_SIZE, MS_SYNC) == -1) {
      err(EX_IOERR, "Could not write " EMBER_AF_TOKEN_FILENAME " to disk");
    }
  )
}

void halInternalSetMfgTokenData(uint16_t token, void *data, uint8_t len)
{
  // TODO: Handle manufacturing tokens.
  assert(false);
}

static void initializeTokenSystem(void)
{
  assert(!isInitialized());

  int fd = open(EMBER_AF_TOKEN_FILENAME,
                (O_RDWR | O_CREAT),      // open read/write, create if missing
                (S_IRUSR | S_IWUSR));    // read/write permissions for owner
  if (fd == -1) {
    err(EX_NOINPUT, "Could not open or create " EMBER_AF_TOKEN_FILENAME);
  }

  struct stat buf;
  if (fstat(fd, &buf) == -1) {
    err(EX_IOERR, "Could not determine size of " EMBER_AF_TOKEN_FILENAME);
  }

  // TODO: Handle resized tokens.
  bool reset = (buf.st_size != TOTAL_SIZE);
  if (reset && ftruncate(fd, TOTAL_SIZE) == -1) {
    err(EX_IOERR, "Could not set size of " EMBER_AF_TOKEN_FILENAME);
  }

  nvm = mmap(NULL,                     // let system choose address
             TOTAL_SIZE,
             (PROT_READ | PROT_WRITE), // data can be read/written
             MAP_SHARED,               // writes change the file
             fd,
             0);                       // no offset
  if (!isInitialized()) {
    err(EX_UNAVAILABLE, "Could not map " EMBER_AF_TOKEN_FILENAME " to memory");
  }

  uint8_t *finger = nvm;
  if (!reset) {
    // TODO: Handle older token files.
    reset = (*finger++ != VERSION);
  }
  if (!reset) {
    // TODO: Handle reordered tokens.
    // TODO: Handle resized tokens.
    for (size_t i = 0; i < TOKEN_COUNT; i++) {
      if (   *finger++ != HIGH_BYTE(tokenCreators[i])
          || *finger++ != LOW_BYTE(tokenCreators[i])
          || *finger++ != tokenIsCnt[i]
          || *finger++ != tokenSize[i]
          || *finger++ != tokenArraySize[i]) {
        reset = true;
        break;
      }
      finger += tokenArraySize[i] * tokenSize[i];
    }
  }

  if (reset) {
    resetTokenData();
  }
}

static void resetTokenData(void)
{
  assert(isInitialized());

  uint8_t *finger = nvm;
  *finger++ = VERSION;
  for (size_t i = 0; i < TOKEN_COUNT; i++) {
    *finger++ = HIGH_BYTE(tokenCreators[i]);
    *finger++ = LOW_BYTE(tokenCreators[i]);
    *finger++ = tokenIsCnt[i];
    *finger++ = tokenSize[i];
    *finger++ = tokenArraySize[i];
    finger += tokenArraySize[i] * tokenSize[i];
  }

  #define DEFINETOKENS
  #define TOKEN_MFG(name,creator,iscnt,isidx,type,arraysize,...)
  #define TOKEN_DEF(name,creator,iscnt,isidx,type,arraysize,...)          \
    {                                                                     \
      type data = __VA_ARGS__;                                            \
      if (arraysize == 1) {                                               \
        halInternalSetTokenData(TOKEN_##name, 0x7F, &data, sizeof(type)); \
      } else {                                                            \
        uint8_t i;                                                        \
        for (i = 0; i < arraysize; i++) {                                 \
          halInternalSetTokenData(TOKEN_##name, i, &data, sizeof(type));  \
        }                                                                 \
      }                                                                   \
    }
  #include "stack/config/token-stack.h"
  #undef TOKEN_DEF
  #undef TOKEN_MFG
  #undef DEFINETOKENS
}

static size_t getNvmOffset(uint16_t token, uint8_t index, uint8_t len)
{
  if (!isInitialized()) {
    initializeTokenSystem();
    assert(isInitialized());
  }

  size_t offset = 1 + addresses[token] + (token + 1) * PER_TOKEN_OVERHEAD;
  if (index != 0x7F) {
    offset += (index * len);
  }
  return offset;
}

#endif // EMBER_AF_API_TOKEN
