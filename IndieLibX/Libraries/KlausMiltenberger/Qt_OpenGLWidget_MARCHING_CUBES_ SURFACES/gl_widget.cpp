/*
 * gl_widget.cpp
 *  Last Changes: 06.05.2018
 *  Math. Code Reference: Paul Bourke , Marching-Cubes-Algorithm
 *  Author: manitoo Qt/OpenGL/C++ Coding
 *  QOpenGLWidget , QOpenGLFunctions Implementation
 */

#include "gl_widget.h"

//Constructor
GL_Widget::GL_Widget(GLbyte Resolution) : iDataSetSize(Resolution)
{
    qDebug() << "    [ GL_Widget        constructed | Main-TID: " << QThread::currentThreadId() << " | " << isWidgetType() << " ]";
    qDebug() << "    - inherits QObject " << inherits("QObject");
    qDebug() << "    - inherits QWidget " << inherits("QWidget");
    qDebug() << "    - parent   Widget  " << parentWidget();

    mca = new MC_ALG(Resolution);

    qDebug() << "      Memory Infos";
    qDebug() << "       - Size    GL_Widget :" << sizeof(*this) << " Bytes";
    qDebug() << "       - Size    MC_ALG    :" << sizeof(*mca)  << " Bytes\n";

    //GUI-GL Controls
    LEVEL_SET           = 1;
    PARTITION_MODE      = 1;
    POLYGON_MODE        = 1;
    PROJECTION_MODE     = 1;
    BLENDING_MODE       = 2;
    CULLING_MODE        = 2;

    //Approx. Stepsize
    fStepSize       = 1.0/iDataSetSize;

    //Rotation-Angles
    rotX =  0.0; rotY =  0.0; rotZ =  0.0;

    //Wheel Scaling
    currScaling     = 1.5;
    zoomFactor      = 0.7;

    //Timer Animation
    timer_Id = 0; t = 0.0; dt = 0.25;

    makeCurrent();
}

//Destructor
GL_Widget::~GL_Widget()
{
    qDebug() << "    [ GL_Widget         destructed ]";
}

// ---------------- [ Reimplemented Virtual GL-Functions ] ----------------

//Sets up the OpenGL resources and state
//Gets called once before the first time resizeGL() or paintGL() is called.
void GL_Widget::initializeGL()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    makeCurrent();
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);

    // ------------------------ [ Create Shaders ] ------------------------
    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);

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
            "normal = normalize(gl_NormalMatrix * gl_Normal);\n"
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

    program = new QOpenGLShaderProgram(this);
    program->addShader(vshader);
    program->addShader(fshader);
    program->bindAttributeLocation("vertex",0);

    program->link();
    program->bind();

    QVector3D   Color(1.0,1.0,1.0);
    QVector3D   LightColor1(1.0,0.3,0.0);
    QVector3D   LightColor2(0.5,1.0,0.0);
    QVector3D   LightColor3(0.0,0.6,1.0);
    QVector3D   MaterialColor(1.0,1.0,1.0);

    QVector3D   LightPos1(+10.0,+10.0,+10.0);
    QVector3D   LightPos2(-10.0,+10.0,+10.0);
    QVector3D   LightPos3(+10.0,-10.0,-10.0);

    colorLocation[0] = program->uniformLocation("Color");
    colorLocation[1] = program->uniformLocation("LightColor1");
    colorLocation[2] = program->uniformLocation("LightColor2");
    colorLocation[3] = program->uniformLocation("LightColor3");
    MaterialColor[0] = program->uniformLocation("MaterialColor");

    program->setUniformValue(colorLocation[0],Color);
    program->setUniformValue(colorLocation[1],LightColor1);
    program->setUniformValue(colorLocation[2],LightColor2);
    program->setUniformValue(colorLocation[3],LightColor3);
    program->setUniformValue(MaterialColor[0],MaterialColor);


    lightLocation[0] = program->uniformLocation("LightPos1");
    lightLocation[1] = program->uniformLocation("LightPos2");
    lightLocation[2] = program->uniformLocation("LightPos3");


    program->setUniformValue(lightLocation[0],LightPos1);
    program->setUniformValue(lightLocation[1],LightPos2);
    program->setUniformValue(lightLocation[2],LightPos3);

    cout << "\n  OpenGL Infos";
    cout << "\n   - GL_RENDERER     : " << glGetString(GL_RENDERER);
    cout << "\n   - GL_VERSION      : " << glGetString(GL_VERSION);
    cout << "\n   - GL_GLSL_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";

/*  Not Used !
    cout << "\n   GL-Buffer Infos";
    GLint *rBit = new GLint; glGetIntegerv(GL_RED_BITS,rBit);
    GLint *gBit = new GLint; glGetIntegerv(GL_GREEN_BITS,gBit);
    GLint *bBit = new GLint; glGetIntegerv(GL_BLUE_BITS,bBit);
    GLint *aBit = new GLint; glGetIntegerv(GL_ALPHA_BITS,aBit);
    GLint *dBit = new GLint; glGetIntegerv(GL_DEPTH_BITS,dBit);

    cout << "\n      - GL_RED_BITS     : " << setw(2) << *rBit << " Bits";
    cout << "\n      - GL_GREEN_BITS   : " << setw(2) << *gBit << " Bits";
    cout << "\n      - GL_BLUE_BITS    : " << setw(2) << *bBit << " Bits";
    cout << "\n      - GL_ALPHA_BITS   : " << setw(2) << *aBit << " Bits";
    cout << "\n      - GL_DEPTH_BITS   : " << setw(2) << *dBit << " Bits\n";

    cout << "\n   GL-Type Infos";
    cout << "\n      - Size GLbyte     : " << setw(2) << sizeof(GLbyte) << " Bytes";
    cout << "\n      - Size GLfloat    : " << setw(2) << sizeof(GLfloat) << " Bytes";
    cout << "\n      - Size GLdouble   : " << setw(2) << sizeof(GLdouble) << " Bytes\n";
*/

    // -------------------- [ Create Display Lists , Stored Server-Side ] --------------------
    qtime.restart();
    Dodecahedron = glGenLists(1);
    glNewList(Dodecahedron,GL_COMPILE);
        //Dodecahedron Faces);
        glBegin(GL_LINE_LOOP);
            for(size_t i = 0; i < sizeof(faces)/sizeof(faces[0]); i++)
            {
                glVertex3f(D[faces[i][0]][0]+0.5,D[faces[i][0]][1]+0.5,D[faces[i][0]][2]+0.5);
                glVertex3f(D[faces[i][1]][0]+0.5,D[faces[i][1]][1]+0.5,D[faces[i][1]][2]+0.5);
                glVertex3f(D[faces[i][2]][0]+0.5,D[faces[i][2]][1]+0.5,D[faces[i][2]][2]+0.5);
                glVertex3f(D[faces[i][3]][0]+0.5,D[faces[i][3]][1]+0.5,D[faces[i][3]][2]+0.5);
                glVertex3f(D[faces[i][4]][0]+0.5,D[faces[i][4]][1]+0.5,D[faces[i][4]][2]+0.5);
            }
        glEnd();
    glEndList();
    qDebug("       - DSP-List Dodecahedron | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_11 = glGenLists(1);    
    glNewList(Level_Set_11,GL_COMPILE);
       glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(1, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_11 | Time elapsed %6d ms", qtime.elapsed());

    qtime.start();
    Level_Set_12 = glGenLists(1);
    glNewList(Level_Set_12,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(1, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_12 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_21 = glGenLists(1);
    glNewList(Level_Set_21,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(2, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_21 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_22 = glGenLists(1);
    glNewList(Level_Set_22,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(2, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_22 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_31 = glGenLists(1);
    glNewList(Level_Set_31,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(3, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_31 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_32 = glGenLists(1);
    glNewList(Level_Set_32,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(3, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_32 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_41 = glGenLists(1);
    glNewList(Level_Set_41,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(4, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_41 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_42 = glGenLists(1);
    glNewList(Level_Set_42,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(4, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_42 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_51 = glGenLists(1);
    glNewList(Level_Set_51,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(5, 1);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_51 | Time elapsed %6d ms", qtime.elapsed());

    qtime.restart();
    Level_Set_52 = glGenLists(1);
    glNewList(Level_Set_52,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                mca->vMarchingCubes(5, 2);
        glEnd();
    glEndList();
    qDebug("       - DSP-List Level_Set_52 | Time elapsed %6d ms", qtime.elapsed());

    delete mca;
}


//Gets called whenever the Widget has been resized
//and when it is shown for the first time, all newly created Widgets get a resize event
void GL_Widget::resizeGL(GLsizei w, GLsizei h)
{
    switch( PROJECTION_MODE )
    {
        case 1: //Orthographic Projection
        {
            if (h == 0) h = 1;
            glMatrixMode(GL_PROJECTION);
            GLdouble nRange = 1.3;
            glLoadIdentity();
            if (w <= h) glOrtho(-nRange,nRange,-nRange*h/w,nRange*h/w,-nRange,nRange);
            else        glOrtho(-nRange*w/h,nRange*w/h,-nRange,nRange,-nRange,nRange);
            break;
        }
        case 2: //Perspective Projection
        {
           if (h == 0) h = 1;
           glMatrixMode(GL_PROJECTION);
           glLoadIdentity();
           GLdouble fAspect = (GLdouble)w/(GLdouble)h;
           glFrustum(-fAspect,fAspect, -1.0,1.0, 0.5,5000.0); //Viewing Volume , Clipping Planes
           glTranslated(0.0,0.0,-1.25);
           break;
        }
        default: break;
    }
}

void GL_Widget::paintGL()
{
    //Clear Buffers
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Projection Mode
    resizeGL(width(),height());

    //Restore ModelViewMatrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //Rotation-Matrices (Animation)
    glRotatef(rotX, 1.0, 0.0, 0.0);
    glRotatef(rotY, 0.0, 1.0, 0.0);
    glRotatef(rotZ, 0.0, 0.0, 1.0);

    //Non Moving Axes
    axes(1.2,1.2,1.2);

    //Wheel Scaling
    glScalef(currScaling,currScaling,currScaling);

    //Timer Animation
    GLfloat s = 0.5+pow(cos(0.05*t),2);
    glScalef(s,s,s);
    glRotatef(t,1,1,1);

    //Moving Axes
    axes(0.5,0.5,0.5);

    //ModelView-Transformation
    glTranslatef(-0.5, -0.5, -0.5);

    switch( POLYGON_MODE )
    {
        case 1: {glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;}
        case 2: {glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;}
        default: break;
    }

    switch( BLENDING_MODE )
    {
        case 1:
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_ONE,GL_ONE);
            //glBlendFunc(GL_SRC_ALPHA_SATURATE,GL_ONE);
            break;
        }
        case 2: {glDisable(GL_BLEND); break;}
        default: break;
    }

    switch( CULLING_MODE )
    {
        case 1: {glEnable(GL_CULL_FACE); glCullFace(GL_BACK); break;}
        case 2: {glDisable(GL_CULL_FACE); break;}
        default: break;
    }


    //Show Displaylists
    if (LEVEL_SET == 1 && PARTITION_MODE == 1)
    {
        glCallList(Level_Set_11);
        glCallList(Dodecahedron);
    }

    if (LEVEL_SET == 1 && PARTITION_MODE == 2)
    {
        glCallList(Level_Set_12);
        glCallList(Dodecahedron);
    }

    if (LEVEL_SET == 2 && PARTITION_MODE == 1)
    {
        glCallList(Level_Set_21);
    }

    if (LEVEL_SET == 2 && PARTITION_MODE == 2)
    {
        glCallList(Level_Set_22);
    }

    if (LEVEL_SET == 3 && PARTITION_MODE == 1)
    {
        glCallList(Level_Set_31);
    }

    if (LEVEL_SET == 3 && PARTITION_MODE == 2)
    {
        glCallList(Level_Set_32);
    }

    if (LEVEL_SET == 4 && PARTITION_MODE == 1)
    {
        glCallList(Level_Set_41);
    }

    if (LEVEL_SET == 4 && PARTITION_MODE == 2)
    {
        glCallList(Level_Set_42);
    }

    if (LEVEL_SET == 5 && PARTITION_MODE == 1)
    {
        glCallList(Level_Set_51);
    }

    if (LEVEL_SET == 5 && PARTITION_MODE == 2)
    {
        glCallList(Level_Set_52);
    }

    program->removeAllShaders();
}

// ------------------------------ [ Slot Functions ] ------------------------------
    void GL_Widget::Level_Set(GLbyte levs)	{LEVEL_SET      = levs; update();}
    void GL_Widget::Partition_Mode(GLbyte part)	{PARTITION_MODE = part; update();}

    void GL_Widget::animate(void)
    {
        if ( timer_Id == 0)  {timer_Id = startTimer(10);}
        else                 {killTimer(timer_Id); timer_Id = 0;}
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



// ----------------------------- [ Event  Handlers ] -----------------------------
    void GL_Widget::mousePressEvent(QMouseEvent *event) {lastPos = event->pos();}

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
    {
        if (event->button() != Qt::LeftButton )	return;
        if( timer_Id == 0)	{timer_Id = startTimer(10);}
        else                {killTimer(timer_Id); timer_Id = 0;}
    }

    void GL_Widget::wheelEvent(QWheelEvent *event)
    {   int numDegrees = event->delta() / 32;
        GLfloat numSteps = numDegrees / 50.0;
        zoom(pow(zoomFactor, numSteps));
    }

    void GL_Widget::timerEvent(QTimerEvent *event)
    {
        if (event->timerId() == timer_Id)   {t += dt; update();}
    }


void GL_Widget::zoom(const GLfloat zoomFactor) { currScaling *= zoomFactor; update(); }

void GL_Widget::axes(const GLfloat &x, const GLfloat &y, const GLfloat &z)
{
    glBegin(GL_LINES);
        glVertex3f(-x,0.0,0.0); glVertex3f(+x,0.0,0.0);
        glVertex3f(0.0,-y,0.0); glVertex3f(0.0,+y,0.0);
        glVertex3f(0.0,0.0,-z); glVertex3f(0.0,0.0,+z);
    glEnd();
}

