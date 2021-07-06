# Makefile for whirlgif utility
#####################################
# Choose your favorite compiler.
#
# GNU C
CC = gcc
#
# standard
#CC = cc

#####################################
# Optimization flags
#
#  GNU C
#OPTIMIZE= -fcombine-regs -O
# standard
OPTIMIZE = -O

#####################################
# Debug flags
#
#DEBUG = -g
#DEBUG = -ggdb

#####################################
# CFLAGS
#		If you need <strings.h>, add -DUSE_STRINGS_H
#
CFLAGS	=  $(DEBUG) $(OPTIMIZE)

all: whirlgif

whirlgif:
	$(CC) $(CFLAGS) -o whirlgif whirlgif.c


# DO NOT DELETE THIS LINE AND DEPENDECIES BELOW

whirlgif: whirlgif.h
whirlgif: whirlgif.c
