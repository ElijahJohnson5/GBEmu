#ifndef _UTIL_H
#define _UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

char *readFile(const char* filePath, const char* fileMode, long* size);

#ifdef __cplusplus
}
#endif

#endif