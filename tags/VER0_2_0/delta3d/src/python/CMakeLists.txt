########################################
#
# dtPython module
#
########################################

PROJECT( DT_PYTHON )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc ../../../../../usr/include/python2.3 )
INCLUDE_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/include )
LINK_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/lib )
LINK_DIRECTORIES( ../../lib ../../ext/lib )

ADD_LIBRARY( dtpython SHARED applicationbindings audiomanagerbindings baseabcbindings basebindings camerabindings characterbindings clouddomebindings cloudplanebindings compassbindings deltadrawablebindings deltawinbindings distypebindings dtabcbindings dtaudiobindings dtcharbindings dtcorebindings dtguiflbindings  dthlabindings effectmanagerbindings entitybindings enveffectbindings environmentbindings flymotionmodelbindings infiniteterrainbindings inputdevicebindings inputmapperbindings isectorbindings joystickbindings keyboardbindings listenerbindings logicalinputdevicebindings motionmodelbindings mousebindings noisegeneratorbindings objectbindings orbitmotionmodelbindings particlesystembindings physicalbindings pointaxisbindings recorderbindings rticonnectionbindings scenebindings skyboxbindings skydomebindings soundeffectbinderbindings systembindings terrainbindings trackerbindings transformablebindings transformbindings tripodbindings ufomotionmodelbindings uidrawablebindings walkmotionmodelbindings weatherbindings widgetbindings )
# dtsoarxbindings soarxterrainbindings
TARGET_LINK_LIBRARIES( dtpython dtcore dtabc dtaudio dtchar gui_fl )
# soarx
