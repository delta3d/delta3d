########################################
#
# dtCore: CMakeLists.txt
#
########################################

ADD_LIBRARY( dtcore SHARED base camera clouddome cloudplane compass cui_openglrenderer cui_openglshader deltadrawable deltarendersurface deltawin effectmanager enveffect environment ephemeris flymotionmodel globals infinitelight infiniteterrain inputdevice inputmapper isector joystick keyboard light loadable logicalinputdevice motionmodel mouse notify object orbitmotionmodel particlesystem physical pnoise pointaxis positionallight recorder scene skybox skydome skydomeshader spotlight stats sunlightshader system terrain timer tracker transform transformable tripod uidrawable ufomotionmodel walkmotionmodel )
TARGET_LINK_LIBRARIES( dtcore glgui isense ode OpenThreads osg osgDB osgGL2 osgParticle osgTerrain osgText osgUtil plibsg plibul plibjs Producer tinyxml )

IF(EXISTS /usr/X11R6/lib/libXxf86vm.so )
TARGET_LINK_LIBRARIES( dtcore Xxf86vm )
ENDIF(EXISTS /usr/X11R6/lib/libXxf86vm.so )

INSTALL_TARGETS( ../lib dtcore )
