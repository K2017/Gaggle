#!/bin/bash

#SBATCH --job-name=raytracer
#SBATCH --nodes=8
#SBATCH --time=00:30:00
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=24
#SBATCH --mem=400
#SBATCH --partition=short

module load Boost

if [[ -d $1 ]] 
then
	echo "Running in batch render mode."
	for scene in $1/*.json 
	do
		echo "$scene"
		srun build/ray "$scene"		
	done
	exit 0
elif [[ -f $1 ]]
then
	echo "Running in oneshot render mode."
	srun build/ray $1
	exit 0
else
	exit 1
fi
