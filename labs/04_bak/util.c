#include <stddef.h>
#include "util.h"

void memset(unsigned int *beg, unsigned int val, unsigned int len)
{
	while(len--){
		*beg = val;
		beg++;
	}
}

int strcmp(const char *s1, const char *s2)
{
    while(*s1 || *s2)
    {
        if(*s1 == *s2){
            s1++; s2++;
        }else if(*s1 != *s2){
            return *s2-*s1;
        }
    }
    
    //match
    return 0;
}

int strlen(const char *s)
{
    int i=0;
    while(s[i]){
        i++;
    }
    
    return i-1;
}

void *memcpy(void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;
    while (len--) *d++ = *s++;
    return dest;
}
