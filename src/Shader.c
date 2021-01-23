#include "Shader.h"
#include <stdio.h>
#include <stdlib.h>

#include "Util.h"

#include <glad/glad.h>

uint32_t createShaderProgram(const char* vertexPath, const char* fragmentPath)
{
    const char* vertexCode = readFile(vertexPath, "r", NULL);
    const char* fragmentCode = readFile(fragmentPath, "r", NULL);

    uint32_t vertex, fragment;
    int32_t success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Shader vertex compilation failed: %s\n", infoLog);
        return 0;
    };

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("Fragment vertex compilation failed: %s\n", infoLog);
        return 0;
    };

    uint32_t id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);

    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        printf("Linking failed: %s\n", infoLog);
        return 0;
    }
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    free((void*)fragmentCode);
    free((void*)vertexCode);

    return id;
}