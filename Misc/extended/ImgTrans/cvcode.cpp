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

struct stat st = {0};

/// Global variables
char* source_window = "Source image";
char* warp_window = "Warp";
char* file_name;
char* file_path;
//char* file_dir; //= "./frames/";

int main ( int argc, char** argv)
{


Mat src, warp_dst;
int warp_d, rot_d, i,j;  
src = imread( argv[1],1);
file_path = argv[1];
file_name = strndup(file_path+12,16);
printf("\nThe name of the file is : %s", file_name);
warp_d = atoi(argv[2]);
rot_d = atoi(argv[3]);

printf("\nThe Warp degree is %d",warp_d);
printf("\nThe Rotation degree is %d\n",rot_d);

Point2f src_quad[4];
Point2f dst_quad[4];

//definig a 1X4 matrix {x,y,z,1} for vectors to be provided to the warp fucnction
Mat rot_mat( 1, 4, CV_32FC1 );
Mat warp_mat( 1, 4, CV_32FC1 );

//setting the destination image coordinates
warp_dst = Mat::zeros( src.rows, src.cols, src.type() );

i=warp_d;


//creating a directory
if (stat("./final_frames", &st) == -1) {
    mkdir("./final_frames", 0700);
}

//defining the source image coordinates
src_quad[0] = Point2f( 0,0 );
src_quad[1] = Point2f( src.cols - 1, 0 );
src_quad[2] = Point2f( 0, src.rows - 1 );
src_quad[3] = Point2f( src.cols - 1, src.rows - 1 );
  
//defining the destination image coordinates
dst_quad[0] = Point2f( i, 0 );
dst_quad[1] = Point2f( src.cols-(i+1), 0 );
dst_quad[2] = Point2f( i, src.rows-1 );
dst_quad[3] = Point2f( src.cols-(i+1), src.rows-1 );

/// Get the Affine Transform
warp_mat = getPerspectiveTransform( src_quad, dst_quad );

/// Apply the Affine Transform just found to the src image
warpPerspective(src, warp_dst, warp_mat, warp_dst.size());



/// Show what you got
//namedWindow( source_window, CV_WINDOW_AUTOSIZE );
//imshow( source_window, src );
//namedWindow( warp_window, CV_WINDOW_AUTOSIZE );
//imshow( warp_window, warp_dst );

j = chdir("./final_frames/");
if (j==0)
printf("SUCESS changeing DIR\n");
else
printf("FAILED changing DIR\n");
imwrite(file_name, warp_dst);


/// Wait until user exits the program
//waitKey(0);

return 0;
}


