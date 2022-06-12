#include <stdio.h>
#include "my_ufs.h"
#include "mystdio.h"

int main() {
    
    mymkfs(10240);
    // // mymount("myFile.txt", " ", " ", 0, " "); // SEG FAULT IN THIS FUNCTION
    // // printf("mounted\n");
    // // return 0;
    // int fd1 = myfopen("/noah", O_CREAT);
    // int fd11 = myopen("/noah", 0);
    // // myDIR* md1 = myopendir("/rashi");
    // myDIR* md2 = myopendir("/rashi/bear");
    // int fd5 = myopen("/rashi/bear/weiss/pachino", O_CREAT);
    // // myDIR* md3 = myopendir("/rashi/bear/bitchface/bla");
    // char buff[] = "hello world!";
    // int bla = 5;
    // for(int i = 0; i < bla; i++) {
    //     // printf("write # %d\n", i);
    //     mywrite(fd5, buff, 12);
    // }
    // // mywrite(fd5, buff, 12);
    // int curzz = mylseek(fd5, -12, SEEK_CUR);
    // // printf("curzz returned: %d\n", curzz);
    // char text[1000] = {'\0'};
    // myread(fd5, text, 12);
    // printf("read from fd5: :%s:\n", text);
    // printf("fd1 is: %d\n", fd1);
    // printf("fd11 is: %d\n", fd11);
    // // printf("md1 is: %d\n", md1->inode);
    // printf("md2 is: %d\n", md2->inode);
    // // printf("md3 is: %d\n", md3->inode);
    // printf("fd5 is: %d\n", fd5);
    // // mydirent* mrent = myreaddir(md3);
    // // printf("mrent is -> %p\n", mrent);
    // // while(mrent != NULL) {
    // //     printf("obj name -> :%s:\n", mrent->d_name);
    // //     free(mrent);
    // //     mrent = myreaddir(md3);
    // // }
    // // free(mrent);
    // myprint();
    // // myclosedir(md1);
    // myclosedir(md2);
    // // myclosedir(md3);
    // myclose(fd1);
    // myclose(fd5);
    // myclose(fd11);
    // free_mem();
    // printf("done\n");

    myFILE* fp1 = myfopen("/noah", "w");
    char bla[] = "hello rashi! and noah!";
    int wrote = myfwrite(bla, 22, 1, fp1);
    printf("wrote-> %d\n", wrote);

    myFILE* fp2 = myfopen("/noah", "r");
    char res[13] = {'\0'};
    int read = myfread(res, 12, 1, fp2);
    printf("read-> %d\n", read);
    printf("please-> :%s:\n", res);
    myfseek(fp2, -12, SEEK_CUR);
    char res1[23] = {'\0'};
    read = myfread(res1, 22, 1, fp2);
    printf("read-> %d\n", read);


    printf("please-> :%s:\n", res1);
    printf("done\n");

    myfclose(fp1);
    myfclose(fp2);

    return 0;
}
