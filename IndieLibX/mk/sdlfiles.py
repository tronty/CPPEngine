import os, sys, re
import string

filePath='../Libraries/SDL2-2.0.0/Xcode-iOS/SDL/SDL.xcodeproj/project.pbxproj'

def system(file):
	x=os.system('find ../Libraries/SDL2-2.0.0/src -name '+file)
	return x

x='''FD99B8BD0DD52E6D00FB1D6B /* Library Source */ = {
			isa = PBXGroup;
			children = (
			);
			name = audio;
			path = ../../src/audio;
			};


006E9886119552DD001DE610 /* SDL_rwopsbundlesupport.h */ = {isa = PBXFileReference; ; path = SDL_mixer.c;};
'''
def xxrun(id,apath):
	input = open(filePath)
	outtext=input.read()
	input.close()
	#regexf = re.compile(r'[a-zA-Z0-9]{24}.*? = {.*?};', re.S|re.M)
	regexf = re.compile(r''+id+'.*? = {.*?};', re.S|re.M)
	m=regexf.findall(outtext)
		
	#print m
	if m:
		for i in m:
			if 'isa = PBXGroup;' in i:
				#print 'PBXGroup'

				regexm = re.compile(r'; path = (\w+);', re.S|re.M)
				mm=regexm.match(i)
				if mm:
					apath+='/'+mm.group(0)

				regexch = re.compile(r'\bchildren = (.*?);', re.S|re.M)
				mm=regexch.match(i)
				if mm:
					regexid = re.compile(r'[a-zA-Z0-9]{24}', re.S|re.M)
					mmm=regexf.findall(mm.group(0))
					for i in mmm:
						xxrun(i,apath)
			
			if 'isa = PBXFileReference;' in i:
				#print 'PBXFileReference'
				regexc = re.compile(r'; path = (\w+).c;', re.S|re.M)
				regexm = re.compile(r'; path = (\w+).m;', re.S|re.M)
				mm=regexc.match(i)
				if mm:
					print path+'/'+mm.group(0)
				mm=regexm.match(i)
				if mm:
					print path+'/'+mm.group(0)

				

def xrun():
	input = open(filePath)
	outtext=input.read()
	input.close()

	regexf = re.compile(r'[a-zA-Z0-9]{24}.*? = {isa = PBXFileReference;', re.S|re.M)
	regexg = re.compile(r'[a-zA-Z0-9]{24}.*? \bpath = (.*?);', re.S|re.M)

	regexfm = re.compile(r'([a-zA-Z0-9]{24}).*? = {isa = PBXFileReference;', re.S|re.M)
	regexgm = re.compile(r'([a-zA-Z0-9]{24}).*? \bpath = (.*?);', re.S|re.M)

	m=regexf.findall(outtext)
	#print m
	if m:
		for i in m:		
			m=regexfm.match(outtext)
			if m:
				id=m.group(0)

				regexgm = re.compile(r''+id+' .*?\bpath = (.*?);', re.S|re.M)
				m=regexgm.match(outtext)
				if m:
					path=m.group(0)
			
					print path+'/'+m.group(0)
xxrun('FD99B8BD0DD52E6D00FB1D6B','')

