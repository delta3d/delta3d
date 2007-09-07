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
#include <prefix/dtgameprefix-src.h>
#include <dtABC/application.h>
#include <dtABC/applicationconfighandler.h>
#include <dtABC/applicationconfigwriter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/keyboard.h>
#include <dtCore/generickeyboardlistener.h>
#include <dtCore/globals.h>
#include <dtUtil/fileutils.h>                  // for verification when writing the config file
#include <dtUtil/stringutils.h>                // for dtUtil::ToInt
#include <dtUtil/xercesparser.h>               // for parsing
#include <dtUtil/librarysharingmanager.h>      // make sure this gets configured properly.
#include <Producer/KeyboardMouse>              // for keyboardkey and keyboard char

namespace dtTest
{
   /// unit tests for dtCore::Application
   class ApplicationTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE( ApplicationTests );
      CPPUNIT_TEST( TestInput );
      CPPUNIT_TEST( TestConfigProperties );
      CPPUNIT_TEST( TestConfigSupport );
      CPPUNIT_TEST( TestConfigSaveLoad );
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();
         void TestInput();
         void TestConfigProperties();
         void TestConfigSupport();
         void TestConfigSaveLoad();

      private:
         std::string mConfigName;
         
         void CompareConfigData(const dtABC::ApplicationConfigData& truth, const dtABC::ApplicationConfigData& actual)
         {
            CPPUNIT_ASSERT_EQUAL( actual.CHANGE_RESOLUTION , truth.CHANGE_RESOLUTION );
            CPPUNIT_ASSERT_EQUAL( actual.FULL_SCREEN , truth.FULL_SCREEN );
            CPPUNIT_ASSERT_EQUAL( actual.RESOLUTION.bitDepth , truth.RESOLUTION.bitDepth );
            CPPUNIT_ASSERT_EQUAL( actual.RESOLUTION.height , truth.RESOLUTION.height );
            CPPUNIT_ASSERT_EQUAL( actual.RESOLUTION.refresh , truth.RESOLUTION.refresh );
            CPPUNIT_ASSERT_EQUAL( actual.RESOLUTION.width , truth.RESOLUTION.width );
            CPPUNIT_ASSERT_EQUAL( actual.SCENE_NAME , truth.SCENE_NAME );
            CPPUNIT_ASSERT_EQUAL( actual.SHOW_CURSOR , truth.SHOW_CURSOR );
            CPPUNIT_ASSERT_EQUAL( actual.WINDOW_NAME , truth.WINDOW_NAME );
            CPPUNIT_ASSERT_EQUAL( actual.WINDOW_X , truth.WINDOW_X );
            CPPUNIT_ASSERT_EQUAL( actual.WINDOW_Y , truth.WINDOW_Y );
            CPPUNIT_ASSERT_EQUAL( actual.CAMERA_NAME , truth.CAMERA_NAME );

            std::ostringstream ss;
            ss << "Expected map:\n";
            for (std::map<std::string, std::string>::const_iterator i = truth.LOG_LEVELS.begin();
               i != truth.LOG_LEVELS.end(); ++i)
            {
               ss << " " << i->first << " " << i->second << std::endl;
            }

            ss << "Actual map:\n";
            for (std::map<std::string, std::string>::const_iterator i = actual.LOG_LEVELS.begin();
               i != actual.LOG_LEVELS.end(); ++i)
            {
               ss << " " << i->first << " " << i->second << std::endl;
            }

            CPPUNIT_ASSERT_MESSAGE(ss.str(), truth.LOG_LEVELS == actual.LOG_LEVELS );

            CPPUNIT_ASSERT_MESSAGE("Library path lists should match.", truth.LIBRARY_PATHS == actual.LIBRARY_PATHS );

            CPPUNIT_ASSERT_MESSAGE("Property sets should match", truth.mProperties == actual.mProperties );
         }
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

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION( ApplicationTests );

   void ApplicationTests::setUp()
   {
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      mConfigName = "test_config.xml";
      // delete the file
      dtUtil::FileUtils::GetInstance().FileDelete( mConfigName );
   }

   void ApplicationTests::tearDown()
   {
      std::vector<std::string> paths;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(paths);
      for(unsigned int i = 0; i < paths.size(); i++)
      {
         dtUtil::LibrarySharingManager::GetInstance().RemoveFromSearchPath(paths[i]);
      }
   }

   void ApplicationTests::TestInput()
   {
      dtCore::RefPtr<dtTest::TestApp> app(new dtTest::TestApp(Producer::Key_N,Producer::KeyChar_n));
      app->GetWindow()->SetPosition(0, 0, 50, 50);

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

   void ApplicationTests::TestConfigProperties()
   {
      dtCore::RefPtr<dtTest::TestApp> app(new dtTest::TestApp(Producer::Key_N,Producer::KeyChar_n));
      
      const std::string testDefault("abcd");
      const std::string testValue1("qqq");
      const std::string testValue2("qqv");
      const std::string testName("xyz");
      
      CPPUNIT_ASSERT_EQUAL(std::string(), app->GetConfigPropertyValue(testName));
      CPPUNIT_ASSERT_EQUAL(testDefault, app->GetConfigPropertyValue(testName, testDefault));
      
      app->SetConfigPropertyValue(testName, testValue1);
      CPPUNIT_ASSERT_EQUAL(testValue1, app->GetConfigPropertyValue(testName, testDefault));

      app->SetConfigPropertyValue(testName, testValue2);
      CPPUNIT_ASSERT_EQUAL(testValue2, app->GetConfigPropertyValue(testName, testDefault));

      app->RemoveConfigPropertyValue(testName);
      CPPUNIT_ASSERT_EQUAL(testDefault, app->GetConfigPropertyValue(testName, testDefault));
   }

   void ApplicationTests::TestConfigSupport()
   {
      //make sure the file doesn't already exist.
      CPPUNIT_ASSERT_MESSAGE("The config file should not exist yet.", !dtUtil::FileUtils::GetInstance().FileExists( mConfigName ) );

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
      
      CompareConfigData(truth, handler.mConfigData);

      // delete the file
      dtUtil::FileUtils::GetInstance().FileDelete( mConfigName );

      // make sure it does not exist
      CPPUNIT_ASSERT( !dtUtil::FileUtils::GetInstance().FileExists( mConfigName ) );

      ///\todo test applying the data to an Application instance
   }

   void ApplicationTests::TestConfigSaveLoad()
   {
      //make sure the file doesn't already exist.
      CPPUNIT_ASSERT_MESSAGE("The config file should not exist yet.", !dtUtil::FileUtils::GetInstance().FileExists( mConfigName ) );

      dtABC::ApplicationConfigData truth;
      truth.CHANGE_RESOLUTION = false;
      truth.FULL_SCREEN = false;
   
      truth.RESOLUTION.bitDepth = 16;
      truth.RESOLUTION.height = 32;
      truth.RESOLUTION.width = 19;
      truth.RESOLUTION.refresh = 50;

      truth.SCENE_NAME = "SomeScene";
      truth.SHOW_CURSOR = false;
      truth.WINDOW_NAME = "SomeWin";
      truth.WINDOW_X = 23;
      truth.WINDOW_Y = 97;
      truth.CAMERA_NAME = "SomeCam";
      
      truth.SCENE_INSTANCE = "SomeScene";
      truth.WINDOW_INSTANCE = "SomeWin";
      
      truth.LOG_LEVELS.insert(std::make_pair(dtUtil::Log::GetInstance().GetName(), "ERROR"));
      truth.LOG_LEVELS.insert(std::make_pair("SomeName", "Warn"));
      truth.LOG_LEVELS.insert(std::make_pair("AnotherName", "Error"));
      truth.LOG_LEVELS.insert(std::make_pair("horse", "Info"));
      truth.LOG_LEVELS.insert(std::make_pair("cow", "Debug"));
      truth.LOG_LEVELS.insert(std::make_pair("chicken", "Always"));
      
      truth.LIBRARY_PATHS.push_back("mypath1");
      truth.LIBRARY_PATHS.push_back("mypath2");
      truth.LIBRARY_PATHS.push_back("mypath3");
      
      truth.mProperties.insert(std::make_pair("Name1", "TestVal1"));
      truth.mProperties.insert(std::make_pair("Name2", "TestVal2"));
      truth.mProperties.insert(std::make_pair("Name3", "TestVal3"));

      dtABC::ApplicationConfigWriter acw;
      acw(mConfigName, truth);

      // make sure it exists
      CPPUNIT_ASSERT( dtUtil::FileUtils::GetInstance().FileExists( mConfigName ) );

      // test the content from the parser
      dtABC::ApplicationConfigHandler handler;
      dtUtil::XercesParser parser;

      CPPUNIT_ASSERT( parser.Parse( mConfigName , handler , "application.xsd" ) );

      CompareConfigData(truth, handler.mConfigData);

      //create an app to parse the config and actually load the values
      dtCore::RefPtr<dtABC::Application> app = new dtABC::Application(mConfigName);
      
      CPPUNIT_ASSERT_EQUAL(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance().GetLogLevel());
      CPPUNIT_ASSERT_EQUAL(dtUtil::Log::LOG_WARNING, dtUtil::Log::GetInstance("SomeName").GetLogLevel());
      CPPUNIT_ASSERT_EQUAL(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance("AnotherName").GetLogLevel());
      CPPUNIT_ASSERT_EQUAL(dtUtil::Log::LOG_INFO, dtUtil::Log::GetInstance("horse").GetLogLevel());
      CPPUNIT_ASSERT_EQUAL(dtUtil::Log::LOG_DEBUG, dtUtil::Log::GetInstance("cow").GetLogLevel());
      CPPUNIT_ASSERT_EQUAL(dtUtil::Log::LOG_ALWAYS, dtUtil::Log::GetInstance("chicken").GetLogLevel());

      std::vector<std::string> libPath;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(libPath);
      CPPUNIT_ASSERT_EQUAL(size_t(3), libPath.size());
      CPPUNIT_ASSERT_EQUAL(std::string("mypath1"), libPath[0]);
      CPPUNIT_ASSERT_EQUAL(std::string("mypath2"), libPath[1]);
      CPPUNIT_ASSERT_EQUAL(std::string("mypath3"), libPath[2]);

      CPPUNIT_ASSERT_EQUAL(std::string("Value1"), app->GetConfigPropertyValue("Name1"));
      CPPUNIT_ASSERT_EQUAL(std::string("Value2"), app->GetConfigPropertyValue("Name2"));
      CPPUNIT_ASSERT_EQUAL(std::string("Value3"), app->GetConfigPropertyValue("Name3"));

      app = NULL;
      
      // delete the file
      dtUtil::FileUtils::GetInstance().FileDelete( mConfigName );

      // make sure it does not exist
      CPPUNIT_ASSERT( !dtUtil::FileUtils::GetInstance().FileExists( mConfigName ) );
      
   }
}
