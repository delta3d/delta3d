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

#include <dtDirectorQt/editornotifier.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   EditorNotifier::EditorNotifier(DirectorEditor* editor)
      : mEditor(editor)
   {
      mTime = dtCore::Timer::Instance()->Tick();
   }

   ////////////////////////////////////////////////////////////////////////////////
   EditorNotifier::~EditorNotifier()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EditorNotifier::Update(bool unpause)
   {
      const float GLOW_SPEED = 1.0f;

      float delta = dtCore::Timer::Instance()->DeltaSec(mTime, dtCore::Timer::Instance()->Tick());

      if (delta >= 0.05f)
      {
         std::vector<Node*> removeList;
         // Reduces the glow of each node over time.
         std::map<Node*, GlowData>::iterator iter;
         for (iter = mGlowMap.begin(); iter != mGlowMap.end(); ++iter)
         {
            GlowData& data = iter->second;

            // Only update the glow on nodes that are not paused, or we
            // should unpause.
            if (data.isPaused && !unpause)
            {
               continue;
            }

            data.isPaused = false;
            data.glow -= GLOW_SPEED * delta;

            if (data.glow <= 0.0f)
            {
               data.glow = 0.0f;

               if (!data.hasBreakPoint)
               {
                  removeList.push_back(iter->first);
               }
            }

            for (int index = 0; index < (int)data.outputGlows.size(); ++index)
            {
               data.outputGlows[index] -= GLOW_SPEED * delta;

               if (data.outputGlows[index] <= 0.0f)
               {
                  data.outputGlows[index] = 0.0f;
               }
            }

            mEditor->RefreshNode(iter->first);
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

         mTime = dtCore::Timer::Instance()->Tick();
      }
   }

#define GLOW_MIN 0.50f
#define GLOW_INC 0.10f
#define GLOW_MAX 0.50f

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
         data.isPaused = false;
         data.glow = 0.0f;
         data.nodeID = node->GetID();
         mGlowMap[node] = data;
      }

      GlowData& data = mGlowMap[node];
      if (data.glow < GLOW_MIN)
      {
         data.glow = GLOW_MIN;
      }
      else
      {
         data.glow += GLOW_INC;

         if (data.glow > GLOW_MAX)
         {
            data.glow = GLOW_MAX;
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
                  data.outputGlows.push_back(0.0f);
               }

               if (data.outputGlows[subIndex] < GLOW_MIN)
               {
                  data.outputGlows[subIndex] = GLOW_MIN;
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
      //if (!node)
      //{
      //   return;
      //}

      //std::map<Node*, GlowData>::iterator iter = mGlowMap.find(node);
      //if (iter == mGlowMap.end())
      //{
      //   GlowData data;
      //   data.hasBreakPoint = false;
      //   data.isPaused = false;
      //   data.glow = 0.0f;
      //   data.nodeID = node->GetID();
      //   mGlowMap[node] = data;
      //}

      //GlowData& data = mGlowMap[node];
      //if (data.glow < GLOW_MIN)
      //{
      //   data.glow = GLOW_MIN;
      //}
      //else
      //{
      //   data.glow += GLOW_INC;

      //   if (data.glow > GLOW_MAX)
      //   {
      //      data.glow = GLOW_MAX;
      //   }
      //}
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
         data.isPaused = false;
         data.glow = GLOW_MIN;
         data.nodeID = node->GetID();
         mGlowMap[node] = data;
      }

      GlowData& data = mGlowMap[node];
      data.isPaused = true;

      if (shouldFocus)
      {
         mEditor->FocusNode(node);
         mEditor->RefreshButtonStates();
      }

      mEditor->RefreshNode(node);
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
         data.isPaused = false;
         data.glow = 0.0f;
         data.nodeID = node->GetID();
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
