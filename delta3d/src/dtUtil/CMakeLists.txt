########################################
#
# dtUtil: CMakeLists.txt
#
########################################

ADD_LIBRARY( dtutil SHARED domain )
TARGET_LINK_LIBRARIES( dtutil plibsg plibul )

INSTALL_TARGETS( ../../lib dtutil )
