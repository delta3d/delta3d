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

INCLUDE_DIRECTORIES( ../inc ../ext/inc )
LINK_DIRECTORIES( ../lib ../ext/lib )

ADD_LIBRARY( dtcore SHARED base camera clouddome cloudplane compass cui_openglrenderer cui_openglshader deltawin effectmanager enveffect environment ephemeris flymotionmodel globals infiniteterrain inputdevice inputmapper isector joystick keyboard logicalinputdevice motionmodel mouse notify object orbitmotionmodel particlesystem physical pnoise pointaxis recorder scene skybox skydome skydomeshader sound stats sunlightshader system terrain tracker transform transformable tripod uidrawable ufomotionmodel walkmotionmodel )

TARGET_LINK_LIBRARIES( dtcore glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )

ADD_LIBRARY( gui_fl SHARED gui_fltk guimgr )

TARGET_LINK_LIBRARIES( gui_fl dtcore fltk GL glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )

ADD_LIBRARY( dtabc SHARED application baseabc weather widget )
TARGET_LINK_LIBRARIES( dtabc dtcore gui_fl glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )

#ADD_LIBRARY( dtchar SHARED character )
#TARGET_LINK_LIBRARIES( dtchar dtcore glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )
# add replicantbody

ADD_LIBRARY( dtvis SHARED bspcullcallback )
TARGET_LINK_LIBRARIES( dtvis GL osg osgDB osgUtil )

ADD_LIBRARY( dtaudio SHARED audiomanager listener soundeffectbinder )
TARGET_LINK_LIBRARIES( dtaudio glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )

#SUBDIRS( python soarx )

INSTALL_TARGETS( ${DELTA_SOURCE_DIR}/../lib dtcore dtabc dtvis dtaudio gui_fl )

#INCLUDE_DIRECTORIES( ../ext/RTI-1.3NGv6.4.1/Linux-rh9.0-i386-gcc-3.2.2-opt-mt/include )
INCLUDE_DIRECTORIES( $RTI_HOME/$RTI_BUILD_TYPE/include )
#LINK_DIRECTORIES( ../ext/RTI-1.3NGv6.4.1/Linux-rh9.0-i386-gcc-3.2.2-opt-mt/lib )
LINK_DIRECTORIES( $RTI_HOME/$RTI_BUILD_TYPE/lib )
ADD_LIBRARY( dthla SHARED dis_types entity rticonnection )
TARGET_LINK_LIBRARIES( dthla dtcore GL glgui Producer ode opcode OpenThreads osg osgDB osgSim osgText osgUtil plibsg plibul tinyxml RTI-NG fedtime )
