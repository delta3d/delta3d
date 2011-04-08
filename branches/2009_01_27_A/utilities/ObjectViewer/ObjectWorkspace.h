/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef DELTA_OBJECTWORKSPACE
#define DELTA_OBJECTWORKSPACE

#include <QtGui/QMainWindow>

///////////////////////////////////////////////////////////////////////////////

class ResourceDock;
class QAction;
class QToolBar;

namespace dtQt
{
   class OSGAdapterWidget;
}

///////////////////////////////////////////////////////////////////////////////

class ObjectWorkspace : public QMainWindow
{
   friend class Delta3DThread;
   Q_OBJECT
public:
   ObjectWorkspace();
   ~ObjectWorkspace();

   dtQt::OSGAdapterWidget* GetGLWidget() { return mGLWidget; }
   QObject* GetResourceObject();

   virtual void dragEnterEvent(QDragEnterEvent* event);
   virtual void dropEvent(QDropEvent* event);

signals:
   
   // File menu
   void FileToLoad(const QString&);
   void LoadShaderDefinition(const QString&);
   void ReloadShaderDefinition();

   // Toolbar
   void ToggleGrid(bool shouldShow);

   // Settings menu
   void SetGenerateTangentAttribute(bool shouldGenerate);

public slots:

   void OnInitialization();
   void OnToggleShadingToolbar();
   void OnToggleGenerateTangents();
   void OnRecompileClicked();
   void OnToggleVertexShaderSource(bool enabled);
   void OnToggleFragmentShaderSource(bool enabled);
   void OnRemoveShaderDef(const std::string& filename);

private:
   void CreateMenus();
   void CreateFileMenuActions();
   void CreateModeToolbarActions();
   void CreateDisplayToolbarActions();
   void CreateShaderToolbarActions();
   void CreateToolbars();
   void UpdateResourceLists();
   void LoadObjectFile(const QString& filename);

   // File menu
   QAction* mLoadShaderDefAction;
   QAction* mLoadGeometryAction;
   QAction* mChangeContextAction;
   QAction* mExitAct;

   // Settings menu
   QAction* mGenerateTangentsAction;

   // Mode Toolbar
   QAction* mObjectModeAction;
   QAction* mLightModeAction;

   // Display Toolbar
   QAction* mWireframeAction;
   QAction* mShadedAction;
   QAction* mShadedWireAction;
   QAction* mGridAction;
   QAction* mDiffuseLightAction;
   QAction* mPointLightAction;

   // Shader Toolbar
   QAction* mRecompileAction;
   QAction* mOpenVertexShaderAction;
   QAction* mOpenFragmentShaderAction;

   QToolBar* mDisplayToolbar;
   QToolBar* mShaderToolbar;
   QToolBar* mModeToolbar;

   ResourceDock* mResourceDock;

   std::string mContextPath;
   QString mShaderDefinitionName;

   QList<std::string> mAdditionalShaderFiles;

   dtQt::OSGAdapterWidget* mGLWidget;

private slots:

   // File menu callbacks
   void OnLoadShaderDefinition();
   void OnLoadMap(const std::string& mapName);
   void OnLoadGeometry();
   void OnLoadGeometry(const std::string& fullName);
   void OnChangeContext();

   void OnToggleGridClicked(bool toggledOn);

   std::string GetContextPathFromUser();
   void SaveCurrentContextPath();
   void SaveCurrentShaderFiles();
};

#endif // DELTA_OBJECTWORKSPACE