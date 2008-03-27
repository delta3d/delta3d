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



################
SET(DAL_BINDINGS "PyDtDAL")

ADD_LIBRARY ( ${DAL_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
  actorpropertybindings.cpp
  actorproxybindings.cpp
  datatypebindings.cpp
  dtdalbindings.cpp
  mapbindings.cpp
  projectbindings.cpp
)

ADD_DEPENDENCIES (${DAL_BINDINGS}
  dtDAL
)

TARGET_LINK_LIBRARIES ( ${DAL_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized dtDAL
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${DAL_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${DAL_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${DAL_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)



################
SET(PLIB_BINDINGS "PyDtInputPLIB")

ADD_LIBRARY ( ${PLIB_BINDINGS} SHARED
  joystickbindings.cpp
)

ADD_DEPENDENCIES (${PLIB_BINDINGS}
  dtCore
  dtInputPLIB
)

TARGET_LINK_LIBRARIES ( ${PLIB_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized dtCore
  optimized dtInputPLIB
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${PLIB_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${PLIB_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${PLIB_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)



################
SET(INSPECTOR_BINDINGS "PyDtInspector")

ADD_LIBRARY ( ${INSPECTOR_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
  dtinspectorbindings.cpp
)

ADD_DEPENDENCIES (${INSPECTOR_BINDINGS}
  dtInspector
)

TARGET_LINK_LIBRARIES ( ${INSPECTOR_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized dtInspector
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${INSPECTOR_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${INSPECTOR_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${INSPECTOR_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)

################
SET(TERRAIN_BINDINGS "PyDtTerrain")

ADD_LIBRARY ( ${TERRAIN_BINDINGS} SHARED
  ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h
  dtTerrainbindings.cpp
  geocoordinateBindings.cpp
  terrainBindings.cpp
)

ADD_DEPENDENCIES (${TERRAIN_BINDINGS}
  dtTerrain
)

TARGET_LINK_LIBRARIES ( ${TERRAIN_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
  optimized dtTerrain
)

IF (WIN32)
  SET_TARGET_PROPERTIES( ${TERRAIN_BINDINGS} PROPERTIES SUFFIX ".pyd")
ENDIF (WIN32)

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${TERRAIN_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${TERRAIN_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)

