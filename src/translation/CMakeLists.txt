#FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.desktop")
#INSTALL(FILES ${files} DESTINATION /usr/share/sax3/modules.d)
install(FILES sax3-touchpad.po sax3-keyboard.po sax3-mouse.po sax3-monitor.po DESTINATION /usr/share/sax3/translation)
