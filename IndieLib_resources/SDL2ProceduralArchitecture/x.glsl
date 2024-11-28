#include <SDL2/SDL.h>
#include "importgl.h"
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex Shader Source
const char* vertexShaderSource = R"(
#version 130
attribute vec3 aPos;
attribute vec3 aColor;
attribute vec3 aNormal;

varying vec3 vertexColor;
varying vec3 FragPos;
varying vec3 Normal;
varying vec4 ShadowCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform float time;

// Simple 3D noise function
float noise(vec3 p) {
    return fract(sin(dot(p, vec3(12.9898, 78.233, 45.164))) * 43758.5453);
}

// Inverse function for mat4
mat4 inverse(mat4 m) {
    float 
        a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
        a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
        a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
        a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3];

    float 
        b00 = a00 * a11 - a01 * a10,
        b01 = a00 * a12 - a02 * a10,
        b02 = a00 * a13 - a03 * a10,
        b03 = a01 * a12 - a02 * a11,
        b04 = a01 * a13 - a03 * a11,
        b05 = a02 * a13 - a03 * a12,
        b06 = a20 * a31 - a21 * a30,
        b07 = a20 * a32 - a22 * a30,
        b08 = a20 * a33 - a23 * a30,
        b09 = a21 * a32 - a22 * a31,
        b10 = a21 * a33 - a23 * a31,
        b11 = a22 * a33 - a23 * a32;

    float det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;
    if (det == 0.0) return mat4(0.0);

    float invDet = 1.0 / det;

    return mat4(
        (a11 * b11 - a12 * b10 + a13 * b09) * invDet,
        (-a01 * b11 + a02 * b10 - a03 * b09) * invDet,
        (a31 * b05 - a32 * b04 + a33 * b03) * invDet,
        (-a21 * b05 + a22 * b04 - a23 * b03) * invDet,
        (-a10 * b11 + a12 * b08 - a13 * b07) * invDet,
        (a00 * b11 - a02 * b08 + a03 * b07) * invDet,
        (-a30 * b05 + a32 * b02 - a33 * b01) * invDet,
        (a20 * b05 - a22 * b02 + a23 * b01) * invDet,
        (a10 * b10 - a11 * b08 + a13 * b06) * invDet,
        (-a00 * b10 + a01 * b08 - a03 * b06) * invDet,
        (a30 * b04 - a31 * b02 + a33 * b00) * invDet,
        (-a20 * b04 + a21 * b02 - a23 * b00) * invDet,
        (-a10 * b09 + a11 * b07 - a12 * b06) * invDet,
        (a00 * b09 - a01 * b07 + a02 * b06) * invDet,
        (-a30 * b03 + a31 * b01 - a32 * b00) * invDet,
        (a20 * b03 - a21 * b01 + a22 * b00) * invDet);
}

// Transpose function for mat4
mat4 transpose(mat4 m) {
    return mat4(
        m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]
    );
}

void main()
{
    vec3 modifiedPos = aPos;
    modifiedPos += normalize(aNormal) * noise(aPos * 10.0 + time) * 0.1; // Add noise-based displacement

    FragPos = vec3(model * vec4(modifiedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    ShadowCoord = lightSpaceMatrix * vec4(modifiedPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
    vertexColor = aColor;
}
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
#version 130
varying vec3 vertexColor;
varying vec3 FragPos;
varying vec3 Normal;
varying vec4 ShadowCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 shadowCoord)
{
    // Perform perspective divide
    vec3 projCoords = shadowCoord.xyz / shadowCoord.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from the shadow map
    float closestDepth = texture2D(shadowMap, projCoords.xy).r;
    // Get current depth value
    float currentDepth = projCoords.z;

    // Check if in shadow
    float shadow = currentDepth > closestDepth + 0.005 ? 0.5 : 1.0;

    return shadow;
}

void main()
{
    // Ambient
    vec3 ambient = 0.1 * ambientColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Shadow
    float shadow = ShadowCalculation(ShadowCoord);

    vec3 result = (ambient + shadow * (diffuse + specular)) * vertexColor;
    gl_FragColor = vec4(result, 1.0);
}
)";

// Function to compile a shader
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create SDL Window
    SDL_Window* window = SDL_CreateWindow("Procedural Architecture", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create OpenGL Context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize OpenGL using importgl.h
    if (!importglInit()) {
        std::cerr << "Failed to initialize OpenGL using importgl.h" << std::endl;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Compile Vertex and Fragment Shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Link Shaders to create a Program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Define vertices for a simple cube (building block)
    float vertices[] = {
        // positions          // colors           // normals
        -0.5f, -0.5f, -0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f,  0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.6f, 0.3f, 0.1f,  0.0f,  0.0f,  1.0f
    };

    unsigned int indices[] = {
        0, 1, 3,  1, 2, 3,  // back face
        4, 5, 7,  5, 6, 7,  // front face
        0, 1, 4,  1, 5, 4,  // bottom face
        2, 3, 6,  3, 7, 6,  // top face
        0, 3, 4,  3, 7, 4,  // left face
        1, 2, 5,  2, 6, 5   // right face
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    // Set up shadow map framebuffer
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Configure matrices
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    float near_plane = 1.0f, far_plane = 7.5f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    lightView = glm::lookAt(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;

    // Set up shader uniforms for lighting
    glUseProgram(shaderProgram);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"), 1.2f, 1.0f, 2.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"), 0.0f, 0.0f, 3.0f);
    glUniform3f(glGetUniformLocation(shaderProgram, "ambientColor"), 0.2f, 0.2f, 0.2f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Render to depth map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        // Render scene from light's perspective (depth-only)
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render scene
        glViewport(0, 0, 800, 600);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Activate shader program
        glUseProgram(shaderProgram);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), (float)SDL_GetTicks() / 1000.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 0);

        // Bind VAO and draw elements
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(window);
    }

    // Clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteTextures(1, &depthMap);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

