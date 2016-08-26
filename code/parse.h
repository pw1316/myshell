#ifndef __PARSE_H__
#define __PARSE_H__

#include <stddef.h>

//#define PARSE_DEBUG

/*0 on success ; 1 on failure*/
int getAbsolutePath(char *dst, const char *src, size_t size);

#endif