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

#ifndef DELTA_PHYSICS_COMPILER_TOOL_PLUGIN_H
#define DELTA_PHYSICS_COMPILER_TOOL_PLUGIN_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "export.h"
#include "ui_physicscompilerpanel.h"
// DELTA
#include <dtCore/datatype.h>
#include <dtCore/project.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/plugininterface.h>
#include <dtPhysics/palphysicsworld.h>
#include <dtPhysics/physicscompiler.h>
// QT
#include <QtGui/QDockWidget>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class PhysicsCompilerPanel;
}

namespace dtPhysics
{
   class PhysicsActComp;
}



////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////
typedef dtCore::RefPtr<dtPhysics::PhysicsCompileResult> PhysicsCompileResultPtr;

Q_DECLARE_METATYPE(PhysicsCompileResultPtr);
Q_DECLARE_METATYPE(dtPhysics::PhysicsObjectArray);
static const int typePhysCompResult = qRegisterMetaType<PhysicsCompileResultPtr>();
static const int typePhysObjArray = qRegisterMetaType<dtPhysics::PhysicsObjectArray>();



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
struct PhysicsFileOptions
{
   PhysicsFileOptions();

   void SetTargetDirectory(const std::string& dir);

   std::string mFilePrefix;
   std::string mFileSuffix;
   std::string mTargetDir;
   std::string mTargetDirAsCategory;
   const dtCore::DataType* mTargetDataType; // STATIC_MESH typically
   dtCore::Project::ContextSlot mTargetContextSlot;
};



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class DT_PHYSICS_COMPILER_TOOL_PLUGIN_EXPORT PhysicsCompilerToolPlugin 
   : public QDockWidget
   , public dtEditQt::Plugin
{
   Q_OBJECT

public:
   typedef QDockWidget BaseClass;

   const static std::string PLUGIN_NAME;
   
   PhysicsCompilerToolPlugin(dtEditQt::MainWindow* mw);

   ~PhysicsCompilerToolPlugin();

   virtual void Create();

   virtual void Destroy();

   bool IsCompiling() const;

   void SetPanelEnabled(bool enabled);
   bool IsPanelEnabled() const;

   dtPhysics::PhysicsCompiler& GetCompiler() const;

   void UpdateUI();

   int AddObjects(dtPhysics::PhysicsObjectArray& objects,
      dtPhysics::PhysicsActComp& actorComp);

   int AddObjectsToActor(dtPhysics::PhysicsObjectArray& objects,
      dtCore::BaseActorObject& actor);

   bool WriteGeometryFile(const PhysicsFileOptions options,
      PhysicsCompileResultPtr result);

signals:
   void SignalCompileResult(PhysicsCompileResultPtr result);
   void SignalCompileCompleted(dtPhysics::PhysicsObjectArray objects);

public slots:

   /**
   * Handles when actors are selected.
   *
   * @param[in]  actors  The list of actors being selected.
   */
   void onActorsSelected(ActorRefPtrVector& actors);

   void OnTargetActorClicked();

   void OnCompileClicked();
   void OnInputMeshEnabledChanged(int checkState);
   void OnInputMeshFileClicked();
   void OnAllowDefaultMaterialChanged(int checkState);
   void OnMaxVertsPerMeshChanged(int count);
   void OnMaxEdgeLengthChanged(double length);

   void OnClearExistingObjectsChanged(int checkState);
   void OnPrimitiveTypeChanged(int index);
   void OnMechanicsTypeChanged(int index);
   void OnMassChanged(double mass);
   void OnCollisionMarginChanged(double margin);

   void OnTargetDirectoryClicked();
   void OnFilePrefixChanged(const QString& prefix);
   void OnFileSuffixChanged(const QString& prefix);

   // NOTE: Result passed by copy for thread safety.
   void OnCompileResult(PhysicsCompileResultPtr result);
   void OnCompileResult_CompileThread(dtPhysics::PhysicsCompileResult& result);

   // NOTE: Array passed by copy for thread safety.
   void OnCompileCompleted(dtPhysics::PhysicsObjectArray objArray);
   void OnCompileCompleted_CompileThread(dtPhysics::PhysicsObjectArray& objArray);

protected:
   void CreateConnections();

   void SetupComboboxes();

   void EnsurePhysicsWorld();

   void ShowWarningMessage(const std::string& text,
      const std::string& infoText, const std::string& detailsText = "");
   
   void SaveSettings();
   void LoadSettings();

private:
   dtEditQt::MainWindow* mMainWindow;

   Ui::PhysicsCompilerPanel mUI;

   bool mIsCompiling;
   std::string mInputMeshFile;
   dtCore::RefPtr<osg::Node> mInputMesh;
   dtPhysics::PhysicsCompileOptions mCompileOptions;
   dtPhysics::PhysicsObjectOptions mObjectOptions;
   PhysicsFileOptions mFileOptions;
   dtCore::RefPtr<dtPhysics::PhysicsCompiler> mCompiler;
   dtCore::RefPtr<dtPhysics::PhysicsWorld> mPhysicsWorld;

   typedef std::list<std::string> StrList;
   StrList mFailedFiles;

   dtPhysics::VertexDataTable mVertData;

   ActorWeakPtrVector mActors;

   // Mutex for updating this object's variables from external threads.
   OpenThreads::Mutex mUiUpdateMutex;
};

#endif // FENCE_TOOL_PLUGIN
