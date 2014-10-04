#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

using namespace cv;
using namespace std;
struct stat st = {0};


int main (int argc, char** argv)

{
char* video_file_name;
char* video_ffmpeg_command;
video_file_name = argv[1];
int file_count = 0, j;
DIR * dirp;
struct dirent * entry;



//creating a directory
if (stat("./og_frames", &st) == -1) {
    mkdir("./og_frames", 0700);
}

//calling the ffmpeg conversion file which accepts file name
system("bash ffmpeg_convert");

//opening the directory to find the number of frame files

dirp = opendir("./og_frames/"); /* There should be error handling after this */
while ((entry = readdir(dirp)) != NULL) {
    if (entry->d_type == DT_REG) { /* If the entry is a regular file */
         file_count++;
    }
}
closedir(dirp);

printf ("\n --> %d frames were created", file_count);

}
