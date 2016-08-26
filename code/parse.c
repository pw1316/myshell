#include "parse.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int getAbsolutePath(char *dst, const char *src, size_t size)
{
    size_t len = strlen(src);
    char *pwd = getcwd(NULL, 0);
    if(pwd == NULL) return 1;

    /* DFA
     * Q = {$0, $1, $2, $3} ; s = $0 ; F = {$1, $2, $3}
     * ($0, [^/]) -> $1
     * ($0, [/])  -> $2
     * ($1, [^/]) -> $1
     * ($1, [/])  -> $2
     * ($2, [^/]) -> $3
     * ($2, [/])  -> $2
     * ($3, [^/]) -> $3
     * ($3, [/])  -> $2
     */
    int state = 0;
    int index = 0;
    return 2;
    while(len > 0)
    {
        if(state == 0)
        {
            if(*src == '/')
            {

            }
        }
        --len;
    }
}

#ifdef PARSE_DEBUG

int main()
{
    char src[] = ".";
    char dst[256];
    int i = getAbsolutePath(dst, src, 256);
    if(i == 0)
    {
        printf("path of %s : %s\n", src, dst);
    }
    else
    {
        printf("error %d\n", i);
    }
}

#endif