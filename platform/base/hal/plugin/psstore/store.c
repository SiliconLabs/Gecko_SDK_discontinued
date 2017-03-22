
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "psstore-flash.h"
#include "store.h"
#include PLATFORM_HEADER


#include "bg_message.h"
#include "bg_crc.h"

#include "em_cmu.h"


//lint -e740 unusual pointer cast

// base-dependent apps need to initialize the clock line for the GPCRC
static inline void crc_init()
{
    // Enable clock for gpcrc
    CMU_ClockEnable(cmuClock_GPCRC, true);
}

static inline uint32_t crc32_calc(const uint8_t* data, int size)
{ 
    return bg_crc_calc(0xFFFFFFFFL,size,data);
}

typedef struct
{
    uint32_t crc;
    uint16_t tag;
    uint16_t flags_and_value_len_in_bytes;
} db_header_t;

//Define a segment to ensure the linker doesn't place anything overlapping
//the PSStore memory and allows SimEE to be packed just below.
VAR_AT_SEGMENT(NO_STRIPPING uint8_t psStore[FLASH_PAGE_SIZE*2], __PSSTORE__);
#define DB1_START_ADDR  ((void*)(FLASH_BASE+FLASH_SIZE-FLASH_PAGE_SIZE*2))
#define DB2_START_ADDR  ((void*)(FLASH_BASE+FLASH_SIZE-FLASH_PAGE_SIZE))
#define DB_VERSION      1
#define LEN_IN_TOTAL_WORDS(X) (2+((X)+3)/4)

#define ON_SAME_PAGE(X,Y) ((((uint32_t)(X))&~(FLASH_PAGE_SIZE-1))==(((uint32_t)(Y))&~(FLASH_PAGE_SIZE-1)))

db_store_t *volatile db_store[] =
{
    DB1_START_ADDR,
    DB2_START_ADDR
};

signed char    current_db = -1;
typedef struct
{
    db_header_t header;
    uint32_t value[];
} db_object_t;

bg_message_sender bg_store_sender;

static int erase_db(db_store_t *db)
{
    uint32_t crc;
    flash_erase_page(db);
    flash_write_u32(&db->count, 0);
    flash_write_u32(&db->version, DB_VERSION);
    crc = crc32_calc((unsigned char*) &db->count, 8);

    flash_write_u32(&db->crc, crc);
    return 0;
}

static inline uint16_t get_tag(const db_header_t *h)
{
    return h->tag;
}

static inline void set_hdr(db_header_t *h, uint16_t tag, uint8_t len, uint8_t flags)
{
    h->tag = tag;
    h->flags_and_value_len_in_bytes = (((uint16_t) flags) << 8) | len;
}

char header_invalid(db_header_t *h)
{
    if ((((uint32_t) h) & (FLASH_PAGE_SIZE - 1)) + sizeof(db_header_t) >= (uint32_t) FLASH_PAGE_SIZE) //lint !e650
    { //check overflow
        return 1;
    }
    return h->tag == 0xffff;
}

void store_init()
{
    uint32_t crc;
    flash_init();
    crc_init();
    crc = crc32_calc((unsigned char*) &db_store[0]->count, 8);
    if (crc != db_store[0]->crc)
    {
        erase_db(db_store[0]);
    }

    crc = crc32_calc((unsigned char*) &db_store[1]->count, 8);
    if (crc != db_store[1]->crc)
    {
        erase_db(db_store[1]);
    }
    uint32_t count0=db_store[0]->count;
    if (count0 < db_store[1]->count)
    {
        current_db = 1;
    }
    else
    {
        current_db = 0;
    }
}

static db_object_t *find_obj(uint16_t tag)
{
    uint32_t crc;

    db_object_t *obj;
    db_store_t *db_src;
    uint32_t *addr_src;

    db_src = db_store[current_db];
    addr_src = db_src->store;

    while (1)
    {
        obj = (db_object_t*) addr_src; //lint !e740 !e826
        if (header_invalid(&obj->header))
            break;
        crc = crc32_calc((unsigned char*) &obj->header.tag, 4 + (obj->header.flags_and_value_len_in_bytes & 0xff));
        if ((crc == obj->header.crc) && (get_tag(&obj->header) == tag))
        {
            return obj;
        }
        addr_src += LEN_IN_TOTAL_WORDS(obj->header.flags_and_value_len_in_bytes & 0xff);
        if (!ON_SAME_PAGE(addr_src, db_src->store))
            break;
    }
    return NULL;
}

errorcode_t store_delete(uint16_t tag, uint16_t mask)
{
    db_header_t hdr;
    db_object_t *obj;
    db_store_t *db;
    uint32_t *addr;

    if (tag == 0xffff || tag == 0)
        return bg_err_bt_invalid_command_parameters;
    db = db_store[current_db];
    addr = db->store;

    while (1)
    {
        obj = (db_object_t*) addr; //lint !e740 !e826
        if (header_invalid(&obj->header))
            break;
        if ((get_tag(&obj->header)& mask) == (tag & mask))
        {
            hdr = obj->header;
            //clear tag to notify not in use
            //flip bits in tag to make 0, do not change any other bits
            hdr.tag ^= 0xffff;
            hdr.flags_and_value_len_in_bytes=0xffff;
            flash_write_u32((uint32_t*) &obj->header.tag, *(uint32_t*)&hdr.tag);
        }
        addr += LEN_IN_TOTAL_WORDS((obj->header.flags_and_value_len_in_bytes & 0xff));
        if (!ON_SAME_PAGE(addr, db->store))
            break;
    }
    return bg_err_success;
}

errorcode_t store_write(uint16_t tag, uint8_t flags, uint8_t len, const void *vval)
{
    uint32_t crc;
    const uint8_t *bval = vval;
    int n;
    db_header_t hdr;
    db_object_t *obj;
    db_store_t *db;
    uint32_t *addr;
    uint32_t *crc_addr;
    if (tag == 0xffff || tag == 0)
        return bg_err_bt_invalid_command_parameters;
    db = db_store[current_db];
    addr = db->store;

    uint32_t write_err = bg_err_success;
    while (write_err == bg_err_success)
    {
        obj = (db_object_t*) addr; //lint !e740 !e826
        if (header_invalid(&obj->header))
            break;
        crc = crc32_calc((unsigned char*) &obj->header.tag, 4 + (obj->header.flags_and_value_len_in_bytes & 0xff));
        if (crc == obj->header.crc &&
            get_tag(&obj->header) == tag)
        {
            if (len == (obj->header.flags_and_value_len_in_bytes & 0xff))
                if (memcmp(vval, obj->value, len) == 0)
                    return bg_err_success; //found key is same, do nothing

            hdr = obj->header;
            //clear tag to notify not in use
            hdr.tag ^= 0xffff;
            hdr.flags_and_value_len_in_bytes=0xffff;
            
            write_err = flash_write_u32((uint32_t*) &obj->header.tag, *(uint32_t*)&hdr.tag);
        }
        addr += LEN_IN_TOTAL_WORDS((obj->header.flags_and_value_len_in_bytes & 0xff));
        if (!ON_SAME_PAGE(addr, db->store))
            break;
    }
//TODO:check for enough space
    if (write_err == bg_err_success && len > 0)
    {
        //if((((uint32_t)addr)&(FLASH_PAGE_SIZE-1))+8+len>FLASH_PAGE_SIZE)
        if (!ON_SAME_PAGE(((uint32_t )addr) + sizeof(db_header_t) + len, db->store))
        {
            addr = store_defrag();
            //offset inside page
            uint32_t offset=((uint32_t) addr) & (FLASH_PAGE_SIZE - 1);
            //if offset is 0 (database is full), or data does not fit in page
            if (offset == 0 ||
                offset + 8 + len > FLASH_PAGE_SIZE)
                return bg_err_hardware_ps_store_full;
        }
        set_hdr(&hdr, tag, len, flags);
        crc_addr = addr++;
        write_err = flash_write_u32(addr++, *(uint32_t*) &hdr.tag);            //lint !e740 !e826
        for (n = 0; write_err == bg_err_success && n < (len + 3) / 4; n++)
        {
            uint32_t val;
            val = (*bval++);
            val |= (*bval++) << 8;
            val |= (*bval++) << 16;
            val |= ((uint32_t)*bval++) << 24;

            write_err = flash_write_u32(addr++, val);
        }
        crc = crc32_calc((unsigned char*) (crc_addr + 1), 4 + len);
        write_err = flash_write_u32(crc_addr, crc);
    }

    return (errorcode_t) write_err;
}

errorcode_t store_read_index(uint8_t _index, uint16_t *key, uint8_t *flags, uint8_t *len, uint32_t** value)
{
    uint32_t crc;
    db_object_t *obj;
    db_store_t *db_src;
    uint32_t *addr_src;

    db_src = db_store[current_db];
    addr_src = db_src->store;

    do
    {
        obj = (db_object_t*) addr_src;            //lint !e740 !e826
        if (header_invalid(&obj->header))
            return bg_err_hardware_ps_key_not_found;
        crc = crc32_calc((unsigned char*) &obj->header.tag, 4 + (obj->header.flags_and_value_len_in_bytes & 0xff));
        if (obj->header.crc != crc)
        {            //don't care when crc doesn't match
            _index++;
        }
        addr_src += LEN_IN_TOTAL_WORDS(obj->header.flags_and_value_len_in_bytes % 0xff);
        if (!ON_SAME_PAGE(addr_src, db_src->store))
            return bg_err_hardware_ps_key_not_found;
    } while (_index--);

    *key = get_tag(&obj->header);
    if(flags)*flags = obj->header.flags_and_value_len_in_bytes >> 8;
    *len = obj->header.flags_and_value_len_in_bytes & 0xff;
    *value = obj->value;
    return bg_err_success;

}

errorcode_t store_read(uint16_t tag, uint8_t *flags, uint8_t *len, void** value)
{
    db_object_t *obj;
    obj = find_obj(tag);
    if (!obj)
    {
        if(flags)*flags = 0;
        *len = 0;
        *value = NULL;
        return bg_err_hardware_ps_key_not_found;
    }
    if(flags)*flags = obj->header.flags_and_value_len_in_bytes >> 8;
    *len = obj->header.flags_and_value_len_in_bytes & 0xff;
    *value = obj->value;
    return bg_err_success;
}

errorcode_t store_read_uint16(uint16_t tag, uint8_t *flags, uint16_t *value)
{
    uint8_t len;
    uint8_t *pdata;
    if((store_read(tag,flags,&len,(void**)&pdata)!=bg_err_success) ||
            len == 0)
        return bg_err_hardware_ps_key_not_found;

    *value=pdata[0];
    if(len>1)//add upper bit if exists
    {
        *value |= (uint16_t)pdata[1]<<8;
    }

    return bg_err_success;
}

uint32_t* store_defrag(void)
{
    uint32_t crc;
    int n;
    db_object_t *obj;
    db_store_t *db_src;
    db_store_t *db_dst;
    uint32_t *addr_src;
    uint32_t *addr_dst;

    db_src = db_store[current_db];
    addr_src = db_src->store;
    db_dst = db_store[current_db ^ 1];
    addr_dst = db_dst->store;

    flash_erase_page(db_dst);

    while (1)
    {
        if (!ON_SAME_PAGE(addr_src, db_src->store))
            break;
        obj = (db_object_t*) addr_src;            //lint !e740 !e826
        if (header_invalid(&obj->header))
            break;
        crc = crc32_calc((unsigned char*) &obj->header.tag, 4 + (obj->header.flags_and_value_len_in_bytes & 0xff));
        if (crc != obj->header.crc)
        {
            addr_src += LEN_IN_TOTAL_WORDS(obj->header.flags_and_value_len_in_bytes & 0xff);
            continue;
        }
        for (n = 0; n < LEN_IN_TOTAL_WORDS(obj->header.flags_and_value_len_in_bytes & 0xff); n++)
        {
            flash_write_u32(addr_dst++, *addr_src++);
        }
    }
    flash_write_u32(&db_dst->count, db_src->count + 1);
    flash_write_u32(&db_dst->version, DB_VERSION);
    crc = crc32_calc((unsigned char*) &db_dst->count, 8);
    flash_write_u32(&db_dst->crc, crc);
    //
    current_db ^= 1;
    return addr_dst;
}

void store_eraseall(void)
{
    erase_db(db_store[0]);
    erase_db(db_store[1]);
    current_db = 0;
}

