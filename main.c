#include <stdio.h>
#include <assert.h>
#include "mystdio.h"

int main() {

    mymkfs(10240); // creates the file system

    int ret = mymount("myFile.txt", NULL, NULL, 0, NULL);
    assert(ret == 0);
    printf("mounted\n");

    myFILE* fp1 = myfopen("/zion", "w"); // this will create a file "/zion"
    myfclose(fp1);

    myFILE* fp2 = myfopen("/zion", "r+"); // open in read and write mode
    char bla[] = "hello rashi! and noah!";

    int wrote = myfprintf(fp2, "%c%c%c%c%c", *bla, *(bla+1), *(bla+2), *(bla+3), *(bla+4));
    assert(wrote == 5);
    printf("successfully wrote in myfprintf -> %d\n", wrote);

    wrote = myfwrite(bla + 5, 17, 1, fp2);
    assert(wrote == 17);
    printf("successfully wrote in myfwrite -> %d\n", wrote);

    int seeked = myfseek(fp2, 0, SEEK_SET);
    assert(seeked == 0);

    char res[23] = {'\0'};
    int read = myfscanf(fp2, "%c %c %c %c %c", res, res+1, res+2, res+3, res+4);
    assert(read == 5);
    printf("successfully read in myfscanf -> %d\n", read);
    printf("read from file -> %c%c%c%c%c\n", *res, *(res+1), *(res+2), *(res+3), *(res+4));
    
    seeked = myfseek(fp2, 0, SEEK_SET);
    assert(seeked == 0);
    
    read = myfread(res, 22, 1, fp2);
    assert(read == 22);
    printf("successfully read in myfread -> %d\n", read);
    printf("read from file -> %s\n", res);
    assert(strcmp(res, "hello rashi! and noah!") == 0);
    myfclose(fp2);

    myDIR* md = myopendir("/animal/doggo"); // create this path
    myFILE* fp3 = myfopen("/animal/doggo/pup", "a"); // add file at this path
    wrote = mywrite(fp3->fd, "mocha is a doggo", 16);
    assert(wrote == 16);
    myclosedir(md);
    myfclose(fp3);

    myFILE* fp4 = myfopen("/animal/doggo/pup", "r"); // checking it opens successfully
    myfclose(fp4);

    printf("\n");
    myprint(); // print info on the file system
    free_mem(); // free all memory
    printf("done\n");

    return 0;
}
