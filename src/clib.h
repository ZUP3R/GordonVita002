#ifndef __CLIB__
#define __CLIB__

#include <psp2/paf.h>
#include <psp2/kernel/clib.h> 

#define my_strlen(x) sceClibStrnlen(x, 1024)
#define my_strcmp(x, y) sceClibStrncmp(x, y,  my_strlen(y)+1)
#define my_strncmp sceClibStrncmp
#define my_strcpy(x, y) sceClibStrncpy(x, y, my_strlen(y)+1)
#define my_strncpy sceClibStrncpy
#define my_sprintf(x, y, ...) sceClibSnprintf(x, sizeof(x), y, __VA_ARGS__)
#define my_snprintf sceClibSnprintf
#define my_vsprintf(x, y, z) sceClibVsnprintf(x, sizeof(x), y, z)
#define my_vsnprintf sceClibVsnprintf
#define my_strrchr sceClibStrrchr
#define my_strchr sce_paf_private_strchr
#define my_strstr sceClibStrstr
#define my_strcat(x, y) sceClibStrncat(x, y, my_strlen(y)+1)
#define my_memset sceClibMemset
#define my_malloc malloc
#define my_free free

#endif