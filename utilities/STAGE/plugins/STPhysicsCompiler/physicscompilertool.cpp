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
 *
 * @author Chris Rodgers
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "physicscompilertool.h"
// DELTA
#include <dtCore/deltadrawable.h>
#include <dtCore/actorfactory.h>
#include <dtCore/project.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>
#include <dtGame/actorcomponentcontainer.h>
#include <dtPhysics/physicsactcomp.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/threadpool.h>
// QT
#include <QtCore/QSettings>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtGui/QMouseEvent>

using namespace dtPhysics;



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const std::string PhysicsCompilerToolPlugin::PLUGIN_NAME("Physics Compiler Tool");
static const QString PLUGIN_TITLE(PhysicsCompilerToolPlugin::PLUGIN_NAME.c_str());
static const QString PLUGIN_SETTINGS_NAME("STAGE");
// File Settings
static const QString SETTING_FILE_OPTIONS_ENABLED("FileOptionsEnabled");
static const QString SETTING_TARGET_DIRECTORY("TargetDirectory");
static const QString SETTING_FILE_PREFIX("FilePrefix");
static const QString SETTING_FILE_SUFFIX("FileSuffix");
// Compile Settings
static const QString SETTING_INPUT_MESH_FILE("InputMeshFile");
static const QString SETTING_INPUT_MESH_FILE_ENABLED("InputMeshFileEnabled");
static const QString SETTING_MAX_VERTS_PER_MESH("MaxVertsPerMesh");
static const QString SETTING_MAX_EDGE_LENGTH("MaxEdgeLength");
static const QString SETTING_TRIANGLE_SUBDIVISION_ENABLED("TriangleSubdivisionEnabled");
// Object Settings
static const QString SETTING_MASS("Mass");
static const QString SETTING_COLLISION_MARGIN("CollisionMargin");
static const QString SETTING_CLEAR_EXISTING_OBJECTS("ClearExistingObjects");



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
PhysicsFileOptions::PhysicsFileOptions()
   : mTargetDataType(&dtCore::DataType::STATIC_MESH)
   , mTargetContextSlot(dtCore::Project::DEFAULT_SLOT_VALUE)
{}

void PhysicsFileOptions::SetTargetDirectory(const std::string& dir)
{
   mTargetDir = dir;

   std::string contextPath;
   dtCore::Project::ContextSlot slot = dtCore::Project::DEFAULT_SLOT_VALUE;
   dtCore::Project& proj = dtCore::Project::GetInstance();
   try
   {
      slot = proj.GetContextSlotForPath(dir);
      contextPath = proj.GetContext(slot);

      dtUtil::FindAndReplace(contextPath, "\\", "/");
   }
   catch (dtUtil::Exception& ex)
   {
      LOG_ERROR(ex.ToString());
   }
   catch (...)
   {
      LOG_ERROR("Unknown exception hit.");
   }

   if (contextPath.empty())
   {
      LOG_ERROR("No context path set.");
      return;
   }

   std::string delimiter;
   delimiter += dtCore::ResourceDescriptor::DESCRIPTOR_SEPARATOR;
   
   // Determine a project category from the directory path.
   std::string category(dir);
   size_t index = category.find(contextPath);
   if (index != std::string::npos)
   {
      // Remove the context part of the path.
      category = category.substr(index + contextPath.length());

      // Trim any leading slash.
      if (category.at(0) == '/')
      {
         category = category.substr(1);
      }

      // The relative path can be changed to a
      // descriptor by changing the delimiter.
      dtUtil::FindAndReplace(category, "/", delimiter);
   }

   // Determine the datatype from the category if it exists.
   // NOTE: This block ought to be removed when datatype is
   // no longer used to enforce a specific rigid directory structure.
   // This is yucky but has to be done until a refactor happens.
   const dtCore::DataType* dataType = &dtCore::DataType::STATIC_MESH;
   if ( ! category.empty())
   {
      std::string typeStr = category;
      size_t index = category.find(delimiter);
      bool hasSubDirectory = index != std::string::npos;
      if (hasSubDirectory)
      {
         // Top directory in project determines datatype.
         typeStr = category.substr(0, index);

         // Remove the directory from the category string.
         category = category.substr(index + 1);
      }

      const dtCore::DataType* actualDataType = dtCore::DataType::GetValueForName(typeStr);
      if (actualDataType != NULL)
      {
         dataType = actualDataType;

         // If the category is the only directory...
         if ( ! hasSubDirectory)
         {
            // ...clear it since it has determined the type.
            category.clear();
         }
      }
   }

   mTargetDirAsCategory = category;
   mTargetDataType = dataType;
   mTargetContextSlot = slot;
}



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class PhysicsCompileThread : public dtUtil::ThreadPoolTask
{
public:
   PhysicsCompileThread(PhysicsCompilerToolPlugin& tool,
      osg::Node& node)
      : mTool(&tool)
      , mNode(&node)
      , mVertData(NULL)
   {}

   virtual ~PhysicsCompileThread()
   {}

   virtual void operator()()
   {
      PhysicsCompiler& compiler = mTool->GetCompiler();

      compiler.CompilePhysicsForNode(*mNode, mCompileOptions, *mVertData);

      PhysicsObjectArray objArray;
      if (mVertData != NULL && ! mVertData->empty())
      {
         TriangleRecorder::VertexDataArray* curArray = NULL;
         VertexDataTable::iterator curIter = mVertData->begin();
         VertexDataTable::iterator endIter = mVertData->end();
         for (; curIter != endIter; ++curIter)
         {
            curArray = &curIter->second;
            compiler.CreatePhysicsObjectsForGeometry(mObjectOptions, *curArray, objArray);
         }
      }

      mTool->OnCompileCompleted_CompileThread(objArray);
   }

   PhysicsCompilerToolPlugin* mTool;
   dtCore::RefPtr<osg::Node> mNode;
   VertexDataTable* mVertData;
   dtPhysics::PhysicsCompileOptions mCompileOptions;
   dtPhysics::PhysicsObjectOptions mObjectOptions;
};



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
PhysicsCompilerToolPlugin::PhysicsCompilerToolPlugin(dtEditQt::MainWindow* mw)
   : mMainWindow(mw)
   , mIsCompiling(false)
{
   setWidget(new QWidget(this));
   mUI.setupUi(widget());

   setWindowTitle(tr(PhysicsCompilerToolPlugin::PLUGIN_NAME.c_str()));

   mCompiler = new PhysicsCompiler;
   GeometryCompiledCallback compileCompleteFunc(this, &PhysicsCompilerToolPlugin::OnCompileResult_CompileThread);
   mCompiler->SetGeometryCompiledCallback(compileCompleteFunc);
   
   //add dock widget to STAGE main window
   mw->addDockWidget(Qt::RightDockWidgetArea, this);
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::Create()
{
   LoadSettings();

   SetupComboboxes();

   // Set default values on UI before setting up connections
   // so that needless signal processing does not occur or interfere.
   UpdateUI();

   mUI.mProgress->setVisible(false);

   CreateConnections();

   EnsurePhysicsWorld();

   // Disable until actors are selected.
   SetPanelEnabled(false);
}

////////////////////////////////////////////////////////////////////////////////
PhysicsCompilerToolPlugin::~PhysicsCompilerToolPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::Destroy()
{
   // Plugin instance is about to shutdown.
   // Backup settings to be used for the next session.
   SaveSettings();

   // TODO: Ensure compile thread is dead if it is running.
}

////////////////////////////////////////////////////////////////////////////////
bool PhysicsCompilerToolPlugin::IsCompiling() const
{
   return mIsCompiling;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::SetPanelEnabled(bool enabled)
{
   mUI.mButtonTargetActor->setEnabled(enabled);
   mUI.mFileOptions->setEnabled(enabled);

   mUI.mButtonTargetActor->setEnabled(enabled);
   if ( ! IsCompiling())
   {
      mUI.mButtonCompile->setEnabled(enabled);
   }
}

////////////////////////////////////////////////////////////////////////////////
bool PhysicsCompilerToolPlugin::IsPanelEnabled() const
{
   return mUI.mButtonTargetActor->isEnabled();
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::CreateConnections()
{
   // Plugin Interface Connections
   connect(&dtEditQt::EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorRefPtrVector&)),
      this, SLOT(onActorsSelected(ActorRefPtrVector&)));

   // Target Actor Button Connection
   connect(mUI.mButtonTargetActor, SIGNAL(clicked()),
      this, SLOT(OnTargetActorClicked()));

   // Compiler Connections
   connect(mUI.mButtonCompile, SIGNAL(clicked()),
      this, SLOT(OnCompileClicked()));
   connect(mUI.mButtonInputMeshFile, SIGNAL(clicked()),
      this, SLOT(OnInputMeshFileClicked()));
   connect(mUI.mInputMeshEnabled, SIGNAL(stateChanged(int)),
      this, SLOT(OnInputMeshEnabledChanged(int)));
   connect(mUI.mAllowDefaultMaterial, SIGNAL(stateChanged(int)),
      this, SLOT(OnAllowDefaultMaterialChanged(int)));
   connect(mUI.mMaxVertsPerMesh, SIGNAL(valueChanged(int)),
      this, SLOT(OnMaxVertsPerMeshChanged(int)));
   connect(mUI.mMaxEdgeLength, SIGNAL(valueChanged(double)),
      this, SLOT(OnMaxEdgeLengthChanged(double)));

   connect(mUI.mClearExistingObjects, SIGNAL(stateChanged(int)),
      this, SLOT(OnClearExistingObjectsChanged(int)));
   connect(mUI.mPrimitiveType, SIGNAL(currentIndexChanged(int)),
      this, SLOT(OnPrimitiveTypeChanged(int)));
   connect(mUI.mMechanicsType, SIGNAL(currentIndexChanged(int)),
      this, SLOT(OnMechanicsTypeChanged(int)));
   connect(mUI.mMass, SIGNAL(valueChanged(double)),
      this, SLOT(OnMassChanged(double)));
   connect(mUI.mCollisionMargin, SIGNAL(valueChanged(double)),
      this, SLOT(OnCollisionMarginChanged(double)));

   // File Option Connections
   connect(mUI.mButtonTargetDirectory, SIGNAL(clicked()),
      this, SLOT(OnTargetDirectoryClicked()));
   connect(mUI.mFilePrefix, SIGNAL(textChanged(const QString&)),
      this, SLOT(OnFilePrefixChanged(const QString&)));
   connect(mUI.mFileSuffix, SIGNAL(textChanged(const QString&)),
      this, SLOT(OnFileSuffixChanged(const QString&)));

   // Internal Thread-related Connections
   connect(this, SIGNAL(SignalCompileResult(PhysicsCompileResultPtr)),
      this, SLOT(OnCompileResult(PhysicsCompileResultPtr)),
      Qt::QueuedConnection);

   connect(this, SIGNAL(SignalCompileCompleted(dtPhysics::PhysicsObjectArray)),
      this, SLOT(OnCompileCompleted(dtPhysics::PhysicsObjectArray)),
      Qt::QueuedConnection);
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::SetupComboboxes()
{
   typedef std::vector<dtUtil::Enumeration*> EnumArray;

   dtUtil::Enumeration* curEnum = NULL;

   const EnumArray& primArray = PrimitiveType::Enumerate();
   EnumArray::const_iterator curIter = primArray.begin();
   EnumArray::const_iterator endIter = primArray.end();
   for (; curIter != endIter; ++curIter)
   {
      curEnum = *curIter;

      mUI.mPrimitiveType->addItem(tr(curEnum->GetName().c_str()));
   }

   const EnumArray& mechArray = MechanicsType::Enumerate();
   curIter = mechArray.begin();
   endIter = mechArray.end();
   for (; curIter != endIter; ++curIter)
   {
      curEnum = *curIter;

      mUI.mMechanicsType->addItem(tr(curEnum->GetName().c_str()));
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::EnsurePhysicsWorld()
{
   if ( ! PhysicsWorld::IsInitialized())
   {
      try
      {
         mPhysicsWorld = new PhysicsWorld("Bullet");
         mPhysicsWorld->Init();
      }
      catch(dtUtil::Exception& ex)
      {
         LOG_ERROR("Exception hit while initializing PhysicsWorld: " + ex.ToString());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
dtPhysics::PhysicsCompiler& PhysicsCompilerToolPlugin::GetCompiler() const
{
   return *mCompiler;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::UpdateUI()
{
   // Geometry Options
   PhysicsCompileOptions& options = mCompileOptions;
   mUI.mAllowDefaultMaterial->setChecked(options.mAllowDefaultMaterial);
   mUI.mMaxVertsPerMesh->setValue(options.mMaxVertsPerMesh);
   mUI.mMaxEdgeLength->setValue(options.mMaxEdgeLength);

   // Object Options
   PhysicsObjectOptions& objOptions = mObjectOptions;
   int index = mUI.mPrimitiveType->findText(tr(objOptions.mPrimitiveType->GetName().c_str()));
   mUI.mPrimitiveType->setCurrentIndex(index);

   index = mUI.mMechanicsType->findText(tr(objOptions.mMechanicsType->GetName().c_str()));
   mUI.mMechanicsType->setCurrentIndex(index);

   mUI.mClearExistingObjects->setChecked(objOptions.mClearExistingObjects);
   mUI.mMass->setValue(mObjectOptions.mMass);
   mUI.mCollisionMargin->setValue(mObjectOptions.mCollisionMargin);

   // File Options
   mUI.mTargetDirectory->setText(tr(mFileOptions.mTargetDir.c_str()));
   mUI.mFilePrefix->setText(tr(mFileOptions.mFilePrefix.c_str()));
   mUI.mFileSuffix->setText(tr(mFileOptions.mFileSuffix.c_str()));
}

////////////////////////////////////////////////////////////////////////////////
int PhysicsCompilerToolPlugin::AddObjects(dtPhysics::PhysicsObjectArray& objArray,
   dtPhysics::PhysicsActComp& actorComp)
{
   int results = 0;

   if (mObjectOptions.mClearExistingObjects)
   {
      actorComp.ClearAllPhysicsObjects();
   }

   PhysicsObject* curObj = NULL;
   PhysicsObjectArray::iterator curObjIter = objArray.begin();
   PhysicsObjectArray::iterator endObjIter = objArray.end();
   for (; curObjIter != endObjIter; ++curObjIter)
   {
      curObj = curObjIter->get();

      actorComp.AddPhysicsObject(*curObj);

      ++results;
   }

   /*
      //Get properties for the physics objects.
      PhysicsObjectArray::iterator i, iend;
      i = mPhysicsObjects.begin();
      iend = mPhysicsObjects.end();
      for(; i != iend; ++i)
      {
         PhysicsObject& physObj = **i;
         physObj.BuildPropertyMap();
         physObj.ForEachProperty(dtUtil::MakeFunctor(&PropertyContainer::AddProperty, this));
      }*/

   return results;
}

////////////////////////////////////////////////////////////////////////////////
int PhysicsCompilerToolPlugin::AddObjectsToActor(
   dtPhysics::PhysicsObjectArray& objArray,
   dtCore::BaseActorObject& actor)
{
   int results = 0;

   dtGame::ActorComponentContainer* gameActor
      = dynamic_cast<dtGame::ActorComponentContainer*>(&actor);

   // Add the objects to the actor component, if one is specified.
   if (gameActor != NULL && ! objArray.empty())
   {
      PhysicsActComp* physActComp = NULL;
      gameActor->GetComponent(physActComp);

      // Ensure a physics component is available for adding physics objects.
      if (physActComp == NULL)
      {
         physActComp = new PhysicsActComp;
         gameActor->AddComponent(*physActComp);
         //physActComp->OnEnteredWorld();
      }

      results = AddObjects(objArray, *physActComp);
   }

   return results;
}

////////////////////////////////////////////////////////////////////////////////
bool PhysicsCompilerToolPlugin::WriteGeometryFile(const PhysicsFileOptions options,
   PhysicsCompileResultPtr compileResult)
{
   // NOTE: This method may be called from the compile thread.

   bool result = false;

   // Construct the file name from file options.
   std::stringstream resName;
   resName << options.mFilePrefix << compileResult->mMaterialName;
   // --- Add a suffix if the result is one of many.
   if (compileResult->mPartTotalInProgress > 1)
   {
      resName << options.mFileSuffix << compileResult->mPartIndex;
   }

   // Construct the fulle file path from file options.
   std::stringstream filepath;
   filepath << options.mTargetDir << "/" << resName.str();


   // Add the extension.
   filepath << ".dtphys";
   
   // Attempt writing the file.
   try
   {
      std::string strFilepath(filepath.str());

      result = dtPhysics::PhysicsReaderWriter::SaveTriangleDataFile(
         *compileResult->mVertData, strFilepath);

      if (result)
      {
         // The project instance is about to be accessed.
         // Ensure that other threads do not change it while
         // attempting to add a new resource descriptor.
         OpenThreads::ScopedLock<OpenThreads::Mutex> sl(mUiUpdateMutex);

         // Get the resource descriptor.
         dtCore::Project& proj = dtCore::Project::GetInstance();
         proj.Refresh();

         // TODO: Remove the data type part of the path once the
         // delta3d project system no longer forces certain directories
         // for data types.
         const char delimiter = + dtCore::ResourceDescriptor::DESCRIPTOR_SEPARATOR;
         dtCore::ResourceDescriptor res(options.mTargetDataType->GetName() + delimiter
            + options.mTargetDirAsCategory + delimiter + resName.str() + ".dtphys");
         if ( ! proj.GetResourcePath(res).empty())
         {
            compileResult->mVertData->mOutputFile = res;
         }
         else
         {
            LOG_ERROR("Could not create valid resource descriptor for: " + strFilepath);
         }
      }
   }
   catch(dtUtil::Exception& ex)
   {
      LOG_ERROR("Exception writing file \"" + filepath.str() + "\": " + ex.ToString());
   }
   catch(...)
   {
      LOG_ERROR("Exception writing file \"" + filepath.str() + "\".");
   }
   
   if ( ! result)
   {
      OpenThreads::ScopedLock<OpenThreads::Mutex> sl(mUiUpdateMutex);

      mFailedFiles.push_back(filepath.str());
   }

   return result;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::onActorsSelected(ActorRefPtrVector& actors)
{
   // Avoid changing the target actor if a compilation is in progress.
   if (IsCompiling())
   {
      return;
   }

   // Clear previous selection references.
   mActors.clear();

   // For now work with a single actor.
   SetPanelEnabled(actors.size() == 1);

   // Avoid any unneccessary processing.
   if (actors.empty())
   {
      QString qstr;
      mUI.mTargetActorName->setText(qstr);
      mUI.mTargetActorName->setToolTip(qstr);

      return;
   }

   // Copy the new selection references just to be safe.
   dtCore::ActorRefPtrVector tmpActors;
   tmpActors = actors;
   mActors.reserve(tmpActors.size());

   dtCore::ActorPtr curActor;
   dtCore::ActorRefPtrVector::iterator curIter = tmpActors.begin();
   dtCore::ActorRefPtrVector::iterator endIter = tmpActors.end();
   for (; curIter != endIter; ++curIter)
   {
      curActor = curIter->get();
      mActors.push_back(curActor.get());

      QString qstr(curActor->GetName().c_str());
      mUI.mTargetActorName->setText(qstr);
      mUI.mTargetActorName->setToolTip(qstr);
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnTargetActorClicked()
{
   if ( ! mActors.empty())
   {
      dtCore::ActorPtr actor = mActors.front().get();
      dtEditQt::EditorEvents::GetInstance().emitGotoActor(actor);
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCompileClicked()
{
   mFailedFiles.clear();
   mVertData.clear();

   if ( ! mActors.empty())
   {
      osg::Node* node = mInputMesh.get();
      bool hasInputFile = mUI.mInputMeshEnabled->checkState() == Qt::Checked
            && mUI.mInputMeshFile->text().length() > 0;

      // TODO: Handle multiple nodes instead of only one.
      {
         // Try to load the input mesh if it has been specified in settings but not yet loaded.
         if (hasInputFile)
         {
            std::string filepath = mUI.mInputMeshFile->text().toStdString();
            try
            {
               mInputMesh = dtUtil::FileUtils::GetInstance().ReadNode(filepath);
               node = mInputMesh.get();
            }
            catch (...)
            {
               LOG_ERROR("Could not load input mesh: " + filepath);
            }

            // WORKAROUND
            // Project may not have been set when plugin was instantiated, thus
            // the target directory category may not have been set since the
            // project path was not accessible. Assuming the target directory is
            // within the project, this block should ensure that the target directory
            // can be converted to a resource category.
            if ( ! mFileOptions.mTargetDir.empty() && mFileOptions.mTargetDirAsCategory.empty())
            {
               mFileOptions.SetTargetDirectory(mFileOptions.mTargetDir);
            }
         }

         // Fallback...
         if (node == NULL)
         {
            dtCore::DeltaDrawable* drawable = mActors.front()->GetDrawable();

            if (drawable != NULL)
            {
               node = drawable->GetOSGNode();
            }
         }
      }
      
      // Compile the specified geometry.
      if (node != NULL)
      {
         mUI.mMaterialList->setRowCount(0);
         mUI.mProgress->show();
         mUI.mButtonCompile->setEnabled(false);

         mIsCompiling = true;

         // Modify compile options non-destrcutively for
         // other settings that do not translate directly
         // to explicit options/variables.
         dtPhysics::PhysicsCompileOptions compileOptions = mCompileOptions;
         // Determine if trangle splitting should be disabled.
         if ( ! mUI.mTriangleSubdivisionEnabled->isChecked())
         {
            compileOptions.mMaxEdgeLength = 0.0f;
         }

         // Compile to a separate thread so UI can update independently.
         dtCore::RefPtr<PhysicsCompileThread> compileThread = new PhysicsCompileThread(*this, *node);
         compileThread->mCompileOptions = compileOptions;
         compileThread->mObjectOptions = mObjectOptions;
         compileThread->mVertData = &mVertData;
         dtUtil::ThreadPool::AddTask(*compileThread, dtUtil::ThreadPool::IO);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnInputMeshEnabledChanged(int checkState)
{
   mUI.mButtonInputMeshFile->setEnabled(Qt::Checked == checkState);
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnInputMeshFileClicked()
{
   QFileDialog dialog;
   dialog.setWindowTitle("Physics Input Mesh File");
   dialog.setFileMode(QFileDialog::ExistingFile);
   dialog.setDirectory(tr(mFileOptions.mTargetDir.c_str()));

   // TODO: Add file filter for supported file types.

   if (dialog.exec())
   {
      QStringList files = dialog.selectedFiles();
      std::string filepath;

      if ( ! files.empty())
      {
         filepath = files.at(0).toStdString();

         std::string contextDir = dtCore::Project::GetInstance().GetContext();
         dtUtil::FindAndReplace(contextDir, "\\", "/");

         // Verify if the selected file is within the project context.
         if (filepath.find(contextDir) == std::string::npos)
         {
            std::string message = "Input mesh file \"" + filepath
               + "\" is not within the current project context \"" +
               contextDir + "\".";
            std::string infoText("Files output to the target directory might not load with the current map on the next reload.");

            ShowWarningMessage(message, infoText);
         }

         QString qstr(filepath.c_str());
         mUI.mInputMeshFile->setText(qstr);
         mUI.mInputMeshFile->setToolTip(qstr);

         // Load the mesh file.
         mInputMesh = dtUtil::FileUtils::GetInstance().ReadNode(filepath);

         if ( ! mInputMesh.valid())
         {
            std::string message = "Geometry could not be loaded for input mesh file \"" + filepath
               + "\".";
            ShowWarningMessage(message, "");
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnAllowDefaultMaterialChanged(int checkState)
{
   mCompileOptions.mAllowDefaultMaterial = Qt::Checked == checkState;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnMaxVertsPerMeshChanged(int count)
{
   mCompileOptions.mMaxVertsPerMesh = count;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnMaxEdgeLengthChanged(double length)
{
   mCompileOptions.mMaxEdgeLength = (float)length;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnClearExistingObjectsChanged(int checkState)
{
   mObjectOptions.mClearExistingObjects = Qt::Checked == checkState;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnPrimitiveTypeChanged(int index)
{
   QString qstr = mUI.mPrimitiveType->currentText();
   mObjectOptions.mPrimitiveType = PrimitiveType::GetValueForName(qstr.toStdString());
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnMechanicsTypeChanged(int index)
{
   QString qstr = mUI.mMechanicsType->currentText();
   mObjectOptions.mMechanicsType = MechanicsType::GetValueForName(qstr.toStdString());
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnMassChanged(double mass)
{
   mObjectOptions.mMass = mass;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCollisionMarginChanged(double margin)
{
   mObjectOptions.mCollisionMargin = margin;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnTargetDirectoryClicked()
{
   QFileDialog dialog;
   dialog.setWindowTitle("Physics Output Target Directory");
   dialog.setFileMode(QFileDialog::DirectoryOnly);
   dialog.setDirectory(tr(mFileOptions.mTargetDir.c_str()));

   if (dialog.exec())
   {
      QStringList files = dialog.selectedFiles();
      std::string dir;

      if ( ! files.empty())
      {
         dir = files.at(0).toStdString();

         std::string contextDir = dtCore::Project::GetInstance().GetContext();
         dtUtil::FindAndReplace(contextDir, "\\", "/");

         // Verify if the selected directory is within the project context.
         if (dir.find(contextDir) == std::string::npos)
         {
            std::string message = "Target directory \"" + dir
               + "\" is not within the current project context \"" +
               contextDir + "\".";
            std::string infoText("Files output to the target directory might not load with the current map on the next reload.");

            ShowWarningMessage(message, infoText);
         }

         QString qstr(dir.c_str());
         mUI.mTargetDirectory->setText(qstr);
         mUI.mTargetDirectory->setToolTip(qstr);

         mFileOptions.SetTargetDirectory(dir);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnFilePrefixChanged(const QString& prefix)
{
   mFileOptions.mFilePrefix = prefix.toStdString();
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnFileSuffixChanged(const QString& suffix)
{
   mFileOptions.mFileSuffix = suffix.toStdString();
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCompileResult_CompileThread(PhysicsCompileResult& result)
{
   // NOTE: This method is called from the compile thread.

   PhysicsCompileResultPtr resultPtr = &result;

   if (mUI.mFileOptions->isChecked())
   {
      WriteGeometryFile(mFileOptions, resultPtr);
   }

   emit SignalCompileResult(resultPtr);
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCompileResult(PhysicsCompileResultPtr result)
{
   QString qstr(result->mMaterialName.c_str());
   if(mUI.mMaterialList->findItems(qstr, Qt::MatchExactly).empty())
   {
      QTableWidgetItem* item = new QTableWidgetItem(qstr);
      int row = mUI.mMaterialList->rowCount();
      mUI.mMaterialList->insertRow(row);
      mUI.mMaterialList->setItem(row, 0, item);
   }

   // Update the progress bar.
   if (result->mPartTotalInProgress > 0)
   {
      int maxValue = mUI.mProgress->maximum();
      float percentage = float(result->mPartIndex + 1) / float(result->mPartTotalInProgress);
      percentage *= maxValue;

      mUI.mProgress->setValue(int(percentage));
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCompileCompleted(dtPhysics::PhysicsObjectArray objArray)
{
   mIsCompiling = false;

   mUI.mProgress->hide();
   mUI.mButtonCompile->setEnabled(true);

   // Inform of any files that failed to write.
   if ( ! mFailedFiles.empty())
   {
      std::stringstream details;

      StrList::iterator curIter = mFailedFiles.begin();
      StrList::iterator endIter = mFailedFiles.end();
      for (; curIter != endIter; ++curIter)
      {
         details << curIter->c_str() << std::endl;
      }

      std::string message("Failed to write some geometry files.");
      std::string infoText;
      ShowWarningMessage(message, infoText, details.str());
   }

   if (objArray.empty()) // No objects were created.
   {
      std::string message("No physics objects could be created.");
      std::string infoText;

      ShowWarningMessage(message, infoText);
   }

   // Add the objects to selected actors.
   if ( ! mActors.empty())
   {
      // For now work with a single actor.
      dtCore::BaseActorObject* actor = mActors.front().get();
      if (actor != NULL)
      {
         AddObjectsToActor(objArray, *actor);
      }
   }

   // Save settings since compile succeeded.
   SaveSettings();
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCompileCompleted_CompileThread(dtPhysics::PhysicsObjectArray& objArray)
{
   emit SignalCompileCompleted(objArray);
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::ShowWarningMessage(const std::string& text,
   const std::string& infoText, const std::string& detailsText)
{
   QString qtitle((PLUGIN_NAME + " - Warning").c_str());

   QMessageBox mbox;
   mbox.setWindowTitle(qtitle);
   mbox.setIcon(QMessageBox::Warning);
   mbox.setText(tr(text.c_str()));
   mbox.setStandardButtons(QMessageBox::Ok);

   if ( ! infoText.empty())
   {
      mbox.setInformativeText(tr(infoText.c_str()));
   }

   if ( ! detailsText.empty())
   {
      mbox.setDetailedText(tr(detailsText.c_str()));
   }

   mbox.exec();
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::SaveSettings()
{
   QSettings settings(PLUGIN_SETTINGS_NAME, PLUGIN_TITLE);

   try
   {
      // File Settings
      settings.setValue(SETTING_FILE_OPTIONS_ENABLED, mUI.mFileOptions->isChecked());
      settings.setValue(SETTING_TARGET_DIRECTORY, mFileOptions.mTargetDir.c_str());
      settings.setValue(SETTING_FILE_PREFIX, mFileOptions.mFilePrefix.c_str());
      settings.setValue(SETTING_FILE_SUFFIX, mFileOptions.mFileSuffix.c_str());

      // Compile Settings
      settings.setValue(SETTING_INPUT_MESH_FILE, mUI.mInputMeshFile->text());
      settings.setValue(SETTING_INPUT_MESH_FILE_ENABLED, mUI.mInputMeshEnabled->isChecked());
      settings.setValue(SETTING_MAX_VERTS_PER_MESH, mCompileOptions.mMaxVertsPerMesh);
      settings.setValue(SETTING_MAX_EDGE_LENGTH, mCompileOptions.mMaxEdgeLength);
      settings.setValue(SETTING_TRIANGLE_SUBDIVISION_ENABLED, mUI.mTriangleSubdivisionEnabled->isChecked());

      // Object Settings
      settings.setValue(SETTING_MASS, mObjectOptions.mMass);
      settings.setValue(SETTING_COLLISION_MARGIN, mObjectOptions.mCollisionMargin);
      settings.setValue(SETTING_CLEAR_EXISTING_OBJECTS, mObjectOptions.mClearExistingObjects);

      settings.sync();
   }
   catch (const dtUtil::Exception &e)
   {
      QMessageBox::critical((QWidget *)this, tr("Error"), tr(e.What().c_str()), tr("Ok"));
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::LoadSettings()
{
   QSettings settings(PLUGIN_SETTINGS_NAME, PLUGIN_TITLE);

   try
   {
      // Only load settings if they were previously set.
      // Otherwise, the options structs will have sensible default values
      // and should not be nullified by non-existant setting values.
      if (settings.contains(SETTING_TARGET_DIRECTORY))
      {
         // File Settings
         mFileOptions.SetTargetDirectory(settings.value(SETTING_TARGET_DIRECTORY).toString().toStdString());
         mFileOptions.mFilePrefix = settings.value(SETTING_FILE_PREFIX).toString().toStdString();
         mFileOptions.mFileSuffix = settings.value(SETTING_FILE_SUFFIX).toString().toStdString();
         bool enableFileOptions = settings.value(SETTING_FILE_OPTIONS_ENABLED).toBool();
         mUI.mFileOptions->setChecked(enableFileOptions);
         
         // Compile Settings
         mUI.mInputMeshFile->setText(settings.value(SETTING_INPUT_MESH_FILE).toString());
         mUI.mInputMeshEnabled->setChecked(settings.value(SETTING_INPUT_MESH_FILE_ENABLED).toBool());
         mCompileOptions.mMaxVertsPerMesh = settings.value(SETTING_MAX_VERTS_PER_MESH).toUInt();
         mCompileOptions.mMaxEdgeLength = settings.value(SETTING_MAX_EDGE_LENGTH).toFloat();
         mUI.mTriangleSubdivisionEnabled->setChecked(settings.value(SETTING_TRIANGLE_SUBDIVISION_ENABLED).toBool());
         
         // Object Settings
         mObjectOptions.mMass = settings.value(SETTING_MASS).toDouble();
         mObjectOptions.mCollisionMargin = settings.value(SETTING_COLLISION_MARGIN).toDouble();
         mObjectOptions.mClearExistingObjects = settings.value(SETTING_CLEAR_EXISTING_OBJECTS).toBool();

         // Validate the target directory.
         if (mFileOptions.mTargetDir.empty()
            || ! dtUtil::FileUtils::GetInstance().DirExists(mFileOptions.mTargetDir))
         {
            std::string message("Target directory could not be found.");

            ShowWarningMessage(message, mFileOptions.mTargetDir);
         }
      }
   }
   catch (const dtUtil::Exception &e)
   {
      QMessageBox::critical((QWidget *)this, tr("Error"), tr(e.What().c_str()), tr("Ok"));
   }
}



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
namespace PhysicsCompilerTool
{
   class DT_PHYSICS_COMPILER_TOOL_PLUGIN_EXPORT PluginFactory : public dtEditQt::PluginFactory
   {
   public:
      PluginFactory() {}
      ~PluginFactory() {}

      /** get the name of the plugin */
      virtual std::string GetName() { return PhysicsCompilerToolPlugin::PLUGIN_NAME; }

      /** get a description of the plugin */
      virtual std::string GetDescription() { return "Creates physics geometry from selected actors."; }

      virtual void GetDependencies(std::list<std::string>& deps) 
      {
         //deps.push_back("");
      }

       /** construct the plugin and return a pointer to it */
      virtual dtEditQt::Plugin* Create(dtEditQt::MainWindow* mw) 
      {
         mPlugin = new PhysicsCompilerToolPlugin(mw);
         return mPlugin;
      }

      virtual void Destroy() 
      {
         delete mPlugin;
         mPlugin = NULL;
      }

   private:

      PhysicsCompilerToolPlugin* mPlugin;
   }; 
} //namespace PhysicsCompilerToolPlugin

extern "C" DT_PHYSICS_COMPILER_TOOL_PLUGIN_EXPORT dtEditQt::PluginFactory* CreatePluginFactory()
{
   return new PhysicsCompilerTool::PluginFactory;
}
