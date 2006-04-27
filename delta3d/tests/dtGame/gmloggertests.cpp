/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Matthew W. Campbell
 */
 
#include <dtGame/binarylogstream.h>
#include <dtGame/clientgamemanager.h>
#include <dtGame/serverloggercomponent.h>
#include <dtGame/logcontroller.h>
#include <dtGame/gamemanager.h>
#include <dtCore/refptr.h>
#include <dtCore/scene.h>
#include <dtCore/system.h>
#include <dtCore/sigslot.h>
#include <dtCore/globals.h>
#include <dtUtil/exception.h>
#include <cppunit/extensions/HelperMacros.h> 
#include <dtGame/loggermessages.h>
#include <dtGame/logstatus.h>
#include <dtGame/logtag.h>
#include <dtGame/logkeyframe.h>
#include <dtGame/basemessages.h>
#include <dtGame/defaultmessageprocessor.h>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #include <Windows.h>
   #define SLEEP(milliseconds) Sleep((milliseconds))
#else
   #include <unistd.h>
   #define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

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
      CPPUNIT_TEST(TestLogControllerComponent);
      CPPUNIT_TEST(TestControllerSignals);
      CPPUNIT_TEST(TestServerLogger);
      CPPUNIT_TEST(TestServerLogger2);
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
      void TestLogControllerComponent();
      void TestControllerSignals();
      void TestServerLogger();
      void TestServerLogger2();
         
      void CompareKeyframeLists(const std::vector<dtGame::LogKeyframe> listOne,
         const std::vector<dtGame::LogKeyframe> listTwo);
         
      void CompareTagLists(const std::vector<dtGame::LogTag> listOne,
         const std::vector<dtGame::LogTag> listTwo);
      
   private:
      dtCore::RefPtr<dtGame::ClientGameManager> mGameManager;

      dtGame::LogStatus status;
      dtGame::LogKeyframe keyframe;
      dtGame::LogTag tag;

      double d1;
      double d2;
      double d3;
      double d4;

      unsigned long long1;
      long long2;
      
      static char* mTestGameActorLibrary;
      static char* mTestActorLibrary;      
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/**
 * This class tests that messages are sent.  It's a component that traps sent & processed messages
 * This is a duplicate of hte class in messagetests.cpp
 */
class TestComponent: public dtGame::GMComponent
{
   public:
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedProcessMessages() 
         { return mReceivedProcessMessages; }
      std::vector<dtCore::RefPtr<const dtGame::Message> >& GetReceivedSendMessages() 
         { return mReceivedSendMessages; }
      
      virtual void ProcessMessage(const dtGame::Message& msg)
      {
         mReceivedProcessMessages.push_back(&msg);
      }
      virtual void SendMessage(const dtGame::Message& msg)
      {
         mReceivedSendMessages.push_back(&msg);            
      }
      
      void reset() 
      {
         mReceivedSendMessages.clear();
         mReceivedProcessMessages.clear();
      }
      
      dtCore::RefPtr<const dtGame::Message> FindProcessMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedProcessMessages.size(); ++i)
         {
            if (mReceivedProcessMessages[i]->GetMessageType() == type)
               return mReceivedProcessMessages[i];
         }
         return NULL;
      }
      dtCore::RefPtr<const dtGame::Message> FindSendMessageOfType(const dtGame::MessageType& type)
      {
         for (unsigned i = 0; i < mReceivedSendMessages.size(); ++i)
         {
            if (mReceivedSendMessages[i]->GetMessageType() == type)
               return mReceivedSendMessages[i];
         }
         return NULL;
      }
   private:
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedProcessMessages;
      std::vector<dtCore::RefPtr<const dtGame::Message> > mReceivedSendMessages;
};

//////////////////////////////////////////////////////////////////////////
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

      void OnReceivedStatus(const dtGame::LogStatus &newStatus) 
      {
         mStatus = newStatus;
         mStatusSignalReceived = true;
      }
      void OnReceivedRejection(const dtGame::Message &newMessage) 
      {
         mRejectMessage = &newMessage;
         mRejectionSignalReceived = true;
      }
      void RegisterSignals(dtGame::LogController &logController)
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
      TestLogStream(dtGame::MessageFactory &msgFactory, dtCore::RefPtr<dtGame::ClientGameManager> newGameManager) 
         : dtGame::LogStream(msgFactory) 
      { 
         mGameManager = newGameManager; 
         mExceptionEnabled = false;
      }
      void Create(const std::string &logsPath, const std::string &logResourceName) { DoException(); }
      void Close() 
      { 
         // Note, real close doesn't throw exception at the moment, but left in
         // because it's easier to test failure/reject code 
         DoException(); 
      } 
      void Open(const std::string &logsPath, const std::string &logResourceName) { DoException(); }
      void WriteMessage(const dtGame::Message &msg, double timeStamp) { DoException(); }
      dtCore::RefPtr<dtGame::Message> ReadMessage(double &timeStamp)
      {
         DoException(); 
         return mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::LOG_INFO_STATUS);
      }
      void InsertTag(dtGame::LogTag &newTag) { DoException(); }
      void InsertKeyFrame(dtGame::LogKeyframe &newKeyFrame) { DoException(); }
      void JumpToKeyFrame(const dtGame::LogKeyframe &keyFrame) { DoException(); } 
      void GetTagIndex(std::vector<dtGame::LogTag> &tags) { DoException(); }
      void GetKeyFrameIndex(std::vector<dtGame::LogKeyframe> &keyFrames) { DoException(); }
      void Flush() { DoException(); }
      void GetAvailableLogs(const std::string &logsPath, 
            std::vector<std::string> &logs) { }
      virtual void Delete(const std::string &logsPath, const std::string &logResourceName) { }
      
      bool mExceptionEnabled;
   protected:
      // allow for exception testing
      void DoException() 
      {
         if (mExceptionEnabled)          
            EXCEPT(dtGame::LogStreamException::LOGGER_IO_EXCEPTION,"Test Exception - Not an Error!");    
      }
      virtual ~TestLogStream() { }      
   private: 
      dtCore::RefPtr<dtGame::ClientGameManager> mGameManager;
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(GMLoggerTests);
const std::string LOGFILE = "testlog";
const std::string TESTS_DIR = dtCore::GetDeltaRootPath()+"\\tests";

#if defined (_DEBUG) && (defined (WIN32) || defined (_WIN32) || defined (__WIN32__))
char* GMLoggerTests::mTestGameActorLibrary="testGameActorLibraryd";
char* GMLoggerTests::mTestActorLibrary="testActorLibraryd";
#else
char* GMLoggerTests::mTestGameActorLibrary="testGameActorLibrary";
char* GMLoggerTests::mTestActorLibrary="testActorLibrary";
#endif

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
   keyframe.SetActiveMap("myMap");
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
   status.SetEstPlaybackTimeRemaining(d3);
   status.SetCurrentRecordDuration(d4);
   status.SetNumRecordedMessages(long1);
   status.SetActiveMap("myMap");
   status.SetLogFile("myLogFile");

   try 
   {
      dtCore::Scene* scene = new dtCore::Scene();
      mGameManager = new dtGame::ClientGameManager(*scene);
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      mGameManager->LoadActorRegistry(mTestGameActorLibrary);
      dtCore::System::Instance()->Start();
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL((std::string("Error: ") + ex.What()).c_str());
   }
      
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::tearDown()
{
   if (mGameManager.valid())
   {
      try {
         dtCore::System::Instance()->SetPause(false);
         dtCore::System::Instance()->Stop();
         mGameManager->DeleteAllActors();
         mGameManager->UnloadActorRegistry(mTestGameActorLibrary);
         mGameManager = NULL;  
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
      }
   }
   
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamCreate()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   
   try 
   {
      stream->Create(TESTS_DIR,LOGFILE);
      stream->Close(); 
   }
   catch(const dtUtil::Exception &e) 
   {      
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamOpen()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   
   try
   {
      stream->Create(TESTS_DIR,LOGFILE);
      stream->Close();
      stream->Open(TESTS_DIR,LOGFILE);
      stream->Close();
   }
   catch(const dtUtil::Exception &e) 
   {      
      CPPUNIT_FAIL("Error: " + e.What());
   }   
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamDeleteLog()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   
   try
   {
      std::vector<std::string> logList;
      
      stream->Create(TESTS_DIR,LOGFILE);
      stream->Create(TESTS_DIR,LOGFILE+"1");
      stream->Create(TESTS_DIR,LOGFILE+"2");
      stream->Create(TESTS_DIR,LOGFILE+"3");
      
      try
      {
         stream->Delete("./testblahblah","nothing");
         CPPUNIT_FAIL("Should get an exception when deleting from a log directory that does not exist.");
      }
      catch (dtUtil::Exception &e) {}
      
      try
      {
         stream->Delete(TESTS_DIR,"nothing");
         CPPUNIT_FAIL("Should get an exception when deleting for a log that does not exist.");
      }
      catch (dtUtil::Exception &e) {}
      
      //This list will also return the default log as well...      
      stream->GetAvailableLogs(TESTS_DIR,logList);
      CPPUNIT_ASSERT_MESSAGE("There should be five logs in the list, but there are " + dtUtil::ToString( logList.size() ), logList.size() == 5);      
      
      stream->Delete(TESTS_DIR,LOGFILE);
      stream->Delete(TESTS_DIR,LOGFILE+"1");
      stream->Delete(TESTS_DIR,LOGFILE+"2");
      stream->Delete(TESTS_DIR,LOGFILE+"3");
      
      stream->GetAvailableLogs(TESTS_DIR,logList);
      CPPUNIT_ASSERT_MESSAGE("There should be only one log in the list, but there are " + dtUtil::ToString( logList.size() ), logList.size() == 1);      
   }
   catch(const dtUtil::Exception &e) 
   {      
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamGetLogs()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   
   try
   {      
      std::vector<std::string> logList;
    
      stream->Create(TESTS_DIR,LOGFILE);
      stream->Create(TESTS_DIR,LOGFILE+"1");
      stream->Create(TESTS_DIR,LOGFILE+"2");
      stream->Create(TESTS_DIR,LOGFILE+"3");
      
      try
      {
         //Should throw an exception if a non-existent directory is queried.
         stream->GetAvailableLogs("./testblahblah",logList);
         CPPUNIT_FAIL("Should get an exception when checking a directory for logs that does not exist.");
      }
      catch (dtUtil::Exception &e) {}
            
      //This list will also return the default log as well...
      stream->GetAvailableLogs(TESTS_DIR,logList);
      CPPUNIT_ASSERT_MESSAGE("There should be five logs in the list.", logList.size() == 5);      
      int count = 0;
      for (unsigned int i=0; i<logList.size(); i++)
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
   catch(const dtUtil::Exception &e) 
   {      
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamReadWriteErrors()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::TickMessage> tickMessage = 
      (dtGame::TickMessage *)(msgFactory.CreateMessage(dtGame::MessageType::TICK_LOCAL)).get();
        
   //Now make sure the proper exceptions are thrown when writing to a 
   //read only log stream.
   try
   {
      try
      {
         dtCore::RefPtr<dtGame::BinaryLogStream> errorStream = 
            new dtGame::BinaryLogStream(msgFactory);
         
         //First, open for read...
         errorStream->Open(TESTS_DIR,LOGFILE);
         
         //Now try writing to it.. this should throw an exception.
         errorStream->WriteMessage(*tickMessage.get(),100.0);
         CPPUNIT_FAIL("Should have caught a LOGGER_IO_EXCEPTION.");
      }
      catch (const dtUtil::Exception &e)    
      {
      }
      
      //Now make sure the proper exceptions are thrown when reading from a
      //write only log stream.
      try
      {
         dtCore::RefPtr<dtGame::BinaryLogStream> errorStream = 
            new dtGame::BinaryLogStream(msgFactory);
         
         //First, open for read...
         errorStream->Create(TESTS_DIR,LOGFILE);
         
         //Now try reading from it.. this should throw an exception.
         double timeStamp;
         tickMessage = (dtGame::TickMessage *)(errorStream->ReadMessage(timeStamp)).get();
         CPPUNIT_FAIL("Should have caught a LOGGER_IO_EXCEPTION.");
      }
      catch (const dtUtil::Exception &e)    
      {
      }          
   }
   catch(const dtUtil::Exception &e) 
   {      
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamReadWriteMessages()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   dtCore::RefPtr<dtGame::TickMessage> tickMessage = 
      (dtGame::TickMessage *)(msgFactory.CreateMessage(dtGame::MessageType::TICK_LOCAL)).get();
   int i;
   
   try 
   {
      stream->Create(TESTS_DIR,LOGFILE);     
      for (i=0; i<100; i++)
      {
         tickMessage->SetDeltaSimTime(i*2.0f);
         tickMessage->SetDeltaRealTime(i*3.0f);
         tickMessage->SetSimTimeScale(i);
         tickMessage->SetSimulationTime((double)i);
         stream->WriteMessage(*tickMessage.get(),100.045);
      }
      stream->Close();
      
      stream->Open(TESTS_DIR,LOGFILE);
      for (i=0; i<100; i++)
      {
         float t1 = i*2.0f;
         float t2 = i*3.0f;
         float t3 = (float)i;
         long t4 = i;
         
         double timeStamp;
         dtCore::RefPtr<dtGame::TickMessage> msgToTest =
            (dtGame::TickMessage *)(stream->ReadMessage(timeStamp)).get();
                  
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
   catch(const dtUtil::Exception &e) 
   {      
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamTags()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   
   try
   {
      std::vector<dtGame::LogTag> tagList;
      dtGame::LogTag newTag,tag2;
                 
      //First we create the file and do a little sanity check to make sure we have no tags.
      stream->Create(TESTS_DIR,LOGFILE);
      stream->GetTagIndex(tagList);
      CPPUNIT_ASSERT_MESSAGE("Newly created binary log files should not contain any tags", 
         tagList.size() == 0);      
      stream->Close();
      
      stream->Open(TESTS_DIR,LOGFILE);
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
      stream->Open(TESTS_DIR,LOGFILE);
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
      stream->Create(TESTS_DIR,LOGFILE);
      for (i=0; i<100; i++)
      {
         newTag.SetUniqueId(dtCore::UniqueId());
         newTag.SetSimTimeStamp(i*100);
         newTag.SetKeyframeUniqueId(dtCore::UniqueId());
         if ((i%2) == 0)
            newTag.SetCaptureKeyframe(true);
         else
            newTag.SetCaptureKeyframe(false);
            
         stream->InsertTag(newTag);
         tagsToCompare.push_back(newTag);
         
         if ((i%10) == 0)
            stream->Flush();
      }                      
      stream->Close();
      
      stream->Open(TESTS_DIR,LOGFILE);
      stream->GetTagIndex(tagList);
      CPPUNIT_ASSERT_MESSAGE("Tag list should have a 100 tags.",tagList.size() == tagsToCompare.size());
      for (i=0; i<100; i++)
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
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamKeyFrames()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   
   try
   {
      std::vector<dtGame::LogKeyframe> kfList;
      dtGame::LogKeyframe newFrame,frame2;
                 
      //First we create the file and do a little sanity check to make sure we have no key frames.
      stream->Create(TESTS_DIR,LOGFILE);
      stream->GetKeyFrameIndex(kfList);
      CPPUNIT_ASSERT_MESSAGE("Newly created binary log files should not contain any keyframe entries.", 
         kfList.size() == 0);      
      stream->Close();
      
      stream->Open(TESTS_DIR,LOGFILE);
      stream->GetKeyFrameIndex(kfList);
      CPPUNIT_ASSERT_MESSAGE("Keyframe list should have been empty.",kfList.size() == 0);
      
      //Now add a key frames and make sure it is in the list of returned key frames.
      newFrame.SetName("bob");
      newFrame.SetDescription("bob_desc");
      newFrame.SetSimTimeStamp(10.001);
      newFrame.SetActiveMap("testmap.xml");
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
      CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",frame2.GetActiveMap() == newFrame.GetActiveMap());  
      stream->Close();      
      
      //Now we need to make sure the key frames got saved and loaded from the index tables file properly.
      stream->Open(TESTS_DIR,LOGFILE);
      stream->GetKeyFrameIndex(kfList);   
      CPPUNIT_ASSERT_MESSAGE("Keyframe list should have a single key frame.",kfList.size() == 1);      
      frame2 = kfList[0];
      CPPUNIT_ASSERT_MESSAGE("Keyframe unique ids should be equal",newFrame == frame2);
      CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",
         newFrame.GetTagUniqueId() == frame2.GetTagUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Keyframe names should be equal.",frame2.GetName() == newFrame.GetName());      
      CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions should be equal.",frame2.GetDescription() == newFrame.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Keyframe simulation times should be equal.",frame2.GetSimTimeStamp() == newFrame.GetSimTimeStamp());  
      CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",frame2.GetActiveMap() == newFrame.GetActiveMap()); 
      stream->Close();
      
      //Now make sure we can read and write many many key frames.
      int i;
      std::vector<dtGame::LogKeyframe> framesToCompare;
      newFrame.SetName("bob");
      newFrame.SetDescription("bob_desc");
      newFrame.SetActiveMap("activemap.xml");
      stream->Create(TESTS_DIR,LOGFILE);
      for (i=0; i<100; i++)
      {
         newFrame.SetUniqueId(dtCore::UniqueId());
         newFrame.SetTagUniqueId(dtCore::UniqueId("tag:"+ dtUtil::ToString(i)));
         newFrame.SetSimTimeStamp(i*100);
         stream->InsertKeyFrame(newFrame);
         framesToCompare.push_back(newFrame);
         
         if ((i%10) == 0)
            stream->Flush();
      }                      
      stream->Close();
      
      stream->Open(TESTS_DIR,LOGFILE);
      stream->GetKeyFrameIndex(kfList);
      CPPUNIT_ASSERT_MESSAGE("KeyFrame list should have a 100 key frames.",kfList.size() == framesToCompare.size());
      for (i=0; i<100; i++)
      {
         CPPUNIT_ASSERT_MESSAGE("Keyframe unique ids should be equal",kfList[i] == framesToCompare[i]);
         CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",
            kfList[i].GetTagUniqueId() == framesToCompare[i].GetTagUniqueId());         
         CPPUNIT_ASSERT_MESSAGE("Keyframe names should be equal.",kfList[i].GetName() == framesToCompare[i].GetName());      
         CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions should be equal.",kfList[i].GetDescription() == framesToCompare[i].GetDescription());
         CPPUNIT_ASSERT_MESSAGE("Keyframe simulation times should be equal.",kfList[i].GetSimTimeStamp() == framesToCompare[i].GetSimTimeStamp());  
         CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",kfList[i].GetActiveMap() == framesToCompare[i].GetActiveMap()); 
      }
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamTagsAndKeyFrames()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   unsigned int i;
   
   try
   {
      std::vector<dtGame::LogTag> tagList,tagCompareList;
      std::vector<dtGame::LogKeyframe> kfList,kfCompareList;
            
      //This test makes sure that interleaved keyframes and tags are correctly
      //stored and retreived from the log stream's index table.
      stream->Create(TESTS_DIR,LOGFILE);
      for (i=0; i<1000; i++)
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
            kf.SetActiveMap("bobtestmap.xml");
            kf.SetTagUniqueId(dtCore::UniqueId("tagunique" + dtUtil::ToString(i)));
            kf.SetSimTimeStamp(i*10);
            kf.SetLogFileOffset(i);
            stream->InsertKeyFrame(kf);
            kfCompareList.push_back(kf);            
         }
      }     
      
      stream->Close();
      
      //Now read it back and compare with the lists we just built.
      stream->Open(TESTS_DIR,LOGFILE);
      stream->GetTagIndex(tagList);
      stream->GetKeyFrameIndex(kfList);
      
      CPPUNIT_ASSERT_MESSAGE("Tag list from file not of correct size.",tagList.size() == tagCompareList.size());
      CPPUNIT_ASSERT_MESSAGE("Keyframe list from file not of correct size.",kfList.size() == kfCompareList.size());      
      for (i=0; i<kfList.size(); i++)
      {
         CPPUNIT_ASSERT_MESSAGE("Keyframe unique ids should be equal",kfList[i] == kfCompareList[i]);
         CPPUNIT_ASSERT_MESSAGE("Tag unique ids should be equal",
            kfList[i].GetTagUniqueId() == kfCompareList[i].GetTagUniqueId());
         CPPUNIT_ASSERT_MESSAGE("Keyframe names should be equal.",kfList[i].GetName() == kfCompareList[i].GetName());      
         CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions should be equal.",kfList[i].GetDescription() == kfCompareList[i].GetDescription());
         CPPUNIT_ASSERT_MESSAGE("Keyframe simulation times should be equal.",kfList[i].GetSimTimeStamp() == kfCompareList[i].GetSimTimeStamp());  
         CPPUNIT_ASSERT_MESSAGE("Keyframe active map names should be equal.",kfList[i].GetActiveMap() == kfCompareList[i].GetActiveMap()); 
      }
      for (i=0; i<tagList.size(); i++)
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
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestBinaryLogStreamJumpToKeyFrame()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream =  new dtGame::BinaryLogStream(msgFactory);
   dtCore::RefPtr<dtGame::TickMessage> tickMessage = 
      (dtGame::TickMessage *)(msgFactory.CreateMessage(dtGame::MessageType::TICK_LOCAL)).get();
   unsigned int i;
   
   try
   {
      stream->Create(TESTS_DIR,LOGFILE);
      for (i=0; i<1000; i++)
      {
         tickMessage->SetDeltaSimTime(i);
         tickMessage->SetDeltaRealTime(i+1);
         tickMessage->SetSimTimeScale(i+2);
         tickMessage->SetSimulationTime(i+3);
         
         //Insert a keyframe every several messages...
         if ((i%25) == 0)
         {
            dtGame::LogKeyframe keyFrame;
            keyFrame.SetName("bob");
            keyFrame.SetDescription("bob_desc");
            keyFrame.SetSimTimeStamp(i);
            stream->InsertKeyFrame(keyFrame);
         }
         
         stream->WriteMessage(*tickMessage.get(),i);         
      }      
      
      stream->Close();
      stream->Open(TESTS_DIR,LOGFILE);
      
      std::vector<dtGame::LogKeyframe> kfList;
      stream->GetKeyFrameIndex(kfList);
      for (i=0; i<kfList.size(); i++)
      {
         stream->JumpToKeyFrame(kfList[i]);  
         
         double timeStamp;
         dtCore::RefPtr<dtGame::TickMessage> msgToTest =
            (dtGame::TickMessage *)(stream->ReadMessage(timeStamp)).get();          

         CPPUNIT_ASSERT_MESSAGE("Timestamps should be equal.",
            (msgToTest->GetDeltaSimTime() == kfList[i].GetSimTimeStamp()) &&
            (msgToTest->GetDeltaSimTime() == timeStamp));
      }
      
      stream->Close();
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestPlaybackRecordCycle()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
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
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = NULL;
      dtCore::RefPtr<dtGame::GameActorProxy> gameProxy = NULL;
      
      mGameManager->AddComponent(*serverController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*(new dtGame::DefaultMessageProcessor()), dtGame::GameManager::ComponentPriority::HIGHEST);
      dtCore::RefPtr<TestComponent> tc = new TestComponent();
      dtCore::RefPtr<TestControllerSignal> testSignal = new TestControllerSignal();
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      testSignal->RegisterSignals(*logController);
      
      dtCore::RefPtr<dtDAL::ActorType> actorType = 
         mGameManager->FindActorType("ExampleActors", "TestPlayer");
      CPPUNIT_ASSERT_MESSAGE("Could not find test player actor type.",
         actorType != NULL);
      
      proxy = mGameManager->CreateActor(*actorType);
      CPPUNIT_ASSERT_MESSAGE("Could not create test player actor.",
         actorType != NULL);
         
      gameProxy =  dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      mGameManager->AddActor(*gameProxy,false,false);     
      SLEEP(10);
      dtCore::System::Instance()->Step();
      
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of game actors before changing to record.",
         (unsigned)1,mGameManager->GetNumGameActors());
      
      //First, we'll start recording and then add some actors thus generating
      //actor create messages.
      logController->RequestChangeStateToRecord();
      SLEEP(10);
      dtCore::System::Instance()->Step();
            
      actorType = mGameManager->FindActorType("ExampleActors", "Test1Actor");
      
      proxy = mGameManager->CreateActor(*actorType);
      gameProxy =  dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      mGameManager->AddActor(*gameProxy,false,false);           
      
      proxy = mGameManager->CreateActor(*actorType);
      gameProxy = dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      mGameManager->AddActor(*gameProxy,false,false);

      SLEEP(10);
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of actors correctly added during record.",
         (unsigned)3,mGameManager->GetNumGameActors());

      testSignal->Reset();
      logController->RequestServerGetStatus();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Should be RECORD", 
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);

      logController->RequestChangeStateToIdle();
      SLEEP(10);
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Should be IDLE", 
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);
      
      tc->reset();      
      logController->RequestChangeStateToPlayback();
      SLEEP(10);
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Should be Playback", 
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_PLAYBACK);
      CPPUNIT_ASSERT_MESSAGE("We should have some number of messages", tc->GetReceivedProcessMessages().size() >= 3);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of game actors after changing to playback",
         (unsigned)1,mGameManager->GetNumGameActors());
   
      //Go through the playback cycle until the end has been reached at
      //which point the system should be paused.
      while (!mGameManager->IsPaused())
      {
         SLEEP(10);
         dtCore::System::Instance()->Step();
      }

      testSignal->Reset();
      logController->RequestServerGetStatus();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
         
      //There should be three actors.  The test player and the two actors added
      //during playback.
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Wrong number of game actors after concluding playback.",
         (unsigned)3,mGameManager->GetNumGameActors());
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL("Error: " + e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
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
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
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
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerGetKeyframes()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = NULL;
   dtCore::RefPtr<dtGame::ServerLoggerComponent> serverLoggerComp = NULL;
   dtCore::RefPtr<dtGame::LogController> logController = NULL;
   dtCore::RefPtr<TestControllerSignal> rejectMsgSignal = NULL;
      
   try 
   {
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = NULL;
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
      
      dtCore::RefPtr<dtDAL::ActorType> actorType = 
         mGameManager->FindActorType("ExampleActors", "TestPlayer");
      CPPUNIT_ASSERT_MESSAGE("Could not find test player actor type.",
         actorType != NULL);
      
      proxy = mGameManager->CreateActor(*actorType);
      CPPUNIT_ASSERT_MESSAGE("Could not create test player actor.",
         actorType != NULL);
         
      gameProxy =  dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      mGameManager->AddActor(*gameProxy,false,false);     
      
      dtCore::System::Instance()->Step();
      SLEEP(10);
      
      logController->RequestChangeStateToRecord(); 
      dtCore::System::Instance()->Step();
      SLEEP(10);
            
      //At this point, there should be one keyframe.
      logController->RequestServerGetKeyframes();
      dtCore::System::Instance()->Step();
      SLEEP(10);
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("Should have one keyframe.",kfList.size() == 1);      
      
      if (rejectMsgSignal->mRejectMessage != NULL)
      {
         if (rejectMsgSignal->mRejectMessage->GetCausingMessage() != NULL)
            rejectMsgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);              
         
         CPPUNIT_ASSERT_MESSAGE("Received error message from server before requesting "
            "first keyframe: " + errorMsg, rejectMsgSignal->mRejectMessage != NULL);
      }
            
      proxy = mGameManager->CreateActor(*actorType);
      gameProxy =  dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      mGameManager->AddActor(*gameProxy,false,false);           
      dtCore::System::Instance()->Step();
      SLEEP(10);
            
      dtGame::LogKeyframe keyFrame;
      keyFrame.SetName("bob_keyframe");
      keyFrame.SetDescription("bob_description");
      keyFrame.SetActiveMap("bob_map");           
      logController->RequestCaptureKeyframe(keyFrame);
      dtCore::System::Instance()->Step();
      SLEEP(10);
      
      //Now there should be two keyframes.
      logController->RequestServerGetKeyframes();
      dtCore::System::Instance()->Step();
      SLEEP(10);
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("Should have two keyframes.",kfList.size() == 2);            
      
      if (rejectMsgSignal->mRejectMessage != NULL)
      {
         if (rejectMsgSignal->mRejectMessage->GetCausingMessage() != NULL)
            rejectMsgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);              
         
         CPPUNIT_ASSERT_MESSAGE("Received error message from server before requesting "
            "second keyframe: " + errorMsg, rejectMsgSignal->mRejectMessage != NULL);
      }
      
      logController->RequestChangeStateToIdle();
      mGameManager->DeleteAllActors();
      SLEEP(10);
      dtCore::System::Instance()->Step();

      logController->RequestChangeStateToRecord();
      SLEEP(10);
      dtCore::System::Instance()->Step();
      
      proxy = mGameManager->CreateActor(*actorType);
      gameProxy =  dynamic_cast<dtGame::GameActorProxy*>(proxy.get());
      mGameManager->AddActor(*gameProxy,false,false);           
      dtCore::System::Instance()->Step();
      SLEEP(10);
      
      //Now go through a loop adding keyframe after keyframe...
      int i;
      std::string loopCount;
      for (i=0; i<100; i++)
      {
         loopCount = dtUtil::ToString(i);
         keyFrame.SetName("bob_keyframe" + loopCount);
         keyFrame.SetDescription("bob_description" + loopCount);
         keyFrame.SetActiveMap("bob_map" + loopCount);           
         logController->RequestCaptureKeyframe(keyFrame);
         dtCore::System::Instance()->Step();
         SLEEP(10);
         
         if (rejectMsgSignal->mRejectMessage != NULL)
         {
            if (rejectMsgSignal->mRejectMessage->GetCausingMessage() != NULL)
               rejectMsgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);              
            
            CPPUNIT_ASSERT_MESSAGE("Received error message from server while generating keyframe " + 
               loopCount + errorMsg, rejectMsgSignal->mRejectMessage != NULL);
         }
      }
      
      //Verify the long list of keyframes..
      logController->RequestServerGetKeyframes();
      dtCore::System::Instance()->Step();
      SLEEP(10);
      kfList = logController->GetLastKnownKeyframeList();

      //Should be 101 because we just captured 100 keyframes plus the keyframe captured when
      //recording began.
      CPPUNIT_ASSERT_MESSAGE("Should have 101 keyframes.",kfList.size() == 101); 
      
      //Check the keyframe data to make sure they all got sent around properly.           
      for (i=1; i<101; i++)
      {
         loopCount = dtUtil::ToString(i-1);
         dtGame::LogKeyframe &k = kfList[i];
         CPPUNIT_ASSERT_MESSAGE("Names were not equal on keyframe " + loopCount,
            k.GetName() == std::string("bob_keyframe" + loopCount));
         CPPUNIT_ASSERT_MESSAGE("Descriptions were not equal on keyframe " + loopCount,
            k.GetDescription() == std::string("bob_description" + loopCount));
         CPPUNIT_ASSERT_MESSAGE("Active map names were not equal on keyframe " + loopCount,
            k.GetActiveMap() == std::string("bob_map" + loopCount));
      }
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::CompareKeyframeLists(const std::vector<dtGame::LogKeyframe> listOne,
   const std::vector<dtGame::LogKeyframe> listTwo)
{
   std::vector<dtGame::LogKeyframe>::const_iterator itor,itor2;
 
   CPPUNIT_ASSERT_MESSAGE("Keyframe list sizes should be equal.",listOne.size() == listTwo.size());
   
   for (itor=listOne.begin(), itor2=listTwo.begin(); 
      itor!=listOne.end() && itor2!=listTwo.end(); ++itor,++itor2)
   {
      const dtGame::LogKeyframe &first = *itor;
      const dtGame::LogKeyframe &second = *itor2;
      
      CPPUNIT_ASSERT_MESSAGE("Keyframe names were not equal.  Should be: " + 
         first.GetName() + " but was " + second.GetName(),first.GetName() == second.GetName());
      CPPUNIT_ASSERT_MESSAGE("Keyframe descriptions were not equal.  Should be: " + 
         first.GetDescription() + " but was " + second.GetDescription(),
         first.GetDescription() == second.GetDescription());
      CPPUNIT_ASSERT_MESSAGE("Keyframe active maps were not equal.  Should be: " + 
         first.GetActiveMap() + " but was " + second.GetActiveMap(),first.GetActiveMap() == second.GetActiveMap());
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
      
      for (unsigned int i=0; i<10; i++)
      {
         std::string num = dtUtil::ToString(i);
         dtGame::LogKeyframe kf;
         kf.SetName("FrameOne" + num);
         kf.SetDescription("Description One" + num);
         kf.SetSimTimeStamp(i*10.0f);
         kf.SetUniqueId("UniqueID" + num);
         kf.SetActiveMap("ActiveMap" + num);
         kf.SetTagUniqueId("TagUniqueID" + num);
         testList.push_back(kf);
      }
      
      msg->SetKeyframeList(testList);
      resultList = msg->GetKeyframeList();
      
      CPPUNIT_ASSERT_MESSAGE("Keyframe list size should be 10.",resultList.size() == 10);
      CompareKeyframeLists(testList,resultList);
      
      dtCore::RefPtr<dtGame::LogGetKeyframeListMessage> otherMsg = 
         new dtGame::LogGetKeyframeListMessage();
      
      std::string str;
      dtGame::DataStream ds;
      
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
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::CompareTagLists(const std::vector<dtGame::LogTag> listOne,
   const std::vector<dtGame::LogTag> listTwo)
{
   std::vector<dtGame::LogTag>::const_iterator itor,itor2;
 
   CPPUNIT_ASSERT_MESSAGE("Tag list sizes should be equal.",listOne.size() == listTwo.size());
   
   for (itor=listOne.begin(), itor2=listTwo.begin(); 
      itor!=listOne.end() && itor2!=listTwo.end(); ++itor,++itor2)
   {
      const dtGame::LogTag &first = *itor;
      const dtGame::LogTag &second = *itor2;
      
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
      
      for (unsigned int i=0; i<10; i++)
      {
         std::string num = dtUtil::ToString(i);
         dtGame::LogTag tag;
         tag.SetName("Tag" + num);
         tag.SetDescription("Description" + num);
         tag.SetSimTimeStamp(i*10.0f);
         tag.SetUniqueId("UniqueID" + num);
         
         if ((i%2) == 0)
            tag.SetCaptureKeyframe(true);
         else
            tag.SetCaptureKeyframe(false);
            
         tag.SetKeyframeUniqueId("KeyframeUniqueID" + num);
         testList.push_back(tag);
      }
      
      msg->SetTagList(testList);
      resultList = msg->GetTagList();
      
      CPPUNIT_ASSERT_MESSAGE("Tag list size should be 10.",resultList.size() == 10);
      CompareTagLists(testList,resultList);
      
      dtCore::RefPtr<dtGame::LogGetTagListMessage> otherMsg = 
         new dtGame::LogGetTagListMessage();
      
      std::string str;
      dtGame::DataStream ds;
      
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
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerGetTags()
{
   dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
   dtCore::RefPtr<dtGame::BinaryLogStream> stream = NULL;
   dtCore::RefPtr<dtGame::ServerLoggerComponent> serverLoggerComp = NULL;
   dtCore::RefPtr<dtGame::LogController> logController = NULL;
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
      dtCore::System::Instance()->Step();
      SLEEP(10);
      tagList = logController->GetLastKnownTagList();
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("There should not be any tags in the log when first started.",
         tagList.empty() == true);
      CPPUNIT_ASSERT_MESSAGE("There should not be any keyframes in the log when first started.",
         kfList.empty() == true);
      
      logController->RequestChangeStateToRecord();
      dtCore::System::Instance()->Step();
      SLEEP(10);    
      
      for (i=0; i<20; i++)
      {
         dtGame::LogTag tag;
         std::string count = dtUtil::ToString(i);
         tag.SetName("Tag" + count);
         tag.SetDescription("TagDescription" + count);
         tag.SetSimTimeStamp(i*10);
         tag.SetUniqueId(dtCore::UniqueId("TagUniqueId" + count));
         if ((i%2) == 0)
            tag.SetCaptureKeyframe(true);
         else
            tag.SetCaptureKeyframe(false);
            
         logController->RequestInsertTag(tag);
         dtCore::System::Instance()->Step();
         SLEEP(10);
      }
      
      logController->RequestChangeStateToIdle();      
      dtCore::System::Instance()->Step();
      SLEEP(10);
            
      logController->RequestChangeStateToPlayback();
      SLEEP(10);
      
      //Make sure tags and keyframes got recorded properly.
      logController->RequestServerGetTags();
      logController->RequestServerGetKeyframes();
      dtCore::System::Instance()->Step();
      SLEEP(10);
      tagList = logController->GetLastKnownTagList();
      kfList = logController->GetLastKnownKeyframeList();
      
      CPPUNIT_ASSERT_MESSAGE("There should be 20 tags in the log.",tagList.size() == 20);
      CPPUNIT_ASSERT_MESSAGE("There should be 11 keyframes in the log.",kfList.size() == 11);           
            
      //Make sure we did not get any error messages during the keyframe capture.
      if (msgSignal->mRejectMessage != NULL)
      {
         if (msgSignal->mRejectMessage->GetCausingMessage() != NULL)
            msgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);              
         
         CPPUNIT_ASSERT_MESSAGE("Received error message while auto capturing keyframes " + 
            errorMsg, msgSignal->mRejectMessage != NULL);
      }
      
      //Well we seem to have gotten valid data, but lets make sure.
      for (i=0; i<20; i++)
      {
         std::string count = dtUtil::ToString(i);
         
         CPPUNIT_ASSERT_MESSAGE("Tag name was not correct.",tagList[i].GetName() == std::string("Tag" + count));
         CPPUNIT_ASSERT_MESSAGE("Tag description was not correct.",
            tagList[i].GetDescription() == std::string("TagDescription" + count));
         CPPUNIT_ASSERT_MESSAGE("Tag sim time was not correct",tagList[i].GetSimTimeStamp() == i*10);
         CPPUNIT_ASSERT_MESSAGE("Tag unique id was not correct",
            tagList[i].GetUniqueId() == std::string("TagUniqueId" + count));
         if ((i%2) == 0)
            CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe was not correct.",
               tagList[i].GetCaptureKeyframe() == true);
         else
            CPPUNIT_ASSERT_MESSAGE("Tag capture keyframe was not correct.",
               tagList[i].GetCaptureKeyframe() == false);
      }
      
      //Make sure the keyframes/tags cross references were assigned correctly.
      int kfCount = 1;
      for (i=0; i<20; i+=2)
      {
         CPPUNIT_ASSERT_MESSAGE("Tag " + dtUtil::ToString(i) + " did not have the correct keyframe id.",
            tagList[i].GetKeyframeUniqueId() == kfList[kfCount].GetUniqueId());
         
         CPPUNIT_ASSERT_MESSAGE("Keyframe + " + dtUtil::ToString(i) + " did not have the correct tag id.",
            kfList[kfCount].GetTagUniqueId() == tagList[i].GetUniqueId());
            
         kfCount++;
      }
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
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
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = NULL;
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
      dtCore::System::Instance()->Step();
      SLEEP(10);
      
      //First make sure the user cannot set the interval to negative time..
      msgSignal->Reset();
      logController->RequestSetAutoKeyframeInterval(-10.0);
      dtCore::System::Instance()->Step();
      SLEEP(10);
      CPPUNIT_ASSERT_MESSAGE("Should not be allowed to set a negative auto keyframe interval",
         msgSignal->mRejectionSignalReceived);      

      msgSignal->Reset();
      logController->RequestSetAutoKeyframeInterval(0.01f);
      dtCore::System::Instance()->Step();
      SLEEP(10);

      logController->RequestChangeStateToRecord();
      SLEEP(10);
      dtCore::System::Instance()->Step();
      
      logController->RequestServerGetKeyframes();
      dtCore::System::Instance()->Step();
      SLEEP(10);
      kfList = logController->GetLastKnownKeyframeList();
      CPPUNIT_ASSERT_MESSAGE("Should have one keyframe.",kfList.size() == 1);  
      
      double startTime = mGameManager->GetSimulationTime() * 1000.0;
      double endTime;
      int i = 0;
      while (i < 1000)
      {
         dtCore::System::Instance()->Step();
         ++i;
         
         //Make sure we did not get any error messages during the keyframe capture.
         if (msgSignal->mRejectMessage != NULL)
         {
            if (msgSignal->mRejectMessage->GetCausingMessage() != NULL)
               msgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);              
            
            CPPUNIT_ASSERT_MESSAGE("Received error message while auto capturing keyframes " + 
               errorMsg, msgSignal->mRejectMessage != NULL);
         }         
      }
      endTime = mGameManager->GetSimulationTime() * 1000.0;
      unsigned int numKeyFrames = (unsigned int)((endTime-startTime) / 10.0);
      SLEEP(10);
      
      logController->RequestServerGetKeyframes();
      dtCore::System::Instance()->Step();
      SLEEP(10);
      dtCore::System::Instance()->Step();
      SLEEP(10);
      kfList = logController->GetLastKnownKeyframeList();

      std::ostringstream ss;      
      ss << "Total simulation time elapsed was: " << endTime-startTime << " milliseconds."
         << " Should have gotten " << numKeyFrames << " keyframes during that amount of time "
         "but instead got " << kfList.size()-1 << " keyframes.";
      
      //Do not assert direct equality since there could have been slight error
      //introduced if the elapsed time was in fact a multiple of 10 millis but 
      //the code had not yet executed for the timer to fire. 
      CPPUNIT_ASSERT_MESSAGE(ss.str(),(kfList.size() - 1) == numKeyFrames);

      logController->RequestChangeStateToIdle();
      dtCore::System::Instance()->Step();
      SLEEP(10);
                  
      logController->RequestChangeStateToPlayback();
      dtCore::System::Instance()->Step();
      SLEEP(10);
      
      for (i=0; i<5000; i++)
         dtCore::System::Instance()->Step();
        
      if (msgSignal->mRejectMessage != NULL)
      {
         if (msgSignal->mRejectMessage->GetCausingMessage() != NULL)
            msgSignal->mRejectMessage->GetCausingMessage()->ToString(errorMsg);              
         
         CPPUNIT_ASSERT_MESSAGE("Received error message while auto capturing keyframes " + 
            errorMsg, msgSignal->mRejectMessage != NULL);
      }      
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLoggerKeyframeMessage()
{
   try
   {
       dtCore::RefPtr<dtGame::LogCaptureKeyframeMessage> captureKeyframeMsg = new dtGame::LogCaptureKeyframeMessage();

      // test default 0 values 

      dtGame::LogKeyframe emptyKeyframe;
      CPPUNIT_ASSERT(emptyKeyframe.GetSimTimeStamp() == 0.0);
      CPPUNIT_ASSERT(emptyKeyframe.GetLogFileOffset() == 0.0);

      // test basic set/get behavior on keyframe        
      CPPUNIT_ASSERT_MESSAGE("Keyframe should be able to set/get name", keyframe.GetName() == "myName");
      CPPUNIT_ASSERT_MESSAGE("Keyframe should be able to set/get description", keyframe.GetDescription() == "myDescription");
      CPPUNIT_ASSERT_MESSAGE("Keyframe should be able to set/get sim time stamp", osg::equivalent(d1, keyframe.GetSimTimeStamp(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Keyframe should be able to set/get map", keyframe.GetActiveMap() == "myMap");
      CPPUNIT_ASSERT_MESSAGE("Keyframe should be able to set/get log file offset", long2 == keyframe.GetLogFileOffset());

      // test copy constructor of keyframe
      dtGame::LogKeyframe keyframe2(keyframe);
      CPPUNIT_ASSERT_MESSAGE("Keyframe Copy Constructor should be able to set name", keyframe2.GetName() == "myName");
      CPPUNIT_ASSERT_MESSAGE("Keyframe Copy Constructor should be able to set description", keyframe2.GetDescription() == "myDescription");
      CPPUNIT_ASSERT_MESSAGE("Keyframe Copy Constructor should be able to set uniqueid", keyframe2.GetUniqueId() == keyframe.GetUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Keyframe Copy Constructor should be able to set sim time stamp", osg::equivalent(d1, keyframe2.GetSimTimeStamp(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Keyframe Copy Constructor should be able to set map", keyframe2.GetActiveMap() == "myMap");
      CPPUNIT_ASSERT_MESSAGE("Keyframe Copy Constructor should be able to set tag unique id", keyframe.GetTagUniqueId() == keyframe2.GetTagUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Keyframe Copy Constructor should be able to set log file offset", long2 == keyframe2.GetLogFileOffset());

      // test assignment of keyframe

      dtGame::LogKeyframe keyframe3;
      keyframe3 = keyframe2;
      CPPUNIT_ASSERT_MESSAGE("Keyframe Assignment(=) should be able to set name", keyframe3.GetName() == "myName");
      CPPUNIT_ASSERT_MESSAGE("Keyframe Assignment(=) should be able to set description", keyframe3.GetDescription() == "myDescription");
      CPPUNIT_ASSERT_MESSAGE("Keyframe Assignment(=) should be able to set uniqueid", keyframe3.GetUniqueId() == keyframe2.GetUniqueId());
      CPPUNIT_ASSERT_MESSAGE("Keyframe Assignment(=) should be able to set sim time stamp", osg::equivalent(d1, keyframe3.GetSimTimeStamp(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Keyframe Assignment(=) should be able to set map", keyframe3.GetActiveMap() == "myMap");
      CPPUNIT_ASSERT_MESSAGE("Keyframe Assignment(=) should be able to set log file offset", long2 == keyframe3.GetLogFileOffset());
      CPPUNIT_ASSERT_MESSAGE("Keyframe Assignment(=) should be able to set tag unique id", keyframe.GetTagUniqueId() == keyframe3.GetTagUniqueId());
      // test equality ---  <, >, !=, ==

      dtGame::LogKeyframe keyframe4; // 4 will have a different unique id.
      CPPUNIT_ASSERT_MESSAGE("Keyframe should support !=", keyframe2 != keyframe4);
      CPPUNIT_ASSERT_MESSAGE("Keyframe should support ==", keyframe2 == keyframe3);
      CPPUNIT_ASSERT_MESSAGE("Keyframe should support >", // can't check id's since they are generated, but can check the compare
         (keyframe4 > keyframe2) == (keyframe4.GetUniqueId() > keyframe2.GetUniqueId()));
      CPPUNIT_ASSERT_MESSAGE("Keyframe should support <", // can't check id's since they are generated, but can check the compare
         (keyframe2 < keyframe4) == (keyframe2.GetUniqueId() < keyframe4.GetUniqueId()));

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
         keyframe.GetActiveMap() == keyframe5.GetActiveMap());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct tag "
         "uniqueid.",keyframe.GetTagUniqueId() == keyframe5.GetTagUniqueId());      
      
      // note, file offset is not sent on the message.  Internal to server
      //CPPUNIT_ASSERT_MESSAGE("Get/Set LogKeyframe on LogCaptureKeyframeMessage should have correct log file offset", 
      //   osg::equivalent(keyframe.GetLogFileOffset(), keyframe5.GetLogFileOffset(), 1e-2));

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
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
      CPPUNIT_ASSERT(emptyStatus.GetEstPlaybackTimeRemaining() == 0.0);
      CPPUNIT_ASSERT(emptyStatus.GetCurrentRecordDuration() == 0.0);
      CPPUNIT_ASSERT(emptyStatus.GetNumRecordedMessages() == 0);

      // test basic set/gets

      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get state enum", status.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get sim time", osg::equivalent(d1, status.GetCurrentSimTime(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get keyframe interval", osg::equivalent(d2, status.GetAutoRecordKeyframeInterval(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get playback remaining", osg::equivalent(d3, status.GetEstPlaybackTimeRemaining(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get record duration", osg::equivalent(d4, status.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get num recorded messages", status.GetNumRecordedMessages() == long1);
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get active map", status.GetActiveMap() == "myMap");
      CPPUNIT_ASSERT_MESSAGE("Status should be able to set/get log file", status.GetLogFile() == "myLogFile");

      // test copy constructor of status

      dtGame::LogStatus status2(status);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set state enum", status2.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set sim time", osg::equivalent(d1, status2.GetCurrentSimTime(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set keyframe interval", osg::equivalent(d2, status2.GetAutoRecordKeyframeInterval(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set playback remaining", osg::equivalent(d3, status2.GetEstPlaybackTimeRemaining(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set record duration", osg::equivalent(d4, status2.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set num recorded messages", status2.GetNumRecordedMessages() == long1);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set active map", status2.GetActiveMap() == "myMap");
      CPPUNIT_ASSERT_MESSAGE("LogStatus Copy Constructor should be able to set log file", status2.GetLogFile() == "myLogFile");

      // test assignment of status

      dtGame::LogStatus status3;
      status3 = status2;
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set state enum", status3.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set sim time", osg::equivalent(d1, status3.GetCurrentSimTime(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set keyframe interval", osg::equivalent(d2, status3.GetAutoRecordKeyframeInterval(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set playback remaining", osg::equivalent(d3, status3.GetEstPlaybackTimeRemaining(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set record duration", osg::equivalent(d4, status3.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set num recorded messages", status3.GetNumRecordedMessages() == long1);
      CPPUNIT_ASSERT_MESSAGE("LogStatus Assignment(=) should be able to set active map", status3.GetActiveMap() == "myMap");
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
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct playback remaining", 
         osg::equivalent(status.GetEstPlaybackTimeRemaining(), status4.GetEstPlaybackTimeRemaining(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct record duration", 
         osg::equivalent(status.GetCurrentRecordDuration(), status4.GetCurrentRecordDuration(), 1e-2));
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct num recorded messages", 
         status.GetNumRecordedMessages() == status4.GetNumRecordedMessages());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct active map", 
         status.GetActiveMap() == status4.GetActiveMap());
      CPPUNIT_ASSERT_MESSAGE("Get/Set LogStatus on LogStatusMessage should have correct log file", 
         status.GetLogFile() == status4.GetLogFile());
   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
   {
      CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   }
}

//////////////////////////////////////////////////////////////////////////
void GMLoggerTests::TestLogControllerComponent()
{
   try
   {
      dtCore::RefPtr<TestComponent> tc = new TestComponent();
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
      double deltaTime[2] = {0.3, 0.3}; 
      dtCore::System::Instance()->SendMessage("preframe", &deltaTime);

      // validate them.
      for (unsigned i = 0; i < tc->GetReceivedProcessMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedProcessMessages()[i].valid());
      } 
      for (unsigned i = 0; i < tc->GetReceivedSendMessages().size(); ++i)
      {
         CPPUNIT_ASSERT(tc->GetReceivedSendMessages()[i].valid());
      } 

      // find the processed (processMessage) Messages
      dtCore::RefPtr<const dtGame::Message> temp;

      dtCore::RefPtr<const dtGame::Message> procReqStatePlayback = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_PLAYBACK);
      dtCore::RefPtr<const dtGame::Message> procReqStateRecord = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_RECORD);
      dtCore::RefPtr<const dtGame::Message> procReqStateIdle = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_IDLE);
      dtCore::RefPtr<const dtGame::LogCaptureKeyframeMessage> procCaptureKeyframeMsg = 
         (const dtGame::LogCaptureKeyframeMessage *)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_CAPTURE_KEYFRAME)).get();
      dtCore::RefPtr<const dtGame::Message> procReqGeyKeyframes = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_KEYFRAMES);
      dtCore::RefPtr<const dtGame::Message> procReqGetLogfiles = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_LOGFILES);
      dtCore::RefPtr<const dtGame::Message> procReqGetTags = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_TAGS);
      dtCore::RefPtr<const dtGame::Message> procReqGetStatus = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_GET_STATUS);
      dtCore::RefPtr<const dtGame::LogInsertTagMessage> procInsertTagMsg = 
         (const dtGame::LogInsertTagMessage *)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_INSERT_TAG)).get();
      dtCore::RefPtr<const dtGame::LogDeleteLogfileMessage> procDeleteLogMsg = 
         (const dtGame::LogDeleteLogfileMessage *)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_DELETE_LOG)).get();
      dtCore::RefPtr<const dtGame::LogSetLogfileMessage> procSetLogMsg = 
         (const dtGame::LogSetLogfileMessage *)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_SET_LOGFILE)).get();
      dtCore::RefPtr<const dtGame::LogSetAutoKeyframeIntervalMessage> procSetKeyframeIntMsg = 
         (const dtGame::LogSetAutoKeyframeIntervalMessage *)( tc->FindProcessMessageOfType(dtGame::MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL)).get();

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

      dtCore::RefPtr<const dtGame::Message> sentReqStatePlayback = tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_PLAYBACK);
      dtCore::RefPtr<const dtGame::Message> sentReqStateRecord = tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_RECORD);
      dtCore::RefPtr<const dtGame::Message> sentReqStateIdle = tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_CHANGESTATE_IDLE);
      dtCore::RefPtr<const dtGame::LogCaptureKeyframeMessage> sentCaptureKeyframeMsg = 
         (const dtGame::LogCaptureKeyframeMessage *)( tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_CAPTURE_KEYFRAME)).get();
      dtCore::RefPtr<const dtGame::Message> sentReqGeyKeyframes = tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_GET_KEYFRAMES);
      dtCore::RefPtr<const dtGame::Message> sentReqGetLogfiles = tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_GET_LOGFILES);
      dtCore::RefPtr<const dtGame::Message> sentReqGetTags = tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_GET_TAGS);
      dtCore::RefPtr<const dtGame::Message> sentReqGetStatus = tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_GET_STATUS);
      dtCore::RefPtr<const dtGame::LogInsertTagMessage> sentInsertTagMsg = 
         (const dtGame::LogInsertTagMessage *)( tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_INSERT_TAG)).get();
      dtCore::RefPtr<const dtGame::LogDeleteLogfileMessage> sentDeleteLogMsg = 
         (const dtGame::LogDeleteLogfileMessage *)( tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_DELETE_LOG)).get();
      dtCore::RefPtr<const dtGame::LogSetLogfileMessage> sentSetLogMsg = 
         (const dtGame::LogSetLogfileMessage *)( tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_SET_LOGFILE)).get();
      dtCore::RefPtr<const dtGame::LogSetAutoKeyframeIntervalMessage> sentSetKeyframeIntMsg = 
         (const dtGame::LogSetAutoKeyframeIntervalMessage *)( tc->FindSendMessageOfType(dtGame::MessageType::LOG_REQ_SET_AUTOKEYFRAMEINTERVAL)).get();
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
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
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
      dtGame::LogStatusMessage *pMsg = static_cast<dtGame::LogStatusMessage *> (statusMessage.get());
      pMsg->SetStatus(status);
      mGameManager->ProcessMessage(*statusMessage);
      //mGameManager->SendMessage(*statusMessage);
      double deltaTime[2] = {0.3, 0.3}; 
      dtCore::System::Instance()->SendMessage("preframe", &deltaTime); // let the GM send 'em.

      // check the results

      CPPUNIT_ASSERT_MESSAGE("Log Controller should send a status signal when receiving message LOG_INFO_STATUS.", 
         testSignal->mStatusSignalReceived); 
      dtGame::LogStatus resultStatus = testSignal->mStatus;
      CPPUNIT_ASSERT_MESSAGE("Log Controller should have status data from the signal for message LOG_INFO_STATUS.", 
         (status.GetActiveMap() == resultStatus.GetActiveMap()) && (status.GetCurrentRecordDuration() == resultStatus.GetCurrentRecordDuration()));

      // add tests for the log_info keyframes, tags, and logs

      // test the status message getting to the controller
      dtGame::LogStatus contStatus = logController->GetLastKnownStatus();
      CPPUNIT_ASSERT_MESSAGE("Log Controller should have a copy of the last known status from the last LOG_INFO_STATUS message.", 
         (status.GetActiveMap() == contStatus.GetActiveMap()) && (status.GetCurrentRecordDuration() == contStatus.GetCurrentRecordDuration()));

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
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
      dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
      dtCore::RefPtr<TestLogStream> testStream = new TestLogStream(msgFactory, mGameManager);
      dtCore::RefPtr<dtGame::ServerLoggerComponent> serverController = 
         new dtGame::ServerLoggerComponent(*testStream.get());
      dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
      dtCore::RefPtr<TestComponent> tc = new TestComponent();
      dtCore::RefPtr<TestControllerSignal> testSignal = new TestControllerSignal();
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*serverController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::HIGHEST);
      testSignal->RegisterSignals(*logController);
      testSignal->Reset();
      tc->reset();

      // FULL SUITE TEST!!! From logger controller, to gm, to server controller, back to gm, to controller, and to signal tester

      logController->RequestServerGetStatus();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Request Status Msg should have caused a Status response message.", 
         testSignal->mStatusSignalReceived); 

      // Try to change to Idle while already in idle - should send status just to us

      testSignal->Reset();
      tc->reset();
      logController->RequestChangeStateToIdle();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Idle (when in idle) should have caused a Status response message.", 
         testSignal->mStatusSignalReceived); 
      dtCore::RefPtr<const dtGame::Message> status2 = tc->FindProcessMessageOfType(dtGame::MessageType::LOG_INFO_STATUS);
      CPPUNIT_ASSERT_MESSAGE("Request Change State to idle (when in idle) should have sent status just to me", 
         (status2->GetDestination() == &mGameManager->GetMachineInfo()));

      // Set the Log File and see if it gets back in a status

      testSignal->Reset();
      CPPUNIT_ASSERT_MESSAGE("A silly check for Test Signal - should have received signal status flag = false.  Would stink to have someone break signal class and all our tests are bogus", !testSignal->mStatusSignalReceived); 
      logController->RequestSetLogFile("UnitTestLoggerFile");
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Set Log File should cause a Status message.", 
         testSignal->mStatusSignalReceived); 
      CPPUNIT_ASSERT_MESSAGE("Set Log File should return log file in status", 
         testSignal->mStatus.GetLogFile() == "UnitTestLoggerFile");

      // change the server state to RECORD

      msgCount = 0;
      testSignal->Reset();
      logController->RequestChangeStateToRecord();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Record should have caused a Status response message.", 
         testSignal->mStatusSignalReceived); 
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Record should have sent status with RECORD.", 
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      msgCount = testSignal->mStatus.GetNumRecordedMessages();
      // force at least one extra message to check that # count goes up
      logController->RequestServerGetStatus();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("We should have recorded a msg", msgCount < testSignal->mStatus.GetNumRecordedMessages());

      // try to set the log file while in Record mode, make sure it fails.

      testSignal->Reset();
      logController->RequestSetLogFile("ERRORSET");
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Set Log File with error should NOT cause a Status message.", !testSignal->mStatusSignalReceived); 
      // should have gotten a rejection message
      const dtGame::Message *causeMsg2 = testSignal->mRejectMessage->GetCausingMessage();
      CPPUNIT_ASSERT_MESSAGE("Set Log File with error should have gotten reject message.", 
         (testSignal->mRejectMessage->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED) && 
         (causeMsg2 != NULL) && (causeMsg2->GetMessageType() == dtGame::MessageType::LOG_REQ_SET_LOGFILE));
      // make sure the bad set log file didn't change the log file name on the server component
      testSignal->Reset();
      logController->RequestServerGetStatus();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Failed Set Log File should not change file name ", 
         testSignal->mStatus.GetLogFile() == "UnitTestLoggerFile");
         
      // should we check setting log file while in playback mode too? or overkill?

      // check the request change state - IDLE - message

      testSignal->Reset();
      logController->RequestChangeStateToIdle();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Valid Request Change State to Idle should have caused a Status response message.", 
         testSignal->mStatusSignalReceived); 
      CPPUNIT_ASSERT_MESSAGE("Valid Request Change State to Idle should have sent status with IDLE.", 
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);


      // check the ability to change to IDLE that also has an exception

      // first, put into record state.
      testSignal->Reset();
      logController->RequestChangeStateToRecord();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Request Change State to Record should have sent status with RECORD.", 
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_RECORD);
      // now, force exception and switch back to idle
      testSignal->Reset();
      testStream->mExceptionEnabled = true;  // force exception
      logController->RequestChangeStateToIdle();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      // do the checks
      CPPUNIT_ASSERT_MESSAGE("With exception, should still get Status message.", testSignal->mStatusSignalReceived);
      CPPUNIT_ASSERT_MESSAGE("With exception, should still get reject message.", testSignal->mRejectionSignalReceived);
      const dtGame::Message *causeMsg3 = testSignal->mRejectMessage->GetCausingMessage();
      CPPUNIT_ASSERT_MESSAGE("Change to Idle with error should have gotten reject message.", 
         (testSignal->mRejectMessage->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED) && 
         (causeMsg3 != NULL) && (causeMsg3->GetMessageType() == dtGame::MessageType::LOG_REQ_CHANGESTATE_IDLE));

      // Test setting to RECORD that fails with an exception

      // first, put into record state.
      testSignal->Reset();
      testStream->mExceptionEnabled = true;  // force exception
      logController->RequestChangeStateToRecord();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Failed Record (exception) should still get Status message.", testSignal->mStatusSignalReceived);
      CPPUNIT_ASSERT_MESSAGE("Failed Record (exception) should have sent status with IDLE.", 
         testSignal->mStatus.GetStateEnum() == dtGame::LogStateEnumeration::LOGGER_STATE_IDLE);
      CPPUNIT_ASSERT_MESSAGE("Failed Record (exceptio), should get reject message.", testSignal->mRejectionSignalReceived);
      const dtGame::Message *causeMsg4 = testSignal->mRejectMessage->GetCausingMessage();
      CPPUNIT_ASSERT_MESSAGE("Change to Idle with error should have gotten reject message.", 
         (testSignal->mRejectMessage->GetMessageType() == dtGame::MessageType::SERVER_REQUEST_REJECTED) && 
         (causeMsg4 != NULL) && (causeMsg4->GetMessageType() == dtGame::MessageType::LOG_REQ_CHANGESTATE_RECORD));

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   catch (const std::exception &e)
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
      dtGame::MessageFactory &msgFactory = mGameManager->GetMessageFactory();
      dtCore::RefPtr<TestLogStream> testStream = new TestLogStream(msgFactory, mGameManager);
      dtCore::RefPtr<dtGame::ServerLoggerComponent> serverController = 
         new dtGame::ServerLoggerComponent(*testStream.get());
      dtCore::RefPtr<dtGame::LogController> logController = new dtGame::LogController();
      dtCore::RefPtr<TestComponent> tc = new TestComponent();
      dtCore::RefPtr<TestControllerSignal> testSignal = new TestControllerSignal();
      mGameManager->AddComponent(*logController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*serverController, dtGame::GameManager::ComponentPriority::NORMAL);
      mGameManager->AddComponent(*tc, dtGame::GameManager::ComponentPriority::NORMAL);
      testSignal->RegisterSignals(*logController);
      testSignal->Reset();
      tc->reset();

      // Try changing map, then request a status, see if status has correct map

      testSignal->Reset();
      message = mGameManager->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_MAP_LOADED);
      dtGame::MapLoadedMessage *mapLoadedMsg = (dtGame::MapLoadedMessage *) message.get();
      mapLoadedMsg->SetLoadedMapName("MyBogusMapName");
      mGameManager->ProcessMessage(*mapLoadedMsg);
      logController->RequestServerGetStatus();
      SLEEP(10); // tick the GM so it can send the messages
      dtCore::System::Instance()->Step();
      CPPUNIT_ASSERT_MESSAGE("Change map msg should change map on logstatus", testSignal->mStatus.GetActiveMap() == "MyBogusMapName");

   }
   catch (const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
   //catch (const std::exception &e)
   //{
   //   CPPUNIT_FAIL(std::string("Caught exception of type: ") + typeid(e).name() + " " + e.what());
   //}
}

