SUBDIRS = infrastructure example

bin_PROGRAMS = createslideshow

EXTRA_DIST =


createslideshow_SOURCES  = createslideshow.cc tools.cc tools.h converter.cc converter.h
createslideshow_CXXFLAGS = $(MAGICK_CXXFLAGS)
createslideshow_LDADD    = $(MAGICK_LIBS)
