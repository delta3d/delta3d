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
#include <dtDirectorNodes/stringmanipulatoraction.h>

#include <dtDirector/director.h>

#include <dtDAL/stringactorproperty.h>
#include <dtDAL/intactorproperty.h>

#include <dtUtil/stringutils.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   StringManipulatorAction::StringManipulatorAction()
      : ActionNode()
      , mIndex(0)
      , mResultProp(NULL)
   {
      AddAuthor("Jeff P. Houde");
   }

   ////////////////////////////////////////////////////////////////////////////////
   StringManipulatorAction::~StringManipulatorAction()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringManipulatorAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);

      // Create multiple inputs for different operations.
      mInputs.clear();
      mInputs.push_back(InputLink(this, "Insert", "Insert text B within a character index of text A."));
      mInputs.push_back(InputLink(this, "Append", "Appends text B to the end of text A."));
      mInputs.push_back(InputLink(this, "Remove", "Removes all instances of text B from text A."));
      mInputs.push_back(InputLink(this, "Replace", "Replaces all instances of text B from text A and replaces it with a replacement text."));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringManipulatorAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::IntActorProperty* indexProp = new dtDAL::IntActorProperty(
         "Insert Index", "Insert Index",
         dtDAL::IntActorProperty::SetFuncType(this, &StringManipulatorAction::SetIndex),
         dtDAL::IntActorProperty::GetFuncType(this, &StringManipulatorAction::GetIndex),
         "The character index to insert to.");
      AddProperty(indexProp);

      dtDAL::StringActorProperty* leftProp = new dtDAL::StringActorProperty(
         "A", "A",
         dtDAL::StringActorProperty::SetFuncType(this, &StringManipulatorAction::SetA),
         dtDAL::StringActorProperty::GetFuncType(this, &StringManipulatorAction::GetA),
         "The Left value.");
      AddProperty(leftProp);

      dtDAL::StringActorProperty* rightProp = new dtDAL::StringActorProperty(
         "B", "B",
         dtDAL::StringActorProperty::SetFuncType(this, &StringManipulatorAction::SetB),
         dtDAL::StringActorProperty::GetFuncType(this, &StringManipulatorAction::GetB),
         "The Right value.");
      AddProperty(rightProp);

      dtDAL::StringActorProperty* replaceProp = new dtDAL::StringActorProperty(
         "Replace With", "Replace With",
         dtDAL::StringActorProperty::SetFuncType(this, &StringManipulatorAction::SetReplaceWith),
         dtDAL::StringActorProperty::GetFuncType(this, &StringManipulatorAction::GetReplaceWith),
         "The Right value.");
      AddProperty(replaceProp);

      mResultProp = new dtDAL::StringActorProperty(
         "Result", "Result",
         dtDAL::StringActorProperty::SetFuncType(this, &StringManipulatorAction::SetResult),
         dtDAL::StringActorProperty::GetFuncType(this, &StringManipulatorAction::GetResult),
         "The resulting output value.");

      // This will expose the properties in the editor and allow
      // them to be connected to ValueNodes.
      mValues.push_back(ValueLink(this, indexProp, false, false, true, false));
      mValues.push_back(ValueLink(this, leftProp, false, false, false));
      mValues.push_back(ValueLink(this, rightProp, false, false, false));
      mValues.push_back(ValueLink(this, replaceProp, false, false, false, false));
      mValues.push_back(ValueLink(this, mResultProp.get(), true, true, true));
   }

   //////////////////////////////////////////////////////////////////////////
   bool StringManipulatorAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      std::string left = GetString("A");
      std::string right = GetString("B");
      std::string result;
      std::string replaceWith;

      switch (input)
      {
      case INPUT_INSERT:
         {
            int index = GetInt("Insert Index");
            if (index >= 0 && index < (int)left.length())
            {
               result = left;
               result.insert(index, right);
               break;
            }
         }

      case INPUT_APPEND:
         {
            result = left + right;
         }
         break;

      case INPUT_REPLACE:
         {
            replaceWith = GetString("Replace With");
         }

      case INPUT_REMOVE:
         {
            result = left;
            dtUtil::FindAndReplace(result, right, replaceWith);
         }
         break;
      }

      SetString(result, "Result");

      return ActionNode::Update(simDelta, delta, input, firstUpdate);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringManipulatorAction::SetIndex(int value)
   {
      mIndex = value;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int StringManipulatorAction::GetIndex() const
   {
      return mIndex;
   }

   //////////////////////////////////////////////////////////////////////////
   void StringManipulatorAction::SetA(const std::string& value)
   {
      mValueA = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string StringManipulatorAction::GetA() const
   {
      return mValueA;
   }

   //////////////////////////////////////////////////////////////////////////
   void StringManipulatorAction::SetB(const std::string& value)
   {
      mValueB = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string StringManipulatorAction::GetB() const
   {
      return mValueB;
   }

   //////////////////////////////////////////////////////////////////////////
   void StringManipulatorAction::SetReplaceWith(const std::string& value)
   {
      mReplaceWith = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string StringManipulatorAction::GetReplaceWith() const
   {
      return mReplaceWith;
   }

   //////////////////////////////////////////////////////////////////////////
   void StringManipulatorAction::SetResult(const std::string& value)
   {
      mResult = value;
   }

   //////////////////////////////////////////////////////////////////////////
   std::string StringManipulatorAction::GetResult() const
   {
      return mResult;
   }
}

////////////////////////////////////////////////////////////////////////////////
