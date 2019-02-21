#!/bin/bash

bin=./openav/facedetect

cascade=--cascade="/opt/data/libs/opencv-4.0.1/amd64/share/opencv4/haarcascades/haarcascade_frontalface_alt.xml"
nested=--nested-cascade="/opt/data/libs/opencv-4.0.1/amd64/share/opencv4/haarcascades/haarcascade_eye_tree_eyeglasses.xml"
scale=--scale=1.1

#dir=--dir="/media/v/windata2/imgs/kitty"
dir=--dir="/media/v/Amuse2/imgs/littlekitty/"

other="$*"

cmd="$bin $cascade $nested $scale $dir $other"

echo $cmd
$cmd

