#!/usr/bin/env bash

FOLDER_NAME=project1-23201116

rm -rf $FOLDER_NAME
rm -rf $FOLDER_NAME.zip
mkdir -p $FOLDER_NAME/codebase/rbf

# Root files
cp makefile.inc $FOLDER_NAME/codebase || exit 1
cp project1_report.txt $FOLDER_NAME/codebase || exit 1
cp readme.txt $FOLDER_NAME/codebase || exit 1

# Get files under rbf directly from git
# (avoid including unnecessary files)
for f in `git archive --format=tar HEAD:rbf | tar t`; do
    cp -R rbf/$f $FOLDER_NAME/codebase/rbf || exit 1
done

zip -r $FOLDER_NAME.zip $FOLDER_NAME || exit 1
rm -rf $FOLDER_NAME
