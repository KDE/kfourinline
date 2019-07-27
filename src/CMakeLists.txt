
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
   kgamedebugdialog.cpp
   kgamedialog.cpp
   kgamedialogconfig.cpp
   kgameconnectdialog.cpp
   kchatdialog.cpp
   pixmapsprite.cpp
   kfontutils.cpp
)
qt5_add_resources(kwin4_SRCS kfourinline.qrc) 

configure_file(config-src.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

ki18n_wrap_ui(kwin4_SRCS settings.ui statuswidget.ui statistics.ui )

kconfig_add_kcfg_files(kwin4_SRCS prefs.kcfgc )

ecm_setup_version(${KDE_APPLICATIONS_VERSION} VARIABLE_PREFIX KFOURINLINE VERSION_HEADER kfourinline_version.h)

ecm_qt_declare_logging_category(kwin4_SRCS HEADER kfourinline_debug.h IDENTIFIER KFOURINLINE_LOG CATEGORY_NAME log_fourinline)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../*-apps-k*.png")
ecm_add_app_icon(kwin4_SRCS ICONS ${ICONS_SRCS})

add_executable(kfourinline ${kwin4_SRCS})

target_link_libraries(kfourinline
   KF5::CoreAddons
   KF5::Crash
   KF5::DNSSD
   KF5::XmlGui
   KF5::WidgetsAddons
   KF5KDEGames
   KF5KDEGamesPrivate
   Qt5::Svg
)

install(TARGETS kfourinline  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kwin4proc_SRCS 
    aiboard.cpp
    kgameprocess.cpp
    kwin4proc.cpp
)

add_executable(kfourinlineproc ${kwin4proc_SRCS})

target_link_libraries(kfourinlineproc KF5KDEGames KF5KDEGamesPrivate Qt5::Svg)

install(TARGETS kfourinlineproc ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

install( FILES kwin4.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR} )


