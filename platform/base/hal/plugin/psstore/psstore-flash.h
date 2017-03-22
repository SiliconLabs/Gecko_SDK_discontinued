#ifndef FLASH_H
#define FLASH_H
int flash_init();
int flash_erase_page(void *address);
int flash_write_u32(uint32_t *address,uint32_t val);
int flash_write_buf(uint32_t *address,const uint32_t *d,uint32_t len);
#endif
