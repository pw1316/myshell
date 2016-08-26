/* MyShell main file
 * This is the main processing function of MyShell.
 * Last Update: 2016.8.25 by Joker Yough
 */
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "parse.h"

/* The maximum length command */
#define MAX_CMD_LEN 80
#define MAX_PATH_LEN 256

/*environment*/
char pwd[MAX_PATH_LEN] = "";
char shell[MAX_PATH_LEN] = "";
const char PS1[] = "myshell:%s$ ";
const char PS2[] = "> ";

int main(int argc, char *argv[])
{
    /* command line arguments */
    char *cmdarg[MAX_CMD_LEN / 2 + 1];
    /*readme path*/
    char readme[MAX_CMD_LEN];
    char *cp;
    int i,j,pid,fd,fd2,bg;
    /*rf:reading commands*/
    /*pid: process ID*/
    /*fd: open file*/
    /*bg: background flag(external commands)*/
    DIR* dp;/*dir*/
    struct dirent* subdp;/*dir member*/
    struct stat statbuf;/*stat*/
    
    if(getcwd(pwd, MAX_PATH_LEN) == NULL)
    {
        printf("PWD too long!\n");
        return 1;
    }
    if(getAbsolutePath(shell, argv[0], MAX_PATH_LEN) == 1)
    {
        printf("SHELL path too long!\n");
        return 1;
    }

    int child = 0;
    int PS1orPS2 = 1;
    int bytesRead = 0;
    char command[MAX_CMD_LEN] = "";
    while(1)
    {
        if(!child)
        {
            if(PS1orPS2)
            {
                printf(PS1,pwd);
                fflush(stdout);
                bytesRead = 0;
                memset(command, 0, MAX_CMD_LEN);
                
                int flag = read(0, command, MAX_CMD_LEN);
                if(flag <= 1) continue;
                command[--flag] = '\0';
                if(command[flag - 1] == '\\')
                {
                    bytesRead += flag - 1;
                    PS1orPS2 = 0;
                    continue;
                }
            }
            else
            {
                printf(PS2);
                fflush(stdout);

                int flag = read(0, command + bytesRead, MAX_CMD_LEN - bytesRead);
                if(flag <= 1)
                {
                    command[bytesRead] = '\0';
                    PS1orPS2 = 1;
                }
                else
                {
                    command[bytesRead + (--flag)] = '\0';
                    if(command[bytesRead + flag - 1] == '\\')
                    {
                        bytesRead += flag - 1;
                        continue;
                    }
                    PS1orPS2 = 1;
                }
            }
        }
        /*get arguments*/
        cmdarg[0]=strtok(command," ");
        for(i=1;cmdarg[i]=strtok(NULL," ");i++);
        /*get arguments*/

        if(strcmp(cmdarg[0],"cd")==0){
            if(!cmdarg[1]){
                printf("cur dir: %s\n",pwd);
            }
            /*no argument and no &*/
            else{
                if(strcmp(cmdarg[1],"&")==0){
                    pid=fork();
                    if(pid<0){
                        printf("fork error!\n");
                    }
                    /*error*/
                    else if(pid==0){
                        printf("cur dir: %s\n",pwd);
                        return 0;
                    }
                    /*child*/
                    else{
                        printf("[%d]\n",pid);
                    }
                    /*parent*/
                }
                /*no argument but has &*/
                else if((dp=opendir(cmdarg[1]))==NULL) printf("No such directory!\n");
                /*no dir exists*/
                else{
                    if(cmdarg[2]&&strcmp(cmdarg[2],"&")==0){
                        pid=fork();
                        if(pid<0){
                            printf("fork error!\n");
                        }
                        /*error*/
                        else if(pid==0){
                            chdir(cmdarg[1]);
                            getcwd(pwd,256);
                            closedir(dp);
                            return 0;
                        }
                        /*child*/
                        else{
                            closedir(dp);
                            printf("[%d]\n",pid);
                        }
                        /*parent need to close the dir*/
                    }
                    /*background*/
                    else{
                        chdir(cmdarg[1]);
                        getcwd(pwd,256);
                        closedir(dp);
                    }
                    /*foreground*/
                }
            }
            if(child) break;
        }
        /* cd,cd &,cd <dir>,cd <dir> & */

        else if(strcmp(cmdarg[0],"dir")==0){
            if(!cmdarg[1]){
                dp=opendir(pwd);
                while((subdp=readdir(dp))!=NULL){
                    lstat(subdp->d_name,&statbuf);
                    if (S_ISREG(statbuf.st_mode))
                        printf(" REG   %s\n",subdp->d_name);
                    else if (S_ISDIR(statbuf.st_mode))
                        printf(" DIR   %s\n",subdp->d_name);
                    else if (S_ISCHR(statbuf.st_mode))
                        printf(" CHR   %s\n",subdp->d_name);
                    else if (S_ISBLK(statbuf.st_mode))
                        printf(" BLK   %s\n",subdp->d_name);
                    else if (S_ISFIFO(statbuf.st_mode))
                        printf(" FIFO  %s\n",subdp->d_name);
                    else if (S_ISLNK(statbuf.st_mode))
                        printf(" SLINK %s\n",subdp->d_name);
                    else if (S_ISSOCK(statbuf.st_mode))
                        printf(" SOCK  %s\n",subdp->d_name);
                    else
                        printf(" ????  %s\n",subdp->d_name);
                }
                closedir(dp);
            }
            /*dir*/
            else if(strcmp(cmdarg[1],"&")==0){
                pid=fork();
                if(pid<0){
                    printf("fork error!\n");
                }
                /*error*/
                else if(pid==0){
                    dp=opendir(pwd);
                    while((subdp=readdir(dp))!=NULL){
                        lstat(subdp->d_name,&statbuf);
                        if (S_ISREG(statbuf.st_mode))
                            printf(" REG   %s\n",subdp->d_name);
                        else if (S_ISDIR(statbuf.st_mode))
                            printf(" DIR   %s\n",subdp->d_name);
                        else if (S_ISCHR(statbuf.st_mode))
                            printf(" CHR   %s\n",subdp->d_name);
                        else if (S_ISBLK(statbuf.st_mode))
                            printf(" BLK   %s\n",subdp->d_name);
                        else if (S_ISFIFO(statbuf.st_mode))
                            printf(" FIFO  %s\n",subdp->d_name);
                        else if (S_ISLNK(statbuf.st_mode))
                            printf(" SLINK %s\n",subdp->d_name);
                        else if (S_ISSOCK(statbuf.st_mode))
                            printf(" SOCK  %s\n",subdp->d_name);
                        else
                            printf(" ????  %s\n",subdp->d_name);
                    }
                    closedir(dp);
                    return 0;
                }
                /*child*/
                else{
                    printf("[%d]\n",pid);
                }
                /*parent*/
            }
            /*dir &*/
            else if(strcmp(cmdarg[1],">")==0||strcmp(cmdarg[1],">>")==0){
                if(!cmdarg[2]||strcmp(cmdarg[2],"&")==0){printf("wrong output file!");}
                else if(!cmdarg[3]||strcmp(cmdarg[3],"&")!=0){
                    if(strcmp(cmdarg[1],">")==0) fd=open(cmdarg[2],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                    else fd=open(cmdarg[2],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                    dp=opendir(pwd);
                    while((subdp=readdir(dp))!=NULL){
                        lstat(subdp->d_name,&statbuf);
                        if (S_ISREG(statbuf.st_mode)){write(fd," REG   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        else if (S_ISDIR(statbuf.st_mode)){write(fd," DIR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        else if (S_ISCHR(statbuf.st_mode)){write(fd," CHR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        else if (S_ISBLK(statbuf.st_mode)){write(fd," BLK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        else if (S_ISFIFO(statbuf.st_mode)){write(fd," FIFO  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        else if (S_ISLNK(statbuf.st_mode)){write(fd," LNK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        else if (S_ISSOCK(statbuf.st_mode)){write(fd," SOCK  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        else{write(fd," ????  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                    }
                    closedir(dp);
                    close(fd);
                }
                /*dir > file*/
                else{
                    pid=fork();
                    if(pid<0){
                        printf("fork error!\n");
                    }
                    /*error*/
                    else if(pid==0){
                        if(strcmp(cmdarg[1],">")==0) fd=open(cmdarg[2],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                        else fd=open(cmdarg[2],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                        dp=opendir(pwd);
                        while((subdp=readdir(dp))!=NULL){
                            lstat(subdp->d_name,&statbuf);
                            if (S_ISREG(statbuf.st_mode)){write(fd," REG   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISDIR(statbuf.st_mode)){write(fd," DIR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISCHR(statbuf.st_mode)){write(fd," CHR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISBLK(statbuf.st_mode)){write(fd," BLK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISFIFO(statbuf.st_mode)){write(fd," FIFO  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISLNK(statbuf.st_mode)){write(fd," LNK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISSOCK(statbuf.st_mode)){write(fd," SOCK  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else{write(fd," ????  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        }
                        closedir(dp);
                        close(fd);
                        return 0;
                    }
                    /*child*/
                    else{
                        printf("[%d]\n",pid);
                        closedir(dp);
                    }
                    /*parent*/
                }
                /*dir > file &*/
            }
            /*dir > file [&] */
            else{
                if((dp=opendir(cmdarg[1]))==NULL) printf("No such directory!\n");
                else if(!cmdarg[2]){
                    chdir(cmdarg[1]);
                    while((subdp=readdir(dp))!=NULL){
                        lstat(subdp->d_name,&statbuf);
                        if (S_ISREG(statbuf.st_mode))
                            printf(" REG   %s\n",subdp->d_name);
                        else if (S_ISDIR(statbuf.st_mode))
                            printf(" DIR   %s\n",subdp->d_name);
                        else if (S_ISCHR(statbuf.st_mode))
                            printf(" CHR   %s\n",subdp->d_name);
                        else if (S_ISBLK(statbuf.st_mode))
                            printf(" BLK   %s\n",subdp->d_name);
                        else if (S_ISFIFO(statbuf.st_mode))
                            printf(" FIFO  %s\n",subdp->d_name);
                        else if (S_ISLNK(statbuf.st_mode))
                            printf(" SLINK %s\n",subdp->d_name);
                        else if (S_ISSOCK(statbuf.st_mode))
                            printf(" SOCK  %s\n",subdp->d_name);
                        else
                            printf(" ????  %s\n",subdp->d_name);
                    }
                    closedir(dp);
                    chdir(pwd);
                }
                /*dir <dir>*/
                else if(cmdarg[2]&&strcmp(cmdarg[2],"&")==0){
                    pid=fork();
                    if(pid<0){
                        printf("fork error!\n");
                    }
                    /*error*/
                    else if(pid==0){
                        chdir(cmdarg[1]);
                        while((subdp=readdir(dp))!=NULL){
                            lstat(subdp->d_name,&statbuf);
                            if (S_ISREG(statbuf.st_mode))
                                printf(" REG   %s\n",subdp->d_name);
                            else if (S_ISDIR(statbuf.st_mode))
                                printf(" DIR   %s\n",subdp->d_name);
                            else if (S_ISCHR(statbuf.st_mode))
                                printf(" CHR   %s\n",subdp->d_name);
                            else if (S_ISBLK(statbuf.st_mode))
                                printf(" BLK   %s\n",subdp->d_name);
                            else if (S_ISFIFO(statbuf.st_mode))
                                printf(" FIFO  %s\n",subdp->d_name);
                            else if (S_ISLNK(statbuf.st_mode))
                                printf(" SLINK %s\n",subdp->d_name);
                            else if (S_ISSOCK(statbuf.st_mode))
                                printf(" SOCK  %s\n",subdp->d_name);
                            else
                                printf(" ????  %s\n",subdp->d_name);
                        }
                        closedir(dp);
                        chdir(pwd);
                        return 0;
                    }
                    /*child*/
                    else{
                        printf("[%d]\n",pid);
                        closedir(dp);
                    }
                    /*parent*/
                }
                /*dir <dir> &*/
                else if(strcmp(cmdarg[2],">")==0||strcmp(cmdarg[2],">>")==0){
                    if(!cmdarg[3]||strcmp(cmdarg[3],"&")==0){printf("wrong output file!");}
                    else if(!cmdarg[4]||strcmp(cmdarg[4],"&")!=0){
                        if(strcmp(cmdarg[2],">")==0) fd=open(cmdarg[3],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                        else fd=open(cmdarg[3],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                        chdir(cmdarg[1]);
                        while((subdp=readdir(dp))!=NULL){
                            lstat(subdp->d_name,&statbuf);
                            if (S_ISREG(statbuf.st_mode)){write(fd," REG   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISDIR(statbuf.st_mode)){write(fd," DIR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISCHR(statbuf.st_mode)){write(fd," CHR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISBLK(statbuf.st_mode)){write(fd," BLK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISFIFO(statbuf.st_mode)){write(fd," FIFO  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISLNK(statbuf.st_mode)){write(fd," LNK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else if (S_ISSOCK(statbuf.st_mode)){write(fd," SOCK  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            else{write(fd," ????  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                        }
                        closedir(dp);
                        close(fd);
                        chdir(pwd);
                    }
                    /*dir <dir> > file*/
                    else{
                        pid=fork();
                        if(pid<0){
                            printf("fork error!\n");
                        }
                        /*error*/
                        else if(pid==0){
                            if(strcmp(cmdarg[2],">")==0) fd=open(cmdarg[3],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                            else fd=open(cmdarg[3],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                            chdir(cmdarg[1]);
                            while((subdp=readdir(dp))!=NULL){
                                lstat(subdp->d_name,&statbuf);
                                if (S_ISREG(statbuf.st_mode)){write(fd," REG   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                                else if (S_ISDIR(statbuf.st_mode)){write(fd," DIR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                                else if (S_ISCHR(statbuf.st_mode)){write(fd," CHR   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                                else if (S_ISBLK(statbuf.st_mode)){write(fd," BLK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                                else if (S_ISFIFO(statbuf.st_mode)){write(fd," FIFO  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                                else if (S_ISLNK(statbuf.st_mode)){write(fd," LNK   ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                                else if (S_ISSOCK(statbuf.st_mode)){write(fd," SOCK  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                                else{write(fd," ????  ",7);write(fd,subdp->d_name,strlen(subdp->d_name));write(fd,"\n",1);}
                            }
                            closedir(dp);
                            close(fd);
                            chdir(pwd);
                            return 0;
                        }
                        /*child*/
                        else{
                            printf("[%d]\n",pid);
                            closedir(dp);
                        }
                        /*parent*/
                    }
                    /*dir <dir> > file &*/
                }
                /*dir <dir> > file [&]*/
            }
            /*dir <dir> .....*/
            if(child) break;
        }
        /* dir,dir &,dir > file,dir > file &,dir <dir>,dir <dir> &,dir <dir> > file,dir <dir> > file & */

        else if(strcmp(cmdarg[0],"environ")==0){
            if(!cmdarg[1]){
                printf("pwd=%s\n",pwd);
                printf("shell=%smyshell\n",shell);
            }
            /*environ*/
            else if(strcmp(cmdarg[1],"&")==0){
                pid=fork();
                if(pid<0){
                    printf("fork error!\n");
                }
                /*error*/
                else if(pid==0){
                    printf("pwd=%s\n",pwd);
                    printf("shell=%smyshell\n",shell);
                    return 0;
                }
                /*child*/
                else{
                    printf("[%d]\n",pid);
                }
                /*parent*/
            }
            /* environ & */
            else if(strcmp(cmdarg[1],">")==0||strcmp(cmdarg[1],">>")==0){
                if(!cmdarg[2]||strcmp(cmdarg[2],"&")==0){printf("wrong output file!");}
                else if(!cmdarg[3]||strcmp(cmdarg[3],"&")!=0){
                    if(strcmp(cmdarg[1],">")==0) fd=open(cmdarg[2],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                    else fd=open(cmdarg[2],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                    write(fd,"pwd=",4);write(fd,pwd,strlen(pwd));write(fd,"\n",1);
                    write(fd,"shell=",6);write(fd,shell,strlen(shell));write(fd,"/myshell\n",9);
                    close(fd);
                }
                /*environ > file*/
                else{
                    pid=fork();
                    if(pid<0){
                        printf("fork error!\n");
                    }
                    /*error*/
                    else if(pid==0){
                        if(strcmp(cmdarg[1],">")==0) fd=open(cmdarg[2],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                        else fd=open(cmdarg[2],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                        write(fd,"pwd=",4);write(fd,pwd,strlen(pwd));write(fd,"\n",1);
                        write(fd,"shell=",6);write(fd,shell,strlen(shell));write(fd,"/myshell\n",9);
                        close(fd);
                        return 0;
                    }
                    /*child*/
                    else{
                        printf("[%d]\n",pid);
                    }
                    /*parent*/
                }
                /*environ > file &*/
            }
            /*environ > file [&] */
            if(child) break;
        }
        /*environ [&]*/

        else if(strcmp(cmdarg[0],"echo")==0){
            if(!cmdarg[1]){
                printf("\n");
            }
            else{
                i=1;
                while(cmdarg[i]) i++;
                if(strcmp(cmdarg[i-1],"&")==0){
                    i=1;
                    while(cmdarg[i]) {
                        if(strcmp(cmdarg[i],">")==0||strcmp(cmdarg[i],">>")==0) break; 
                        i++;
                    }
                    if(cmdarg[i]){
                        if(!cmdarg[i+1]||strcmp(cmdarg[i+1],"&")==0) printf("wrong filename!\n");
                        else{
                            pid=fork();
                            if(pid<0) printf("fork error!\n");
                            /*error*/
                            else if(pid==0){
                                if(strcmp(cmdarg[i],">")==0) fd=open(cmdarg[i+1],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                                else fd=open(cmdarg[i+1],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                                for(j=1;j<i;j++){
                                    write(fd,cmdarg[j],strlen(cmdarg[j]));write(fd," ",1);
                                }
                                write(fd,"\n",1);
                                close(fd);
                                return 0;
                            }
                            /*child*/
                            else{
                                printf("[%d]\n",pid);
                            }
                            /*parent*/
                        }
                    }
                    /*echo something > file & */
                    else{
                        pid=fork();
                        if(pid<0) printf("fork error!\n");
                        /*error*/
                        else if(pid==0){
                            for(j=1;j<i-1;j++){
                                printf("%s ",cmdarg[j]);
                            }
                            printf("\n");
                            return 0;
                        }
                        /*child*/
                        else{
                            printf("[%d]\n",pid);
                        }
                        /*parent*/
                    }
                    /*echo something &*/
                }
                /*echo [...] & */
                else{
                    i=1;
                    while(cmdarg[i]) {
                        if(strcmp(cmdarg[i],">")==0||strcmp(cmdarg[i],">>")==0) break; 
                        i++;
                    }
                    if(cmdarg[i]){
                        if(!cmdarg[i+1]||strcmp(cmdarg[i+1],"&")==0) printf("wrong filename!\n");
                        else{
                            if(strcmp(cmdarg[i],">")==0) fd=open(cmdarg[i+1],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                            else fd=open(cmdarg[i+1],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                            for(j=1;j<i;j++){
                                write(fd,cmdarg[j],strlen(cmdarg[j]));write(fd," ",1);
                            }
                            write(fd,"\n",1);
                            close(fd);
                        }
                    }
                    /*echo something > file */
                    else{
                        for(j=1;j<i;j++){
                            printf("%s ",cmdarg[j]);
                        }
                        printf("\n");
                    }
                    /*echo something*/
                }
                /*echo [...] */
            }
            if(child) break;
        }

        else if(strcmp(cmdarg[0],"quit")==0){
            break;
        }
        /*quit*/

        else if(strcmp(cmdarg[0],"myshell")==0){
            if(cmdarg[1]){
                fd=open(cmdarg[1],O_RDONLY);
                if(fd==-1){printf("open error!\n"); continue;}
                cp=command;
                while(read(fd,cp,1)!=0){
                    if(*cp=='\n'){
                        *cp='\0';
                        pid=fork();
                        if(pid<0) printf("fork error!\n");
                        else if(pid==0){
                            child=1;
                            break;
                        }
                        else wait(NULL);
                        cp=command;
                        *cp='\0';
                    }
                    else{
                        cp++;
                    }                    
                }
                if(command[0]){
                    *cp='\0';
                    pid=fork();
                    if(pid<0) printf("fork error!\n");
                    else if(pid==0){
                        child=1;
                        break;
                    }
                    else wait(NULL);
                }
                close(fd);
            }
        }
        /*read from file*/

        else if(strcmp(cmdarg[0],"clr")==0){
            printf("%c%c%c%c%c%c",27,'[','H',27,'[','J');
            if(child) break;
        }
        /*clear*/

        else if(strcmp(cmdarg[0],"help")==0){
            if(!cmdarg[1]){
                sprintf(readme,"%s/readme",shell);
                fd=open(readme,O_RDONLY);
                if(fd<0){printf("open error!"); continue;}
                while(read(fd,command,1)!=0){
                    write(1,command,1);            
                }
                close(fd);
            }
            /*help*/
            else if(strcmp(cmdarg[1],">")==0||strcmp(cmdarg[1],">>")==0){
                if(!cmdarg[2]||strcmp(cmdarg[2],"&")==0) printf("wrong file!\n");
                else{
                    if(strcmp(cmdarg[1],">")==0) fd=open(cmdarg[2],O_WRONLY|O_TRUNC|O_CREAT,S_IRUSR|S_IWUSR);/* > */
                    else fd=open(cmdarg[2],O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR);/* >> */
                    sprintf(readme,"%s/readme",shell);
                    fd2=open(readme,O_RDONLY);
                    if(fd2<0){printf("open error!"); close(fd); continue;}
                    while(read(fd2,command,1)!=0){
                        write(fd,command,1);            
                    }
                    close(fd2);
                    close(fd);
                }
            }
            /*help > file */
        }
        /*help*/

        else{
            i=0;
            bg=0;
            while(cmdarg[i++])
            if(strcmp(cmdarg[i-1],"&")==0){
                cmdarg[i-1]=NULL;
                bg=1;
            }
            if(bg){
                pid=fork();
                if(pid<0){
                    printf("fork error!\n");
                }
                /*error*/
                else if(pid==0){
                    execvp(cmdarg[0],cmdarg);
                }
                /*child*/
                else{
                    printf("[%d]\n",pid);
                }
                /*parent*/
            }
            else{
                pid=fork();
                if(pid<0){
                    printf("fork error!\n");
                }
                /*error*/
                else if(pid==0){
                    execvp(cmdarg[0],cmdarg);
                }
                /*child*/
                else{
                    printf("[%d]\n",pid);
                    wait(NULL);
                }
                /*parent*/
            }
            if(child) break;
        }
        /*external inst*/
        
        /**
        * After reading user input, the steps are: 
        *internal:
        *…..
        *external:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execvp()
        * (3) if command included &, parent will invoke wait()
        *…..
        */
    }
    return 0;
}

