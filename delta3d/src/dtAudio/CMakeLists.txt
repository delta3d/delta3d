########################################
#
# dtAudio: CMakeLists.txt
#
########################################

ADD_LIBRARY( dtaudio SHARED audiomanager listener sound soundeffectbinder )
TARGET_LINK_LIBRARIES( dtaudio openal OpenThreads osgDB Producer plibsg plibul tinyxml )

INSTALL_TARGETS( ../lib dtaudio )
