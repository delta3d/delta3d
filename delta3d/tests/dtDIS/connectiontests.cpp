#include <cppunit/extensions/HelperMacros.h>
#include <dtDIS/Connection.h>
#include <DIS/DataStream.h>

#include <cstdlib>  // for NULL


namespace dtTest
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

using namespace dtTest;
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
   unsigned int mtu(1500);

   dtDIS::Connection discon;
   discon.Connect( inport , host.c_str() );

   // make a buffer to write to the socket
   int ia(1), ib(2);
   char ic('c'), id('d');

   DIS::DataStream outbuf(endian);
   outbuf << ia << ib << ic << id;

   //const DIS::DataStream::BufferType& obuffer = outbuf.GetBuffer();

   // write to the port
   discon.Send( &(outbuf[0]), outbuf.size() );

   // read from the port
   char ibuffer[1500];    /// needs to be the same as mtu
   discon.Receive( ibuffer , mtu );

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

