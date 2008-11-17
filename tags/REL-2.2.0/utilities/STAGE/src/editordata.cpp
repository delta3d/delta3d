/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
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
   EditorData::EditorData() : 
      mainWindow(NULL), 
      mGroupUIRegistry(new GroupUIRegistry), 
      gridSize(16), 
      loadLastProject(true), 
      loadLastMap(true), 
      rigidCamera(true), 
      selectionColor(Qt::red)    
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
   void EditorData::GetSelectedActors(std::vector<dtDAL::ActorProxy*>& toFill)
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
   void EditorData::setCurrentTerrainResource(dtDAL::ResourceDescriptor newResource)
   {
      terrainResource = newResource;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setOriginalOsgLibraryPath(const std::string &path)
   {
       osgLibraryPath = path;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentSkeletalModelResource(const dtDAL::ResourceDescriptor selectedResource)
   {
      skeletalModelResource = selectedResource;
   }
}

