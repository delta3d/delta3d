#ifndef MYNETWORK_INCLUDE
#define MYNETWORK_INCLUDE

#include <dtnet/netmgr.h>
#include <dtCore/refptr.h>
#include <dtCore/object.h>

class MyNetwork : public dtNet::NetMgr
{
public:
   MyNetwork(void);
   virtual ~MyNetwork(void);

   virtual void OnReceive( GNE::Connection &conn);
   virtual void OnConnect( GNE::SyncConnection &conn);
   virtual void OnNewConn( GNE::SyncConnection &conn);
   virtual void OnExit( GNE::Connection &conn);

private:
   dtCore::RefPtr<dtCore::Object> mOtherPlayer;

   void MakePlayer();
   GNE::Mutex mMutex;
};

#endif // _DEBUG