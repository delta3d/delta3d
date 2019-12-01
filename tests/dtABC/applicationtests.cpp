/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, MOVES Institute
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
 *
 * @author John K. Grant, Curtiss Murphy
 */

#include <prefix/unittestprefix.h>
#include <dtABC/application.h>
#include <dtABC/applicationconfighandler.h>
#include <dtABC/applicationconfigwriter.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtCore/system.h>
#include <dtCore/keyboard.h>
#include <dtCore/generickeyboardlistener.h>
#include <dtCore/scene.h>
#include <dtCore/databasepager.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/xercesparser.h>
#include <dtUtil/xerceswriter.h>
#include <dtUtil/librarysharingmanager.h>
#include <dtUtil/log.h>

#include <osgDB/DatabasePager>
#include <osgViewer/View>
#include <osg/Version>


//////////////////////////////////////////////////////////////////////////
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/util/XMLString.hpp>

/// Little util to generate a mal-formed config xml file
void WriteBadConfigFile(const std::string& filename)
{
   XERCES_CPP_NAMESPACE_USE

   dtCore::RefPtr<dtUtil::XercesWriter> writer = new dtUtil::XercesWriter();
   writer->CreateDocument("Application");
   XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc = writer->GetDocument();
   DOMElement* app = doc->getDocumentElement();

   DOMElement* camera = doc->createElement(XMLString::transcode("Camera"));
   app->appendChild(camera);

   writer->WriteFile(filename);
}


//////////////////////////////////////////////////////////////////////////
namespace dtTest
{
   /// unit tests for dtCore::Application
   class ApplicationTests : public CPPUNIT_NS::TestFixture
   {
      CPPUNIT_TEST_SUITE(ApplicationTests);
      CPPUNIT_TEST(TestInput);
      CPPUNIT_TEST(TestConstructorAddDeltaData);
      CPPUNIT_TEST(TestConfigProperties);
      CPPUNIT_TEST(TestConfigSupport);
      CPPUNIT_TEST(TestConfigSaveLoad);
      CPPUNIT_TEST(TestConfigLogLevelDefaultOverride);
      CPPUNIT_TEST(TestReadSystemProperties);
      CPPUNIT_TEST(TestSupplyingWindowToApplicationConstructor);
      CPPUNIT_TEST(TestReadingBadConfigFile);
      CPPUNIT_TEST(TestRemovingView);
      CPPUNIT_TEST(TestGettingView);
      CPPUNIT_TEST_SUITE_END();

      public:
         void setUp();
         void tearDown();
         void TestInput();
         void TestConstructorAddDeltaData();
         void TestConfigProperties();
         void TestConfigSupport();
         void TestConfigSaveLoad();
         void TestConfigLogLevelDefaultOverride();
         void TestReadSystemProperties();
         void TestSupplyingWindowToApplicationConstructor();
         void TestReadingBadConfigFile();
         void TestRemovingView();
         void TestGettingView();

      private:
         std::string mConfigName;
         dtUtil::Log::LogMessageType mLogLevel;

         void ResetConfigPropertyDefaults(dtABC::Application& app);

         void CompareConfigData(const dtABC::ApplicationConfigData& truth, const dtABC::ApplicationConfigData& actual)
         {
            CPPUNIT_ASSERT_EQUAL(actual.CHANGE_RESOLUTION, truth.CHANGE_RESOLUTION);
            CPPUNIT_ASSERT_EQUAL(actual.FULL_SCREEN, truth.FULL_SCREEN);
            CPPUNIT_ASSERT_EQUAL(actual.REALIZE_UPON_CREATE, truth.REALIZE_UPON_CREATE);
            CPPUNIT_ASSERT_EQUAL(actual.RESOLUTION.bitDepth, truth.RESOLUTION.bitDepth);
            CPPUNIT_ASSERT_EQUAL(actual.RESOLUTION.height, truth.RESOLUTION.height);
            CPPUNIT_ASSERT_EQUAL(actual.RESOLUTION.refresh, truth.RESOLUTION.refresh);
            CPPUNIT_ASSERT_EQUAL(actual.RESOLUTION.width, truth.RESOLUTION.width);
            CPPUNIT_ASSERT_EQUAL(actual.SCENE_NAME, truth.SCENE_NAME);
            CPPUNIT_ASSERT_EQUAL(actual.SHOW_CURSOR, truth.SHOW_CURSOR);
            CPPUNIT_ASSERT_EQUAL(actual.WINDOW_NAME, truth.WINDOW_NAME);
            CPPUNIT_ASSERT_EQUAL(actual.WINDOW_X, truth.WINDOW_X);
            CPPUNIT_ASSERT_EQUAL(actual.WINDOW_Y, truth.WINDOW_Y);
            CPPUNIT_ASSERT_EQUAL(actual.CAMERA_NAME, truth.CAMERA_NAME);
            CPPUNIT_ASSERT_EQUAL(actual.REALIZE_UPON_CREATE, truth.REALIZE_UPON_CREATE);
            CPPUNIT_ASSERT_EQUAL(actual.VSYNC, truth.VSYNC);
            CPPUNIT_ASSERT_EQUAL(actual.MULTI_SAMPLE, truth.MULTI_SAMPLE);

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

            CPPUNIT_ASSERT_MESSAGE(ss.str(), truth.LOG_LEVELS == actual.LOG_LEVELS);

            CPPUNIT_ASSERT_MESSAGE("Library path lists should match.", truth.LIBRARY_PATHS == actual.LIBRARY_PATHS);

            CPPUNIT_ASSERT_MESSAGE("Property sets should match", truth.mProperties == actual.mProperties);
         }
   };

   class TestApp : public dtABC::Application
   {
   public:
      typedef dtABC::Application BaseClass;

      TestApp(int key): BaseClass("config.xml"),
         mPressedHit(false),
         mReleasedHit(false),
         mKey(key)
      {
         GetKeyboardListener()->SetReleasedCallback(dtCore::GenericKeyboardListener::CallbackType(this,&TestApp::KeyReleased));
      }

      TestApp(const std::string& config, int key): BaseClass(config),
         mPressedHit(false),
         mReleasedHit(false),
         mKey(key)
      {
         GetKeyboardListener()->SetReleasedCallback(dtCore::GenericKeyboardListener::CallbackType(this,&TestApp::KeyReleased));
      }

      void Config()
      {
         BaseClass::Config();
      }

      //overridden to make it public for the test.
      void ReadSystemProperties()
      {
         BaseClass::ReadSystemProperties();
      }

      void ResetHits()
      {
         mPressedHit = false;
         mReleasedHit = false;
      }

      bool KeyPressed(const dtCore::Keyboard* kb, int key)
      {
         mPressedHit = true;

         if (key == mKey)
         {
            return true;
         }
         return false;
      }

      bool KeyReleased(const dtCore::Keyboard* kb, int key)
      {
         mReleasedHit = true;

         if (key == mKey)
         {
            return true;
         }
         return false;
      }

      int GetKey() const { return mKey; }
      bool GetPressedHit() const { return mPressedHit; }
      bool GetReleasedHit() const { return mReleasedHit; }

   private:
      bool mPressedHit;
      bool mReleasedHit;
      int mKey;
   };

   // Registers the fixture into the 'registry'
   CPPUNIT_TEST_SUITE_REGISTRATION(ApplicationTests);

   void ApplicationTests::setUp()
   {
      mLogLevel = dtUtil::Log::GetInstance().GetLogLevel();
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
      mConfigName = "test_config.xml";
      // delete the file
      dtUtil::FileUtils::GetInstance().FileDelete(mConfigName);
   }

   void ApplicationTests::tearDown()
   {
      std::vector<std::string> paths;
      dtUtil::LibrarySharingManager::GetInstance().GetSearchPath(paths);
      for (unsigned int i = 0; i < paths.size(); i++)
      {
         dtUtil::LibrarySharingManager::GetInstance().RemoveFromSearchPath(paths[i]);
      }
      dtUtil::Log::GetInstance().SetLogLevel(mLogLevel);
   }

   void ApplicationTests::TestConstructorAddDeltaData()
   {
      dtUtil::SetDataFilePathList("");
      dtCore::RefPtr<dtTest::TestApp> app(new dtTest::TestApp('N'));
      CPPUNIT_ASSERT(dtUtil::GetDataFilePathList().find(dtUtil::GetDeltaDataPathList(), 0) != std::string::npos);
      std::string pathList = dtUtil::GetDataFilePathList();
      dtCore::RefPtr<dtTest::TestApp> app2(new dtTest::TestApp('N'));
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Delta data should not have been added twice.", pathList, dtUtil::GetDataFilePathList());
   }

   void ApplicationTests::TestInput()
   {
      dtCore::RefPtr<dtTest::TestApp> app(new dtTest::TestApp('N'));
      app->GetWindow()->SetPosition(0, 0, 50, 50);

      app->Config();
      dtCore::RefPtr<dtCore::Keyboard> kb = app->GetKeyboard();
      const dtCore::KeyboardListener* applistener = app->GetKeyboardListener();
      const dtCore::KeyboardListener* firstlistener = kb->GetListeners().front().get();
      CPPUNIT_ASSERT_EQUAL(firstlistener, applistener);  // better be the app's listener

      app->ResetHits();
      CPPUNIT_ASSERT(!app->GetPressedHit());  // better not be hit
      CPPUNIT_ASSERT(!app->GetReleasedHit());  // better not be hit

      // test to see if the applicaiton's pressed callback is connected
      CPPUNIT_ASSERT(!kb->KeyDown('M'));  // better NOT handle it
      CPPUNIT_ASSERT(app->GetPressedHit());  // better be hit
      CPPUNIT_ASSERT(!app->GetReleasedHit());  // better NOT be hit
      CPPUNIT_ASSERT(kb->KeyDown(app->GetKey()));  // better handle it

      app->ResetHits();
      CPPUNIT_ASSERT(!app->GetPressedHit());  // better not be hit
      CPPUNIT_ASSERT(!app->GetReleasedHit());  // better not be hit

      // test to see if the application's released callback is connected
      CPPUNIT_ASSERT(!kb->KeyUp('M'));  // better NOT handle it
      CPPUNIT_ASSERT(!app->GetPressedHit());  // better be hit
      CPPUNIT_ASSERT(app->GetReleasedHit());  // better be hit
      CPPUNIT_ASSERT(kb->KeyUp(app->GetKey()));  // better handle it
   }

   void ApplicationTests::TestConfigProperties()
   {
      dtCore::RefPtr<dtTest::TestApp> app(new dtTest::TestApp('N'));

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

      app->SetConfigPropertyValue("blah." + testName, testValue1);
      app->SetConfigPropertyValue("blah." + testName + "2", testValue2);

      std::vector<std::pair<std::string, std::string> > nameVal;
      app->GetConfigPropertiesWithPrefix("blah.", nameVal, true);

      CPPUNIT_ASSERT_EQUAL(2U, unsigned(nameVal.size()));

      CPPUNIT_ASSERT_EQUAL(testName, nameVal[0].first);
      CPPUNIT_ASSERT_EQUAL(testValue1, nameVal[0].second);
      CPPUNIT_ASSERT_EQUAL(testName+"2", nameVal[1].first);
      CPPUNIT_ASSERT_EQUAL(testValue2, nameVal[1].second);

      app->GetConfigPropertiesWithPrefix("blah.", nameVal, false);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("It should not clear the vector.", 4U, unsigned(nameVal.size()));

      nameVal.clear();
      app->GetConfigPropertiesWithPrefix("blah.", nameVal, false);
      CPPUNIT_ASSERT_EQUAL(2U, unsigned(nameVal.size()));

      CPPUNIT_ASSERT_EQUAL("blah." + testName, nameVal[0].first);
      CPPUNIT_ASSERT_EQUAL(testValue1, nameVal[0].second);
      CPPUNIT_ASSERT_EQUAL("blah." + testName+"2", nameVal[1].first);
      CPPUNIT_ASSERT_EQUAL(testValue2, nameVal[1].second);

   }

   void ApplicationTests::TestConfigSupport()
   {
      //make sure the file doesn't already exist.
      CPPUNIT_ASSERT_MESSAGE("The config file should not exist yet.", 
         !dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

      // create the file
      const std::string created( dtABC::Application::GenerateDefaultConfigFile(mConfigName));

      // make sure it exists
      CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

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
      CPPUNIT_ASSERT(parser.Parse(mConfigName, handler, "application.xsd"));

      // compare the content that was parsed to the advertised default values
      // in order to confirm that:
      // a) the parser works, but more importantly
      // b) the writer wrote the right default values
      dtABC::ApplicationConfigData truth = dtABC::Application::GetDefaultConfigData();

      CompareConfigData(truth, handler.mConfigData);

      // delete the file
      dtUtil::FileUtils::GetInstance().FileDelete(mConfigName);

      // make sure it does not exist
      CPPUNIT_ASSERT( !dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

      ///\todo test applying the data to an Application instance
   }

   void ApplicationTests::TestConfigSaveLoad()
   {
      //make sure the file doesn't already exist.
      CPPUNIT_ASSERT_MESSAGE("The config file should not exist yet.", 
         !dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

      dtABC::ApplicationConfigData truth;
      truth.CHANGE_RESOLUTION = false;
      truth.FULL_SCREEN = false;
      truth.REALIZE_UPON_CREATE = false;

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
      truth.VIEW_NAME = "SomeCam";

      truth.SCENE_INSTANCE = "SomeScene";
      truth.WINDOW_INSTANCE = "SomeWin";
      truth.CAMERA_INSTANCE = "SomeCam";
      truth.VSYNC = false;
      truth.MULTI_SAMPLE = 24;

      // Can't write the default to the config file or it will overwrite all other log values.
      // Behavior tested in TestConfigLogLevelDefaultOverride.
      //truth.LOG_LEVELS.insert(std::make_pair(dtUtil::Log::GetInstance().GetName(), "ERROR"));
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
      CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

      // test the content from the parser
      dtABC::ApplicationConfigHandler handler;
      dtUtil::XercesParser parser;

      CPPUNIT_ASSERT(parser.Parse(mConfigName, handler, "application.xsd"));

      CompareConfigData(truth, handler.mConfigData);

      //create an app to parse the config and actually load the values
      dtCore::RefPtr<dtABC::Application> app = new dtTest::TestApp(mConfigName, 0);

      // Can't write the default to the config file or it will overwrite all other log values.
      // Behavior tested in TestConfigLogLevelDefaultOverride.
      //CPPUNIT_ASSERT_EQUAL(dtUtil::Log::LOG_ERROR, dtUtil::Log::GetInstance().GetLogLevel());
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

      CPPUNIT_ASSERT_EQUAL(std::string("TestVal1"), app->GetConfigPropertyValue("Name1"));
      CPPUNIT_ASSERT_EQUAL(std::string("TestVal2"), app->GetConfigPropertyValue("Name2"));
      CPPUNIT_ASSERT_EQUAL(std::string("TestVal3"), app->GetConfigPropertyValue("Name3"));

      app = NULL;

      // delete the file
      dtUtil::FileUtils::GetInstance().FileDelete(mConfigName);

      // make sure it does not exist
      CPPUNIT_ASSERT( !dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

   }

   ////////////////////////////////////////////////////////////////////
   void ApplicationTests::TestConfigLogLevelDefaultOverride()
   {
      //Create, write, and load an application config file.
      //Verify that specified Log instances retain their log levels, while
      //unspecified Log instances get the defined default level.  Also check
      //that new Log instances use the default log level.

      using namespace dtUtil;

      //make sure the file doesn't already exist.
      CPPUNIT_ASSERT_MESSAGE("The config file should not exist yet.", 
         !dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

      dtABC::ApplicationConfigData truth = dtABC::Application::GetDefaultConfigData();
      truth.LOG_LEVELS.insert(std::make_pair("", "ERROR"));
      truth.LOG_LEVELS.insert(std::make_pair("SomeName", "Warn"));
      truth.LOG_LEVELS.insert(std::make_pair("AnotherName", "Debug"));
      truth.LOG_LEVELS.insert(std::make_pair("horse", "Info"));
      truth.LOG_LEVELS.insert(std::make_pair("cow", "Debug"));
      truth.LOG_LEVELS.insert(std::make_pair("chicken", "Always"));
      truth.GLOBAL_LOG_LEVEL = "ERROR";

      dtABC::ApplicationConfigWriter acw;
      acw(mConfigName, truth);

      // make sure it exists
      CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().FileExists(mConfigName));

      // test the content from the parser
      dtABC::ApplicationConfigHandler handler;
      dtUtil::XercesParser parser;

      CPPUNIT_ASSERT(parser.Parse(mConfigName, handler, "application.xsd"));

      CompareConfigData(truth, handler.mConfigData);

      //make a new Log instance that doesn't have it's log level set by the config
      dtUtil::Log::GetInstance("ShouldBeSetByGlobalLevel");

      //create an app to parse the config and actually load the values
      dtCore::RefPtr<dtABC::Application> app = new dtABC::Application(mConfigName);

      // Ensure the specified log levels were *not* changed
      std::map<std::string, std::string>::iterator itr = truth.LOG_LEVELS.begin();
      while (itr != truth.LOG_LEVELS.end())
      {
         CPPUNIT_ASSERT_EQUAL(Log::GetLogLevelForString(itr->second), Log::GetInstance(itr->first).GetLogLevel());
         
         ++itr;
      }

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The unspecified Log instance should have it's level set by the global default level",
                                   dtUtil::Log::GetLogLevelForString(truth.GLOBAL_LOG_LEVEL),
                                   dtUtil::Log::GetInstance("ShouldBeSetByGlobalLevel").GetLogLevel());

      //A brand new Log should have it's level the same as the default, after the config.xml has been loaded
      CPPUNIT_ASSERT_EQUAL_MESSAGE("A new Log instance should have it's level set by the global default level",
                                    dtUtil::Log::GetLogLevelForString(truth.GLOBAL_LOG_LEVEL),
                                    dtUtil::Log::GetInstance("BrandNewLog").GetLogLevel());

      app = NULL;

      // delete the file
      dtUtil::FileUtils::GetInstance().FileDelete(mConfigName);
   }


   void ApplicationTests::ResetConfigPropertyDefaults(dtABC::Application& app)
   {
      dtCore::System& system = dtCore::System::GetInstance();

      system.SetUseFixedTimeStep(false);
      system.SetFrameRate(60.0);
      system.SetMaxTimeBetweenDraws(0.003);

      osgDB::DatabasePager* pager = app.GetView()->GetOsgViewerView()->getDatabasePager();
      if (pager != NULL)
      {
#if defined(OSG_VERSION_MAJOR) && OSG_VERSION_MAJOR >= 3
         if (pager->getIncrementalCompileOperation() == NULL)
         {
            pager->setIncrementalCompileOperation(new osgUtil::IncrementalCompileOperation());
         }
         pager->getIncrementalCompileOperation()->setTargetFrameRate(100);
         pager->getIncrementalCompileOperation()->setMaximumNumOfObjectsToCompilePerFrame(2);
#else
         pager->setTargetFrameRate(100);
         pager->setMinimumTimeAvailableForGLCompileAndDeletePerFrame(0.001);
         pager->setExpiryDelay(10.0);
#endif


         pager->setDoPreCompile(true);
         pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_DEFAULT);
         pager->setDrawablePolicy(osgDB::DatabasePager::DO_NOT_MODIFY_DRAWABLE_SETTINGS);
      }
   }

   void ApplicationTests::TestReadSystemProperties()
   {
      dtCore::RefPtr<dtTest::TestApp> app(new dtTest::TestApp('N'));
      //app->GetView()->GetScene()->EnablePaging();
      ResetConfigPropertyDefaults(*app);

      osgDB::DatabasePager* pager = app->GetView()->GetDatabasePager()->GetOsgDatabasePager();

      pager->setSchedulePriority(OpenThreads::Thread::THREAD_PRIORITY_DEFAULT);

      app->SetConfigPropertyValue(dtABC::Application::SIM_FRAME_RATE, "32.1");
      app->SetConfigPropertyValue(dtABC::Application::MAX_TIME_BETWEEN_DRAWS, "1.03");
      app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_PRECOMPILE_OBJECTS, "false");
      app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_MAX_OBJECTS_TO_COMPILE_PER_FRAME, "700");
      app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_MIN_TIME_FOR_OBJECT_COMPILE, "31.0");
      app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_EXPIRY_DELAY, "6.23");

      app->SetConfigPropertyValue(dtABC::Application::USE_FIXED_TIME_STEP, "false");

      try
      {
         dtCore::System& system = dtCore::System::GetInstance();

         //-------------------------------------
         app->ReadSystemProperties();

         CPPUNIT_ASSERT(!system.GetUsesFixedTimeStep());
         CPPUNIT_ASSERT_DOUBLES_EQUAL(32.1f, system.GetFrameRate(), 0.01);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(1.03, system.GetMaxTimeBetweenDraws(), 0.01);

         CPPUNIT_ASSERT(! pager->getDoPreCompile());
#if defined(OSG_VERSION_MAJOR) && OSG_VERSION_MAJOR >= 3
         CPPUNIT_ASSERT_EQUAL(700U, pager->getIncrementalCompileOperation()->getMaximumNumOfObjectsToCompilePerFrame());
         CPPUNIT_ASSERT_DOUBLES_EQUAL(31.0, pager->getIncrementalCompileOperation()->getMinimumTimeAvailableForGLCompileAndDeletePerFrame(), 0.01);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, pager->getIncrementalCompileOperation()->getTargetFrameRate(), 0.01);
#else
         CPPUNIT_ASSERT_EQUAL(700U, pager->getMaximumNumOfObjectsToCompilePerFrame());
         CPPUNIT_ASSERT_DOUBLES_EQUAL(31.0, pager->getMinimumTimeAvailableForGLCompileAndDeletePerFrame(), 0.01);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(6.23, pager->getExpiryDelay(), 0.01);
         CPPUNIT_ASSERT_DOUBLES_EQUAL(100.0, pager->getTargetFrameRate(), 0.01);
#endif
         CPPUNIT_ASSERT(pager->getDrawablePolicy() == osgDB::DatabasePager::DO_NOT_MODIFY_DRAWABLE_SETTINGS);

         //-------------------------------------

         app->SetConfigPropertyValue(dtABC::Application::USE_FIXED_TIME_STEP, "true");
         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_DRAWABLE_POLICY, "DisplayList");
         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_THREAD_PRIORITY, "MAX");

         app->ReadSystemProperties();

         CPPUNIT_ASSERT(pager->getDrawablePolicy() == osgDB::DatabasePager::USE_DISPLAY_LISTS);
         //It no longer does this
//         CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(
//                  "If the system is set to fixed frame rate, and the pager target framerate is not "
//                  "set, should pickup the one from the system",
//                  system.GetFrameRate(), pager->getTargetFrameRate(), 0.01);

         //-------------------------------------

         app->SetConfigPropertyValue(dtABC::Application::USE_FIXED_TIME_STEP, "false");
         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_DRAWABLE_POLICY, "VBO");
         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_THREAD_PRIORITY, "HIGH");
         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_TARGET_FRAMERATE, "31.6");

         app->ReadSystemProperties();

         ///Check before deleting the pager.
         CPPUNIT_ASSERT(pager->getDrawablePolicy() == osgDB::DatabasePager::USE_VERTEX_BUFFER_OBJECTS);
#if defined(OSG_VERSION_MAJOR) && OSG_VERSION_MAJOR >= 3
         CPPUNIT_ASSERT_DOUBLES_EQUAL(31.6, pager->getIncrementalCompileOperation()->getTargetFrameRate(), 0.01);
#else
         CPPUNIT_ASSERT_DOUBLES_EQUAL(31.6, pager->getTargetFrameRate(), 0.01);
#endif


         // Destroying and re-creating the pager should not affect the settings because it should re-read them.
         // so we test that be deleting and recreating the pager after setting and reading the properties.
         dtCore::RefPtr<dtCore::DatabasePager> savedPager = app->GetView()->GetDatabasePager();
         app->GetView()->SetDatabasePager(NULL);
         CPPUNIT_ASSERT_MESSAGE("The pager should be deleted", app->GetView()->GetDatabasePager() == NULL);
         app->GetView()->SetDatabasePager( savedPager.get() );
         pager = app->GetView()->GetDatabasePager()->GetOsgDatabasePager();
         CPPUNIT_ASSERT_MESSAGE("The pager should now exist again.", pager != NULL);

         ///Check after deleting the pager.
         CPPUNIT_ASSERT(pager->getDrawablePolicy() == osgDB::DatabasePager::USE_VERTEX_BUFFER_OBJECTS);

#if defined(OSG_VERSION_MAJOR) && OSG_VERSION_MAJOR >= 3
         CPPUNIT_ASSERT_DOUBLES_EQUAL(31.6, pager->getIncrementalCompileOperation()->getTargetFrameRate(), 0.01);
#else
         CPPUNIT_ASSERT_DOUBLES_EQUAL(31.6, pager->getTargetFrameRate(), 0.01);
#endif

#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR <= 2  && OSG_VERSION_MINOR <= 4
         CPPUNIT_ASSERT(ignoreThreadPrioritySupport ||
                  pager->getSchedulePriority() == OpenThreads::Thread::THREAD_PRIORITY_HIGH);
#endif
         //-------------------------------------

         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_DRAWABLE_POLICY, "VertexArrays");
         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_THREAD_PRIORITY, "NOMINAL");

         app->ReadSystemProperties();

         CPPUNIT_ASSERT(pager->getDrawablePolicy() == osgDB::DatabasePager::USE_VERTEX_ARRAYS);
#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR <= 2  && OSG_VERSION_MINOR <= 4
         CPPUNIT_ASSERT(ignoreThreadPrioritySupport ||
                  pager->getSchedulePriority() == OpenThreads::Thread::THREAD_PRIORITY_NOMINAL);
#endif
         //-------------------------------------

         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_DRAWABLE_POLICY, "DoNotModify");
         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_THREAD_PRIORITY, "LOW");

         app->ReadSystemProperties();

         CPPUNIT_ASSERT(pager->getDrawablePolicy() == osgDB::DatabasePager::DO_NOT_MODIFY_DRAWABLE_SETTINGS);

#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR <= 2  && OSG_VERSION_MINOR <= 4
         CPPUNIT_ASSERT(ignoreThreadPrioritySupport ||
                  pager->getSchedulePriority() == OpenThreads::Thread::THREAD_PRIORITY_LOW);
#endif
         //-------------------------------------

         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_THREAD_PRIORITY, "MIN");

         app->ReadSystemProperties();

#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR <= 2  && OSG_VERSION_MINOR <= 4
         CPPUNIT_ASSERT(ignoreThreadPrioritySupport ||
                  pager->getSchedulePriority() == OpenThreads::Thread::THREAD_PRIORITY_MIN);
#endif
         //-------------------------------------

         app->SetConfigPropertyValue(dtCore::DatabasePager::DATABASE_PAGER_THREAD_PRIORITY, "DEFAULT");

         app->ReadSystemProperties();

#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR <= 2  && OSG_VERSION_MINOR <= 4
         CPPUNIT_ASSERT(ignoreThreadPrioritySupport ||
                  pager->getSchedulePriority() == OpenThreads::Thread::THREAD_PRIORITY_DEFAULT);
#endif
      }
      catch (...)
      {
         ResetConfigPropertyDefaults(*app);
         throw;
      }
      ResetConfigPropertyDefaults(*app);
   }

   void ApplicationTests::TestSupplyingWindowToApplicationConstructor()
   {
      using namespace dtCore;
      using namespace dtABC;
      RefPtr<DeltaWin> win = new DeltaWin();

      RefPtr<Application> app = new Application("testie", win.get());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Supplied DeltaWin is not what was returned",
                                    win.get(), app->GetWindow() );

      app = NULL;
      win = NULL;
   }


   //////////////////////////////////////////////////////////////////////////
   void ApplicationTests::TestReadingBadConfigFile()
   {
      // We turn off 'error' logging here because the read bad config
      // prints out ".... error:32 ..." which causes MS Visual Studio to
      // flag it as an error. The teardown will unset this.
      dtUtil::Log::GetInstance().SetLogLevel(dtUtil::Log::LOG_ALWAYS);

      //verify Application can continue living with a bad config file
      const std::string filename = "badconfig.xml";

      //write out bad config file
      WriteBadConfigFile(filename);

      dtCore::RefPtr<dtABC::Application> app;
      try
      {
         app = new dtABC::Application(filename);
      }
      catch (...)
      {
         CPPUNIT_FAIL("Application should not have thrown an exception when parsing a bad config file");
      }

      //delete config file
      dtUtil::FileUtils::GetInstance().FileDelete( filename );
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ApplicationTests::TestRemovingView()
   {
      dtCore::RefPtr<dtABC::Application> app = new dtTest::TestApp("config.xml", 0);
      dtCore::RefPtr<dtCore::View> view = new dtCore::View("testView");
      dtCore::ObserverPtr<dtCore::View> viewObserver = view.get();

      app->AddView(*view);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Application should contain the added View",
                                   true, app->ContainsView(*view));

      app->RemoveView(*view);

      //Application needs a frame to remove the view
      dtCore::System::GetInstance().Start();
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Application should not contain the removed View",
                                    false, app->ContainsView(*view));

      //remove local reference, should cause View to be destructed and the ObserverPtr to be invalid
      view = NULL;

      CPPUNIT_ASSERT_MESSAGE("The View did not get destroyed when removed from Application",
                              viewObserver.valid() == false);

      dtCore::System::GetInstance().Stop();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ApplicationTests::TestGettingView()
   {
      dtCore::RefPtr<dtABC::Application> app = new dtABC::Application();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Application should have one View by default",
                                   unsigned(1), app->GetNumberOfViews());

      dtCore::RefPtr<dtCore::View> view  = new dtCore::View();
      app->AddView(*view);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Application's View count didn't get updated with added View",
                                   unsigned(2), app->GetNumberOfViews());

      dtCore::RefPtr<dtCore::View> retView = app->GetView(1);
      CPPUNIT_ASSERT_MESSAGE("Application returned the wrong View",
                             view.get() == retView.get());

      CPPUNIT_ASSERT_MESSAGE("Application should not have returned a View with index out of bounds",
                             NULL == app->GetView(10));
   }

   //////////////////////////////////////////////////////////////////////////
   class ApplicationSetupTests : public CPPUNIT_NS::TestFixture
   {

      CPPUNIT_TEST_SUITE(ApplicationSetupTests);
      CPPUNIT_TEST(TestInit);
      CPPUNIT_TEST(TestReplaceScene);
      CPPUNIT_TEST(TestReplaceCamera);
      CPPUNIT_TEST_SUITE_END();

   public:
      ApplicationSetupTests() {};
      ~ApplicationSetupTests() {};

      void TestInit();
      void TestReplaceScene();
      void TestReplaceCamera();

   protected:

   private:
   };

   CPPUNIT_TEST_SUITE_REGISTRATION(ApplicationSetupTests);


   void ApplicationSetupTests::TestInit()
   {
      dtCore::RefPtr<dtABC::Application> app = new dtABC::Application();
      CPPUNIT_ASSERT_MESSAGE("No valid Scene", app->GetScene() != NULL);
      CPPUNIT_ASSERT_MESSAGE("No valid Camera", app->GetCamera() != NULL);
      CPPUNIT_ASSERT_MESSAGE("No valid Window", app->GetWindow() != NULL);
      CPPUNIT_ASSERT_MESSAGE("No valid View", app->GetView() != NULL);
   }

   void ApplicationSetupTests::TestReplaceScene()
   {
      dtCore::RefPtr<dtABC::Application> app = new dtABC::Application();
      dtCore::RefPtr<dtCore::Scene> newScene = new dtCore::Scene();

      app->SetScene(newScene.get());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("App didn't get the new Scene",
         newScene.get(), app->GetScene());

      //new scene should have ended up in the app's View
      CPPUNIT_ASSERT_EQUAL_MESSAGE("View didn't get the new Scene",
         newScene.get(), app->GetView()->GetScene());

      //verify the osg node's are the same too
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Scene nodes don't match",
         newScene->GetSceneNode(), app->GetView()->GetOsgViewerView()->getSceneData()->asGroup());
   }

   void ApplicationSetupTests::TestReplaceCamera()
   {
      dtCore::RefPtr<dtABC::Application> app = new dtABC::Application();
      dtCore::RefPtr<dtCore::Camera> newCam = new dtCore::Camera();

      app->SetCamera(newCam.get());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("App didn't get the new Camera",
         newCam.get(), app->GetCamera());

      //the new Camera should have ended up in the app's View
      CPPUNIT_ASSERT_EQUAL_MESSAGE("View didn't get the new Camera",
         newCam.get(), app->GetView()->GetCamera());

      //the new osgCamera should have ended up in the app's View
      CPPUNIT_ASSERT_EQUAL_MESSAGE("osgCamera's don't match",
         newCam->GetOSGCamera(), app->GetView()->GetOsgViewerView()->getCamera());
   }

}
