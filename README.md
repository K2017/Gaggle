# Flock
Our raytracing submission for the Computer Graphics course competition

[[https://github.com/iriediese/Flock/master/Gifs/zoom1.gif|alt=zoom1]]

Starting off from our last raytracing assignment, we have created a number of new frames using the Boost implementation of MPI on the Peregrine cluster. Following that, we assembled our frames into gifs in an attempt to create smooth animations.

In order to parallelise the tasks, we implemented the MPI in our main.cpp, so in its current state it can only be run on the cluster with the Boost module loaded. We used bash scripts to submit jobs and create frames.
