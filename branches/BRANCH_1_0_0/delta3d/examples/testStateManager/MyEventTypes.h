#ifndef MYEVENTTYPES_INC
#define MYEVENTTYPES_INC

#include <dtABC/event.h>

class MyEventType : public dtABC::Event::Type
{
   DECLARE_ENUM(MyEventType); 
public:
   static const MyEventType ALT;
   static const MyEventType START;
protected:
   MyEventType( const std::string& name );
   virtual ~MyEventType();
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

#endif  // MYEVENTTYPES_INC
