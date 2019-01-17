#!/bin/bash

function install_opengl() {
	sudo apt install libglew-dev freeglut3-dev libglfw3-dev
}

function log() {
	echo -e "$*"
}

log "install opengl"

install_opengl

log "done"
