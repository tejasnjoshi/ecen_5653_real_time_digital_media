#!/bin/bash
FRAME=$1
HRES=$2
exec > commands.sh
	touch commands.sh  #creates a file command.sh
	d=0  #variable for warp degree

#=====================EDIT "2997" by the number of frames in og_folder===========================================================
	for ((c= 1;c<=$FRAME;c++))     
	do 			#for loop to generate commands.sh
#=================EDIT 1280 by the horizontal pixel resolution of the file being used
	      if [ $d -eq $HRES ]  #replace 5 by the horizontal frame width in pixels
		then
		    d=0
	        else
           	:
	      fi
        #e= %05d $c
        k= printf "./cvcode ./og_frames/output_%05d.ppm %d 0\n" $c $d 
	d=$((d+1))
        done
	sed -i '1i#!/bin/bash' commands.sh
	chmod +x commands.sh
	./commands.sh

