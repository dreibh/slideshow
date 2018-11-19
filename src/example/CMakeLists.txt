#############################################################################
# EXAMPLE
#############################################################################

SET(example_files
   demo-background.png
   demo-icon.png
   demo-maintitle.html
   demo-section1.html
   demo-section2.html
   demo-slideshow
   demo-tail.html
   demo-head.html
   demo-stylesheet.css
)

INSTALL(FILES ${example_files} DESTINATION ${CMAKE_INSTALL_DATADIR}/slideshow/example)


#############################################################################
# SOURCE DIRECTORIES
#############################################################################

ADD_SUBDIRECTORY(demo-images)
