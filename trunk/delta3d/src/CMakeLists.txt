########################################
#
# Delta3D's CMakeLists.txt
#
########################################
# 
# CMake uses this file to automatically generate Linux make's
# and MSVC Project files.
#

PROJECT( DELTA )

SET( CMAKE_INSTALL_PREFIX ${DELTA_SOURCE_DIR}/ )

INCLUDE_DIRECTORIES( ../inc ../ext/inc )
LINK_DIRECTORIES( ../lib ../ext/lib )

IF(EXISTS /usr/X11R6/lib )
LINK_DIRECTORIES( /usr/X11R6/lib )
ENDIF(EXISTS /usr/X11R6/lib )

ADD_LIBRARY( dtcore SHARED base camera clouddome cloudplane compass cui_openglrenderer cui_openglshader deltawin effectmanager enveffect environment ephemeris flymotionmodel globals infiniteterrain inputdevice inputmapper isector joystick keyboard logicalinputdevice motionmodel mouse notify object orbitmotionmodel particlesystem physical pnoise pointaxis recorder scene skybox skydome skydomeshader sound stats sunlightshader system terrain tracker transform transformable tripod uidrawable ufomotionmodel walkmotionmodel )
TARGET_LINK_LIBRARIES( dtcore glgui isense Producer ode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml Xxf86vm )

IF(EXISTS /usr/X11R6/lib/libXxf86vm.so )
TARGET_LINK_LIBRARIES( Xxf86vm )
ENDIF(EXISTS /usr/X11R6/lib/libXxf86vm.so )

ADD_LIBRARY( gui_fl SHARED gui_fltk guimgr )
TARGET_LINK_LIBRARIES( gui_fl dtcore fltk GL plibsg tinyxml )

ADD_LIBRARY( dtabc SHARED application baseabc weather widget )
TARGET_LINK_LIBRARIES( dtabc dtcore OpenThreads Producer plibsg )

ADD_LIBRARY( dtchar SHARED character )
TARGET_LINK_LIBRARIES( dtchar dtcore cal3d OpenThreads plibsg rbody cfgscript )

ADD_LIBRARY( dtvis SHARED bspcullcallback )
TARGET_LINK_LIBRARIES( dtvis GL osg osgDB osgUtil )

ADD_LIBRARY( dtaudio SHARED audiomanager listener soundeffectbinder )
TARGET_LINK_LIBRARIES( dtaudio openal OpenThreads plibsg )

INSTALL_TARGETS( ../lib dtcore dtabc dtchar dtvis dtaudio gui_fl )

# Note! You must have RTI installed for dthla to build. 
# We cannot distribute RTI, so you are on your own.
#INCLUDE_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/include )
#LINK_DIRECTORIES( $ENV{RTI_HOME}/$ENV{RTI_BUILD_TYPE}/lib )
#ADD_LIBRARY( dthla SHARED dis_types entity rticonnection )
#TARGET_LINK_LIBRARIES( dthla dtcore OpenThreads plibsg plibul RTI-NG fedtime )
#INSTALL_TARGETS( ../lib dthla )
