if(CLICK_MODE)
    install(FILES gallery-app.svg DESTINATION
        ${CMAKE_INSTALL_DATADIR}/icons)
	set(ICON "${CMAKE_INSTALL_DATADIR}/icons/${GALLERY}.svg")
	set(APP_EXEC "./${GALLERY}")
else(CLICK_MODE)
    install(DIRECTORY icons/hicolor DESTINATION DESTINATION
            ${CMAKE_INSTALL_DATADIR}/icons FILES_MATCHING PATTERN *.png)
	set(APP_EXEC "${GALLERY}")
	set(ICON "${GALLERY}")
endif(CLICK_MODE)

set(GALLERY_URL_DISPATCHER gallery-app.url-dispatcher)

configure_file(${DESKTOP_FILE}.in.in ${DESKTOP_FILE}.in @ONLY)
add_custom_target(${DESKTOP_FILE} ALL
                  COMMENT "Merging translations into ${DESKTOP_FILE}"
                  COMMAND ${INTLTOOL_MERGE} -d -u ${CMAKE_SOURCE_DIR}/po
                          ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}.in ${DESKTOP_FILE})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)

install(FILES ${GALLERY_URL_DISPATCHER}
    DESTINATION ${CMAKE_INSTALL_DATADIR}/url-dispatcher/urls
    )
