#############################################################################
# PROGRAMS
#############################################################################

ADD_EXECUTABLE(createslideshow createslideshow.cc tools.cc converter.cc)
TARGET_LINK_LIBRARIES(createslideshow ${GraphicsMagick_LIBRARY})
TARGET_INCLUDE_DIRECTORIES(createslideshow PUBLIC ${GraphicsMagick_INCLUDE_DIR})
INSTALL(TARGETS createslideshow   DESTINATION ${CMAKE_INSTALL_BINDIR})
INSTALL(FILES   createslideshow.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)


#############################################################################
# SOURCE DIRECTORIES
#############################################################################

ADD_SUBDIRECTORY(infrastructure)
ADD_SUBDIRECTORY(example)
