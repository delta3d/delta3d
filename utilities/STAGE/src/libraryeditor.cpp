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
 * William E. Johnson II
 */
#include <prefix/stageprefix.h>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QListWidgetItem>
#include <QtGui/QListWidget>
#include <QtCore/QStringList>
#include <QtGui/QMainWindow>
#include <QtGui/QGroupBox>

#include <dtEditQt/libraryeditor.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/mainwindow.h>
#include <dtCore/actorfactory.h>
#include <dtCore/actorpluginregistry.h>
#include <dtCore/map.h>
#include <dtUtil/log.h>

#include <osgDB/FileNameUtils>

#include <sstream>

namespace dtEditQt
{

   LibraryEditor::LibraryEditor(QWidget* parent)
      : dtQt::BaseLibraryListEditor(parent)
   {
   }

   LibraryEditor::~LibraryEditor()
   {
   }

   void LibraryEditor::GetLibraryNames(std::vector<QListWidgetItem*>& items) const
   {
      items.clear();
      dtCore::Map* currentMap = EditorData::GetInstance().getCurrentMap();
      if (currentMap == NULL)
      {
         return;
      }

      const std::vector<std::string>& libNames = currentMap->GetAllLibraries();

      for (unsigned int i = 0; i < libNames.size(); ++i)
      {
         QListWidgetItem* p = new QListWidgetItem;
         dtCore::ActorPluginRegistry* reg =
            dtCore::ActorFactory::GetInstance().GetRegistry(libNames[i]);
         QString toolTip = tr("File: ") +
            tr(dtCore::ActorFactory::GetInstance().GetPlatformSpecificLibraryName(libNames[i]).c_str()) +
            tr(" \nDescription: ") + tr(reg->GetDescription().c_str());
         p->setText(tr(libNames[i].c_str()));
         p->setToolTip(toolTip);
         items.push_back(p);
      }
   }

   ///////////////////////// Slots /////////////////////////
   void LibraryEditor::SpawnFileBrowser()
   {
      const std::pair<std::string, std::string> libPair = SelectLibraryToOpen(EditorData::GetInstance().getCurrentLibraryDirectory());

      const std::string& libName = libPair.first;
      const std::string& filePath = libPair.second;
      const std::string& pathOnly = libPair.second.substr(0, libPair.second.find_last_of("\\/"));

      if (filePath.empty())
      {
         return;
      }

      if (libName == "dtActors" || libName == "dtActorsd")
      {
         QMessageBox::information(this, tr("Library already loaded"),
            tr("This is the base Delta3D actors library and is loaded by default"),
            tr("&OK"));
         return;
      }

      // If the map already contains this library, no point in continuing
      std::vector<std::string> curLibs = EditorData::GetInstance().getCurrentMap()->GetAllLibraries();
      for (unsigned int i = 0; i < curLibs.size(); ++i)
      {
         if (curLibs[i] == libName)
         {
            return;
         }
      }

      //before attempting to load, ensure the filePath is in search path list
      dtUtil::LibrarySharingManager::GetInstance().AddToSearchPath(pathOnly);

      try
      {
         dtCore::ActorFactory::GetInstance().LoadActorRegistry(libName);
      }
      catch(const dtUtil::Exception& e)
      {
         LOG_ERROR(e.What());
         HandleFailure(ERROR_INVALID_LIB, e.What());
         return;
      }
      EditorData::GetInstance().getCurrentMap()->AddLibrary(libName, "");

      RefreshLibraries();
      EditorEvents::GetInstance().emitMapLibraryImported();
      GetLibraryListWidget().setCurrentItem(GetLibraryListWidget().item(GetLibraryListWidget().count() - 1));
      EditorData::GetInstance().setCurrentLibraryDirectory(osgDB::getFilePath(filePath));
      EditorData::GetInstance().getMainWindow()->setWindowTitle(
         EditorActions::GetInstance().getWindowName().c_str());
   }

   void LibraryEditor::SpawnDeleteConfirmation()
   {
      if (QMessageBox::question(this, tr("Confirm deletion"),
         tr("Are you sure you want to remove this library?"),
         tr("&Yes"), tr("&No"), QString::null, 1) == 0)
      {
         dtCore::Map* curMap = EditorData::GetInstance().getCurrentMap();
         std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > proxies;
         curMap->GetAllProxies(proxies);
         std::vector<std::string> loadedLibs = curMap->GetAllLibraries();

         std::string libToRemove = GetLibraryListWidget().currentItem()->text().toStdString();
         // Does the map have this library?
         for (unsigned int i = 0; i < loadedLibs.size(); ++i)
         {
            if (loadedLibs[i] == libToRemove)
            {
               dtCore::ActorPluginRegistry* reg =
                  dtCore::ActorFactory::GetInstance().GetRegistry(loadedLibs[i]);

               unsigned int numActorsInScene = 0;
               // fail if actors are in the library
               for (unsigned int j = 0; j < proxies.size(); ++j)
               {
                  dtCore::RefPtr<const dtCore::ActorType> type = &proxies[j]->GetActorType();
                  if (reg->IsActorTypeSupported(*type))
                  {
                     ++numActorsInScene;
                  }
               }

               if (numActorsInScene > 0)
               {
                  std::ostringstream ss;
                  ss << "Failed to remove the library, \""  << numActorsInScene << "\" actors exist in the system that depend on it.";
                  HandleFailure(ERROR_OBJECTS_IN_LIB_EXIST, ss.str());
                  return;
               }

               EditorEvents::GetInstance().emitLibraryAboutToBeRemoved();
               curMap->RemoveLibrary(libToRemove);
               dtCore::ActorFactory::GetInstance().UnloadActorRegistry(libToRemove);
               RefreshLibraries();
               EditorEvents::GetInstance().emitMapLibraryRemoved();
               if (curMap->GetAllLibraries().size() > 0)
                  GetLibraryListWidget().setCurrentItem(GetLibraryListWidget().item(GetLibraryListWidget().count() - 1));
               // we're done
               break;
            }
         }
      }
   }

   void LibraryEditor::ShiftLibraryUp()
   {
      if (GetLibraryListWidget().currentItem() != NULL && GetLibraryListWidget().currentRow() > 0)
      {
         int row = GetLibraryListWidget().currentRow();
         dtCore::Map* curMap = EditorData::GetInstance().getCurrentMap();
         curMap->InsertLibrary(row - 1, GetLibraryListWidget().currentItem()->text().toStdString(), "");
         dtQt::BaseLibraryListEditor::ShiftLibraryUp();
      }
   }

   void LibraryEditor::ShiftLibraryDown()
   {
      if (GetLibraryListWidget().currentItem() != NULL && GetLibraryListWidget().currentRow() < (GetLibraryListWidget().count() - 1))
      {
         int row = GetLibraryListWidget().currentRow();
         dtCore::Map* curMap = EditorData::GetInstance().getCurrentMap();
         curMap->InsertLibrary(row + 1, GetLibraryListWidget().currentItem()->text().toStdString(), "");
         dtQt::BaseLibraryListEditor::ShiftLibraryDown();
      }
   }

} // namespace dtEditQt
