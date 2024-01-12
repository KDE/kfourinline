# SPDX-FileCopyrightText: 2024 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause

function(install_theme name)
    cmake_parse_arguments(ARG "NO_SVG_FILE" "SVG;PREVIEW" "" ${ARGN})
    if (NOT ARG_SVG AND NOT ARG_NO_SVG_FILE)
        set(ARG_SVG "${name}.svg")
    endif()
    if (NOT ARG_RC)
        set(ARG_RC "${name}.rc")
    endif()
    if (ARG_SVG)
        set(svgz "${CMAKE_CURRENT_BINARY_DIR}/${ARG_SVG}z")
        generate_svgz(${ARG_SVG} ${svgz} "theme-")
    else()
        set(svgz)
    endif()
    install(
        FILES
            ${name}.desktop
            ${ARG_RC}
            ${svgz}
        DESTINATION ${KDE_INSTALL_DATADIR}/kfourinline/themes
    )
endfunction()

install_theme(default)
install_theme(yellow)
install_theme(gray_reflection)
install_theme(yellow_reflection NO_SVG_FILE)
