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
 * Author: Jeff P. Houde
 */

#include <dtDirectorQt/libraryeditor.h>
#include <dtDirectorQt/directoreditor.h>

#include <dtDirector/director.h>
#include <dtDirector/nodepluginregistry.h>
#include <dtDirector/nodemanager.h>

#include <QtCore/QStringList>
#include <dtDAL/librarymanager.h>
#include <dtUtil/log.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QListWidgetItem>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QGroupBox>

#include <osgDB/FileNameUtils>

#include <sstream>

namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   LibraryEditor::LibraryEditor(DirectorEditor* editor)
      : dtQt::BaseLibraryListEditor(editor)
      , mEditor(editor)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   LibraryEditor::~LibraryEditor()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void LibraryEditor::SpawnFileBrowser()
   {
      const std::pair<std::string, std::string> libPair = SelectLibraryToOpen(mCurrentDir);

      const std::string& libName = libPair.first;
      const std::string& filePath = libPair.second;
      const std::string& pathOnly = libPair.second.substr(0, libPair.second.find_last_of("\\/"));

      if (filePath.empty())
      {
         return;
      }

      //if (libName == "dtDirectorNodes" || libName == "dtDirectorNodesd")
      //{
      //   QMessageBox::information(this, tr("Library already loaded"),
      //      tr("This is the base Delta3D Director library and is loaded by default"),
      //      tr("&OK"));
      //   return;
      //}

      // If the map already contains this library, no point in continuing
      std::vector<std::string> curLibs = mEditor->GetDirector()->GetAllLibraries();
      for (unsigned int i = 0; i < curLibs.size(); ++i)
      {
         if (curLibs[i] == libName)
         {
            return;
         }
      }

      // before attempting to load, ensure the filePath is in search path list
      dtUtil::LibrarySharingManager::GetInstance().AddToSearchPath(pathOnly);

      try
      {
         //NodeManager::GetInstance().LoadNodeRegistry(libName);
         mEditor->GetDirector()->AddLibrary(libName, "");
      }
      catch(const dtUtil::Exception& e)
      {
         LOG_ERROR(e.What());
         HandleFailure(ERROR_INVALID_LIB, e.What());
         return;
      }

      RefreshLibraries();
      GetLibraryListWidget().setCurrentItem(GetLibraryListWidget().item(GetLibraryListWidget().count() - 1));
      mCurrentDir = filePath;
      mEditor->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void LibraryEditor::SpawnDeleteConfirmation()
   {
      if (QMessageBox::question(this, tr("Confirm deletion"),
         tr("Are you sure you want to remove this library?"),
         tr("&Yes"), tr("&No"), QString::null, 1) == 0)
      {
         std::vector<Node*> nodes;
         mEditor->GetDirector()->GetAllNodes(nodes);

         std::vector<std::string> loadedLibs = mEditor->GetDirector()->GetAllLibraries();

         std::string libToRemove = GetLibraryListWidget().currentItem()->text().toStdString();
         // Does the map have this library?
         for (unsigned int i = 0; i < loadedLibs.size(); ++i)
         {
            if (loadedLibs[i] == libToRemove)
            {
               NodePluginRegistry* reg =
                  NodeManager::GetInstance().GetRegistry(loadedLibs[i]);

               unsigned int foundNodes = 0;

               // Fail if we are still using nodes in the removed library.
               int nodeCount = (int)nodes.size();
               for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
               {
                  Node* node = nodes[nodeIndex];
                  if (node)
                  {
                     dtCore::RefPtr<const NodeType> type = &node->GetType();
                     if (reg->IsNodeTypeSupported(type))
                     {
                        foundNodes++;
                     }
                  }
               }

               if (foundNodes > 0)
               {
                  std::ostringstream ss;
                  ss << "Failed to remove the library, \""  << foundNodes << "\" nodes exist in the system that depend on it.";
                  HandleFailure(ERROR_OBJECTS_IN_LIB_EXIST, ss.str());
                  return;
               }

               mEditor->GetDirector()->RemoveLibrary(libToRemove);
               //NodeManager::GetInstance().UnloadNodeRegistry(libToRemove);

               RefreshLibraries();

               if (mEditor->GetDirector()->GetAllLibraries().size() > 0)
                  GetLibraryListWidget().setCurrentItem(GetLibraryListWidget().item(GetLibraryListWidget().count() - 1));

               break;
            }
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void LibraryEditor::ShiftLibraryUp()
   {
      if (GetLibraryListWidget().currentItem() != NULL && GetLibraryListWidget().currentRow() > 0)
      {
         int row = GetLibraryListWidget().currentRow();
         mEditor->GetDirector()->InsertLibrary(row - 1, GetLibraryListWidget().currentItem()->text().toStdString(), "");
         dtQt::BaseLibraryListEditor::ShiftLibraryUp();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void LibraryEditor::ShiftLibraryDown()
   {
      if (GetLibraryListWidget().currentItem() != NULL && GetLibraryListWidget().currentRow() < (GetLibraryListWidget().count() - 1))
      {
         int row = GetLibraryListWidget().currentRow();
         mEditor->GetDirector()->InsertLibrary(row + 1, GetLibraryListWidget().currentItem()->text().toStdString(), "");
         dtQt::BaseLibraryListEditor::ShiftLibraryDown();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void LibraryEditor::GetLibraryNames(std::vector<QListWidgetItem*>& items) const
   {
      items.clear();

      const std::vector<std::string>& libNames = mEditor->GetDirector()->GetAllLibraries();

      int libCount = (int)libNames.size();
      for (int libIndex = 0; libIndex < libCount; libIndex++)
      {
         QListWidgetItem* p = new QListWidgetItem;
         NodePluginRegistry* reg =
            NodeManager::GetInstance().GetRegistry(libNames[libIndex]);
         QString toolTip = tr("File: ") +
            tr(NodeManager::GetInstance().GetPlatformSpecificLibraryName(libNames[libIndex]).c_str()) +
            tr(" \nDescription: ") + tr(reg->GetDescription().c_str());
         p->setText(tr(libNames[libIndex].c_str()));
         p->setToolTip(toolTip);
         items.push_back(p);
      }
   }
} // namespace dtDirector
