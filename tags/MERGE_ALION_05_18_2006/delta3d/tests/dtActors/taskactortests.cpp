/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
#include <cppunit/extensions/HelperMacros.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtActors/taskactor.h>
#include <dtCore/globals.h>
#include <dtUtil/stringutils.h>

#include <vector>

/**
 * This test suite tests the base task actor proxy as well as the different
 * task subclasses.
 */
class TaskActorTests : public CPPUNIT_NS::TestFixture 
{
   CPPUNIT_TEST_SUITE(TaskActorTests);
      CPPUNIT_TEST(TestTaskActorDefaultValues);
      CPPUNIT_TEST(TestTaskSubTasks);
      CPPUNIT_TEST(TestTaskReparentOnAdd);
   CPPUNIT_TEST_SUITE_END();

public:
   void setUp();
   void tearDown();
   
   void TestTaskActorDefaultValues();
   void TestTaskSubTasks();
   void TestTaskReparentOnAdd();

private:
   dtDAL::LibraryManager *mLibraryManager;
};

//Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(TaskActorTests);


///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::setUp()
{
   try
   {
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
      mLibraryManager = &dtDAL::LibraryManager::GetInstance();
      CPPUNIT_ASSERT_MESSAGE("Library manager was invalid.",mLibraryManager != NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }    
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::tearDown()
{
   mLibraryManager = NULL;
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskActorDefaultValues()
{
   try
   {
      dtCore::RefPtr<dtDAL::ActorType> taskActorType = 
         mLibraryManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());
      
      dtCore::RefPtr<dtDAL::ActorProxy> proxy = mLibraryManager->CreateActorProxy(*taskActorType);
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",proxy.valid());
      
      //Make sure the correct properties exist on the proxy.
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a description property.",
         proxy->GetProperty("Description") != NULL);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a passing score property.",
         proxy->GetProperty("PassingScore") != NULL);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a score property.",
         proxy->GetProperty("Score") != NULL);
      CPPUNIT_ASSERT_MESSAGE("Task actor should have a weight property.",
         proxy->GetProperty("Weight") != NULL);
         
      //Check the default values of a newly created base task actor.    
      dtDAL::StringActorProperty *descProp = 
         static_cast<dtDAL::StringActorProperty *>(proxy->GetProperty("Description"));
      CPPUNIT_ASSERT_MESSAGE("Task description should be empty.",descProp->GetValue().empty());
      
      dtDAL::FloatActorProperty *valueProp = 
         static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("PassingScore"));
      CPPUNIT_ASSERT_MESSAGE("Task passing score should be 1.0.",valueProp->GetValue() == 1.0);
      
      valueProp = static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("Score"));
      CPPUNIT_ASSERT_MESSAGE("Task score should be 0.0.",valueProp->GetValue() == 0.0f);
      
      valueProp = static_cast<dtDAL::FloatActorProperty *>(proxy->GetProperty("Weight"));
      CPPUNIT_ASSERT_MESSAGE("Task weight should be 1.0.",valueProp->GetValue() == 1.0f);    
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }    
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskSubTasks()
{
   try
   {
      unsigned int i;
      
      dtCore::RefPtr<dtDAL::ActorType> taskActorType = 
         mLibraryManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());
      
      dtCore::RefPtr<dtActors::TaskActorProxy> parentProxy = 
         dynamic_cast<dtActors::TaskActorProxy *>(mLibraryManager->CreateActorProxy(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",parentProxy.valid());
      
      //Create a bunch of actors and add them as children.
      for (i=0; i<25; i++)
      {
         dtCore::RefPtr<dtActors::TaskActorProxy> childProxy = NULL;
         
         childProxy = dynamic_cast<dtActors::TaskActorProxy *>(mLibraryManager->CreateActorProxy(*taskActorType).get());
         CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy.valid());
         
         childProxy->SetName("ChildProxy" + dtUtil::ToString(i));
         parentProxy->AddSubTaskProxy(*childProxy);         
      }
      
      std::vector<dtCore::RefPtr<dtActors::TaskActorProxy> > children;
      
      parentProxy->GetAllSubTaskProxies(children);
      CPPUNIT_ASSERT_MESSAGE("Number of child tasks should have been 25.",children.size() == 25);
      
      //Make sure the parent was set correctly and that we can find the task.
      for (i=0; i<25; i++)
      {
         CPPUNIT_ASSERT_MESSAGE("Parent was not set correctly.",children[i]->GetParentTaskProxy() == parentProxy.get());
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by unique id.",
            parentProxy->FindSubTaskProxy(children[i]->GetGameActor().GetUniqueId()) != NULL);
         CPPUNIT_ASSERT_MESSAGE("Should have found the task by unique id.",
            parentProxy->FindSubTaskProxy(children[i]->GetGameActor().GetName()) != NULL);            
      }
      
      for (i=0; i<25; i+=2)
      {
         parentProxy->RemoveSubTaskProxy(*children[i]);
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be NULL after removing it.",
            children[i]->GetParentTaskProxy() == NULL);
      }
      
      parentProxy->GetAllSubTaskProxies(children);
      for (i=0; i<children.size(); i++)
      {         
         parentProxy->RemoveSubTaskProxy(children[i]->GetGameActor().GetName());
         CPPUNIT_ASSERT_MESSAGE("Child's parent task should be NULL after removing it by name.",
            children[i]->GetParentTaskProxy() == NULL);
      }
      
      CPPUNIT_ASSERT_MESSAGE("There should be no more child tasks left.",parentProxy->GetNumSubTaskProxies() == 0);               
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
}

///////////////////////////////////////////////////////////////////////////////
void TaskActorTests::TestTaskReparentOnAdd()
{
   try
   {
      //This tests a nested hierarchy of tasks.
      //parent 
      //   --> Child1
      //          --> Child2
      //                 --> Child3
           
      dtCore::RefPtr<dtDAL::ActorType> taskActorType = 
         mLibraryManager->FindActorType("dtcore.Tasks","Task Actor");
      CPPUNIT_ASSERT_MESSAGE("Could not find actor type.",taskActorType.valid());
      
      dtCore::RefPtr<dtActors::TaskActorProxy> parentProxy = 
         dynamic_cast<dtActors::TaskActorProxy *>(mLibraryManager->CreateActorProxy(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",parentProxy.valid());
      
      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy1 =          
         dynamic_cast<dtActors::TaskActorProxy *>(mLibraryManager->CreateActorProxy(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy1.valid());     
      
      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy2 =          
         dynamic_cast<dtActors::TaskActorProxy *>(mLibraryManager->CreateActorProxy(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy2.valid());     
      
      dtCore::RefPtr<dtActors::TaskActorProxy> childProxy3 =          
         dynamic_cast<dtActors::TaskActorProxy *>(mLibraryManager->CreateActorProxy(*taskActorType).get());
      CPPUNIT_ASSERT_MESSAGE("Could not create task actor proxy.",childProxy3.valid());  
      
      parentProxy->AddSubTaskProxy(*childProxy1);
      childProxy1->AddSubTaskProxy(*childProxy2);
      childProxy2->AddSubTaskProxy(*childProxy3);
      
      CPPUNIT_ASSERT_MESSAGE("Parent proxy should be parent of child 1.",
         childProxy1->GetParentTaskProxy() == parentProxy.get());
      CPPUNIT_ASSERT_MESSAGE("Child1 proxy should be parent of child 2.",
         childProxy2->GetParentTaskProxy() == childProxy1.get());
      CPPUNIT_ASSERT_MESSAGE("Child2 proxy should be parent of child 3.",
         childProxy3->GetParentTaskProxy() == childProxy2.get());
         
      parentProxy->RemoveSubTaskProxy(*childProxy1);
      childProxy1->RemoveSubTaskProxy(*childProxy2);
      childProxy2->RemoveSubTaskProxy(*childProxy3);
      CPPUNIT_ASSERT_MESSAGE("Child1 proxy's parent should be NULL",
         childProxy1->GetParentTaskProxy() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Child2 proxy's should be parent of child 2.",
         childProxy2->GetParentTaskProxy() == NULL);
      CPPUNIT_ASSERT_MESSAGE("Child3 proxy's should be parent of child 3.",
         childProxy3->GetParentTaskProxy() == NULL);
   }
   catch (const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.What()).c_str());
   }
   catch (const std::exception& e)
   {
      CPPUNIT_FAIL((std::string("Error: ") + e.what()).c_str());
   }
}

