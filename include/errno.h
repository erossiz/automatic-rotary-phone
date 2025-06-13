#ifndef ERRNO_H
#define ERRNO_H

#include <psp2/types.h>

#define ERR_MEM_BLOCK_ALLOC             (0x01)
#define ERR_MEM_BLOCK_BASE_ADDRESS_GET  (0x02)
#define ERR_MEM_BLOCK_BASE_ADDRESS_FIND (0x03)
#define ERR_MEM_BLOCK_FREE              (0x04)
#define ERR_MSPACE_CREATE               (0x05)
#define ERR_MSPACE_MALLOC               (0x06)
#define ERR_FILE_OPEN                   (0x07)
#define ERR_FILE_READ                   (0x08)
#define ERR_FILE_CLOSE                  (0x09)

#define ERR_NULL_POINTER                (0xff)

extern SceInt errno;

#endif

