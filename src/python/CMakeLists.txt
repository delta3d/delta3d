################
SET(UTIL_BINDINGS "PyDtUtil")

SET(TARGET_SRC 
  dtutilbindings.cpp
  enumerationbindings.cpp
  logbindings.cpp
  matrixutilbindings.cpp
  noisebindings.cpp
  polardecompbindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING( ${UTIL_BINDINGS} 
                                              DTUTIL_LIBRARY
                                              BOOST_PYTHON_LIBRARY
                                              PYTHON_LIBRARY
                                              OSG_LIBRARY)




################
SET(CORE_BINDINGS "PyDtCore")
  
SET(TARGET_SRC
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
  
SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)
 
BUILD_PYTHON_BINDING(${CORE_BINDINGS}
                                          DTUTIL_LIBRARY
                                          DTCORE_LIBRARY
                                          BOOST_PYTHON_LIBRARY
                                          PYTHON_LIBRARY
                                          OSG_LIBRARY)
 
################
SET(ABC_BINDINGS "PyDtABC")
  
  SET(TARGET_SRC 
  applicationbindings.cpp
  baseabcbindings.cpp
  dtabcbindings.cpp
  proximitytriggerbindings.cpp
  triggerbindings.cpp
  weatherbindings.cpp
  widgetbindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING( ${ABC_BINDINGS}
                                          DTABC_LIBRARY
                                          BOOST_PYTHON_LIBRARY
                                          PYTHON_LIBRARY
                                          OSG_LIBRARY)


################
SET(ACTORS_BINDINGS "PyDtActors")

SET(TARGET_SRC
  dtactorsbindings.cpp
  waypointvolumebindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING (${ACTORS_BINDINGS}
                                              DTCORE_LIBRARY
                                              DTACTORS_LIBRARY
                                              BOOST_PYTHON_LIBRARY
                                              PYTHON_LIBRARY
                                              OSG_LIBRARY)


################
SET(AI_BINDINGS "PyDtAI")

SET(TARGET_SRC 
  astarconfigbindings.cpp
  dtaibindings.cpp
  waypointastarbindings.cpp
  waypointbindings.cpp
  waypointmanagerbindings.cpp
  waypointpairbindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING(${AI_BINDINGS} 
                                        BOOST_PYTHON_LIBRARY
                                        PYTHON_LIBRARY
                                        OSG_LIBRARY
                                        DTCORE_LIBRARY
                                        DTAI_LIBRARY)



################
SET(AUDIO_BINDINGS "PyDtAudio")

SET(TARGET_SRC 
  audiomanagerbindings.cpp
  dtaudiobindings.cpp
  listenerbindings.cpp
  soundeffectbinderbindings.cpp
) 

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING(${AUDIO_BINDINGS} 
                                        BOOST_PYTHON_LIBRARY
                                        PYTHON_LIBRARY
                                        DTAUDIO_LIBRARY)


################
SET(DAL_BINDINGS "PyDtDAL")

SET(TARGET_SRC 
  actorpropertybindings.cpp
  actorproxybindings.cpp
  datatypebindings.cpp
  dtdalbindings.cpp
  mapbindings.cpp
  projectbindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING(${DAL_BINDINGS} 
                                        BOOST_PYTHON_LIBRARY
                                        PYTHON_LIBRARY
                                        DTDAL_LIBRARY)


################

IF(BUILD_PLIB)

	SET(PLIB_BINDINGS "PyDtInputPLIB")

    SET(TARGET_SRC   joystickbindings.cpp	)

    SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)


	
	BUILD_PYTHON_BINDING(${PLIB_BINDINGS} 
                                            BOOST_PYTHON_LIBRARY
                                            PYTHON_LIBRARY
                                            DTCORE_LIBRARY
                                            DTINPUT_PLIB_LIBRARY)
ENDIF(BUILD_PLIB)


################
SET(INSPECTOR_BINDINGS "PyDtInspector")

SET(TARGET_SRC 
  dtinspectorbindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING(${INSPECTOR_BINDINGS} 
                                        BOOST_PYTHON_LIBRARY
                                        PYTHON_LIBRARY
                                        DTINSPECTOR_LIBRARY)


################
SET(TERRAIN_BINDINGS "PyDtTerrain")

SET(TARGET_SRC 
  dtTerrainBindings.cpp
  geocoordinateBindings.cpp
  terrainBindings.cpp
)
SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)



BUILD_PYTHON_BINDING(${TERRAIN_BINDINGS} 
                                        BOOST_PYTHON_LIBRARY
                                        PYTHON_LIBRARY
                                        DTCORE_LIBRARY
                                        DTTERRAIN_LIBRARY)

