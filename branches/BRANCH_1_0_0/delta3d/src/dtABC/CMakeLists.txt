########################################
#
# dtABC: CMakeLists.txt
#
########################################

ADD_LIBRARY( dtabc SHARED application baseabc weather widget )
TARGET_LINK_LIBRARIES( dtabc dtcore osgDB Producer plibsg plibul tinyxml )

INSTALL_TARGETS( ../lib dtabc )
