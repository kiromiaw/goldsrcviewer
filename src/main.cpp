#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <cmath>
#include <cstdio>

//shader
const char* vertSrc = R"glsl(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

uniform float uAngle;

out vec3 vColor;

void main() {
    float c = cos(uAngle);
    float s = sin(uAngle);
    vec2 rotated = vec2(
        c * aPos.x - s * aPos.y,
        s * aPos.x + c * aPos.y
    );
    gl_Position = vec4(rotated, 0.0, 1.0);
    vColor = aColor;
}
)glsl";

const char* fragSrc = R"glsl(
#version 330 core
in  vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)glsl";

//helpers
static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        printf("Shader error: %s\n", log);
    }
    return s;
}

static GLuint createProgram(const char* vert, const char* frag) {
    GLuint vs = compileShader(GL_VERTEX_SHADER,   vert);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, frag);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(prog, 512, nullptr, log);
        printf("Link error: %s\n", log);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_Window* window = SDL_CreateWindow(
        "Spinning Triangle — OpenGL 3.3",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window) { printf("Window failed: %s\n", SDL_GetError()); return -1; }

    SDL_GLContext ctx = SDL_GL_CreateContext(window);
    if (!ctx) { printf("Context failed: %s\n", SDL_GetError()); return -1; }

    SDL_GL_SetSwapInterval(1); // vsync

    // glew
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { printf("GLEW init failed\n"); return -1; }

    printf("OpenGL: %s\n", glGetString(GL_VERSION));

    // xyrgb
    float verts[] = {
         0.0f,  0.6f,   1.0f, 0.2f, 0.2f,   // top r
         0.5f, -0.4f,   0.2f, 1.0f, 0.3f,   // right g 
        -0.5f, -0.4f,   0.2f, 0.4f, 1.0f,   // left b
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // position (location = 0): 2 floats, stride = 5 floats
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color (location = 1): 3 floats, offset = 2 floats
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // shader
    GLuint prog    = createProgram(vertSrc, fragSrc);
    GLint  uAngle  = glGetUniformLocation(prog, "uAngle");

    // loop
    float  angle   = 0.0f;
    bool   running = true;
    SDL_Event e;
    Uint64 prev = SDL_GetPerformanceCounter();

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
        }

        // dt
        Uint64 now  = SDL_GetPerformanceCounter();
        float  dt   = (float)(now - prev) / (float)SDL_GetPerformanceFrequency();
        prev        = now;

        angle += dt * 1.2f;

        //resize
        int w, h;
        SDL_GetWindowSize(window, &w, &h);
        glViewport(0, 0, w, h);

        //draw
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(prog);
        glUniform1f(uAngle, angle);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        SDL_GL_SwapWindow(window);
    }

    //end
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(prog);

    SDL_GL_DeleteContext(ctx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}