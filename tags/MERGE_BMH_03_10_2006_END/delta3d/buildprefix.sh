#!/bin/bash

OS=`uname -s`

FRAMEWORK_INCLUDES=""
DEFINES=""

buildHeader() {

  SOURCE="inc/prefix/$1-src.h"
  DEST="inc/prefix/$1.h.gch"
  if ! [ -e "$DEST" ]; then
    CMD="g++ $DEFAULT_FLAGS $DEFINES $CPPFLAGS $FRAMEWORK_INCLUDES $INCLUDES $SOURCE -o $DEST"
    echo $CMD
    $CMD
  fi
}



MODE="DEBUG"

while [ -n "$1" ]; do
   
  if [ "$1" = "-c" ]; then
    echo "Cleaning..."
    echo ""
    rm -f inc/prefix/*.gch
  else
    MODE="$1"
  fi
  
  shift
done

if [ "${MODE}a" = "RELEASEa" ]; then

  echo "Building release headers." 
  echo ""

  DEFINES="-DSIGSLOT_PURE_ISO -DNDEBUG"
  if echo $OS | grep Darwin > /dev/null ; then 
     DEFAULT_FLAGS="-Os -pipe -fPIC -Wall -Wno-non-virtual-dtor"
     DEFINES="$DEFINES -D__USE_OSX_AGL_IMPLEMENTATION__ -DMAC_OS_X_VERSION_MIN_REQUIRED=1030"
  else
     DEFAULT_FLAGS="-O2 -pipe -fPIC -Wall -Wno-non-virtual-dtor"
  fi

else

  echo "Building debug headers"
  echo ""

  DEFINES="-DSIGSLOT_PURE_ISO"
  if echo $OS | grep Darwin > /dev/null ; then 
     DEFINES="$DEFINES -D_DEBUG -D__USE_OSX_AGL_IMPLEMENTATION__ -DMAC_OS_X_VERSION_MIN_REQUIRED=1030"
     DEFAULT_FLAGS="-g -O0 -pipe -fPIC -Wall -Wno-non-virtual-dtor"
  else
     DEFAULT_FLAGS="-g -O0 -pipe -fPIC -Wall -Wno-non-virtual-dtor"
  fi
fi  

INCLUDES="-I/usr/include/python2.4 -Iinc -Iext/inc -Iext/inc/CEGUI -I/usr/X11R6/include -I/usr/include -I/usr/local/include" 

if echo $OS | grep Darwin > /dev/null; then
  FRAMEWORK_INCLUDES="-Fext/Frameworks"
  if [ -n "$QTDIR" ]; then
    FRAMEWORK_INCLUDES="$FRAMEWORK_INCLUDES -F$QTDIR/lib"
  fi 
  INCLUDES="$INCLUDES -Iutilities/editor/inc -Iext/Frameworks/Xerces.framework/Headers -Iext/Frameworks/CEGUI.framework/Headers -Iext/Frameworks/gdal.framework/Headers"
else
  INCLUDES="$INCLUDES -Iutilities/editor/inc"
  if [ -n "$QTDIR" ]; then
    INCLUDES="$INCLUDES -I$QTDIR/include"
  fi 
fi 

buildHeader dtcoreprefix 

buildHeader dtprefix

buildHeader dtutilprefix

buildHeader dtdalprefix

buildHeader dtgameprefix

#Not used for now
#buildHeader inc/prefix/dtpythonprefix-src.h inc/prefix/dtpythonprefix.h.gch

DEFINES="$DEFINES -DQT_THREAD_SUPPORT -DQT_CORE_LIB -DQT_GUI_LIB -DQT_SHARED -D_REENTRANT"

buildHeader dtstageprefix
