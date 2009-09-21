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

#ifndef __DELTA__NPCSTATE_H__
#define __DELTA__NPCSTATE_H__

#include <dtAI/export.h>
#include <dtUtil/enumeration.h>
#include <dtUtil/command.h>
#include <dtUtil/functor.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <list>
#include <string>

namespace dtAI
{

   class DT_AI_EXPORT NPCState: public osg::Referenced
   {
   public:
      typedef dtUtil::Enumeration Type;
      typedef std::list<osg::ref_ptr<dtUtil::Command<void> > > CommandList;
      typedef dtUtil::Command<void>* CommandPtr;
      typedef dtUtil::Functor<void, TYPELIST_1(double)>  UpdateFunctor;

   public:
      NPCState();
      NPCState(const Type* pType);

      // the object factory doesn't allow creation with anything but
      // a default constructor so we need a set type
      void SetType(const Type* stateType);

      const Type* GetType() const;
      const std::string& GetName() const;

      /// Executes the entry commands
      void OnEntry();

      // Executes the exit commands
      void OnExit();

      void AddEntryCommand(CommandPtr pCommand);
      void AddExitCommand(CommandPtr pCommand);

      void RemoveEntryCommand(CommandPtr pCommand);
      void RemoveExitCommand(CommandPtr pCommand);

      void SetUpdate(const UpdateFunctor& pUpdate);
      UpdateFunctor& GetUpdate();

   protected:
      ~NPCState();
      NPCState(const NPCState&); //not implemented by design
      const NPCState& operator=(const NPCState&); //not implemented by design

   private:
      void DefaultUpdateFunctor(double dt);
      void ExecuteCommands(CommandList& pList);

      const Type* mType;

      CommandList mOnStart;
      CommandList mOnFinish;
      UpdateFunctor mOnUpdate;
   };


   class DT_AI_EXPORT NPCStateTypes: public NPCState::Type
   {
      DECLARE_ENUM(NPCStateTypes);

   public:
      NPCStateTypes(const std::string& stateName);

      static const NPCStateTypes   NPC_STATE_DEFAULT;
      static const NPCStateTypes   NPC_STATE_SPAWN;
      static const NPCStateTypes   NPC_STATE_DIE;
      static const NPCStateTypes   NPC_STATE_IDLE;

   protected:
      ~NPCStateTypes();

   private:
      NPCStateTypes(); //not implemented by design
   };

} // namespace dtAI

#endif // __DELTA__NPCSTATE_H__
