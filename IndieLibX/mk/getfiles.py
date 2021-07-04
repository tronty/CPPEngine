import os, sys, re

def find_all(name, path):
    result = []
    for root, dirs, files in os.walk(path):
        if name in files:
            result.append(os.path.join(root, name))
    return result

def find_darwin(v):
	for i in v:
		f=i.find('src/SDL')
		if f > -1:
			return i
		f=i.find('darwin')
		if f > -1:
			return i
		f=i.find('macosx')
		if f > -1:
			return i
		f=i.find('linux')
		if f > -1:
			return i
		f=i.find('unix')
		if f > -1:
			return i
		f=i.find('generic')
		if f > -1:
			return i
		f=i.find('dummy')
		if f > -1:
			return i
	return ''
    					

def getfiles(filePath):
	input = open(filePath)
	text=input.read()
	input.close()
	v=[]

	v1=text.split('isa = PBXGroup;')

	for i in v1:

		v2 = re.findall(r'\/\* ([a-zA-Z0-9_]+\.c) \*\/', i)
		for ii in v2:
			v4=find_all(ii, '../Libraries/SDL2-2.0.5/src')
			f=find_darwin(v4)
			if not f in v:
				v.append(f)
		v3 = re.findall(r'\/\* ([a-zA-Z0-9_]+\.m) \*\/', i)
		for ii in v3:
			v5=find_all(ii, '../Libraries/SDL2-2.0.5/src')
			for iii in v5:
				if not iii in v:
					v.append(iii)

	for i in v:
		print i

fl='/home/tronty/Lataukset/SDL2-2.0.5/Xcode/SDL/SDL.xcodeproj/project.pbxproj'
getfiles(fl)

