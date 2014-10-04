#include <stdio.h>
#include <unistd.h>
#include <time.h>
// On x86 Pentium machines which implement the Time Stamp
// Counter in the PMU, pass in -DPMU_ANALYSIS.  For the Pentium,
// CPU cycles will be measured and CPI estimated based upon known
// instruction count.
//
// Leave the #define LONG_LONG_OK if your compiler and architecture
// support 64-bit unsigned integers, declared as unsigned long long in
// ANSI C.
//
// If not, please remove the #define below for 32-bit unsigned
// long declarations.
//

#define FIB_LIMIT_FOR_32_BIT 47

struct timespec t1, t2, t3, t4;

typedef unsigned long long int UINT64;

UINT64 cycleCnt = 0;

UINT64 idx = 0, jdx = 1;
UINT64 seqIterations = FIB_LIMIT_FOR_32_BIT;
UINT64 reqIterations = 1, Iterations = 1;
UINT64 fib = 0, fib0 = 0, fib1 = 1;

//Code that will provide the Fibonacci Calculations
#define FIB_TEST(seqCnt, iterCnt)      \
   for(idx=0; idx < iterCnt; idx++)    \
   {                                   \
      fib = fib0 + fib1;               \
      while(jdx < seqCnt)              \
      {                                \
         fib0 = fib1;                  \
         fib1 = fib;                   \
         fib = fib0 + fib1;            \
         jdx++;                        \
      }                                \
   }                                   \


void fib_wrapper(void)
{
   FIB_TEST(seqIterations, Iterations);
}

//Defininf the inner and outer loop instruction counts for Fi-Bonacci
#define INST_CNT_FIB_INNER 15
#define INST_CNT_FIB_OUTTER 6



//THE MAIN ROUTINE

int main( int argc, char *argv[] ) //takes in arguments
{	
   double clkRate = 0.0, fibCPI = 0.0;	//definition of variables
   UINT64 instCnt = 0; 	//Define instCnt (count of instructions)

   if(argc == 2)	//Check if 2 arguments are provided
   {
      sscanf(argv[1], "%ld", &reqIterations);  //Store the second argument value in the variable reqIterations

      seqIterations = reqIterations % FIB_LIMIT_FOR_32_BIT;
//find the number of seqIterations by mod with 47
      Iterations = reqIterations / seqIterations;
//find the number of program iterations
   }
   else if(argc == 1)                
//if no second argument is present go ahead with the default number of iterations
      printf("Using defaults\n");
   else
      printf("Usage: fibtest [Num iterations]\n");

//calculating the number of instructions
   instCnt = (INST_CNT_FIB_INNER * seqIterations) +
             (INST_CNT_FIB_OUTTER * Iterations) + 1;

   // Estimate CPU clock rate
   clock_gettime(CLOCK_REALTIME, &t1);
   usleep(1000000);
   clock_gettime(CLOCK_REALTIME, &t2);
   //calculating eplapsed time in nanoseconds
   cycleCnt = ((t2.tv_sec-t1.tv_sec)*1000000000)+(t2.tv_nsec-t1.tv_nsec);


//Printing the number of cycles from the differnece in the CLOCK_REALTIME values obtained 
   printf("Cycle Count=%llu\n", cycleCnt);
//calculating the the Clock Rate
   clkRate = ((double)cycleCnt/1000000000);
//Printing the clock to the screen
   printf("Based on usleep accuracy, CPU clk rate = %lu clks/sec,",
          cycleCnt);
   printf(" %7.1f Ghz\n", clkRate);

//Printing the iteration numbers and the length number of FB calculations
   printf("\nRunning Fibonacci(%d) Test for %ld iterations\n",
          seqIterations, Iterations);


   // START Timed Fibonacci Test
      clock_gettime(CLOCK_REALTIME, &t3);
   
   //calls the FIB_TEST routine which calculates the Fibonacci value in the default/user supplied iterations
   FIB_TEST(seqIterations, Iterations);
   
   // END Timed Fibonacci Test
   clock_gettime(CLOCK_REALTIME, &t4);


   printf("startTSC =0x%016x\n", t3.tv_sec);
   printf("stopTSC =0x%016x\n", t4.tv_sec);

   cycleCnt = ((t4.tv_sec-t3.tv_sec)*1000000000)+(t4.tv_nsec-t3.tv_nsec);
   printf("\nFibonacci(%lu)=%lu (0x%08lx)\n", seqIterations, fib, fib);
   printf("\nCycle Count=%llu\n", cycleCnt);
   printf("\nInst Count=%lu\n", instCnt);
   fibCPI = ((double)cycleCnt) / ((double)instCnt);
   printf("\nCPI=%1.32f\n", fibCPI);

}
