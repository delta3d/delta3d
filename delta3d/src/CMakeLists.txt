########################################
#
# Delta3D's CMakeLists.txt
#
########################################
#
# Author: Chris Osborn, cwosborn@nps.edu
# 
# CMake uses this file to automatically generate Linux make's
# and MSVC Project files.
#

SET( DELTA /home/cwosborn/delta3d )

INCLUDE_DIRECTORIES( ${DELTA}/inc ${DELTA}/ext/inc )
LINK_DIRECTORIES( ${DELTA}/lib ${DELTA}/ext/lib )

PROJECT( dtcore )
SET( DT_CORE_SRCS base camera clouddome cloudplane compass cui_openglrenderer cui_openglshader deltawin effectmanager enveffect environment ephemeris flymotionmodel globals infiniteterrain inputdevice inputmapper isector joystick keyboard logicalinputdevice motionmodel mouse notify object orbitmotionmodel particlesystem physical pnoise pointaxis recorder scene skybox skydome skydomeshader sound soundeffectbinder stats sunlightshader system terrain tracker transform transformable tripod uidrawable ufomotionmodel walkmotionmodel )
ADD_LIBRARY( dtcore SHARED ${DT_CORE_SRCS} )
TARGET_LINK_LIBRARIES( dtcore glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )

PROJECT( gui_fl )
SET( GUI_FL_SRCS gui_fltk guimgr )
ADD_LIBRARY( gui_fl SHARED ${GUI_FL_SRCS} )
TARGET_LINK_LIBRARIES( gui_fl dtcore fltk GL glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )

PROJECT( dtabc )
SET( DT_ABC_SRCS application baseabc weather widget )
ADD_LIBRARY( dtabc SHARED ${DT_ABC_SRCS} )
TARGET_LINK_LIBRARIES( dtabc dtcore gui_fl glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )

#PROJECT( dtchar )
#SET( DT_CHAR_SRCS character )
#ADD_LIBRARY( dtchar SHARED ${DT_CHAR_SRCS} )
#TARGET_LINK_LIBRARIES( dtchar dtcore glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )
# add replicantbody

#PROJECT( dthla )
#SET( DT_HLA_SRCS dis_types entity rticonnection )
#ADD_LIBRARY( dthla SHARED ${DT_HLA_SRCS} )
#TARGET_LINK_LIBRARIES( dthla dtcore GL glgui Producer ode opcode OpenThreads osg osgDB osgSim osgText osgUtil plibsg plibul tinyxml )
# add RTI-NG ws2_32(?)

PROJECT( dtvis )
SET( DT_VIS_SRCS bspcullcallback )
ADD_LIBRARY( dtvis SHARED ${DT_VIS_SRCS} )
TARGET_LINK_LIBRARIES( dtvis GL osg osgDB osgUtil )

PROJECT( dtaudio )
SET( DT_AUDIO_SRCS audiomanager listener )
ADD_LIBRARY( dtaudio SHARED ${DT_AUDIO_SRCS} )
TARGET_LINK_LIBRARIES( dtaudio glgui isense Producer ode opcode openal OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs tinyxml )