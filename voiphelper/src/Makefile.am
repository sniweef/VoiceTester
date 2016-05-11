noinst_PROGRAMS = vc_helper
vc_helper_LDADD = @LIBOBJS@

vc_helper_SOURCES = cli/cli_main.c  \
				   cli/cli_cmdmap.c \
				   base/hzg_string.c \
				   base/socket.c \
				   voiphelper/voiphelper_main.c \
				   voiphelper/voiphelper_config.c \
				   contrib/iniparser/iniparser.c \
				   contrib/iniparser/dictionary.c
				   
DEFS = -DCX_DEBUG -DVOIP_CFG_ALL
AM_CFLAGS = -g -Wall -I $(top_srcdir)/src -I $(top_srcdir)/src/contrib/iniparser
