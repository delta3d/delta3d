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
#include <prefix/dtdirectorqtprefix.h>
#include <dtDirectorQt/editornotifier.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   EditorNotifier::EditorNotifier()
   {
      mTime = dtCore::Timer::Instance()->Tick();
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorNotifier::~EditorNotifier()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::AddEditor(DirectorEditor* editor)
   {
      mEditorList.push_back(editor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::RemoveEditor(DirectorEditor* editor)
   {
      int count = (int)mEditorList.size();
      for (int index = 0; index < count; ++index)
      {
         if (mEditorList[index] == editor)
         {
            mEditorList.erase(mEditorList.begin() + index);
            break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::vector<DirectorEditor*>& EditorNotifier::GetEditors() const
   {
      return mEditorList;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::Update(bool pause, bool step)
   {
      const float GLOW_SPEED = 2.0f;

      float delta = dtCore::Timer::Instance()->DeltaSec(mTime, dtCore::Timer::Instance()->Tick());

      if (delta >= 0.05f || step)
      {
         std::vector<Node*> removeList;

         // Reduces the glow of each node over time.
         std::map<Node*, GlowData>::iterator iter;
         for (iter = mGlowMap.begin(); iter != mGlowMap.end(); ++iter)
         {
            GlowData& data = iter->second;

            bool shouldRemove = true;
            if (data.node.valid())
            {
               if (data.hasBreakPoint)
               {
                  shouldRemove = false;
               }

               if (!pause || step)
               {
                  data.goal = 0.0f;
                  data.inputGoal = 0.0f;

                  for (int index = 0; index < (int)data.outputGoals.size(); ++index)
                  {
                     data.outputGoals[index] = 0.0f;
                  }
               }

               if (data.glow > 0.0f)
               {
                  shouldRemove = false;
                  data.glow -= GLOW_SPEED * delta;

                  if (data.glow <= data.goal)
                  {
                     data.glow = data.goal;
                  }
               }
               else
               {
                  data.glow = 0.0f;
               }

               if (data.inputGlow > 0.0f)
               {
                  shouldRemove = false;
                  data.inputGlow -= GLOW_SPEED * delta;

                  if (data.inputGlow <= data.inputGoal)
                  {
                     data.inputGlow = data.inputGoal;
                  }
               }
               else
               {
                  data.inputGlow = -1.0f;
               }

               for (int index = 0; index < (int)data.outputGlows.size(); ++index)
               {
                  if (data.outputGlows[index] > 0.0f)
                  {
                     shouldRemove = false;
                     data.outputGlows[index] -= GLOW_SPEED * delta;

                     if (data.outputGlows[index] <= data.outputGoals[index])
                     {
                        data.outputGlows[index] = data.outputGoals[index];
                     }
                  }
                  else
                  {
                     data.outputGlows[index] = -1.0f;
                  }
               }

               int editorCount = (int)mEditorList.size();
               for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
               {
                  mEditorList[editorIndex]->RefreshGlow(data.node.get());
               }
            }

            if (shouldRemove)
            {
               removeList.push_back(iter->first);
            }
         }

         while (!removeList.empty())
         {
            iter = mGlowMap.find(removeList.back());
            removeList.pop_back();
            if (iter != mGlowMap.end())
            {
               mGlowMap.erase(iter);
            }
         }

         std::map<Node*, dtCore::ObserverPtr<Node> >::iterator valIter;
         for (valIter = mChangedValueMap.begin(); valIter != mChangedValueMap.end(); ++valIter)
         {
            Node* node = valIter->second.get();
            if (node)
            {
               int editorCount = (int)mEditorList.size();
               for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
               {
                  DirectorEditor* editor = mEditorList[editorIndex];
                  if (editor)
                  {
                     editor->RefreshNodeItem(node);
                  }
               }
            }
         }
         mChangedValueMap.clear();

         mTime = dtCore::Timer::Instance()->Tick();
      }
   }

#define GLOW_START 1.0f
#define GLOW_INC   0.5f
#define GLOW_MAX   1.0f

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::OnNodeExecution(Node* node, const std::string& input, const std::vector<std::string>& outputs)
   {
      if (!node)
      {
         return;
      }

      std::map<Node*, GlowData>::iterator iter = mGlowMap.find(node);
      if (iter == mGlowMap.end())
      {
         GlowData data;
         data.hasBreakPoint = false;
         data.glow = 0.0f;
         data.goal = 0.0f;
         data.input = -1;
         data.inputGlow = 0.0f;
         data.inputGoal = 0.0f;
         data.node = node;
         mGlowMap[node] = data;
      }

      GlowData& data = mGlowMap[node];
      if (data.glow < GLOW_START)
      {
         data.glow = GLOW_START;
      }
      else
      {
         data.glow += GLOW_INC;

         if (data.glow > GLOW_MAX)
         {
            data.glow = GLOW_MAX;
         }
      }

      for (int index = 0; index < (int)node->GetInputLinks().size(); ++index)
      {
         InputLink& link = node->GetInputLinks()[index];
         if (link.GetName() == input)
         {
            data.input = index;
            break;
         }
      }

      if (data.inputGlow < GLOW_START)
      {
         data.inputGlow = GLOW_START;
      }
      else
      {
         data.inputGlow += GLOW_INC;

         if (data.inputGlow > GLOW_MAX)
         {
            data.inputGlow = GLOW_MAX;
         }
      }

      for (int index = 0; index < (int)outputs.size(); ++index)
      {
         const std::string& name = outputs[index];

         for (int subIndex = 0; subIndex < (int)node->GetOutputLinks().size(); ++subIndex)
         {
            OutputLink& link = node->GetOutputLinks()[subIndex];
            if (link.GetName() == name)
            {
               while (subIndex >= (int)data.outputGlows.size())
               {
                  data.outputGlows.push_back(-1.0f);
                  data.outputGoals.push_back(0.0f);
               }

               if (data.outputGlows[subIndex] < GLOW_START)
               {
                  data.outputGlows[subIndex] = GLOW_START;
               }
               else
               {
                  data.outputGlows[subIndex] += GLOW_INC;
                  if (data.outputGlows[subIndex] > GLOW_MAX)
                  {
                     data.outputGlows[subIndex] = GLOW_MAX;
                  }
               }
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::OnValueChanged(Node* node)
   {
      std::map<Node*, dtCore::ObserverPtr<Node> >::iterator iter = mChangedValueMap.find(node);
      if (iter == mChangedValueMap.end())
      {
         mChangedValueMap[node] = node;
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorNotifier::OnBeginDebugging()
   {
      int editorCount = (int)mEditorList.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         DirectorEditor* editor = mEditorList[editorIndex];
         if (editor)
         {
            editor->OnBeginDebug();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorNotifier::OnEndDebugging()
   {
      int editorCount = (int)mEditorList.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         DirectorEditor* editor = mEditorList[editorIndex];
         if (editor)
         {
            editor->OnEndDebug();
         }
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void EditorNotifier::OnStepDebugging()
   {
      int editorCount = (int)mEditorList.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         DirectorEditor* editor = mEditorList[editorIndex];
         if (editor)
         {
            editor->OnStepDebug();
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool EditorNotifier::ShouldBreak(Node* node)
   {
      GlowData* data = GetGlowData(node);
      if (data)
      {
         return data->hasBreakPoint;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::BreakNode(Node* node, bool shouldFocus)
   {
      if (!node)
      {
         return;
      }

      std::map<Node*, GlowData>::iterator iter = mGlowMap.find(node);
      if (iter == mGlowMap.end())
      {
         GlowData data;
         data.hasBreakPoint = false;
         data.glow = 0.0f;
         data.goal = 0.0f;
         data.input = -1;
         data.inputGlow = 0.0f;
         data.inputGoal = 0.0f;
         data.node = node;
         mGlowMap[node] = data;
      }

      GlowData& data = mGlowMap[node];
      data.goal = GLOW_START;
      data.inputGoal = GLOW_START;

      for (int index = 0; index < (int)data.outputGlows.size(); ++index)
      {
         if (data.outputGlows[index] >= GLOW_START)
         {
            data.outputGoals[index] = GLOW_START;
         }
         else
         {
            data.outputGoals[index] = 0.0f;
         }
      }

      int editorCount = (int)mEditorList.size();
      for (int editorIndex = 0; editorIndex < editorCount; ++editorIndex)
      {
         DirectorEditor* editor = mEditorList[editorIndex];
         if (editor)
         {
            if (shouldFocus)
            {
               editor->FocusNode(node);
               editor->RefreshButtonStates();
            }

            editor->RefreshGlow(node);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::ToggleBreakPoint(Node* node)
   {
      if (!node)
      {
         return;
      }

      std::map<Node*, GlowData>::iterator iter = mGlowMap.find(node);
      if (iter == mGlowMap.end())
      {
         GlowData data;
         data.hasBreakPoint = true;
         data.glow = 0.0f;
         data.goal = 0.0f;
         data.input = -1;
         data.inputGlow = 0.0f;
         data.inputGoal = 0.0f;
         data.node = node;
         mGlowMap[node] = data;
         return;
      }

      GlowData& data = mGlowMap[node];
      data.hasBreakPoint = !data.hasBreakPoint;
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorNotifier::GlowData* EditorNotifier::GetGlowData(Node* node)
   {
      std::map<Node*, GlowData>::iterator iter = mGlowMap.find(node);
      if (iter != mGlowMap.end())
      {
         return &iter->second;
      }

      return NULL;
   }
}
