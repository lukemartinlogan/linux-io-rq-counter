#!/bin/bash

OPT=$1
MAKEFILE_DIR=$2

if [ $OPT -eq 1 ]; then
	cd ${MAKEFILE_DIR}
	make
fi

if [ $OPT -eq 2 ]; then
	cd ${MAKEFILE_DIR}
	make clean
fi
