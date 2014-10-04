#!/bin/bash
d=0
for ((c= 1;c<=2997;c++))
do
      if [ $d -eq 1280 ]  #replace 5 by the horizontal frame width in pixels
	then
	    d=0
        else
           :
      fi
        #e= %05d $c
        k= printf "./cvcode ./og_frames/output_%05d.ppm %d 0\n" $c $d
	#e= printf %d.ppm $c
	#echo $e
        #f= printf %s $e" "$d" "0 
	#echo $f
        d=$((d+1))
        #echo $d
        #eval ./cvcode ./og_files/$c.ppm $d 0
	#echo frame $c processed 
done

