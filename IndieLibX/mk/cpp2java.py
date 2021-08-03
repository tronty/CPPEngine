import os
from os import path
#from os import unlink
import sys
import re
import string

'''
import java.awt.DisplayMode;
import javax.media.opengl.GL2;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLCapabilities;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.GLProfile;
import javax.media.opengl.awt.GLCanvas;
import javax.media.opengl.glu.GLU;

import javax.swing.JFrame;

import com.jogamp.opengl.util.FPSAnimator;

public class Cube implements GLEventListener {

   public static DisplayMode dm, dm_old;
   private GLU glu = new GLU();
   private float rquad = 0.0f;
      
   @Override
   public void display( GLAutoDrawable drawable ) {
	
      final GL2 gl = drawable.getGL().getGL2();
      gl.glClear(GL2.GL_COLOR_BUFFER_BIT | GL2.GL_DEPTH_BUFFER_BIT );
      gl.glLoadIdentity();
      gl.glTranslatef( 0f, 0f, -5.0f ); 

      // Rotate The Cube On X, Y & Z
      gl.glRotatef(rquad, 1.0f, 1.0f, 1.0f); 
 
      //giving different colors to different sides
      gl.glBegin(GL2.GL_QUADS); // Start Drawing The Cube
      gl.glColor3f(1f,0f,0f); //red color
      gl.glVertex3f(1.0f, 1.0f, -1.0f); // Top Right Of The Quad (Top)
      gl.glVertex3f( -1.0f, 1.0f, -1.0f); // Top Left Of The Quad (Top)
      gl.glVertex3f( -1.0f, 1.0f, 1.0f ); // Bottom Left Of The Quad (Top)
      gl.glVertex3f( 1.0f, 1.0f, 1.0f ); // Bottom Right Of The Quad (Top)
		
      gl.glColor3f( 0f,1f,0f ); //green color
      gl.glVertex3f( 1.0f, -1.0f, 1.0f ); // Top Right Of The Quad
      gl.glVertex3f( -1.0f, -1.0f, 1.0f ); // Top Left Of The Quad
      gl.glVertex3f( -1.0f, -1.0f, -1.0f ); // Bottom Left Of The Quad
      gl.glVertex3f( 1.0f, -1.0f, -1.0f ); // Bottom Right Of The Quad 

      gl.glColor3f( 0f,0f,1f ); //blue color
      gl.glVertex3f( 1.0f, 1.0f, 1.0f ); // Top Right Of The Quad (Front)
      gl.glVertex3f( -1.0f, 1.0f, 1.0f ); // Top Left Of The Quad (Front)
      gl.glVertex3f( -1.0f, -1.0f, 1.0f ); // Bottom Left Of The Quad
      gl.glVertex3f( 1.0f, -1.0f, 1.0f ); // Bottom Right Of The Quad 

      gl.glColor3f( 1f,1f,0f ); //yellow (red + green)
      gl.glVertex3f( 1.0f, -1.0f, -1.0f ); // Bottom Left Of The Quad
      gl.glVertex3f( -1.0f, -1.0f, -1.0f ); // Bottom Right Of The Quad
      gl.glVertex3f( -1.0f, 1.0f, -1.0f ); // Top Right Of The Quad (Back)
      gl.glVertex3f( 1.0f, 1.0f, -1.0f ); // Top Left Of The Quad (Back)

      gl.glColor3f( 1f,0f,1f ); //purple (red + green)
      gl.glVertex3f( -1.0f, 1.0f, 1.0f ); // Top Right Of The Quad (Left)
      gl.glVertex3f( -1.0f, 1.0f, -1.0f ); // Top Left Of The Quad (Left)
      gl.glVertex3f( -1.0f, -1.0f, -1.0f ); // Bottom Left Of The Quad
      gl.glVertex3f( -1.0f, -1.0f, 1.0f ); // Bottom Right Of The Quad 

      gl.glColor3f( 0f,1f, 1f ); //sky blue (blue +green)
      gl.glVertex3f( 1.0f, 1.0f, -1.0f ); // Top Right Of The Quad (Right)
      gl.glVertex3f( 1.0f, 1.0f, 1.0f ); // Top Left Of The Quad
      gl.glVertex3f( 1.0f, -1.0f, 1.0f ); // Bottom Left Of The Quad
      gl.glVertex3f( 1.0f, -1.0f, -1.0f ); // Bottom Right Of The Quad
      gl.glEnd(); // Done Drawing The Quad
      gl.glFlush();
		
      rquad -= 0.15f;
   }
   
   @Override
   public void dispose( GLAutoDrawable drawable ) {
      // TODO Auto-generated method stub
   }
   
   @Override
   public void init( GLAutoDrawable drawable ) {
	
      final GL2 gl = drawable.getGL().getGL2();
      gl.glShadeModel( GL2.GL_SMOOTH );
      gl.glClearColor( 0f, 0f, 0f, 0f );
      gl.glClearDepth( 1.0f );
      gl.glEnable( GL2.GL_DEPTH_TEST );
      gl.glDepthFunc( GL2.GL_LEQUAL );
      gl.glHint( GL2.GL_PERSPECTIVE_CORRECTION_HINT, GL2.GL_NICEST );
   }
      
   @Override
   public void reshape( GLAutoDrawable drawable, int x, int y, int width, int height ) {
	
      // TODO Auto-generated method stub
      final GL2 gl = drawable.getGL().getGL2();
      if( height lt;= 0 )
         height = 1;
			
      final float h = ( float ) width / ( float ) height;
      gl.glViewport( 0, 0, width, height );
      gl.glMatrixMode( GL2.GL_PROJECTION );
      gl.glLoadIdentity();
		
      glu.gluPerspective( 45.0f, h, 1.0, 20.0 );
      gl.glMatrixMode( GL2.GL_MODELVIEW );
      gl.glLoadIdentity();
   }
      
   public static void main( String[] args ) {
	
      final GLProfile profile = GLProfile.get( GLProfile.GL2 );
      GLCapabilities capabilities = new GLCapabilities( profile );
      
      // The canvas
      final GLCanvas glcanvas = new GLCanvas( capabilities );
      Cube cube = new Cube();
		
      glcanvas.addGLEventListener( cube );
      glcanvas.setSize( 400, 400 );
		
      final JFrame frame = new JFrame ( " Multicolored cube" );
      frame.getContentPane().add( glcanvas );
      frame.setSize( frame.getContentPane().getPreferredSize() );
      frame.setVisible( true );
      final FPSAnimator animator = new FPSAnimator(glcanvas, 300,true);
		
      animator.start();
   }
	
}
'''

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

classes=[]
def accumulatestructsclasses(s):
	v=re.findall(r'\bstruct\s+(\w+)',s,re.MULTILINE)
	print v
	for i in v:
		print i
		classes.append(i)
	v=re.findall(r'\bclass\s+(\w+)',s,re.MULTILINE)
	print v
	for i in v:
		print i
		classes.append(i)

def replaceStringInFile2(filePath):
	return
	input = open(filePath)
	outtext=input.read()
	input.close()
	accumulatestructsclasses(outtext)

def replaceStringInFile1(filePath):
	print filePath
	head, tail = os.path.split(filePath)
	name, ext = os.path.splitext(tail)
	filePath2=head+'/'+name+'.ext.java'
	if ext=='.h':
		filePath2=head+'/'+name+'.h.java'
	elif ext=='.hxx':
		filePath2=head+'/'+name+'.hxx.java'
	elif ext=='.inl':
		filePath2=head+'/'+name+'.inl.java'
	elif ext=='.c':
		filePath2=head+'/'+name+'.c.java'
	elif ext=='.cc':
		filePath2=head+'/'+name+'.cc.java'
	elif ext=='.cpp':
		filePath2=head+'/'+name+'.cpp.java'
	elif ext=='.cxx':
		filePath2=head+'/'+name+'.cxx.java'
	else:
		return
	filePath2=filePath2.replace('../','../../IndieLibJ/')
	head, tail = os.path.split(filePath2)
	input = open(filePath)
	outtext=input.read()
	input.close()
	##c=c+len(re.findall( r'\bdelete\b', outtext))
	##print re.findall( r'\bdelete\b', outtext)
	v=accumulatesignatures(outtext)
	#return
	print filePath2
	#print head
	if not os.path.exists(head):
		osmakedirs(head)
	outtext=string.join(v, '\n')
	outtext=re.sub(r'/\*.*?\*/', '', outtext, flags=re.MULTILINE)
	outtext=re.sub(r'//.*?\n', '', outtext)#, flags=re.IGNORECASE)

	outtext=re.sub(r'\boperator\b.*?\;', '', outtext, flags=re.MULTILINE) # operator ???
	outtext=re.sub(r'\#define.*?\n', '', outtext)

	outtext=re.sub(r'\btypedef\s+struct\b', 'struct', outtext, flags=re.MULTILINE)
	outtext=re.sub(r'\btypedef\s+class\b', 'class', outtext, flags=re.MULTILINE)

	outtext=re.sub(r'\btypedef\b.*?\;', '', outtext, flags=re.MULTILINE)

	outtext=re.sub('\#\s*\bif\b\s+0\b.*?\#\s*\belif\b', '#if', outtext, re.MULTILINE)
	outtext=re.sub('\#\s*\belif\b\s+0\b.*?\#\s*\belif\b', '#if', outtext, re.MULTILINE)
	outtext=re.sub('\#\s*\bif\b\s+0\b.*?\#\s*\belse\b', '#if 1', outtext, re.MULTILINE)
	outtext=re.sub('\#\s*\belif\b\s+0\b.*?\#\s*\belse\b', '#if 1', outtext, re.MULTILINE)
	outtext=re.sub(r'\-\s*\>','.', outtext)
	outtext=re.sub(r'\&','', outtext)
	outtext=re.sub(r'\bLOG_PRINT\b','System.out.println', outtext)
	outtext=re.sub(r'\b[/]+LOG_FNLN\b[;]+','', outtext)
	outtext=re.sub(r'\bdelete\s*\[\s*\]\s*\b','', outtext)
	outtext=re.sub(r'\bdelete\b','', outtext)
	outtext=re.sub(r'\bconst\s*char\s*\*\s*','java.lang.String', outtext)
	outtext=re.sub(r'\bconst\s*char\s*','java.lang.String', outtext)
	outtext=re.sub(r'\bchar\s*\*\s*','java.lang.String', outtext)
	outtext=re.sub(r'\bchar\s*const\s*\*','java.lang.String', outtext)
	outtext=re.sub(r'\bchar\s*const\b','java.lang.String', outtext)
	outtext=re.sub(r'\bNULL\b','null', outtext)
	outtext=re.sub(r'\bunsigned\b','', outtext)
	#outtext=re.sub(r'\bchar\b','java.lang.String', outtext)
	outtext=re.sub(r'\bBOOL\b','boolean', outtext)
	outtext=re.sub(r'\bWINAPI\b','', outtext)

	outtext=re.sub(r'\bD3DXFROMWINE_PI\b','java.lang.Math.PI', outtext)
	outtext=re.sub(r'\babs\b','java.lang.Math.abs', outtext)
	outtext=re.sub(r'\bpow\b','java.lang.Math.pow', outtext)
	outtext=re.sub(r'\babsf\b','java.lang.Math.abs', outtext)
	outtext=re.sub(r'\bpowf\b','java.lang.Math.pow', outtext)
	outtext=re.sub(r'\babsl\b','java.lang.Math.abs', outtext)
	outtext=re.sub(r'\bpowl\b','java.lang.Math.pow', outtext)

	outtext=re.sub(r'\bsinl\b','java.lang.Math.sin', outtext)
	outtext=re.sub(r'\bcosl\b','java.lang.Math.cos', outtext)
	outtext=re.sub(r'\btanl\b','java.lang.Math.tan', outtext)
	outtext=re.sub(r'\basinl\b','java.lang.Math.asin', outtext)
	outtext=re.sub(r'\bacosl\b','java.lang.Math.acos', outtext)
	outtext=re.sub(r'\batan\b','java.lang.Math.atan', outtext)

	outtext=re.sub(r'\bsinhl\b','java.lang.Math.sinh', outtext)
	outtext=re.sub(r'\bcoshl\b','java.lang.Math.cosh', outtext)
	outtext=re.sub(r'\btanhl\b','java.lang.Math.tanh', outtext)
	outtext=re.sub(r'\basinhl\b','java.lang.Math.asinh', outtext)
	outtext=re.sub(r'\bacoshl\b','java.lang.Math.acosh', outtext)
	outtext=re.sub(r'\batanhl\b','java.lang.Math.atanh', outtext)

	outtext=re.sub(r'\bsinf\b','java.lang.Math.sin', outtext)
	outtext=re.sub(r'\bcosf\b','java.lang.Math.cos', outtext)
	outtext=re.sub(r'\btanf\b','java.lang.Math.tan', outtext)
	outtext=re.sub(r'\basinf\b','java.lang.Math.asin', outtext)
	outtext=re.sub(r'\bacosf\b','java.lang.Math.acos', outtext)
	outtext=re.sub(r'\batanf\b','java.lang.Math.atan', outtext)

	outtext=re.sub(r'\bsinhf\b','java.lang.Math.sinh', outtext)
	outtext=re.sub(r'\bcoshf\b','java.lang.Math.cosh', outtext)
	outtext=re.sub(r'\btanhf\b','java.lang.Math.tanh', outtext)
	outtext=re.sub(r'\basinhf\b','java.lang.Math.asinh', outtext)
	outtext=re.sub(r'\bacoshf\b','java.lang.Math.acosh', outtext)
	outtext=re.sub(r'\batanhf\b','java.lang.Math.atanh', outtext)

	outtext=re.sub(r'\bsin\b','java.lang.Math.sin', outtext)
	outtext=re.sub(r'\bcos\b','java.lang.Math.cos', outtext)
	outtext=re.sub(r'\btan\b','java.lang.Math.tan', outtext)
	outtext=re.sub(r'\basin\b','java.lang.Math.asin', outtext)
	outtext=re.sub(r'\bacos\b','java.lang.Math.acos', outtext)
	outtext=re.sub(r'\batan\b','java.lang.Math.atan', outtext)

	outtext=re.sub(r'\bsinh\b','java.lang.Math.sinh', outtext)
	outtext=re.sub(r'\bcosh\b','java.lang.Math.cosh', outtext)
	outtext=re.sub(r'\btanh\b','java.lang.Math.tanh', outtext)
	outtext=re.sub(r'\basinh\b','java.lang.Math.asinh', outtext)
	outtext=re.sub(r'\bacosh\b','java.lang.Math.acosh', outtext)
	outtext=re.sub(r'\batanh\b','java.lang.Math.atanh', outtext)

	outtext=re.sub(r'\batan2\b','java.lang.Math.atan2', outtext)
	outtext=re.sub(r'\bcbrt\b','java.lang.Math.cbrt', outtext)
	outtext=re.sub(r'\bceil\b','java.lang.Math.ceil', outtext)
	outtext=re.sub(r'\bexp\b','java.lang.Math.exp', outtext)
	outtext=re.sub(r'\bfloor\b','java.lang.Math.floor', outtext)
	outtext=re.sub(r'\blog\b','java.lang.Math.log', outtext)
	outtext=re.sub(r'\blog10\b','java.lang.Math.log10', outtext)
	outtext=re.sub(r'\blog\b','java.lang.Math.log', outtext)
	outtext=re.sub(r'\bstx_max\b','java.lang.Math.max', outtext)
	outtext=re.sub(r'\bstx_min\b','java.lang.Math.min', outtext)
	outtext=re.sub(r'\bstx_rand\b','java.lang.Math.random', outtext)
	outtext=re.sub(r'\bround\b','java.lang.Math.round', outtext)
	outtext=re.sub(r'\bsqrt\b','java.lang.Math.sqrt', outtext)

	outtext=re.sub(r'\batan2f\b','java.lang.Math.atan2', outtext)
	outtext=re.sub(r'\bcbrtf\b','java.lang.Math.cbrt', outtext)
	outtext=re.sub(r'\bceilf\b','java.lang.Math.ceil', outtext)
	outtext=re.sub(r'\bexpf\b','java.lang.Math.exp', outtext)
	outtext=re.sub(r'\bfloorf\b','java.lang.Math.floor', outtext)
	outtext=re.sub(r'\blogf\b','java.lang.Math.log', outtext)
	outtext=re.sub(r'\blog10f\b','java.lang.Math.log10', outtext)
	outtext=re.sub(r'\blogf\b','java.lang.Math.log', outtext)
	outtext=re.sub(r'\broundf\b','java.lang.Math.round', outtext)
	outtext=re.sub(r'\bsqrtf\b','java.lang.Math.sqrt', outtext)

	outtext=re.sub(r'\batan2l\b','java.lang.Math.atan2', outtext)
	outtext=re.sub(r'\bcbrtl\b','java.lang.Math.cbrt', outtext)
	outtext=re.sub(r'\bceill\b','java.lang.Math.ceil', outtext)
	outtext=re.sub(r'\bexpl\b','java.lang.Math.exp', outtext)
	outtext=re.sub(r'\bfloorl\b','java.lang.Math.floor', outtext)
	outtext=re.sub(r'\blogl\b','java.lang.Math.log', outtext)
	outtext=re.sub(r'\blog10l\b','java.lang.Math.log10', outtext)
	outtext=re.sub(r'\blogl\b','java.lang.Math.log', outtext)
	outtext=re.sub(r'\broundl\b','java.lang.Math.round', outtext)
	outtext=re.sub(r'\bsqrtl\b','java.lang.Math.sqrt', outtext)

	outtext=re.sub(r'\*\s*this\b','this', outtext)

	outtext=re.sub(r'\bD3DXFROMWINEToDegree\b','toDegrees', outtext)
	outtext=re.sub(r'\bD3DXFROMWINEToRadian\b','toRadians', outtext)

	outtext=re.sub(r'\binline\s*\b',' ', outtext)
	outtext=re.sub(r'\bCONST\b','const', outtext)
	outtext=re.sub(r'\bFLOAT\b','float', outtext)
	outtext=re.sub(r'\bDOUBLE\b','double', outtext)

	outtext=re.sub(r'\bWORD\b','short', outtext)
	outtext=re.sub(r'\bDWORD\b','int', outtext)
	outtext=re.sub(r'\b__DWORD__\b','int', outtext)
	outtext=re.sub(r'\bUCHAR\b','char', outtext)
	outtext=re.sub(r'\bUSHORT\b','short', outtext)
	outtext=re.sub(r'\bUINT\b','int', outtext)
	outtext=re.sub(r'\bULONG\b','long', outtext)
	outtext=re.sub(r'\bCHAR\b','char', outtext)
	outtext=re.sub(r'\bSHORT\b','short', outtext)
	outtext=re.sub(r'\bINT\b','int', outtext)
	outtext=re.sub(r'\bLONG\b','long', outtext)
	outtext=re.sub(r'\bUBYTE\b','byte', outtext)
	outtext=re.sub(r'\bBYTE\b','byte', outtext)


	for i in classes:
		outtext=re.sub(r'\b'+i+r'\s*\:\s*\:','', outtext)

	output = open(filePath2, 'w')
	#for i in v:
	output.write(outtext)
	output.close()
	
def myfunh1(dummy, dirr, filess):
	for child in filess:
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile1(dirr+'/'+child)
	    elif '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile1(dirr+'/'+child)
	    elif '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile1(dirr+'/'+child)
	
def myfunh2(dummy, dirr, filess):
	for child in filess:
	    if '.h' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile2(dirr+'/'+child)
	    elif '.inl' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile2(dirr+'/'+child)
	    elif '.hxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile2(dirr+'/'+child)
	
def myfuncpp1(dummy, dirr, filess):
	for child in filess:
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile1(dirr+'/'+child)
	    elif '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile1(dirr+'/'+child)
	    elif '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile1(dirr+'/'+child)
	    elif '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile1(dirr+'/'+child)
	
def myfuncpp2(dummy, dirr, filess):
	for child in filess:
	    if '.cpp' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile2(dirr+'/'+child)
	    elif '.c' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile2(dirr+'/'+child)
	    elif '.cc' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile2(dirr+'/'+child)
	    elif '.cxx' == os.path.splitext(child)[1] and os.path.isfile(dirr+'/'+child):
		    replaceStringInFile2(dirr+'/'+child)

def load(file):
	ffile = os.path.abspath(file)
	f=open(ffile,'r')
	lines=f.readlines()
	f.close()
	return string.join(lines,'')

def main1():
	txt=load('../projects/projectsX/IndieLib_Static/IndieLib_Static.vcproj')
	p = re.compile(r"\bRelativePath\b\s*\=\s*\".*?\.cpp\"")
	l=p.findall(txt)
	#print l
	for i in l:
		p2 = re.compile(r"\bRelativePath\b\s*\=\s*\"(.*?)\.cpp\"")
		m=p2.match(i)
		if m:
			#print m.group(1)+'.cpp'
			fn=m.group(1)+'.cpp'
			fn=fn.replace('..\\..\\..\\IndieLib\\IndieLib\\Common\\','..\\IndieLib\\IndieLib\\Common\\')
			fn=fn.replace('\\','/')
			txt2=load(fn)
			p3 = re.compile(r'''\binclude\b\s+[\"\<]+.*?\n''')
			l3=p3.findall(txt2)
			print l3

def main():
	DATA = "Hey&, you* - what[7] _are_ _you doing_; here;"
	print re.findall(r"[\w]+", DATA)

hcpp=['../Libraries/STX', '../Libraries/MeshRenderer2', '../Libraries/Framework3', '../Libraries/FW3', '../Libraries/otl', '../Libraries/RadeonTerrainDemoLibrary', '../Libraries/IndieLib/IndieLib/Common','../Libraries/spine-2.3', '../Libraries/NSGUI', '../Libraries/Image3', '../Libraries/nova-0.15.0']
h=['../Libraries/tinyxml', '../Libraries/hlsl2glslfork']
f=['../Libraries/hlsl2glslfork/include/hlsl2glsl.h', '../Libraries/tinyxml/include/tinyxml/tinystr.h', '../Libraries/tinyxml/include/tinyxml/tinyxml.h']


# '../Libraries/c3dbool', '../Libraries/NatureSceneLibrary']
#, '../Samples']
#v=['../Samples/SimpleTexture_']
#v=['../Libraries/Framework3']

# split ;,[]\ \t\n
# cpp signatures
# 1: \;.*?\(.*?\).*?[=0].*?; 
# 2: stack { }

main()

if not os.path.exists('../../IndieLibJ'):
	osmakedirs('../../IndieLibJ')
for i in h:
	os.path.walk(i, myfunh1, 13)
for i in h:
	os.path.walk(i, myfunh2, 13)
for i in hcpp:
	os.path.walk(i, myfuncpp1, 13)
for i in hcpp:
	os.path.walk(i, myfuncpp2, 13)
for i in f:
	replaceStringInFile1(i)
	replaceStringInFile2(i)
print classes

