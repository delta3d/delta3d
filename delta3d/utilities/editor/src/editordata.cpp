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
 * Curtiss Murphy
 */
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/groupuiplugin.h>
#include <dtDAL/map.h>
#include <dtUtil/log.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPluginLoader>

namespace dtEditQt
{
   
   //Singleton global variable for this class.
   dtCore::RefPtr<EditorData> EditorData::instance;
   ///////////////////////////////////////////////////////////////////////////////
   EditorData::EditorData(): mainWindow(NULL), mGroupUIRegistry(new GroupUIRegistry), gridSize(16), 
      loadLastProject(true), loadLastMap(true), rigidCamera(true), selectionColor(Qt::red)    
   {
      LOG_INFO("Initializing Editor Data.");

      LOG_ALWAYS("--- Registering Static Group UI Plugins ---");      
      const QObjectList& staticPluginList = QPluginLoader::staticInstances();
      for (QObjectList::const_iterator i = staticPluginList.constBegin(); i != staticPluginList.constEnd(); ++i)
      {
         QObject* o = *i;
         GroupUIPlugin* groupUI = qobject_cast<GroupUIPlugin*>(o);
         if (groupUI != NULL)
            mGroupUIRegistry->RegisterPlugin(*groupUI);
      }
      LOG_ALWAYS("--- Finished Registering Static Group UI Plugins ---");      
   }

   ///////////////////////////////////////////////////////////////////////////////
   GroupUIRegistry& EditorData::GetGroupUIRegistry()
   {
      return *mGroupUIRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData::EditorData(const EditorData &rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData &EditorData::operator=(EditorData &rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData::~EditorData()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorData::addRecentMap(const std::string &name)
   {
      for(std::list<std::string>::iterator i = recentMaps.begin();
          i != recentMaps.end(); ++i)
      {
         if((*i) == name)
         {
            recentMaps.erase(i);
            break;
         }
      }
      
      if(recentMaps.size() < 4)
         recentMaps.push_front(name);
      else
      {
         recentMaps.pop_back();
         recentMaps.push_front(name);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorData::addRecentProject(const std::string &name)
   {
      for(std::list<std::string>::iterator i = recentProjects.begin();
          i != recentProjects.end(); ++i)
      {
         if((*i) == name)
         {
            recentProjects.erase(i);
            break;
         }
      }
      
      if(recentProjects.size() < 4)
         recentProjects.push_front(name);
      else
      {
         recentProjects.pop_back();
         recentProjects.push_front(name);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData &EditorData::GetInstance()
   {
      if (EditorData::instance.get() == NULL)
         EditorData::instance = new EditorData();
      return *(EditorData::instance.get());
   }
   
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setMainWindow(MainWindow *window)
   {
      mainWindow = window;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentMap(dtDAL::Map *map)
   {
      myMap = map;
   }

   //////////////////////////////////////////////////////////////////////////////
   dtDAL::Map* EditorData::getCurrentMap()
   {
      return myMap.get();
   }
   
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::GetSelectedActors(std::vector<dtDAL::ActorProxy*> toFill)
   {
      getMainWindow()->GetPropertyEditor().GetSelectedActors(toFill);
   }
   
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentSoundResource(dtDAL::ResourceDescriptor newResource)
   {
      soundResource = newResource;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentMeshResource(dtDAL::ResourceDescriptor newResource)
   {
      meshResource = newResource;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentTextureResource(dtDAL::ResourceDescriptor newResource)
   {
      textureResource = newResource;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentParticleResource(dtDAL::ResourceDescriptor newResource)
   {
      particleResource = newResource;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentCharacterResource(dtDAL::ResourceDescriptor newResource)
   {
      characterResource = newResource;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentTerrainResource(dtDAL::ResourceDescriptor newResource)
   {
      terrainResource = newResource;
   }
}

