/* -*-c++-*-
* testGameActorLibrary - This source file (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
* 
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Matthew W. Campbell
*/
#include "countertaskactor.h"

#include <dtCore/intactorproperty.h>

#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>
#include <dtGame/messagefactory.h>
#include <dtGame/messagetype.h>

//////////////////////////////////////////////////////////////////////////////
CounterTaskActor::CounterTaskActor(dtGame::GameActorProxy& parent) : dtActors::TaskActor(parent)
{
}
 
//////////////////////////////////////////////////////////////////////////////
CounterTaskActor::~CounterTaskActor()
{
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
CounterTaskActorProxy::CounterTaskActorProxy()
{
   mCounter = -1000000;
   mTarget = 1000000;
}

//////////////////////////////////////////////////////////////////////////////
CounterTaskActorProxy::~CounterTaskActorProxy()
{
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::BuildPropertyMap()
{
   dtActors::TaskActorProxy::BuildPropertyMap();
   
   AddProperty(new dtCore::IntActorProperty("Counter","Counter",
            dtCore::IntActorProperty::SetFuncType(this,&CounterTaskActorProxy::SetCounter),
            dtCore::IntActorProperty::GetFuncType(this,&CounterTaskActorProxy::GetCounter)));
      
   AddProperty(new dtCore::IntActorProperty("TargetCount","TargetCount",
            dtCore::IntActorProperty::SetFuncType(this,&CounterTaskActorProxy::SetTargetCount),
            dtCore::IntActorProperty::GetFuncType(this,&CounterTaskActorProxy::GetTargetCount)));
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::BuildInvokables()
{
   dtActors::TaskActorProxy::BuildInvokables();
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::SetCounter(int value)
{
   mCounter = value;
   if (mCounter >= mTarget)
   {
      dtActors::TaskActor *task = dynamic_cast<dtActors::TaskActor *>(GetDrawable());
      task->SetComplete(true);
   }
   
   if (!IsRemote() && IsInGM())
   {
      dtCore::RefPtr<dtGame::Message> updateMsg = 
         GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message);
      
      GetGameManager()->SendMessage(*updateMsg);
   }
}

//////////////////////////////////////////////////////////////////////////////
void CounterTaskActorProxy::SetTargetCount(int value)
{
   mTarget = value;
    
   if (!IsRemote() && IsInGM())
   {
      dtCore::RefPtr<dtGame::Message> updateMsg = 
         GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message);
      
      GetGameManager()->SendMessage(*updateMsg);
   }
}
