install(FILES gallery-app.desktop 
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )

install(DIRECTORY icons/hicolor DESTINATION DESTINATION
    ${CMAKE_INSTALL_DATADIR}/icons FILES_MATCHING PATTERN *.png
    )
install(DIRECTORY icons/ubuntu-mobile DESTINATION DESTINATION
    ${CMAKE_INSTALL_DATADIR}/icons FILES_MATCHING PATTERN *.png
    )
