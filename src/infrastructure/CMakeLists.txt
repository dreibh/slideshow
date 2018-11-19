SET(infrastructure_files
   1leftarrow.png
   1rightarrow.png
   1uparrow.png
   2leftarrow.png
   2rightarrow.png
   imageviewer.js
   player_pause.png
   player_play.png
   rotate.png
   slideshowcontrol.html
   slideshow.js
)

INSTALL(FILES ${infrastructure_files} DESTINATION ${CMAKE_INSTALL_DATADIR}/slideshow/infrastructure)
