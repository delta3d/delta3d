// made for mts ship fire fighter level. this will prebuild the geometry.

#include <iostream>
#include <osg/ArgumentParser>
#include <osg/Matrix>
#include <osg/Vec3>
#include <vector>
#include <dtCore/refptr.h>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Drawable>
#include <osg/TriangleFunctor>
#include <osg/MatrixTransform>
#include <osg/NodeVisitor>
#include <osg/Referenced>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <sstream>
#include <osg/io_utils>
#include <set>

#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtABC/application.h>
#include <dtCore/shadermanager.h>
#include <dtCore/project.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtPhysics/physicsactorregistry.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicsreaderwriter.h>
#include <dtPhysics/trianglerecordervisitor.h>

#include <osgUtil/Simplifier>

#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/mathdefines.h>

#include <dtCore/project.h>

#include <cmath>

class GeodeCounter : public osg::NodeVisitor
{
public:

   GeodeCounter()
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
      , mNodeCounter(0)
      , mGeodeCount(0)
      , mDrawableCounter(0)
      , mExportSpecificMaterial(false)
      , mSkipSpecificMaterial(false)
   {}

   virtual void apply(osg::Node& node)
   {
      ++mNodeCounter;
      traverse(node);
   }

   void CheckDesc(osg::Node& node)
   {
      if(!node.getDescriptions().empty())
      {
         // Use *last* description as material tag
         mCurrentDescription = node.getDescription(node.getNumDescriptions()-1);
      }
      else
      {
         mCurrentDescription.clear();
      }
   }

   /**
   * Visits the specified geode.
   *
   * @param node the geode to visit
   */
   virtual void apply(osg::Geode& node)
   {
      CheckDesc(node);

      //allow skipping one specific material or only exporting one material
      if((mExportSpecificMaterial && (mCurrentDescription != mSpecificDescription)) || (mSkipSpecificMaterial && (mCurrentDescription == mSpecificDescription)))
      {
         //std::cout << "Skipping material: " << mCurrentDescription << std::endl;
         return;
      }

      ++mGeodeCount;
      mDrawableCounter += node.getNumDrawables();
      traverse(node);
   }

   unsigned mNodeCounter;
   unsigned mGeodeCount;
   unsigned mDrawableCounter;

   bool mExportSpecificMaterial, mSkipSpecificMaterial;
   std::string mSpecificDescription;
   std::string mCurrentDescription;

};

class ApplicationHandler : public osg::Referenced
{
public:
   ApplicationHandler()
      : mDefaultMatID(0)
   {

   }
   ~ApplicationHandler()
   {
      Shutdown();
   }

   void Init(const std::string& context)
   {
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());

      mApp = new dtABC::Application();
      mApp->Config();
      mScene = mApp->GetScene();

      dtCore::System::GetInstance().Config();
      dtCore::System::GetInstance().Start();

      mGM = new dtGame::GameManager(*mScene);
      mGM->SetApplication(*mApp);

      dtCore::Project::GetInstance().SetContext(context, true);
   }

   void Shutdown()
   {
      if (mApp.valid())
      {
         mApp->GetScene()->RemoveAllDrawables();
      }

      dtCore::ShaderManager::GetInstance().Clear();

      if (mGM.valid())
      {
         mGM->Shutdown();
         mGM = NULL;
      }
   }

   dtPhysics::MaterialIndex GetMaterialID(const std::string& commentFlag)
   {
      dtPhysics::MaterialIndex index = GetDefaultMaterialIndex();

      if(!commentFlag.empty())
      {
         typedef std::vector<dtCore::ActorProxy* > ProxyContainer;
         ProxyContainer proxies;

         mGM->FindActorsByType(*dtPhysics::PhysicsActorRegistry::PHYSICS_MATERIAL_ACTOR_TYPE, proxies);

         bool notFound = true;
         if(!proxies.empty())
         {
            ProxyContainer::iterator iter = proxies.begin();
            ProxyContainer::iterator iterEnd = proxies.end();

            for(;iter != iterEnd && notFound; ++iter)
            {
               dtPhysics::MaterialActor* actor = dynamic_cast<dtPhysics::MaterialActor*>((*iter)->GetDrawable());
               if(actor != NULL && actor->GetName() == commentFlag)
               {
                  index = dtPhysics::MaterialIndex(actor->GetMateralDef().GetMaterialIndex());
                  notFound = false;
               }
            }
         }

         if(notFound)
         {
            LOG_ERROR("Cannot find physics material with the name: " + commentFlag + ".");
         }
      }

      return index;
   }

   void LoadMap(const std::string& mapName)
   {
      //load map
      try
      {
         mGM->ChangeMap(mapName);

         //step a few times to ensure the map loaded
         dtCore::System::GetInstance().Step();
         dtCore::System::GetInstance().Step();
         dtCore::System::GetInstance().Step();
      }
      catch (dtUtil::Exception&)
      {
         LOG_ERROR("Unable to load map: " + mapName + ".");
      }

   }

   void SetDefaultMaterial(const std::string& mat)
   {
      mDefaultMaterial = mat;
      mDefaultMatID = GetMaterialID(mDefaultMaterial);
   }

   dtPhysics::MaterialIndex GetDefaultMaterialIndex()
   {
      return dtPhysics::MaterialIndex(mDefaultMatID);
   }

   bool SetDirectory(const std::string& dir)
   {
      bool success = false;

      if (!dir.empty())
      {
         mSaveDirectory = dtCore::Project::GetInstance().GetContext() + "/" + dtCore::DataType::STATIC_MESH.GetName() +  "/" + dir;

         if(!dtUtil::FileUtils::GetInstance().DirExists(mSaveDirectory))
         {
            std::cerr << "Directory \"" << mSaveDirectory << "\" does not exist. Create directory y/n?" << std::endl;
            char yesOrNo = 'n';
            std::cin >> yesOrNo;
            if(yesOrNo == 'y' || yesOrNo == 'Y')
            {
               dtUtil::FileUtils::GetInstance().MakeDirectory(mSaveDirectory);
               std::cerr << "Directory \"" << mSaveDirectory << "\" has been created." << std::endl;
               success = true;
            }
         }
         else
         {
            success = true;
         }
      }

      return success;
   }

   const std::string& GetDirectory() const
   {
      return mSaveDirectory;
   }

   void SetFilePrefix(const std::string& fix)
   {
      mFilePrefix = fix;
   }

   const std::string& GetFilePrefix() const
   {
      return mFilePrefix;
   }


private:
   unsigned mDefaultMatID;
   std::string mCurrentMap;
   std::string mSaveDirectory;
   std::string mFilePrefix;
   std::string mProjectContext;
   std::string mDefaultMaterial;

   dtCore::RefPtr<dtABC::Application> mApp;
   dtCore::RefPtr<dtGame::GameManager> mGM;
   dtCore::RefPtr<dtCore::Scene> mScene;
};

//this is our global instance
dtCore::RefPtr<ApplicationHandler> GlobalApp;



void Simplify(osg::Node* n)
{
   osgUtil::Simplifier simple;
   simple.setSmoothing(true);
   simple.setSampleRatio(0.4f);
   n->accept(simple);
}



////////////////////////////////////////////////////////////////
class CollectDescVisitor : public osg::NodeVisitor
{
public:

   std::string mCurrentDescription;
   std::set<std::string>& mDescriptionList;

   /**
   * Constructor.
   */
   CollectDescVisitor(std::set<std::string>& desc)
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
      , mDescriptionList(desc)
   {}

   void CheckDesc(osg::Node& node)
   {
      if( node.getDescriptions().empty() )
      {
         std::cout << "---Geode \"" << node.getName() << "\" has no description" << std::endl;
         mCurrentDescription.clear();
      }
      else
      {
         if(node.getNumDescriptions() > 1)
         {
             std::cout << "Geode " << node.getName() << " has multiple descriptions" << std::endl;
             for ( size_t i=0; i<node.getNumDescriptions(); i++ )
             {
                 // Use *last* description as material flag
                 mCurrentDescription = node.getDescription(i);
                 std::cout << "  \"" << mCurrentDescription << "\"" << std::endl;
             }
         }
         else
         {
             mCurrentDescription = node.getDescription(0);
             std::cout << "Geode " << node.getName() << " has description \"" << node.getDescription(0) << "\"" << std::endl;
         }
      }
   }

   /**
   * Applies this visitor to a geode.
   *
   * @param node the geode to visit
   */
   virtual void apply(osg::Geode& node)
   {
      CheckDesc(node);
      mDescriptionList.insert(mCurrentDescription);
   }

   virtual void apply(osg::Billboard& node)
   {
      //do nothing
   }
};

bool CookMesh(dtPhysics::TriangleRecorder& tr, const std::string& fileName)
{

   //std::cout << "Parsed data file, resulting vertices: " << mv.mFunctor.mVertices.size() << ", resulting triangles: " << mv.mFunctor.mTriangles.size() << std::endl;

   bool result = dtPhysics::PhysicsReaderWriter::SaveTriangleDataFile(*tr.mData, fileName);
   return result;
}


////////////////////////////////////////////////////////////////
void CookPhysicsFromNode(osg::Node* node, float maxPerMesh, float maxEdge)
{
   /**
   * This will export each material as a separate physics object
   */
   std::set<std::string> descList;
   CollectDescVisitor cdv(descList);
   node->accept(cdv);

   //that last line prints out all the descriptions to the console so this will add some space
   std::cout << std::endl << std::endl << std::endl;

   dtPhysics::TriangleRecorder::MaterialLookupFunc materialLookup(GlobalApp.get(), &ApplicationHandler::GetMaterialID);

   std::set<std::string>::iterator iter = descList.begin();
   std::set<std::string>::iterator iterEnd = descList.end();
   for(;iter != iterEnd; ++iter)
   {
      dtPhysics::TriangleRecorderVisitor<dtPhysics::TriangleRecorder> mv(materialLookup);
      mv.mFunctor.SetMaxEdgeLength(maxEdge);
      mv.mExportSpecificMaterial = true;
      mv.mSpecificDescription = (*iter);
      node->accept(mv);
      std::string materialName = (*iter);
      if ( materialName.empty() ) materialName = "_default_";

      GeodeCounter gc;
      gc.mExportSpecificMaterial = true;
      gc.mSpecificDescription = (*iter);
      gc.mGeodeCount = 1;
      if(mv.mFunctor.mData->mIndices.size() > maxPerMesh)
      {
         node->accept(gc);
      }

      // if we have too much geometry break it into multiple pieces
      // but we can't break it up smaller than per geode.
      if(gc.mGeodeCount > 1)
      {
         unsigned exportCount = 1 + (mv.mFunctor.mData->mIndices.size() / maxPerMesh);
         if (exportCount > gc.mGeodeCount)
         {
            exportCount = gc.mGeodeCount;
         }
         std::cout << "Splitting material \"" << materialName << "\" into " << exportCount
                   << " pieces because it contains " << mv.mFunctor.mData->mIndices.size()
                   << " triangles, which exceeds the maximum size." << std::endl;

         for (unsigned i = 0; i <= exportCount; ++i)
         {
            dtPhysics::TriangleRecorderVisitor<dtPhysics::TriangleRecorder> mv2(materialLookup);
            mv2.mExportSpecificMaterial = true;
            mv2.mSpecificDescription = (*iter);
            mv2.mSplit = i;
            mv2.mSplitCount = exportCount;
            mv2.mNumGeodes = gc.mGeodeCount;

            node->accept(mv2);

            if ( mv2.mFunctor.mData->mIndices.empty() )
            {
                std::cout << std::endl << "Finished material: " << materialName << " with " << i << " files." << std::endl;
                break;
            }
            else
            {
                std::string fileWithPath = GlobalApp->GetDirectory() + "/" + GlobalApp->GetFilePrefix() + materialName + "_Split" + dtUtil::ToString(i + 1) + ".dtphys";
                std::cout << std::endl << "Cooking mesh for material name \"" << materialName << "\", with full path \"" << fileWithPath << "\"." << std::endl;
                if ( !CookMesh(mv2.mFunctor, fileWithPath) )
                {
                    std::cout << std::endl << "Error cooking mesh for material: " << materialName << std::endl;
                }
            }
         }
      }
      else
      {
         std::string fileWithPath = GlobalApp->GetDirectory() + "/" + GlobalApp->GetFilePrefix() + materialName + ".dtphys";
         std::cout << std::endl << "Cooking mesh for material name \"" << materialName << "\", with full path \"" << fileWithPath << "\"." << std::endl;
         if (mv.mFunctor.mData->mIndices.empty() || !CookMesh(mv.mFunctor, fileWithPath))
         {
            std::cout << std::endl << "Error cooking mesh for material: " << materialName << std::endl;
         }
      }
   }
}


osg::Node* loadFile(const std::string& filename)
{
   dtCore::RefPtr<osgDB::ReaderWriter::Options> options = new osgDB::ReaderWriter::Options;
   options->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_NONE);

   osg::Node *model = osgDB::readNodeFile(filename, options.get());
   if (model != 0)
   {
      printf("Model found: %s \n", filename.c_str());
      return model;
   }
   else
   {
      printf("Model %s not found.\n", filename.c_str());
      return NULL;
   }
}


int main(int argc, char** argv)
{
   osg::ArgumentParser parser(&argc, argv);

   parser.getApplicationUsage()->setApplicationName(parser.getApplicationName());
   parser.getApplicationUsage()->setCommandLineUsage(parser.getApplicationName()+" [options] filename ...");
   parser.getApplicationUsage()->addCommandLineOption("-h or --help", "Display command line options");

   parser.getApplicationUsage()->addCommandLineOption("--projectPath", "The path (either relative or absolute) to the project context you wish to use. This defaults to the current working directory.");
   parser.getApplicationUsage()->addCommandLineOption("--mapName", "The name of the map to load in. This must be a map that is located within the project path specified");
   parser.getApplicationUsage()->addCommandLineOption("--defaultMaterial", "The name of material to use by default.");
   parser.getApplicationUsage()->addCommandLineOption("--directoryName", "The name of directory within the Terrains folder to save the physics files.");
   parser.getApplicationUsage()->addCommandLineOption("--filePrefix", "The prefix to use for each file saved out, the prefix will be followed directly by the material name.");
   parser.getApplicationUsage()->addCommandLineOption("--maxPerMesh", "The number of triangles we try to put into each output file: default 300000.");
   parser.getApplicationUsage()->addCommandLineOption("--maxTriangleEdgeLength", "The maximum length of a triangle edge before it subdivides the triangle.  This helps physics stability: default 20.");

   int maxPerMesh = 300000;

   if (parser.argc()<=1)
   {
      parser.getApplicationUsage()->write(std::cerr, osg::ApplicationUsage::COMMAND_LINE_OPTION);
      return 1;
   }
   else if (parser.errors())
   {
      parser.writeErrorMessages(std::cerr);
      return 1;
   }
   else if(parser.read("-h") || parser.read("--help") || parser.read("-?") || parser.read("--?"))
   {
      parser.getApplicationUsage()->write(std::cerr);
      return 1;
   }

   std::string tempString;

   GlobalApp = new ApplicationHandler();

   if (parser.read("--projectPath", tempString))
   {
      GlobalApp->Init(tempString);
   }
   else
   {
      std::cerr << "Error: no project path specified." << std::endl;
      return 1;
   }

   if(parser.read("--mapName", tempString))
   {
      GlobalApp->LoadMap(tempString);
   }
   else
   {
      std::cerr << "Error: no material map specified" << std::endl;
      return 1;
   }

   if(parser.read("--defaultMaterial", tempString))
   {
      GlobalApp->SetDefaultMaterial(tempString);
   }
   else
   {
      //none specified so just use road
      GlobalApp->SetDefaultMaterial("Mat_Road");
   }

   if(parser.read("--directoryName", tempString) && GlobalApp->SetDirectory(tempString))
   {
      std::cout << "directoryName successfully set to \"" << tempString << "\"" << std::endl;
   }
   else
   {
      std::cerr << "Error: no save directory specified, refers to a directory in the Terrains folder, e.g. \"--directoryName Physics\"" << std::endl;
      return 1;
   }

   if(parser.read("--filePrefix", tempString))
   {
      GlobalApp->SetFilePrefix(tempString);
   }
   else
   {
      std::cerr << "Error: no file prefix specified, e.g. \"--filePrefix Terrain\"" << std::endl;
      return 1;
   }

   int tempInt = -1;
   if(parser.read("--maxPerMesh", tempInt))
   {
       maxPerMesh = tempInt;
   }

   float maxEdge = 20.0f;
   parser.read("--maxTriangleEdgeLength", maxEdge);

   osg::Node* ourNode = loadFile(parser[1]);
   CookPhysicsFromNode(ourNode, maxPerMesh, maxEdge);

   GlobalApp->Shutdown();

   return 0;
}
