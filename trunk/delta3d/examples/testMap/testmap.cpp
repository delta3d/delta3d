/// Demo application illustrating map loading and actor manipulation
/// @author Eddie Johnson

#include <dtABC/dtabc.h>
#include <dtCore/dt.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtUtil/exception.h>
#include <vector>

using namespace dtCore;
using namespace dtDAL;
using namespace dtABC; 

class TestApp : public Application
{
public:

   TestApp() : Application("testMap.xml")
    {
        // Set up a motion model so we may move the camera
        wmm = new WalkMotionModel(GetKeyboard(), GetMouse());
        wmm->SetTarget(GetCamera());

        // Steps to load a map into Delta3D
        // 1. Set the project context, just like in the editor
        // 2. Get a reference to the map
        // 3. Load the map into the scene through the project class
        std::string contextName = GetDeltaDataPathList() + "/demoMap";
        Project::GetInstance().SetContext(contextName);
        Map &myMap = Project::GetInstance().GetMap("MyCoolMap");
        Project::GetInstance().LoadMapIntoScene(myMap, *GetScene());

        // Get the proxies from the map
        std::vector< osg::ref_ptr<ActorProxy> > proxies;
        myMap.FindProxies(proxies, "StaticMesh*");

        // Initialization
        helicopter = tree = NULL;
        smoke = new ParticleSystem;
        explosion = new EffectManager;
        step = -1.0f;
        bufferExplosion = true;
        smoke->LoadFile("Particles/smoke.osg");
        explosion->AddDetonationTypeMapping(HighExplosiveDetonation, "Particles/explosion.osg");

        for(unsigned int i = 0; i < proxies.size(); i++)
        {
            // Find our helicopter by name
            if(proxies[i]->GetName() == "StaticMesh0")
                helicopter = dynamic_cast<Object*> (proxies[i]->GetActor());

            // Find our tree by name
            if(proxies[i]->GetName() == "StaticMesh1")
                tree = dynamic_cast<Object*> (proxies[i]->GetActor());
        }

        // Error check
        if(!helicopter)
        {
           std::cout << "Failed to locate the helicopter\n";
            Quit();
        }

        if(!tree)
        {
            std::cout << "Failed to find the tree\n";
            Quit();
        }

        // move our tree away a little bit
        tree->SetTransform(new Transform(-50, 0, -1, 0, 0, 0));
        smoke->SetEnabled(true);
        GetScene()->AddDrawable(explosion);

        // translate the camera back some
        GetCamera()->SetTransform(new Transform(-25, -100, 0, 0, 0, 0));

        Config();
    }

    virtual void OnMessage(MessageData *data)
    {
        Transform tTree, tHeli;

        tree->GetTransform(&tTree);
        helicopter->GetTransform(&tHeli);
        
        osg::Vec3 tTreeTranslation;
        tTree.GetTranslation(tTreeTranslation);
        
        osg::Vec3 tHeliTranslation;
        tHeli.GetTranslation(tHeliTranslation);
        
        // If the helicopter isn't at the tree yet, keep translating...
        if(tTreeTranslation.x() < tHeliTranslation.x())
            helicopter->SetTransform(new Transform(step, 0, 1, 90, 0, 0));

        // It's there
        else if(tTreeTranslation.x() >= tHeliTranslation.x())
        {
            float x, y, z;
            tHeli.GetTranslation(x, y, z);
            osg::Vec3 heliPos(x, y, z);

            // This is somewhat of a hack. The explosion will loop continuously in here
            // because of the conditions on which it is invoked. It eats framerate and 
            // causes the app to chug, not to mention doesn't even look like an explosion
            if(bufferExplosion)
            {
                explosion->AddDetonation(heliPos);
                bufferExplosion = false;
                smoke->SetParent(NULL);
                helicopter->AddChild(smoke);
                smoke->SetEnabled(true);
                // Make sure to orient the new mesh to match the old one
                helicopter->SetTransform(new Transform(x, y, 0, 90, -30, 0));
            }
        }

        // Reset the scene
        if(GetKeyboard()->GetKeyState(Producer::Key_R))
            Reset();

        step -= 0.05f;
    }

    // Re-initialize to default conditions
    void Reset()
    {
        GetScene()->AddDrawable(tree);
        helicopter->SetTransform(new Transform(0, 0, 1, 90, 0, 0));
        helicopter->LoadFile("StaticMeshes/uh-1n.ive");
        smoke->SetEnabled(false);
        step = -0.00001f;
        bufferExplosion = true;
    }

    virtual ~TestApp()
    {
        
    }

private:

    WalkMotionModel *wmm;
    Object *helicopter, *tree;
    ParticleSystem *smoke;
    EffectManager  *explosion;
    float step;
    bool bufferExplosion;
};

int main()
{
   dtCore::SetDataFilePathList( GetDeltaRootPath() + "/examples/testMap/;" +
                                 dtCore::GetDeltaDataPathList() + ";" +
                                 GetDeltaDataPathList()+"/effects/" );
   RefPtr<TestApp> app;
   
   try
   {
      app = new TestApp;
      app->Run();
   }
   catch (const dtUtil::Exception &e)
   {
      std::cout << e.What() << '\n';
   }

   return 0;
}
