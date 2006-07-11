/// Demo application illustrating map loading and actor manipulation
/// @author Eddie Johnson
/// @author Chris Osborn

#include <dtABC/dtabc.h>
#include <dtCore/dt.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtUtil/exception.h>
#include <vector>

using namespace dtCore;
using namespace dtDAL;
using namespace dtABC; 

class TestMap : public Application
{
   public:
   
   TestMap() :
      Application("testMap.xml"),
      mWmm(0),
      mHelicopter(0),
      mTree(0),
      mSmoke(0),
      mExplosionManager(0),
      mStep(-1.0f),
      mBufferExplosion(true)
   {
      // Set up a motion model so we may move the camera
      mWmm = new WalkMotionModel(GetKeyboard(), GetMouse());
      mWmm->SetTarget(GetCamera());
      
      // Steps to load a map into Delta3D
      // 1. Set the project context, just like in the editor
      // 2. Get a reference to the map
      // 3. Load the map into the scene through the project class
      std::string contextName = GetDeltaDataPathList() + "/demoMap";
      Project::GetInstance().SetContext(contextName);
      Map &myMap = Project::GetInstance().GetMap("MyCoolMap");
      
      //Since we are in an Application we can simply call...
      LoadMap(myMap);
      
      // But if wanted to load the map into another Scene we would
      // have to call:
      //Project::GetInstance().LoadMapIntoScene(myMap, *GetScene());
      
      // Get the proxies from the map
      std::vector< dtCore::RefPtr<ActorProxy> > proxies;
      myMap.FindProxies(proxies, "StaticMesh*");
      
      // Initialization
      mSmoke = new ParticleSystem;
      mExplosionManager = new EffectManager;
      mSmoke->LoadFile("Particles/smoke.osg");
      mExplosionManager->AddDetonationTypeMapping("HighExplosiveDetonation", "Particles/explosion.osg");
      
      for(unsigned int i = 0; i < proxies.size(); i++)
      {
         // Find our mHelicopter by name
         if(proxies[i]->GetName() == "StaticMesh0")
         {
            mHelicopter = dynamic_cast<Object*> (proxies[i]->GetActor());
         }
         
         // Find our mTree by name
         if(proxies[i]->GetName() == "StaticMesh1")
         {
            mTree = dynamic_cast<Object*> (proxies[i]->GetActor());
         }
      }
      
      // Error check
      if(!mHelicopter.valid())
      {
         std::cout << "Failed to locate the helicopter\n";
         Quit();
      }
      
      if(!mTree.valid())
      {
         std::cout << "Failed to find the tree\n";
         Quit();
      }
      
      // move our tree away a little bit
      Transform xform(-50.0f, 0.0f, -1.0f);
      mTree->SetTransform(&xform);
      mSmoke->SetEnabled(true);
      GetScene()->AddDrawable(mExplosionManager.get());
      
      // translate the camera back some
      xform.SetTranslation(-25.0f,-100.0f,0.0f);
      GetCamera()->SetTransform(&xform);
   }
   
   virtual void OnMessage(MessageData *data)
   {
      Transform tTree, tHeli;
      
      mTree->GetTransform(&tTree);
      mHelicopter->GetTransform(&tHeli);
      
      osg::Vec3 tTreeTranslation;
      tTree.GetTranslation(tTreeTranslation);
      
      osg::Vec3 tHeliTranslation;
      tHeli.GetTranslation(tHeliTranslation);
      
      // If the mHelicopter isn't at the tree yet, keep translating...
      if(tTreeTranslation.x() < tHeliTranslation.x())
      {
         Transform xform(mStep, 0.0f, 1.0f, 90.0f, 0.0f, 0.0f);
         mHelicopter->SetTransform(&xform);
      }
      // It's there
      else if(tTreeTranslation.x() >= tHeliTranslation.x())
      {
         float x, y, z;
         tHeli.GetTranslation(x, y, z);
         osg::Vec3 heliPos(x, y, z);
         
         // This is somewhat of a hack. The explosion will loop continuously in here
         // because of the conditions on which it is invoked. It eats framerate and 
            // causes the app to chug, not to mention doesn't even look like an explosion
         if(mBufferExplosion)
         {
            mExplosionManager->AddDetonation(heliPos);
            mBufferExplosion = false;
            mSmoke->SetParent(0);
            mHelicopter->AddChild(mSmoke.get());
            mSmoke->SetEnabled(true);
            // Make sure to orient the new mesh to match the old one
            Transform xform(x,y,0.0f,90.0f,-30.0f,0.0f);
            mHelicopter->SetTransform(&xform);
         }
      }
      
      // Reset the scene
      if(GetKeyboard()->GetKeyState(Producer::Key_R))
      {
         Reset();
      }
      
      mStep -= 0.05f;
   }
   
   // Re-initialize to default conditions
   void Reset()
   {
      GetScene()->AddDrawable(mTree.get());
      Transform xform(0.0f,0.0f,1.0f,90.0f,0.0f,0.0f);
      mHelicopter->SetTransform(&xform);
      mHelicopter->LoadFile("StaticMeshes/uh-1n.ive");
      mSmoke->SetEnabled(false);
      mStep = -1.0f;
      mBufferExplosion = true;
   }

protected:
   
   virtual ~TestMap()
   {
   }
   
private:

   RefPtr<WalkMotionModel> mWmm;
   RefPtr<Object> mHelicopter;
   RefPtr<Object> mTree;
   RefPtr<ParticleSystem> mSmoke;
   RefPtr<EffectManager>  mExplosionManager;
   float mStep;
   bool mBufferExplosion;
};

int main()
{
   dtCore::SetDataFilePathList( GetDeltaRootPath() + "/examples/testMap/;" +
                                GetDeltaDataPathList() + ";" +
                                GetDeltaDataPathList() + "/demoMap/Particles;" );
   RefPtr<TestMap> app;
   
   try
   {
      app = new TestMap;
      app->Config();
      app->Run();
   }
   catch (const dtUtil::Exception &e)
   {
      std::cout << e.What() << '\n';
   }

   return 0;
}
