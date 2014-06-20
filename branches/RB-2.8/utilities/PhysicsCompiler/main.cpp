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
#include <dtCore/project.h>
#include <dtGame/gamemanager.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>
#include <dtPhysics/physicsactorregistry.h>
#include <dtPhysics/physicsmaterialactor.h>
#include <dtPhysics/physicsreaderwriter.h>

#include <osgUtil/Simplifier>

#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>

#include <dtCore/project.h>

#include <cmath>

template <typename VecType>
bool IsVecFinite(const VecType value)
{
   for (size_t i = 0; i < VecType::num_components; ++i)
   {
#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
      if (!_finite(value[i]))
      {
         return false;
      }
#else
      if (!std::isfinite(value[i]))
      {
         return false;
      }
#endif
   }
   return true;
}

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
         mSaveDirectory = dtCore::Project::GetInstance().GetContext() + "/Terrains/" + dir;

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


////////////////////////////////////////////////////////////////
class TriangleRecorder
{
protected:
   ~TriangleRecorder(){}

public:
   TriangleRecorder()
      : mCurrentMaterial(0)
   {
      mData.mVertices = new osg::Vec3Array();
      mData.mFaces = new osg::UIntArray();
      mData.mMaterialFlags = new osg::UIntArray();
   }

   typedef std::map<osg::Vec3, int> VertexMap;

   int mCurrentMaterial;
   VertexMap mVertIndexSet;
   dtPhysics::PhysicsReaderWriter::PhysicsTriangleData mData;

   osg::Matrix mMatrix;

   /**
   * Called once for each visited triangle.
   *
   * @param v1 the triangle's first vertex
   * @param v2 the triangle's second vertex
   * @param v3 the triangle's third vertex
   * @param treatVertexDataAsTemporary whether or not to treat the vertex data
   * as temporary
   */
   void operator()(const osg::Vec3& v1,
      const osg::Vec3& v2,
      const osg::Vec3& v3,
      bool treatVertexDataAsTemporary)
   {
      //osg::Vec3 tv1 = osg::Matrix::transform3x3(v1, mMatrix);
      //osg::Vec3 tv2 = osg::Matrix::transform3x3(v2, mMatrix);
      //osg::Vec3 tv3 = osg::Matrix::transform3x3(v3, mMatrix);
      osg::Vec3 tv1 = v1*mMatrix,
         tv2 = v2*mMatrix,
         tv3 = v3*mMatrix;

      if(IsVecFinite(tv1) && IsVecFinite(tv2) && IsVecFinite(tv3))
      {
         VertexMap::iterator iter1 = mVertIndexSet.find(tv1);
         VertexMap::iterator iter2 = mVertIndexSet.find(tv2);
         VertexMap::iterator iter3 = mVertIndexSet.find(tv3);

         int index1, index2, index3;
         if(iter1 != mVertIndexSet.end())
         {
            index1 = (*iter1).second;
         }
         else
         {
            index1 = mData.mVertices->size();
            mData.mVertices->push_back(tv1);
            mVertIndexSet.insert(std::make_pair(tv1, index1));
         }

         if(iter2 != mVertIndexSet.end())
         {
            index2 = (*iter2).second;
         }
         else
         {
            index2 = mData.mVertices->size();
            mData.mVertices->push_back(tv2);
            mVertIndexSet.insert(std::make_pair(tv2, index2));
         }

         if(iter3 != mVertIndexSet.end())
         {
            index3 = (*iter3).second;
         }
         else
         {
            index3 = mData.mVertices->size();
            mData.mVertices->push_back(tv3);
            mVertIndexSet.insert(std::make_pair(tv3, index3));
         }

         mData.mFaces->push_back(index1);
         mData.mFaces->push_back(index2);
         mData.mFaces->push_back(index3);

         mData.mMaterialFlags->push_back(mCurrentMaterial);
      }
      else
      {
         std::ostringstream ss;
         ss << "Found non-finite triangle data.  The three vertices of the triangle are \"";
         ss << tv1 << "\", \"" << tv2 << "\", and \"" << tv3 << "\".";
         std::cerr << ss.str() << std::endl;
      }
   }
};

void Simplify(osg::Node* n)
{
   osgUtil::Simplifier simple;
   simple.setSmoothing(true);
   simple.setSampleRatio(0.4f);
   n->accept(simple);
}


////////////////////////////////////////////////////////////////
template< class T >
class DrawableVisitor : public osg::NodeVisitor
{
public:

   int mSplit, mSplitCount, mNumGeodes, mGeodeExportCounter;
   bool mExportSpecificMaterial, mSkipSpecificMaterial;
   std::string mSpecificDescription;
   std::string mCurrentDescription;
   osg::TriangleFunctor<T> mFunctor;

   /**
   * Constructor.
   */
   DrawableVisitor()
      : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ACTIVE_CHILDREN)
      , mSplit(-1)
      , mSplitCount(1)
      , mNumGeodes(0)
      , mGeodeExportCounter(0)
      , mExportSpecificMaterial(false)
      , mSkipSpecificMaterial(false)
   {}

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

   virtual void apply(osg::Node& node)
   {
      CheckDesc(node);
      osg::NodeVisitor::apply(node);
   }

   virtual void apply(osg::Group& gnode)
   {
      CheckDesc(gnode);
      osg::NodeVisitor::apply(gnode);
   }

   /**
   * Applies this visitor to a geode.
   *
   * @param node the geode to visit
   */
   virtual void apply(osg::Geode& node)
   {
      CheckDesc(node);
      //allow skipping one specific material or only exporting one material
      if((mExportSpecificMaterial && (mCurrentDescription != mSpecificDescription))
         || (mSkipSpecificMaterial && (mCurrentDescription == mSpecificDescription)))
      {
         //std::cout << "Skipping material: " << mCurrentDescription << std::endl;
         return;
      }

      if(mSplit != -1)
      {
         ++mGeodeExportCounter;
         int divCount = mNumGeodes / mSplitCount;
         if(((mGeodeExportCounter - 1) / divCount) != mSplit)
         {
            //skip this one since we are breaking it up into multiple parts
            return;
         }
         else
         {
            //std::cout << "Exporting tile number: " << mGeodeExportCounter << std::endl;
         }
      }

      //for some reason if we do this on the whole scene it crashes, so we are doing it per geode
      //Simplify(&node);

      dtPhysics::MaterialIndex matID = 0;
      if(GlobalApp.valid())
      {
          matID = GlobalApp->GetMaterialID(mSpecificDescription);
      }

      for(size_t i=0;i<node.getNumDrawables();i++)
      {
         osg::Drawable* d = node.getDrawable(i);

         if(d->supports(mFunctor))
         {
            osg::NodePath nodePath = getNodePath();
            mFunctor.mMatrix = osg::computeLocalToWorld(nodePath);
            mFunctor.mCurrentMaterial = matID;
            d->accept(mFunctor);
         }
      }
   }

   virtual void apply(osg::Billboard& node)
   {
      //do nothing
   }
};

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

bool CookMesh(DrawableVisitor<TriangleRecorder>& mv, const std::string& fileName)
{

   //std::cout << "Parsed data file, resulting vertices: " << mv.mFunctor.mVertices->size() << ", resulting triangles: " << mv.mFunctor.mTriangles.size() << std::endl;

   bool result = dtPhysics::PhysicsReaderWriter::SaveTriangleDataFile(mv.mFunctor.mData, fileName);
   return result;
}


////////////////////////////////////////////////////////////////
void CookPhysicsFromNode(osg::Node* node)
{
   /**
   * This will export each material as a separate physics object
   */
   std::set<std::string> descList;
   CollectDescVisitor cdv(descList);
   node->accept(cdv);

   //that last line prints out all the descriptions to the console so this will add some space
   std::cout << std::endl << std::endl << std::endl;

   std::set<std::string>::iterator iter = descList.begin();
   std::set<std::string>::iterator iterEnd = descList.end();
   for(;iter != iterEnd; ++iter)
   {
      DrawableVisitor<TriangleRecorder> mv;
      mv.mExportSpecificMaterial = true;
      mv.mSpecificDescription = (*iter);
      node->accept(mv);
      std::string materialName = (*iter);
      if ( materialName.empty() ) materialName = "_default_";

      //if we have too much geometry break it into multiple pieces
      if(mv.mFunctor.mData.mFaces->size() > 300000)
      {
         int exportCount = 1 + (mv.mFunctor.mData.mFaces->size() / 300000);
         std::cout << "Splitting material \"" << materialName << "\" into " << exportCount
                   << " pieces because it contains " << mv.mFunctor.mData.mFaces->size()
                   << " triangles, which exceeds the maximum size." << std::endl;

         GeodeCounter gc;
         gc.mExportSpecificMaterial = true;
         gc.mSpecificDescription = (*iter);
         node->accept(gc);

         for(int i = 0; i < exportCount; ++i)
         {
            DrawableVisitor<TriangleRecorder> mv2;
            mv2.mExportSpecificMaterial = true;
            mv2.mSpecificDescription = (*iter);
            mv2.mSplit = i;
            mv2.mSplitCount = exportCount;
            mv2.mNumGeodes = gc.mGeodeCount;

            node->accept(mv2);

            if ( mv2.mFunctor.mData.mFaces->empty() )
            {
                std::cout << std::endl << "Finished material: " << materialName << " with " << i << " files." << std::endl;
                break;
            }
            else
            {
                std::string fileWithPath = GlobalApp->GetDirectory() + "/" + GlobalApp->GetFilePrefix() + materialName + "_Split" + dtUtil::ToString(i + 1) + ".dtphys";
                std::cout << std::endl << "Cooking mesh for material name \"" << materialName << "\", with full path \"" << fileWithPath << "\"." << std::endl;
                if ( !CookMesh(mv2, fileWithPath) )
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
         if(mv.mFunctor.mData.mFaces->empty() || !CookMesh(mv, fileWithPath))
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

   osg::Node* ourNode = loadFile(parser[1]);
   CookPhysicsFromNode(ourNode);

   return 0;
}
