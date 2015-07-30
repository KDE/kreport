#!/bin/sh

# Formats all .kreport files in the same dir with 1 spaces of indentation.
# Reports namespace and misplaced tag/quotes errors.

cd `dirname $0`
for fname in `find . -name \*.kreport` ; do
    tmp=`mktemp -u $fname.XXXXXXXX`
    XMLLINT_INDENT=" " xmllint --format $fname --output $tmp && mv $tmp $fname
done
