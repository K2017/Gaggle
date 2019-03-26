#!/bin/bash

# $1 = reps
# $2 = filename

sx=$(awk '/Eye/{print $2}' $2 | grep -o '[0-9]\+')
sy=$(awk '/Eye/{print $3}' $2 | grep -o '[0-9]\+')
sz=$(awk '/Eye/{print $4}' $2 | grep -o '[0-9]\+')

lx=$(awk '/Look/{print $2}' $2 | grep -o '[0-9]\+')
ly=$(awk '/Look/{print $3}' $2 | grep -o '[0-9]\+')
lz=$(awk '/Look/{print $4}' $2 | grep -o '[0-9]\+')

xstep=$3
ystep=$4
zstep=$5

lxstep=$6
lystep=$7
lzstep=$8

for ((i=1; i<=$1; i++))
do
	sx=$(($sx + $xstep))
	sy=$(($sy + $ystep))
	sz=$(($sz + $zstep))

	lx=$(($lx + $lxstep))
	ly=$(($ly + $lystep))
	lz=$(($lz + $lzstep))

	echo $sx $sy $sz
	filename="$(date '+%H%M%S%N')ver$sx$sy$sz$2"
	cp $2 $filename
	sed -i 's/Eye.*\],/Eye\"\:\ \['$sx', '$sy', '$sz'\],/g' $filename
	sed -i 's/Look.*\],/Look\"\:\ \['$lx', '$ly', '$lz'\],/g' $filename
done
echo $reps


