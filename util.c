#include <util.h>
#include <limits.h>
#include <math.h>


bool is_power_of(u32 num, u32 power)
{
    if(num == 1)
        return true;

    u32 val = power;
    for(u32 i = 1; val <= num; i++)
    {
        val = pow(power, i);
        if(val == num)
            return true;
    }

    return false;
}


struct bitmap *bm_creat(u32 size)
{
    static struct bitmap btm;
    if(size == 0)
        return NULL;

    u32 elems = (size / CHAR_BIT) + 1;

    if((btm.bm = malloc(elems)) == NULL)
        return NULL;

    memset(btm.bm, 0, elems);
    btm.size = elems;
    return &btm;
}


int bm_set(struct bitmap *btm, u32 index)
{
    if(index > btm->size)
        return -1;
    
    u8 *byte = btm->bm + (index / CHAR_BIT);
    u8 bit = index % CHAR_BIT;
    
    *byte |= (1 << bit);
    return 0;
}


int bm_get(struct bitmap *btm, u32 index)
{
    if(index > btm->size)
        return -1;
    
    u8 byte = *(btm->bm + (index / CHAR_BIT));
    u8 bit = index % CHAR_BIT;

    return (byte >> bit) & 1;
}


void err_sys(const char *msg)
{
    fprintf(stderr, "%s : %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}