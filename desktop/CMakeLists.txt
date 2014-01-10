if(CLICK_MODE)
    install(FILES gallery-app.svg DESTINATION
        ${CMAKE_INSTALL_DATADIR}/icons)
    set(ICON "${CMAKE_INSTALL_DATADIR}/icons/gallery-app.svg")
else(CLICK_MODE)
    install(DIRECTORY icons/hicolor DESTINATION DESTINATION
            ${CMAKE_INSTALL_DATADIR}/icons FILES_MATCHING PATTERN *.png)
    set(ICON "gallery-app")
endif(CLICK_MODE)

file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE})
file(STRINGS ${DESKTOP_FILE}.in DESKTOP_FILE_CONTENTS)
foreach(LINE ${DESKTOP_FILE_CONTENTS})
    string(REGEX REPLACE "tr\\\(\"(.*)\"\\\)" "\\1" LINE "${LINE}")
    string(CONFIGURE "${LINE}" LINE)
    file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE} "${LINE}\n")
endforeach(LINE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
