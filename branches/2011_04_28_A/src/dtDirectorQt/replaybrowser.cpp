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

#include <dtDirectorQt/replaybrowser.h>
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
   ReplayThreadItem::ReplayThreadItem(DirectorEditor* editor, Director::RecordThreadData* thread, Director::RecordNodeData* nodeData, OutputLink* output, float time, QListWidget* parent)
      : QListWidgetItem(parent)
      , mEditor(editor)
      , mNode(NULL)
      , mOutput(output)
      , mThread(thread)
      , mValid(false)
   {
      if (nodeData)
      {
         mNode = nodeData;
         mValid = true;
      }

      if (!mEditor)
      {
         setText("<--Back");
         setTextColor(Qt::blue);
         return;
      }

      if (nodeData && mEditor->GetDirector())
      {
         Node* node = mEditor->GetDirector()->GetNode(nodeData->nodeID);
         if (!node)
         {
            setText("Node not found!");
            setTextColor(Qt::darkRed);
            return;
         }

         std::string text;

         if (time < nodeData->time)
         {
            float elapsedTime = nodeData->time - time;
            text += "[+" + dtUtil::ToString(elapsedTime) + "]: ";
         }

         if (!nodeData->input.empty())
         {
            text += "Fired (" + nodeData->input + ") on ";
         }

         if (node->AsEventNode())
         {
            text += "Event ";
         }
         else if (node->AsActionNode())
         {
            text += "Action ";
         }

         text += "Node \'" + node->GetName() + "\'";
         
         if (!node->GetComment().empty())
         {
            text += " - " + node->GetComment();
         }

         setText(text.c_str());
      }
      else 
      {
         setText("-----------------------");
         setTextColor(Qt::darkGreen);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////////////////////////////
   ReplayBrowser::ReplayBrowser(QMainWindow* parent)
      : QDockWidget(parent)
      , mGroupBox(NULL)
      , mThreadList(NULL)
   {
      mCurrentNode.node = NULL;

      setWindowTitle("Replay Browser");

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
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ReplayBrowser::SetDirectorEditor(DirectorEditor* editor)
   {
      mEditor = editor;
   }

   //////////////////////////////////////////////////////////////////////////
   void ReplayBrowser::BuildThreadList()
   {
      if (!mEditor || !mEditor->GetPropertyEditor()->GetScene()) {return;}

      disconnect(mThreadList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
         this, SLOT(OnItemChanged(QListWidgetItem*, QListWidgetItem*)));

      mEditor->SetReplayNode();
      mEditor->GetPropertyEditor()->GetScene()->Refresh();

      mThreadList->clear();

      if (!mCurrentNode.node)
      {
         while (!mNodePath.empty())
         {
            mNodePath.pop();
         }
      }

      Director* director = mEditor->GetDirector();
      if (!director) return;

      std::vector<Director::RecordThreadData*> threads = director->GetRecordingData();

      // Get the currently selected items.
      QList<QGraphicsItem*> selected = mEditor->GetPropertyEditor()->GetScene()->selectedItems();
      int count = (int)selected.size();

      // If no items are selected, show all threads that are in the same graph.
      if (count == 0)
      {
         int threadCount = (int)threads.size();
         for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
         {
            Director::RecordThreadData* thread = threads[threadIndex];

            TestThreadGraph(mEditor->GetPropertyEditor()->GetScene()->GetGraph(), thread);
         }
      }
      else
      {
         for (int index = 0; index < count; index++)
         {
            NodeItem* nodeItem = dynamic_cast<NodeItem*>(selected[index]);
            if (nodeItem)
            {
               bool addSpacer = false;
               int threadCount = (int)threads.size();
               for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
               {
                  int currentCount = mThreadList->count();

                  Director::RecordThreadData* thread = threads[threadIndex];

                  if (TestThreadNode(nodeItem->GetID(), thread))
                  {
                     if (addSpacer)
                     {
                        ReplayThreadItem* item = new ReplayThreadItem(mEditor, NULL, NULL, NULL, 0, NULL);
                        mThreadList->insertItem(currentCount, item);
                     }

                     addSpacer = true;
                  }
               }
            }
         }

         // If there are any threads to return back to, add the back option.
         if (mNodePath.size() > 1)
         {
            new ReplayThreadItem(mEditor, NULL, NULL, NULL, 0, mThreadList);
            new ReplayThreadItem(NULL, NULL, NULL, NULL, 0, mThreadList);
         }
      }

      connect(mThreadList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
         this, SLOT(OnItemChanged(QListWidgetItem*, QListWidgetItem*)));
   }

   //////////////////////////////////////////////////////////////////////////
   void ReplayBrowser::OnItemChanged(QListWidgetItem* current, QListWidgetItem* previous)
   {
      ReplayThreadItem* item = dynamic_cast<ReplayThreadItem*>(current);
      if (item && item->IsValid())
      {
         InputLink* input = NULL;
         Node* node = mEditor->GetDirector()->GetNode(item->GetNode()->nodeID);
         if (node)
         {
            input = node->GetInputLink(item->GetNode()->input);
         }

         mEditor->SetReplayNode(item->GetNode(), input, item->GetOutput());
      }
      else if (item && item->IsBackOption() && mNodePath.size() > 1)
      {
         ThreadPathData curNode = mNodePath.top();
         mNodePath.pop();

         Node* node = mEditor->GetDirector()->GetNode(curNode.node->nodeID);
         if (node)
         {
            InputLink* input = node->GetInputLink(curNode.node->input);
            OutputLink* output = curNode.output;

            mEditor->SetReplayNode(mNodePath.top().node, input, output);
         }

         mNodePath.push(curNode);
      }
      else
      {
         mEditor->SetReplayNode();
      }
      mEditor->GetPropertyEditor()->GetScene()->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void ReplayBrowser::OnItemDoubleClicked(QListWidgetItem* current)
   {
      ReplayThreadItem* item = dynamic_cast<ReplayThreadItem*>(current);
      if (item)
      {
         if (item->IsValid() || item->IsBackOption())
         {
            mEditor->SetReplayNode();

            mCurrentNode.node = NULL;
            if (item->IsValid())
            {
               mCurrentNode.node   = item->GetNode();
               mCurrentNode.output = item->GetOutput();

               mNodePath.push(mCurrentNode);
            }
            else if (mNodePath.size() > 1)
            {
               mNodePath.pop();

               mCurrentNode = mNodePath.top();
            }

            if (mCurrentNode.node)
            {
               Node* node = mEditor->GetDirector()->GetNode(mCurrentNode.node->nodeID);
               if (node)
               {
                  mEditor->GetPropertyEditor()->GetScene()->SetGraph(node->GetGraph());
                  NodeItem* nodeItem = mEditor->GetPropertyEditor()->GetScene()->GetNodeItem(node->GetID(), true);
                  if (nodeItem)
                  {
                     nodeItem->setSelected(true);
                     mEditor->GetPropertyEditor()->GetScene()->CenterSelection();
                  }
               }
            }

            mCurrentNode.node = NULL;
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool ReplayBrowser::TestThreadGraph(DirectorGraph* graph, Director::RecordThreadData* thread)
   {
      if (!thread) return false;

      // Find the first valid node within this thread.
      int count = (int)thread->nodes.size();
      for (int index = 0; index < count; index++)
      {
         Director::RecordNodeData& nodeData = thread->nodes[index];
         Node* node = mEditor->GetDirector()->GetNode(nodeData.nodeID);
         if (node)
         {
            if (node->GetGraph() == graph)
            {
               new ReplayThreadItem(mEditor, thread, &nodeData, NULL, 0, mThreadList);
               return true;
            }
            return false;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ReplayBrowser::TestThreadNode(const dtCore::UniqueId& nodeID, Director::RecordThreadData* thread, bool testCurrentThread)
   {
      if (!thread) return false;

      // Search this thread for the node.
      bool result = false;
      int nodeCount = (int)thread->nodes.size();
      float nodeTime = -1.0f;

      for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
      {
         Director::RecordNodeData& node = thread->nodes[nodeIndex];
         
         // If we are in a specific thread, only look for things that are a branch
         // of the current.
         if (testCurrentThread && mCurrentNode.node && mCurrentNode.node != &node)
         {
            int threadCount = (int)node.subThreads.size();
            for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
            {
               Director::RecordThreadData* subThread = node.subThreads[threadIndex];

               if (TestThreadNode(nodeID, subThread, true))
               {
                  result = true;
               }
            }

            continue;
         }
         else
         {
            // Make sure that any sub-thread tested from this point does not
            // check the current thread, as they are all children of that thread.
            testCurrentThread = false;
         }

         // If we have found our node.
         if (nodeID == node.nodeID)
         {
            Node* curNode = mEditor->GetDirector()->GetNode(node.nodeID);

            if (nodeTime == -1.0f)
            {
               nodeTime = node.time;
            }

            bool pushedNode = false;
            int linkCount = 0;

            // First push the next node into the list.
            if (nodeIndex + 1 < nodeCount)
            {
               Director::RecordNodeData& nextNode = thread->nodes[nodeIndex + 1];

               if (nextNode.nodeID != nodeID)
               {
                  pushedNode = true;

                  if (curNode)
                  {
                     int linkIndex = -1;
                     int triggerCount = (int)node.outputs.size();
                     for (int triggerIndex = 0; triggerIndex < triggerCount; triggerIndex++)
                     {
                        if (linkIndex >= linkCount) break;

                        OutputLink* link = curNode->GetOutputLink(node.outputs[triggerIndex]);
                        if (!link)
                        {
                           linkIndex++;
                           continue;
                        }

                        bool pushedLink = false;
                        int outputCount = (int)link->GetLinks().size();
                        for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
                        {
                           linkIndex++;

                           if (linkIndex == linkCount)
                           {
                              new ReplayThreadItem(mEditor, thread, &nextNode, link, nodeTime, mThreadList);
                              pushedLink = true;
                              linkCount++;
                              result = true;
                              break;
                           }
                        }

                        if (!pushedLink)
                        {
                           new ReplayThreadItem(mEditor, thread, &nextNode, NULL, nodeTime, mThreadList);
                        }
                     }
                  }
               }
            }

            // Add the first node from our sub-threads to the out list.
            int threadCount = (int)node.subThreads.size();
            for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
            {
               Director::RecordThreadData* subThread = node.subThreads[threadIndex];
               if (subThread)
               {
                  int subNodeCount = (int)subThread->nodes.size();
                  for (int subNodeIndex = 0; subNodeIndex < subNodeCount; subNodeIndex++)
                  {
                     Director::RecordNodeData* subNode = &subThread->nodes[subNodeIndex];
                     if (subNode)
                     {
                        pushedNode = true;

                        if (curNode)
                        {
                           int linkIndex = -1;
                           int triggerCount = (int)node.outputs.size();
                           for (int triggerIndex = 0; triggerIndex < triggerCount; triggerIndex++)
                           {
                              if (linkIndex >= linkCount) break;

                              OutputLink* link = curNode->GetOutputLink(node.outputs[triggerIndex]);
                              if (!link)
                              {
                                 linkIndex++;
                                 continue;
                              }

                              bool pushedLink = false;
                              int outputCount = (int)link->GetLinks().size();
                              for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
                              {
                                 linkIndex++;

                                 if (linkIndex == linkCount)
                                 {
                                    new ReplayThreadItem(mEditor, thread, subNode, link, nodeTime, mThreadList);
                                    pushedLink = true;
                                    linkCount++;
                                    result = true;
                                    break;
                                 }
                              }

                              if (!pushedLink)
                              {
                                 new ReplayThreadItem(mEditor, thread, subNode, NULL, nodeTime, mThreadList);
                              }
                           }
                        }
                        break;
                     }
                  }
               }
            }
         }
         // If we did not find the node, check all of the current node's
         // sub-threads.
         else
         {
            int threadCount = (int)node.subThreads.size();
            for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
            {
               Director::RecordThreadData* subThread = node.subThreads[threadIndex];

               if (TestThreadNode(nodeID, subThread, testCurrentThread))
               {
                  result = true;
               }
            }
         }
      }

      return result;
   }
}

//////////////////////////////////////////////////////////////////////////
