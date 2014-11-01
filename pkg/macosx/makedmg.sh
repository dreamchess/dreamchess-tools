#!/bin/sh

rm -rf dmg
mkdir dmg || exit
mkdir dmg/Licenses || exit
cp -R src/DreamChess.app dmg || exit
cp README dmg/README.txt || exit
cp COPYING dmg/COPYING.txt || exit
cd dmg/Licenses || exit
dreamchess-get-licenses.sh || exit
cd ../.. || exit
VERSION=$(defaults read $(cd dmg; pwd)/DreamChess.app/Contents/Info.plist CFBundleShortVersionString)

rm -f pack.temp.dmg
rm -f "DreamChess $VERSION.dmg"

hdiutil create -srcfolder dmg -volname "DreamChess $VERSION" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW -size 10m pack.temp || exit
device=$(hdiutil attach -readwrite -noverify -noautoopen "pack.temp.dmg" | egrep '^/dev/' | sed 1q | awk '{print $1}') || exit
sleep 2

echo '
   tell application "Finder"
     tell disk "'DreamChess $VERSION'"
           open
           set current view of container window to icon view
           set toolbar visible of container window to false
           set statusbar visible of container window to false
           set the bounds of container window to {400, 100, 875, 430}
           set theViewOptions to the icon view options of container window
           set arrangement of theViewOptions to not arranged
           set icon size of theViewOptions to 72
#           set background picture of theViewOptions to file ".background:'${backgroundPictureName}'"
           make new alias file at container window to POSIX file "/Applications" with properties {name:"Applications"}
           set position of item "'DreamChess.app'" of container window to {100, 100}
           set position of item "Applications" of container window to {375, 100}
           set position of item "'README.txt'" of container window to {100, 250}
           set position of item "'COPYING.txt'" of container window to {237, 250}
           set position of item "'Licenses'" of container window to {375, 250}
           set position of item "'.DS_Store'" of container window to {100, 500}
           set position of item "'.fseventsd'" of container window to {237, 500}
           set position of item "'.Trashes'" of container window to {375, 500}
           update without registering applications
           delay 5
           close
     end tell
   end tell
' | osascript || exit

chmod -Rf go-w /Volumes/"DreamChess $VERSION"
sync
sync
hdiutil detach ${device} || exit
hdiutil convert "pack.temp.dmg" -format UDZO -imagekey zlib-level=9 -o "DreamChess $VERSION" || exit
rm -f pack.temp.dmg 
