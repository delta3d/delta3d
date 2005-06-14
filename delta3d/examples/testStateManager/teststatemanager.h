#ifndef DELTA_TESTSTATEMANAGER
#define DELTA_TESTSTATEMANAGER

#include "dtCore/base.h"  // for base class
#include "dtABC/statemanager.h"

class MyEventType : public dtABC::Event::Type
{
   DECLARE_ENUM(MyEventType); 
public:
   static const MyEventType ALT;
   static const MyEventType START;
private:

   MyEventType( const std::string& name ) : dtABC::State::Type(name)
   {
      AddInstance(this); 
   }	
};

class MyStateType : public dtABC::State::Type
{

   DECLARE_ENUM(MyStateType); 

public:

   static const MyStateType SHELL;
   static const MyStateType OPTIONS;
   static const MyStateType GAME;

private:

   MyStateType( const std::string& name ) : dtABC::State::Type(name)
   {
      AddInstance(this); 
   }	

};

class Shell : public dtABC::State
{ 
public:
   Shell( std::string name = "Shell" );
   virtual void Enable( dtABC::Event* data = 0 ) {}
};

class Options : public dtABC::State
{
public:
   Options( std::string name = "Options" );
   virtual void Enable( dtABC::Event* data = 0 ) {}
};

class Game : public dtABC::State
{
public:
   Game( std::string name = "Game" );
   virtual void Enable( dtABC::Event* data = 0 ) {}
};

class Alt : public dtABC::Event
{
public:
   Alt();
};

class Start : public dtABC::Event
{
public:
   Start();
};

class TestStateManager : public dtCore::Base
{
   DECLARE_MANAGEMENT_LAYER(TestStateManager)
public:
   typedef dtABC::StateManager<MyEventType,MyStateType> MyStateManager;

   TestStateManager(const std::string& config);
   virtual ~TestStateManager();

private:

   typedef std::vector<const dtABC::Event::Type*> EventTypeVector;

   int GetInput();
   void OnMessage( MessageData* data );
   EventTypeVector GetEvents(const dtABC::State* state );

   static const int  mBufferSize;
   dtABC::State*     mStartState;
};

#endif // DELTA_TESTSTATEMANAGER
