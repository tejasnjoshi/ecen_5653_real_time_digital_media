#ifndef _PERFLIB_H_

#define _PERFLIB_H_

//dont know why the PS3 clock rate is define here (unused)
//#define PS3_CLK_RATE 3192000000
//defines the UINT32 and UINT64 types 
typedef unsigned int UINT32;
typedef unsigned long long int UINT64;

//implicit declarations for functions
double readTOD(void);
double elapsedTOD(double stopTOD, double startTOD);
void printTOD(double stopTOD, double startTOD);

#endif
