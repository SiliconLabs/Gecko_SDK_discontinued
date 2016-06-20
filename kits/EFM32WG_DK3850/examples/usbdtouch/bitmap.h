#ifndef _PICTURE_H_
#define _PICTURE_H_

#ifdef __cplusplus
extern "C" {
#endif


uint32_t BITMAP_CreateFileAndSaveData(const char *filename, const char *data, uint32_t len);
int BITMAP_Init( void );
int BITMAP_TakePicture(void);
bool BITMAP_USBHandler(void);

#ifdef __cplusplus
}
#endif

#endif
