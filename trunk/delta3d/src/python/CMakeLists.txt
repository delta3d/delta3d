########################################
#
# dtPython module
#
########################################

PROJECT( DT_PYTHON )

SET( CMAKE_INSTALL_PREFIX ${DT_PYTHON_SOURCE_DIR}/ )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc /usr/include/python2.3 )
LINK_DIRECTORIES( ../../lib ../../ext/lib /usr/local/lib /usr/lib/python2.3/config )

ADD_LIBRARY( dtcorebindings SHARED basebindings camerabindings clouddomebindings cloudplanebindings compassbindings deltadrawablebindings deltawinbindings dtcorebindings effectmanagerbindings enveffectbindings environmentbindings flymotionmodelbindings infinitelightbindings infiniteterrainbindings inputdevicebindings inputmapperbindings isectorbindings joystickbindings keyboardbindings lightbindings loadablebindings logicalinputdevicebindings motionmodelbindings mousebindings noisegeneratorbindings objectbindings orbitmotionmodelbindings particlesystembindings physicalbindings pointaxisbindings positionallightbindings recorderbindings scenebindings skyboxbindings skydomebindings spotlightbindings systembindings terrainbindings trackerbindings transformablebindings transformbindings tripodbindings ufomotionmodelbindings uidrawablebindings walkmotionmodelbindings )
TARGET_LINK_LIBRARIES( dtcorebindings dtcore boost_python python2.3 util )

ADD_LIBRARY( dtabcbindings SHARED applicationbindings baseabcbindings dtabcbindings weatherbindings widgetbindings )
TARGET_LINK_LIBRARIES( dtabcbindings dtabc boost_python python2.3 util )

ADD_LIBRARY( dtaudiobindings SHARED audiomanagerbindings dtaudiobindings listenerbindings soundeffectbinderbindings )
TARGET_LINK_LIBRARIES( dtaudiobindings dtaudio boost_python python2.3 util )

ADD_LIBRARY( dtcharbindings SHARED characterbindings dtcharbindings )
TARGET_LINK_LIBRARIES( dtcharbindings dtchar boost_python python2.3 util )

ADD_LIBRARY( gui_flbindings SHARED dtguiflbindings )
TARGET_LINK_LIBRARIES( gui_flbindings gui_fl boost_python python2.3 util )

INSTALL_TARGETS( ../../lib dtcorebindings dtabcbindings dtcharbindings dtaudiobindings gui_flbindings )

#INCLUDE_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/include )
#LINK_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/lib )

#ADD_LIBRARY( dthlabindings SHARED distypebindings dthlabindings entitybindings rticonnectionbindings )
#TARGET_LINK_LIBRARIES( dthlabindings dthla boost_python python2.3 util )

#INSTALL_TARGETS( ../../lib dthlabindings )
