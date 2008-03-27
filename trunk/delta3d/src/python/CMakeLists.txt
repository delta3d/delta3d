################
SET(UTIL_BINDINGS "PyDtUtil")

ADD_LIBRARY ( ${UTIL_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
  dtutilbindings.cpp
  enumerationbindings.cpp
  logbindings.cpp
  matrixutilbindings.cpp
  noisebindings.cpp
  polardecompbindings.cpp
)

ADD_DEPENDENCIES (${UTIL_BINDINGS}
  dtUtil
)
TARGET_LINK_LIBRARIES ( ${UTIL_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${UTIL_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${UTIL_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${UTIL_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)


################
SET(CORE_BINDINGS "PyDtCore")

ADD_LIBRARY ( ${CORE_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
basebindings.cpp 
camerabindings.cpp 
clouddomebindings.cpp
cloudplanebindings.cpp
collisionmotionmodelbindings.cpp
compassbindings.cpp
deltadrawablebindings.cpp
deltawinbindings.cpp
dtcorebindings.cpp
effectmanagerbindings.cpp
enveffectbindings.cpp
environmentbindings.cpp   
flymotionmodelbindings.cpp
fpscolliderbindings.cpp
fpsmotionmodelbindings.cpp
infinitelightbindings.cpp
infiniteterrainbindings.cpp
inputdevicebindings.cpp
inputmapperbindings.cpp
isectorbindings.cpp
keyboardbindings.cpp     
lightbindings.cpp     
loadablebindings.cpp    
logicalinputdevicebindings.cpp
motionmodelbindings.cpp
mousebindings.cpp   
objectbindings.cpp      
orbitmotionmodelbindings.cpp        
osgmath.cpp           
osgmatrix.cpp           
osgvec2.cpp             
osgvec3.cpp
osgvec4.cpp                        
particlesystembindings.cpp                 
physicalbindings.cpp             
pointaxisbindings.cpp           
positionallightbindings.cpp            
recorderbindings.cpp   
scenebindings.cpp      
skyboxbindings.cpp       
skydomebindings.cpp      
spotlightbindings.cpp    
systembindings.cpp    
transformablebindings.cpp  
transformbindings.cpp    
tripodbindings.cpp  
ufomotionmodelbindings.cpp
walkmotionmodelbindings.cpp
)

ADD_DEPENDENCIES (${CORE_BINDINGS}
  dtCore
)
  
TARGET_LINK_LIBRARIES ( ${CORE_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized ${OSG_LIBRARY}
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${CORE_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${CORE_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${CORE_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)



################
SET(ABC_BINDINGS "PyDtABC")

ADD_LIBRARY ( ${ABC_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
  applicationbindings.cpp
  baseabcbindings.cpp
  dtabcbindings.cpp
  proximitytriggerbindings.cpp
  triggerbindings.cpp
  weatherbindings.cpp
  widgetbindings.cpp
)

ADD_DEPENDENCIES (${ABC_BINDINGS}
  dtABC
  dtCore
)

TARGET_LINK_LIBRARIES ( ${ABC_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized ${OSG_LIBRARY}
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${ABC_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${ABC_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${ABC_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)


################
SET(AI_BINDINGS "PyDtAI")

ADD_LIBRARY ( ${AI_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
  astarconfigbindings.cpp
  dtaibindings.cpp
  waypointastarbindings.cpp
  waypointbindings.cpp
  waypointmanagerbindings.cpp
  waypointpairbindings.cpp
)

ADD_DEPENDENCIES (${AI_BINDINGS}
  dtAI
  dtCore
)

TARGET_LINK_LIBRARIES ( ${AI_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized ${OSG_LIBRARY}
  optimized dtCore
  optimized dtAI
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${AI_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${AI_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${AI_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)


################
SET(AUDIO_BINDINGS "PyDtAudio")

ADD_LIBRARY ( ${AUDIO_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
  audiomanagerbindings.cpp
  dtaudiobindings.cpp
  listenerbindings.cpp
  soundeffectbinderbindings.cpp
)

ADD_DEPENDENCIES (${AUDIO_BINDINGS}
  dtAudio
)

TARGET_LINK_LIBRARIES ( ${AUDIO_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized dtAudio
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${AUDIO_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${AUDIO_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${AUDIO_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)
