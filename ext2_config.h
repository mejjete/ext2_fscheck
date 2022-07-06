#ifndef EXT2_CONFIG_H
#define EXT2_CONFIG_H


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kernel.h>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;


typedef u32 block_t;
typedef u32 bitmap_id;


#endif // EXT2_CONFIG_H