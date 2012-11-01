from PyDtUtil import *
from PyDtCore import *
from PyDtABC import *

import sys

class TestMap(Application):

   def Config(self) :
   
      self.helicopter = None
      self.tree = None
      self.smoke = ParticleSystem()
      self.explosion = EffectManager()
      self.step = -1.0
      self.bufferExplosion = 1
   
      # Set up a motion model so we may move the camera
      self.wmm = WalkMotionModel()
      self.wmm.SetDefaultMappings( self.GetKeyboard(), self.GetMouse() )
      self.wmm.SetTarget( self.GetCamera() )
      
      # Steps to load a map into Delta3D
      # 1. Set the project context, just like in the editor
      # 2. Get a reference to the map
      # 3. Load the map into the scene through the project class
      contextName = GetDeltaRootPath() + "/examples/data/demoMap"    
      Project.GetInstance().SetContext(contextName)
      myMap = Project.GetInstance().GetMap("MyCoolMap")
      
      self.LoadMap(myMap)

      # Get the proxies from the map
      self.proxies = ActorProxyVector()
      myMap.FindProxies(self.proxies,"*")

      self.smoke.LoadFile("Particles/smoke.osg")
      self.explosion.AddDetonationTypeMapping("HighExplosiveDetonation", "Particles/explosion.osg")
 
      print 'self.proxies[0] = ', self.proxies[0]
      
      for actor in self.proxies :
         print "Next Actor name:", actor.GetName()
         # Find our helicopter by name
         if actor.GetName() == "helo" :
            self.helicopter = actor.GetActor()
         # Find our tree by name
         if actor.GetName() == "StaticMesh1" :
            self.tree = actor.GetActor()
         
      
      # Error check
      if self.helicopter is None :
         print "Failed to locate the helicopter"
         sys.exit()
       
      if self.tree is None :
         print "Failed to find the tree"
         sys.exit()
     
      # move our tree away a little bit
      self.tree.SetTransform(Transform(-50, 0, -1, 0, 0, 0))
      self.smoke.SetEnabled(1)
      self.GetScene().AddDrawable(self.explosion)

      # translate the camera to a better view angle
      self.GetCamera().SetTransform(Transform(-25, -100, 0, 0, 0, 0))

   
   def PreFrame(self, deltaFrameTime) :

      tTree = Transform()
      tHeli = Transform()

      self.tree.GetTransform(tTree)
      self.helicopter.GetTransform(tHeli)
        
      tTreeTranslation = Vec3()
      tTree.GetTranslation(tTreeTranslation)
       
      tHeliTranslation = Vec3()
      tHeli.GetTranslation(tHeliTranslation)
        
      # If the helicopter isn't at the tree yet, keep translating...
      if tTreeTranslation[0] < tHeliTranslation[0] :
         self.helicopter.SetTransform(Transform(self.step, 0, 1, 90, 0, 0))
      ## It's there
      elif tTreeTranslation[0] >= tHeliTranslation[0] :    
         xyz = Vec3()
         tHeli.GetTranslation(xyz)
         heliPos = xyz

         # This is somewhat of a hack. The explosion will loop continuously in here
         # because of the conditions on which it is invoked. It eats framerate and 
         # causes the app to chug, not to mention doesn't even look like an explosion
         if self.bufferExplosion :
          
            self.explosion.AddDetonation(heliPos)
            self.bufferExplosion = 0
            self.smoke.SetParent(None) 
            self.helicopter.AddChild(self.smoke)
            self.smoke.SetEnabled(1)
            # Make sure to orient the new mesh to match the old one
            self.helicopter.SetTransform(Transform(xyz[0], xyz[1], 0, 90, -30, 0))
            
      # Reset the scene
      if self.GetKeyboard().GetKeyState( ord('R') ) :
          self.Reset()

      self.step -= 0.05
      
   def Reset(self) :
      self.helicopter.SetTransform(Transform(0, 0, 1, 90, 0, 0))
      self.smoke.SetEnabled(0)
      self.step = -0.00001
      self.bufferExplosion = 1

dataPath = GetDeltaDataPathList()
SetDataFilePathList ( dataPath + ";" + GetDeltaRootPath() + "/examples/data;")

app = TestMap('testMap.xml')

app.Config()
app.Run()
