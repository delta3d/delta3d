#!/bin/sh

cd ../../lib

echo 'In directory ../../lib ...'
echo 'Linking from dtCore.so -> libdtcorebindings.so'
ln -sf libdtcorebindings.so dtCore.so
echo 'Linking from dtabcbindings.so -> libdtABC.so'
ln -sf libdtabcbindings.so dtABC.so
echo 'Linking from dtcharbindings.so -> libdtChar.so'
ln -sf libdtcharbindings.so dtChar.so
echo 'Linking from dtaudiobindings.so -> libdtAudio.so'
ln -sf libdtaudiobindings.so dtAudio.so
echo 'Linking from gui_flbindings.so -> libdtGUIFl.so'
ln -sf libgui_flbindings.so dtGUIFl.so
#echo 'Linking from dthlabindings.so -> libdt.so'
#ln -sf libdthlabindings.so dtHLA.so

