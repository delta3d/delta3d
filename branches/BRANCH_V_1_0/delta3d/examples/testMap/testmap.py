from PyDtCore import *
from PyDtABC import *
from PyDtDAL import *

class TestMap(Application):

   def Config(self) :
      
      # Steps to load a map into Delta3D
      # 1. Set the project context, just like in the editor
      # 2. Get a reference to the map
      # 3. Load the map into the scene through the project class
      contextName = GetDeltaDataPathList() + "/demoMap"
      
      Project.GetInstance().SetContext(contextName)
      myMap = Project.GetInstance().GetMap("MyCoolMap")
      Project.GetInstance().LoadMapIntoScene(myMap, GetScene())
      
      # Get the proxies from the map
      proxies = []
      myMap.FindProxies(proxies, "StaticMesh*")
      
      self.helicopter = None
      self.tree = None
      self.smoke = ParticleSystem()
      self.expolosion = EffectManager()
      self.step = -1.0
      self.bufferExplosion = 1
      
      smoke.LoadFile("Particles/smoke.osg")
      explosion.AddDetonationTypeMapping(HighExplosiveDetonation, "Particles/explosion.osg")
      
      for i in range(0, proxies.size()) :
         # Find our helicopter by name
         if proxies[i].GetName() == "StaticMesh0" :
            helicopter = proxies[i].GetActor()
         # Find our tree by name
         if proxies[i].GetName() == "StaticMesh1" :
            tree = proxies[i].GetActor()
      
      # Error check
      if helicopter is None :
         print "Failed to locate the helicopter\n"
         Quit()
      
      if tree is None :
         print "Failed to find the tree\n"
         Quit()
      
      # move our tree away a little bit
      tree.SetTransform(Transform(-50, 0, -1, 0, 0, 0))
      smoke.SetEnabled(1)
      self.GetScene().AddDrawable(explosion)

      # translate the camera back some
      self.GetCamera().SetTransform(Transform(-25, -100, 0, 0, 0, 0))

      # Set up a motion model so we may move the camera
      self.wmm = WalkMotionModel()
      self.wmm.SetDefaultMappings( self.GetKeyboard(), self.GetMouse() )
      self.wmm.SetTarget( self.GetCamera() )
   
   def PreFrame(self, deltaFrameTime) :
      
      tTree = Transform()
      tHeli = Transform()

      tree.GetTransform(tTree)
      helicopter.GetTransform(tHeli)
        
      tTreeTranslation = Vec3
      tTree.GetTranslation(tTreeTranslation)
       
      tHeliTranslation = Vec3
      tHeli.GetTranslation(tHeliTranslation)
        
      # If the helicopter isn't at the tree yet, keep translating...
      if tTreeTranslation.x() < tHeliTranslation.x() :
         helicopter.SetTransform(Transform(step, 0, 1, 90, 0, 0))
      # It's there
      elif tTreeTranslation.x() >= tHeliTranslation.x() :    
         x = 0.0
         y = 0.0
         z = 0.0
         tHeli.GetTranslation(x, y, z)
         heliPos = Vec3(x, y, z)

         # This is somewhat of a hack. The explosion will loop continuously in here
         # because of the conditions on which it is invoked. It eats framerate and 
         # causes the app to chug, not to mention doesn't even look like an explosion
         if bufferExplosion :
         
            explosion.AddDetonation(heliPos)
            bufferExplosion = 0
            smoke.SetParent(None) #?
            helicopter.AddChild(smoke)
            smoke.SetEnabled(1)
            # Make sure to orient the new mesh to match the old one
            helicopter.SetTransform(Transform(x, y, 0, 90, -30, 0))
            
      # Reset the scene
      #if(GetKeyboard()->GetKeyState(Producer::Key_R))
      #    Reset()

      step -= 0.05
      
   def Reset(self) :
      GetScene().AddDrawable(tree);
      self.helicopter.SetTransform(Transform(0, 0, 1, 90, 0, 0))
      self.helicopter.LoadFile("StaticMeshes/uh-1n.ive")
      self.smoke.SetEnabled(0)
      self.step = -0.00001
      self.bufferExplosion = 1

SetDataFilePathList (   GetDeltaRootPath() + "/examples/testMap/;" + 
                        GetDeltaDataPathList() )

app = TestMap()

app.Config()
app.Run()
