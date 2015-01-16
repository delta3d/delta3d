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
#include <prefix/dtdirectornodesprefix.h>
#include <dtDirector/colors.h>
#include <dtDirector/directortypefactory.h>
#include <dtDirectorNodes/referencescriptaction.h>

#include <dtCore/project.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/booleanactorproperty.h>

#include <osgDB/FileNameUtils>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ReferenceScriptAction::ReferenceScriptAction()
      : ActionNode()
      , mUseCache(true)
   {
      AddAuthor("Jeff P. Houde");
      SetColorRGB(Colors::GREEN);

      mScriptResource = dtCore::ResourceDescriptor::NULL_RESOURCE;
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
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::OnStart()
   {
      if (mScript.valid())
      {
         mScript->OnStart();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      mCoreValueIndex = (int)mValues.size();

      dtCore::BooleanActorProperty* cacheProp = new dtCore::BooleanActorProperty(
         "Use Cache", "Use Cache",
         dtCore::BooleanActorProperty::SetFuncType(this, &ReferenceScriptAction::SetUseCache),
         dtCore::BooleanActorProperty::GetFuncType(this, &ReferenceScriptAction::GetUseCache),
         "Whether this script should be saved and loaded from cached memory.");
      AddProperty(cacheProp);

      dtCore::ResourceActorProperty* scriptProp = new dtCore::ResourceActorProperty(
         dtCore::DataType::DIRECTOR, "DirectorGraph", "Director Graph",
         dtCore::ResourceActorProperty::SetDescFuncType(this, &ReferenceScriptAction::SetDirectorResource),
         dtCore::ResourceActorProperty::GetDescFuncType(this, &ReferenceScriptAction::GetDirectorResource),
         "A Director Graph Resource.", "", "DirectorEditor");
      AddProperty(scriptProp);
   }

   //////////////////////////////////////////////////////////////////////////
   bool ReferenceScriptAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::OnLinkValueChanged(const std::string& linkName)
   {
      ActionNode::OnLinkValueChanged(linkName);

      // Propagate the changed event to the value link within the
      // referenced script.
      if (mScript.valid())
      {
         DirectorGraph* graph = mScript->GetGraphRoot();
         if (graph)
         {
            std::vector<dtCore::RefPtr<ValueNode> > values = graph->GetExternalValueNodes();
            int count = (int)values.size();
            for (int index = 0; index < count; index++)
            {
               ValueNode* node = values[index];
               if (node->GetName() == linkName)
               {
                  node->OnLinkValueChanged(linkName);
                  break;
               }
            }
         }

         if (linkName == "Enabled")
         {
            mScript->SetEnabled(GetBoolean("Enabled"));
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::SetEnabled(bool enabled)
   {
      ActionNode::SetEnabled(enabled);

      if (mScript.valid())
      {
         mScript->SetEnabled(enabled);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::SetDirectorResource(const dtCore::ResourceDescriptor& value)
   {
      mScriptResource = value;
      LoadScript();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::ResourceDescriptor ReferenceScriptAction::GetDirectorResource() const
   {
      return mScriptResource;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::SetUseCache(bool value)
   {
      mUseCache = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ReferenceScriptAction::GetUseCache() const
   {
      return mUseCache;
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtDirector::Director* ReferenceScriptAction::GetDirectorScript() const
   {
      return mScript;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::RefreshLinks()
   {
      std::vector<InputLink>  oldInputs = mInputs;
      std::vector<OutputLink> oldOutputs = mOutputs;
      std::vector<ValueLink>  oldValues = mValues;

      mInputs.clear();
      mOutputs.clear();

      if ((int)mValues.size() > mCoreValueIndex)
      {
         mValues.erase(mValues.begin() + mCoreValueIndex, mValues.end());
      }

      if (!mScript)
      {
         return;
      }

      DirectorGraph* graph = mScript->GetGraphRoot();
      if (graph)
      {
         // Set up the links.
         std::vector<dtCore::RefPtr<EventNode> > inputs = graph->GetInputNodes();
         int count = (int)inputs.size();
         for (int index = 0; index < count; index++)
         {
            InputLink* link = &inputs[index]->GetInputLinks()[0];
            link->RedirectLink(NULL);

            bool found = false;
            int testCount = (int)oldInputs.size();
            for (int testIndex = 0; testIndex < testCount; ++testIndex)
            {
               if (oldInputs[testIndex].GetName() == link->GetName())
               {
                  oldInputs[testIndex].RedirectLink(link);
                  mInputs.push_back(oldInputs[testIndex]);
                  found = true;
                  break;
               }
            }

            if (!found)
            {
               InputLink newLink = InputLink(this, link->GetName(), inputs[index]->GetComment());
               newLink.RedirectLink(link);
               mInputs.push_back(newLink);
            }
         }

         std::vector<dtCore::RefPtr<ActionNode> > outputs = graph->GetOutputNodes();
         count = (int)outputs.size();
         mOutputs.reserve(count);
         for (int index = 0; index < count; index++)
         {
            OutputLink* link = &outputs[index]->GetOutputLinks()[0];
            link->RedirectLink(NULL);

            bool found = false;
            int testCount = (int)oldOutputs.size();
            for (int testIndex = 0; testIndex < testCount; ++testIndex)
            {
               if (oldOutputs[testIndex].GetName() == link->GetName())
               {
                  mOutputs.push_back(oldOutputs[testIndex]);
                  link->RedirectLink(&mOutputs.back());
                  found = true;
                  break;
               }
            }

            if (!found)
            {
               OutputLink newLink = OutputLink(this, link->GetName(), outputs[index]->GetComment());
               mOutputs.push_back(newLink);
               link->RedirectLink(&mOutputs.back());
            }
         }

         std::vector<dtCore::RefPtr<ValueNode> > values = graph->GetExternalValueNodes();
         count = (int)values.size();
         mValues.reserve(count + mCoreValueIndex);
         for (int index = 0; index < count; index++)
         {
            ValueLink* link = &values[index]->GetValueLinks()[0];
            link->RedirectLink(NULL);

            bool found = false;
            int testCount = (int)oldValues.size();
            for (int testIndex = 0; testIndex < testCount; ++testIndex)
            {
               if (oldValues[testIndex].GetName() == link->GetName())
               {
                  mValues.push_back(oldValues[testIndex]);
                  link->RedirectLink(&mValues.back());
                  found = true;
                  break;
               }
            }

            if (!found)
            {
               ValueLink newLink = ValueLink(this, NULL, link->IsOutLink(), link->AllowMultiple(), link->IsTypeChecking(), true);
               newLink.SetComment(values[index]->GetComment());
               newLink.SetName(link->GetName());
               mValues.push_back(newLink);
               link->RedirectLink(&mValues.back());
            }
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ReferenceScriptAction::LoadScript()
   {
      mName = "";
      mScript = NULL;

      // Now load the Director Script if able.
      if (mScriptResource != dtCore::ResourceDescriptor::NULL_RESOURCE)
      {
         DirectorTypeFactory* factory = DirectorTypeFactory::GetInstance();
         if (factory)
         {
            try
            {
               mScript = factory->LoadScript(dtCore::Project::GetInstance().GetResourcePath(mScriptResource), GetDirector()->GetGameManager(), GetDirector()->GetMap(), GetBoolean("Use Cache"));

               if (mScript)
               {
                  mScript->SetParent(GetDirector());
                  mScript->SetScriptOwner(GetDirector()->GetScriptOwner());
                  mScript->SetNodeLogging(GetDirector()->GetNodeLogging());
                  mScript->SetResource(mScriptResource);

                  if (GetDirector()->HasStarted())
                  {
                     OnStart();
                  }

                  mName = osgDB::getNameLessExtension(mScriptResource.GetResourceName());
               }
            }
            catch (const dtUtil::Exception& e)
            {
               std::string error = std::string("Unable to parse ") + mScriptResource.GetDisplayName().c_str() + " with error " + e.ToString();
               LOG_ERROR(error.c_str());

               // If the script loaded, null out the resource
               if (mScript)
               {
                  mScript->SetResource(dtCore::ResourceDescriptor::NULL_RESOURCE);
               }

               mName = "<i>Invalid Script!</i>";
            }
         }
      }

      RefreshLinks();
   }
}

////////////////////////////////////////////////////////////////////////////////
