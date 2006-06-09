/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author John K. Grant
 */
#include <dtABC/application.h>
#include <dtABC/applicationconfighandler.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/keyboard.h>
#include <dtCore/generickeyboardlistener.h>
#include <dtUtil/fileutils.h>                  // for verification when writing the config file
#include <dtUtil/stringutils.h>                // for dtUtil::ToInt
#include <dtUtil/xercesparser.h>               // for parsing

namespace dtTest
{
   /// unit tests for dtCore::Application
   class ApplicationTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ApplicationTests );
      CPPUNIT_TEST( TestInput );
      CPPUNIT_TEST( TestConfigSupport );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown() {}

         void TestInput();
         void TestConfigSupport();

      private:
         std::string mConfigName;
   };

   class TestApp : public dtABC::Application
   {
   public:
      typedef dtABC::Application BaseClass;

      TestApp(Producer::KeyboardKey key, Producer::KeyCharacter kc): BaseClass(),
         mPressedHit(false),
         mReleasedHit(false),
         mKey(key),
         mChar(kc)
      {
         GetKeyboardListener()->SetReleasedCallback(dtCore::GenericKeyboardListener::CallbackType(this,&TestApp::KeyReleased));
      }

      void Config()
      {
         BaseClass::Config();
      }

      void ResetHits()
      {
         mPressedHit = false;
         mReleasedHit = false;
      }

      bool KeyPressed(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
      {
         mPressedHit = true;

         if( key==mKey && kc==mChar )
         {
            return true;
         }
         return false;
      }

      bool KeyReleased(const dtCore::Keyboard* kb, Producer::KeyboardKey key, Producer::KeyCharacter kc)
      {
         mReleasedHit = true;

         if( key==mKey && kc==mChar )
         {
            return true;
         }
         return false;
      }

      Producer::KeyboardKey GetKey() const { return mKey; }
      Producer::KeyCharacter GetCharacter() const { return mChar; }
      bool GetPressedHit() const { return mPressedHit; }
      bool GetReleasedHit() const { return mReleasedHit; }

   private:
      bool mPressedHit;
      bool mReleasedHit;
      Producer::KeyboardKey mKey;
      Producer::KeyCharacter mChar;
   };
}

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( dtTest::ApplicationTests );

using namespace dtTest;

void ApplicationTests::setUp()
{
   mConfigName = "test_config.xml";
}

void ApplicationTests::TestInput()
{
   dtCore::RefPtr<dtTest::TestApp> app(new dtTest::TestApp(Producer::Key_N,Producer::KeyChar_n));

   app->Config();
   dtCore::RefPtr<dtCore::Keyboard> kb = app->GetKeyboard();
   const dtCore::KeyboardListener* applistener = app->GetKeyboardListener();
   const dtCore::KeyboardListener* firstlistener = kb->GetListeners().front().get();
   CPPUNIT_ASSERT_EQUAL( firstlistener , applistener );  // better be the app's listener

   app->ResetHits();
   CPPUNIT_ASSERT( !app->GetPressedHit() );  // better not be hit
   CPPUNIT_ASSERT( !app->GetReleasedHit() );  // better not be hit

   // test to see if the applicaiton's pressed callback is connected
   CPPUNIT_ASSERT( !kb->KeyDown(Producer::KeyChar_M) );  // better NOT handle it
   CPPUNIT_ASSERT( app->GetPressedHit() );  // better be hit
   CPPUNIT_ASSERT( !app->GetReleasedHit() );  // better NOT be hit
   CPPUNIT_ASSERT( kb->KeyDown(app->GetCharacter()) );  // better handle it

   app->ResetHits();
   CPPUNIT_ASSERT( !app->GetPressedHit() );  // better not be hit
   CPPUNIT_ASSERT( !app->GetReleasedHit() );  // better not be hit

   // test to see if the application's released callback is connected
   CPPUNIT_ASSERT( !kb->KeyUp(Producer::KeyChar_M) );  // better NOT handle it
   CPPUNIT_ASSERT( !app->GetPressedHit() );  // better be hit
   CPPUNIT_ASSERT( app->GetReleasedHit() );  // better be hit
   CPPUNIT_ASSERT( kb->KeyUp(app->GetCharacter()) );  // better handle it
}

void ApplicationTests::TestConfigSupport()
{
   // create the file
   const std::string created( dtABC::Application::GenerateDefaultConfigFile( mConfigName ) );

   // make sure it exists
   CPPUNIT_ASSERT( dtUtil::FileUtils::GetInstance().FileExists( mConfigName ) );

   // machine specific, should not be tested
   //dtCore::RefPtr<dtABC::Application> app(new dtABC::Application(created));
   //dtCore::DeltaWin* dwin = app->GetWindow();
   //dtCore::DeltaWin::Resolution res = dwin->GetCurrentResolution();
   //CPPUNIT_ASSERT_EQUAL( res.width , dtABC::Application::ConfigDefaultModel::RESOLUTION.width );
   //CPPUNIT_ASSERT_EQUAL( res.height , dtABC::Application::ConfigDefaultModel::RESOLUTION.height );
   //CPPUNIT_ASSERT_EQUAL( res.bitDepth , dtABC::Application::ConfigDefaultModel::RESOLUTION.bitDepth );
   //CPPUNIT_ASSERT_EQUAL( res.refresh , dtABC::Application::ConfigDefaultModel::RESOLUTION.refresh );

   // test the content from the parser
   dtABC::ApplicationConfigHandler handler;
   dtUtil::XercesParser parser;
   CPPUNIT_ASSERT( parser.Parse( mConfigName , handler , "application.xsd" ) );

   // compare the content that was parsed to the advertised default values
   // in order to confirm that:
   // a) the parser works, but more importantly
   // b) the writer wrote the right default values
   dtABC::ApplicationConfigData truth = dtABC::Application::GetDefaultConfigData();
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.CAMERA_NAME , truth.CAMERA_NAME );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.CHANGE_RESOLUTION , truth.CHANGE_RESOLUTION );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.FULL_SCREEN , truth.FULL_SCREEN );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.RESOLUTION.bitDepth , truth.RESOLUTION.bitDepth );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.RESOLUTION.height , truth.RESOLUTION.height );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.RESOLUTION.refresh , truth.RESOLUTION.refresh );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.RESOLUTION.width , truth.RESOLUTION.width );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.SCENE_NAME , truth.SCENE_NAME );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.SHOW_CURSOR , truth.SHOW_CURSOR );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.WINDOW_NAME , truth.WINDOW_NAME );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.WINDOW_X , truth.WINDOW_X );
   CPPUNIT_ASSERT_EQUAL( handler.mConfigData.WINDOW_Y , truth.WINDOW_Y );

   // delete the file
   dtUtil::FileUtils::GetInstance().FileDelete( mConfigName );

   // make sure it does not exist
   CPPUNIT_ASSERT( !dtUtil::FileUtils::GetInstance().FileExists( mConfigName ) );

   ///\todo test applying the data to an Application instance
}

