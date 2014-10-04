#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define TEST_RAID_STRING "#0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ##0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ##0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ##0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ##0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ##0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ##0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ##0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ#"

static char testLBA1[512]= TEST_RAID_STRING;
static char testLBA2[512]= TEST_RAID_STRING;


int main(void)
{
    int ssd1, ssd2, ssd3, ssd4, ssd5, ssd6, ssd7;

    if((ssd1=open("/dev/ssd1", O_RDWR)) < 0)
    {
        printf("ERROR opening SSD\n");
        exit(-1);
    }
    else
    {
        printf("ssd=%d opened\n", ssd1);
    }

    // Read and print out first N bytes
    printf("READ FROM SSD:\n");
    read(ssd1, &testLBA1[0], 4);
    printf("test = %2X %2X %2X %2X\n", testLBA1[0], testLBA1[1], testLBA1[2], testLBA1[3]);

    printf("WRITE TO SSD:\n");
    lseek(ssd1, 0, SEEK_SET);
    write(ssd1, &testLBA2[0], 4);

    printf("READ FROM SSD:\n");
    lseek(ssd1, 0, SEEK_SET);
    read(ssd1, &testLBA1[0], 4);
    printf("test = %2X %2X %2X %2X\n", testLBA1[0], testLBA1[1], testLBA1[2], testLBA1[3]);

    close(ssd1);
}
