AC_PREREQ([2.64])
AC_INIT([lxc-machined-start],
        [0.1],
        [http://github.com/Mic92/lxc-machined-start/issues],
        [lxc-machined-start],
        [http://github.com/Mic92/lxc-machined-start])
AM_INIT_AUTOMAKE([foreign 1.11 -Wall])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])
AC_GNU_SOURCE
AC_CONFIG_FILES([
    Makefile
    src/Makefile
])
AC_CONFIG_MACRO_DIR([m4])

AX_CHECK_COMPILE_FLAG([-std=c11],
 [AX_APPEND_FLAG([-std=c11])],
 [AC_MSG_WARN([-std=c11 not supported, you may have to set CFLAGS to enable C11 support.])])

m4_pattern_forbid([^_?PKG_[A-Z_]+$],[*** pkg.m4 missing, please install pkg-config])

PKG_CHECK_MODULES(DBUS, [dbus-1 >= 1.3.2])
PKG_CHECK_MODULES(LXC, [lxc >= 1.0.0])

AC_OUTPUT
