#ifndef MYSTATETYPES_INC
#define MYSTATETYPES_INC

#include <dtABC/state.h>

class MyStateType : public dtABC::State::Type
{
   DECLARE_ENUM(MyStateType); 
public:
   static const MyStateType SHELL;
   static const MyStateType OPTIONS;
   static const MyStateType GAME;

protected:
   MyStateType(const std::string& name);
   virtual ~MyStateType();
};

class Shell : public dtABC::State
{ 
public:
   Shell(const std::string& name = "Shell" );
   virtual void HandleEvent( dtABC::Event* event = 0 ) {}
protected:
   virtual ~Shell() {}
};

class Options : public dtABC::State
{
public:
   Options(const std::string& name = "Options" );
   virtual void HandleEvent( dtABC::Event* event = 0 ) {}
protected:
   virtual ~Options() {}
};

class Game : public dtABC::State
{
public:
   Game(const std::string& name = "Game" );
   virtual void HandleEvent( dtABC::Event* event = 0 ) {}
protected:
   virtual ~Game() {}
};

#endif // MYSTATETYPES_INC
