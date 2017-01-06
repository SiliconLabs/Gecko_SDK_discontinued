
#include <yfuns.h>
#include "..\Serial.h"

size_t __write(int handle, const unsigned char *buf, size_t size) {
  size_t nChars = 0;

  if (buf == 0) {
    /*
     * This means that we should flush internal buffers.  Since we
     * don't we just return.  (Remember, "handle" == -1 means that all
     * handles should be flushed.)
     */
    return 0;
  }

  /* This template only writes to "standard out" and "standard err",
   * for all other file handles it returns failure. */
  if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size != 0; --size) {
    SER_PutChar(*buf++);
    ++nChars;
  }

  return nChars;
}

size_t __read(int handle, unsigned char *buf, size_t size) {
  size_t nChars = 0;
  int    c;

  /* This template only reads from "standard in", for all other file
   * handles it returns failure. */
  if (handle != _LLIO_STDIN) {
    return _LLIO_ERROR;
  }

  for (/* Empty */; size > 0; --size) {
    c = SER_GetChar();
    if (c == '\r') break;
    *buf++ = c;
    ++nChars;
  }

  return nChars;
}
