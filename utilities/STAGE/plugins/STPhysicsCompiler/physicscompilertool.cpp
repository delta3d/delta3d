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
#include <dtCore/librarymanager.h>
#include <dtCore/project.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/pluginmanager.h>
#include <dtGame/actorcomponentcontainer.h>
#include <dtPhysics/physicsactcomp.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/threadpool.h>
// QT
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>

using namespace dtPhysics;



////////////////////////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////////////////////////
const std::string PhysicsCompilerToolPlugin::PLUGIN_NAME = "Physics Compiler Tool";



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
         VertexDataArray* curArray = NULL;
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

   SetupComboboxes();

   // Set default values on UI before setting up connections
   // so that needless signal processing does not occur or interfere.
   UpdateUI();

   mUI.mProgress->setVisible(false);
   mUI.mFileOptions->setChecked(mUI.mTargetDirectory->text().length() > 0);

   CreateConnections();

   EnsurePhysicsWorld();
}

////////////////////////////////////////////////////////////////////////////////
PhysicsCompilerToolPlugin::~PhysicsCompilerToolPlugin()
{
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::Destroy()
{
   // TODO: Ensure compile thread is dead if it is running.
}

////////////////////////////////////////////////////////////////////////////////
bool PhysicsCompilerToolPlugin::IsCompiling() const
{
   return mIsCompiling;
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::CreateConnections()
{
   // Plugin Interface Connections
   connect(&dtEditQt::EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector&)),
      this, SLOT(onActorsSelected(ActorProxyRefPtrVector&)));

   // Target Button Connection
   connect(mUI.mButtonTargetDirectory, SIGNAL(clicked()),
      this, SLOT(OnTargetDirectoryClicked()));

   // Compiler Connections
   connect(mUI.mButtonCompile, SIGNAL(clicked()),
      this, SLOT(OnCompileClicked()));
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
   if (PhysicsWorld::IsInitialized())
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

   UpdateMaterialList();
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::UpdateMaterialList()
{
   // TODO:
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

   PhysicsObjectArray::iterator curObjIter = objArray.begin();
   PhysicsObjectArray::iterator endObjIter = objArray.end();
   for (; curObjIter != endObjIter; ++curObjIter)
   {
      actorComp.AddPhysicsObject(*curObjIter->get());

      ++results;
   }

   return results;
}

////////////////////////////////////////////////////////////////////////////////
bool PhysicsCompilerToolPlugin::WriteGeometryFile(const PhysicsFileOptions options,
   PhysicsCompileResultPtr compileResult)
{
   // NOTE: This method may be called from the compile thread.

   bool result = false;

   // Construct the file path from file options.
   std::stringstream filepath;
   filepath << options.mTargetDir << "/"
      << options.mFilePrefix << compileResult->mMaterialName;

   // Add a suffix if the result is one of many.
   if (compileResult->mPartTotalInProgress > 1)
   {
      filepath << options.mFileSuffix << compileResult->mPartIndex;
   }

   // Add the extension.
   filepath << ".dtphys";
   
   // Attempt writing the file.
   try
   {
      result = dtPhysics::PhysicsReaderWriter::SaveTriangleDataFile(
         *compileResult->mVertData, filepath.str());
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
void PhysicsCompilerToolPlugin::onActorsSelected(ActorProxyRefPtrVector& actors)
{
   // Avoid changing the target actor if a compilation is in progress.
   if (IsCompiling())
   {
      return;
   }

   // Clear previous selection references.
   mActors.clear();

   // Avoid any unneccessary processing.
   if (actors.empty())
   {
      return;
   }

   // Copy the new selection references just to be safe.
   ActorPtrVector tmpActors;
   tmpActors = actors;
   mActors.reserve(tmpActors.size());

   ActorPtr curActor;
   ActorPtrVector::iterator curIter = tmpActors.begin();
   ActorPtrVector::iterator endIter = tmpActors.end();
   for (; curIter != endIter; ++curIter)
   {
      curActor = curIter->get();
      mActors.push_back(curActor.get());

      mUI.mTargetActorName->setText(tr(curActor->GetName().c_str()));
   }
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnTargetActor()
{
   // TODO:
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCompileClicked()
{
   mFailedFiles.clear();
   mVertData.clear();

   if ( ! mActors.empty())
   {
      // TODO: Handle multiple nodes instead of only one.
      dtCore::DeltaDrawable* drawable = mActors.front()->GetDrawable();

      if (drawable != NULL)
      {
         osg::Node* node = drawable->GetOSGNode();

         if (node != NULL)
         {
            mUI.mProgress->show();
            mUI.mButtonCompile->setEnabled(false);

            PhysicsCompileOptions options = mCompileOptions;

            mIsCompiling = true;

            // Compile to a separate thread so UI can update.
            dtCore::RefPtr<PhysicsCompileThread> compileThread = new PhysicsCompileThread(*this, *node);
            compileThread->mCompileOptions = mCompileOptions;
            compileThread->mObjectOptions = mObjectOptions;
            compileThread->mVertData = &mVertData;
            dtUtil::ThreadPool::AddTask(*compileThread, dtUtil::ThreadPool::IO);
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

         mFileOptions.mTargetDir = dir;
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

   // The vertex data table should be updated. Update the material list.
   UpdateMaterialList();

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
}

////////////////////////////////////////////////////////////////////////////////
void PhysicsCompilerToolPlugin::OnCompileCompleted_CompileThread(dtPhysics::PhysicsObjectArray& objArray)
{
   if ( ! mActors.empty())
   {
      // TODO: Handle multiple actors instead of only one.
      dtGame::ActorComponentContainer* actor
         = dynamic_cast<dtGame::ActorComponentContainer*>(mActors.front().get());

      // Add the objects to the actor component, if one is specified.
      if (actor != NULL && ! objArray.empty())
      {
         PhysicsActComp* physActComp = NULL;
         actor->GetComponent(physActComp);

         // Ensure a physics component is available for adding physics objects.
         if (physActComp == NULL)
         {
            physActComp = new PhysicsActComp;
            actor->AddComponent(*physActComp);
            //physActComp->OnEnteredWorld();
         }

         AddObjects(objArray, *physActComp);
      }
   }

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
