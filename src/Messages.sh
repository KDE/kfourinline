#! /usr/bin/env bash
$EXTRACTRC *.rc *.ui *.kcfg >> rc.cpp
#Extract colornames from .rc files
for color in `grep -h colorNamePlayer0=  ../grafix/*.rc | cut -d '=' -f 2`; do
    echo "i18nc(\"Player 0 color\", \"$color\");" >> rc.cpp
done
for color in `grep -h colorNamePlayer1=  ../grafix/*.rc | cut -d '=' -f 2`; do
    echo "i18nc(\"Player 1 color\", \"$color\");" >> rc.cpp
done
$XGETTEXT *.cpp -o $podir/kfourinline.pot 

