#include "tinykv/tinykv.h"

#define TINYKV_VERSION "0.1.0-dev"

const char *tkv_version(void)
{
    return TINYKV_VERSION;
}
