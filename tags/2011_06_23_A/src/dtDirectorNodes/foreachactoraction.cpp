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
#include <dtDirectorNodes/foreachactoraction.h>

#include <dtDAL/stringselectoractorproperty.h>
#include <dtDAL/booleanactorproperty.h>
#include <dtDAL/librarymanager.h>

#include <dtDirector/director.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   ForEachActorAction::ForEachActorAction()
      : ActionNode()
      , mClassType("<None>")
      , mCurrentIndex(-1)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   ForEachActorAction::~ForEachActorAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      mOutputs.clear();
      mOutputs.push_back(OutputLink(this, "Each Actor", "Activates once for each actor found."));
      mOutputs.push_back(OutputLink(this, "Finished", "Activates after all actors have been iterated through."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringSelectorActorProperty* classProp = new dtDAL::StringSelectorActorProperty(
         "Actor Type", "Actor Type",
         dtDAL::StringSelectorActorProperty::SetFuncType(this, &ForEachActorAction::SetClassType),
         dtDAL::StringSelectorActorProperty::GetFuncType(this, &ForEachActorAction::GetClassType),
         dtDAL::StringSelectorActorProperty::GetListFuncType(this, &ForEachActorAction::GetClassTypeList),
         "The type of actor to iterate through (optional).", "", false);
      AddProperty(classProp);

      dtDAL::StringActorProperty* filterProp = new dtDAL::StringActorProperty(
         "Name Filter", "Name Filter",
         dtDAL::StringActorProperty::SetFuncType(this, &ForEachActorAction::SetNameFilter),
         dtDAL::StringActorProperty::GetFuncType(this, &ForEachActorAction::GetNameFilter),
         "The name to filter actors with (optional).");
      AddProperty(filterProp);

      mActorProp = new dtDAL::ActorIDActorProperty(
         "Current Actor", "Current Actor",
         dtDAL::ActorIDActorProperty::SetFuncType(this, &ForEachActorAction::SetCurrentActor),
         dtDAL::ActorIDActorProperty::GetFuncType(this, &ForEachActorAction::GetCurrentActor),
         "", "The currently iterated actor.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, classProp, false, false, true, false));
      mValues.push_back(ValueLink(this, filterProp, false, false, true, false));
      mValues.push_back(ValueLink(this, mActorProp, true, true, true, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool ForEachActorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      // On the first update, find all the actors we want to iterate through.
      if (input == 0)
      {
         if (firstUpdate)
         {
            mFoundActors.clear();
            mCurrentIndex = 0;

            std::string classType = GetString("Actor Type");
            std::string nameFilter = GetString("Name Filter");

            int count = dtDAL::BaseActorObject::GetInstanceCount();
            for (int index = 0; index < count; ++index)
            {
               dtDAL::BaseActorObject* object = dtDAL::BaseActorObject::GetInstance(index);
               if (object)
               {
                  if (!classType.empty() && classType != "<None>" && object->GetActorType().GetFullName() != classType)
                  {
                     continue;
                  }

                  if (!nameFilter.empty())
                  {
                     for (std::string::iterator iter = nameFilter.begin();
                        iter != nameFilter.end(); ++iter)
                     {
                        *iter = tolower(*iter);
                     }

                     std::string name = object->GetName();
                     for (std::string::iterator iter = name.begin();
                        iter != name.end(); ++iter)
                     {
                        *iter = tolower(*iter);
                     }

                     size_t found = name.find(nameFilter.c_str());
                     if (found == std::string::npos)
                     {
                        continue;
                     }
                  }

                  dtDAL::ActorProperty* prototypeProp = object->GetProperty("Initial Ownership");
                  dtDAL::ActorProperty* ghostProp = object->GetProperty("Is Ghost");
                  if ((!prototypeProp || prototypeProp->ToString() != "PROTOTYPE") &&
                     (!ghostProp || ghostProp->ToString() == "false"))
                  {
                     mFoundActors.push_back(object->GetId());
                  }
               }
            }
         }

         // Iterate through each found actor.
         if (mCurrentIndex < (int)mFoundActors.size())
         {
            dtCore::UniqueId currentActor = mFoundActors[mCurrentIndex];

            SetActorID(currentActor, "Current Actor");

            // We push a stack so that we can execute the entire "For Each"
            // output chain before we return back to this node.
            GetDirector()->PushStack(this, 10);

            mCurrentIndex++;
            return true;
         }

         ActivateOutput("Finished");
         return false;
      }
      // If the input is 10, it means we are iterating through an actor.
      else if (input == 10)
      {
         ActivateOutput("Each Actor");
         return false;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::UpdateName()
   {
      std::string classType = GetString("Actor Type");
      std::string nameFilter = GetString("Name Filter");

      mName.clear();

      if (!classType.empty())
      {
         mName += classType;
      }

      if (!nameFilter.empty())
      {
         if (!mName.empty())
         {
            mName += " ";
         }

         mName += "(" + nameFilter + ")";
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::SetClassType(const std::string& value)
   {
      mClassType = value;
      UpdateName();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ForEachActorAction::GetClassType() const
   {
      return mClassType;
   }

   //////////////////////////////////////////////////////////////////////////
   std::vector<std::string> ForEachActorAction::GetClassTypeList()
   {
      std::map<std::string, std::string> listMap;

      std::vector<const dtDAL::ActorType*> types;
      dtDAL::LibraryManager::GetInstance().GetActorTypes(types);
      int count = (int)types.size();
      for (int index = 0; index < count; ++index)
      {
         const dtDAL::ActorType* type = types[index];
         if (type)
         {
            listMap[type->GetFullName()] = "x";
         }
      }

      std::vector<std::string> list;
      list.push_back("<None>");
      std::map<std::string, std::string>::iterator iter;
      for (iter = listMap.begin(); iter != listMap.end(); ++iter)
      {
         list.push_back(iter->first);
      }

      return list;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::SetNameFilter(const std::string& value)
   {
      mNameFilter = value;
      UpdateName();
   }

   //////////////////////////////////////////////////////////////////////////
   const std::string& ForEachActorAction::GetNameFilter() const
   {
      return mNameFilter;
   }

   //////////////////////////////////////////////////////////////////////////
   void ForEachActorAction::SetCurrentActor(const dtCore::UniqueId& value)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::UniqueId ForEachActorAction::GetCurrentActor() const
   {
      dtCore::UniqueId id;
      id = "";
      return id;
   }
}

////////////////////////////////////////////////////////////////////////////////
