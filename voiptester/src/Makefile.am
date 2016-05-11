noinst_PROGRAMS = vt_shell
vt_shell_LDADD = @LIBOBJS@

vt_shell_SOURCES = cli/cli_main.c cli/cli_main.h \
				   cli/cli_cmdmap.c cli/cli_cmdmap.h \
				   cli/cli_cmdHandler.c cli/cli_cmdHandler.h \
				   base/string.c base/string.h \
				   base/socket.c base/socket.h \
				   base/voip_comm.h base/voip_localeArchive.h \
				   base/voip_localeCfg.h \
				   base/voip_locale.c base/voip_locale.h \
				   vt/vt_send.c vt/vt_send.h \
				   vt/vt_action.c vt/vt_action.h \
				   vt/vt_event.c vt/vt_event.h \
				   vt/vt_msgQueue.c vt/vt_msgQueue.h \
				   vt/vt_config.c vt/vt_config.h \
				   vt/vt_fsm.c vt/vt_fsm.h \
				   vt/vt_timer.c vt/vt_timer.h \
				   contrib/iniparser/iniparser.c \
				   contrib/iniparser/iniparser.h \
				   contrib/iniparser/dictionary.c \
				   contrib/iniparser/dictionary.h
				   
DEFS = -DCX_DEBUG -DVOIP_CFG_ALL
AM_CFLAGS = -g3 -Wall -I $(top_srcdir)/src -I $(top_srcdir)/src/contrib/iniparser
