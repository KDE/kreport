#!/bin/sh

# Extract strings from all source files.
# EXTRACT_TR_STRINGS extracts strings with lupdate and convert them to .pot with
# lconvert.
$EXTRACT_TR_STRINGS `find . -not -path ./plugins/\* \( -name \*.cpp -o -name \*.h -o -name \*.ui -o -name \*.qml \) -type f` \
    -o $podir/kreport_examples.pot
