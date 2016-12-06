#include <unistd.h>

#define _noreturn_       __attribute__((noreturn))
#define _cleanup_(x)     __attribute__((cleanup(x)))
#define _cleanup_free_   _cleanup_(freep)
#define _cleanup_close_  _cleanup_(closep)

static inline void freep(void *p) { free(*(void **)p); }
static inline void closep(int *fd) { if (*fd >= 0) close(*fd); }
