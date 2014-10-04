
palOldNew="/Users/david/development/ext/src/pal-branch/lib/libpal.dylib @executable_path/../lib/libpal.dylib"

mkdir Qt
mkdir QtPlugins

pushd Qt
targetDir=$PWD

rm -rf Qt*.framework phonon.framework
pushd /usr/local/Trolltech/Qt-4.8.6/lib
cp -av phonon.framework QtCore.framework    QtGui.framework     QtNetwork.framework QtOpenGL.framework  QtWebKit.framework $targetDir
cp -av ../plugins/* ~/delta3d/QtPlugins
popd

for token in *.framework; do 
   install_name_tool -id @executable_path/../Frameworks/$token/Versions/4/${token%.framework}  $token/Versions/4/${token%.framework}
   for tokenInner in *.framework; do
       #if [[ ${tokenInner} = ${token} ]]; then
          install_name_tool -change /usr/local/Trolltech/Qt-4.8.6/lib/${tokenInner}/Versions/4/${tokenInner%.framework} @executable_path/../Frameworks/${tokenInner}/Versions/4/${tokenInner%.framework}  $token/${token%.framework}
       #fi
   done
   #otool -L $token/${token%.framework}

   targetFrw="/usr/local/Trolltech/Qt-4.8.6/lib/${token}/Versions/4/${token%.framework} @executable_path/../Frameworks/${token}/Versions/4/${token%.framework}"
   
   pushd ../Frameworks
   
   for dep in *.framework; do
       #if [[ ${tokenInner} = ${token} ]]; then
          install_name_tool -change $targetFrw  $dep/${dep%.framework}
          install_name_tool -change $palOldNew $dep/${dep%.framework}
       #fi
   done

   popd

   pushd ../QtPlugins 
   find . -name "*.dylib" -exec install_name_tool -change $targetFrw {} \; 
   popd

   pushd ../Applications
   
   for dep in *.app; do
       #if [[ ${tokenInner} = ${token} ]]; then
          install_name_tool -change $targetFrw  $dep/Contents/MacOS/${dep%.app}
          install_name_tool -change $palOldNew $dep/Contents/MacOS/${dep%.app}
          #otool -L $dep/Contents/MacOS/${dep%.app}
       #fi
   done

   popd

   pushd ../lib

   for dep in *.dylib; do
       #if [[ ${tokenInner} = ${token} ]]; then
          install_name_tool -change $targetFrw  $dep
          install_name_tool -change $palOldNew $dep
       #fi
   done

   popd
   pushd ../bin

   for dep in *; do
       #if [[ ${tokenInner} = ${token} ]]; then
          install_name_tool -change $targetFrw  $dep
          install_name_tool -change $palOldNew $dep
       #fi
   done

   popd

   pushd ../lib/directorplugins

   for dep in *.so; do
       #if [[ ${tokenInner} = ${token} ]]; then
          install_name_tool -change $targetFrw  $dep
          install_name_tool -change $palOldNew $dep
       #fi
   done

   popd

   pushd ../lib/stplugins

   for dep in *.so; do
       #if [[ ${tokenInner} = ${token} ]]; then
          install_name_tool -change $targetFrw  $dep
          install_name_tool -change $palOldNew $dep
       #fi
   done

   popd
done

popd

