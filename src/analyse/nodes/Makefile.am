noinst_LTLIBRARIES = libanalysisnodes.la

libanalysisnodes_la_SOURCES = calculate.cpp collect1d.cpp collect2d.cpp collect3d.cpp
libanalysisnodes_la_SOURCES += dynamicsite.cpp
libanalysisnodes_la_SOURCES += exclude.cpp
libanalysisnodes_la_SOURCES += fit1d.cpp
libanalysisnodes_la_SOURCES += node.cpp nodereference.cpp
libanalysisnodes_la_SOURCES += process1d.cpp process2d.cpp process3d.cpp
libanalysisnodes_la_SOURCES += select.cpp sequence.cpp


noinst_HEADERS = calculate.h
noinst_HEADERS += collect1d.h collect2d.h collect3d.h
noinst_HEADERS += dynamicsite.h
noinst_HEADERS += exclude.h
noinst_HEADERS += fit1d.h
noinst_HEADERS += node.h nodereference.h nodes.h
noinst_HEADERS += process1d.h process2d.h process3d.h
noinst_HEADERS += select.h sequence.h

AM_CPPFLAGS = -I../ -I../../ @DISSOLVE_CFLAGS@

