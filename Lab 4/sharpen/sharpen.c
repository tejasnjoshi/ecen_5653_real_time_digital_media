#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "perflib.h"
#include "perflib.c"

#ifdef IPP
#include <ipp.h>
#include <ippdefs.h>
#endif

typedef double FLOAT;
//typedef float FLOAT;

// Cycle Counter Code
//
// Can be replaced with ippGetCpuFreqMhz and ippGetCpuClocks
// when IPP core functions are available.
//
typedef unsigned int UINT32;
typedef unsigned long long int UINT64;
typedef unsigned char UINT8;


UINT64 startTSC = 0;
UINT64 stopTSC = 0;
UINT64 cycleCnt = 0;

#define PMC_ASM(instructions,N,buf) \
  __asm__ __volatile__ ( instructions : "=A" (buf) : "c" (N) )

#define PMC_ASM_READ_TSC(buf) \
  __asm__ __volatile__ ( "rdtsc" : "=A" (buf) )

//#define PMC_ASM_READ_PMC(N,buf) PMC_ASM("rdpmc" "\n\t" "andl $255,%%edx",N,buf)
#define PMC_ASM_READ_PMC(N,buf) PMC_ASM("rdpmc",N,buf)

#define PMC_ASM_READ_CR(N,buf) \
  __asm__ __volatile__ ( "movl %%cr" #N ",%0" : "=r" (buf) )

UINT64 readTSC(void)
{
   UINT64 ts;

   __asm__ volatile(".byte 0x0f,0x31" : "=A" (ts));
   return ts;
}


UINT64 cyclesElapsed(UINT64 stopTS, UINT64 startTS)
{
   return (stopTS - startTS);
}
						    
// PPM Edge Enhancement Code
//
UINT8 header[22];
UINT8 R[1440000];
UINT8 G[1440000];
UINT8 B[1440000];
UINT8 convR[1440000];
UINT8 convG[1440000];
UINT8 convB[1440000];

#define K 4.0

FLOAT PSF[9] = {-K/8.0, -K/8.0, -K/8.0, -K/8.0, K+1.0, -K/8.0, -K/8.0, -K/8.0, -K/8.0};

int main(int argc, char *argv[])
{
    int fdin, fdout, bytesRead=0, bytesLeft, i, j,k;
    UINT64 microsecs=0, clksPerMicro=0, millisecs=0;
    FLOAT temp, clkRate;
double startTOD=0.0, stopTOD=0.0, 	total_read=0.0,total_write=0.0,total_conv=0.0,total_close=0.0, total_io =0.0;    
    // Estimate CPU clock rate
    startTSC = readTSC();
    usleep(1000000);
    stopTSC = readTSC();
    cycleCnt = cyclesElapsed(stopTSC, startTSC);

    printf("Cycle Count=%llu\n", cycleCnt);
    clkRate = ((FLOAT)cycleCnt)/1000000.0;
    clksPerMicro=(UINT64)clkRate;
    printf("Based on usleep accuracy, CPU clk rate = %llu clks/sec,",
          cycleCnt);
    printf(" %7.1f Mhz\n", clkRate);
    
    //printf("argc = %d\n", argc);

    if(argc < 2)
    {
       printf("Usage: sharpen file.ppm\n");
       exit(-1);
    }
    else
    {
	//printf("PSF:\n");
	//for(i=0;i<9;i++)
	//{
	//    printf("PSF[%d]=%lf\n", i, PSF[i]);
	//}

        //printf("Will open file %s\n", argv[1]);

        if((fdin = open(argv[1], O_RDONLY, 0644)) < 0)
        {
            printf("Error opening %s\n", argv[1]);
        }
        //else
        //    printf("File opened successfully\n");

        if((fdout = open("sharpen.ppm", (O_RDWR | O_CREAT), 0666)) < 0)
        {
            printf("Error opening %s\n", argv[1]);
        }
        //else
        //    printf("Output file=%s opened successfully\n", "sharpen.ppm");
    }

    bytesLeft=21;

    //printf("Reading header\n");

  // for(k=0;k<30;k++)
  // {
     
     
     startTOD = readTOD();
    do
    {
        //printf("bytesRead=%d, bytesLeft=%d\n", bytesRead, bytesLeft);
        bytesRead=read(fdin, (void *)header, bytesLeft);
        bytesLeft -= bytesRead;
    } while(bytesLeft > 0);
    
    header[21]='\0';
   

    //printf("header = %s\n", header); 
    
    // Read RGB data
    for(i=0; i<1440000; i++)
    {
        read(fdin, (void *)&R[i], 1); convR[i]=R[i];
        read(fdin, (void *)&G[i], 1); convG[i]=G[i];
        read(fdin, (void *)&B[i], 1); convB[i]=B[i];
    }
    stopTOD =readTOD();
    total_read = elapsedTOD(stopTOD,startTOD);
    // Start of convolution time stamp

    startTOD = readTOD();
    startTSC = readTSC();

    // Skip first and last row, no neighbors to convolve with
    for(i=1; i<899; i++)
    {

        // Skip first and last column, no neighbors to convolve with
        for(j=1; j<1599; j++)
        {
            temp=0;
            temp += (PSF[0] * (FLOAT)R[((i-1)*1600)+j-1]);
            temp += (PSF[1] * (FLOAT)R[((i-1)*1600)+j]);
            temp += (PSF[2] * (FLOAT)R[((i-1)*1600)+j+1]);
            temp += (PSF[3] * (FLOAT)R[((i)*1600)+j-1]);
            temp += (PSF[4] * (FLOAT)R[((i)*1600)+j]);
            temp += (PSF[5] * (FLOAT)R[((i)*1600)+j+1]);
            temp += (PSF[6] * (FLOAT)R[((i+1)*1600)+j-1]);
            temp += (PSF[7] * (FLOAT)R[((i+1)*1600)+j]);
            temp += (PSF[8] * (FLOAT)R[((i+1)*1600)+j+1]);
	    if(temp<0.0) temp=0.0;
	    if(temp>255.0) temp=255.0;
	    convR[(i*1600)+j]=(UINT8)temp;

            temp=0;
            temp += (PSF[0] * (FLOAT)G[((i-1)*1600)+j-1]);
            temp += (PSF[1] * (FLOAT)G[((i-1)*1600)+j]);
            temp += (PSF[2] * (FLOAT)G[((i-1)*1600)+j+1]);
            temp += (PSF[3] * (FLOAT)G[((i)*1600)+j-1]);
            temp += (PSF[4] * (FLOAT)G[((i)*1600)+j]);
            temp += (PSF[5] * (FLOAT)G[((i)*1600)+j+1]);
            temp += (PSF[6] * (FLOAT)G[((i+1)*1600)+j-1]);
            temp += (PSF[7] * (FLOAT)G[((i+1)*1600)+j]);
            temp += (PSF[8] * (FLOAT)G[((i+1)*1600)+j+1]);
	    if(temp<0.0) temp=0.0;
	    if(temp>255.0) temp=255.0;
	    convG[(i*1600)+j]=(UINT8)temp;

            temp=0;
            temp += (PSF[0] * (FLOAT)B[((i-1)*1600)+j-1]);
            temp += (PSF[1] * (FLOAT)B[((i-1)*1600)+j]);
            temp += (PSF[2] * (FLOAT)B[((i-1)*1600)+j+1]);
            temp += (PSF[3] * (FLOAT)B[((i)*1600)+j-1]);
            temp += (PSF[4] * (FLOAT)B[((i)*1600)+j]);
            temp += (PSF[5] * (FLOAT)B[((i)*1600)+j+1]);
            temp += (PSF[6] * (FLOAT)B[((i+1)*1600)+j-1]);
            temp += (PSF[7] * (FLOAT)B[((i+1)*1600)+j]);
            temp += (PSF[8] * (FLOAT)B[((i+1)*1600)+j+1]);
	    if(temp<0.0) temp=0.0;
	    if(temp>255.0) temp=255.0;
	    convB[(i*1600)+j]=(UINT8)temp;
        }
    }

    // End of convolution time stamp
    stopTOD = readTOD();
    total_conv= elapsedTOD(stopTOD,startTOD);
    stopTSC = readTSC();
    cycleCnt = cyclesElapsed(stopTSC, startTSC);
    microsecs = cycleCnt/clksPerMicro;
    millisecs = microsecs/1000;

    printf("Convolution time in cycles=%llu, rate=%llu, about %llu millisecs\n",
	    cycleCnt, clksPerMicro, millisecs);
    

    startTOD  = readTOD();
    write(fdout, (void *)header, 21);

    // Write RGB data
    for(i=0; i<1440000; i++)
    {
        write(fdout, (void *)&convR[i], 1);
        write(fdout, (void *)&convG[i], 1);
        write(fdout, (void *)&convB[i], 1);
    }
    stopTOD  = readTOD();
    total_write = elapsedTOD(stopTOD,startTOD);
    
    
    close(fdin);
    close(fdout);
    
    //}

   printf("\nTotal read time: %lf millisec", total_read*30000);
   printf("\nTotal write time: %lf millisec", total_write*30000);
   total_io = total_read + total_write;
   printf("\n\nTotal IO time: %lf millisec", total_io*30000);
   printf("\nTotal convolution time: %lf millisec\n", total_conv*30000);
}
