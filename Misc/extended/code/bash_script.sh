#!/bin/bash


FILE=$1  #accepts 1st argument as a mp4 video file

#checks for extension
NAME=`echo "$FILE" | cut -d'.' -f1`
EXTENSION=`echo "$FILE" | cut -d'.' -f2`
#MP4="mp4"

if [ "$EXTENSION" == "mp4" ]
then
	echo Converting the File to Frames using ffmpeg...
else
	echo 'BAD FILE (Please use a *.mp4file)'
fi

#creation of directorty to store fram images
if [ -d "og_frames" ]; then
  # Control will enter here if $DIRECTORY exist
        :
else
	mkdir ./og_frames
fi

# ffmpeg command to split video file and store the frames in .ppm format to "og_frames" directory


#counts thenumber of files/frames in the directory
cd og_frames
numfiles=(*)
numfiles=${#numfiles[@]}
cd ..
echo $numfiles


#gets the resolution of the video file
hres= `ffprobe $FILE | cut -d ',' -f3`
vres= `ffprobe $FILE | cut -d 'x' -f4`
echo hres = $hres vres = $vres

#for loop section where the filenames are to be passed as arguments to the ./cvcode executabel to perform affine trasformation on all frames in that folder
d= '0'
for ((c= 1;c<= $numfiles;c++))
do
	
	#if [ $d -eq $hres]
	 #      	then	     
	#	$d=0
	
	#fi  

        f= printf "output_%05d.ppm %d 0" $c $d
	#./cvcode <name_of_frame_file.ppm> <int warp_degree> <int rotation_degree>
	./cvcode $f 
	echo 'done processing frame $c'
	let d++

done
