#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "perflib.h"

//this fincction uses gettimeofday to get system time when the function is called
double readTOD(void)
{
   struct timeval tv;
   //ft here is the return variable
   double ft=0.0;
   //checks if the value  returned by gettimeofday() is NULL
   if(gettimeofday(&tv, NULL) != 0)
   {
       //prints a system error message and returns 0.0 	
       perror("readTOD");
       return 0.0;
   }
   else
   {
       //calculates the elapesd time from the timespec structure in microseconds	
       ft = ((double)(((double)tv.tv_sec) + (((double)tv.tv_usec) / 1000000.0)));
       printf("tv_sec=%ld, tv_usec=%ld, sec=%lf\n", tv.tv_sec, tv.tv_usec, ft);
   }

   return ft;
}

//Function which calculates the difference between the stopTOD and startTOD variables storing the time acquizitions 
void elapsedTODPrint(double stopTOD, double startTOD)
{
   double dt;

   if(stopTOD > startTOD)  //stopTOD has to be greater than start or result may cause negetive values.
   {
       dt = (stopTOD - startTOD);  //difference calulation
       printf("dt=%lf\n", dt);
   }
	//error handling 
   else
   {
       printf("WARNING: OVERFLOW\n");
       dt=0.0; 
   }
}

//same function as elapsedTODPrint but returns a double value
double elapsedTOD(double stopTOD, double startTOD)
{
   double dt;

   if(stopTOD > startTOD)
   {
       dt = (stopTOD - startTOD);
       printf("stopTOD=%lf, startTOD=%lf, dt=%lf\n", stopTOD, startTOD, dt);
   }
   else
   {
       printf("WARNING: OVERFLOW\n");
       dt=0.0; 
   }

   return dt;
}
