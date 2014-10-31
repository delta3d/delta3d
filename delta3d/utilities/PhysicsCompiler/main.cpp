/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
// OSG
#include <osgDB/ReadFile>
// DELTA3D
#include <dtABC/application.h>
#include <dtCore/project.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtGame/gamemanager.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/physicscompiler.h>
#include <dtPhysics/physicscomponent.h>
#include <dtPhysics/trianglerecordervisitor.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class ApplicationHandler : public osg::Referenced
{
public:
   ApplicationHandler()
   {}

   ~ApplicationHandler()
   {
      Shutdown();
   }

   bool IsValid()
   {
      return mCompiler.valid();
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
      mGM->LoadActorRegistry("dtPhysics");

      dtCore::Project::GetInstance().SetContext(context, true);

      /*try
      {
         dtCore::RefPtr<dtPhysics::PhysicsWorld> world = new dtPhysics::PhysicsWorld("bullet");
         world->Init();
         mPhysicsComp = new dtPhysics::PhysicsComponent(*world, false);
         mGM->AddComponent(*mPhysicsComp, dtGame::GameManager::ComponentPriority::NORMAL);
      
         mCompiler = new dtPhysics::PhysicsCompiler;
      }
      catch(dtUtil::Exception& ex)
      {
         std::cerr << "Exception: " << ex.ToString() << std::endl;
      }*/

      mCompiler = new dtPhysics::PhysicsCompiler;

      // Material indices willl be search from material actors.
      // This is more efficient than setting up a physics world instance.
      mCompiler->SetMaterialSearchByActor(mGM.get());

      // Set the method for the compiler to call when a geometry's
      // compilation completes.
      dtPhysics::GeometryCompiledCallback geomCompileCallback
         = dtPhysics::GeometryCompiledCallback(this, &ApplicationHandler::OnGeometryCompileCompleted);
      mCompiler->SetGeometryCompiledCallback(geomCompileCallback);
   }

   void Shutdown()
   {
      if (mApp.valid())
      {
         mApp->GetScene()->RemoveAllDrawables();
      }

      if (mGM.valid())
      {
         mGM->Shutdown();
         mGM = NULL;
      }
   }

   dtPhysics::PhysicsCompiler& GetCompiler()
   {
      return *mCompiler;
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

   bool SetDirectory(const std::string& dir)
   {
      bool success = false;

      if (!dir.empty())
      {
         mSaveDirectory = dtCore::Project::GetInstance().GetContext() + "/" + dir;

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

   std::string CreateFilePath(dtPhysics::PhysicsCompileResult& result)
   {
      // Construct the path and file name prefix.
      std::string filepath = GetDirectory() + "/" + GetFilePrefix()
         + result.mMaterialName;

      // Determine if a suffix should be added.
      if (result.mPartTotalInProgress > 1)
      {
         filepath += "_split" + dtUtil::ToString(result.mPartIndex + 1);
      }

      filepath += ".dtphys";

      return filepath;
   }

   void OnGeometryCompileCompleted(dtPhysics::PhysicsCompileResult& result)
   {
      std::string filepath(CreateFilePath(result));

      if ( ! WritePhysicsFile(*result.mVertData, filepath))
      {
         LOG_ERROR("Failed writing physics file: " + filepath);
      }
   }

   bool WritePhysicsFile(dtPhysics::VertexData& objData, const std::string& filepath)
   {
      //std::cout << "Parsed data file, resulting vertices: " << mv.mFunctor.mVertices.size() << ", resulting triangles: " << mv.mFunctor.mTriangles.size() << std::endl;

      bool result = dtPhysics::PhysicsReaderWriter::SaveTriangleDataFile(objData, filepath);
      return result;
   }

private:
   std::string mCurrentMap;
   std::string mSaveDirectory;
   std::string mFilePrefix;
   std::string mProjectContext;

   dtCore::RefPtr<dtABC::Application> mApp;
   dtCore::RefPtr<dtGame::GameManager> mGM;
   dtCore::RefPtr<dtCore::Scene> mScene;
   dtCore::RefPtr<dtPhysics::PhysicsComponent> mPhysicsComp;
   dtCore::RefPtr<dtPhysics::PhysicsCompiler> mCompiler;
};

//this is our global instance
dtCore::RefPtr<ApplicationHandler> GlobalApp;



////////////////////////////////////////////////////////////////
void CompileAndWritePhysicsFiles(osg::Node& node, dtPhysics::PhysicsCompileOptions& options)
{
   using namespace dtPhysics;

   VertexDataTable data;

   GlobalApp->GetCompiler().CompilePhysicsForNode(node, options, data);

   if (data.empty())
   {
      LOG_ERROR("Could not create physics geometry for node: " + node.getName());
      return;
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



////////////////////////////////////////////////////////////////////////////////
// MAIN PROGRAM
////////////////////////////////////////////////////////////////////////////////
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
   parser.getApplicationUsage()->addCommandLineOption("--maxTianglesPerMesh", "The number of triangles we try to put into each output file: default 300000.");
   parser.getApplicationUsage()->addCommandLineOption("--maxTriangleEdgeLength", "The maximum length of a triangle edge before it subdivides the triangle.  This helps physics stability: default 20.");

   dtPhysics::PhysicsCompileOptions options;

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

      if ( ! GlobalApp->IsValid())
      {
         std::cerr << "Error: Did not initialize successfully. Exiting program." << std::endl;
         return 1;
      }
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
      GlobalApp->GetCompiler().SetDefaultMaterialName(tempString);
   }
   else
   {
      //none specified so just use road
      GlobalApp->GetCompiler().SetDefaultMaterialName("Mat_Road");
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

   int tempInt = -1;
   if(parser.read("--maxVertsPerMesh", tempInt))
   {
       options.mMaxVertsPerMesh = tempInt;
   }

   parser.read("--maxTriangleEdgeLength", options.mMaxEdgeLength);

   osg::Node* ourNode = loadFile(parser[1]);
   CompileAndWritePhysicsFiles(*ourNode, options);

   return 0;
}
