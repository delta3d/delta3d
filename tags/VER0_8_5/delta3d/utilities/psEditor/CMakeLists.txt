########################################
#
# psEditor: Particle Systems Editor
#
########################################

PROJECT( PS_EDITOR )

INCLUDE_DIRECTORIES( ../../inc ../../ext/inc )
LINK_DIRECTORIES( ../../lib ../../ext/lib )

ADD_EXECUTABLE( psEditor main psEditorGUI texturepreview )
TARGET_LINK_LIBRARIES( psEditor dtcore dtabc fltk osg osgParticle osgText osgUtil plibjs )



