project(kfourinline)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")
set (KF5_MIN_VERSION "5.15.0")

find_package(ECM 1.7.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets Svg)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    Completion
    CoreAddons
    Crash
    DNSSD
    DocTools
    I18n
    XmlGui
    WidgetsAddons
)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(ECMAddAppIcon)


add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)

add_subdirectory(src)
add_subdirectory(grafix)
add_subdirectory(doc)


########### install files ###############

install(PROGRAMS org.kde.kfourinline.desktop  DESTINATION  ${KDE_INSTALL_APPDIR})

ecm_install_icons(ICONS
    128-apps-kfourinline.png
    16-apps-kfourinline.png
    22-apps-kfourinline.png
    32-apps-kfourinline.png
    48-apps-kfourinline.png
    64-apps-kfourinline.png
    DESTINATION ${KDE_INSTALL_ICONDIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

