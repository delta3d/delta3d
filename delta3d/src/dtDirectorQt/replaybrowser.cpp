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

#include <QtGui/QWidget>
#include <QtGui/QGroupBox>
#include <QtGui/QGridLayout>
#include <QtGui/QListWidget>
#include <QtGui/QGraphicsItem>


namespace dtDirector
{
   //////////////////////////////////////////////////////////////////////////
   ReplayThreadItem::ReplayThreadItem(DirectorEditor* editor, Director::RecordNodeData* nodeData, OutputLink* output, QListWidget* parent)
      : QListWidgetItem(parent)
      , mEditor(editor)
      , mValid(false)
      , mOutput(output)
   {
      if (nodeData)
      {
         mNode = *nodeData;
         mValid = true;
      }

      if (nodeData && mEditor && mEditor->GetDirector())
      {
         Node* node = mEditor->GetDirector()->GetNode(nodeData->nodeID);
         if (!node)
         {
            setText("Node not found!");
            setTextColor(Qt::darkRed);
            return;
         }

         std::string text;

         if (!nodeData->input.empty())
         {
            text = "Fired (" + nodeData->input + ") on ";
         }

         if (dynamic_cast<EventNode*>(node))
         {
            text += "Event ";
         }
         else if (dynamic_cast<ActionNode*>(node))
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
   ReplayBrowser::ReplayBrowser(DirectorEditor* parent)
      : QDockWidget(parent)
      , mEditor(parent)
      , mGroupBox(NULL)
      , mThreadList(NULL)
      , mCurrentThread(NULL)
   {
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

   //////////////////////////////////////////////////////////////////////////
   void ReplayBrowser::BuildThreadList(bool keepThread)
   {
      if (!mEditor) return;

      if (!keepThread) mCurrentThread = false;

      disconnect(mThreadList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
         this, SLOT(OnItemChanged(QListWidgetItem*, QListWidgetItem*)));

      mEditor->SetReplayNode(NULL, NULL);
      mEditor->GetPropertyEditor()->GetScene()->Refresh();

      mThreadList->clear();

      Director* director = mEditor->GetDirector();
      if (!director) return;

      std::vector<Director::RecordThreadData> threads = director->GetRecordingData();

      // Get the currently selected items.
      QList<QGraphicsItem*> selected = mEditor->GetPropertyEditor()->GetScene()->selectedItems();
      int count = (int)selected.size();

      // If no items are selected, show all threads that are in the same graph.
      if (count == 0)
      {
         int threadCount = (int)threads.size();
         for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
         {
            Director::RecordThreadData& thread = threads[threadIndex];

            Director::RecordNodeData* node;
            if (TestThreadGraph(mEditor->GetPropertyEditor()->GetScene()->GetGraph(), thread, &node))
            {
               ReplayThreadItem* item = new ReplayThreadItem(mEditor, node, NULL, mThreadList);
            }
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
                  if (addSpacer)
                  {
                     ReplayThreadItem* item = new ReplayThreadItem(mEditor, NULL, NULL, mThreadList);
                     addSpacer = false;
                  }

                  Director::RecordThreadData& thread = threads[threadIndex];

                  std::vector<Director::RecordNodeData*> newNodes;
                  std::vector<OutputLink*> newLinks;
                  if (TestThreadNode(nodeItem->GetID(), thread, newNodes, newLinks))
                  {
                     int newNodeCount = (int)newNodes.size();
                     for (int newNodeIndex = 0; newNodeIndex < newNodeCount; newNodeIndex++)
                     {
                        Director::RecordNodeData* newNode = newNodes[newNodeIndex];
                        ReplayThreadItem* item = new ReplayThreadItem(mEditor, newNode, newLinks[newNodeIndex], mThreadList);
                        addSpacer = true;
                     }
                  }
               }
            }
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
         mEditor->SetReplayNode(&item->GetNode(), item->GetOutput());
      }
      else
      {
         mEditor->SetReplayNode(NULL, NULL);
      }
      mEditor->GetPropertyEditor()->GetScene()->Refresh();
   }

   //////////////////////////////////////////////////////////////////////////
   void ReplayBrowser::OnItemDoubleClicked(QListWidgetItem* current)
   {
      ReplayThreadItem* item = dynamic_cast<ReplayThreadItem*>(current);
      if (item && item->IsValid())
      {
         mEditor->SetReplayNode(NULL, NULL);

         Node* node = mEditor->GetDirector()->GetNode(item->GetNode().nodeID);
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

         //mEditor->GetPropertyEditor()->GetScene()->Refresh();
      }
   }

   //////////////////////////////////////////////////////////////////////////
   bool ReplayBrowser::TestThreadGraph(DirectorGraph* graph, Director::RecordThreadData& thread, Director::RecordNodeData** outNode)
   {
      // Find the first valid node within this thread.
      int count = (int)thread.nodes.size();
      for (int index = 0; index < count; index++)
      {
         Director::RecordNodeData& nodeData = thread.nodes[index];
         Node* node = mEditor->GetDirector()->GetNode(nodeData.nodeID);
         if (node)
         {
            if (node->GetGraph() == graph)
            {
               if (outNode) *outNode = &nodeData;
               return true;
            }
            return false;
         }
      }

      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   bool ReplayBrowser::TestThreadNode(const dtCore::UniqueId& nodeID, Director::RecordThreadData& thread, std::vector<Director::RecordNodeData*>& outNodes, std::vector<OutputLink*>& outLinks)
   {
      // Search this thread for the node.
      bool result = false;
      int nodeCount = (int)thread.nodes.size();
      for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
      {
         Director::RecordNodeData& node = thread.nodes[nodeIndex];
         
         // If we have found our node.
         if (nodeID == node.nodeID)
         {
            Node* curNode = mEditor->GetDirector()->GetNode(node.nodeID);

            bool pushedNode = false;
            int linkCount = 0;

            // First push the next node into the list.
            if (nodeIndex + 1 < nodeCount)
            {
               Director::RecordNodeData& nextNode = thread.nodes[nodeIndex + 1];

               if (nextNode.nodeID != nodeID)
               {
                  outNodes.push_back(&nextNode);
                  pushedNode = true;

                  bool pushedLink = false;
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

                        int outputCount = (int)link->GetLinks().size();
                        for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
                        {
                           linkIndex++;

                           if (linkIndex == linkCount)
                           {
                              outLinks.push_back(link);
                              pushedLink = true;
                              linkCount++;
                              break;
                           }
                        }
                     }
                  }

                  if (!pushedLink)
                  {
                     outLinks.push_back(NULL);
                  }
               }
            }

            // Add the first node from our sub-threads to the out list.
            int threadCount = (int)node.subThreads.size();
            for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
            {
               Director::RecordThreadData* subThread = &node.subThreads[threadIndex];
               if (subThread)
               {
                  int subNodeCount = (int)subThread->nodes.size();
                  for (int subNodeIndex = 0; subNodeIndex < subNodeCount; subNodeIndex++)
                  {
                     Director::RecordNodeData* subNode = &subThread->nodes[subNodeIndex];
                     if (subNode)
                     {
                        outNodes.push_back(subNode);
                        pushedNode = true;

                        bool pushedLink = false;
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

                              int outputCount = (int)link->GetLinks().size();
                              for (int outputIndex = 0; outputIndex < outputCount; outputIndex++)
                              {
                                 linkIndex++;

                                 if (linkIndex == linkCount)
                                 {
                                    outLinks.push_back(link);
                                    pushedLink = true;
                                    linkCount++;
                                    break;
                                 }
                              }
                           }
                        }

                        if (!pushedLink)
                        {
                           outLinks.push_back(NULL);
                        }
                        break;
                     }
                  }
               }
            }

            result = true;
         }
         // If we did not find the node, check all of the current node's
         // sub-threads.
         else
         {
            int threadCount = (int)node.subThreads.size();
            for (int threadIndex = 0; threadIndex < threadCount; threadIndex++)
            {
               Director::RecordThreadData& subThread = node.subThreads[threadIndex];

               if (TestThreadNode(nodeID, subThread, outNodes, outLinks))
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
