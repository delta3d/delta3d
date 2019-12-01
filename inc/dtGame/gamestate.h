/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology, BMH Operation
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
 * Bradley Anderegg
 */

#ifndef DELTA_GAME_STATE_H
#define DELTA_GAME_STATE_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtUtil/enumeration.h>
#include <dtUtil/command.h>
#include <dtUtil/functor.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <list>
#include <string>
#include <dtGame/export.h>



namespace dtGame
{
   //////////////////////////////////////////////////////////////////////////
   // STATE TYPE CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_GAME_EXPORT StateType : public dtUtil::Enumeration
   {
      DECLARE_ENUM(StateType);

      public:
         typedef dtUtil::Enumeration BaseClass;   
         
         static StateType STATE_UNKNOWN;
         static StateType STATE_SPLASH;
         static StateType STATE_MENU;
         static StateType STATE_LOGIN;
         static StateType STATE_INTRO;
         static StateType STATE_CUTSCENE;
         static StateType STATE_LOADING;
         static StateType STATE_RUNNING;
         static StateType STATE_SHUTDOWN;

         StateType(const std::string &name, bool paused = false)
            : BaseClass(name)
            , mPaused(paused)
         {
            AddInstance(this);         
         }

         bool IsPausedState() const
         {
            return mPaused;
         }

      protected:
         virtual ~StateType(){}

         /// Flag to determine if the state i
         bool mPaused;
   };
   
   typedef StateType GameStateType;



   //////////////////////////////////////////////////////////////////////////
   // EVENT TYPE CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_GAME_EXPORT EventType: public dtUtil::Enumeration
   {
      DECLARE_ENUM(EventType);

      public:
         typedef dtUtil::Functor<void, TYPELIST_1(const EventType*)> HandleEventFunctor;
         typedef dtUtil::Enumeration BaseClass;

         static EventType TRANSITION_OCCURRED; //used for testing
         static EventType TRANSITION_FORWARD;
         static EventType TRANSITION_BACK;
         static EventType TRANSITION_QUIT;
         static EventType TRANSITION_GAME_OVER;

         EventType(const std::string &name): BaseClass(name)
         {
            AddInstance(this);
         }

      protected:
         virtual ~EventType(){}
   };



   //////////////////////////////////////////////////////////////////////////
   // GAME STATE CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_GAME_EXPORT GameState: public osg::Referenced
   {
      public:

         typedef StateType Type;

         typedef std::list<osg::ref_ptr<dtUtil::Command<void> > > CommandList;
         typedef dtUtil::Command<void>* CommandPtr; 
         typedef dtUtil::Functor<void, TYPELIST_1(float)>  UpdateFunctor;

         GameState(const StateType*);

         const StateType* GetType() const;      
         const std::string& GetName() const;

         ///Executes the entry commands
         void OnEntry();

         //Executes the exit commands
         void OnExit();

         void AddEntryCommand(CommandPtr);
         void AddExitCommand(CommandPtr);

         void RemoveEntryCommand(CommandPtr);
         void RemoveExitCommand(CommandPtr);

         void SetUpdate(const UpdateFunctor&);
         UpdateFunctor& GetUpdate();

      protected:
         virtual ~GameState();
         GameState(const GameState&); //not implemented by design
         const GameState& operator=(const GameState&); //not implemented by design

      private:
         void DefaultUpdateFunctor(float);
         void ExecuteCommands(CommandList&);

         const StateType* mState;

         CommandList mOnStart;
         CommandList mOnFinish;
         UpdateFunctor mOnUpdate;

   };

}

#endif
