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

#include <dtDirector/colors.h>
#include <dtDirectorNodes/referencescriptaction.h>

#include <dtDAL/project.h>
#include <dtDAL/resourceactorproperty.h>

#include <osgDB/FileNameUtils>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ReferenceScriptAction::ReferenceScriptAction()
      : ActionNode()
   {
      AddAuthor("Jeff P. Houde");
      SetColorRGB(Colors::GREEN); 

      mScriptResource = dtDAL::ResourceDescriptor::NULL_RESOURCE;
      mCoreValueIndex = 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ReferenceScriptAction::~ReferenceScriptAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mOutputs.clear();

      mLabel = ActionNode::GetName();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      mCoreValueIndex = (int)mValues.size();

      // Create our value links.
      dtDAL::ResourceActorProperty* scriptProp = new dtDAL::ResourceActorProperty(
         dtDAL::DataType::DIRECTOR, "DirectorGraph", "Director Graph",
         dtDAL::ResourceActorProperty::SetDescFuncType(this, &ReferenceScriptAction::SetDirectorResource),
         dtDAL::ResourceActorProperty::GetDescFuncType(this, &ReferenceScriptAction::GetDirectorResource),
         "A Director Graph Resource.");
      AddProperty(scriptProp);
   }

   //////////////////////////////////////////////////////////////////////////
   bool ReferenceScriptAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      return false;
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ReferenceScriptAction::GetName()
   {
      return mLabel;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::SetDirectorResource(const dtDAL::ResourceDescriptor& value)
   {
      mScriptResource = value;
      UpdateLinks();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDAL::ResourceDescriptor ReferenceScriptAction::GetDirectorResource()
   {
      return mScriptResource;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDirector::Director* ReferenceScriptAction::GetDirectorScript()
   {
      return mScript;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::UpdateLinks()
   {
      // Clear all links.
      mInputs.clear();
      mOutputs.clear();
      
      if ((int)mValues.size() > mCoreValueIndex)
      {
         mValues.erase(mValues.begin() + mCoreValueIndex, mValues.end());
      }

      // Now load the Director Script if able.
      if (mScriptResource != dtDAL::ResourceDescriptor::NULL_RESOURCE)
      {
         if (!mScript.valid())
         {
            mScript = new Director();
            mScript->Init(GetDirector()->GetGameManager(), GetDirector()->GetMap());
            mScript->SetPlayer(GetDirector()->GetPlayer());
            //mScript->SetCamera(GetDirector()->GetCamera());
            mScript->SetNodeLogging(GetDirector()->GetNodeLogging());
            // TODO: Record.

            // Make sure all new threads and stacks are created on the parent director.
            mScript->SetParent(GetDirector());
         }

         if (mScript.valid())
         {
            // If we successfully load the script, create our links for this node.
            mScript->LoadScript(dtDAL::Project::GetInstance().GetResourcePath(mScriptResource));

            DirectorGraph* graph = mScript->GetGraphRoot();
            if (graph)
            {
               mLabel = osgDB::getNameLessExtension(mScriptResource.GetResourceName());

               // Set up the links.
               std::vector<dtCore::RefPtr<EventNode> > inputs = graph->GetInputNodes();
               int count = (int)inputs.size();
               for (int index = 0; index < count; index++)
               {
                  if (inputs[index]->IsEnabled())
                  {
                     InputLink* link = &inputs[index]->GetInputLinks()[0];

                     InputLink newLink = InputLink(this, link->GetName());
                     newLink.RedirectLink(link);
                     mInputs.push_back(newLink);
                  }
               }

               std::vector<dtCore::RefPtr<ActionNode> > outputs = graph->GetOutputNodes();
               count = (int)outputs.size();
               mOutputs.reserve(count);
               for (int index = 0; index < count; index++)
               {
                  if (outputs[index]->IsEnabled())
                  {
                     OutputLink* link = &outputs[index]->GetOutputLinks()[0];

                     OutputLink newLink = OutputLink(this, link->GetName());
                     mOutputs.push_back(newLink);
                     link->RedirectLink(&mOutputs.back());
                  }
               }

               std::vector<dtCore::RefPtr<ValueNode> > values = graph->GetExternalValueNodes();
               count = (int)values.size();
               mValues.reserve(count + mCoreValueIndex);
               for (int index = 0; index < count; index++)
               {
                  if (values[index]->IsEnabled())
                  {
                     ValueLink* link = &values[index]->GetValueLinks()[0];

                     ValueLink newLink = ValueLink(this, NULL, link->IsOutLink(), link->AllowMultiple(), link->IsTypeChecking(), true);
                     newLink.SetName(link->GetName());
                     mValues.push_back(newLink);
                     link->RedirectLink(&mValues.back());
                  }
               }
            }
         }
      }
      else
      {
         mScript = NULL;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
