
########### next target ###############

set(kwin4_SRCS
   kfourinline_debug.cpp 
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
   kgamedebugdialog.cpp
   kgamedialog.cpp
   kgamedialogconfig.cpp
   kgameconnectdialog.cpp
   kchatdialog.cpp
   pixmapsprite.cpp
   kfontutils.cpp)

configure_file(config-src.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

ki18n_wrap_ui(kwin4_SRCS settings.ui statuswidget.ui statistics.ui )

kconfig_add_kcfg_files(kwin4_SRCS prefs.kcfgc )

kde4_add_app_icon(kwin4_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../hi*-app-kfourinline.png")
add_executable(kfourinline ${kwin4_SRCS})

target_link_libraries(kfourinline 
   KF5::KIOCore
   KF5::DNSSD
   KF5::KDELibs4Support
   KF5KDEGames
   KF5KDEGamesPrivate
   Qt5::Svg)

install(TARGETS kfourinline  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kwin4proc_SRCS 
    aiboard.cpp
    kgameprocess.cpp
    kwin4proc.cpp )

add_executable(kfourinlineproc ${kwin4proc_SRCS})

target_link_libraries(kfourinlineproc KF5::KDELibs4Support KF5KDEGames KF5KDEGamesPrivate Qt5::Svg)

install(TARGETS kfourinlineproc ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES kwin4.kcfg  DESTINATION  ${KCFG_INSTALL_DIR} )
install( FILES kfourinlineui.rc  DESTINATION  ${KXMLGUI_INSTALL_DIR}/kfourinline )


