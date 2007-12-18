
include_directories( ${CMAKE_SOURCE_DIR}/libkdegames/kgame  )



########### next target ###############

set(kwin4_SRCS 
   main.cpp 
   chatdlg.cpp
   kwin4.cpp 
   spritenotify.cpp
   thememanager.cpp
   displayintro.cpp 
   displaygame.cpp 
   kwin4view.cpp 
   kwin4doc.cpp 
   kwin4player.cpp 
   buttonsprite.cpp
   introsprite.cpp
   piecesprite.cpp
   scoresprite.cpp
   score.cpp
   aiboard.cpp
   reflectiongraphicsscene.cpp
   pixmapsprite.cpp)

configure_file(config-src.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

kde4_add_ui_files(kwin4_SRCS settings.ui statuswidget.ui statistics.ui )

kde4_add_kcfg_files(kwin4_SRCS prefs.kcfgc )

set(kfourinline_icons
    ${CMAKE_CURRENT_SOURCE_DIR}/../hi16-app-kfourinline.png
    ${CMAKE_CURRENT_SOURCE_DIR}/../hi32-app-kfourinline.png
    ${CMAKE_CURRENT_SOURCE_DIR}/../hi48-app-kfourinline.png
    ${CMAKE_CURRENT_SOURCE_DIR}/../hi64-app-kfourinline.png
    ${CMAKE_CURRENT_SOURCE_DIR}/../hi128-app-kfourinline.png
)
kde4_add_win32_app_icon(kwin4_SRCS ${kfourinline_icons})
kde4_add_executable(kfourinline ${kwin4_SRCS})

target_link_libraries(kfourinline  ${KDE4_KDEUI_LIBS} kdegames )

install(TARGETS kfourinline  DESTINATION ${BIN_INSTALL_DIR} )


########### next target ###############

set(kwin4proc_SRCS 
    aiboard.cpp
    kwin4proc.cpp )

kde4_add_executable(kfourinlineproc ${kwin4proc_SRCS})

target_link_libraries(kfourinlineproc ${KDE4_KDECORE_LIBS} kdegames )

install(TARGETS kfourinlineproc DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES kwin4.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kfourinlineui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kfourinline )

