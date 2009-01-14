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
#include <QtGui/QDockWidget>
#include <QtGui/QMainWindow>
#include <QtCore/QObject>

#include <dtCore/infinitelight.h>

///////////////////////////////////////////////////////////////////////////////

class QTabWidget;
class QTreeWidget;
class QTreeWidgetItem;

struct LightInfo;

namespace dtCore
{
   class Light;
}

///////////////////////////////////////////////////////////////////////////////

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

   QTreeWidgetItem* FindGeometryItem(const std::string& fullName) const;
   QTreeWidgetItem* FindShaderFileItem(const std::string& filename) const;
   QTreeWidgetItem* FindShaderGroupItem(const std::string& groupName, const QTreeWidgetItem* fileItem) const;

   void SetGeometry(const std::string& fullName, bool shouldDisplay) const;
   void SetGeometry(QTreeWidgetItem* geometryItem, bool shouldDisplay) const;

signals:

   void ApplyShader(const std::string& group, const std::string& name);
   void RemoveShader();

   void LoadGeometry(const std::string& filename);
   void UnloadGeometry();

   void AddLight();
   void RemoveLight(int id);

   void SetAmbient(int id, const osg::Vec4& ambient);
   void SetDiffuse(int id, const osg::Vec4& diffuse);
   void SetSpecular(int id, const osg::Vec4& specular);

public slots:
   
   void OnNewShader(const std::string& filename, const std::string& shaderGroup, const std::string& shaderProgram);
   void OnShaderItemChanged(QTreeWidgetItem* item, int column);

   void OnNewGeometry(const std::string& path, const std::string& filename);
   void OnGeometryItemChanged(QTreeWidgetItem* item, int column);
   
   void OnLightUpdate(const LightInfo& lightInfo);

   void OnOpenCurrentVertexShaderSources();
   void OnOpenCurrentFragmentShaderSources();

private:

   struct LightItems
   {
      QTreeWidgetItem* type;
      QTreeWidgetItem* position;
      QTreeWidgetItem* ambient;
      QTreeWidgetItem* diffuse;
      QTreeWidgetItem* specular;
   };

   QTabWidget* mTabs;

   QTreeWidget* mShaderTreeWidget;
   QTreeWidget* mGeometryTreeWidget;  
   QTreeWidget* mLightTreeWidget; 

   LightItems mLightItems[dtCore::MAX_LIGHTS];  

   QString mCurrentShaderFile;
   QString mCurrentShaderGroup;
   QString mCurrentShaderProgram;

   void CreateLightItems();

   QTreeWidgetItem* CreatePositionItem(QTreeWidgetItem* parent);
   QTreeWidgetItem* CreateColorItem(const std::string& name, QTreeWidgetItem* parent);

   void SetPositionItem(QTreeWidgetItem* item, const osg::Vec3& position);
   void SetColorItem(QTreeWidgetItem* item, const osg::Vec4& color);

   void OpenFilesInTextEditor(const std::vector<std::string>& fileList);

   int GetLightIDFromItem(const QTreeWidgetItem* item);

private slots:

   void OnLightItemClicked(QTreeWidgetItem* item, int column);

};


#endif // DELTA_RESOURCE_DOCK
