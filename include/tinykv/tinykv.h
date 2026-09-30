#ifndef TINYKV_H
#define TINYKV_H

/* Returns the library's version string, e.g. "0.1.0-dev".
 * The returned pointer is a static string literal; the caller does not own it
 * and must not free it. */
const char *tkv_version(void);

#endif /* TINYKV_H */
