/*
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc.
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
*/

#include "ResourceDock.h"
#include "TextEdit.h"

#include <QtGui/QAction>
#include <QtGui/QTabWidget>
#include <QtGui/QTreeWidget>

#include <osg/LightSource>
#include <dtCore/globals.h>
#include <dtCore/shadermanager.h>

#include <algorithm>
#include <cctype>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
ResourceDock::ResourceDock()
  : QDockWidget()
  , mTabs(NULL)
{
   setWindowTitle(tr("Resources"));
   setMouseTracking(true);
  
   mGeometryTreeWidget = new QTreeWidget(this);
   mShaderTreeWidget   = new QTreeWidget(this);  
   mLightTreeWidget     = new QTreeWidget(this);  

   mGeometryTreeWidget->headerItem()->setText(0, "");
   mShaderTreeWidget->headerItem()->setText(0, "");
   mLightTreeWidget->headerItem()->setText(0, "");

   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
           this, SLOT(OnShaderItemChanged(QTreeWidgetItem*, int))); 

   connect(mGeometryTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
           this, SLOT(OnGeometryItemChanged(QTreeWidgetItem*, int))); 

   mTabs = new QTabWidget;
   mTabs->addTab(mGeometryTreeWidget, tr("Geometry"));
   mTabs->addTab(mShaderTreeWidget, tr("Shaders"));
   mTabs->addTab(mLightTreeWidget, tr("Lights"));

   setWidget(mTabs);     

   CreateLightItems();
}

///////////////////////////////////////////////////////////////////////////////
ResourceDock::~ResourceDock(){}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::FindGeometryItem(const std::string &fullName) const
{
   for (int itemIndex = 0; itemIndex < mGeometryTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem *childItem = mGeometryTreeWidget->topLevelItem(itemIndex);

      std::string currentPath = childItem->toolTip(0).toStdString();
      std::string findPath = fullName;

      std::transform(currentPath.begin(), currentPath.end(), currentPath.begin(), (int(*)(int))std::tolower);
      std::transform(findPath.begin(), findPath.end(), findPath.begin(), (int(*)(int))std::tolower);

      // Case insensitive comparison
      if (currentPath == findPath)
      {
         return childItem; 
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::FindShaderGroupItem(const std::string &name) const
{
   for (int itemIndex = 0; itemIndex < mShaderTreeWidget->topLevelItemCount(); ++itemIndex)
   {
      QTreeWidgetItem *childItem = mShaderTreeWidget->topLevelItem(itemIndex);

      if (name == childItem->text(0).toStdString())
      {
         return childItem; 
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetGeometry(const std::string &fullName, bool shouldDisplay) const
{  
   QTreeWidgetItem *geometryItem = FindGeometryItem(fullName);
   
   if (geometryItem)
   {
      SetGeometry(geometryItem, shouldDisplay);
   }   
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::SetGeometry(QTreeWidgetItem *geometryItem, bool shouldDisplay) const
{
   std::string fullName = geometryItem->toolTip(0).toStdString();
   geometryItem->setCheckState(0, (shouldDisplay) ? Qt::Checked : Qt::Unchecked);
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnNewShader(const std::string &shaderGroup, const std::string &shaderProgram)
{
   //// We don't want this signal emitted when we're adding a shader
   disconnect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
              this, SLOT(OnShaderItemChanged(QTreeWidgetItem*, int)));

   QTreeWidgetItem *shaderItem = FindShaderGroupItem(shaderGroup);
   QTreeWidgetItem *programItem = new QTreeWidgetItem();

   // If the group doesn't exist, create a new one
   if (shaderItem == NULL)
   {
      shaderItem = new QTreeWidgetItem;
      mShaderTreeWidget->addTopLevelItem(shaderItem); 
   }

   shaderItem->setText(0, shaderGroup.c_str());
   programItem->setText(0, shaderProgram.c_str());

   shaderItem->setFlags(Qt::ItemIsSelectable |
                        Qt::ItemIsUserCheckable |
                        Qt::ItemIsEnabled);

   // The shader itself should have a checkbox
   programItem->setCheckState(0, Qt::Unchecked);
   
   shaderItem->addChild(programItem);

   connect(mShaderTreeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), 
           this, SLOT(OnShaderItemChanged(QTreeWidgetItem*, int)));
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnShaderItemChanged(QTreeWidgetItem* item, int column)
{ 
   if (column == 0)
   {
      QString programName = item->text(0);
      QString groupName   = item->parent()->text(0);

      if (item->checkState(0) == Qt::Checked)
      {
         // Store so we know where the source files can be found
         mCurrentShaderGroup   = groupName;
         mCurrentShaderProgram = programName;

         emit ApplyShader(groupName.toStdString(), programName.toStdString());         
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         mCurrentShaderGroup.clear();
         mCurrentShaderProgram.clear();

         emit RemoveShader();
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnNewGeometry(const std::string& path, const std::string& filename)
{
   QTreeWidgetItem *geometryItem = new QTreeWidgetItem();
   geometryItem->setText(0, filename.c_str());
   geometryItem->setToolTip(0, (path + "/" + filename).c_str());

   geometryItem->setFlags(Qt::ItemIsSelectable |
                          Qt::ItemIsUserCheckable |
                          Qt::ItemIsEnabled);
   
   geometryItem->setCheckState(0, Qt::Unchecked);

   mGeometryTreeWidget->addTopLevelItem(geometryItem);    
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnGeometryItemChanged(QTreeWidgetItem* item, int column)
{
   if (column == 0)
   {
      // The full path is stored in the tooltip
      QString geomName = item->toolTip(0);

      if (item->checkState(0) == Qt::Checked)
      {
         QTreeWidgetItemIterator treeIter(mGeometryTreeWidget);

         // Uncheck the previously checked item
         while (*treeIter)
         {
            if ((*treeIter)->checkState(0) == Qt::Checked && (*treeIter) != item)
            {
               (*treeIter)->setCheckState(0, Qt::Unchecked);
            }

            ++treeIter;
         }

         emit LoadGeometry(geomName.toStdString());         
      }
      else if (item->checkState(0) == Qt::Unchecked)
      {
         emit UnloadGeometry();
      }
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnLightUpdate(const dtCore::Light* light)
{
   int lightNumber = light->GetNumber();

   dtCore::Transform lightTransform;
   light->GetTransform(lightTransform);

   const osg::LightSource* osgSource = light->GetLightSource();
   const osg::Light* osgLight = osgSource->getLight();

   const osg::Vec4& position = osgLight->getPosition();
   const osg::Vec4& ambient  = osgLight->getAmbient();
   const osg::Vec4& diffuse  = osgLight->getDiffuse();
   const osg::Vec4& specular = osgLight->getSpecular();

   //std::ostringstream oss;
   //oss << "light #" << lightNumber << ": (" 
   //    << position.x() << ", " << position.y() << ", " << position.z() 
   //    << ")";

   //std::cout << oss.str() << std::endl;   
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnOpenCurrentVertexShaderSources()
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();

   dtCore::ShaderProgram *program = 
      shaderManager.FindShaderPrototype(mCurrentShaderProgram.toStdString(), mCurrentShaderGroup.toStdString());
   
   if (program)
   {
      const std::vector<std::string>& vertShaderList = program->GetVertexShaders();
      OpenFilesInTextEditor(vertShaderList);     
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OnOpenCurrentFragmentShaderSources()
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();

   dtCore::ShaderProgram *program = 
      shaderManager.FindShaderPrototype(mCurrentShaderProgram.toStdString(), mCurrentShaderGroup.toStdString());

   if (program)
   {
      const std::vector<std::string>& fragShaderList = program->GetFragmentShaders();
      OpenFilesInTextEditor(fragShaderList);     
   }  
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::CreateLightItems()
{  
   QStringList headerLables;
   headerLables << "Property" << "Value";

   mLightTreeWidget->setHeaderLabels(headerLables);

   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; ++lightIndex)
   {
      std::ostringstream oss;
      oss << "Light" << lightIndex;

      QTreeWidgetItem* newLightItem = new QTreeWidgetItem;
      newLightItem->setText(0, oss.str().c_str());  
      newLightItem->setText(1, "Disabled");

      QTreeWidgetItem* type = new QTreeWidgetItem(newLightItem);
      type->setText(0, "Type");  
      type->setText(1, "Infinite");      

      mLightTreeWidget->addTopLevelItem(newLightItem);

      mLightItems[lightIndex].type     = type;
      mLightItems[lightIndex].position = CreatePositionItem(newLightItem);
      mLightItems[lightIndex].ambient  = CreateColorItem("Ambient", newLightItem);
      mLightItems[lightIndex].diffuse  = CreateColorItem("Diffuse", newLightItem);
      mLightItems[lightIndex].specular = CreateColorItem("Specular", newLightItem);
   }
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::CreatePositionItem(QTreeWidgetItem* parent)
{
   QTreeWidgetItem* position = new QTreeWidgetItem(parent);
   position->setText(0, "Position");  

   QTreeWidgetItem* xItem = new QTreeWidgetItem(position);
   QTreeWidgetItem* yItem = new QTreeWidgetItem(position);
   QTreeWidgetItem* zItem = new QTreeWidgetItem(position);

   xItem->setText(0, "x");
   yItem->setText(0, "y");
   zItem->setText(0, "z");       

   return position;
}

///////////////////////////////////////////////////////////////////////////////
QTreeWidgetItem* ResourceDock::CreateColorItem(const std::string& name, QTreeWidgetItem* parent)
{
   QTreeWidgetItem* ambientItem = new QTreeWidgetItem(parent);
   ambientItem->setText(0, name.c_str());

   QTreeWidgetItem* redAmbientItem   = new QTreeWidgetItem(ambientItem);
   QTreeWidgetItem* greenAmbientItem = new QTreeWidgetItem(ambientItem);
   QTreeWidgetItem* blueAmbientItem  = new QTreeWidgetItem(ambientItem);
   QTreeWidgetItem* alphaAmbientItem = new QTreeWidgetItem(ambientItem);

   redAmbientItem->setText(0, "r");
   greenAmbientItem->setText(0, "g");
   blueAmbientItem->setText(0, "b");
   alphaAmbientItem->setText(0, "a");

   return ambientItem;
}

///////////////////////////////////////////////////////////////////////////////
void ResourceDock::OpenFilesInTextEditor(const std::vector<std::string>& fileList)
{   
   for (size_t fileIndex = 0; fileIndex < fileList.size(); ++fileIndex)
   {
      std::string fileName = dtCore::FindFileInPathList(fileList[fileIndex]);      

      TextEdit* editor = new TextEdit;
      editor->resize(700, 800);
      editor->load(fileName.c_str());
      editor->show();
   }      
}


