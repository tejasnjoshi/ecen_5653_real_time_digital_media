#!/bin/bash
c= 0
for ((i=0;i<=2994;i++))
	do 
		if [ $c -eq 1280 ]
		then
	 	c=0
	 	else 
    	 	:
		fi

k= printf "./cvcode ./og_frames/output_%05d.ppm %d 0\n" $i $c

c= $((c+1))
done
