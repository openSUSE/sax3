#FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.desktop")
#INSTALL(FILES ${files} DESTINATION /usr/share/sax3/modules.d)
install(FILES sax3-touchpad.pot sax3-keyboard.pot sax3-mouse.pot sax3-monitor.pot DESTINATION /usr/share/sax3/translation)
