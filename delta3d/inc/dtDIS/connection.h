#ifndef _dtdis_dis_connection_h_
#define _dtdis_dis_connection_h_

#include <nl.h>                          // for member & implementation
#include <cstddef>                       // for size_t definition
#include <dtDIS/dtdisexport.h>           // for library export definitions

namespace dtDIS
{
   ///\brief Makes a multicast connection to support DIS networks.
   ///\note requires the HawkNL socket library.
   /// http://www.hawksoft.com/hawknl/
   class DT_DIS_EXPORT Connection
   {
   public:
      ///\brief makes a socket connection for the specified network.
      /// @param port the serial port for the connection's network host.
      /// @param host the name of the network host.
      void Connect(unsigned int port, const char* host);

      ///\brief closes the socket connection.
      void Disconnect();

      /// \brief publishes the data to the network.
      /// @param buf the buffer to be written to with network bytes.
      /// @param numbytes the number of bytes contained in the the buffer.
      void Send(const char* buf, size_t numbytes);

      ///\brief allocates buf with size numbytes.
      /// @param buf the buffer to be written to with network bytes
      /// @param numbytes the maximum index used for the buffer (buf)
      /// @return the number of bytes read from the connection
      size_t Receive(char* buf, size_t numbytes);

   private:
      void HandleError();

      NLsocket mSocket;
   };
}

#endif  // _dtdis_dis_connection_h_
