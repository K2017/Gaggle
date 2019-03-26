# Flock
Our raytracing submission for the Computer Graphics course competition

<p align="center">
  <img src="Gifs/zoom1.gif" width="400" title="zoom1">
  <img src="Gifs/zoom2.gif" width="400" title="zoom2">
  <img src="Gifs/bunnynormal.gif" width="400" title="bunnynormal">
  <img src="Gifs/bunnytransparent.gif" width="400" title="bunnytransparent">
  <img src="Gifs/flyover1.gif" width="400" title="flyover1">
 </p>

Starting off from our last raytracing assignment, we have created a number of new frames using the Boost implementation of MPI on the Peregrine cluster. Following that, we assembled our frames into gifs in an attempt to create smooth animations.

In order to parallelise the tasks, we implemented the MPI in our main.cpp, so in its current state it can only be run on the cluster with the Boost module loaded. We used bash scripts to submit jobs and create frames.
