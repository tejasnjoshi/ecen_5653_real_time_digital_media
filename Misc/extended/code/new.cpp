#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace cv;
using namespace std;

int main()
{
int i;
char* a;

for (i=0;i <10;i++)
{
 	sprintf(a,"\noutput_%05d.ppm",i);
	printf("%s",a);
}

return 0;
}
