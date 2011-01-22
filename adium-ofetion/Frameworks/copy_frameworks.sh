#!/bin/sh 

ADIUM_FRAMEWORKS_DIR="/Users/Evan/adium/build/Debug/Adium.app/Contents/Frameworks"

rm -rf *.framework
cp -r "$ADIUM_FRAMEWORKS_DIR"/AIUtilities.framework ./
cp -r "$ADIUM_FRAMEWORKS_DIR"/Adium.framework ./
cp -r "$ADIUM_FRAMEWORKS_DIR"/AdiumLibpurple.framework ./
cp -r "$ADIUM_FRAMEWORKS_DIR"/FriBidi.framework ./
cp -r "$ADIUM_FRAMEWORKS_DIR"/libglib.framework ./
cp -r "$ADIUM_FRAMEWORKS_DIR"/libintl.framework ./
cp -r "$ADIUM_FRAMEWORKS_DIR"/libpurple.framework ./

echo "Done"
