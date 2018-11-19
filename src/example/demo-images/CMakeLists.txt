#############################################################################
# EXAMPLE
#############################################################################

SET(demoimages_files
   alpha01.fsf
   alpha01.png
   alpha02.fsf
   alpha02.png
   alpha03.fsf
   alpha03.png
   alpha04.fsf
   alpha04.png
   alpha05.fsf
   alpha05.png
   alpha06.fsf
   alpha06.png
   alpha07.fsf
   alpha07.png
   alpha08.fsf
   alpha08.png
)

INSTALL(FILES ${demoimages_files} DESTINATION ${CMAKE_INSTALL_DATADIR}/slideshow/example/demo-images)
