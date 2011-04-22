from PyDtCore import *
from PyDtABC import *
from PyDtDAL import *

import sys

class TestMap(Application):

   def Config(self) :
      
      # Steps to load a map into Delta3D
      # 1. Set the project context, just like in the editor
      # 2. Get a reference to the map
      # 3. Load the map into the scene through the project class
      contextName = GetDeltaDataPathList() + "/demoMap"
      
      Project.GetInstance().SetContext(contextName)
      myMap = Project.GetInstance().GetMap("MyCoolMap")
      Project.GetInstance().LoadMapIntoScene(myMap, self.GetScene())

      # This doesn't work! I can't get Boost.Python to properly
      # translate between the retrn value of this method:
      # std::vector< osg::_ref< ActorProxy > > and my own wrapper
      # ActorProxyVec...
      
      # Get the proxies from the map
      proxies = ActorProxyVector()
      #proxies = myMap.FindProxies("StaticMesh*")

      self.helicopter = None
      self.tree = None
      self.smoke = ParticleSystem()
      self.explosion = EffectManager()
      self.step = -1.0
      self.bufferExplosion = 1
      
      self.smoke.LoadFile("Particles/smoke.osg")
      self.explosion.AddDetonationTypeMapping(HighExplosiveDetonation, "Particles/explosion.osg")
      
      #for actor in proxies :
      #   # Find our helicopter by name
      #   if actor.GetName() == "StaticMesh0" :
      #      self.helicopter = actor.GetActor()
      #   # Find our tree by name
      #   if actor.GetName() == "StaticMesh1" :
      #      self.tree = actor.GetActor()
      
      # Error check
      #if self.helicopter is None :
      #   print "Failed to locate the helicopter"
      #   sys.exit()
      # 
      #if self.tree is None :
      #    print "Failed to find the tree"
      #    sys.exit()
     
      # move our tree away a little bit
      #self.tree.SetTransform(Transform(-50, 0, -1, 0, 0, 0))
      #self.smoke.SetEnabled(1)
      #self.GetScene().AddDrawable(explosion)

      # translate the camera back some
      self.GetCamera().SetTransform(Transform(-25, -100, 0, 0, 0, 0))

      # Set up a motion model so we may move the camera
      self.wmm = WalkMotionModel()
      self.wmm.SetDefaultMappings( self.GetKeyboard(), self.GetMouse() )
      self.wmm.SetTarget( self.GetCamera() )
   
   #def PreFrame(self, deltaFrameTime) :
      
      #tTree = Transform()
      #tHeli = Transform()

      #self.tree.GetTransform(tTree)
      #self.helicopter.GetTransform(tHeli)
        
      #tTreeTranslation = Vec3
      #tTree.GetTranslation(tTreeTranslation)
       
      #tHeliTranslation = Vec3
      #tHeli.GetTranslation(tHeliTranslation)
        
      # If the helicopter isn't at the tree yet, keep translating...
      #if tTreeTranslation.x() < tHeliTranslation.x() :
      #   self.helicopter.SetTransform(Transform(step, 0, 1, 90, 0, 0))
      ## It's there
      #elif tTreeTranslation.x() >= tHeliTranslation.x() :    
      #   x = 0.0
      #   y = 0.0
      #   z = 0.0
      #   tHeli.GetTranslation(x, y, z)
      #   heliPos = Vec3(x, y, z)

         # This is somewhat of a hack. The explosion will loop continuously in here
         # because of the conditions on which it is invoked. It eats framerate and 
         # causes the app to chug, not to mention doesn't even look like an explosion
         #if bufferExplosion :
         # 
         #   self.explosion.AddDetonation(heliPos)
         #   bufferExplosion = 0
         #   self.smoke.SetParent(None) #?
         #   self.helicopter.AddChild(smoke)
         #   self.smoke.SetEnabled(1)
         #   # Make sure to orient the new mesh to match the old one
         #   self.helicopter.SetTransform(Transform(x, y, 0, 90, -30, 0))
            
      # Reset the scene
      #if GetKeyboard().GetKeyState( KeyboardKey.Key_R ) :
      #    Reset()

      #step -= 0.05
      
   def Reset(self) :
      GetScene().AddDrawable(tree);
      self.helicopter.SetTransform(Transform(0, 0, 1, 90, 0, 0))
      self.helicopter.LoadFile("StaticMeshes/uh-1n.ive")
      self.smoke.SetEnabled(0)
      self.step = -0.00001
      self.bufferExplosion = 1

SetDataFilePathList (   GetDeltaRootPath() + "/examples/testMap/;" + 
                        GetDeltaDataPathList() )

app = TestMap('testMap.xml')

app.Config()
app.Run()