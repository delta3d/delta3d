
/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
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
 * Bradley Anderegg 03/08/2006
*/


#include <dtAI/npcstate.h>
#include <algorithm>

namespace dtAI
{


struct funcExecuteCmds
{
   template <typename _Type>
      void operator()(_Type& pCommand)
   {
      pCommand->operator()();
   }
};


NPCState::NPCState(const Type* pType):
mType(pType)
{
   //visual studio complains if this is in the initializer list Warning:C4355
   mOnUpdate = UpdateFunctor(this, &dtAI::NPCState::DefaultUpdateFunctor);
}

NPCState::NPCState()
: mType(0)
{
   //visual studio complains if this is in the initializer list Warning:C4355
   mOnUpdate = UpdateFunctor(this, &dtAI::NPCState::DefaultUpdateFunctor);
}

NPCState::~NPCState()
{

}

void NPCState::SetType(const Type* stateType)
{
   mType = stateType;
}

const NPCState::Type* NPCState::GetType() const
{
   return mType;
}

const std::string& NPCState::GetName() const
{
   return mType->GetName();
}

///Executes the entry commands
void NPCState::OnEntry()
{
   ExecuteCommands(mOnStart);
}

//Executes the exit commands
void NPCState::OnExit()
{
   ExecuteCommands(mOnFinish);
}

void NPCState::AddEntryCommand(CommandPtr pCommand)
{
   mOnStart.push_back(pCommand);
}

void NPCState::AddExitCommand(CommandPtr pCommand)
{
   mOnFinish.push_back(pCommand);
}

void NPCState::RemoveEntryCommand(CommandPtr pCommand)
{
   mOnStart.remove(pCommand);
}

void NPCState::RemoveExitCommand(CommandPtr pCommand)
{
   mOnFinish.remove(pCommand);
}

void NPCState::SetUpdate(const UpdateFunctor& pUpdate)
{
   mOnUpdate = pUpdate;
}

NPCState::UpdateFunctor& NPCState::GetUpdate()
{
   return mOnUpdate;
}


void NPCState::ExecuteCommands(CommandList& pList)
{
   //execute all the commands
   std::for_each(pList.begin(), pList.end(), funcExecuteCmds());   
}

void NPCState::DefaultUpdateFunctor(double dt)
{

}

//////////////////////////////////////////////////////////////////////////


IMPLEMENT_ENUM(NPCStateTypes);

const NPCStateTypes NPCStateTypes::NPC_STATE_DEFAULT("NPC_STATE_DEFAULT");
const NPCStateTypes NPCStateTypes::NPC_STATE_SPAWN("NPC_STATE_SPAWN");
const NPCStateTypes NPCStateTypes::NPC_STATE_DIE("NPC_STATE_DIE");
const NPCStateTypes NPCStateTypes::NPC_STATE_IDLE("NPC_STATE_IDLE");


NPCStateTypes::NPCStateTypes(const std::string& pName): dtUtil::Enumeration(pName)
{
}

NPCStateTypes::~NPCStateTypes()
{
}




}//namespace dtAI
