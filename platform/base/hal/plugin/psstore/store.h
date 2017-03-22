
#ifndef STORE_H
#define STORE_H

#include <stdint.h>
#include "bg_message.h"

#define DB_FLAGS_NONE       0x00
#define DB_FLAGS_READ_ONLY  0x01

typedef struct
{
    uint32_t    crc;
    uint32_t    count;
    uint32_t    version;
    uint32_t    store[];
} db_store_t;

/**
 * Write key and value to store
 * @param tag key to use
 * @param flags to use, set to 0
 * @param len data length
 * @param val pointer to data
 * @return bg_err_success if succesful
 */
errorcode_t store_write(uint16_t tag, uint8_t flags, uint8_t len,const void *val);
/**
 * Read key and data from store
 * @param tag key to read
 * @param flags pointer where to store flags if not NULL
 * @param len return key length
 * @param value pointer to key_data in persistent store, pointer is only valid until any store_write is called
 * @return bg_err_success if succesful
 */
errorcode_t store_read(uint16_t tag, uint8_t *flags, uint8_t *len,void** value);

/**
 * Read a uint16_t value from store.
 * @param tag key to read
 * @param flags pointer where to store flags if not NULL
 * @param the pointer to return the read value
 * @return bg_err_success if succesful
 */
errorcode_t store_read_uint16(uint16_t tag, uint8_t *flags, uint16_t *value);

/**
 * Read key from beginning of store, is used to iterate through all keys
 * @param index key index to read from
 * @param tag return key tag
 * @param flags key flags
 * @param len key length
 * @param value pointer to key value
 * @return
 */
errorcode_t store_read_index(uint8_t index, uint16_t *tag, uint8_t *flags, uint8_t *len,uint32_t** value);
/**
 * Delete all keys where tag&mask == stored_key&mask
 * @param tag tag to delete
 * @param mask mask to check against set to 0xffff to match tag exactly, 0 matches all keys
 */
errorcode_t store_delete(uint16_t tag, uint16_t mask);

/**
 * Erase everything in ps-store
 */
void store_eraseall(void);
/**
 * Initialize persistent store
 */
void store_init();

/**
 * Defragment persistent store
 *
 * @return (used_internally) pointer to new page
 */
uint32_t* store_defrag(void);

#endif
