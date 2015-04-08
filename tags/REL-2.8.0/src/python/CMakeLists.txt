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
                      ${DTUTIL_LIBRARY}
                    )
                    

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
  odebindings.cpp
  odebodywrapbindings.cpp
  orbitmotionmodelbindings.cpp        
  osgmath.cpp           
  osgmatrix.cpp
  osgnodebindings.cpp
  osgvec2.cpp             
  osgvec3.cpp
  osgvec4.cpp                        
  particlesystembindings.cpp                 
  physicalbindings.cpp             
  pointaxisbindings.cpp           
  positionallightbindings.cpp            
  recorderbindings.cpp   
  rtsmotionmodelbindings.cpp
  scenebindings.cpp      
  shadermanagerbindings.cpp
  shaderprogrambindings.cpp

  shaderparameterbindings.cpp
  shaderparamfloatbindings.cpp
  shaderparamintbindings.cpp
  shaderparamtexture2dbindings.cpp
  shaderparamtexturebindings.cpp
  shaderparamvec4bindings.cpp

  skyboxbindings.cpp       
  skydomebindings.cpp      
  spotlightbindings.cpp    
  systembindings.cpp    
  transformablebindings.cpp  
  transformbindings.cpp    
  tripodbindings.cpp  
  ufomotionmodelbindings.cpp
  viewbindings.cpp
  walkmotionmodelbindings.cpp
  
  actorpropertybindings.cpp
  actorproxybindings.cpp
  datatypebindings.cpp
  mapbindings.cpp
  projectbindings.cpp
)
  
SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)
 
BUILD_PYTHON_BINDING(${CORE_BINDINGS}
                     ${DTCORE_LIBRARY}
                    )
                    
################
SET(ABC_BINDINGS "PyDtABC")
  
  SET(TARGET_SRC 
  applicationbindings.cpp
  baseabcbindings.cpp
  dtabcbindings.cpp
  labelactorbindings.cpp
  proximitytriggerbindings.cpp
  triggerbindings.cpp
  weatherbindings.cpp
  widgetbindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING( ${ABC_BINDINGS}
                      ${DTABC_LIBRARY}
                    )

################
SET(ACTORS_BINDINGS "PyDtActors")

SET(TARGET_SRC
  dtactorsbindings.cpp
  waypointvolumebindings.cpp
)

SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

BUILD_PYTHON_BINDING (${ACTORS_BINDINGS}
                      ${DTACTORS_LIBRARY}
                     )

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
                     ${DTAI_LIBRARY}
                    )



################
IF (DTAUDIO_AVAILABLE)
	SET(AUDIO_BINDINGS "PyDtAudio")

	SET(TARGET_SRC 
	  audiomanagerbindings.cpp
	  dtaudiobindings.cpp
	  listenerbindings.cpp
	  soundeffectbinderbindings.cpp
	) 

	SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

	BUILD_PYTHON_BINDING(${AUDIO_BINDINGS} 
						 ${DTAUDIO_LIBRARY}
                        )
ENDIF (DTAUDIO_AVAILABLE)


################
IF (DTANIM_AVAILABLE)
	SET(ANIM_BINDINGS "PyDtAnim")

	SET(TARGET_SRC 
	  dtanimbindings.cpp
	  characterwrapperbindings.cpp
	)

	SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

	BUILD_PYTHON_BINDING(${ANIM_BINDINGS} 
						 ${DTANIM_LIBRARY}
						)
ENDIF (DTANIM_AVAILABLE)
                    
################

IF(BUILD_PLIB)

  SET(PLIB_BINDINGS "PyDtInputPLIB")

    SET(TARGET_SRC   joystickbindings.cpp  )

    SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)


  
  BUILD_PYTHON_BINDING(${PLIB_BINDINGS} 
                       ${DTINPUTPLIB_LIBRARY}
                      )
ENDIF(BUILD_PLIB)


################
IF (BUILD_WITH_QT)
  SET(INSPECTOR_BINDINGS "PyDtInspectorQt")

  SET(TARGET_SRC 
    dtinspectorbindings.cpp
  )

  SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)

  BUILD_PYTHON_BINDING(${INSPECTOR_BINDINGS} 
                       ${DTINSPECTORQT_LIBRARY}
                      )
ENDIF(BUILD_WITH_QT)

################
IF (BUILD_TERRAIN)
    SET(TERRAIN_BINDINGS "PyDtTerrain")

    SET(TARGET_SRC 
      dtTerrainBindings.cpp
      geocoordinateBindings.cpp
      terrainBindings.cpp
    )
    SET(TARGET_H ${CMAKE_SOURCE_DIR}/inc/python/dtpython.h)



    BUILD_PYTHON_BINDING(${TERRAIN_BINDINGS} 
                         ${DTTERRAIN_LIBRARY}
                        )
ENDIF(BUILD_TERRAIN)

