
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

kde4_add_app_icon(kwin4_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../hi*-app-kfourinline.png")
kde4_add_executable(kfourinline ${kwin4_SRCS})

target_link_libraries(kfourinline  ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} kdegames kdegamesprivate)

install(TARGETS kfourinline  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kwin4proc_SRCS 
    aiboard.cpp
    kwin4proc.cpp )

kde4_add_executable(kfourinlineproc ${kwin4proc_SRCS})

target_link_libraries(kfourinlineproc ${KDE4_KDECORE_LIBS} kdegames kdegamesprivate)

install(TARGETS kfourinlineproc ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES kwin4.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kfourinlineui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kfourinline )


