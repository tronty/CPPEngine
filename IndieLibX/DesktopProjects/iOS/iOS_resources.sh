#!/bin/bash
source ../../../make.cnf

TOPDIR_="./Documents/IndieLib_resources"
if [ -n "$1" ]
then
	TOPDIR_="$1/Documents/IndieLib_resources"
fi

DIR=../../../..

#mkdir -p "$TOPDIR_"

CURDIR=`pwd -P` 



