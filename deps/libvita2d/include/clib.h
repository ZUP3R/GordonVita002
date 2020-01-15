#ifndef __CLIB__
#define __CLIB__

#include <psp2/kernel/clib.h> 
#define my_strlen(x) sceClibStrnlen(x, 1024)
#define my_strcmp(x, y) sceClibStrncmp(x, y,  my_strlen(y)+1)
#define my_strcpy(x, y) sceClibStrncpy(x, y, my_strlen(y)+1)
#define my_strncpy(x, y, z) sceClibStrncpy(x, y, z)
#define my_sprintf(x, y, ...) sceClibSnprintf(x, sizeof(x), y, __VA_ARGS__)
#define my_snprintf(x, a, y, ...) sceClibSnprintf(x, a, y, __VA_ARGS__)
#define my_vsprintf(x, y, z) sceClibVsnprintf(x, sizeof(x), y, z)
#define my_vsnprintf(x, a, y, z) sceClibVsnprintf(x, a, y, z)
#define my_strrchr(x, y) sceClibStrrchr(x, y)
#define my_strchr strchr
#define my_strstr(x, y) sceClibStrstr(x, y)
#define my_strcat(x, y) sceClibStrncat(x, y, my_strlen(y)+1)
#define my_memset(x, y, z) sceClibMemset(x, y, z)

#endif