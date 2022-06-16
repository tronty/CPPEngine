#
# Copyright (c) 2019 Tommi Roenty   http://www.tommironty.fi/
# Licensed under The GNU Lesser General Public License, version 2.1:
#     http://opensource.org/licenses/LGPL-2.1
#

from wand import image
import os, sys, re

dds=[	'BackPlate_Diff',
	'Helmet_Diff',
	'RightShin_Diff',
	'Belt_Diff',
	'LeftArmBand_Diff',
	'RightShoulder_Diff',
	'Fan_Diff',
	'LeftShin_Diff',
	'RightThigh_Diff',
	'Frame_Diff',
	'LeftShoulder_Diff',
	'Warrior_Diff',
	'Ground_Diff',
	'LeftThigh_Diff',
	'Hammer_Diff',
	'RightArmBand_Diff']

def replaceStringInFile(f_):
	head, tail = os.path.split(f_)
	name, ext = os.path.splitext(tail)
	png=head+name+'.png'
	with image.Image(filename=f_) as img:
		img.compression = "no"
		img.save(filename=png)
v=['.']
for i in dds:
	replaceStringInFile(i+'.dds')
