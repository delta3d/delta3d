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

#include <dialoglineregistry.h>

#include <dialogdelayline.h>
#include <dialoglogline.h>
#include <dialogswitchline.h>
#include <dialogconditionline.h>
#include <dialogrefline.h>

#include <dtDirector/node.h>


DialogLineRegistry* DialogLineRegistry::mInstance = NULL;

////////////////////////////////////////////////////////////////////////////////
DialogLineRegistry& DialogLineRegistry::GetInstance()
{
   if (!mInstance)
   {
      mInstance = new DialogLineRegistry();
   }
   return *mInstance;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineRegistry::RegisterStartedLineType(DialogLineType* line)
{
   mStartedLineType = line;
}

////////////////////////////////////////////////////////////////////////////////
void DialogLineRegistry::RegisterEndedLineType(DialogLineType* line)
{
   mEndedLineType = line;
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineRegistry::RegisterLineType(DialogLineType* line)
{
   if (!line)
   {
      return false;
   }

   const QString& name = line->GetName();
   if (name.isEmpty())
   {
      return false;
   }

   std::map<QString, dtCore::RefPtr<DialogLineType> >::iterator iter;
   iter = mLineTypes.find(name);

   if (iter != mLineTypes.end())
   {
      return false;
   }

   mLineTypes[name] = line;
   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool DialogLineRegistry::UnRegisterLineType(const QString& name)
{
   if (name.isEmpty())
   {
      return false;
   }

   std::map<QString, dtCore::RefPtr<DialogLineType> >::iterator iter;
   iter = mLineTypes.find(name);

   if (iter == mLineTypes.end())
   {
      return false;
   }

   mLineTypes.erase(iter);
   return true;
}

////////////////////////////////////////////////////////////////////////////////
const DialogLineType* DialogLineRegistry::GetStartedLineType() const
{
   return mStartedLineType;
}

////////////////////////////////////////////////////////////////////////////////
const DialogLineType* DialogLineRegistry::GetEndedLineType() const
{
   return mEndedLineType;
}

////////////////////////////////////////////////////////////////////////////////
std::vector<QString> DialogLineRegistry::GetLineTypes() const
{
   std::vector<QString> typeList;

   std::map<QString, dtCore::RefPtr<DialogLineType> >::const_iterator iter;
   for (iter = mLineTypes.begin(); iter != mLineTypes.end(); ++iter)
   {
      typeList.push_back(iter->first);
   }

   return typeList;
}

////////////////////////////////////////////////////////////////////////////////
const DialogLineType* DialogLineRegistry::GetLineTypeForNode(const dtDirector::Node* node) const
{
   if (!node)
   {
      return NULL;
   }

   std::map<QString, dtCore::RefPtr<DialogLineType> >::const_iterator iter;
   for (iter = mLineTypes.begin(); iter != mLineTypes.end(); ++iter)
   {
      const DialogLineType* lineType = iter->second;
      if (lineType->ShouldOperateOn(node))
      {
         return lineType;
      }
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
const DialogLineType* DialogLineRegistry::GetLineTypeForName(const QString& name) const
{
   if (name.isEmpty())
   {
      return NULL;
   }

   std::map<QString, dtCore::RefPtr<DialogLineType> >::const_iterator iter;
   iter = mLineTypes.find(name);
   if (iter != mLineTypes.end())
   {
      return iter->second;
   }

   return NULL;
}

////////////////////////////////////////////////////////////////////////////////
DialogLineRegistry::DialogLineRegistry()
{
   //RegisterLineType(new DialogDelayLineType());
   RegisterLineType(new DialogLogLineType());
   RegisterLineType(new DialogSwitchLineType());
   RegisterLineType(new DialogConditionLineType());
   RegisterLineType(new DialogRefLineType());
}

////////////////////////////////////////////////////////////////////////////////
DialogLineRegistry::~DialogLineRegistry()
{
}

////////////////////////////////////////////////////////////////////////////////
