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

#ifndef DELTA_RESOURCE_DOCK
#define DELTA_RESOURCE_DOCK

#include <QtGui/QWidget>
#include <QtGui/QTreeWidget>
#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>
#include <QtCore/QObject>

#include <dtCore/infinitelight.h>

///////////////////////////////////////////////////////////////////////////////

class QTabWidget;
class QTreeWidgetItem;

struct LightInfo;

namespace dtCore
{
   class Light;
}

///////////////////////////////////////////////////////////////////////////////

/**
* @class GeometryTree
* @brief This class contains the context menu support for the geometry tree.
*/
class GeometryTree : public QTreeWidget
{
public:
   GeometryTree(QWidget* parent = NULL);
   ~GeometryTree();

private:

   void CreateContextActions();
   void CreateContextMenus();

   void contextMenuEvent(QContextMenuEvent* contextEvent);

   // Context Menu
   QAction* mSaveAs;

   QMenu*   mObjectContext;
};

/**
* @class ShaderTree
* @brief This class contains the context menu support for the shader tree.
*/
class ShaderTree : public QTreeWidget
{
public:
   ShaderTree(QWidget* parent = NULL);
   ~ShaderTree();

   void SetShaderSourceEnabled(bool vertexEnabled, bool fragmentEnabled);

private:

   void CreateContextActions();
   void CreateContextMenus();

   void contextMenuEvent(QContextMenuEvent* contextEvent);

   // Context Menu
   QAction* mEditShaderDef;
   QAction* mRemoveShaderDef;

   QAction* mOpenVertexSource;
   QAction* mOpenFragmentSource;

   QMenu*   mDefinitionContext;
   QMenu*   mProgramContext;
};

/**
* @class ResourceDock
* @brief This class holds all project resource tabs.
*/
class ResourceDock : public QDockWidget
{
   Q_OBJECT
public:
  
   ResourceDock();  
   virtual ~ResourceDock();

   QTreeWidgetItem* FindMapItem(std::string fullName) const;
   QTreeWidgetItem* FindGeometryItem(const std::string& fullName) const;
   QTreeWidgetItem* FindShaderFileItem(const std::string& filename) const;
   QTreeWidgetItem* FindShaderGroupItem(const std::string& groupName, const QTreeWidgetItem* fileItem) const;
   void ReselectCurrentShaderItem();

   bool FindShaderFileEntryName(const std::string& entryName) const;

   void SetGeometry(const std::string& fullName, bool shouldDisplay) const;
   void SetGeometry(QTreeWidgetItem* geometryItem, bool shouldDisplay) const;

signals:

   void LoadMap(const std::string& mapName);
   void LoadGeometry(const std::string& filename);
   void UnloadGeometry();

   void ApplyShader(const std::string& group, const std::string& name);
   void RemoveShader();
   void ToggleVertexShaderSources(bool enabled);
   void ToggleFragmentShaderSources(bool enabled);
   void RemoveShaderDef(const std::string& filename);

   void AddLight();
   void RemoveLight(int id);
   void FixLights();
   void SetCurrentLight(int id);
   void SetLightEnabled(int id, bool enabled);
   void SetLightType(int id, int type);
   void SetLightPosition(int id, const osg::Vec3& position);
   void SetLightRotation(int id, const osg::Vec3& rotation);
   void SetAmbient(int id, const osg::Vec4& ambient);
   void SetDiffuse(int id, const osg::Vec4& diffuse);
   void SetSpecular(int id, const osg::Vec4& specular);
   void SetLightCutoff(int id, float cutoff);
   void SetLightExponent(int id, float exponent);
   void SetLightConstant(int id, float constant);
   void SetLightLinear(int id, float linear);
   void SetLightQuadratic(int id, float quadratic);

public slots:
   
   void OnNewMap(const std::string& mapName);
   void OnNewGeometry(const std::string& path, const std::string& filename);
   void OnGeometryItemChanged(QTreeWidgetItem* item, int column);
   void OnSaveAs();

   void OnNewShader(const std::string& filename, const std::string& shaderGroup, const std::string& shaderProgram);
   void OnShaderSelectionChanged();
   void OnShaderItemChanged(QTreeWidgetItem* item, int column);
   void OnReloadShaderFiles();
   void OnEditShaderDef();
   void OnRemoveShaderDef();

   void OnOpenCurrentVertexShaderSources();
   void OnOpenCurrentFragmentShaderSources();

   void OnLightUpdate(const LightInfo& lightInfo);
   void OnLightItemClicked(QTreeWidgetItem* item, int column);
   void OnLightItemChanged(QTreeWidgetItem* item, int column);

private:

   struct LightItems
   {
      QTreeWidgetItem* light;
      QTreeWidgetItem* type;
      QTreeWidgetItem* position;
      QTreeWidgetItem* rotation;
      QTreeWidgetItem* ambient;
      QTreeWidgetItem* diffuse;
      QTreeWidgetItem* specular;
      QTreeWidgetItem* custom;
   };

   QTabWidget* mTabs;

   GeometryTree*  mGeometryTreeWidget;  
   ShaderTree*    mShaderTreeWidget;
   QTreeWidget*   mLightTreeWidget; 

   LightItems mLightItems[dtCore::MAX_LIGHTS];  

   QString mCurrentShaderFile;
   QString mCurrentShaderGroup;
   QString mCurrentShaderProgram;

   bool mLoadingMap;

   void InitGeometryTree();
   void ExportTexturesFromNode(const QString& path, osg::Node* node);
   void ExportTexturesFromStateSet(const QString& path, osg::StateSet* stateSet);

   void CreateLightItems();
   QTreeWidgetItem* CreatePositionItem(QTreeWidgetItem* parent);
   QTreeWidgetItem* CreateRotationItem(QTreeWidgetItem* parent);
   QTreeWidgetItem* CreateColorItem(const std::string& name, QTreeWidgetItem* parent);

   void SetPositionItem(QTreeWidgetItem* item, const osg::Vec3& position);
   void SetColorItem(QTreeWidgetItem* item, const osg::Vec4& color);

   void OpenFilesInTextEditor(const std::vector<std::string>& fileList);

   int GetLightIDFromItem(QTreeWidgetItem* item);

   void DeleteTreeItem(QTreeWidgetItem* item);
   QTreeWidgetItem* CreateTreeItem(const QString& name, const QString& value, Qt::ItemFlags flags, QTreeWidgetItem* parent);
};


#endif // DELTA_RESOURCE_DOCK
