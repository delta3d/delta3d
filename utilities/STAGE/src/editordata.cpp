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
#include <dtCore/actorfactory.h>
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
      , mActorCreationDistance(5.0f)
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

      static const std::string AUDIO_ACTOR_LIBRARY("dtAudio");
      static const std::string ANIM_ACTOR_LIBRARY("dtAnim");
      static const std::string PHYSICS_ACTOR_LIBRARY("dtPhysics");
      // TODO This should be in the config somewhere.
      dtCore::ActorFactory::GetInstance().LoadActorRegistry(dtCore::ActorFactory::DEFAULT_ACTOR_LIBRARY);
      dtCore::ActorFactory::GetInstance().LoadOptionalActorRegistry(AUDIO_ACTOR_LIBRARY);
      dtCore::ActorFactory::GetInstance().LoadOptionalActorRegistry(ANIM_ACTOR_LIBRARY);
      dtCore::ActorFactory::GetInstance().LoadOptionalActorRegistry(PHYSICS_ACTOR_LIBRARY);
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
   void EditorData::GetSelectedActors(dtCore::ActorPtrVector& toFill)
   {
      PropertyEditor* propEditor = getMainWindow()->GetPropertyEditor();
      if(propEditor != NULL)
      {
         dtCore::PropContPtrVector toFillPC;

         propEditor->GetSelectedPropertyContainers(toFillPC);

         toFill.reserve(toFill.size() + toFillPC.size());
         for (size_t i = 0; i != toFillPC.size(); ++i)
         {
            // This is very dangerous, but at the moment, necessary.
            toFill.push_back(static_cast<dtCore::BaseActorObject*>(toFillPC[i]));
         }

      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorData::setCurrentResource(dtCore::DataType& type,
                                       const dtCore::ResourceDescriptor& selectedResource)
   {
      mCurrentResources[&type] = selectedResource;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor EditorData::getCurrentResource(dtCore::DataType& type)
   {
      return mCurrentResources[&type];
   }
} // namespace dtEditQt
