#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int getAbsolutePath(char *dst, const char *src, size_t size)
{
    size_t len = strlen(src);
    char *pwd = getcwd(NULL, 0);
    if(pwd == NULL) return 1;

    /* DFA
     * Q = {$0, $1, $2} ; s = $0 ; F = {$1, $2}
     * ($0, [^/]) -> $1
     * ($0, [/])  -> $2
     * ($1, [^/]) -> $1
     * ($1, [/])  -> $2
     * ($2, [^/]) -> $1
     * ($2, [/])  -> $2
     */
    int state = 0;
    char *tmp = (char *)malloc(sizeof(char) * (len + 1));
    if(tmp == NULL)
    {
        if(pwd) free(pwd);
        return 1;
    }
    int index = 0;
    /*error below is done at tag error*/
    while(len-- > 0)
    {
        int flag = 0;
        if(state == 0)
        {
            if(*src == '/')
            {
                chdir("/");
                state = 2;
                ++src;
                index = 0;
                continue;
            }
            tmp[index++] = *src++;
            state = 1;
        }
        else if(state == 1)
        {
            if(*src == '/')
            {
                tmp[index] = '\0';
                if(chdir(tmp) == -1) goto error;
                state = 2;
                ++src;
                index = 0;
                continue;
            }
            tmp[index++] = *src++;
            state = 1;
        }
        else if(state == 2)
        {
            if(*src == '/')
            {
                state = 2;
                ++src;
                index = 0;
                continue;
            }
            tmp[index++] = *src++;
            state = 1;
        }
    }
    /*DFA out*/
    if(state == 1)
    {
        tmp[index] = '\0';
        chdir(tmp);
    }
    else if(state != 2)
    {
        goto error;
    }
    /*get path*/
    if(getcwd(dst, size) == NULL) goto error;
    state = chdir(pwd);
    free(pwd);
    free(tmp);
    return state;

    error:
    state = chdir(pwd);
    free(pwd);
    free(tmp);
    return 1;
}

#ifdef PARSE_DEBUG

int main(int argc, char *argv[])
{
    char src[] = "./lalala";
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