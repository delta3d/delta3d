########################################
#
# dtPython module
#
########################################

PROJECT( DT_PYTHON )

SET( CMAKE_INSTALL_PREFIX ${DT_PYTHON_SOURCE_DIR}/ )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc /usr/include/python2.3 )
LINK_DIRECTORIES( ../../lib ../../ext/lib /usr/local/lib /usr/lib/python2.3/config )

#INCLUDE_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/include )
#LINK_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/lib )

ADD_LIBRARY( dtcorebindings SHARED basebindings camerabindings clouddomebindings cloudplanebindings compassbindings deltadrawablebindings deltawinbindings dtcorebindings effectmanagerbindings enveffectbindings environmentbindings flymotionmodelbindings infiniteterrainbindings inputdevicebindings inputmapperbindings isectorbindings joystickbindings keyboardbindings logicalinputdevicebindings motionmodelbindings mousebindings noisegeneratorbindings objectbindings orbitmotionmodelbindings particlesystembindings physicalbindings pointaxisbindings recorderbindings scenebindings skyboxbindings skydomebindings systembindings terrainbindings trackerbindings transformablebindings transformbindings tripodbindings ufomotionmodelbindings uidrawablebindings walkmotionmodelbindings )
TARGET_LINK_LIBRARIES( dtcorebindings boost_python-gcc-mt-d python2.3 )

ADD_LIBRARY( dtabcbindings SHARED applicationbindings baseabcbindings dtabcbindings weatherbindings widgetbindings )
TARGET_LINK_LIBRARIES( dtabcbindings boost_python-gcc-mt-d python2.3 )

ADD_LIBRARY( dtaudiobindings SHARED audiomanagerbindings dtaudiobindings listenerbindings soundeffectbinderbindings )
TARGET_LINK_LIBRARIES( dtaudiobindings boost_python-gcc-mt-d python2.3 )

ADD_LIBRARY( dtcharbindings SHARED characterbindings dtcharbindings )
TARGET_LINK_LIBRARIES( dtcharbindings boost_python-gcc-mt-d python2.3 )

ADD_LIBRARY( dtguiflbindings SHARED dtguiflbindings )
TARGET_LINK_LIBRARIES( dtguiflbindings boost_python-gcc-mt-d python2.3 )

#ADD_LIBRARY( dthlabindings SHARED distypebindings dthlabindings entitybindings rticonnectionbindings )
#TARGET_LINK_LIBRARIES( dthlabindings boost_python-gcc-mt-d python2.3 )

#ADD_LIBRARY( dtsoarxbindings SHARED dtsoarxbindings soarxterrainbindings )
#TARGET_LINK_LIBRARIES( dtsoarxbindings boost_python-gcc-mt-d python2.3 )

INSTALL_TARGETS( ../../lib dtcorebindings dtabcbindings dtcharbindings dtaudiobindings gui_flbindings )
