/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009 MOVES Institute
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
 * Author: MG
 */

#include <prefix/dtdirectornodesprefix.h>
#include <dtDirectorNodes/stringcomposeraction.h>

//#include <dtDAL/intactorproperty.h>
//#include <dtDAL/arrayactorproperty.h>
//#include <dtDAL/enumactorproperty.h>
//#include <dtDirector/director.h>
//#include <dtDirectorNodes/stringarrayvalue.h>
//#include <dtGame/gamemanager.h>
//#include <dtUtil/stringutils.h>

#include <assert.h>

namespace dtDirector
{
   static const std::string ARGUMENT_SEQUENCE("%%");

   // There are hidden value nodes in the first two slots,
   // manually accounting for it here for now
   static const int DEFAULT_VALUE_LINK_COUNT = 2;

   /////////////////////////////////////////////////////////////////////////////
   StringComposerAction::StringComposerAction()
      : ActionNode()
   {
      AddAuthor("MG");
   }

   /////////////////////////////////////////////////////////////////////////////
   StringComposerAction::~StringComposerAction()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   const std::string& StringComposerAction::GetName()
   {
      return mText;
   }

   /////////////////////////////////////////////////////////////////////////////
   void StringComposerAction::Init(const NodeType& nodeType, DirectorGraph* graph)
   {
      ActionNode::Init(nodeType, graph);
   }

   /////////////////////////////////////////////////////////////////////////////
   void StringComposerAction::BuildPropertyMap()
   {
      ActionNode::BuildPropertyMap();

      dtDAL::StringActorProperty* textProp = new dtDAL::StringActorProperty("Text", "Text",
         dtDAL::StringActorProperty::SetFuncType(this, &StringComposerAction::SetText),
         dtDAL::StringActorProperty::GetFuncType(this, &StringComposerAction::GetText),
         "The text that is used to compose the final string");
      AddProperty(textProp);

      mValues.push_back(ValueLink(this, textProp, false, false, true, false));

      // Set the default text as an example of how to use this node
      SetText("Default text with 2 args in parenthesis (%%, %%)");
   }

   /////////////////////////////////////////////////////////////////////////////
   bool StringComposerAction::Update(float simDelta, float delta, int input, bool firstUpdate)
   {
      if (firstUpdate)
      {
         SetString("Text", GetComposedString());

         OutputLink* link = GetOutputLink("Out");
         if (link)
         {
            link->Activate();
         }
      }

      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   void StringComposerAction::SetText(const std::string& newText)
   {
      int numberOfArguments = GetNumberOfArgumentSequences(newText);

      // If we have too many, remove some
      while (mValues.size() > (size_t)numberOfArguments + DEFAULT_VALUE_LINK_COUNT)
      {
         mValues.pop_back();
      }

      // If we don't have enough, add some
      while (mValues.size() < (size_t)numberOfArguments + DEFAULT_VALUE_LINK_COUNT)
      {
         // Start counting with 1 like a non programmer
         int argNumber = mValues.size() - DEFAULT_VALUE_LINK_COUNT + 1;

         std::string propertyName = std::string("Arg") + dtUtil::ToString(argNumber);
         dtDAL::StringActorProperty* argProperty = new dtDAL::StringActorProperty(propertyName, propertyName,
            dtDAL::StringActorProperty::SetFuncType(this, &StringComposerAction::SetArgument),
            dtDAL::StringActorProperty::GetFuncType(this, &StringComposerAction::GetArgument),
            "Sets an argument used to compose the text.");

         mValues.push_back(ValueLink(this, argProperty, false, false, false));
      }

      mText = newText;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string StringComposerAction::GetText() const
   {
      return mText;
   }

   /////////////////////////////////////////////////////////////////////////////
   int StringComposerAction::GetNumberOfArgumentSequences(const std::string& sourceText)
   {
      int numberOfArguments = 0;

      size_t position = 0;
      while (1)
      {
         position = sourceText.find(ARGUMENT_SEQUENCE, position);

         if (position != std::string::npos)
         {
            ++numberOfArguments;
            position += ARGUMENT_SEQUENCE.length();
         }
         else
         {
            break;
         }
      }

      return numberOfArguments;
   }

   /////////////////////////////////////////////////////////////////////////////
   std::string StringComposerAction::GetComposedString()
   {
      std::string composedString;

      size_t startPosition = 0;
      size_t argumentIndex = DEFAULT_VALUE_LINK_COUNT;

      mText = GetString("Text");

      // Make sure any \n become proper newline characters
      dtUtil::FindAndReplace(mText, "\\n", "\n");

      while (1)
      {
         // Find the position of the next argument sequence %%
         size_t delimPosition = mText.find(ARGUMENT_SEQUENCE, startPosition);

         if (delimPosition != std::string::npos)
         {
            std::string::const_iterator startIter = mText.begin() + startPosition;
            std::string::const_iterator endIter = mText.begin() + delimPosition;

            if (startIter != endIter)
            {
               composedString.append(std::string(startIter, endIter));
               startPosition = delimPosition + ARGUMENT_SEQUENCE.size();
            }

            // If a value exists for this argument, insert it
            if (mValues.size() > argumentIndex)
            {
               composedString.append(mValues[argumentIndex++].GetProperty()->GetValueString());
            }

            // Increment to the position past the delimiter
            startPosition = delimPosition + ARGUMENT_SEQUENCE.length();
         }
         else
         {
            // Grab text remaining after the last delimiter
            if (startPosition < mText.length())
            {
               composedString.append(std::string(mText.begin() + startPosition, mText.end()));
            }
            break;
         }
      }

      return composedString;
   }
}

////////////////////////////////////////////////////////////////////////////////
