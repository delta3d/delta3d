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

TARGET_LINK_LIBRARIES ( ${UTIL_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
)

SET_TARGET_PROPERTIES( ${UTIL_BINDINGS} PROPERTIES SUFFIX ".pyd")

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

TARGET_LINK_LIBRARIES ( ${CORE_BINDINGS}
  optimized ${Boost_LIBRARY_DIRS}/boost_python-vc80-mt 
  optimized ${PYTHON_LIBRARY}
)

SET_TARGET_PROPERTIES( ${CORE_BINDINGS} PROPERTIES SUFFIX ".pyd")

#little hack to get the .dll into /bin and .lib into /lib
IF (MSVC)
  SET_TARGET_PROPERTIES( ${CORE_BINDINGS} PROPERTIES PREFIX "/../../bin/")
  SET_TARGET_PROPERTIES( ${CORE_BINDINGS} PROPERTIES IMPORT_PREFIX "../")
ENDIF (MSVC)






































































































































































