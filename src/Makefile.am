bin_PROGRAMS = lxc-machined-start
lxc_machined_start_SOURCES = lxc-machined-start.c util.c
lxc_machined_start_LDADD = \
    $(LXC_LIBS) \
    $(LIBSYSTEMD_LIBS)
lxc_machined_start_CFLAGS = \
    $(AM_CFLAGS) \
    $(LXC_CFLAGS) \
    $(LIBSYSTEMD_CFLAGS)
