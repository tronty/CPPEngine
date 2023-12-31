#include <pez.h>
#include <glew.h>
#include <glsw.h>
#include <vectormath.h>

static void CreateIcosahedron();
static void LoadEffect();

typedef struct {
    GLuint Projection;
    GLuint Modelview;
    GLuint NormalMatrix;
    GLuint LightPosition;
    GLuint AmbientMaterial;
    GLuint DiffuseMaterial;
    GLuint TessLevelInner;
    GLuint TessLevelOuter;
} ShaderUniforms;

static GLsizei IndexCount;
static const GLuint PositionSlot = 0;
static GLuint ProgramHandle;
static Matrix4 ProjectionMatrix;
static Matrix4 ModelviewMatrix;
static Matrix3 NormalMatrix;
static ShaderUniforms Uniforms;
static float TessLevelInner;
static float TessLevelOuter;

void PezRender(GLuint fbo)
{
    glUniform1f(Uniforms.TessLevelInner, TessLevelInner);
    glUniform1f(Uniforms.TessLevelOuter, TessLevelOuter);
    
    Vector4 lightPosition = V4MakeFromElems(0.25, 0.25, 1, 0);
    glUniform3fv(Uniforms.LightPosition, 1, &lightPosition.x);
    
    glUniformMatrix4fv(Uniforms.Projection, 1, 0, &ProjectionMatrix.col0.x);
    glUniformMatrix4fv(Uniforms.Modelview, 1, 0, &ModelviewMatrix.col0.x);

    Matrix3 nm = M3Transpose(NormalMatrix);
    float packed[9] = { nm.col0.x, nm.col1.x, nm.col2.x,
                        nm.col0.y, nm.col1.y, nm.col2.y,
                        nm.col0.z, nm.col1.z, nm.col2.z };
    glUniformMatrix3fv(Uniforms.NormalMatrix, 1, 0, &packed[0]);

    // Render the scene:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glUniform3f(Uniforms.AmbientMaterial, 0.04f, 0.04f, 0.04f);
    glUniform3f(Uniforms.DiffuseMaterial, 0, 0.75, 0.75);
    glDrawElements(GL_PATCHES, IndexCount, GL_UNSIGNED_INT, 0);
}

const char* PezInitialize(int width, int height)
{
    TessLevelInner = 3;
    TessLevelOuter = 2;

    CreateIcosahedron();
    LoadEffect();

    Uniforms.Projection = glGetUniformLocation(ProgramHandle, "Projection");
    Uniforms.Modelview = glGetUniformLocation(ProgramHandle, "Modelview");
    Uniforms.NormalMatrix = glGetUniformLocation(ProgramHandle, "NormalMatrix");
    Uniforms.LightPosition = glGetUniformLocation(ProgramHandle, "LightPosition");
    Uniforms.AmbientMaterial = glGetUniformLocation(ProgramHandle, "AmbientMaterial");
    Uniforms.DiffuseMaterial = glGetUniformLocation(ProgramHandle, "DiffuseMaterial");
    Uniforms.TessLevelInner = glGetUniformLocation(ProgramHandle, "TessLevelInner");
    Uniforms.TessLevelOuter = glGetUniformLocation(ProgramHandle, "TessLevelOuter");

    // Set up the projection matrix:
    const float HalfWidth = 0.6f;
    const float HalfHeight = HalfWidth * PEZ_VIEWPORT_HEIGHT / PEZ_VIEWPORT_WIDTH;
    ProjectionMatrix = M4MakeFrustum(-HalfWidth, +HalfWidth, -HalfHeight, +HalfHeight, 5, 150);

    // Initialize various state:
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.7f, 0.6f, 0.5f, 1.0f);

    return "Tessellation Demo";
}

static void CreateIcosahedron()
{
    const int Faces[] = {
        2, 1, 0,
        3, 2, 0,
        4, 3, 0,
        5, 4, 0,
        1, 5, 0,

        11, 6,  7,
        11, 7,  8,
        11, 8,  9,
        11, 9,  10,
        11, 10, 6,

        1, 2, 6,
        2, 3, 7,
        3, 4, 8,
        4, 5, 9,
        5, 1, 10,

        2,  7, 6,
        3,  8, 7,
        4,  9, 8,
        5, 10, 9,
        1, 6, 10 };

    const float Verts[] = {
         0.000f,  0.000f,  1.000f,
         0.894f,  0.000f,  0.447f,
         0.276f,  0.851f,  0.447f,
        -0.724f,  0.526f,  0.447f,
        -0.724f, -0.526f,  0.447f,
         0.276f, -0.851f,  0.447f,
         0.724f,  0.526f, -0.447f,
        -0.276f,  0.851f, -0.447f,
        -0.894f,  0.000f, -0.447f,
        -0.276f, -0.851f, -0.447f,
         0.724f, -0.526f, -0.447f,
         0.000f,  0.000f, -1.000f };

    IndexCount = sizeof(Faces) / sizeof(Faces[0]);

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO for positions:
    GLuint positions;
    GLsizei stride = 3 * sizeof(float);
    glGenBuffers(1, &positions);
    glBindBuffer(GL_ARRAY_BUFFER, positions);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Verts), Verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 3, GL_FLOAT, GL_FALSE, stride, 0);

    // Create the VBO for indices:
    GLuint indices;
    glGenBuffers(1, &indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW);
}

static void LoadEffect()
{
    GLint compileSuccess, linkSuccess;
    GLchar compilerSpew[256];

    glswInit();
    glswSetPath("../", ".glsl");
    glswAddDirectiveToken("*", "#version 400");

    const char* vsSource = glswGetShader("Geodesic.Vertex");
    const char* tcsSource = glswGetShader("Geodesic.TessControl");
    const char* tesSource = glswGetShader("Geodesic.TessEval");
    const char* gsSource = glswGetShader("Geodesic.Geometry");
    const char* fsSource = glswGetShader("Geodesic.Fragment");
    const char* msg = "Can't find %s shader.  Does '../BicubicPath.glsl' exist?\n";
    PezCheckCondition(vsSource != 0, msg, "vertex");
    PezCheckCondition(tcsSource != 0, msg, "tess control");
    PezCheckCondition(tesSource != 0, msg, "tess eval");
    PezCheckCondition(gsSource != 0, msg, "geometry");
    PezCheckCondition(fsSource != 0, msg, "fragment");

    GLuint vsHandle = glCreateShader(GL_VERTEX_SHADER);
    GLuint tcsHandle = glCreateShader(GL_TESS_CONTROL_SHADER);
    GLuint tesHandle = glCreateShader(GL_TESS_EVALUATION_SHADER);
    GLuint gsHandle = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vsHandle, 1, &vsSource, 0);
    glCompileShader(vsHandle);
    glGetShaderiv(vsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(vsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Vertex Shader Errors:\n%s", compilerSpew);

    glShaderSource(tcsHandle, 1, &tcsSource, 0);
    glCompileShader(tcsHandle);
    glGetShaderiv(tcsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(tcsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Tess Control Shader Errors:\n%s", compilerSpew);

    glShaderSource(tesHandle, 1, &tesSource, 0);
    glCompileShader(tesHandle);
    glGetShaderiv(tesHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(tesHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Tess Eval Shader Errors:\n%s", compilerSpew);

    glShaderSource(gsHandle, 1, &gsSource, 0);
    glCompileShader(gsHandle);
    glGetShaderiv(gsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(gsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Geometry Shader Errors:\n%s", compilerSpew);

    glShaderSource(fsHandle, 1, &fsSource, 0);
    glCompileShader(fsHandle);
    glGetShaderiv(fsHandle, GL_COMPILE_STATUS, &compileSuccess);
    glGetShaderInfoLog(fsHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(compileSuccess, "Fragment Shader Errors:\n%s", compilerSpew);

    ProgramHandle = glCreateProgram();
    glAttachShader(ProgramHandle, vsHandle);
    glAttachShader(ProgramHandle, tcsHandle);
    glAttachShader(ProgramHandle, tesHandle);
    glAttachShader(ProgramHandle, gsHandle);
    glAttachShader(ProgramHandle, fsHandle);
    glBindAttribLocation(ProgramHandle, PositionSlot, "Position");
    glLinkProgram(ProgramHandle);
    glGetProgramiv(ProgramHandle, GL_LINK_STATUS, &linkSuccess);
    glGetProgramInfoLog(ProgramHandle, sizeof(compilerSpew), 0, compilerSpew);
    PezCheckCondition(linkSuccess, "Shader Link Errors:\n%s", compilerSpew);

    glUseProgram(ProgramHandle);
}

void PezUpdate(unsigned int elapsedMicroseconds)
{
    const float RadiansPerMicrosecond = 0.0000005f;
    static float Theta = 0;
    Theta += elapsedMicroseconds * RadiansPerMicrosecond;
    Matrix4 rotation = M4MakeRotationX(Theta);
    Point3 eyePosition = P3MakeFromElems(0, 0, -10);
    Point3 targetPosition = P3MakeFromElems(0, 0, 0);
    Vector3 upVector = V3MakeFromElems(0, 1, 0);
    Matrix4 lookAt = M4MakeLookAt(eyePosition, targetPosition, upVector);
    ModelviewMatrix = M4Mul(lookAt, rotation);
    NormalMatrix = M4GetUpper3x3(ModelviewMatrix);

    const int VK_LEFT = 0x25;
    const int VK_UP = 0x26;
    const int VK_RIGHT = 0x27;
    const int VK_DOWN = 0x28;

    if (PezIsPressing(VK_RIGHT)) TessLevelInner++;
    if (PezIsPressing(VK_LEFT))  TessLevelInner = TessLevelInner > 1 ? TessLevelInner - 1 : 1;
    if (PezIsPressing(VK_UP))    TessLevelOuter++;
    if (PezIsPressing(VK_DOWN))  TessLevelOuter = TessLevelOuter > 1 ? TessLevelOuter - 1 : 1;
}

void PezHandleMouse(int x, int y, int action)
{
}
