/* -*-c++-*-
 * allTests - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * @author Matthew W. Campbell
 * @author Curtiss Murphy
 */

#include <prefix/unittestprefix.h>
#include <dtGame/binarylogstream.h>
#include <dtGame/gamemanager.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/logcontroller.h>
#include <dtGame/gamemanager.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/sigslot.h>
#include <dtCore/timer.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtGame/loggermessages.h>
#include <dtGame/logstatus.h>
#include <dtGame/logtag.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/messagetype.h>
#include <dtGame/basemessages.h>
#include <dtGame/gameactor.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/actorupdatemessage.h>
#include <dtActors/gamemeshactor.h>
#include <dtActors/engineactorregistry.h>
#include <dtCore/actorfactory.h>
#include <dtCore/actorproxy.h>

#include <dtGame/testcomponent.h>

#include <dtABC/application.h>
extern dtABC::Application& GetGlobalApplication();

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
 * This class contains a suite of methods used to test the logging and playback
 * facilities of the GameManager ServerLoggerComponent, BinaryLogStream, and
 * LogController component.
 */
class GMLoggerTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(GMLoggerTests);
      CPPUNIT_TEST(TestBinaryLogStreamCreate);
      CPPUNIT_TEST(TestBinaryLogStreamOpen);
      CPPUNIT_TEST(TestBinaryLogStreamGetLogs);
      CPPUNIT_TEST(TestBinaryLogStreamDeleteLog);
      CPPUNIT_TEST(TestBinaryLogStreamReadWriteErrors);
      CPPUNIT_TEST(TestBinaryLogStreamReadWriteMessages);
      CPPUNIT_TEST(TestBinaryLogStreamTags);
      CPPUNIT_TEST(TestBinaryLogStreamKeyFrames);
      CPPUNIT_TEST(TestBinaryLogStreamTagsAndKeyFrames);
      CPPUNIT_TEST(TestBinaryLogStreamJumpToKeyFrame);
      CPPUNIT_TEST(TestPlaybackRecordCycle);
      CPPUNIT_TEST(TestLoggerMessages);
      CPPUNIT_TEST(TestLoggerKeyframeMessage);
      CPPUNIT_TEST(TestLoggerTagMessage);
      CPPUNIT_TEST(TestLoggerStatusMessage);
      CPPUNIT_TEST(TestLoggerKeyframeListMessage);
      CPPUNIT_TEST(TestLoggerTagListMessage);
      CPPUNIT_TEST(TestLoggerGetTags);
      CPPUNIT_TEST(TestLoggerGetKeyframes);
      CPPUNIT_TEST(TestLoggerAutoKeyframeInterval);
      CPPUNIT_TEST(TestLoggerActorIDLists);
      CPPUNIT_TEST(TestLogControllerComponent);
      CPPUNIT_TEST(TestControllerSignals);
      CPPUNIT_TEST(TestServerLogger);
      CPPUNIT_TEST(TestServerLogger2);
      CPPUNIT_TEST(TestAddRemoveIgnoredMessageTypeToLogger);
   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();

      void TestBinaryLogStreamCreate();
      void TestBinaryLogStreamOpen();
      void TestBinaryLogStreamReadWriteErrors();
      void TestBinaryLogStreamReadWriteMessages();
      void TestBinaryLogStreamTags();
      void TestBinaryLogStreamKeyFrames();
      void TestBinaryLogStreamTagsAndKeyFrames();
      void TestBinaryLogStreamJumpToKeyFrame();
      void TestBinaryLogStreamDeleteLog();
      void TestBinaryLogStreamGetLogs();
      void TestPlaybackRecordCycle();
      void TestLoggerMessages();
      void TestLoggerKeyframeListMessage();
      void TestLoggerTagListMessage();
      void TestLoggerGetTags();
      void TestLoggerKeyframeMessage();
      void TestLoggerTagMessage();
      void TestLoggerStatusMessage();
      void TestLoggerGetKeyframes();
      void TestLoggerAutoKeyframeInterval();
      void TestLoggerActorIDLists();
      void TestLogControllerComponent();
      void TestControllerSignals();
      void TestServerLogger();
      void TestServerLogger2();
      void TestAddRemoveIgnoredMessageTypeToLogger();

      void CompareKeyframeLists(const std::vector<dtGame::LogKeyframe> listOne,
         const std::vector<dtGame::LogKeyframe> listTwo);

      void CompareTagLists(const std::vector<dtGame::LogTag> listOne,
         const std::vector<dtGame::LogTag> listTwo);

   private:
      dtCore::RefPtr<dtGame::GameManager> mGameManager;

      dtGame::LogStatus status;
      dtGame::LogKeyframe keyframe;
      dtGame::LogTag tag;

      double d1;
      double d2;
      double d3;
      double d4;

      unsigned long long1;
      long long2;

      static const char* mTestGameActorLibrary;
      static const char* mTestActorLibrary;
};

//////////////////////////////////////////////////////////////////////////

/**
 * This class tests that signals are sent/received from the controller.
 */
class TestControllerSignal: public sigslot::has_slots<>, public osg::Referenced
{
   public:
      bool mStatusSignalReceived;
      bool mRejectionSignalReceived;
      dtGame::LogStatus mStatus;
      dtCore::RefPtr<const dtGame::Message> mRejectMessage;

      TestControllerSignal() { Reset(); }

      void OnReceivedStatus(const dtGame::LogStatus& newStatus)
      {
         mStatus = newStatus;
         mStatusSignalReceived = true;
      }
      void OnReceivedRejection(const dtGame::Message& newMessage)
      {
         mRejectMessage = &newMessage;
         mRejectionSignalReceived = true;
      }
      void RegisterSignals(dtGame::LogController& logController)
      {
         logController.SignalReceivedStatus().connect_slot(this, &TestControllerSignal::OnReceivedStatus);
         logController.SignalReceivedRejection().connect_slot(this, &TestControllerSignal::OnReceivedRejection);
      }
      void Reset()
      {
         mStatus = dtGame::LogStatus();
         mStatusSignalReceived = false;

         mRejectMessage = NULL;
         mRejectionSignalReceived = false;
      }
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
 * This class is a test stub for a logstream.
 */
class TestLogStream : public dtGame::LogStream
{
   public:
      TestLogStream(dtGame::MessageFactory& msgFactory, dtCore::RefPtr<dtGame::GameManager> newGameManager)
         : dtGame::LogStream(msgFactory)
      {
         mGameManager = newGameManager;
         mExceptionEnabled = false;
      }
      void Create(const std::string&, const std::string&) { DoException(); }
      void Close()
      {
         // Note, real close doesn't throw exception at the moment, but left in
         // because it's easier to test failure/reject code
         DoException();
      }
      void Open(const std::string& logsPath, const std::string& logResourceName) { DoException(); }
      void WriteMessage(const dtGame::Message& msg, double timeStamp) { DoException(); }
      dtCore::RefPtr<dtGame::Message> ReadMessage(double& timeStamp)
      {
         DoException();
         return mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::LOG_INFO_STATUS);
      }
      void InsertTag(dtGame::LogTag& newTag) { DoException(); }
      void InsertKeyFrame(dtGame::LogKeyframe& newKeyFrame) { DoException(); }
      void JumpToKeyFrame(const dtGame::LogKeyframe& keyFrame) { DoException(); }
      void GetTagIndex(std::vector<dtGame::LogTag>& tags) { DoException(); }
      void GetKeyFrameIndex(std::vector<dtGame::LogKeyframe>& keyFrames) { DoException(); }
      void Flush() { DoException(); }
      void GetAvailableLogs(const std::string& logsPath,
            std::vector<std::string>& logs) { }
      virtual void Delete(const std::string& logsPath, const std::string& logResourceName) { }

      bool mExceptionEnabled;
   protected:
      // allow for exception testing
      void DoException()
      {
         if (mExceptionEnabled)
         {
            throw dtGame::LogStreamIOException(
            "Test Exception - Not an Error!", __FILE__, __LINE__);
         }
      }
      virtual ~TestLogStream() { }
   private:
      dtCore::RefPtr<dtGame::GameManager> mGameManager;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GMLoggerTests);
const std::string LOGFILE = "testlog";
std::string GetTestsDir();

const char* GMLoggerTests::mTestGameActorLibrary="testGameActorLibrary";
const char* GMLoggerTests::mTestActorLibrary="testActorLibrary";

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::setUp()
{
   d1 = 99220.425;
   d2 = 600.001;
   d3 = 900.4;
   d4 = 444.444;

   long1 = 500;
   long2 = 99999;

   keyframe.SetName("myName");
   keyframe.SetDescription("myDescription");
   keyframe.SetSimTimeStamp(d1);

   dtGame::LogStatus::NameVector mapNames;
   mapNames.push_back("myMap");
   mapNames.push_back("myMap2");

   keyframe.SetActiveMaps(mapNames);
   keyframe.SetLogFileOffset(long2);
   keyframe.SetTagUniqueId(dtCore::UniqueId("taguniqueid"));

   tag.SetName("myName");
   tag.SetDescription("myDescription");
   tag.SetSimTimeStamp(d1);
   tag.SetKeyframeUniqueId(dtCore::UniqueId("keyframeuniqueid"));
   tag.SetCaptureKeyframe(false);

   status.SetStateEnum(dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
   status.SetCurrentSimTime(d1);
   status.SetAutoRecordKeyframeInterval(d2);
   status.SetCurrentRecordDuration(d4);
   status.SetNumMessages(long1);
   status.SetActiveMaps(mapNames);
   status.SetLogFile("myLogFile");

   try
   {
      mGameManager = new dtGame::GameManager(*GetGlobalApplication().GetScene());
      mGameManager->SetApplication(GetGlobalApplication());
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      dtCore::System::GetInstance().SetShutdownOnWindowClose(false);
      dtCore::System::GetInstance().Start();
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }

}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::tearDown()
{
   try
   {
      if (mGameManager.valid())
      {
         dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(mGameManager->GetMessageFactory());
         std::vector<std::string> logList;

         mGameManager->DeleteAllActors(true);
         mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
         mGameManager = NULL;

         stream->GetAvailableLogs(GetTestsDir(), logList);
         for (unsigned int i = 0; i < logList.size(); ++i)
         {
            stream->Delete(GetTestsDir(), logList[i]);
         }

         stream = NULL;

         dtCore::System::GetInstance().SetPause(false);
         dtCore::System::GetInstance().Stop();
      }
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamCreate()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);

   try
   {
      stream->Create(GetTestsDir(), LOGFILE);
      stream->Close();
      stream->Delete(GetTestsDir(), LOGFILE);
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamOpen()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);

   try
   {
      stream->Create(GetTestsDir(), LOGFILE);
      stream->Close();

      std::string testFileName = GetTestsDir() + dtUtil::FileUtils::PATH_SEPARATOR + LOGFILE;
      const dtUtil::FileInfo firstInfo = dtUtil::FileUtils::GetInstance().GetFileInfo(testFileName);

      dtCore::AppSleep(1500);

      stream->Open(GetTestsDir(),LOGFILE);
      stream->Close();

      const dtUtil::FileInfo secondInfo = dtUtil::FileUtils::GetInstance().GetFileInfo(testFileName);

      // make sure that the time didn't change on the file between creating and opening. This ensures
      // that the open/close didn't write to the file.
      CPPUNIT_ASSERT_MESSAGE("Time Shouldn't change just from opening and closing log stream.",
         firstInfo.lastModified == secondInfo.lastModified);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamDeleteLog()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = new dtGame::BinaryLogStream(msgFactory);

   try
   {
      std::vector<std::string> logList;

      stream->Create(GetTestsDir(), LOGFILE);
      stream->Create(GetTestsDir(), LOGFILE+"1");
      stream->Create(GetTestsDir(), LOGFILE+"2");
      stream->Create(GetTestsDir(), LOGFILE+"3");

      stream->GetAvailableLogs(GetTestsDir(),logList);
      CPPUNIT_ASSERT(logList.size() == 4);

      stream->Delete(GetTestsDir(), LOGFILE);
      stream->Delete(GetTestsDir(), LOGFILE+"1");
      stream->Delete(GetTestsDir(), LOGFILE+"2");
      stream->Delete(GetTestsDir(), LOGFILE+"3");

      stream->GetAvailableLogs(GetTestsDir(),logList);
      CPPUNIT_ASSERT(logList.size() == 0);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamGetLogs()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = new dtGame::BinaryLogStream(msgFactory);

   try
   {
      std::vector<std::string> logList;

      stream->Create(GetTestsDir(),LOGFILE);
      stream->Create(GetTestsDir(),LOGFILE+"1");
      stream->Create(GetTestsDir(),LOGFILE+"2");
      stream->Create(GetTestsDir(),LOGFILE+"3");

      CPPUNIT_ASSERT_THROW(stream->GetAvailableLogs("./testblahblah",logList),dtUtil::Exception);

      stream->GetAvailableLogs(GetTestsDir(),logList);
      CPPUNIT_ASSERT_EQUAL((size_t)4,logList.size());
      int count = 0;
      for (unsigned int i = 0; i < logList.size(); ++i)
      {
         if (logList[i] == LOGFILE || logList[i] == std::string(LOGFILE+"1") ||
            logList[i] == std::string(LOGFILE+"2") || logList[i] == std::string(LOGFILE+"3"))
         {
            count++;
         }
      }

      std::ostringstream ss;
      ss << (count-4) << " logs were not found in the returned log list.";
      CPPUNIT_ASSERT_MESSAGE(ss.str(),count == 4);
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamReadWriteErrors()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::TickMessage> tickMessage =
      (dtGame::TickMessage *)(msgFactory.CreateMessage(dtGame::MessageType::TICK_LOCAL)).get();

   //Now make sure the proper exceptions are thrown when writing to a
   //read only log stream.
   try
   {
      {
         dtCore::RefPtr<dtGame::BinaryLogStream> errorStream =
            new dtGame::BinaryLogStream(msgFactory);

         errorStream->Create(GetTestsDir(),LOGFILE);

         //First, open for read...
         errorStream->Open(GetTestsDir(),LOGFILE);

         //Now try writing to it.. this should throw an exception.
         CPPUNIT_ASSERT_THROW(errorStream->WriteMessage(*tickMessage.get(),100.0),
                              dtGame::LogStreamIOException);
      }

      {
         //Now make sure the proper exceptions are thrown when reading from a
         //write only log stream.
         dtCore::RefPtr<dtGame::BinaryLogStream> errorStream =
            new dtGame::BinaryLogStream(msgFactory);

         //First, open for writing.
         errorStream->Create(GetTestsDir(),LOGFILE);

         //Now try reading from it.. this should throw an exception.
         double timeStamp(100.0);
         CPPUNIT_ASSERT_THROW(errorStream->ReadMessage(timeStamp),
                              dtGame::LogStreamIOException);
      }
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamReadWriteMessages()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   dtCore::RefPtr<dtGame::TickMessage> tickMessage =
      (dtGame::TickMessage*)(msgFactory.CreateMessage(dtGame::MessageType::TICK_LOCAL)).get();
   int i;

   try
   {
      stream->Create(GetTestsDir(),LOGFILE);
      for (i = 0; i < 100; ++i)
      {
         tickMessage->SetDeltaSimTime(i*2.0f);
         tickMessage->SetDeltaRealTime(i*3.0f);
         tickMessage->SetSimTimeScale(i);
         tickMessage->SetSimulationTime((double)i);
         stream->WriteMessage(*tickMessage.get(),100.045);
      }
      stream->Close();

      stream->Open(GetTestsDir(),LOGFILE);
      for (i = 0; i < 100; ++i)
      {
         float t1 = i*2.0f;
         float t2 = i*3.0f;
         float t3 = (float)i;
         long  t4 = i;

         double timeStamp;
         dtCore::RefPtr<dtGame::TickMessage> msgToTest =
            (dtGame::TickMessage*)(stream->ReadMessage(timeStamp)).get();

         CPPUNIT_ASSERT_MESSAGE("Message types should be equal.",msgToTest->GetMessageType() == tickMessage->GetMessageType());
         CPPUNIT_ASSERT_MESSAGE("Message time stamp was not read correctly.",timeStamp == 100.045);
         CPPUNIT_ASSERT_MESSAGE("Delta Sim time was not read correctly.",
            msgToTest->GetDeltaSimTime() == t1);
         CPPUNIT_ASSERT_MESSAGE("Delta real time was not read correctly.",
            msgToTest->GetDeltaRealTime() == t2);
         CPPUNIT_ASSERT_MESSAGE("Delta time scale was not read correctly.",
            msgToTest->GetSimTimeScale() == t3);
         CPPUNIT_ASSERT_MESSAGE("Delta Sim time of day was not read correctly.",
            msgToTest->GetSimulationTime() == (double)t4);
      }
      stream->Close();
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamTags()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = new dtGame::BinaryLogStream(msgFactory);

   try
   {
      std::vector<dtGame::LogTag> tagList;
      dtGame::LogTag newTag,tag2;

      //First we create the file and do a little sanity check to make sure we have no tags.
      stream->Create(GetTestsDir(),LOGFILE);
      stream->GetTagIndex(tagList);
      CPPUNIT_ASSERT_MESSAGE("Newly created binary log files should not contain any tags",
         tagList.size() == 0);
      stream->Close();

      stream->Create(GetTestsDir(),LOGFILE);
      stream->GetTagIndex(tagList);
      CPPUNIT_ASSERT_MESSAGE("Tag list should have been empty.",tagList.size() == 0);

      //Now add a tag and make sure it is in the list of returned tags.
      newTag.SetName("bob");
      newTag.SetDescription("bob_desc");
      newTag.SetSimTimeStamp(10.001);
      newTag.SetKeyframeUniqueId(dtCore::UniqueId("kfuniqueid"));
      stream->InsertTag(newTag);
      stream->GetTagIndex(tagList);
      CPPUNIT_ASSERT_MESSAGE("Tag list should have a single tag.",tagList.size() == 1);
      tag2 = tagList[0];
      CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",tag2 == newTag);
      CPPUNIT_ASSERT_MESSAGE("Tag keyframe unique ids should be equal.",
         tag2.GetKeyframeUniqueId() == newTag.GetKeyframeUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Tag names should be equal.",tag2.GetName() == newTag.GetName());
      CPPUNIT_ASSERT_MESSAGE("Tag descriptions should be equal.",tag2.GetDescription() == newTag.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Tag simulation times should be equal.",tag2.GetSimTimeStamp() == newTag.GetSimTimeStamp());
      CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe flag should be equal.",
         tag2.GetCaptureKeyframe() == newTag.GetCaptureKeyframe());
      stream->Close();

      //Now we need to make sure the tag got saved and loaded from the index tables file properly.
      stream->Open(GetTestsDir(),LOGFILE);
      stream->GetTagIndex(tagList);
      CPPUNIT_ASSERT_MESSAGE("Tag list should have a single tag.",tagList.size() == 1);

      tag2 = tagList[0];
      CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",tag2 == newTag);
      CPPUNIT_ASSERT_MESSAGE("Tag keyframe unique ids should be equal.",
         tag2.GetKeyframeUniqueId() == newTag.GetKeyframeUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Tag names should be equal.",tag2.GetName() == newTag.GetName());
      CPPUNIT_ASSERT_MESSAGE("Tag descriptions should be equal.",tag2.GetDescription() == newTag.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Tag simulation times should be equal.",tag2.GetSimTimeStamp() == newTag.GetSimTimeStamp());
      CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe flag should be equal.",
         tag2.GetCaptureKeyframe() == newTag.GetCaptureKeyframe());

      //Now make sure we can read and write many many tags.
      int i;
      std::vector<dtGame::LogTag> tagsToCompare;
      newTag.SetName("bob");
      newTag.SetDescription("bob_desc_test");
      stream->Create(GetTestsDir(),LOGFILE);
      for (i = 0; i < 100; ++i)
      {
         newTag.SetUniqueId(dtCore::UniqueId());
         newTag.SetSimTimeStamp(i*100);
         newTag.SetKeyframeUniqueId(dtCore::UniqueId());
         if ((i%2) == 0)
         {
            newTag.SetCaptureKeyframe(true);
         }
         else
         {
            newTag.SetCaptureKeyframe(false);
         }

         stream->InsertTag(newTag);
         tagsToCompare.push_back(newTag);
      }
      stream->Close();

      stream->Open(GetTestsDir(),LOGFILE);
      stream->GetTagIndex(tagList);
      CPPUNIT_ASSERT_MESSAGE("Tag list should have a 100 tags.",tagList.size() == tagsToCompare.size());
      for (i = 0; i < 100; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal.",tagList[i] == tagsToCompare[i]);
         CPPUNIT_ASSERT_MESSAGE("Tag keyframe ids should be equal.",
            tagList[i].GetKeyframeUniqueId() == tagsToCompare[i].GetKeyframeUniqueId());
         CPPUNIT_ASSERT_MESSAGE("Tag names should be equal.",tagList[i].GetName() == tagsToCompare[i].GetName());
         CPPUNIT_ASSERT_MESSAGE("Tag descriptions should be equal.",tagList[i].GetDescription() == tagsToCompare[i].GetDescription());
         CPPUNIT_ASSERT_MESSAGE("Tag sim times should be equal.",tagList[i].GetSimTimeStamp() == tagsToCompare[i].GetSimTimeStamp());
         CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe flag should be equal.",tagList[i].GetCaptureKeyframe() ==
            tagsToCompare[i].GetCaptureKeyframe());
      }
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamKeyFrames()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);

   try
   {
      std::vector<dtGame::LogKeyframe> kfList;
      dtGame::LogKeyframe newFrame,frame2;

      //First we create the file and do a little sanity check to make sure we have no key frames.
      stream->Create(GetTestsDir(),LOGFILE);
      stream->GetKeyFrameIndex(kfList);
      CPPUNIT_ASSERT_MESSAGE("Newly created binary log files should not contain any keyframe entries.",
         kfList.size() == 0);
      stream->Close();

      stream->Create(GetTestsDir(),LOGFILE);
      stream->GetKeyFrameIndex(kfList);
      CPPUNIT_ASSERT_MESSAGE("Keyframe list should have been empty.",kfList.size() == 0);

      //Now add a key frames and make sure it is in the list of returned key frames.
      newFrame.SetName("bob");
      newFrame.SetDescription("bob_desc");
      newFrame.SetSimTimeStamp(10.001);

      dtGame::LogStatus::NameVector mapNames;
      mapNames.push_back("testmap");

      newFrame.SetActiveMaps(mapNames);
      newFrame.SetTagUniqueId(dtCore::UniqueId("tag"));
      stream->InsertKeyFrame(newFrame);
      stream->GetKeyFrameIndex(kfList);
      CPPUNIT_ASSERT_MESSAGE("Keyframe list should have a single key frame.",kfList.size() == 1);
      frame2 = kfList[0];
      CPPUNIT_ASSERT_MESSAGE("Keyframe unique ids should be equal",newFrame == frame2);
      CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",
         newFrame.GetTagUniqueId() == frame2.GetTagUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Keyframe names should be equal.",frame2.GetName() == newFrame.GetName());
      CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions should be equal.",frame2.GetDescription() == newFrame.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Keyframe simulation times should be equal.",frame2.GetSimTimeStamp() == newFrame.GetSimTimeStamp());
      CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",frame2.GetActiveMaps() == newFrame.GetActiveMaps());
      stream->Close();

      //Now we need to make sure the key frames got saved and loaded from the index tables file properly.
      stream->Open(GetTestsDir(),LOGFILE);
      stream->GetKeyFrameIndex(kfList);
      CPPUNIT_ASSERT_MESSAGE("Keyframe list should have a single key frame.",kfList.size() == 1);
      frame2 = kfList[0];
      CPPUNIT_ASSERT_MESSAGE("Keyframe unique ids should be equal",newFrame == frame2);
      CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",
         newFrame.GetTagUniqueId() == frame2.GetTagUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Keyframe names should be equal.",frame2.GetName() == newFrame.GetName());
      CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions should be equal.",frame2.GetDescription() == newFrame.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Keyframe simulation times should be equal.",frame2.GetSimTimeStamp() == newFrame.GetSimTimeStamp());
      CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",frame2.GetActiveMaps() == newFrame.GetActiveMaps());
      stream->Close();

      //Now make sure we can read and write many many key frames.
      int i;
      std::vector<dtGame::LogKeyframe> framesToCompare;
      newFrame.SetName("bob");
      newFrame.SetDescription("bob_desc");

      mapNames.clear();
      mapNames.push_back("activemap");

      newFrame.SetActiveMaps(mapNames);
      stream->Create(GetTestsDir(),LOGFILE);
      for (i = 0; i < 100; ++i)
      {
         newFrame.SetUniqueId(dtCore::UniqueId());
         newFrame.SetTagUniqueId(dtCore::UniqueId("tag:"+ dtUtil::ToString(i)));
         newFrame.SetSimTimeStamp(i*100);
         stream->InsertKeyFrame(newFrame);
         framesToCompare.push_back(newFrame);
      }
      stream->Close();

      stream->Open(GetTestsDir(),LOGFILE);
      stream->GetKeyFrameIndex(kfList);
      //CPPUNIT_ASSERT_MESSAGE("KeyFrame list should have a 100 key frames.",kfList.size() == framesToCompare.size());
      CPPUNIT_ASSERT_EQUAL_MESSAGE("KeyFrame list should have a 100 key frames.",kfList.size(), framesToCompare.size());
      for (i = 0; i < 100; ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("Keyframe unique ids should be equal",kfList[i] == framesToCompare[i]);
         CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",
            kfList[i].GetTagUniqueId() == framesToCompare[i].GetTagUniqueId());
         CPPUNIT_ASSERT_MESSAGE("Keyframe names should be equal.",kfList[i].GetName() == framesToCompare[i].GetName());
         CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions should be equal.",kfList[i].GetDescription() == framesToCompare[i].GetDescription());
         CPPUNIT_ASSERT_MESSAGE("Keyframe simulation times should be equal.",kfList[i].GetSimTimeStamp() == framesToCompare[i].GetSimTimeStamp());
         CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",kfList[i].GetActiveMaps() == framesToCompare[i].GetActiveMaps());
      }
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamTagsAndKeyFrames()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   unsigned int i;

   try
   {
      std::vector<dtGame::LogTag> tagList,tagCompareList;
      std::vector<dtGame::LogKeyframe> kfList,kfCompareList;

      //This test makes sure that interleaved keyframes and tags are correctly
      //stored and retreived from the log stream's index table.
      stream->Create(GetTestsDir(),LOGFILE);
      for (i = 0; i < 1000; ++i)
      {
         if ((i%2) == 0)
         {
            dtGame::LogTag tag;
            tag.SetUniqueId(dtCore::UniqueId());
            tag.SetName("bob");
            tag.SetDescription("bob_desc");
            tag.SetSimTimeStamp(i*10);
            tag.SetKeyframeUniqueId(dtCore::UniqueId("kfunique" +dtUtil::ToString(i)));
            stream->InsertTag(tag);
            tagCompareList.push_back(tag);
         }
         else
         {
            dtGame::LogKeyframe kf;
            kf.SetUniqueId(dtCore::UniqueId());
            kf.SetName("bobkf");
            kf.SetDescription("bobkf_desc");

            dtGame::LogStatus::NameVector mapNames;
            mapNames.push_back("bobtestmap");

            kf.SetActiveMaps(mapNames);
            kf.SetTagUniqueId(dtCore::UniqueId("tagunique" + dtUtil::ToString(i)));
            kf.SetSimTimeStamp(i*10);
            kf.SetLogFileOffset(i);
            stream->InsertKeyFrame(kf);
            kfCompareList.push_back(kf);
         }
      }

      stream->Close();

      //Now read it back and compare with the lists we just built.
      stream->Open(GetTestsDir(),LOGFILE);
      stream->GetTagIndex(tagList);
      stream->GetKeyFrameIndex(kfList);

      CPPUNIT_ASSERT_MESSAGE("Tag list from file not of correct size.",tagList.size() == tagCompareList.size());
      CPPUNIT_ASSERT_MESSAGE("Keyframe list from file not of correct size.",kfList.size() == kfCompareList.size());
      for (i = 0; i < kfList.size(); ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("Keyframe unique ids should be equal",kfList[i] == kfCompareList[i]);
         CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",
            kfList[i].GetTagUniqueId() == kfCompareList[i].GetTagUniqueId());
         CPPUNIT_ASSERT_MESSAGE("Keyframe names should be equal.",kfList[i].GetName() == kfCompareList[i].GetName());
         CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions should be equal.",kfList[i].GetDescription() == kfCompareList[i].GetDescription());
         CPPUNIT_ASSERT_MESSAGE("Keyframe simulation times should be equal.",kfList[i].GetSimTimeStamp() == kfCompareList[i].GetSimTimeStamp());
         CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",kfList[i].GetActiveMaps() == kfCompareList[i].GetActiveMaps());
      }
      for (i = 0; i < tagList.size(); ++i)
      {
         CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal.",tagList[i] == tagCompareList[i]);
         CPPUNIT_ASSERT_MESSAGE("Tag keyframe ids should be equal.",
            tagList[i].GetKeyframeUniqueId() == tagCompareList[i].GetKeyframeUniqueId());
         CPPUNIT_ASSERT_MESSAGE("Tag names should be equal.",tagList[i].GetName() == tagCompareList[i].GetName());
         CPPUNIT_ASSERT_MESSAGE("Tag descriptions should be equal.",tagList[i].GetDescription() == tagCompareList[i].GetDescription());
         CPPUNIT_ASSERT_MESSAGE("Tag sim times should be equal.",tagList[i].GetSimTimeStamp() == tagCompareList[i].GetSimTimeStamp());
         CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe flags should be equal.",
            tagList[i].GetCaptureKeyframe() == tagCompareList[i].GetCaptureKeyframe());
      }

      stream->Close();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamJumpToKeyFrame()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = new dtGame::BinaryLogStream(msgFactory);
   dtCore::RefPtr<dtGame::ActorUpdateMessage> actorUpdateMessage;
   msgFactory.CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED, actorUpdateMessage);
   unsigned int i;

   try
   {
      stream->Create(GetTestsDir(),LOGFILE);
      for (i = 0; i < 250; ++i)
      {
         std::ostringstream ss;
         ss << i;
         actorUpdateMessage->SetName("Jojo" + ss.str());
         actorUpdateMessage->SetActorTypeName("Bob" + ss.str());
         actorUpdateMessage->SetActorTypeCategory("Smith" + ss.str());

         actorUpdateMessage->AddUpdateParameter("Jed" + ss.str(),
            dtCore::DataType::STRING)->FromString("TestMe " + ss.str());

         actorUpdateMessage->AddUpdateParameter("Jobob" + ss.str(),
            dtCore::DataType::INT)->FromString(ss.str());

         //Insert a keyframe every several messages...
         //if ((i%25) == 0)
         //{
         //}

         dtGame::LogKeyframe keyFrame;
         keyFrame.SetName("bob");
         keyFrame.SetDescription("bob_desc");
         keyFrame.SetSimTimeStamp(i);
         stream->InsertKeyFrame(keyFrame);

         stream->WriteMessage(*actorUpdateMessage.get(),i);
      }

      stream->Close();
      stream->Open(GetTestsDir(),LOGFILE);

      std::vector<dtGame::LogKeyframe> kfList;
      stream->GetKeyFrameIndex(kfList);
      for (i = 0; i < kfList.size(); ++i)
      {
         stream->JumpToKeyFrame(kfList[i]);

         double timeStamp;
         dtCore::RefPtr<dtGame::Message> m = stream->ReadMessage(timeStamp).get();
         CPPUNIT_ASSERT(m.valid());

         dtCore::RefPtr<dtGame::ActorUpdateMessage> actorUpdateMessageRead = static_cast<dtGame::ActorUpdateMessage*>(m.get());

         std::ostringstream ss;
         ss << i;

         CPPUNIT_ASSERT_EQUAL(actorUpdateMessageRead->GetName(),"Jojo" + ss.str());
         CPPUNIT_ASSERT_EQUAL(actorUpdateMessageRead->GetActorTypeName(), "Bob" + ss.str());
         CPPUNIT_ASSERT_EQUAL(actorUpdateMessageRead->GetActorTypeCategory(), "Smith" + ss.str());

         CPPUNIT_ASSERT(actorUpdateMessageRead->GetUpdateParameter("Jed" + ss.str()) != NULL);
         CPPUNIT_ASSERT_EQUAL(actorUpdateMessageRead->GetUpdateParameter("Jed" + ss.str())->ToString(), "TestMe " + ss.str());

         CPPUNIT_ASSERT(actorUpdateMessageRead->GetUpdateParameter("Jobob" + ss.str()) != NULL);
         CPPUNIT_ASSERT_EQUAL(actorUpdateMessageRead->GetUpdateParameter("Jobob" + ss.str())->ToString(), ss.str());
      }

      stream->Close();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestPlaybackRecordCycle()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =
      new dtGame::BinaryLogStream(msgFactory);
   dtCore::RefPtr<dtGame::ServerLoggerComponent> serverController =
         new dtGame::ServerLoggerComponent(*stream);
   dtCore::RefPtr<dtGame::LogController> logController =
      new dtGame::LogController();

   //For this test, we will start recording a log file, create several
   //actors for recording purposes, and then attempt to playback those
   //actor create events.
   try
   {
      dtCore::RefPtr<dtCore::BaseActorObject> proxy = NULL;
      dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;

      mGameManager->AddComponent(*serverController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);
      dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent();
      dtCore::RefPtr<TestControllerSignal> testSignal = new TestControllerSignal();
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      testSignal->RegisterSignals(*logController);

      mGameManager->CreateActor("ExampleActors", "TestPlayer", gameProxy);
      CPPUNIT_ASSERT(gameProxy.valid());

      mGameManager->AddActor(*gameProxy,false,false);
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of game actors before changing to record.",
         size_t(1),mGameManager->GetNumGameActors());

      //First, we'll start recording and then add some actors thus generating
      //actor create messages.
      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      mGameManager->CreateActor("ExampleActors", "Test1Actor", gameProxy);
      mGameManager->AddActor(*gameProxy,false,false);

      mGameManager->CreateActor("ExampleActors", "Test1Actor", gameProxy);
      mGameManager->AddActor(*gameProxy,false,false);

      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of actors correctly added during record.",
         size_t(3),mGameManager->GetNumGameActors());

      testSignal->Reset();
      logController->RequestServerGetStatus();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Should be RECORD",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);

      logController->RequestChangeStateToIdle();
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Should be IDLE",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);

      unsigned int numGameActors = (unsigned int)serverController->GetPlaybackActorCount();
      CPPUNIT_ASSERT_MESSAGE("Number of playback actors before changing to playback should be 0",
         0 == numGameActors);

      tc->reset();
      logController->RequestChangeStateToPlayback();
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Should be Playback",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK);
      CPPUNIT_ASSERT_MESSAGE("We should have some number of messages", tc->GetReceivedProcessMessages().size() >= 3);

      //Go through the playback cycle until the end has been reached at
      //which point the system should be paused.
      while (!mGameManager->IsPaused())
      {
         dtCore::AppSleep(10);
         dtCore::System::GetInstance().Step();
      }

      dtCore::RefPtr<const dtGame::Message> procInfoEndOfMessages = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_INFO_PLAYBACK_END_OF_MESSAGES);
      CPPUNIT_ASSERT_MESSAGE("A LOG_INFO_PLAYBACK_END_OF_MESSAGES message should have been sent when all messages were done.", procInfoEndOfMessages != NULL);

      testSignal->Reset();
      logController->RequestServerGetStatus();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();

      //There should be three actors.  The test player and the two actors added
      //during playback.
      numGameActors = mGameManager->GetNumGameActors();
      CPPUNIT_ASSERT_MESSAGE("Number of game actors after concluding playback should be 3",
         (unsigned)3 == numGameActors);

      stream->Close();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerMessages()
{
   try
   {
      // delete log msg paramGetActiveMap() test
      dtCore::RefPtr<dtGame::LogDeleteLogfileMessage> deleteLogMsg = new dtGame::LogDeleteLogfileMessage();
      deleteLogMsg->SetLogFileName("myLogFile");
      CPPUNIT_ASSERT_MESSAGE("LogDeleteLogfileMessage should be able to set/get log file",
         deleteLogMsg->GetLogFileName() == "myLogFile");

      // set log file name msg param test
      dtCore::RefPtr<dtGame::LogSetLogfileMessage> setLogMsg = new dtGame::LogSetLogfileMessage();
      setLogMsg->SetLogFileName("myLogFile2");
      CPPUNIT_ASSERT_MESSAGE("LogSetLogfileMessage should be able to set/get log file",
         setLogMsg->GetLogFileName() == "myLogFile2");

      // set auto keyframe interval param test
      dtCore::RefPtr<dtGame::LogSetAutoKeyframeIntervalMessage> setKeyframeIntMsg = new dtGame::LogSetAutoKeyframeIntervalMessage();
      setKeyframeIntMsg->SetAutoKeyframeInterval(400.45);
      CPPUNIT_ASSERT_MESSAGE("LogSetAutoKeyframeIntervalMessage should be able to set/get auto keyframe interval",
         setKeyframeIntMsg->GetAutoKeyframeInterval() == 400.45);

      // test the end load keyframe message's params
      dtCore::RefPtr<dtGame::LogEndLoadKeyframeMessage> endKeyframeMsg = new dtGame::LogEndLoadKeyframeMessage();
      endKeyframeMsg->SetFailureReason("myBogusFailure");
      endKeyframeMsg->SetSuccessFlag(true);
      CPPUNIT_ASSERT_MESSAGE("LogEndLoadKeyframeMessage should be able to set/get failure reason",
         endKeyframeMsg->GetFailureReason() == "myBogusFailure");
      CPPUNIT_ASSERT_MESSAGE("LogSetAutoKeyframeIntervalMessage should be able to set/get success flag",
         endKeyframeMsg->GetSuccessFlag());

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerTagMessage()
{
   try
   {
      dtCore::RefPtr<dtGame::LogInsertTagMessage> insertTagMsg = new dtGame::LogInsertTagMessage();

      // test default 0 values

      dtGame::LogTag emptyTag;
      CPPUNIT_ASSERT(emptyTag.GetSimTimeStamp() == 0.0);

      // test basic get/set behavior on LogTag

      CPPUNIT_ASSERT_MESSAGE("LogTag should be able to set/get name", tag.GetName() == "myName");
      CPPUNIT_ASSERT_MESSAGE("LogTag should be able to set/get description", tag.GetDescription() == "myDescription");
      CPPUNIT_ASSERT_MESSAGE("LogTag should be able to set/get sim time stamp", osg::equivalent(d1, tag.GetSimTimeStamp(), 1e-2));

      // test copy constructor of tag

      dtGame::LogTag tag2(tag);
      CPPUNIT_ASSERT_MESSAGE("LogTag Copy Constructor should be able to set name", tag2.GetName() == "myName");
      CPPUNIT_ASSERT_MESSAGE("LogTag Copy Constructor should be able to set description", tag2.GetDescription() == "myDescription");
      CPPUNIT_ASSERT_MESSAGE("LogTag Copy Constructor should be able to set uniqueid", tag2.GetUniqueId() == tag.GetUniqueId());
      CPPUNIT_ASSERT_MESSAGE("LogTag Copy Constructor should be able to set sim time stamp", osg::equivalent(d1, tag2.GetSimTimeStamp(), 1e-2));

      // test assignment of tag

      dtGame::LogTag tag3;
      tag3 = tag2;
      CPPUNIT_ASSERT_MESSAGE("LogTag Assignment(=) should be able to set name", tag3.GetName() == "myName");
      CPPUNIT_ASSERT_MESSAGE("LogTag Assignment(=) should be able to set description", tag3.GetDescription() == "myDescription");
      CPPUNIT_ASSERT_MESSAGE("LogTag Assignment(=) should be able to set uniqueid", tag3.GetUniqueId() == tag2.GetUniqueId());
      CPPUNIT_ASSERT_MESSAGE("LogTag Assignment(=) should be able to set sim time stamp", osg::equivalent(d1, tag3.GetSimTimeStamp(), 1e-2));

      // test equality ---  <, >, !=, ==

      dtGame::LogTag tag4; // 4 will have a different unique id.
      CPPUNIT_ASSERT_MESSAGE("LogTag should support !=", tag2 != tag4);
      CPPUNIT_ASSERT_MESSAGE("LogTag should support ==", tag2 == tag3);
      CPPUNIT_ASSERT_MESSAGE("LogTag should support >", // can't check id's since they are generated, but can check the compare
         (tag4 > tag2) == (tag4.GetUniqueId() > tag2.GetUniqueId()));
      CPPUNIT_ASSERT_MESSAGE("LogTag should support <", // can't check id's since they are generated, but can check the compare
         (tag4 < tag2) == (tag4.GetUniqueId() < tag2.GetUniqueId()));

      // set tag on the message.  Get it back.  Should be the same values.

      insertTagMsg->SetTag(tag);
      dtGame::LogTag tag5 = insertTagMsg->GetTag();
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogTag on LogInsertTagMessage should have correct name",
         tag.GetName() == tag5.GetName());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogTag on LogInsertTagMessage should have correct description",
         tag.GetDescription() == tag5.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogTag on LogInsertTagMessage should have correct uniqueId",
         tag.GetUniqueId() == tag5.GetUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogTag on LogInsertTagMessage should have correct time stamp",
         osg::equivalent(tag.GetSimTimeStamp(), tag5.GetSimTimeStamp(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogTag on LogInsertTagMessage should have correct keyframe "
         "capture flag.",tag.GetCaptureKeyframe() == tag5.GetCaptureKeyframe());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogTag on LogInsertTagMessage should have correct keyframe "
         "unique id.",tag.GetKeyframeUniqueId() == tag5.GetKeyframeUniqueId());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerGetKeyframes()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = NULL;
   dtCore::RefPtr<dtGame::ServerLoggerComponent> serverLoggerComp = NULL;
   dtCore::RefPtr<dtGame::LogController> logController = NULL;
   dtCore::RefPtr<TestControllerSignal> rejectMsgSignal = NULL;

   try
   {
      dtCore::RefPtr<dtCore::BaseActorObject> proxy = NULL;
      dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;
      std::string errorMsg;
      std::vector<dtGame::LogKeyframe> kfList;

      stream = new dtGame::BinaryLogStream(msgFactory);
      serverLoggerComp = new dtGame::ServerLoggerComponent(*stream);
      logController = new dtGame::LogController();
      rejectMsgSignal = new TestControllerSignal();

      mGameManager->AddComponent(*serverLoggerComp, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);
      rejectMsgSignal->RegisterSignals(*logController);

      mGameManager->CreateActor("ExampleActors", "TestPlayer", gameProxy);
      mGameManager->AddActor(*gameProxy,false,false);

      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      logController->RequestChangeStateToRecord();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      //At this point, there should be one keyframe.
      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("Should have one keyframe.",kfList.size() == 1);

      if (rejectMsgSignal->mRejectMessage != NULL)
      {
         if (rejectMsgSignal->mRejectMessage->GetCausingMessage() != NULL)
         {
            rejectMsgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);
         }

         CPPUNIT_ASSERT_MESSAGE("Received error message from server before requesting "
            "first keyframe: " + errorMsg, rejectMsgSignal->mRejectMessage != NULL);
      }

      mGameManager->CreateActor("ExampleActors", "TestPlayer", gameProxy);
      mGameManager->AddActor(*gameProxy,false,false);
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      dtGame::LogKeyframe keyFrame;
      keyFrame.SetName("bob_keyframe");
      keyFrame.SetDescription("bob_description");

      dtGame::LogStatus::NameVector mapNames;
      mapNames.push_back("bob_map");

      keyFrame.SetActiveMaps(mapNames);
      logController->RequestCaptureKeyframe(keyFrame);
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      //Now there should be two keyframes.
      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("Should have two keyframes.", kfList.size() == 2);

      if (rejectMsgSignal->mRejectMessage != NULL)
      {
         if (rejectMsgSignal->mRejectMessage->GetCausingMessage() != NULL)
         {
            rejectMsgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);
         }

         CPPUNIT_ASSERT_MESSAGE("Received error message from server before requesting "
            "second keyframe: " + errorMsg, rejectMsgSignal->mRejectMessage != NULL);
      }

      logController->RequestChangeStateToIdle();
      mGameManager->DeleteAllActors();
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      mGameManager->CreateActor("ExampleActors", "TestPlayer", gameProxy);
      mGameManager->AddActor(*gameProxy,false,false);

      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      //Now go through a loop adding keyframe after keyframe...
      unsigned i;
      std::string loopCount;
      for (i = 0; i < 100; ++i)
      {
         loopCount = dtUtil::ToString(i);
         keyFrame.SetName("bob_keyframe" + loopCount);
         keyFrame.SetDescription("bob_description" + loopCount);

         mapNames = keyFrame.GetActiveMaps();
         mapNames.push_back("bob_map" + loopCount);

         keyFrame.SetActiveMaps(mapNames);

         logController->RequestCaptureKeyframe(keyFrame);
         dtCore::System::GetInstance().Step();
         dtCore::AppSleep(10);

         if (rejectMsgSignal->mRejectMessage != NULL)
         {
            if (rejectMsgSignal->mRejectMessage->GetCausingMessage() != NULL)
            {
               rejectMsgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);
            }

            CPPUNIT_ASSERT_MESSAGE("Received error message from server while generating keyframe " +
               loopCount + errorMsg, rejectMsgSignal->mRejectMessage != NULL);
         }
      }

      //Verify the long list of keyframes..
      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);
      kfList = logController->GetLastKnownKeyframeList();

      //Should be 101 because we just captured 100 keyframes plus the keyframe captured when
      //recording began.
      CPPUNIT_ASSERT_MESSAGE("Should have 101 keyframes.",kfList.size() == 101);

      //Check the keyframe data to make sure they all got sent around properly.
      for (i = 1; i < 101; ++i)
      {
         loopCount = dtUtil::ToString(i-1);
         dtGame::LogKeyframe &k = kfList[i];
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Names were not equal on keyframe " + loopCount,
            std::string("bob_keyframe" + loopCount), k.GetName());
         CPPUNIT_ASSERT_EQUAL_MESSAGE("Descriptions were not equal on keyframe " + loopCount,
            std::string("bob_description" + loopCount), k.GetDescription());
         CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be loopCount + 1 maps where loopCount == " + loopCount,
            size_t(i + 1), k.GetActiveMaps().size());

         CPPUNIT_ASSERT_EQUAL_MESSAGE("Active map names were not equal on keyframe " + loopCount,
            std::string("bob_map" + loopCount), k.GetActiveMaps()[i]);
      }

      stream->Close();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::CompareKeyframeLists(const std::vector<dtGame::LogKeyframe> listOne,
   const std::vector<dtGame::LogKeyframe> listTwo)
{
   std::vector<dtGame::LogKeyframe>::const_iterator itor,itor2;

   CPPUNIT_ASSERT_MESSAGE("Keyframe list sizes should be equal.",listOne.size() == listTwo.size());

   for (itor=listOne.begin(), itor2=listTwo.begin();
      itor!=listOne.end() && itor2!=listTwo.end(); ++itor, ++itor2)
   {
      const dtGame::LogKeyframe& first  = *itor;
      const dtGame::LogKeyframe& second = *itor2;

      CPPUNIT_ASSERT_MESSAGE("Keyframe names were not equal.  Should be: " +
         first.GetName() + " but was " + second.GetName(),first.GetName() == second.GetName());
      CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions were not equal.  Should be: " +
         first.GetDescription() + " but was " + second.GetDescription(),
         first.GetDescription() == second.GetDescription());

      CPPUNIT_ASSERT_EQUAL(first.GetActiveMaps().size(), second.GetActiveMaps().size());

      CPPUNIT_ASSERT_MESSAGE("Keyframe active maps were not equal. First map should be: " +
         first.GetActiveMaps()[0] + " but was " + second.GetActiveMaps()[0], first.GetActiveMaps() == second.GetActiveMaps());
      CPPUNIT_ASSERT_MESSAGE("Keyframe unique id's were not equal.  Should be: " +
         first.GetUniqueId().ToString() + " but was " + second.GetUniqueId().ToString(),
         first.GetUniqueId() == second.GetUniqueId());
       CPPUNIT_ASSERT_MESSAGE("Keyframe tag unique id's were not equal.  Should be: " +
         first.GetTagUniqueId().ToString() + " but was " + second.GetTagUniqueId().ToString(),
         first.GetTagUniqueId() == second.GetTagUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Keyframe sim time stamps were not equal.  Should be: " +
         dtUtil::ToString(first.GetSimTimeStamp()) + " but was " + dtUtil::ToString(second.GetSimTimeStamp()),
         first.GetSimTimeStamp() == second.GetSimTimeStamp());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerKeyframeListMessage()
{
   try
   {
      dtCore::RefPtr<dtGame::LogGetKeyframeListMessage> msg =
         new dtGame::LogGetKeyframeListMessage();

      std::vector<dtGame::LogKeyframe> testList;
      std::vector<dtGame::LogKeyframe> resultList;

      for (unsigned int i = 0; i < 10; ++i)
      {
         std::string num = dtUtil::ToString(i);
         dtGame::LogKeyframe kf;
         kf.SetName("FrameOne" + num);
         kf.SetDescription("Description One" + num);
         kf.SetSimTimeStamp(i*10.0f);
         kf.SetUniqueId(dtCore::UniqueId("UniqueID" + num));

         dtGame::LogKeyframe::NameVector mapNames;
         mapNames.push_back("ActiveMap" + num);
         kf.SetActiveMaps(mapNames);

         kf.SetTagUniqueId(dtCore::UniqueId("TagUniqueID" + num));
         testList.push_back(kf);
      }

      msg->SetKeyframeList(testList);
      resultList = msg->GetKeyframeList();

      CPPUNIT_ASSERT_MESSAGE("Keyframe list size should be 10.",resultList.size() == 10);
      CompareKeyframeLists(testList,resultList);

      dtCore::RefPtr<dtGame::LogGetKeyframeListMessage> otherMsg =
         new dtGame::LogGetKeyframeListMessage();

      std::string str;
      dtUtil::DataStream ds;

      msg->ToString(str);
      otherMsg->FromString(str);
      resultList = otherMsg->GetKeyframeList();
      CompareKeyframeLists(testList,resultList);

      otherMsg = new dtGame::LogGetKeyframeListMessage();
      msg->ToDataStream(ds);
      otherMsg->FromDataStream(ds);
      resultList = otherMsg->GetKeyframeList();
      CompareKeyframeLists(testList,resultList);
   }
   catch (const dtUtil::Exception& e)
   {

      CPPUNIT_FAIL(e.ToString());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
      throw;
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::CompareTagLists(const std::vector<dtGame::LogTag> listOne,
   const std::vector<dtGame::LogTag> listTwo)
{
   std::vector<dtGame::LogTag>::const_iterator itor, itor2;

   CPPUNIT_ASSERT_MESSAGE("Tag list sizes should be equal.",listOne.size() == listTwo.size());

   for (itor = listOne.begin(), itor2 = listTwo.begin();
      itor != listOne.end() && itor2 != listTwo.end(); ++itor, ++itor2)
   {
      const dtGame::LogTag& first  = *itor;
      const dtGame::LogTag& second = *itor2;

      CPPUNIT_ASSERT_MESSAGE("Tag names were not equal.  Should be: " +
         first.GetName() + " but was " + second.GetName(),first.GetName() == second.GetName());
      CPPUNIT_ASSERT_MESSAGE("Tag descriptions were not equal.  Should be: " +
         first.GetDescription() + " but was " + second.GetDescription(),
         first.GetDescription() == second.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe flags were not equal.  Should be: " +
         dtUtil::ToString(first.GetCaptureKeyframe()) + " but was " +
            dtUtil::ToString(second.GetCaptureKeyframe()),first.GetCaptureKeyframe() == second.GetCaptureKeyframe());
      CPPUNIT_ASSERT_MESSAGE("Tag unique id's were not equal.  Should be: " +
         first.GetUniqueId().ToString() + " but was " + second.GetUniqueId().ToString(),
         first.GetUniqueId() == second.GetUniqueId());
       CPPUNIT_ASSERT_MESSAGE("Tag keyframe unique id's were not equal.  Should be: " +
         first.GetKeyframeUniqueId().ToString() + " but was " + second.GetKeyframeUniqueId().ToString(),
         first.GetKeyframeUniqueId() == second.GetKeyframeUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Tag sim time stamps were not equal.  Should be: " +
         dtUtil::ToString(first.GetSimTimeStamp()) + " but was " + dtUtil::ToString(second.GetSimTimeStamp()),
         first.GetSimTimeStamp() == second.GetSimTimeStamp());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerTagListMessage()
{
   try
   {
      dtCore::RefPtr<dtGame::LogGetTagListMessage> msg =
         new dtGame::LogGetTagListMessage();

      std::vector<dtGame::LogTag> testList;
      std::vector<dtGame::LogTag> resultList;

      for (unsigned int i = 0; i < 10; ++i)
      {
         std::string num = dtUtil::ToString(i);
         dtGame::LogTag tag;
         tag.SetName("Tag" + num);
         tag.SetDescription("Description" + num);
         tag.SetSimTimeStamp(i*10.0f);
         tag.SetUniqueId(dtCore::UniqueId("UniqueID" + num));

         if ((i%2) == 0)
         {
            tag.SetCaptureKeyframe(true);
         }
         else
         {
            tag.SetCaptureKeyframe(false);
         }

         tag.SetKeyframeUniqueId(dtCore::UniqueId("KeyframeUniqueID" + num));
         testList.push_back(tag);
      }

      msg->SetTagList(testList);
      resultList = msg->GetTagList();

      CPPUNIT_ASSERT_MESSAGE("Tag list size should be 10.",resultList.size() == 10);
      CompareTagLists(testList,resultList);

      dtCore::RefPtr<dtGame::LogGetTagListMessage> otherMsg =
         new dtGame::LogGetTagListMessage();

      std::string str;
      dtUtil::DataStream ds;

      msg->ToString(str);
      otherMsg->FromString(str);
      resultList = otherMsg->GetTagList();
      CompareTagLists(testList,resultList);

      otherMsg = new dtGame::LogGetTagListMessage();
      msg->ToDataStream(ds);
      otherMsg->FromDataStream(ds);
      resultList = otherMsg->GetTagList();
      CompareTagLists(testList,resultList);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerGetTags()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtGame::BinaryLogStream* stream = NULL;
   dtGame::ServerLoggerComponent* serverLoggerComp = NULL;
   dtGame::LogController* logController = NULL;
   dtCore::RefPtr<TestControllerSignal> msgSignal = NULL;

   //This test actually tests both the get tags functionality and the insert
   //tag functionality.
   try
   {
      std::string errorMsg;
      std::vector<dtGame::LogKeyframe> kfList;
      std::vector<dtGame::LogTag> tagList;
      unsigned int i;

      stream = new dtGame::BinaryLogStream(msgFactory);
      serverLoggerComp = new dtGame::ServerLoggerComponent(*stream);
      logController = new dtGame::LogController();
      msgSignal = new TestControllerSignal();

      mGameManager->AddComponent(*serverLoggerComp, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);
      msgSignal->RegisterSignals(*logController);

      logController->RequestServerGetTags();
      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);
      tagList = logController->GetLastKnownTagList();
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("There should not be any tags in the log when first started.",
         tagList.empty() == true);
      CPPUNIT_ASSERT_MESSAGE("There should not be any keyframes in the log when first started.",
         kfList.empty() == true);

      logController->RequestChangeStateToRecord();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      double tagSimTimes[20];
      for (i = 0; i < 20; ++i)
      {
         dtGame::LogTag tag;
         std::string count = dtUtil::ToString(i);
         tag.SetName("Tag" + count);
         tag.SetDescription("TagDescription" + count);
         tag.SetUniqueId(dtCore::UniqueId("TagUniqueId" + count));
         if ((i%2) == 0)
         {
            tag.SetCaptureKeyframe(true);
         }
         else
         {
            tag.SetCaptureKeyframe(false);
         }

         logController->RequestInsertTag(tag);
         tagSimTimes[i] = mGameManager->GetSimulationTime();
         dtCore::System::GetInstance().Step();
         dtCore::AppSleep(10);
      }

      logController->RequestChangeStateToIdle();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      logController->RequestChangeStateToPlayback();
      dtCore::AppSleep(10);

      //Make sure tags and keyframes got recorded properly.
      logController->RequestServerGetTags();
      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);
      tagList = logController->GetLastKnownTagList();
      kfList  = logController->GetLastKnownKeyframeList();

      CPPUNIT_ASSERT_MESSAGE("There should be 20 tags in the log.", tagList.size() == 20);
      CPPUNIT_ASSERT_MESSAGE("There should be 11 keyframes in the log.", kfList.size() == 11);

      //Make sure we did not get any error messages during the keyframe capture.
      if (msgSignal->mRejectMessage != NULL)
      {
         if (msgSignal->mRejectMessage->GetCausingMessage() != NULL)
         {
            msgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);
         }

         CPPUNIT_ASSERT_MESSAGE("Received error message while auto capturing keyframes " +
            errorMsg, msgSignal->mRejectMessage != NULL);
      }

      //Well we seem to have gotten valid data, but lets make sure.
      for (i = 0; i < 20; ++i)
      {
         std::string count = dtUtil::ToString(i);

         CPPUNIT_ASSERT_MESSAGE("Tag name was not correct.",tagList[i].GetName() == std::string("Tag" + count));
         CPPUNIT_ASSERT_MESSAGE("Tag description was not correct.",
            tagList[i].GetDescription() == std::string("TagDescription" + count));

         CPPUNIT_ASSERT(osg::equivalent(tagSimTimes[i], tagList[i].GetSimTimeStamp(), 0.00001));
         CPPUNIT_ASSERT_MESSAGE("Tag unique id was not correct",
            tagList[i].GetUniqueId().ToString() == std::string("TagUniqueId" + count));
         if ((i%2) == 0)
         {
            CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe was not correct.",
               tagList[i].GetCaptureKeyframe() == true);
         }
         else
         {
            CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe was not correct.",
               tagList[i].GetCaptureKeyframe() == false);
         }
      }

      //Make sure the keyframes/tags cross references were assigned correctly.
      int kfCount = 1;
      for (i = 0; i < 20; i += 2)
      {
         CPPUNIT_ASSERT_MESSAGE("Tag " + dtUtil::ToString(i) + " did not have the correct keyframe id.",
            tagList[i].GetKeyframeUniqueId() == kfList[kfCount].GetUniqueId());

         CPPUNIT_ASSERT_MESSAGE("Keyframe + " + dtUtil::ToString(i) + " did not have the correct tag id.",
            kfList[kfCount].GetTagUniqueId() == tagList[i].GetUniqueId());

         ++kfCount;
      }

      stream->Close();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerAutoKeyframeInterval()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = NULL;
   dtCore::RefPtr<dtGame::ServerLoggerComponent> serverLoggerComp = NULL;
   dtCore::RefPtr<dtGame::LogController> logController = NULL;
   dtCore::RefPtr<TestControllerSignal> msgSignal = NULL;

   try
   {
      dtCore::RefPtr<dtCore::BaseActorObject> proxy = NULL;
      dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;
      std::string errorMsg;
      std::vector<dtGame::LogKeyframe> kfList;

      stream = new dtGame::BinaryLogStream(msgFactory);
      serverLoggerComp = new dtGame::ServerLoggerComponent(*stream);
      logController = new dtGame::LogController();
      msgSignal = new TestControllerSignal();

      mGameManager->AddComponent(*serverLoggerComp, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);
      msgSignal->RegisterSignals(*logController);

      logController->RequestChangeStateToIdle();
      dtCore::System::GetInstance().Step();

      //First make sure the user cannot set the interval to negative time..
      msgSignal->Reset();
      logController->RequestSetAutoKeyframeInterval(-10.0);
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Should not be allowed to set a negative auto keyframe interval",
         msgSignal->mRejectionSignalReceived);

      msgSignal->Reset();
      logController->RequestSetAutoKeyframeInterval(0.25f);
      dtCore::System::GetInstance().Step();

      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(300);
      dtCore::System::GetInstance().Step();

      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("Should have one keyframe.",kfList.size() == 1);

      double startTime = mGameManager->GetSimulationTime();
      double currTime=startTime;
      while (currTime <= (startTime+1.0))
      {
         dtCore::System::GetInstance().Step();
         currTime = mGameManager->GetSimulationTime();

         //Make sure we did not get any error messages during the keyframe capture.
         if (msgSignal->mRejectMessage != NULL)
         {
            if (msgSignal->mRejectMessage->GetCausingMessage() != NULL)
            {
               msgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);
            }

            CPPUNIT_ASSERT_MESSAGE("Received error message while auto capturing keyframes " +
               errorMsg, msgSignal->mRejectMessage != NULL);
         }
      }

      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      kfList = logController->GetLastKnownKeyframeList();

      //Should have captured 4ish keyframes (0.25 seconds interval over 1 second) plus
      //the keyframe captured at the start of record.
      CPPUNIT_ASSERT(kfList.size() == 5 || kfList.size() == 4);

      logController->RequestChangeStateToIdle();
      dtCore::System::GetInstance().Step();

      logController->RequestChangeStateToPlayback();
      dtCore::System::GetInstance().Step();

      //for (int i = 0; i < 500; ++i)
      //   dtCore::System::GetInstance().Step();

      if (msgSignal->mRejectMessage != NULL)
      {
         if (msgSignal->mRejectMessage->GetCausingMessage() != NULL)
         {
            msgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);
         }

         CPPUNIT_ASSERT_MESSAGE("Received error message while auto capturing keyframes " +
            errorMsg, msgSignal->mRejectMessage != NULL);
      }

      stream->Close();
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerKeyframeMessage()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = NULL;
   dtCore::RefPtr<dtGame::ServerLoggerComponent> serverLoggerComp = NULL;
   dtCore::RefPtr<dtGame::LogController> logController = NULL;

   try
   {
      dtCore::RefPtr<dtGame::LogCaptureKeyframeMessage> captureKeyframeMsg = new dtGame::LogCaptureKeyframeMessage();

      stream = new dtGame::BinaryLogStream(msgFactory);
      serverLoggerComp = new dtGame::ServerLoggerComponent(*stream);
      logController = new dtGame::LogController();

      mGameManager->AddComponent(*serverLoggerComp, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);

      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(10);
      dtCore::System::GetInstance().Step();

      dtGame::LogKeyframe kf;
      kf.SetName("test");
      kf.SetDescription("testd");
      logController->RequestCaptureKeyframe(kf);
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      logController->RequestServerGetKeyframes();
      dtCore::System::GetInstance().Step();
      dtCore::AppSleep(10);

      std::vector<dtGame::LogKeyframe> kfList;
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_EQUAL((size_t)2,kfList.size());

      dtGame::LogKeyframe emptyKeyframe;
      CPPUNIT_ASSERT_EQUAL(0.0,emptyKeyframe.GetSimTimeStamp());
      CPPUNIT_ASSERT_EQUAL((long)0,emptyKeyframe.GetLogFileOffset());

      // test basic set/get behavior on keyframe
      CPPUNIT_ASSERT_EQUAL(std::string("myName"),keyframe.GetName());
      CPPUNIT_ASSERT_EQUAL(std::string("myDescription"),keyframe.GetDescription());
      CPPUNIT_ASSERT(osg::equivalent(d1, keyframe.GetSimTimeStamp(), 1e-2));
      CPPUNIT_ASSERT_EQUAL(2U, unsigned(keyframe.GetActiveMaps().size()));
      CPPUNIT_ASSERT_EQUAL(std::string("myMap"), keyframe.GetActiveMaps()[0]);
      CPPUNIT_ASSERT_EQUAL(std::string("myMap2"), keyframe.GetActiveMaps()[1]);
      CPPUNIT_ASSERT_EQUAL(long2,keyframe.GetLogFileOffset());

      // test equality ---  <, >, !=, ==
      dtGame::LogKeyframe keyframe4; // 4 will have a different unique id.
      CPPUNIT_ASSERT_MESSAGE("Keyframe should support !=", keyframe != keyframe4);
      CPPUNIT_ASSERT_MESSAGE("Keyframe should support >", // can't check id's since they are generated, but can check the compare
         (keyframe4 > keyframe) == (keyframe4.GetUniqueId() > keyframe.GetUniqueId()));
      CPPUNIT_ASSERT_MESSAGE("Keyframe should support <", // can't check id's since they are generated, but can check the compare
         (keyframe < keyframe4) == (keyframe.GetUniqueId() < keyframe4.GetUniqueId()));

      // set keyframe on the message.  Get it back.  Should be the same values.
      captureKeyframeMsg->SetKeyframe(keyframe);
      dtGame::LogKeyframe keyframe5 = captureKeyframeMsg->GetKeyframe();
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct name",
         keyframe.GetName() == keyframe5.GetName());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct description",
         keyframe.GetDescription() == keyframe5.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct uniqueId",
         keyframe.GetUniqueId() == keyframe5.GetUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct time stamp",
         osg::equivalent(keyframe.GetSimTimeStamp(), keyframe5.GetSimTimeStamp(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct map",
         keyframe.GetActiveMaps() == keyframe5.GetActiveMaps());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct tag "
         "uniqueid.",keyframe.GetTagUniqueId() == keyframe5.GetTagUniqueId());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.ToString());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerStatusMessage()
{
   try
   {
      dtCore::RefPtr<dtGame::LogStatusMessage> infoStatusMsg = new dtGame::LogStatusMessage();

      // Test default 0 values in LogStatus class

      dtGame::LogStatus emptyStatus;
      CPPUNIT_ASSERT(emptyStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);
      CPPUNIT_ASSERT(emptyStatus.GetCurrentSimTime() == 0.0);
      CPPUNIT_ASSERT(emptyStatus.GetAutoRecordKeyframeInterval() == 0.0);
      CPPUNIT_ASSERT(emptyStatus.GetCurrentRecordDuration() == 0.0);
      CPPUNIT_ASSERT(emptyStatus.GetNumMessages() == 0);

      // test basic set/gets

      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get state enum", status.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get sim time", osg::equivalent(d1, status.GetCurrentSimTime(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get keyframe interval", osg::equivalent(d2, status.GetAutoRecordKeyframeInterval(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get record duration", osg::equivalent(d4, status.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get num recorded messages", status.GetNumMessages() == long1);
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get active map", status.GetActiveMaps()[0] == "myMap");
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get active map", status.GetActiveMaps()[1] == "myMap2");
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get log file", status.GetLogFile() == "myLogFile");

      // test copy constructor of status

      dtGame::LogStatus status2(status);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set state enum", status2.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set sim time", osg::equivalent(d1, status2.GetCurrentSimTime(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set keyframe interval", osg::equivalent(d2, status2.GetAutoRecordKeyframeInterval(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set record duration", osg::equivalent(d4, status2.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set num recorded messages", status2.GetNumMessages() == long1);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set active maps", status2.GetActiveMaps()[0] == "myMap");
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set active maps", status2.GetActiveMaps()[1] == "myMap2");
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set log file", status2.GetLogFile() == "myLogFile");

      // test assignment of status

      dtGame::LogStatus status3;
      status3 = status2;
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set state enum", status3.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set sim time", osg::equivalent(d1, status3.GetCurrentSimTime(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set keyframe interval", osg::equivalent(d2, status3.GetAutoRecordKeyframeInterval(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set record duration", osg::equivalent(d4, status3.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set num recorded messages", status3.GetNumMessages() == long1);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set active maps", status3.GetActiveMaps()[0] == "myMap");
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set active maps", status3.GetActiveMaps()[1] == "myMap2");
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set log file", status3.GetLogFile() == "myLogFile");

      // set the status on the message.  Get it back. should be same as what we set.

      infoStatusMsg->SetStatus(status);
      dtGame::LogStatus status4 = infoStatusMsg->GetStatus();
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct state enum",
         status4.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct sim time",
         osg::equivalent(status.GetCurrentSimTime(), status4.GetCurrentSimTime(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct keyframe interval",
         osg::equivalent(status.GetAutoRecordKeyframeInterval(), status4.GetAutoRecordKeyframeInterval(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct record duration",
         osg::equivalent(status.GetCurrentRecordDuration(), status4.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct num recorded messages",
         status.GetNumMessages() == status4.GetNumMessages());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct active maps",
         status.GetActiveMaps() == status4.GetActiveMaps());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct log file",
         status.GetLogFile() == status4.GetLogFile());
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
class TestServerLoggerComponent : public dtGame::ServerLoggerComponent
{
public:
   TestServerLoggerComponent(dtGame::LogStream& logStream);

   int RequestDeletePlaybackActors();

protected:
   virtual ~TestServerLoggerComponent() {}

};

TestServerLoggerComponent::TestServerLoggerComponent(dtGame::LogStream& logStream)
   : ServerLoggerComponent(logStream)
{
}

int TestServerLoggerComponent::RequestDeletePlaybackActors()
{
   return ServerLoggerComponent::RequestDeletePlaybackActors();
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerActorIDLists()
{
   dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = new dtGame::BinaryLogStream(msgFactory);
   dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
   dtCore::RefPtr<TestServerLoggerComponent> serverLoggerComp = new TestServerLoggerComponent(*stream);
   dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent();
   dtCore::RefPtr<TestControllerSignal> msgSignal = new TestControllerSignal();

   mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::HIGHEST);
   mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
   mGameManager->AddComponent(*serverLoggerComp, dtGame::GameManager::ComponentPriority::NORMAL);
   mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);

   msgSignal->RegisterSignals(*logController);

   dtCore::RefPtr<dtGame::GameActorProxy> proxy1;
   mGameManager->CreateActor("ExampleActors", "Test1Actor", proxy1);
   dtCore::RefPtr<dtGame::GameActorProxy> proxy2;
   mGameManager->CreateActor("ExampleActors", "Test1Actor", proxy2);
   dtCore::RefPtr<dtGame::GameActorProxy> proxy3;
   mGameManager->CreateActor("ExampleActors", "Test1Actor", proxy3);
   dtCore::RefPtr<dtGame::GameActorProxy> proxy4;
   mGameManager->CreateActor("ExampleActors", "Test1Actor", proxy4);
   dtCore::RefPtr<dtGame::GameActorProxy> proxy5;
   mGameManager->CreateActor("ExampleActors", "Test1Actor", proxy5);

   // Test Add Ignore
   logController->RequestAddIgnoredActor(proxy1->GetId());
   logController->RequestAddIgnoredActor(proxy2->GetId());
   dtCore::AppSleep(20); dtCore::System::GetInstance().Step();

   // Get Ignore Count
   CPPUNIT_ASSERT_MESSAGE("Processed LogController add 2 ignored actors, there should be 2 unique actor IDs",
      serverLoggerComp->GetIgnoredActorCount() == 2 );

   // Add all actor IDs
   logController->RequestAddIgnoredActor(proxy1->GetId());
   logController->RequestAddIgnoredActor(proxy2->GetId());
   logController->RequestAddIgnoredActor(proxy3->GetId());
   logController->RequestAddIgnoredActor(proxy4->GetId());
   logController->RequestAddIgnoredActor(proxy5->GetId());
   dtCore::AppSleep(50); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Processed LogController add 5 ignore actor IDs (2 re-added), there should be 5 unique actor IDs",
      serverLoggerComp->GetIgnoredActorCount() == 5 );

   // Remove 2 actor IDs
   logController->RequestRemoveIgnoredActor(proxy1->GetId());
   logController->RequestRemoveIgnoredActor(proxy2->GetId());
   dtCore::AppSleep(20); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Processed LogController remove 2 ignore actor IDs (1 & 2), there should be 3 unique actor IDs",
      serverLoggerComp->GetIgnoredActorCount() == 3 );

   // Test to be sure IDs 1 and 2 have been removed
   CPPUNIT_ASSERT_MESSAGE("Processed ServerLoggerComponent.IsIgnoredActorID; IDs 1 & 2 should NOT be found",
      (serverLoggerComp->IsIgnoredActorId(proxy1->GetId()) == false
      && serverLoggerComp->IsIgnoredActorId(proxy2->GetId()) == false) );
   CPPUNIT_ASSERT_MESSAGE("Processed ServerLoggerComponent.IsIgnoredActorID; IDs 3 & 4 & 5 should be found",
      (serverLoggerComp->IsIgnoredActorId(proxy3->GetId())
      && serverLoggerComp->IsIgnoredActorId(proxy4->GetId())
      && serverLoggerComp->IsIgnoredActorId(proxy5->GetId()) ) );


   // Change State to RECORD
   msgSignal->Reset();
   //logController->RequestSetAutoKeyframeInterval(0.1f);

   logController->RequestChangeStateToRecord();
   logController->RequestServerGetStatus();
   dtCore::AppSleep(50); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE( "ServerLoggerComponent should be in RECORD state.",
      logController->GetLastKnownStatus().GetStateEnum()
      == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD );

   tc->reset(); // flush messages from previous steps

   double newTime    = 0.0;
   double timeFactor = 1.0;


   dtGame::LogKeyframe curKeyframe;
   dtGame::LogKeyframe keyframe1;
   dtGame::LogKeyframe keyframe2;
   dtGame::LogKeyframe keyframe3;
   dtGame::LogKeyframe keyframe4;


   // Add actors 1 & 2 into recording (they should not be ignored)
   mGameManager->ChangeTimeSettings(newTime, timeFactor, mGameManager->GetSimulationClockTime());
   dtCore::AppSleep(5); dtCore::System::GetInstance().Step();
   mGameManager->AddActor(*proxy1,false,false); // KF1
   keyframe1.SetName("Frame 1");
   keyframe1.SetSimTimeStamp(newTime);
   logController->RequestCaptureKeyframe( keyframe1 );
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();

   // Wait and snap some keyframes and add more actors
   mGameManager->ChangeTimeSettings((newTime+=1.0), timeFactor, mGameManager->GetSimulationClockTime());
   mGameManager->AddActor(*proxy3,false,false); // KF2
   keyframe2.SetName("Frame 2");
   keyframe2.SetSimTimeStamp(newTime);
   logController->RequestCaptureKeyframe( keyframe2 );
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();

   mGameManager->ChangeTimeSettings((newTime+=1.0), timeFactor, mGameManager->GetSimulationClockTime());
   mGameManager->AddActor(*proxy2,false,false); // KF3
   keyframe3.SetName("Frame 3");
   keyframe3.SetSimTimeStamp(newTime);
   logController->RequestCaptureKeyframe( keyframe3 );
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();

   mGameManager->ChangeTimeSettings((newTime+=1.0), timeFactor, mGameManager->GetSimulationClockTime());
   mGameManager->AddActor(*proxy4,false,false); // KF4
   keyframe4.SetName("Frame 4");
   keyframe4.SetSimTimeStamp(newTime);
   logController->RequestCaptureKeyframe( keyframe4 );
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();


   // Stop RECORD and switch to IDLE
   logController->RequestChangeStateToIdle();
   logController->RequestServerGetStatus();
   dtCore::AppSleep(50); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE( "ServerLoggerComponent should be in IDLE state.",
      logController->GetLastKnownStatus().GetStateEnum()
      == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE );


   // Switch to PLAYBACK
   tc->reset();
   logController->RequestChangeStateToPlayback();
   logController->RequestServerGetStatus();
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE( "ServerLoggerComponent should be in PLAYBACK state.",
      logController->GetLastKnownStatus().GetStateEnum()
      == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK );

   logController->RequestServerGetKeyframes();
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   const std::vector<dtGame::LogKeyframe>& keyframes = logController->GetLastKnownKeyframeList();
   int keyframeCount = keyframes.size();
   CPPUNIT_ASSERT_MESSAGE( "LogController should have at least 4 keyframes", keyframeCount >= 4 );


   //NOTE: commented out due to random failures.  Will have to tease apart
   //this unit test to figure out where the problem really is.

   //// Test the start frame for the existence of actors
   //// Should not be any playback actors.  If there are, try to figure out which
   //// ones by getting their names.
   //std::string names;
   //if (serverLoggerComp->GetPlaybackActorCount() > 0)
   //{
   //   std::vector<dtCore::UniqueId> ids;
   //   serverLoggerComp->GetPlaybackActorIds(ids);
   //   std::vector<dtCore::UniqueId>::const_iterator itr = ids.begin();
   //   while (itr != ids.end())
   //   {
   //      dtCore::BaseActorObject *prox = mGameManager->FindActorById(*itr);
   //      if (prox != NULL)
   //      {
   //         names += " " + prox->GetClassName();
   //      }
   //      ++itr;
   //   }
   //}

   ////CPPUNIT_ASSERT_EQUAL_MESSAGE("Started playback, 0 actor IDs should exist in playback.,
   ////                              0, serverLoggerComp->GetPlaybackActorCount() );
   //CPPUNIT_ASSERT_EQUAL_MESSAGE("Started playback, no actor types should exist in playback.",
   //                              std::string(), names);

   // Check keyframe jumps...
   // --- Forward (1.0 to KF1 - proxy1)
   logController->RequestJumpToKeyframe(keyframes[1]);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Moved to keyframe 1, 1 playback actor ID should exist in playback",
      serverLoggerComp->GetPlaybackActorCount() == 1);

   // --- Forward (1.0 to KF2 - proxy3)
   logController->RequestJumpToKeyframe(keyframes[2]);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Moved to Third keyframe, 1 playback actor ID should exist in playback (ignored actor was added here)",
      serverLoggerComp->GetPlaybackActorCount() == 1);

   // --- Forward (1.0 to KF3 - proxy2)
   logController->RequestJumpToKeyframe(keyframes[3]);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Moved to Fourth keyframe, 2 playback actor IDs should exist in playback",
      serverLoggerComp->GetPlaybackActorCount() == 2);

   // --- Check that an ignore actor does not interfere with playback
   mGameManager->AddActor(*proxy5,false,false);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Added an ignore actor, only 2 playback actor IDs should still exist in playback",
      serverLoggerComp->GetPlaybackActorCount() == 2);

   // --- Forward (1.0 to KF4 - proxy4 - Last Keyframe)
   logController->RequestJumpToKeyframe(keyframes[4]);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Moved to Last keyframe, 2 playback actor IDs should exist in playback",
      serverLoggerComp->GetPlaybackActorCount() == 2);

   // --- Back (3.0 to KF1 - proxy1)
   logController->RequestJumpToKeyframe(keyframes[1]);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Moved to Second keyframe, 1 playback actor ID should exist in playback",
      serverLoggerComp->GetPlaybackActorCount() == 1);

   // --- Back (3.0 to KF0 - proxy1)
   logController->RequestJumpToKeyframe(keyframes[0]);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Moved to First keyframe, 0 playback actor IDs should exist in playback",
      serverLoggerComp->GetPlaybackActorCount() == 0);

   // --- Forward (1.0 to KF4 - proxy4 - Last Keyframe)
   logController->RequestJumpToKeyframe(keyframes[4]);
   dtCore::AppSleep(10); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Moved to Last keyframe, 2 playback actor IDs should exist in playback",
      serverLoggerComp->GetPlaybackActorCount() == 2);

   // Count total game actors
   int totalActors = mGameManager->GetNumGameActors();
   CPPUNIT_ASSERT_MESSAGE("Total game actors should be 5 at the end of playback.", totalActors == 5);

   // Stop RECORD and switch to IDLE
   logController->RequestChangeStateToIdle();
   logController->RequestServerGetStatus();
   dtCore::AppSleep(50); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("ServerLoggerComponent should be in IDLE state.",
      logController->GetLastKnownStatus().GetStateEnum()
      == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);

   // Count game actors after transition to IDLE state
   totalActors = mGameManager->GetNumGameActors();
   CPPUNIT_ASSERT_MESSAGE("Transition to IDLE state should leave 3 ignore actors after deleting the 2 playback actors",
      totalActors == 3);

   // Check playback IDs are cleared
   CPPUNIT_ASSERT_MESSAGE("Transition to IDLE state should have cleared the playback list; no playback actor IDs should exist",
      serverLoggerComp->GetPlaybackActorCount() == 0);

   // Check ignored IDs still exist
   CPPUNIT_ASSERT_MESSAGE("Playback ended, there should still be 3 ignored actor IDs",
      serverLoggerComp->GetIgnoredActorCount() == 3);



   // Check messages to make sure ignored ID have not been recorded
   std::vector< dtCore::RefPtr<const dtGame::Message> >& messages = tc->GetReceivedProcessMessages();
   dtCore::UniqueId curId;
   int limit = messages.size();
   bool ignored = true;
   for (int msg = 0; msg < limit; ++msg)
   {
      if (messages[msg].valid())
      {
         curId = messages[msg]->GetAboutActorId();
         if (curId == proxy3->GetId()
            || curId == proxy4->GetId())
         {
            ignored = false; break;
         }
      }
   }
   CPPUNIT_ASSERT_MESSAGE("Done with playback, actors should have been ignored", ignored);

   // Clear ignored IDs
   logController->RequestClearIgnoreList();
   dtCore::AppSleep(50); dtCore::System::GetInstance().Step();
   CPPUNIT_ASSERT_MESSAGE("Processed LogController clear ignore actor IDs, there should be 0 ignored actor IDs",
      serverLoggerComp->GetIgnoredActorCount() == 0);

   // Cleanup
   proxy1 = NULL;
   proxy2 = NULL;
   proxy3 = NULL;
   proxy4 = NULL;
   proxy5 = NULL;
   tc->reset();

}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLogControllerComponent()
{
   try
   {
      dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent();
      dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);

      logController->RequestCaptureKeyframe(keyframe);
      logController->RequestChangeStateToIdle();
      logController->RequestChangeStateToPlayback();
      logController->RequestChangeStateToRecord();
      logController->RequestDeleteLogFile("myLogFile");
      logController->RequestInsertTag(tag);
      logController->RequestServerGetKeyframes();
      logController->RequestServerGetLogfiles();
      logController->RequestServerGetStatus();
      logController->RequestServerGetTags();
      logController->RequestSetAutoKeyframeInterval(d4);
      logController->RequestSetLogFile("myLogFile2");

      // let the GM send 'em.

      tc->reset();
      dtCore::System::GetInstance().Step(0.3);

      // validate them.
      for (unsigned i = 0; i < tc->GetReceivedProcessMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedProcessMessages()[i].valid());
      }
      for (unsigned i = 0; i < tc->GetReceivedDispatchNetworkMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedDispatchNetworkMessages()[i].valid());
      }

      // find the processed (processMessage) Messages
      dtCore::RefPtr<const dtGame::Message> temp;

      dtCore::RefPtr<const dtGame::Message> procReqStatePlayback = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_PLAYBACK);
      dtCore::RefPtr<const dtGame::Message> procReqStateRecord = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_RECORD);
      dtCore::RefPtr<const dtGame::Message> procReqStateIdle = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_IDLE);
      dtCore::RefPtr<const dtGame::LogCaptureKeyframeMessage> procCaptureKeyframeMsg =
         (const dtGame::LogCaptureKeyframeMessage*)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CAPTURE_KEYFRAME)).get();
      dtCore::RefPtr<const dtGame::Message> procReqGeyKeyframes = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_KEYFRAMES);
      dtCore::RefPtr<const dtGame::Message> procReqGetLogfiles = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_LOGFILES);
      dtCore::RefPtr<const dtGame::Message> procReqGetTags = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_TAGS);
      dtCore::RefPtr<const dtGame::Message> procReqGetStatus = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_STATUS);
      dtCore::RefPtr<const dtGame::LogInsertTagMessage> procInsertTagMsg =
         (const dtGame::LogInsertTagMessage*)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_INSERT_TAG)).get();
      dtCore::RefPtr<const dtGame::LogDeleteLogfileMessage> procDeleteLogMsg =
         (const dtGame::LogDeleteLogfileMessage*)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_DELETE_LOG)).get();
      dtCore::RefPtr<const dtGame::LogSetLogfileMessage> procSetLogMsg =
         (const dtGame::LogSetLogfileMessage*)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_SET_LOGFILE)).get();
      dtCore::RefPtr<const dtGame::LogSetAutoKeyframeIntervalMessage> procSetKeyframeIntMsg =
         (const dtGame::LogSetAutoKeyframeIntervalMessage*)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL)).get();

      // test that they were processed (processMessage())

      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CHANGESTATE_PLAYBACK message should have been processed.", procReqStatePlayback != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CHANGESTATE_RECORD message should have been processed.", procReqStateRecord != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CHANGESTATE_IDLE message should have been processed.", procReqStateIdle != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CAPTURE_KEYFRAME message should have been processed.", procCaptureKeyframeMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_KEYFRAMES message should have been processed.", procReqGeyKeyframes != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_LOGFILES message should have been processed.", procReqGetLogfiles != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_TAGS message should have been processed.", procReqGetTags != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_STATUS message should have been processed.", procReqGetStatus != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_INSERT_TAG message should have been processed.", procInsertTagMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_DELETE_LOG message should have been processed.", procDeleteLogMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_SET_LOGFILE message should have been processed.", procSetLogMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_SET_AUTOKEYFRAMEINTERVAL message should have been processed.", procSetKeyframeIntMsg != NULL);

      // test some of the data on the processed messages

      dtGame::LogKeyframe keyframe2 = procCaptureKeyframeMsg->GetKeyframe();
      CPPUNIT_ASSERT_MESSAGE("Processed Keyframe should have a keyframe.", keyframe2 == keyframe);
      dtGame::LogTag tag2 = procInsertTagMsg->GetTag();
      CPPUNIT_ASSERT_MESSAGE("Processed Insert Tag should have tag.", tag2 == tag);
      CPPUNIT_ASSERT_MESSAGE("Processed Delete Log should have log file.", procDeleteLogMsg->GetLogFileName() == "myLogFile");
      CPPUNIT_ASSERT_MESSAGE("Processed Set Log should have log file.", procSetLogMsg->GetLogFileName() == "myLogFile2");
      CPPUNIT_ASSERT_MESSAGE("Processed Set Keyframe Interval should be set.", procSetKeyframeIntMsg->GetAutoKeyframeInterval() == d4);

      // find the SENT (sendMessage) Messages

      dtCore::RefPtr<const dtGame::Message> sentReqStatePlayback = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_PLAYBACK);
      dtCore::RefPtr<const dtGame::Message> sentReqStateRecord = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_RECORD);
      dtCore::RefPtr<const dtGame::Message> sentReqStateIdle = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_IDLE);
      dtCore::RefPtr<const dtGame::LogCaptureKeyframeMessage> sentCaptureKeyframeMsg =
         (const dtGame::LogCaptureKeyframeMessage*)( tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_CAPTURE_KEYFRAME)).get();
      dtCore::RefPtr<const dtGame::Message> sentReqGeyKeyframes = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_GET_KEYFRAMES);
      dtCore::RefPtr<const dtGame::Message> sentReqGetLogfiles = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_GET_LOGFILES);
      dtCore::RefPtr<const dtGame::Message> sentReqGetTags = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_GET_TAGS);
      dtCore::RefPtr<const dtGame::Message> sentReqGetStatus = tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_GET_STATUS);
      dtCore::RefPtr<const dtGame::LogInsertTagMessage> sentInsertTagMsg =
         (const dtGame::LogInsertTagMessage*)( tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_INSERT_TAG)).get();
      dtCore::RefPtr<const dtGame::LogDeleteLogfileMessage> sentDeleteLogMsg =
         (const dtGame::LogDeleteLogfileMessage*)( tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_DELETE_LOG)).get();
      dtCore::RefPtr<const dtGame::LogSetLogfileMessage> sentSetLogMsg =
         (const dtGame::LogSetLogfileMessage*)( tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_SET_LOGFILE)).get();
      dtCore::RefPtr<const dtGame::LogSetAutoKeyframeIntervalMessage> sentSetKeyframeIntMsg =
         (const dtGame::LogSetAutoKeyframeIntervalMessage*)( tc->FindDispatchNetworkMessageOfType(dtGame::MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL)).get();
      //dtCore::RefPtr<dtGame::LogStatusMessage> infoStatusMsg = tc->FindProcessMessageOfType(dtGame::MessageType::TICK_LOCAL);

      // test that they were SENT(sendMessage())

      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CHANGESTATE_PLAYBACK message should have been sent.", sentReqStatePlayback != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CHANGESTATE_RECORD message should have been sent.", sentReqStateRecord != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CHANGESTATE_IDLE message should have been sent.", sentReqStateIdle != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_CAPTURE_KEYFRAME message should have been sent.", sentCaptureKeyframeMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_KEYFRAMES message should have been sent.", sentReqGeyKeyframes != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_LOGFILES message should have been sent.", sentReqGetLogfiles != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_TAGS message should have been sent.", sentReqGetTags != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_GET_STATUS message should have been sent.", sentReqGetStatus != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_INSERT_TAG message should have been sent.", sentInsertTagMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_DELETE_LOG message should have been sent.", sentDeleteLogMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_SET_LOGFILE message should have been sent.", sentSetLogMsg != NULL);
      CPPUNIT_ASSERT_MESSAGE("A LOG_REQ_SET_AUTOKEYFRAMEINTERVAL message should have been sent.", sentSetKeyframeIntMsg != NULL);

      // test some of the data on the sent messages

      dtGame::LogKeyframe keyframe3 = sentCaptureKeyframeMsg->GetKeyframe();
      CPPUNIT_ASSERT_MESSAGE("Processed Keyframe should have a keyframe.", keyframe3 == keyframe);
      dtGame::LogTag tag3 = sentInsertTagMsg->GetTag();
      CPPUNIT_ASSERT_MESSAGE("Processed Insert Tag should have tag.", tag3 == tag);
      CPPUNIT_ASSERT_MESSAGE("Processed Delete Log should have log file.", sentDeleteLogMsg->GetLogFileName() == "myLogFile");
      CPPUNIT_ASSERT_MESSAGE("Processed Set Log should have log file.", sentSetLogMsg->GetLogFileName() == "myLogFile2");
      CPPUNIT_ASSERT_MESSAGE("Processed Set Keyframe Interval should be set.", sentSetKeyframeIntMsg->GetAutoKeyframeInterval() == d4);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestControllerSignals()
{
   try
   {
      dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);

      // create a test object that can receive signals

      dtCore::RefPtr<TestControllerSignal> testSignal = new TestControllerSignal();
      testSignal->RegisterSignals(*logController);
      testSignal->Reset();

      // send whatever message is needed to generate the signal

      dtCore::RefPtr<dtGame::Message> statusMessage =
         mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::LOG_INFO_STATUS);
      dtGame::LogStatusMessage* pMsg = static_cast<dtGame::LogStatusMessage *> (statusMessage.get());
      pMsg->SetStatus(status);
      mGameManager->SendMessage(*statusMessage);
      //mGameManager->SendNetworkMessage(*statusMessage);
      dtCore::System::GetInstance().Step(0.3); // let the GM send 'em.

      // check the results

      CPPUNIT_ASSERT_MESSAGE("Log Controller should send a status signal when receiving message LOG_INFO_STATUS.",
         testSignal->mStatusSignalReceived);
      dtGame::LogStatus resultStatus = testSignal->mStatus;
      CPPUNIT_ASSERT_MESSAGE("Log Controller should have status data from the signal for message LOG_INFO_STATUS.",
         (status.GetActiveMaps() == resultStatus.GetActiveMaps()) && (status.GetCurrentRecordDuration() == resultStatus.GetCurrentRecordDuration()));

      // add tests for the log_info keyframes, tags, and logs

      // test the status message getting to the controller
      dtGame::LogStatus contStatus = logController->GetLastKnownStatus();
      CPPUNIT_ASSERT_MESSAGE("Log Controller should have a copy of the last known status from the last LOG_INFO_STATUS message.",
         (status.GetActiveMaps() == contStatus.GetActiveMaps()) && (status.GetCurrentRecordDuration() == contStatus.GetCurrentRecordDuration()));

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestServerLogger()
{
   dtGame::LogStatus resultStatus;
   unsigned long msgCount = 0;

   try
   {
      // NOTE - Test setting a log file with an extension - windows and linux

      // add a controller component, the server component, and a test component (SIGNAL)
      dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
      dtCore::RefPtr<TestLogStream> testStream = new TestLogStream(msgFactory, mGameManager);
      dtCore::RefPtr<dtGame::ServerLoggerComponent> serverController =
         new dtGame::ServerLoggerComponent(*testStream.get());
      dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
      dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent();
      dtCore::RefPtr<TestControllerSignal> testSignal = new TestControllerSignal();
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*serverController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::HIGHEST);
      testSignal->RegisterSignals(*logController);
      testSignal->Reset();
      tc->reset();

      // FULL SUITE TEST!!! From logger controller, to gm, to server controller, back to gm, to controller, and to signal tester

      logController->RequestServerGetStatus();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Request Status Msg should have caused a Status response message.",
         testSignal->mStatusSignalReceived);

      // Try to change to Idle while already in idle - should send status just to us

      testSignal->Reset();
      tc->reset();
      logController->RequestChangeStateToIdle();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Idle (when in idle) should have caused a Status response message.",
         testSignal->mStatusSignalReceived);
      dtCore::RefPtr<const dtGame::Message> status2 = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_INFO_STATUS);
      CPPUNIT_ASSERT_MESSAGE("Request Change State to idle (when in idle) should have sent status just to me",
         (status2->GetDestination() == &mGameManager->GetMachineInfo()));

      // Set the Log File and see if it gets back in a status

      testSignal->Reset();
      CPPUNIT_ASSERT_MESSAGE("A silly check for Test Signal - should have received signal status flag = false.  Would stink to have someone break signal class and all our tests are bogus", !testSignal->mStatusSignalReceived);
      logController->RequestSetLogFile("UnitTestLoggerFile");
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Set Log File should cause a Status message.",
         testSignal->mStatusSignalReceived);
      CPPUNIT_ASSERT_MESSAGE("Set Log File should return log file in status",
         testSignal->mStatus.GetLogFile() == "UnitTestLoggerFile");

      // change the server state to RECORD

      msgCount = 0;
      testSignal->Reset();
      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Record should have caused a Status response message.",
         testSignal->mStatusSignalReceived);
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Record should have sent status with RECORD.",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      msgCount = testSignal->mStatus.GetNumMessages();
      // force at least one extra message to check that # count goes up
      logController->RequestServerGetStatus();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("We should have recorded a msg", msgCount < testSignal->mStatus.GetNumMessages());

      // try to set the log file while in Record mode, make sure it fails.

      testSignal->Reset();
      logController->RequestSetLogFile("ERRORSET");
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Set Log File with error should NOT cause a Status message.", !testSignal->mStatusSignalReceived);
      // should have gotten a rejection message
      const dtGame::Message* causeMsg2 = testSignal->mRejectMessage->GetCausingMessage();
      CPPUNIT_ASSERT_MESSAGE("Set Log File with error should have gotten reject message.",
         (testSignal->mRejectMessage->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED) &&
         (causeMsg2 != NULL) && (causeMsg2->GetMessageType() == dtGame::MessageType::LOG_REQ_SET_LOGFILE));
      // make sure the bad set log file didn't change the log file name on the server component
      testSignal->Reset();
      logController->RequestServerGetStatus();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Failed Set Log File should not change file name ",
         testSignal->mStatus.GetLogFile() == "UnitTestLoggerFile");

      // should we check setting log file while in playback mode too? or overkill?

      // check the request change state - IDLE - message

      testSignal->Reset();
      logController->RequestChangeStateToIdle();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Valid Request Change State to Idle should have caused a Status response message.",
         testSignal->mStatusSignalReceived);
      CPPUNIT_ASSERT_MESSAGE("Valid Request Change State to Idle should have sent status with IDLE.",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);

      // Test recording is set to IDLE after a map change event

      msgCount = 0;
      testSignal->Reset();
      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Record should have sent status with RECORD.",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      msgCount = testSignal->mStatus.GetNumMessages();
      unsigned long lastMsgCount = msgCount;
      // Send the map change message
      dtCore::RefPtr<dtGame::Message> message = mGameManager
         ->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_CHANGE_BEGIN);
      mGameManager->SendMessage(*message);
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      // force at least one extra message to check that # count goes up
      logController->RequestServerGetStatus();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("State should have been changed to IDLE after the map change event",
         logController->GetLastKnownStatus().GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);
      msgCount = logController->GetLastKnownStatus().GetNumMessages();
      CPPUNIT_ASSERT_MESSAGE("We should NOT have recorded the msg", msgCount == lastMsgCount);

      // check the ability to change to IDLE that also has an exception

      // first, put into record state.
      testSignal->Reset();
      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Record should have sent status with RECORD.",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      // now, force exception and switch back to idle
      testSignal->Reset();
      testStream->mExceptionEnabled = true;  // force exception
      logController->RequestChangeStateToIdle();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      // do the checks
      CPPUNIT_ASSERT_MESSAGE("With exception, should still get Status message.", testSignal->mStatusSignalReceived);
      CPPUNIT_ASSERT_MESSAGE("With exception, should still get reject message.", testSignal->mRejectionSignalReceived);
      const dtGame::Message* causeMsg3 = testSignal->mRejectMessage->GetCausingMessage();
      CPPUNIT_ASSERT_MESSAGE("Change to Idle with error should have gotten reject message.",
         (testSignal->mRejectMessage->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED) &&
         (causeMsg3 != NULL) && (causeMsg3->GetMessageType() == dtGame::MessageType::LOG_REQ_CHANGESTATE_IDLE));

      // Test setting to RECORD that fails with an exception

      // first, put into record state.
      testSignal->Reset();
      testStream->mExceptionEnabled = true;  // force exception
      logController->RequestChangeStateToRecord();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Failed Record (exception) should still get Status message.", testSignal->mStatusSignalReceived);
      CPPUNIT_ASSERT_MESSAGE("Failed Record (exception) should have sent status with IDLE.",
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);
      CPPUNIT_ASSERT_MESSAGE("Failed Record (exception), should get reject message.", testSignal->mRejectionSignalReceived);
      const dtGame::Message* causeMsg4 = testSignal->mRejectMessage->GetCausingMessage();
      CPPUNIT_ASSERT_MESSAGE("Change to Idle with error should have gotten reject message.",
         (testSignal->mRejectMessage->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED) &&
         (causeMsg4 != NULL) && (causeMsg4->GetMessageType() == dtGame::MessageType::LOG_REQ_CHANGESTATE_RECORD));

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestServerLogger2()
{
   // too many tests in one test makes it hard to refactor later... so... more tests here...

   //dtGame::LogStatus resultStatus;
   dtCore::RefPtr<dtGame::Message> message;

   try
   {
      dtGame::MessageFactory& msgFactory = mGameManager->GetMessageFactory();
      dtCore::RefPtr<TestLogStream> testStream = new TestLogStream(msgFactory, mGameManager);
      dtCore::RefPtr<dtGame::ServerLoggerComponent> serverController =
         new dtGame::ServerLoggerComponent(*testStream.get());
      dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
      dtCore::RefPtr<dtGame::TestComponent> tc = new dtGame::TestComponent();
      dtCore::RefPtr<TestControllerSignal> testSignal = new TestControllerSignal();
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*serverController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      testSignal->RegisterSignals(*logController);
      testSignal->Reset();
      tc->reset();

      // Try changing map, then request a status, see if status has correct map

      testSignal->Reset();
      dtCore::RefPtr<dtGame::MapMessage> mapLoadedMsg;
      msgFactory.CreateMessage(dtGame::MessageType::INFO_MAP_LOADED, mapLoadedMsg);
      dtGame::GameManager::NameVector mapNames;
      mapNames.push_back("myBogusMapName");
      mapNames.push_back("myBogusMapName2");
      mapNames.push_back("myBogusMapName3");

      mapLoadedMsg->SetMapNames(mapNames);

      mGameManager->SendMessage(*mapLoadedMsg);
      logController->RequestServerGetStatus();
      dtCore::AppSleep(10); // tick the GM so it can send the messages
      dtCore::System::GetInstance().Step();
      CPPUNIT_ASSERT_MESSAGE("Change map msg should change map on logstatus", testSignal->mStatus.GetActiveMaps() == mapNames);

   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
   //catch (const std::exception& e)
   //{
   //   CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   //}
}

//////////////////////////////////////////////////////////////////////////
class MessageCaptureLogStream : public dtGame::LogStream
{
public:
   MessageCaptureLogStream(dtGame::MessageFactory& msgFactory)
      :dtGame::LogStream(msgFactory)
   {
   };

   virtual void Close() {};
   virtual void Create(const std::string& logsPath, const std::string& logResourceName) {};
   virtual void Open(const std::string& logsPath, const std::string& logResourceName) {};
   virtual void Delete(const std::string& logsPath, const std::string& logResourceName) {};
   virtual void WriteMessage(const dtGame::Message& msg, double timeStamp)
   {
      mWrittenMsgs.push_back(&msg.GetMessageType());
   };

   virtual dtCore::RefPtr<dtGame::Message> ReadMessage(double& timeStamp)
   {
      dtCore::RefPtr<dtGame::Message> msg = new dtGame::Message();
      return msg;
   }

   virtual void InsertTag(dtGame::LogTag& newTag) {};
   virtual void InsertKeyFrame(dtGame::LogKeyframe& newKeyFrame) {};
   virtual void JumpToKeyFrame(const dtGame::LogKeyframe& keyFrame) {};
   virtual void GetTagIndex(std::vector<dtGame::LogTag>& tags) {};
   virtual void GetKeyFrameIndex(std::vector<dtGame::LogKeyframe>& keyFrames) {};
   virtual void GetAvailableLogs(const std::string& logsPath,std::vector<std::string>& logs) {};
   virtual void Flush() {};

   bool ReceivedMsgType(const dtGame::MessageType& msgType) const
   {
      return std::find(mWrittenMsgs.begin(), mWrittenMsgs.end(), &msgType) != mWrittenMsgs.end();
   }

   std::vector<const dtGame::MessageType*> mWrittenMsgs;

protected:
   virtual ~MessageCaptureLogStream() {};	
};

////////////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestAddRemoveIgnoredMessageTypeToLogger()
{
   dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
   dtCore::RefPtr<MessageCaptureLogStream> stream = new MessageCaptureLogStream(mGameManager->GetMessageFactory());
   dtCore::RefPtr<dtGame::ServerLoggerComponent> serverController = new dtGame::ServerLoggerComponent(*stream);

   mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
   mGameManager->AddComponent(*serverController, dtGame::GameManager::ComponentPriority::NORMAL);
   mGameManager->AddComponent(*new dtGame::DefaultMessageProcessor(), dtGame::GameManager::ComponentPriority::HIGHEST);

   logController->RequestChangeStateToRecord();

   dtCore::RefPtr<dtGame::Message> testMsg = 
      mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_GAME_EVENT);
   mGameManager->SendMessage(*testMsg);

   dtCore::System::GetInstance().Step();

   //LogStream should have received a INFO_GAME_EVENT
   CPPUNIT_ASSERT_EQUAL_MESSAGE("ServerLogger shouldn't have filtered out the received Message",
                                true, stream->ReceivedMsgType(dtGame::MessageType::INFO_GAME_EVENT));

   stream->mWrittenMsgs.clear();

   //request that we ignore this MessageType
   logController->RequestAddIgnoredMessageType(dtGame::MessageType::INFO_GAME_EVENT);
   mGameManager->SendMessage(*testMsg);
   dtCore::System::GetInstance().Step();

   //LogStream shouldn't receive a INFO_GAME_EVENT now
   CPPUNIT_ASSERT_EQUAL_MESSAGE("ServerLogger didn't filter out the requested MessageType",
                                false, stream->ReceivedMsgType(dtGame::MessageType::INFO_GAME_EVENT));

   stream->mWrittenMsgs.clear();

   //request that we stop ignoring this MessageType
   logController->RequestRemoveIgnoredMessageType(dtGame::MessageType::INFO_GAME_EVENT);
   mGameManager->SendMessage(*testMsg);
   dtCore::System::GetInstance().Step();

   //LogStream should have received a INFO_GAME_EVENT
   CPPUNIT_ASSERT_EQUAL_MESSAGE("ServerLogger shouldn't have filtered out the received Message",
                                true, stream->ReceivedMsgType(dtGame::MessageType::INFO_GAME_EVENT));

   mGameManager->RemoveComponent(*logController);
   mGameManager->RemoveComponent(*serverController);
}
