#include "dtCore/dt.h"
#include "dtABC/dtabc.h"
#include "dtHLA/dthla.h"

#include <osg/Billboard>
#include <osg/Geode>
#include <osg/Image>
#include <osg/PolygonMode>
#include <osg/Shape>
#include <osg/ShapeDrawable>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

using namespace dtCore;
using namespace dtABC;
using namespace dtHLA;
using namespace osg;
using namespace std;

/**
 * HLA stealth viewer application.
 */
class HLAStealthViewerApplication : public Application
{
   DECLARE_MANAGEMENT_LAYER(HLAStealthViewerApplication)
   
   
   public:
   
      /**
       * Constructor.
       *
       * @param executionName the name of the federation execution to join
       * @param fedFilename the name of the fed file to use
       */
      HLAStealthViewerApplication(string executionName, string fedFilename)
         : Application("config.xml"),
           mExecutionName(executionName),
           mFedFilename(fedFilename)
      {
         GetWindow()->SetWindowTitle("HLA Stealth Viewer");

         SetDataFilePathList("./data;../../data;./data/images;" + GetDeltaDataPathList());
         
         mEffectManager = new EffectManager;
         
         GetScene()->AddDrawable(mEffectManager.get());
         
         mRTIConnection = new RTIConnection;
         
         mRTIConnection->SetScene(GetScene());
         
         mRTIConnection->SetEffectManager(mEffectManager.get());
         
         mRTIConnection->LoadEntityTypeMappings("entitytypemappings.xml");
         
         mRTIConnection->EnableGlobeMode();
         
         mOrbitMotionModel = new OrbitMotionModel(GetKeyboard(), GetMouse());
         
         mOrbitMotionModel->SetTarget(GetCamera());
         
         mOrbitMotionModel->SetDistance(350.0f);
         
         GetScene()->GetSceneHandler()->GetSceneView()->setClearColor(Vec4(0, 0, 0, 1));
         
         mEarth = new dtCore::Object;
         
         mEarth->LoadFile("earth.osg");
         
         GetScene()->AddDrawable(mEarth.get());
         
         mRTIConnection->SetGlobeRadius(80.0f);
         
         dtCore::Transform transform(0, -350, 0);
         
         GetCamera()->SetTransform(&transform);
      }
      
      /**
       * Configures the application.
       */
      virtual void Config()
      {
         Application::Config();
         
         mRTIConnection->JoinFederationExecution(
            mExecutionName, 
            mFedFilename,
            "StealthViewer"
         );
      }
      
      /**
       * Cleans up after the application.
       */
      virtual void Cleanup()
      {
         mRTIConnection->LeaveFederationExecution();
      }
      
      
   private:
      
      /**
       * The name of the federation execution to join.
       */
      string mExecutionName;
      
      /**
       * The name of the fed filename to use.
       */
      string mFedFilename;
      
      /**
       * The effect manager.
       */
      ref_ptr<EffectManager> mEffectManager;
      
      /**
       * The RTI connection object.
       */
      ref_ptr<RTIConnection> mRTIConnection;
      
      /**
       * Orbit motion model for the camera.
       */
      ref_ptr<OrbitMotionModel> mOrbitMotionModel;
      
      /**
       * The Earth object.
       */
      ref_ptr<dtCore::Object> mEarth;
};

IMPLEMENT_MANAGEMENT_LAYER(HLAStealthViewerApplication)

int main( int argc, char **argv )
{
   bool printHelp = false;
   string executionName = "dtCore";
   string fedFilename = "RPR-FOM.fed";
   
   for(int i=1;i<argc;i++)
   {
      if(argv[i][0] == '-')
      {
         string command = argv[i] + 1;
         
         if(command == "h" || command == "?" || command == "help")
         {
            printHelp = true;
         }
         else if(command == "execution")
         {
            i += 1;
            
            if(i == argc || argv[i][0] == '-')
            {
               cerr << "Missing argument: federation execution name" << endl;
            }
            else
            {
               executionName = argv[i];
            }
         }
         else if(command == "fed")
         {
            i += 1;
            
            if(i == argc || argv[i][0] == '-')
            {
               cerr << "Missing argument: fed filename" << endl;
            }
            else
            {
               fedFilename = argv[i];
            }
         }
         else
         {
            cerr << "Unrecognized argument: " << command << endl;
         }
      }   
   }
   
   if(printHelp)
   {
      cout << "usage: hlaStealthViewer [-options]" << endl;
      cout << endl;
      cout << "where options include:" << endl;
      cout << "    -execution <federation execution name>" << endl;
      cout << "    -fed <fed filename>" << endl;
      return 1;
   }
   
   HLAStealthViewerApplication* hlaStealthViewerApp = 
      new HLAStealthViewerApplication(executionName, fedFilename);
   
   hlaStealthViewerApp->Config();
   hlaStealthViewerApp->Run();
   hlaStealthViewerApp->Cleanup();

   return 0;
}
