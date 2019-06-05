#!/bin/bash

# $1 = reps
# $2 = filename
#  "model": "reducedbunny.obj", "rotation": [0, 0, 0],

sx=$(awk '/reducedbunny.obj", "rotation/{print $2}' $2 | grep -o '[0-9]\+')
sy=$(awk '/reducedbunny.obj", "rotation/{print $3}' $2 | grep -o '[0-9]\+')
sz=$(awk '/reducedbunny.obj", "rotation/{print $4}' $2 | grep -o '[0-9]\+')

xstep=$3
ystep=$4
zstep=$5

for ((i=1; i<=$1; i++))
do
	sx=$(($sx + $xstep))
	sy=$(($sy + $ystep))
	sz=$(($sz + $zstep))

	echo $sx $sy $sz
	file="$(date '+%H%M%S%N')ver$sx$sy$sz$2"
	cp $2 $file
	sed -i 's/reducedbunny.obj", "rotation.*\],/reducedbunny.obj", "rotation\"\:\ \['$sx', '$sy', '$sz'\],/g' $file
	
#	rm $file
done
echo $i


