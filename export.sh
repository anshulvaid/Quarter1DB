#!/usr/bin/env bash

FOLDER_NAME=project1-23201116

rm -rf $FOLDER_NAME
rm -rf $FOLDER_NAME.zip
mkdir -p $FOLDER_NAME/codebase

# Root files
cp makefile.inc $FOLDER_NAME/codebase || exit 1
cp project1_report.txt $FOLDER_NAME/codebase || exit 1
cp readme.txt $FOLDER_NAME/codebase || exit 1

# Get files under rbf directly from git
# (avoid including unnecessary files)
cd rbf
rm -rf /tmp/rbf.zip || exit 1
rm -rf /tmp/rbf || exit 1
git archive --format zip -o /tmp/rbf.zip master || exit 1
unzip /tmp/rbf.zip -d /tmp/rbf || exit 1
cp -R /tmp/rbf ../$FOLDER_NAME/codebase || exit 1
cd ..

zip -r $FOLDER_NAME.zip $FOLDER_NAME || exit 1
rm -rf $FOLDER_NAME
