#
# Copyright (c) 2016 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#
import os 
import os.path
import re
import string
import struct
import getopt
import sys
from xml.etree import ElementTree
from os.path import expanduser
import shutil
import platform

dir='../IndieLib_resources/masters-grammar-interpreter/'

input=["squid_ship", "borg_cube", "testing", "enterprise", "space_station", "klingon", "spike_ship", "wing_ship", "c_station", "rotational_ship", "tie_fighter"]

#input=["testing"]

nodes=["cylinder", "ellipsoid", "rectangle", "wing", "winggun", "ss_back", "ss_side_wing", "fuckoff"]

def load(file):
    xml_file = os.path.abspath(file)
    f=open(xml_file,'r')
    lines=f.readlines()
    f.close()
    lXmlAsString=string.join(lines,'')
    return lXmlAsString, lines

def cmpint(somenumber):
	inNumber = somenumber
	try:
    		inNumberint = int(inNumber)
	    	return True
	except ValueError:
    		pass
	return False

def cmpfloat(somenumber):
	inNumber = somenumber
	try:
	    	inNumberfloat = float(inNumber)
	    	return True
	except ValueError:
	    	pass
	return False

def cmp(v):

	if len(v)<26:
		return -1

	if not v[0]=='name':
		return 0
	if not v[2]=='active':
		return 2
	if not v[4]=='additive':
		return 4
	if not v[6]=='position':
		return 6

	if not cmpfloat(v[7]):
		return 7
	if not cmpfloat(v[8]):
		return 8
	if not cmpfloat(v[9]):
		return 9
	if not v[10]=='extents':
		return 10
	if not cmpfloat(v[11]):
		return 11
	if not cmpfloat(v[12]):
		return 12
	if not cmpfloat(v[13]):
		return 13
	if not v[14]=='orientation':
		return 14
	if not cmpfloat(v[15]):
		return 15
	if not cmpfloat(v[16]):
		return 16
	if not cmpfloat(v[17]):
		return 17
	if not cmpfloat(v[18]):
		return 18
	if not cmpfloat(v[19]):
		return 19
	if not cmpfloat(v[20]):
		return 20
	if not cmpfloat(v[21]):
		return 21
	if not cmpfloat(v[22]):
		return 22
	if not cmpfloat(v[23]):
		return 23
	if not v[24]=='priority':
		return 24

	if not cmpint(v[25]):
		return 25

	return -1

def run():
	txt=''
	for i in range(0,len(input)):
		ff=dir+input[i]+'.txt'
		print ff
		txt, lines=load(ff)
		for j in range(0,len(lines)):
			v=lines[j].split()
			r=cmp(v)
			if not r==-1:
				print r
				print dir+input[i]+'.txt'
	#print txt

x='''
"cylinder", "ellipsoid", "rectangle"


in_name = "", 
in_active = True,  
in_additive = True,
in_position = vec3(), 
in_extents = vec3(), 
in_orientation = mat3(1.0), 
in_priority = 0, 
in_tags = None):

name ellipsoid
active True
additive True
position  0.0 0.0 0.0 
extents  10.0 15.0 10.0 
orientation  1.0 0.0 0.0 0.0 1.0 0.0 0.0 0.0 1.0 
priority  0 
tags_begin  tags_end #
'''

run()

