if(CLICK_MODE)
    if(NOT BZR_REVNO)
        set(BZR_REVNO "latest")
    endif(NOT BZR_REVNO)

    execute_process(
      COMMAND dpkg-architecture -qDEB_HOST_ARCH
      OUTPUT_VARIABLE CLICK_ARCH
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json
            DESTINATION ${CMAKE_INSTALL_PREFIX})
    install(FILES gallery.apparmor gallery-content.json
            DESTINATION ${CMAKE_INSTALL_PREFIX})
endif(CLICK_MODE)

if(NOT CLICK_MODE)
    install(FILES gallery-content.json
            DESTINATION ${CMAKE_INSTALL_DATADIR}/content-hub/peers/ RENAME gallery-app)
endif(NOT CLICK_MODE)

