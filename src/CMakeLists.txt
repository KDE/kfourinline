ecm_setup_version(${RELEASE_SERVICE_VERSION}
    VARIABLE_PREFIX KFOURINLINE
    VERSION_HEADER kfourinline_version.h
)

configure_file(config-src.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-src.h)

add_executable(kfourinline)

target_sources(kfourinline PRIVATE
    aiboard.cpp
    aiboard.h
    buttonsprite.cpp
    buttonsprite.h
    chatdlg.cpp
    chatdlg.h
    displaygame.cpp
    displaygame.h
    displayintro.cpp
    displayintro.h
    introsprite.cpp
    introsprite.h
    kchatdialog.cpp
    kchatdialog.h
    kfontutils.cpp
    kfontutils.h
    kgameconnectdialog.cpp
    kgameconnectdialog.h
    kgamedebugdialog.cpp
    kgamedebugdialog.h
    kgamedialogconfig.cpp
    kgamedialogconfig.h
    kgamedialog.cpp
    kgamedialog.h
    kgamepropertyarray.h
    kwin4.cpp
    kwin4doc.cpp
    kwin4doc.h
    kwin4global.h
    kwin4.h
    kwin4gamesequence.cpp
    kwin4gamesequence.h
    kwin4player.cpp
    kwin4player.h
    kwin4view.cpp
    kwin4view.h
    main.cpp
    piecesprite.cpp
    piecesprite.h
    pixmapsprite.cpp
    pixmapsprite.h
    reflectiongraphicsscene.cpp
    reflectiongraphicsscene.h
    score.cpp
    score.h
    scoresprite.cpp
    scoresprite.h
    spritenotify.cpp
    spritenotify.h
    thememanager.cpp
    thememanager.h


    kfourinline.qrc
)

ki18n_wrap_ui(kfourinline settings.ui statuswidget.ui statistics.ui)

kconfig_add_kcfg_files(kfourinline prefs.kcfgc)

ecm_qt_declare_logging_category(kfourinline
    HEADER kfourinline_debug.h
    IDENTIFIER KFOURINLINE_LOG
    CATEGORY_NAME log_fourinline
    DESCRIPTION "KFourInLine"
    EXPORT KFOURINLINE
)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/icons/*-apps-kfourinline.png")
ecm_add_app_icon(kfourinline ICONS ${ICONS_SRCS})

target_link_libraries(kfourinline
    KF5KDEGames
    KF5KDEGamesPrivate
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::DNSSD
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    Qt${QT_MAJOR_VERSION}::Svg
)

install(TARGETS kfourinline  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

install(PROGRAMS org.kde.kfourinline.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})
install(FILES org.kde.kfourinline.appdata.xml  DESTINATION  ${KDE_INSTALL_METAINFODIR})

add_executable(kfourinlineproc)

target_sources(kfourinlineproc PRIVATE
    aiboard.cpp
    aiboard.h
    kgameprocess.cpp
    kgameprocess.h
    kwin4proc.cpp
    kwin4proc.h
)

target_link_libraries(kfourinlineproc
    KF5KDEGames
    KF5KDEGamesPrivate
    Qt${QT_MAJOR_VERSION}::Svg
)

install(TARGETS kfourinlineproc ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES kwin4.kcfg  DESTINATION  ${KDE_INSTALL_KCFGDIR} )

ecm_qt_install_logging_categories(
    EXPORT KFOURINLINE
    FILE kfourinline.categories
    DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
)
