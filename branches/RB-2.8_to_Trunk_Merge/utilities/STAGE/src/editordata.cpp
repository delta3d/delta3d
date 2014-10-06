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
#include <prefix/stageprefix.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/propertyeditor.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/baseuiplugin.h>
#include <dtCore/map.h>
#include <dtCore/datatype.h>
#include <dtUtil/log.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QPluginLoader>
#include <QtCore/QFileInfo>

namespace dtEditQt
{

   // Singleton global variable for this class.
   dtCore::RefPtr<EditorData> EditorData::sInstance;
   ///////////////////////////////////////////////////////////////////////////////
   EditorData::EditorData()
      : mMainWindow(NULL)
      , mGroupUIRegistry(new GroupUIRegistry)
      , mGridSize(16)
      , mNumRecentProjects(5)
      , mLoadLastProject(true)
      , mLoadLastMap(true)
      , mRigidCamera(true)
      , mUseGlobalOrientationForViewportWidget(false)
      , mActorCreationDistance(5.f)
      , mSelectionColor(Qt::red)
   {
      LOG_DEBUG("Initializing Editor Data.");

      LOG_DEBUG("--- Registering Static Group UI Plugins ---");
      const QObjectList& staticPluginList = QPluginLoader::staticInstances();
      for (QObjectList::const_iterator i = staticPluginList.constBegin(); i != staticPluginList.constEnd(); ++i)
      {
         QObject* o = *i;
         BaseUIPlugin* groupUI = qobject_cast<BaseUIPlugin*>(o);
         if (groupUI != NULL)
         {
            mGroupUIRegistry->RegisterPlugin(*groupUI);
         }
      }
      LOG_DEBUG("--- Finished Registering Static Group UI Plugins ---");
   }

   ///////////////////////////////////////////////////////////////////////////////
   GroupUIRegistry& EditorData::GetGroupUIRegistry()
   {
      return *mGroupUIRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData::EditorData(const EditorData& rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData& EditorData::operator=(EditorData& rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData::~EditorData()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorData::addRecentMap(std::string name)
   {
      for (std::list<std::string>::iterator i = mRecentMaps.begin();
         i != mRecentMaps.end();
         ++i)
      {
         if ((*i) == name)
         {
            mRecentMaps.erase(i);
            break;
         }
      }

      if (mRecentMaps.size() < 4)
      {
         mRecentMaps.push_front(name);
      }
      else
      {
         mRecentMaps.pop_back();
         mRecentMaps.push_front(name);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void EditorData::addRecentProject(std::string name)
   {
      QFileInfo fileinfo(QString::fromStdString(name));

      for (std::list<std::string>::iterator i = mRecentProjects.begin();
         i != mRecentProjects.end(); ++i)
      {
         if ((*i) == fileinfo.absoluteFilePath().toStdString())
         {
            mRecentProjects.erase(i);
            break;
         }
      }

      if (mRecentProjects.size() < mNumRecentProjects)
      {
         mRecentProjects.push_front(fileinfo.absoluteFilePath().toStdString());
      }
      else
      {
         mRecentProjects.pop_back();
         mRecentProjects.push_front(fileinfo.absoluteFilePath().toStdString());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorData &EditorData::GetInstance()
   {
      if (EditorData::sInstance.get() == NULL)
      {
         EditorData::sInstance = new EditorData();
      }
      return *(EditorData::sInstance.get());
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setMainWindow(MainWindow* window)
   {
      mMainWindow = window;
   }
   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentMap(dtCore::Map* map)
   {
      mMap = map;
   }

   //////////////////////////////////////////////////////////////////////////////
   dtCore::Map* EditorData::getCurrentMap()
   {
      return mMap.get();
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::GetSelectedActors(std::vector<dtCore::BaseActorObject*>& toFill)
   {
      PropertyEditor* propEditor = getMainWindow()->GetPropertyEditor();
      if(propEditor != NULL)
      {
         std::vector<dtCore::PropertyContainer*> toFillPC;

         propEditor->GetSelectedPropertyContainers(toFillPC);

         toFill.clear();
         toFill.reserve(toFillPC.size());
         for (size_t i = 0; i != toFillPC.size(); ++i)
         {
            toFill.push_back(static_cast<dtCore::BaseActorObject*>(toFillPC[i]));
         }

      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentSoundResource(dtCore::ResourceDescriptor newResource)
   {
      mSoundResource = newResource;
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentMeshResource(dtCore::ResourceDescriptor newResource)
   {
      mMeshResource = newResource;
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentTextureResource(dtCore::ResourceDescriptor newResource)
   {
      mTextureResource = newResource;
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentParticleResource(dtCore::ResourceDescriptor newResource)
   {
      mParticleResource = newResource;
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentTerrainResource(dtCore::ResourceDescriptor newResource)
   {
      mTerrainResource = newResource;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentPrefabResource(dtCore::ResourceDescriptor newResource)
   {
      mPrefabResource = newResource;
   }

   //////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentSkeletalModelResource(const dtCore::ResourceDescriptor selectedResource)
   {
      mSkeletalModelResource = selectedResource;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentShaderResource(const dtCore::ResourceDescriptor selectedResource)
   {
      mShaderResource = selectedResource;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentDirectorResource(const dtCore::ResourceDescriptor selectedResource)
   {
      mDirectorResource = selectedResource;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentResource(const dtCore::DataType& type,
                                       const dtCore::ResourceDescriptor& selectedResource)
   {
      if (type == dtCore::DataType::SOUND)
      {
         setCurrentSoundResource(selectedResource);
      }
      else if (type == dtCore::DataType::STATIC_MESH)
      {
         setCurrentMeshResource(selectedResource);
      }
      else if (type == dtCore::DataType::TEXTURE)
      {
         setCurrentTextureResource(selectedResource);
      }
      else if (type == dtCore::DataType::PARTICLE_SYSTEM)
      {
         setCurrentParticleResource(selectedResource);
      }
      else if (type == dtCore::DataType::TERRAIN)
      {
         setCurrentTerrainResource(selectedResource);
      }
      else if (type == dtCore::DataType::SKELETAL_MESH)
      {
         setCurrentSkeletalModelResource(selectedResource);
      }
      else if (type == dtCore::DataType::SHADER)
      {
         setCurrentShaderResource(selectedResource);
      }
      else if (type == dtCore::DataType::PREFAB)
      {
         setCurrentPrefabResource(selectedResource);
      }
      else if (type == dtCore::DataType::DIRECTOR)
      {
         setCurrentDirectorResource(selectedResource);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor EditorData::getCurrentResource(const dtCore::DataType& type)
   {
      if (type == dtCore::DataType::SOUND)
      {
         return getCurrentSoundResource();
      }
      else if (type == dtCore::DataType::STATIC_MESH)
      {
         return getCurrentMeshResource();
      }
      else if (type == dtCore::DataType::TEXTURE)
      {
         return getCurrentTextureResource();
      }
      else if (type == dtCore::DataType::PARTICLE_SYSTEM)
      {
         return getCurrentParticleResource();
      }
      else if (type == dtCore::DataType::TERRAIN)
      {
         return getCurrentTerrainResource();
      }
      else if (type == dtCore::DataType::SKELETAL_MESH)
      {
         return getCurrentSkeletalModelResource();
      }
      else if (type == dtCore::DataType::SHADER)
      {
         return getCurrentShaderResource();
      }
      else if (type == dtCore::DataType::PREFAB)
      {
         return getCurrentPrefabResource();
      }
      else if (type == dtCore::DataType::DIRECTOR)
      {
         return getCurrentDirectorResource();
      }
      else
      {
         return dtCore::ResourceDescriptor();
      }
   }
} // namespace dtEditQt
