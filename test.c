#include <stdio.h>
#include "my_ufs.h"

int main() {
    
    // mymkfs(10240);
    mymount("myFile.txt", " ", " ", 0, " "); // SEG FAULT IN THIS FUNCTION
    printf("mounted\n");
    // return 0;
    // int fd1 = myopen("/noah", 0);
    // int fd2 = myopen("/rashi", 1);
    // int fd3 = myopen("/rashi/bear", 0);
    // char buff[] = "hello world!";
    // mywrite(fd3, buff, 12);
    // mylseek(fd3, -12, SEEK_CUR);
    // char text[100];
    // myread(fd3, text, 12);
    // printf("read from fd3: :%s:\n", text);
    // printf("fd1 is: %d\n", fd1);
    // printf("fd2 is: %d\n", fd2);
    // printf("fd3 is: %d\n", fd3);
    myprint();
    free_mem();
    printf("done\n");

    return 0;
}
