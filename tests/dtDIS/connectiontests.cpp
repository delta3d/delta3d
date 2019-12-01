/* -*-c++-*-
* allTests - This source file (.h & .cpp) - Using 'The MIT License'
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#include <prefix/unittestprefix.h>

#include <cppunit/extensions/HelperMacros.h>
#include <dtDIS/connection.h>
#include <DIS/DataStream.h>

#include <dtCore/timer.h>

#include <cstdlib>  // for NULL


namespace dtDIS
{
   /// tests the multicast socket code needed for DIS.
   class ConnectionTests : public CPPUNIT_NS::TestFixture
   {
   public:
      void setup();
      void teardown(); 

      void TestConnection();

      CPPUNIT_TEST_SUITE( ConnectionTests );
         CPPUNIT_TEST( TestConnection );
      CPPUNIT_TEST_SUITE_END();
   };

}

using namespace dtDIS;
CPPUNIT_TEST_SUITE_REGISTRATION( ConnectionTests );

void ConnectionTests::setup()
{
}

void ConnectionTests::teardown()
{
}

void ConnectionTests::TestConnection()
{
   unsigned int inport( 1258 );
   std::string host("234.235.236.237");
   DIS::Endian endian(DIS::BIG);
   const unsigned int mtu(1500);

   dtDIS::Connection discon;
   discon.Connect(inport, host.c_str(), false);

   // make a buffer to write to the socket
   int ia(1), ib(2);
   char ic('c'), id('d');

   DIS::DataStream outbuf(endian);
   outbuf << ia << ib << ic << id;

   //const DIS::DataStream::BufferType& obuffer = outbuf.GetBuffer();

   // write to the port
   discon.Send( &(outbuf[0]), outbuf.size() );

   dtCore::AppSleep(1);

   // read from the port
   char ibuffer[mtu];    /// needs to be the same as mtu
   const size_t r = discon.Receive( ibuffer , mtu );
	 CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of bytes read. If 0, check your firewall settings.", outbuf.size(), r);

   // check to know if the same data was found in the socket
   DIS::DataStream inbuf(endian);
   inbuf.SetStream( ibuffer , mtu , endian );
   int oa(8), ob(9);
   char oc('x'), od('y');
   inbuf >> oa >> ob >> oc >> od;
   CPPUNIT_ASSERT_EQUAL( ia , oa );
   CPPUNIT_ASSERT_EQUAL( ib , ob );
   CPPUNIT_ASSERT_EQUAL( ic , oc );
   CPPUNIT_ASSERT_EQUAL( id , od );

   // finish the unit test, clean up time.
   discon.Disconnect();
}

