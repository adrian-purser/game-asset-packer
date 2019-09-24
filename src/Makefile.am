AUTOMAKE_OPTS = gnu

LIBS = $(GLOBALLIBS) -lm -lpthread -ldl $(X11_LIBS)

SUBDIRS = \
adefs \
adepng \
window

bin_PROGRAMS = gap

gap_CXXFLAGS = $(GLOBALINCLUDE) -D__STDC_CONSTANT_MACROS
gap_LDADD = \
adefs/libadefs.la \
adepng/libadepng.la \
window/libwindow.la 

gap_SOURCES = \
canvas2/canvas2.h \
config.h \
configuration.h \
configuration.cpp \
main.cpp \
programoptions\program_options.h


