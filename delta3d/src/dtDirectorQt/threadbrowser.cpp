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

#include <dtUtil/stringutils.h>

#include <QtGui/QWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QListWidget>
#include <QtGui/QGraphicsItem>


namespace dtDirector
{
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

      mGroupBox = new QGroupBox("Replay Threads", mainAreaWidget);
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
      if (!mEditor)
      {
         return;
      }

      mThreadList->blockSignals(true);

      mThreadList->clear();

      Director* director = mEditor->GetDirector();
      if (!director) return;

      Director* topDirector = director;
      while (director->GetParent())
      {
         topDirector = director->GetParent();
      }

      //int threadCount = (int)threads.size();
      //for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
      //{
      //   Director::RecordThreadData* thread = threads[threadIndex];

      //   TestThreadGraph(mEditor->GetPropertyEditor()->GetScene()->GetGraph(), thread);
      //}

      //// If there are any threads to return back to, add the back option.
      //if (mNodePath.size() > 1)
      //{
      //   new ReplayThreadItem(mEditor, NULL, NULL, NULL, 0, mThreadList);
      //   new ReplayThreadItem(NULL, NULL, NULL, NULL, 0, mThreadList);
      //}

      mThreadList->blockSignals(false);
   }

   //////////////////////////////////////////////////////////////////////////
   void ThreadBrowser::OnItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
   {
      //ReplayThreadItem* item = dynamic_cast<ReplayThreadItem*>(current);
      //if (item && item->IsValid())
      //{
      //   InputLink* input = NULL;
      //   Node* node = mEditor->GetDirector()->GetNode(item->GetNode()->nodeID);
      //   if (node)
      //   {
      //      input = node->GetInputLink(item->GetNode()->input);
      //   }

      //   mEditor->SetReplayNode(item->GetNode(), input, item->GetOutput());
      //}
      //else if (item && item->IsBackOption() && mNodePath.size() > 1)
      //{
      //   ThreadPathData curNode = mNodePath.top();
      //   mNodePath.pop();

      //   Node* node = mEditor->GetDirector()->GetNode(curNode.node->nodeID);
      //   if (node)
      //   {
      //      InputLink* input = node->GetInputLink(curNode.node->input);
      //      OutputLink* output = curNode.output;

      //      mEditor->SetReplayNode(mNodePath.top().node, input, output);
      //   }

      //   mNodePath.push(curNode);
      //}
      //else
      //{
      //   mEditor->SetReplayNode();
      //}
      //mEditor->GetPropertyEditor()->GetScene()->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void ThreadBrowser::OnItemDoubleClicked(QListWidgetItem* current)
   {
      //ReplayThreadItem* item = dynamic_cast<ReplayThreadItem*>(current);
      //if (item)
      //{
      //   if (item->IsValid() || item->IsBackOption())
      //   {
      //      mEditor->SetReplayNode();

      //      mCurrentNode.node = NULL;
      //      if (item->IsValid())
      //      {
      //         mCurrentNode.node   = item->GetNode();
      //         mCurrentNode.output = item->GetOutput();

      //         mNodePath.push(mCurrentNode);
      //      }
      //      else if (mNodePath.size() > 1)
      //      {
      //         mNodePath.pop();

      //         mCurrentNode = mNodePath.top();
      //      }

      //      if (mCurrentNode.node)
      //      {
      //         Node* node = mEditor->GetDirector()->GetNode(mCurrentNode.node->nodeID);
      //         if (node)
      //         {
      //            mEditor->GetPropertyEditor()->GetScene()->SetGraph(node->GetGraph());
      //            NodeItem* nodeItem = mEditor->GetPropertyEditor()->GetScene()->GetNodeItem(node->GetID(), true);
      //            if (nodeItem)
      //            {
      //               nodeItem->setSelected(true);
      //               mEditor->GetPropertyEditor()->GetScene()->CenterSelection();
      //            }
      //         }
      //      }

      //      mCurrentNode.node = NULL;
      //   }
      //}
   }
}

//////////////////////////////////////////////////////////////////////////
