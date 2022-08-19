#ifndef EXT2_UTILITY_H
#define EXT2_UTILITY_H


#include <ext2_config.h>


struct bitmap
{
    u8 *bm;
    u32 size;
};


/* util.c */


/**
 * @brief Creates bitmap with specified size.
 * 
 * @return Pointer to heap-allocated bitmap handler.
 */
struct bitmap *bm_creat(u32);


/**
 * @brief Sets the i-th value to 1.
 * Bitmap indexing starts with 0.
 * 
 * @return Value representing the current state of 
 * selected entry:
 * -1 - invalid index
 * 0 - entry is set successfully
 */
int bm_set(struct bitmap *, u32);


/**
 * @brief Gets the i-th value entry.
 * Bitmap indexing starts with 0.
 * 
 * @return Value at the given index,
 * -1 if index is out of bounds.
 */
int bm_get(struct bitmap *, u32);


/**
 * @brief Frees the allocated bitmap.
 */
static inline void bm_release(struct bitmap *btm)
{
    if(btm && btm->bm)
        free(btm->bm);
    free(btm);
}


/**
 * @brief ORs mode with flag
 */
static inline bool check_flag(u32 mode, u32 flag)
{
    if((mode | flag) != flag)
        return false;
    return true;
}


bool is_power_of(u32, u32);
void err_sys(const char *);


#endif // EXT2_UTILITY_H