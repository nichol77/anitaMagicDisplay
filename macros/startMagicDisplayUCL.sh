#!/bin/bash


if [ -z "$ANITA_UTIL_INSTALL_DIR" ]; then
    echo "Need to set ANITA_UTIL_INSTALL_DIR"
    echo "This is where the ANITA software is installed"
    exit 1
fi  

if [ -z "$1" ]; then
    echo "Usage $0 <run>"
    exit 1
fi

export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${ANITA_UTIL_INSTALL_DIR}/lib
root runMagicDisplayUCL.C\(${1}\)
