#include <dtDIS/connection.h>
#include <dtUtil/log.h>

#include <sstream>

using namespace dtDIS;

void Connection::Connect(unsigned int port, const char* host, bool useBroadcast)
{
   NLboolean success = nlInit();

   if (!success)
   {
      HandleError();
      return;
   }

   if (nlSelectNetwork(NL_IP) == NL_FALSE)
   {
      LOG_ERROR("Can't select network type");
   }

   NLaddress  maddr;
   if (nlStringToAddr(host, &maddr) == NL_FALSE )
   {
      std::ostringstream strm;
      strm << "Can't get address for : " + std::string(host) + ". "
           << "Error:" << nlGetErrorStr(nlGetError())
           << ". System: " << nlGetSystemErrorStr(nlGetSystemError());
      LOG_ERROR( strm.str() );
   }

   nlSetAddrPort(&maddr, port);

   nlHint(NL_REUSE_ADDRESS, NL_TRUE);


   if (useBroadcast)
   {
      mSocket = nlOpen(port, NL_BROADCAST);
   }
   else
   {
      nlHint(NL_MULTICAST_TTL, NL_TTL_LOCAL);
      mSocket = nlOpen(port, NL_UDP_MULTICAST);
   }

   if(mSocket == NL_INVALID)
   {
      std::ostringstream strm;
      strm << "Can't open socket: " << nlGetErrorStr(nlGetError())
           << ". System: " << + nlGetSystemErrorStr(nlGetSystemError());
      LOG_ERROR( strm.str() )
   }

   if (!useBroadcast)
   {
      if(nlConnect(mSocket, &maddr) == NL_FALSE)
      {
         nlClose(mSocket);

         std::ostringstream strm;
         strm << "Can't connect to socket: " << nlGetErrorStr(nlGetError())
            << ". System: " << nlGetSystemErrorStr(nlGetSystemError());
         LOG_ERROR( strm.str() );
      }   
   }
 
}

///\todo is this the ideal NL call?
void Connection::Disconnect()
{
   nlShutdown();
}

void Connection::Send(const char* buf, size_t numbytes)
{
   if( numbytes < 1 )
   {
      return;
   }

   if (int ret = nlWrite(mSocket, (NLvoid *)buf, numbytes ))
   {
      if (ret == 0)
      {
         LOG_WARNING("Network buffers are full");
      }
      else if (ret == NL_INVALID)
      {
         std::ostringstream strm;
         strm << "Problem sending: ";
         LOG_ERROR(strm.str() + nlGetErrorStr(nlGetError()) + ". System: " + nlGetSystemErrorStr(nlGetSystemError()) );
      }
   }
}

size_t Connection::Receive(char* buf, size_t numbytes)
{
   NLint result = nlRead(mSocket, (NLvoid *)buf, (NLint)numbytes);

   if ( result == NL_INVALID )
   {
      HandleError();
      return 0;
   }

   return result;
}

void Connection::HandleError()
{
   NLenum error = nlGetError();
   const NLchar* errorString = nlGetErrorStr( error );

   LOG_ERROR("A network error occurred: " + std::string(errorString));
}

