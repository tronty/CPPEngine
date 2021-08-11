/*
 * gl_widget.cpp
 *  Last Change: 06.05.2018
 *  Math. Code Reference: Paul Bourke , Marching Cubes Algorithm
 *  Author: manitoo Qt/C++ , QGLWidget Class (deprecated!)
 */

#include "gl_widget.h"

//Constructor
GL_Widget::GL_Widget(QWidget *parent, GLuint Resolution) : QOpenGLWidget(parent)
{
    qDebug() << "     [ GL_Widget constructed ]";

    cout << "\n   Memory Infos";
    cout << "\n      - Size    GL_Widget :" << setw(10) << sizeof(*this) << " Bytes";
    cout << "\n      - Algom. Resolution :" << setw(10) << Resolution    << " Cubes/Tetrahedrons\n";

    //GL Actions
    LEVEL_SET           = 1;
    PARTITION_MODE      = 1;
    POLYGON_MODE        = 1;
    PROJECTION_MODE     = 1;
    BLENDING_MODE       = 2;
    CULLING_MODE        = 2;

    //Computational Variables
    iDataSetSize    = Resolution;
    fStepSize       = 1.0/iDataSetSize;

    //Rotation-Angles
    rotX = 0.0f; rotY = 0.0f; rotZ = 0.0f;

    //Wheel Scaling
    currScaling  = 2.0f; zoomFactor = 0.7f;

    //Timer Animation
    timer_Id = 0;    t = 0.0f;
}

//Destructor
GL_Widget::~GL_Widget()
{
    qDebug() << "     [ GL_Widget destructed ]";
}


// ---------------- [ Reimplemented Virtual GL-Functions ] ----------------
void GL_Widget::initializeGL()
{
    //Rendering Context
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    makeCurrent();
    initializeOpenGLFunctions();


    glEnable(GL_DEPTH_TEST);

    // -------------------- [ Create Shaders ] --------------------
    QGLShader *vshader = new QGLShader(QGLShader::Vertex, this);
    QGLShader *fshader = new QGLShader(QGLShader::Fragment, this);

    const char *vshadersrc =
        "#version 120\n"
        "uniform vec3 LightPos1;\n"
        "uniform vec3 LightPos2;\n"
        "uniform vec3 LightPos3;\n"
        "uniform vec3 LightColor1;\n"
        "uniform vec3 LightColor2;\n"
        "uniform vec3 LightColor3;\n"

        "varying vec3 LightColor;\n"

        "void main(void)\n"
        "{\n"
            "vec3 normal;\n"
            "vec3 pos;\n"

            "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
          //"normal = normalize(gl_NormalMatrix * gl_Normal);\n"
            "normal = gl_Normal;\n"
            "pos = vec3(gl_ModelViewMatrix * gl_Vertex);\n"

            "vec3 toLight1 = LightPos1-pos;\n"
            "vec3 toLight2 = LightPos2-pos;\n"
            "vec3 toLight3 = LightPos3-pos;\n"
            "LightColor  = max(dot(normalize(toLight1),normal),0.0) * LightColor1;\n"
            "LightColor += max(dot(normalize(toLight2),normal),0.0) * LightColor2;\n"
            "LightColor += max(dot(normalize(toLight3),normal),0.0) * LightColor3;\n"
        "}\n";

    const char *fshadersrc =
        "#version 120\n"
        "varying vec3 LightColor;\n"
        "uniform vec3 Color;\n"

        "void main(void)\n"
        "{\n"
          "gl_FragColor = vec4(Color * LightColor,1.0);\n"
        "}\n";

    // ----------- [ Compile & Link & Bind Shaders ] -----------
    vshader->compileSourceCode(vshadersrc);
    fshader->compileSourceCode(fshadersrc);

    program = new QGLShaderProgram(this);
    program->addShader(vshader);
    program->addShader(fshader);

    program->link();
    program->bind();

    QVector3D   Color(1.0,1.0,1.0);
    QVector3D   LightColor1(1.0,0.5,0.0);
    QVector3D   LightColor2(0.5,1.0,0.0);
    QVector3D   LightColor3(0.0,0.5,1.0);

    QVector3D   LightPos1(+10.0,+10.0,+10.0);
    QVector3D   LightPos2(-10.0,+10.0,-10.0);
    QVector3D   LightPos3(-10.0,+10.0,-10.0);

    colorLocation[0] = program->uniformLocation("Color");
    colorLocation[1] = program->uniformLocation("LightColor1");
    colorLocation[2] = program->uniformLocation("LightColor2");
    colorLocation[3] = program->uniformLocation("LightColor3");

    program->setUniformValue(colorLocation[0],Color);
    program->setUniformValue(colorLocation[1],LightColor1);
    program->setUniformValue(colorLocation[2],LightColor2);
    program->setUniformValue(colorLocation[3],LightColor3);

    lightLocation[0] = program->uniformLocation("LightPos1");
    lightLocation[1] = program->uniformLocation("LightPos2");
    lightLocation[2] = program->uniformLocation("LightPos3");


    program->setUniformValue(lightLocation[0],LightPos1);
    program->setUniformValue(lightLocation[1],LightPos2);
    program->setUniformValue(lightLocation[2],LightPos3);

        cout << "\n   OpenGL Infos";
        cout << "\n      - GL_GL_RENDERER  : " << glGetString(GL_RENDERER);
        cout << "\n      - GL_GL_VERSION   : " << glGetString(GL_VERSION);
        cout << "\n      - GL_GLSL_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

    QTime qtime;

    /* Resonant Cavity Display Lists */
    qtime.start();
    Level_Set_11 = glGenLists(1);
    glNewList(Level_Set_11,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(1, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Resonant Cavity 11 | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_12 = glGenLists(1);
    glNewList(Level_Set_12,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(1, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Resonant Cavity 12 | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_21 = glGenLists(1);
    glNewList(Level_Set_21,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(2, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Resonant Cavity 21 | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_22 = glGenLists(1);
    glNewList(Level_Set_22,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(2, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Resonant Cavity 22 | Time elapsed %6d ms", qtime.elapsed());

    /* Hydrogen States Display Lists */
    qtime.start();
    Level_Set_31 = glGenLists(1);
    glNewList(Level_Set_31,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(3, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Hydrogen States 31 | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_32 = glGenLists(1);
    glNewList(Level_Set_32,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(3, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Hydrogen States 32 | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_41 = glGenLists(1);
    glNewList(Level_Set_41,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(4, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Hydrogen States 41 | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_42 = glGenLists(1);
    glNewList(Level_Set_42,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(4, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Hydrogen States 42 | Time elapsed %6d ms", qtime.elapsed());
}

void GL_Widget::resizeGL(GLsizei w, GLsizei h)
{
    switch( PROJECTION_MODE )
    {
        case 1: //Orthographic Projection
        {
            if (h == 0) h = 1;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            GLdouble nRange = 1.4;
            if (w <= h) glOrtho(-nRange,+nRange,+nRange*h/w,+nRange*h/w,-nRange,+nRange);
            else        glOrtho(-nRange*w/h,+nRange*w/h,-nRange,+nRange,-nRange,+nRange);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            break;
        }
        case 2: //Perspective Projection
        {
           if (h == 0) h = 1;
           glMatrixMode(GL_PROJECTION);
           glLoadIdentity();           
           GLdouble fAspect = (GLdouble)w/(GLdouble)h;
           struct Frustum
           {
               Frustum(GLdouble Left,GLdouble Right,GLdouble Bottom,GLdouble Top,GLdouble Near,GLdouble Far)
                : left(Left), right(Right), bottom(Bottom), top(Top), near(Near), far(Far) { }

               GLdouble left, right, bottom, top, near, far;
           };
           Frustum fr(-fAspect,fAspect, -1.0,1.0, 0.25,100.0);
           glFrustum(fr.left,fr.right,fr.bottom,fr.top,fr.near,fr.far); //Viewing Volume , Clipping Planes
           glTranslatef(0.0,0.0,-1.25);
           glMatrixMode(GL_MODELVIEW);
           glLoadIdentity();
           break;
        }
    }
}

void GL_Widget::paintGL()
{
    //Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Projection Mode
    resizeGL(width(),height());

    //Restore ModelViewMatrix
    glLoadIdentity();

    //Non Moving Axes
    axes(1.5,1.5,1.5);

    //Animation Rotation-Matrices
    glRotatef(rotX, 1.0, 0.0, 0.0);
    glRotatef(rotY, 0.0, 1.0, 0.0);
    glRotatef(rotZ, 0.0, 0.0, 1.0);

    //Wheel Scaling
    glScalef(currScaling,currScaling,currScaling);

    //Timer Animation
    GLfloat s = 0.5+pow(cos(0.05*t),2);
    glScalef(s,s,s);
    glRotatef(t,1,1,1);

    //Moving Axes
    axes(0.75,0.75,0.75);

    //ModelView-Transformation
    glTranslatef(-0.5, -0.5, -0.5);

    switch( POLYGON_MODE )
    {
        case 1: {glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);  break;}
        case 2: {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;}
        default: break;
    }

    switch( BLENDING_MODE )
    {
        case 1: {glEnable(GL_BLEND); glBlendFunc(GL_ONE,GL_ONE); break;}
        case 2: {glDisable(GL_BLEND); break;}
        default: break;
    }

    switch( CULLING_MODE )
    {
        case 1: {glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;}
        case 2: {glDisable(GL_CULL_FACE); break;}
        default: break;
    }

    //Resonant Cavity Displaylists
    if (LEVEL_SET == 1 && PARTITION_MODE == 1) {glCallList(Level_Set_11);}
    if (LEVEL_SET == 1 && PARTITION_MODE == 2) {glCallList(Level_Set_12);}
    if (LEVEL_SET == 2 && PARTITION_MODE == 1) {glCallList(Level_Set_21);}
    if (LEVEL_SET == 2 && PARTITION_MODE == 2) {glCallList(Level_Set_22);}

    //Hydrogen States Displaylists
    if (LEVEL_SET == 3 && PARTITION_MODE == 1) {glCallList(Level_Set_31);}
    if (LEVEL_SET == 3 && PARTITION_MODE == 2) {glCallList(Level_Set_32);}
    if (LEVEL_SET == 4 && PARTITION_MODE == 1) {glCallList(Level_Set_41);}
    if (LEVEL_SET == 4 && PARTITION_MODE == 2) {glCallList(Level_Set_42);}

    program->removeAllShaders();
 }

// ------------------------------- [ Slot Functions ] -------------------------------
    void GL_Widget::Level_Set(GLuint levs)	{LEVEL_SET          = levs; update();}
    void GL_Widget::Partition_Mode(GLuint part)	{PARTITION_MODE = part; update();}

    void GL_Widget::animate(void)
    {
        if ( timer_Id == 0)  {timer_Id = startTimer(10);}
        else                 {killTimer(timer_Id);timer_Id = 0;}
        update();
    }

    void GL_Widget::shading(void)
    {
        if(POLYGON_MODE == 1)   {POLYGON_MODE = 2; update();}
        else                    {POLYGON_MODE = 1; update();}
    }

    void GL_Widget::blending(void)
    {
        if(BLENDING_MODE == 1)  {BLENDING_MODE = 2; update();}
        else                    {BLENDING_MODE = 1; update();}
    }

    void GL_Widget::culling(void)
    {
        if(CULLING_MODE == 1)   {CULLING_MODE = 2; update();}
        else                    {CULLING_MODE = 1; update();}
    }

    void GL_Widget::projection(void)
    {
        if(PROJECTION_MODE == 1)    {PROJECTION_MODE = 2; update();}
        else                        {PROJECTION_MODE = 1; update();}
    }


// --------------------------- [ Event Handlers ] ---------------------------
void GL_Widget::mousePressEvent(QMouseEvent *event) { lastPos = event->pos();}

void GL_Widget::mouseMoveEvent(QMouseEvent *event)
{GLfloat dx = GLfloat(event->x()-lastPos.x())/width();
 GLfloat dy = GLfloat(event->y()-lastPos.y())/height();

 if (event->buttons() & Qt::LeftButton)
 {
     rotX += 180*dy;
     rotY += 180*dx;
     update();
 }
 else
 if (event->buttons() & Qt::RightButton)
 {
     rotX += 180*dy;
     rotZ += 180*dx;
     update();
 }
 lastPos = event->pos();
}

void GL_Widget::mouseDoubleClickEvent(QMouseEvent *event)
{   if (event->button() != Qt::LeftButton )	return;
    if( timer_Id == 0)	{timer_Id = startTimer(10);}
    else 		{killTimer(timer_Id); timer_Id = 0;}
}

void GL_Widget::wheelEvent(QWheelEvent *event)
{   int numDegrees = event->delta()/32;
    double numSteps = numDegrees/50.0;
    zoom(pow(zoomFactor, numSteps));
}

void GL_Widget::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == timer_Id )
    {
        t += 0.25;
        update();
    }
}

void GL_Widget::zoom(double zoomFactor) { currScaling *= zoomFactor; update(); }

GLvoid GL_Widget::vMarchingCubes(GLint levelset, GLint partition)
{
    GLfloat fTv;    //Targetvalue
    switch ( levelset )
    {
        case 1: {fSample = &GL_Widget::fSample1; fTv = 0.03; break;}
        case 2: {fSample = &GL_Widget::fSample2; fTv = 0.03; break;}
        case 3: {fSample = &GL_Widget::fSample3; fTv = 0.01; break;}
        case 4: {fSample = &GL_Widget::fSample4; fTv = 0.30; break;}
        default:    break;
    }

     switch ( partition )
    {
        case 1: {vMarchCube = &GL_Widget::vMarchCube1; break;}
        case 2: {vMarchCube = &GL_Widget::vMarchCube2; break;}
        default:    break;
    }


    for(GLuint iX = 0; iX < iDataSetSize; iX++)
    for(GLuint iY = 0; iY < iDataSetSize; iY++)
    for(GLuint iZ = 0; iZ < iDataSetSize; iZ++)
    {
        (this->*vMarchCube)(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize, fTv);
    }
}


//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
GLfloat GL_Widget::fGetOffset(const GLfloat &fValue1, const GLfloat &fValue2, const GLfloat &fValueDesired)
{
        GLdouble fDelta = fValue2 - fValue1;

        if(fDelta == 0.0)
        {
                return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
}

GLvoid GL_Widget::vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource)
{
        GLfloat fOldLength;
        GLfloat fScale;

        fOldLength = sqrtf( (rfVectorSource.fX * rfVectorSource.fX) +
                            (rfVectorSource.fY * rfVectorSource.fY) +
                            (rfVectorSource.fZ * rfVectorSource.fZ) );

        if(fOldLength == 0.0)
        {
                rfVectorResult.fX = rfVectorSource.fX;
                rfVectorResult.fY = rfVectorSource.fY;
                rfVectorResult.fZ = rfVectorSource.fZ;
        }
        else
        {
                fScale = 1.0/fOldLength;
                rfVectorResult.fX = rfVectorSource.fX*fScale;
                rfVectorResult.fY = rfVectorSource.fY*fScale;
                rfVectorResult.fZ = rfVectorSource.fZ*fScale;
        }
}

void GL_Widget::axes(const GLfloat &x, const GLfloat &y, const GLfloat &z)
{
    glLineWidth(1);
    glBegin(GL_LINES);
        glVertex3f(-x,0.0,0.0); glVertex3f(+x,0.0,0.0);
        glVertex3f(0.0,-y,0.0); glVertex3f(0.0,+y,0.0);
        glVertex3f(0.0,0.0,-z); glVertex3f(0.0,0.0,+z);
    glEnd();
}

/* ------------------------ Sample Functions ------------------------ */

//fSample1 Level-Set of Spherical Resonant Cavity
GLfloat GL_Widget::fSample1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 5.0, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);

    //Polar Coordinates
    GLdouble    r          = sqrt(dX*dX+dY*dY+dZ*dZ),
                theta      = acos(dZ/r),
                phi        = atan(dY/dX);

    //Solution Spherical Resonant Cavity , Boundary Value Problem
    return abs(sph_bessel(1,4.49341*r)*spherical_harmonic(1,1,theta,phi));
}

//fSample2 Level-Set of Spherical Resonant Cavity
GLfloat GL_Widget::fSample2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 5.0, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);

    //Polar Coordinates
    GLdouble    r          = sqrt(dX*dX+dY*dY+dZ*dZ),
                theta      = acos(dZ/r),
                phi        = atan(dY/dX);

    //Solution Spherical Resonant Cavity , Boundary Value Problem
    return abs(sph_bessel(2,5.76346*r)*spherical_harmonic(2,1,theta,phi));
}

//fSample3 Hydrogen State
GLfloat GL_Widget::fSample3(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 32.0, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);

    //Polar Coordinates
    GLdouble    r          = sqrt(dX*dX+dY*dY+dZ*dZ),
                theta      = acos(dZ/r),
                phi        = atan(dY/dX);

    GLint   n = 2, l = 1, m = 0;

    //Solution Hydrogen State
    return pow(abs(exp(-r/2)*pow(r,l)*laguerre(n+l,2*l+1,r)*spherical_harmonic(l,m,theta,phi)),2);
}

//fSample4 Hydrogen State
GLfloat GL_Widget::fSample4(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 24.0, dX = fscale*(0.5-fX),  dY = fscale*(0.5-fY),  dZ = fscale*(0.5-fZ);

    //Polar Coordinates
    GLdouble    r          = sqrt(dX*dX+dY*dY+dZ*dZ),
                theta      = acos(dZ/r),
                phi        = atan(dY/dX);

    GLint   n = 3, l= 2, m = 0;

    //Solution Hydrogen State
    return pow(abs(exp(-r/2)*pow(r,l)*laguerre(n+l,2*l+1,r)*spherical_harmonic(l,m,theta,phi)),2);
}


//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
GLvoid GL_Widget::vGetNormal(GLvector &rfNormal, const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
        rfNormal.fX = (this->*fSample)(fX-0.01, fY, fZ) - (this->*fSample)(fX+0.01, fY, fZ);
        rfNormal.fY = (this->*fSample)(fX, fY-0.01, fZ) - (this->*fSample)(fX, fY+0.01, fZ);
        rfNormal.fZ = (this->*fSample)(fX, fY, fZ-0.01) - (this->*fSample)(fX, fY, fZ+0.01);
        vNormalizeVector(rfNormal, rfNormal);
}

//vMarchCube1 performs the Marching Cubes algorithm on a single cube
GLvoid GL_Widget::vMarchCube1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
        GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        GLfloat fOffset;
        GLfloat afCubeValue[8];
        GLvector asEdgeVertex[12];
        GLvector asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = (this->*fSample)(fX + a2fVertexOffset[iVertex][0]*fScale,fY + a2fVertexOffset[iVertex][1]*fScale,fZ + a2fVertexOffset[iVertex][2]*fScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= fTv)     iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<iEdge))
            {
                fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ],afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTv);

                asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

                vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
            }
}


        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
            if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

                glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
}

//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
GLvoid GL_Widget::vMarchCube2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
        GLint iVertex, iTetrahedron, iVertexInACube;
        GLvector asCubePosition[8];
        GLfloat  afCubeValue[8];
        GLvector asTetrahedronPosition[4];
        GLfloat  afTetrahedronValue[4];

        //Make a local copy of the cube's corner positions
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                asCubePosition[iVertex].fX = fX + a2fVertexOffset[iVertex][0]*fScale;
                asCubePosition[iVertex].fY = fY + a2fVertexOffset[iVertex][1]*fScale;
                asCubePosition[iVertex].fZ = fZ + a2fVertexOffset[iVertex][2]*fScale;
        }

        //Make a local copy of the cube's corner values
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = (this->*fSample)(asCubePosition[iVertex].fX,asCubePosition[iVertex].fY,asCubePosition[iVertex].fZ);
        }

        for(iTetrahedron = 0; iTetrahedron < 6; iTetrahedron++)
        {
            for(iVertex = 0; iVertex < 4; iVertex++)
            {
                iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
                asTetrahedronPosition[iVertex].fX = asCubePosition[iVertexInACube].fX;
                asTetrahedronPosition[iVertex].fY = asCubePosition[iVertexInACube].fY;
                asTetrahedronPosition[iVertex].fZ = asCubePosition[iVertexInACube].fZ;
                afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
            }
            vMarchTetrahedron(asTetrahedronPosition, afTetrahedronValue, fTv);
        }
}

//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
GLvoid GL_Widget::vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue, GLfloat fTv)
{
        GLint       iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
        GLfloat     fOffset, fInvOffset;
        GLvector    asEdgeVertex[6];
        GLvector    asEdgeNorm[6];

        //Find which vertices are inside of the surface and which are outside
        for(iVertex = 0; iVertex < 4; iVertex++)
        {
                if(pafTetrahedronValue[iVertex] <= fTv)     iFlagIndex |= 1<<iVertex;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiTetrahedronEdgeFlags[iFlagIndex];

        //If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }
        //Find the point of intersection of the surface with each edge
        // Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 6; iEdge++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<iEdge))
            {
                iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
                iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
                fOffset = fGetOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], fTv);
                fInvOffset = 1.0 - fOffset;

                asEdgeVertex[iEdge].fX = fInvOffset*pasTetrahedronPosition[iVert0].fX  +  fOffset*pasTetrahedronPosition[iVert1].fX;
                asEdgeVertex[iEdge].fY = fInvOffset*pasTetrahedronPosition[iVert0].fY  +  fOffset*pasTetrahedronPosition[iVert1].fY;
                asEdgeVertex[iEdge].fZ = fInvOffset*pasTetrahedronPosition[iVert0].fZ  +  fOffset*pasTetrahedronPosition[iVert1].fZ;

                vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
            }
        }
        //Draw the triangles that were found.  There can be up to 2 per tetrahedron
        for(iTriangle = 0; iTriangle < 2; iTriangle++)
        {
            if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)    break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+iCorner];

                glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
}


