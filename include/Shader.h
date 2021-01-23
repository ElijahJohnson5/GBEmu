#ifndef _SHADER_H
#define _SHADER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t createShaderProgram(const char* vertexPath, const char* fragmentPath);


#ifdef __cplusplus
}
#endif

#endif