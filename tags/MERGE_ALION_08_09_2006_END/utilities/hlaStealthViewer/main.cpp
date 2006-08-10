#include <dtABC/application.h>
#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtHLA/rticonnection.h>

#include <iostream>
#include <string>

using namespace dtCore;
using namespace dtABC;
using namespace dtHLA;

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
      HLAStealthViewerApplication(const std::string& executionName, const std::string& fedFilename)
         : Application(),
           mExecutionName(executionName),
           mFedFilename(fedFilename)
      {
         GetWindow()->SetWindowTitle("HLA Stealth Viewer");

         SetDataFilePathList(	GetDeltaRootPath()+"/utilities/hlaStealthViewer/data;"+
								      GetDeltaRootPath()+"/utilities/hlaStealthViewer/data/images;" + 
								      GetDeltaDataPathList() );
         
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
         
         GetCamera()->SetClearColor(0.f, 0.f, 0.f, 1.f);
         
         mEarth = new Object;
         
         mEarth->LoadFile("earth.osg");
         
         GetScene()->AddDrawable(mEarth.get());
         
         mRTIConnection->SetGlobeRadius(80.0f);
         
         Transform transform(0, -350, 0);
         
         GetCamera()->SetTransform(&transform);
      }
      
      /**
       * Configures the application.
       */
      virtual void Config()
      {
         Application::Config();
         
         mRTIConnection->JoinFederationExecution(  mExecutionName, 
                                                   mFedFilename,
                                                   "StealthViewer" );
      }
      
      /**
       * Cleans up after the application.
       */
      virtual void Cleanup()
      {
         mRTIConnection->LeaveFederationExecution();
      }

   protected:

      virtual ~HLAStealthViewerApplication() {}
            
   private:
      
      /**
       * The name of the federation execution to join.
       */
      std::string mExecutionName;
      
      /**
       * The name of the fed filename to use.
       */
      std::string mFedFilename;
      
      /**
       * The effect manager.
       */
      RefPtr<EffectManager> mEffectManager;
      
      /**
       * The RTI connection object.
       */
      RefPtr<RTIConnection> mRTIConnection;
      
      /**
       * Orbit motion model for the camera.
       */
      RefPtr<OrbitMotionModel> mOrbitMotionModel;
      
      /**
       * The Earth object.
       */
      RefPtr<Object> mEarth;
};

IMPLEMENT_MANAGEMENT_LAYER(HLAStealthViewerApplication)

int main( int argc, char **argv )
{
   bool printHelp = false;
   std::string executionName = "dtCore";
   std::string fedFilename = "RPR-FOM.fed";
   
   for(int i=1;i<argc;i++)
   {
      if(argv[i][0] == '-')
      {
         std::string command = argv[i] + 1;
         
         if(command == "h" || command == "?" || command == "help")
         {
            printHelp = true;
         }
         else if(command == "execution")
         {
            i += 1;
            
            if(i == argc || argv[i][0] == '-')
            {
               std::cerr << "Missing argument: federation execution name" << std::endl;
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
               std::cerr << "Missing argument: fed filename" << std::endl;
            }
            else
            {
               fedFilename = argv[i];
            }
         }
         else
         {
            std::cerr << "Unrecognized argument: " << command << std::endl;
         }
      }   
   }
   
   if(printHelp)
   {
      std::cout << "usage: hlaStealthViewer [-options]" << std::endl;
      std::cout << std::endl;
      std::cout << "where options include:" << std::endl;
      std::cout << "    -execution <federation execution name>" << std::endl;
      std::cout << "    -fed <fed filename>" << std::endl;
      return 1;
   }
   
   RefPtr<HLAStealthViewerApplication> hlaStealthViewerApp = 
      new HLAStealthViewerApplication(executionName, fedFilename);
   
   hlaStealthViewerApp->Config();
   hlaStealthViewerApp->Run();
   hlaStealthViewerApp->Cleanup();

   return 0;
}
