#/bin/bash

## ok
function push_srs_rtmp() {
	for((;;)); do \
        ffmpeg -re -i /opt/data/output/x1.flv \
        -vcodec copy -acodec copy \
        -f flv -y rtmp://192.168.0.135/live/livestream; \
        sleep 1; \
    done
}

## fail
function push_srs_rtmp_h264() {
	for((;;)); do \
        ffmpeg -re -i /opt/data/output/x1.h264 \
        -vcodec copy -acodec copy \
        -f h264 -y rtmp://192.168.0.135/live/livestream; \
        sleep 1; \
    done
}

function push_srs_rtmp_h265() {
	for((;;)); do \
        ffmpeg -re -i /opt/data/output/x1.h265 \
        -vcodec copy -acodec copy \
        -f h265 -y rtmp://192.168.0.135/live/livestream; \
        sleep 1; \
    done
}

#push_srs_rtmp
#push_srs_rtmp_h264
push_srs_rtmp_h265
