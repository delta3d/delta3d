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

#include <dtDirectorQt/threadbrowser.h>
#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editorscene.h>
#include <dtDirectorQt/propertyeditor.h>
#include <dtDirectorQt/nodeitem.h>
#include <dtDirectorQt/editornotifier.h>

#include <dtUtil/stringutils.h>

#include <QtGui/QWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QListWidget>
#include <QtGui/QGraphicsItem>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ThreadItem::ThreadItem(Node* node, QListWidget* parent)
      : QListWidgetItem(parent)
      , mNode(node)
   {
      std::string text = mNode->GetTypeName();

      if (!mNode->GetName().empty())
      {
         text += "::";
         text += mNode->GetName();
      }

      if (!mNode->GetComment().empty())
      {
         text += " (";
         text += mNode->GetComment();
         text += ")";
      }

      setText(text.c_str());
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   ThreadBrowser::ThreadBrowser(QMainWindow* parent)
      : QDockWidget(parent)
      , mGroupBox(NULL)
      , mThreadList(NULL)
   {
      setWindowTitle("Thread Browser");

      QWidget* mainAreaWidget = new QWidget(this);
      QGridLayout* mainAreaLayout = new QGridLayout(mainAreaWidget);
      setWidget(mainAreaWidget);

      mGroupBox = new QGroupBox("Active Nodes", mainAreaWidget);
      mGroupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mainAreaLayout->addWidget(mGroupBox, 0, 0);
      QGridLayout* innerLayout = new QGridLayout(mGroupBox);

      mThreadList = new QListWidget(mGroupBox);
      innerLayout->addWidget(mThreadList);
      mThreadList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

      connect(mThreadList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
         this, SLOT(OnItemDoubleClicked(QListWidgetItem*)));
      connect(mThreadList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
         this, SLOT(OnItemChanged(QListWidgetItem*, QListWidgetItem*)));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ThreadBrowser::SetDirectorEditor(DirectorEditor* editor)
   {
      mEditor = editor;
   }

   //////////////////////////////////////////////////////////////////////////
   void ThreadBrowser::BuildThreadList()
   {
      mThreadList->clear();

      if (!mEditor)
      {
         return;
      }

      dtDirector::EditorNotifier* notifier =
         dynamic_cast<dtDirector::EditorNotifier*>(
         mEditor->GetDirector()->GetNotifier());

      if (!notifier)
      {
         return;
      }

      mThreadList->blockSignals(true);

      Director* director = mEditor->GetDirector();
      if (!director) return;

      Director* topDirector = director;
      while (topDirector->GetParent())
      {
         topDirector = director->GetParent();
      }

      const std::map<Node*, dtDirector::EditorNotifier::GlowData>& threads =
         notifier->GetGlowData();

      std::map<Node*, dtDirector::EditorNotifier::GlowData>::const_iterator iter;
      for (iter = threads.begin(); iter != threads.end(); ++iter)
      {
         const dtDirector::EditorNotifier::GlowData& data = iter->second;
         if (data.node.valid() && data.glow >= 1.0f)
         {
            // First find out if the node exists in the current script.
            Node* testNode = mEditor->GetDirector()->GetNode(data.node->GetID());
            if (testNode == data.node.get())
            {
               new ThreadItem(data.node.get(), mThreadList);
            }
         }
      }

      mThreadList->blockSignals(false);
   }

   //////////////////////////////////////////////////////////////////////////
   void ThreadBrowser::OnItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ThreadBrowser::OnItemDoubleClicked(QListWidgetItem* current)
   {
      ThreadItem* item = dynamic_cast<ThreadItem*>(current);
      if (item)
      {
         Node* node = item->GetNode();
         if (node)
         {
            mEditor->FocusNode(node);
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
