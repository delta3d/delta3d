#include "dt.h"

using namespace dtCore;

// Deriving from Base allows us to override the OnMessage() method
class classA : public Base
{
public:
   classA() {}
   virtual ~classA() {}
   virtual void OnMessage(MessageData *data )
   {
      Notify(ALWAYS,"  classA received '%s'", data->message.c_str());
   }

};

// Deriving from Base allows us to override the OnMessage() method
class classB : public Base
{
public:
   classB() {}
   virtual ~classB() {}

   virtual void OnMessage(MessageData *data )
   {
      Notify(ALWAYS,"  classB received '%s'", data->message.c_str());
      Notify(ALWAYS,"     data = '%s'", (char*)data->userData);
   }
   
};


int main( int argc, char **argv )
{
   classA *a = new classA();
   classB *b = new classB();

   a->AddSender(b); //subscribe to b's messages

   //fire off a messsage to whoever is subscribed to b
   b->SendMessage("hello from classB");

   b->AddSender(a); //subscribe to a's messages

   //Send some user data along with the message
   char *buffer = "123";
   a->SendMessage("hello from classA", buffer);
   
   Notify(ALWAYS, "hit Enter to quit");
   getchar();
   return 0;
}