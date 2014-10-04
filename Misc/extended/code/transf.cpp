#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

int main( int argc, char** argv  )
{
  Mat src;
int warp_d, rot_d;  
src = imread( argv[1],1);

warp_d = atoi(argv[2]);
rot_d = atoi(argv[3]);

printf("\nThe Warp degree is %d",warp_d);
printf("\nThe Rotation degree is %d\n",rot_d);

}
