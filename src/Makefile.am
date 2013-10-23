SUBDIRS = infrastructure example

bin_PROGRAMS = createslideshow

dist_man_MANS = createslideshow.1

EXTRA_DIST =


createslideshow_SOURCES  = createslideshow.cc tools.cc tools.h converter.cc converter.h
createslideshow_CXXFLAGS = $(MAGICK_CXXFLAGS)
createslideshow_LDADD    = $(MAGICK_LIBS)
createslideshow_LDFLAGS  =
