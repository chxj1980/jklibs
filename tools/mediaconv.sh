#!/bin/bash

## h264 -> flv
ffmpeg -i x1.h264 -vcodec h264 -s 640 480 -an -f flv x1.flv
