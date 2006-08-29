SUBDIRS = demo

noinst_PROGRAMS = slideshow2 getsize

EXTRA_DIST =


slideshow2_SOURCES  = slideshow2.cc tools.cc tools.h converter.cc converter.h
slideshow2_CXXFLAGS = $(MAGICK_CXXFLAGS)
slideshow2_LDADD    = $(MAGICK_LIBS)

getsize_SOURCES  = getsize.cc
getsize_CXXFLAGS = $(MAGICK_CXXFLAGS)
getsize_LDADD    = $(MAGICK_LIBS)
