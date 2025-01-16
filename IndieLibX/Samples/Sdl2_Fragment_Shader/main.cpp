#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <unordered_map>

typedef unsigned int GLuint;
typedef int GLint;
typedef char GLchar;

// Function pointer type definitions for OpenGL functions
typedef void (*GLFunctionPointer)();
std::unordered_map<std::string, GLFunctionPointer> glFunctions;

// Utility function to load OpenGL functions dynamically
GLFunctionPointer loadOpenGLFunction(const char* name) {
    GLFunctionPointer func = (GLFunctionPointer)SDL_GL_GetProcAddress(name);
    if (!func) {
        std::cerr << "Failed to load OpenGL function: " << name << std::endl;
        exit(EXIT_FAILURE);
    }
    return func;
}

// Function loader macro for convenience
#define LOAD_GL_FUNCTION(name) glFunctions[#name] = loadOpenGLFunction(#name);

// Shorthand for calling OpenGL functions dynamically
#define CALL_GL_FUNCTION(name, ...) \
    ((reinterpret_cast<decltype(&name)>(glFunctions[#name]))(__VA_ARGS__))

// Vertex shader source code (OpenGL 2.1 / GLSL #version 120)
const char* vertexShaderSource = R"(
#version 120
attribute vec2 aPos;
void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

// Fragment shader source code (OpenGL 2.1 / GLSL #version 120)
const char* fragmentShaderSource = R"(
#version 120
void main() {
    gl_FragColor = vec4(gl_FragCoord.x / 800.0, gl_FragCoord.y / 600.0, 0.5, 1.0);
}
)";

// Check for shader compilation errors
void checkShaderCompileStatus(GLuint shader, const std::string& shaderType) {
    GLint success;
    GLchar infoLog[512];
    CALL_GL_FUNCTION(glGetShaderiv, shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        CALL_GL_FUNCTION(glGetShaderInfoLog, shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

// Check for program linking errors
void checkProgramLinkStatus(GLuint program) {
    GLint success;
    GLchar infoLog[512];
    CALL_GL_FUNCTION(glGetProgramiv, program, GL_LINK_STATUS, &success);
    if (!success) {
        CALL_GL_FUNCTION(glGetProgramInfoLog, program, 512, nullptr, infoLog);
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create an SDL window with OpenGL context
    SDL_Window* window = SDL_CreateWindow(
        "SDL2 OpenGL Dynamic Loading", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        800, 
        600, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Load required OpenGL functions
    LOAD_GL_FUNCTION(glGenBuffers);
    LOAD_GL_FUNCTION(glBindBuffer);
    LOAD_GL_FUNCTION(glBufferData);
    LOAD_GL_FUNCTION(glGenVertexArrays);
    LOAD_GL_FUNCTION(glBindVertexArray);
    LOAD_GL_FUNCTION(glVertexAttribPointer);
    LOAD_GL_FUNCTION(glEnableVertexAttribArray);
    LOAD_GL_FUNCTION(glUseProgram);
    LOAD_GL_FUNCTION(glDrawArrays);
    LOAD_GL_FUNCTION(glCreateProgram);
    LOAD_GL_FUNCTION(glCreateShader);
    LOAD_GL_FUNCTION(glShaderSource);
    LOAD_GL_FUNCTION(glCompileShader);
    LOAD_GL_FUNCTION(glAttachShader);
    LOAD_GL_FUNCTION(glLinkProgram);
    LOAD_GL_FUNCTION(glDeleteShader);
    LOAD_GL_FUNCTION(glGetShaderiv);
    LOAD_GL_FUNCTION(glGetShaderInfoLog);
    LOAD_GL_FUNCTION(glGetProgramiv);
    LOAD_GL_FUNCTION(glGetProgramInfoLog);

    // Define the vertex data (fullscreen quad)
    float vertices[] = {
        -1.0f, -1.0f, 
         1.0f, -1.0f, 
        -1.0f,  1.0f, 
         1.0f,  1.0f
    };

    GLuint VAO, VBO;
    CALL_GL_FUNCTION(glGenVertexArrays, 1, &VAO);
    CALL_GL_FUNCTION(glGenBuffers, 1, &VBO);

    CALL_GL_FUNCTION(glBindVertexArray, VAO);
    CALL_GL_FUNCTION(glBindBuffer, GL_ARRAY_BUFFER, VBO);
    CALL_GL_FUNCTION(glBufferData, GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    CALL_GL_FUNCTION(glVertexAttribPointer, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    CALL_GL_FUNCTION(glEnableVertexAttribArray, 0);

    // Compile vertex shader
    GLuint vertexShader = CALL_GL_FUNCTION(glCreateShader, GL_VERTEX_SHADER);
    CALL_GL_FUNCTION(glShaderSource, vertexShader, 1, &vertexShaderSource, nullptr);
    CALL_GL_FUNCTION(glCompileShader, vertexShader);
    checkShaderCompileStatus(vertexShader, "VERTEX");

    // Compile fragment shader
    GLuint fragmentShader = CALL_GL_FUNCTION(glCreateShader, GL_FRAGMENT_SHADER);
    CALL_GL_FUNCTION(glShaderSource, fragmentShader, 1, &fragmentShaderSource, nullptr);
    CALL_GL_FUNCTION(glCompileShader, fragmentShader);
    checkShaderCompileStatus(fragmentShader, "FRAGMENT");

    // Link shaders into a program
    GLuint shaderProgram = CALL_GL_FUNCTION(glCreateProgram);
    CALL_GL_FUNCTION(glAttachShader, shaderProgram, vertexShader);
    CALL_GL_FUNCTION(glAttachShader, shaderProgram, fragmentShader);
    CALL_GL_FUNCTION(glLinkProgram, shaderProgram);
    checkProgramLinkStatus(shaderProgram);

    // Cleanup shaders (they're linked into the program now)
    CALL_GL_FUNCTION(glDeleteShader, vertexShader);
    CALL_GL_FUNCTION(glDeleteShader, fragmentShader);

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        CALL_GL_FUNCTION(glUseProgram, shaderProgram);
        CALL_GL_FUNCTION(glBindVertexArray, VAO);
        CALL_GL_FUNCTION(glDrawArrays, GL_TRIANGLE_STRIP, 0, 4);

        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    CALL_GL_FUNCTION(glDeleteBuffers, 1, &VBO);
    CALL_GL_FUNCTION(glDeleteVertexArrays, 1, &VAO);
    CALL_GL_FUNCTION(glDeleteProgram, shaderProgram);

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

