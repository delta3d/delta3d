#ifndef MYNETWORK_INCLUDE
#define MYNETWORK_INCLUDE

#include <dtNet/netmgr.h>
#include <dtCore/refptr.h>
#include <dtCore/object.h>

/** Deriving from NetMgr will allow use to overwrite some virtual methods.
  * We'll use these methods for controlling our network connections.
  */
class MyNetwork : public dtNet::NetMgr
{
public:
   MyNetwork( dtCore::Scene* scene );
   virtual ~MyNetwork() {}

   virtual void OnReceive( GNE::Connection &conn );
   virtual void OnExit( GNE::Connection &conn );
   virtual void OnDisconnect( GNE::Connection &conn );

   void PreFrame( const double deltaFrameTime );

private:
   dtCore::RefPtr< dtCore::Scene >  mScene;
   std::queue< dtCore::RefPtr<dtCore::Object> >    mObjectsToAdd;
   std::queue< std::string >        mIDsToRemove;

   ///a map of player ID strings and their corresponding Object
   typedef std::map<std::string, dtCore::RefPtr<dtCore::Object> > StringObjectMap;
   StringObjectMap mOtherPlayerMap;

   void MakePlayer(const std::string& ownerID);
   GNE::Mutex mMutex;
};

#endif // _DEBUG
