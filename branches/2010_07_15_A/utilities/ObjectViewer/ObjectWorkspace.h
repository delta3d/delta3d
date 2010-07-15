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
#include <dtCore/refptr.h>

///////////////////////////////////////////////////////////////////////////////

class ResourceDock;
class QAction;
class QToolBar;
class QHBoxLayout;


///////////////////////////////////////////////////////////////////////////////

class ObjectViewer;

class ObjectWorkspace : public QMainWindow
{
   Q_OBJECT
public:
   ObjectWorkspace();
   ~ObjectWorkspace();

   void SetViewer(ObjectViewer* viewer);

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
   void OnLoadMap(const std::string& mapName);
   void OnLoadGeometry(const std::string& fullName);

private:
   QObject* GetResourceObject();
   void CreateMenus();
   void CreateFileMenuActions();
   void CreateModeToolbarActions();
   void CreateEditingToolbarAction();
   void CreateDisplayToolbarActions();
   void CreateShaderToolbarActions();
   void CreateToolbars();
   void UpdateResourceLists();
   void LoadObjectFile(const QString& filename);

   QHBoxLayout* mCentralLayout;

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

   // Coordinate Space Toolbar
   QAction* mWorldSpaceAction;
   QAction* mLocalSpaceAction;

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

   QToolBar* mCoordinateToolbar;
   QToolBar* mDisplayToolbar;
   QToolBar* mShaderToolbar;
   QToolBar* mModeToolbar;

   ResourceDock* mResourceDock;

   std::string mContextPath;
   QString mShaderDefinitionName;

   QList<std::string> mAdditionalShaderFiles;

   ObjectViewer* mViewer;

private slots:

   // File menu callbacks
   void OnLoadShaderDefinition();
   void OnLoadGeometry();
   void OnChangeContext();

   void OnToggleGridClicked(bool toggledOn);

   std::string GetContextPathFromUser();
   void SaveCurrentContextPath();
   void SaveCurrentShaderFiles();
   void SetupConnectionsWithViewer();
};

#endif // DELTA_OBJECTWORKSPACE
