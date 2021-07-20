import os
from os import path
#from os import unlink
import sys
import re
import string
import fileinput

'''
open_list = ["#if 0","#elif 0"]
close_list = ["#elif","#else"]

def removeprepro(filetxt):
    lines2=[]
    lines=filetxt.split('\n')
    inBlockComment = False
    for line in lines:
        if "#if 0" in line:
            inBlockComment = True
        if inBlockComment and ("#elif" or "#else")  in line:
            inBlockComment = False
            # If the */ isn't last, remove through the */
            if line.find("*/") != len(line) - 2:
                line = line[line.find("*/")+2:]
            else:
                continue
        if inBlockComment:
            continue
        #sys.stdout.write(line)
	lines2.append(line)
    return string.join(lines2,'\n')
'''

def removecomments_(content):
	COMMENTS = re.compile(r'''
    (//.*?\n)    # Everything between // and the end of the line/file
    |                     # or
    (/\*.*?\*/)           # Everything between /* and */
''', re.M|re.S)
    	return COMMENTS.sub('\n', content)

def removecomments(content, startsymbol='//', endsymbol='\n', replacement='\n'):
	start=content.find(startsymbol)
	while start>-1:
		end=content.find(endsymbol, start)
		if end>-1:
			content=content[0:start-1]+replacement+content[end+1:len(content)]
		start=content.find(startsymbol)
	return content

def removecomments2(content, startsymbol='//', endsymbol='\n', replacement='\n'):
	stack=[]
	start=content.find(startsymbol)
	while start>-1:
		if ((startsymbol=='{') and (endsymbol=='}')):
			if start>-1:
				stack.append(start)
		end=content.find(endsymbol, start)
		if end>-1 and len(stack):
			start_=stack.pop()			
			content=content[0:start_-1]+replacement+content[end+1:len(content)]
		#if ((startsymbol=='{') and (endsymbol=='}')):
		#	pass
		else:
			content=content[0:start-1]+replacement+content[end+1:len(content)]
		start=content.find(startsymbol)
	return content

def source_code(char):
    if char == '/':
        return comment_begin, ''
    return source_code, char

def comment_begin(char):
    if char == '/':
        return inline_comment, ''
    if char == '*':
        return block_comment, ''
    return source_code, '/'+char

def inline_comment(char):
    if char == '\n':
         return source_code, char
    return inline_comment, ''

def block_comment(char):
    if char == '*':
        return end_block_comment, ''
    return block_comment, ''

def end_block_comment(char):
    if char == '/':
        return source_code, ''
    return block_comment, ''

def remove_comments(content):
    def gen_content():
        parser = source_code
        for character in content:
            parser, text = parser(character)
            yield text

    return ''.join(gen_content())

c=0
open_list = ["[","{","("]
close_list = ["]","}",")"]
open_list = ["{"]
close_list = ["}"]

# Function to check parentheses
def check(myStr):
    stack = []
    for i in myStr:
        if i in open_list:
            stack.append(i)
        elif i in close_list:
            pos = close_list.index(i)
            if ((len(stack) > 0) and
                (open_list[pos] == stack[len(stack)-1])):
                stack.pop()
            else:
                return "Unbalanced"
    if len(stack) == 0:
        return "Balanced"

def osmakedirs(fn):
	if not os.path.exists(fn):
		#print 'osmakedirs(%s)' % (fn)
		os.makedirs(fn)

def load(file):
	ffile = os.path.abspath(file)
	f=open(ffile,'r')
	lines=f.readlines()
	f.close()
	return string.join(lines,'')

def accumulatesignatures(filestr):
	coh=0
	cch=0
    	stack = []
	v=[]
	pos=0
	pos=string.find(filestr, '{', pos)
	if pos>-1:
		#coh+=1
		stack.append(pos)
	while pos>-1:
		if not -1==string.find(filestr, '{', pos):
			coh+=1
			stack.append(pos)
		else:
			return v
		if pos<len(filestr):
			ch=string.find(filestr, '}', pos)
			if not -1==ch:
				cch+=1
				if cch==coh and len(stack):
					oh=stack.pop()
					str=filestr[oh:ch+1]
					pos=ch+1
					if len(str):
						#print str
						v.append(str)
			else:
				return v
		else:
			return v
		if pos==-1:
			return v
	#v=[]
	return v

signatures=[]
def accumulatesignatures(s):
	v=re.findall(r'^(.*?)\;',s,re.MULTILINE)
	#print v
	for i in v:
		#print i
		ln=i.strip()
        	#matchObj = re.search("(\{).*?(\})", ln)
        	prog = re.compile("(\{).*?(\})")
        	result = prog.match(ln)
        	if not result:
		          if not ln[0:7]=='typedef':
		                    if not ln[0:8]=='\ttypedef':
		                              if not ln[0:9]=='//typedef':
			                                       signatures.append(i)

def replaceStringInFile(filePath):
	print filePath
	head, tail = os.path.split(filePath)
	name, ext = os.path.splitext(tail)
	input = open(filePath)
	lines=''
	outtext=''
	if True:
		lines=input.readlines()
		outtext=string.join(lines,'')
	else:
		outtext=input.read()
	input.close()

	s = """return_value ClassName::MethodName(args)
{MethodBody {
	for(int i = 0; i < 5; ++i)
	{
  		for(int j = 0; j < i; ++j)
  		{
    			std::cout.put('*');
  	}
  	std::cout.put('\n');
	return 0;}
} """
	print re.findall(r'[^;]([\w\s\*\&]+)\b(\w+)\s*:\s*:\s*(\w+)\([^{]+\{(.+)}', s, re.S)
	# [('ClassName', 'MethodName', 'MehodBody {} ')]

	s = """return_value ClassName::MethodName(args)
{MethodBody {
	for(int i = 0; i < 5; ++i)
	{
  		for(int j = 0; j < i; ++j)
  		{
    			std::cout.put('*');
  	}
  	std::cout.put('\n');
	return 0;}
} """
	print re.findall(r'[^;]([\w\s\*\&]+)\b(\w+)\s*:\s*:\s*(\w+)\([^{]+\{(.+)}', s, re.S)
	# [('ClassName', 'MethodName', 'MehodBody {} ')]

	'''
C
    c  classes
    d  macro definitions
    e  enumerators (values inside an enumeration)
    f  function definitions
    g  enumeration names
    l  local variables [off]
    m  class, struct, and union members
    n  namespaces
    p  function prototypes [off]
    s  structure names
    t  typedefs
    u  union names
    v  variable definitions
    x  external and forward variable declarations [off]
C++
    c  classes
    d  macro definitions
    e  enumerators (values inside an enumeration)
    f  function definitions
    g  enumeration names
    l  local variables [off]
    m  class, struct, and union members
    n  namespaces
    p  function prototypes [off]
    s  structure names
    t  typedefs
    u  union names
    v  variable definitions
    x  external and forward variable declarations [off]
	'''
	#os.system("ctags -x --c-types=p "+filePath+" | sed -e 's/\s\+/ /g' | cut -d ' ' -f 5- >>"+name+ext+".log")
	#os.system("ctags -x --c-types=f "+filePath+" | sed -e 's/\s\+/ /g' | cut -d ' ' -f 5- >>"+name+ext+".log")
	#return
	os.system("ctags -x --c-types=csf "+filePath+" | sed -e 's/\s\+/ /g' | cut -d ' ' -f 5- >>"+name+ext+".log")
	return
	os.system("ctags -x --c-types=f "+filePath+" | sed -e 's/\s\+/ /g' | cut -d ' ' -f 5- >>"+name+ext+".log")
	os.system("ctags -x --c-types=s "+filePath+" | sed -e 's/\s\+/ /g' | cut -d ' ' -f 5- >>"+name+ext+".log")
	os.system("ctags -x --c-types=c "+filePath+" | sed -e 's/\s\+/ /g' | cut -d ' ' -f 5- >>"+name+ext+".log")
	return

	if True:
		outtext=re.sub('\r', '', outtext, flags=re.M)
		outtext=re.sub('\n', '<NEWLINE>', outtext, flags=re.M)
	else:
		v=outtext.split('\n')
		outtext=string.join(v, '<NEWLINE>')

	if False:
		outtext=re.sub(r'#if 1', '#if defined(ELIF1)', outtext, flags=re.S)
		outtext=re.sub(r'#if 0', '#if defined(ELIF0)', outtext, flags=re.S)
		outtext=re.sub(r'#elif 1', '#elif defined(ELIF1)', outtext, flags=re.S)
		outtext=re.sub(r'#elif 0', '#elif defined(ELIF0)', outtext, flags=re.S)
		f=open('tmp.'+name+'.cpp', 'w')
		f.write(outtext)
		f.close()
		os.system('unifdef -D__cplusplus -D__GNUC__ -DLINUX -D_LINUX -UELIF0 -DELIF1 -ULOG_TRACE_ON -DSDL_2_0_5_BUILD -USDL_1_2_15_BUILD -DSDL_VIDEO_DRIVER_X11 -USDL_VIDEO_DRIVER_WINDOWS -UQT_BUILD -UCYGWIN64 -USWIG -UWIN32 -U_WIN32 -U__WIN32 -U__MINGW32__ -U__MINGW64__ -U__APPLE__ -U_MSC_VER -UANDROID -UOS_IPHONE -UIPHONE_SIMULATOR -UWIN8_APP -UWIN_PHONE_APP tmp.'+name+'.cpp')
		#os.system('cpp -DLINUX -D_LINUX -UWIN32 -U_WIN32 -U__WIN32 -U__MINGW32__ -U__MINGW64__ -U__APPLE__ -U_MSC_VER -U__ANDROID__ -UOS_IPHONE -UIPHONE_SIMULATOR -E -fpreprocessed -P -dD tmp.'+name+ext)
		input = open('tmp.'+name+'.cpp')
		outtext=input.read()
		input.close()
		os.unlink('tmp.'+name+'.cpp')

	#print outtext
	#return

	if False:
		outtext=removecomments(outtext, '//', '\n', '\n')
		outtext=removecomments(outtext, '/*', '*/', '\n')
	elif True:
		outtext=re.sub('//.*?<NEWLINE>', '<NEWLINE>', outtext, flags=re.S)
		outtext=re.sub('/\*.*?\*/', '<NEWLINE>', outtext, flags=re.S)
		outtext=re.sub('\#.*include\b.*?<NEWLINE>', '<NEWLINE>', outtext, flags=re.S)
		outtext=re.sub('\btypedef\b.*?\;', '', outtext, flags=re.S)
		outtext=re.sub('\bpragma\b.*?\)', '', outtext, flags=re.S)
		
	#if False:
	#	outtext=removecomments2(outtext, '{', '}', '\n')

	outtext=re.sub('<NEWLINE>', '\r\n', outtext)
	print outtext
	return

def myfun(dummy, dirr, filess):
	for child in filess:
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.hpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)
	    elif '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile(dirr+'/'+child)

'''
txt = "The rain in Spain"
x = re.search("^The.*Spain$", txt) 
if x:
	print x.group()
	print x.group(0)
exit(0)
'''

replaceStringInFile('../Libraries/STX/include/STX/STXinterfaces.h')
replaceStringInFile('../Libraries/STX/src/stxCPP2.cpp')
exit(0)

v=['../Libraries/STX']
for i in v:
	os.path.walk(i, myfun, 13)
#for i in signatures:
#        print i

