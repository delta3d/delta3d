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
* @author David Guthrie
*/
#include <prefix/unittestprefix.h>
#include <vector>
#include <set>
#include <string>

#include <cstdio>

#include <dtUtil/datetime.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/utiltree.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/datetime.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/version.h>

#include <dtCore/mapxml.h>
#include <dtCore/mapxmlconstants.h>
#include <dtCore/datatype.h>
#include <dtCore/project.h>
#include <dtCore/projectconfig.h>
#include <dtCore/map.h>
#include <dtCore/exceptionenum.h>

// Resource Actor Property has a helper function to make it easier to get a resource path, so it's easiest to test it with
// project.
#include <dtCore/resourceactorproperty.h>

#include <cppunit/extensions/HelperMacros.h>

namespace dtCore
{
   class ResourceTreeNode;
   class DataType;
}

const std::string TEST_PROJECT_DIR("TestProject");

class ProjectTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(ProjectTests);
   CPPUNIT_TEST(TestReadonlyFailure);
   CPPUNIT_TEST(TestCreateContextWithMapsDir);
   CPPUNIT_TEST(TestProject);
   CPPUNIT_TEST(TestGetMapHeader);
   CPPUNIT_TEST(TestSetupFromProjectConfig);
   CPPUNIT_TEST(TestLoadProjectConfigFromFile);
   CPPUNIT_TEST(TestCategories);
   CPPUNIT_TEST(TestResources);
   CPPUNIT_TEST(TestDeletingBackupFromReadOnlyContext);
   CPPUNIT_TEST(TestNonModifiedMapBackup);
   CPPUNIT_TEST(TestModifiedMapBackup);
   CPPUNIT_TEST(TestMapSaveAndLoadMapName);
   CPPUNIT_TEST(TestMapBackupFilename);
   CPPUNIT_TEST(TestOpenMapBackupCreatesBackups);
   CPPUNIT_TEST(TestMapSaveAsExceptions);
   CPPUNIT_TEST(TestMapSaveAsBackups);
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void TestProject();
      void TestGetMapHeader();
      void TestSetupFromProjectConfig();
      void TestLoadProjectConfigFromFile();
      void TestFileIO();
      void TestCategories();
      void TestReadonlyFailure();
      void TestCreateContextWithMapsDir();
      void TestResources();
      void TestDeletingBackupFromReadOnlyContext();
      void TestNonModifiedMapBackup();
      void TestModifiedMapBackup();
      void TestMapSaveAndLoadMapName();
      void TestMapBackupFilename();
      void TestOpenMapBackupCreatesBackups();
      void TestMapSaveAsExceptions();
      void TestMapSaveAsBackups();

   private:
      dtUtil::Log* logger;
      void printTree(const dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator& iter);
      dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator findTreeNodeFromCategory(
            const dtUtil::tree<dtCore::ResourceTreeNode>& currentTree,
            const dtCore::DataType* dt, const std::string& category) const;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(ProjectTests);

std::string GetExamplesDataDir();
std::string GetTestsDir();


void ProjectTests::setUp()
{
   try
   {
      dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
      std::string logName("projectTest");

      //        logger = &dtUtil::Log::GetInstance("project.cpp");
      //        logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
      //        logger = &dtUtil::Log::GetInstance("fileutils.cpp");
      //        logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
      //        logger = &dtUtil::Log::GetInstance("mapxml.cpp");
      //        logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);

      logger = &dtUtil::Log::GetInstance(logName);

      //        logger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
      //        logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Log initialized.\n");
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.ChangeDirectory(GetTestsDir());
      fileUtils.PushDirectory("dtCore");

      if (!fileUtils.DirExists("WorkingProject"))
      {
         fileUtils.MakeDirectory("WorkingProject");
      }
      fileUtils.PushDirectory("WorkingProject");
      fileUtils.DirDelete(dtCore::DataType::STATIC_MESH.GetName(), true);
      fileUtils.DirDelete(dtCore::DataType::TERRAIN.GetName(), true);
      fileUtils.PopDirectory();

      fileUtils.FileDelete("terrain_simple.ive");
      fileUtils.FileDelete("flatdirt.ive");
      fileUtils.DirDelete("Testing", true);
      fileUtils.DirDelete("recursiveDir", true);

      fileUtils.FileCopy(GetExamplesDataDir() + "/StaticMeshes/terrain_simple.ive", ".", false);
      fileUtils.FileCopy(GetExamplesDataDir() + "/StaticMeshes/flatdirt.ive", ".", false);

      dtCore::Project::GetInstance().ClearAllContexts();
      dtCore::Project::GetInstance().SetReadOnly(false);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}


void ProjectTests::tearDown()
{
   dtCore::Project::GetInstance().ClearAllContexts();
   dtCore::Project::GetInstance().SetReadOnly(false);

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   fileUtils.DirDelete("temp", true);
   fileUtils.FileDelete("terrain_simple.ive");
   fileUtils.FileDelete("flatdirt.ive");
   fileUtils.DirDelete("Testing", true);
   fileUtils.DirDelete("recursiveDir", true);

   //Delete a couple other projects
   if (fileUtils.DirExists(TEST_PROJECT_DIR))
   {
      fileUtils.DirDelete(TEST_PROJECT_DIR, true);
   }
   if (fileUtils.DirExists("TestProject1"))
   {
      fileUtils.DirDelete("TestProject1", true);
   }
   if (fileUtils.DirExists("TestProject2"))
   {
      fileUtils.DirDelete("TestProject2", true);
   }
   if (fileUtils.DirExists("Test2Project"))
   {
      fileUtils.DirDelete("Test2Project", true);
   }
   if (fileUtils.DirExists("WorkingProject"))
   {
      fileUtils.DirDelete("WorkingProject", true);
   }
   if (fileUtils.DirExists("WorkingProject2"))
   {
      fileUtils.DirDelete("WorkingProject2", true);
   }

   std::string currentDir = fileUtils.CurrentDirectory();
   std::string projectDir("dtCore");
   if (currentDir.substr(currentDir.size() - projectDir.size()) == projectDir)
   {
      fileUtils.PopDirectory();
   }
}

dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator ProjectTests::findTreeNodeFromCategory(
      const dtUtil::tree<dtCore::ResourceTreeNode>& currentTree,
      const dtCore::DataType* dt, const std::string& category) const {

   if (dt != NULL && !dt->IsResource())
      return currentTree.end();

   std::vector<std::string> tokens;
   dtUtil::StringTokenizer<dtCore::IsCategorySeparator>::tokenize(tokens, category);
   //if dt == NULL, assume that the datatype name is at the front of the category.
   if (dt != NULL)
      //Push the name of the datetype because it's the top level of the tree.
      tokens.insert(tokens.begin(), dt->GetName());

   std::string currentCategory;

   dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator ti = currentTree.get_tree_iterator();

   for (std::vector<std::string>::const_iterator i = tokens.begin(); i != tokens.end(); ++i) {
      if (ti == currentTree.end())
         return currentTree.end();

      //std::cout << *i << std::endl;

      //keep a full category string running
      //to create an accurate tree node to compare against.
      //Skip the first token because it is the datatype, not the category.
      if (i != tokens.begin())
      {
         if (currentCategory == "")
         {
            currentCategory += *i;
         }
         else
         {
            currentCategory += dtCore::ResourceDescriptor::DESCRIPTOR_SEPARATOR + *i;
         }
      }

      ti = ti.tree_ref().find(dtCore::ResourceTreeNode(*i, currentCategory, NULL, 0));
   }
   return ti;
}

void ProjectTests::printTree(const dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator& iter)
{
   for (unsigned tabs = 0; tabs < iter.level(); ++tabs)
   {
      std::cout << "\t";
   }

   std::cout << iter->getNodeText();
   if (!iter->isCategory())
   {
      std::cout << " -- " << iter->getResource().GetResourceIdentifier();
   }
   else
   {
      std::cout << " -> " << iter->getFullCategory();

      std::cout << std::endl;

      for (dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator i = iter.tree_ref().in();
         i != iter.tree_ref().end();
         ++i)
      {
         printTree(i);
      }
   }
}

void ProjectTests::TestReadonlyFailure()
{
   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();


      p.CreateContext(TEST_PROJECT_DIR);

      try
      {
         p.SetContext(TEST_PROJECT_DIR);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("Project should have been able to set context. Exception: ") + e.ToString());
      }

      try
      {
         p.SetContext(TEST_PROJECT_DIR, true);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("Project should have been able to set context. Exception: ") + e.ToString());
      }

      CPPUNIT_ASSERT_MESSAGE("context should be valid", p.IsContextValid());
      CPPUNIT_ASSERT_MESSAGE("context should be valid", p.IsReadOnly());

      try
      {
         p.Refresh();
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("Project should have been able to call refresh: ") + e.ToString());
      }

      try
      {
         dtUtil::tree<dtCore::ResourceTreeNode> toFill;
         p.GetResourcesOfType(dtCore::DataType::STATIC_MESH, toFill);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("Project should have been able to call GetResourcesOfType: ") + e.ToString());
      }

      try
      {
         p.GetAllResources();
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("Project should have been able to call GetResourcesOfType: ") + e.ToString());
      }

      CPPUNIT_ASSERT_THROW_MESSAGE("DeleteMap() should have thrown exception on a ReadOnly ProjectContext",
                                   p.DeleteMap("mojo"), dtCore::ProjectReadOnlyException);

      CPPUNIT_ASSERT_THROW_MESSAGE("SaveMap() should have thrown exception on a ReadOnly ProjectContext",
                                    p.SaveMap("mojo"), dtCore::ProjectReadOnlyException);

      CPPUNIT_ASSERT_THROW_MESSAGE("SaveMapAs() should have thrown exception on a ReadOnly ProjectContext",
                                    p.SaveMapAs("mojo", "a", "b"), dtCore::ProjectReadOnlyException);

      CPPUNIT_ASSERT_THROW_MESSAGE("AddResource() should have thrown exception on a ReadOnly ProjectContext",
                                    p.AddResource("mojo", "../jojo.ive","fun:bigmamajama", dtCore::DataType::STATIC_MESH),
                                    dtCore::ProjectReadOnlyException);

      CPPUNIT_ASSERT_THROW_MESSAGE("RemoveResource() should have thrown exception on a ReadOnly ProjectContext",
                                   p.RemoveResource(dtCore::ResourceDescriptor("","")),dtCore::ProjectReadOnlyException);

      CPPUNIT_ASSERT_THROW_MESSAGE("CreateResourceCategory() should have thrown exception on a ReadOnly ProjectContext",
                                   p.CreateResourceCategory("name-o", dtCore::DataType::STRING),dtCore::ProjectReadOnlyException);

      CPPUNIT_ASSERT_THROW_MESSAGE("RemoveResourceCategory() should have thrown exception on a ReadOnly ProjectContext",
                                   p.RemoveResourceCategory("name-o", dtCore::DataType::SOUND, true),dtCore::ProjectReadOnlyException);

      CPPUNIT_ASSERT_THROW_MESSAGE("CreateMap() should have thrown exception on a ReadOnly ProjectContext",
                                   p.CreateMap("name-o", "testFile"),dtCore::ProjectReadOnlyException);

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception &e) {
   //       CPPUNIT_FAIL(std::string("Caught an exception of type") + typeid(e).name() + " with message " + e.what());
   //    }

}

void ProjectTests::TestCreateContextWithMapsDir()
{
   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();
      p.CreateContext(TEST_PROJECT_DIR, true);
      CPPUNIT_ASSERT(dtUtil::FileUtils::GetInstance().DirExists(TEST_PROJECT_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "maps"));
      dtUtil::FileUtils::GetInstance().DirDelete(TEST_PROJECT_DIR, true);
      p.CreateContext(TEST_PROJECT_DIR, false);
      CPPUNIT_ASSERT( ! dtUtil::FileUtils::GetInstance().DirExists(TEST_PROJECT_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "maps"));
      dtUtil::FileUtils::GetInstance().DirDelete(TEST_PROJECT_DIR, true);
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex)
   //    {
   //        CPPUNIT_FAIL(ex.what());
   //    }
}

void ProjectTests::TestGetMapHeader()
{
   dtCore::Project& p = dtCore::Project::GetInstance();
   p.ClearAllContexts();
   CPPUNIT_ASSERT_THROW(p.GetMapHeader("blah"), dtCore::ProjectInvalidContextException);
   try
   {
      p.CreateContext(TEST_PROJECT_DIR, false);
      p.AddContext(TEST_PROJECT_DIR);
      // Upper case maps to test on case sensitive systems
      dtUtil::FileUtils::GetInstance().MakeDirectory(TEST_PROJECT_DIR + "/Maps");
      dtCore::Map& testMeMap = p.CreateMap("TestMe", "TestMe.dtmap");
      testMeMap.SetAuthor("Joe");
      testMeMap.SetDescription("Frank is Joe's friend");
      testMeMap.SetComment("Frank is an odd fella.");
      testMeMap.SetCopyright("No matter");
      p.SaveMap(testMeMap);
      p.CloseMap(testMeMap);

      dtCore::MapPtr mapHeader = p.GetMapHeader("TestMe");
      CPPUNIT_ASSERT_EQUAL(std::string("Joe"), mapHeader->GetAuthor());
      CPPUNIT_ASSERT_EQUAL(std::string("Frank is Joe's friend"), mapHeader->GetDescription());
      CPPUNIT_ASSERT_EQUAL(std::string("Frank is an odd fella."), mapHeader->GetComment());
      CPPUNIT_ASSERT_EQUAL(std::string("No matter"), mapHeader->GetCopyright());
      CPPUNIT_ASSERT_EQUAL(std::string("TestMe"), mapHeader->GetName());
      CPPUNIT_ASSERT(!mapHeader->GetCreateDateTime().empty());

      // Blast the directory
      dtUtil::FileUtils::GetInstance().DirDelete(TEST_PROJECT_DIR + "/Maps", true);

      // It thinks the file is there, but it will fail to load.
      CPPUNIT_ASSERT_THROW(p.GetMapHeader("TestMe"), dtCore::MapParsingException);
      // It knows the file is not there
      CPPUNIT_ASSERT_THROW(p.GetMapHeader("NoSuchMap"), dtUtil::FileNotFoundException);

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }

}

#define TEST_ACCESSOR(varPtr, accessor, defaultVal, testVal) \
         CPPUNIT_ASSERT_EQUAL(defaultVal, varPtr->Get ## accessor()); \
         varPtr->Set ## accessor(testVal); \
         CPPUNIT_ASSERT_EQUAL(testVal, varPtr->Get ## accessor()); \

namespace dtTest
{
   struct AddWorking
   {
      void operator() (dtCore::ContextData& c)
      {
         c.SetPath("Working" + c.GetPath());
      }
   };
}
void ProjectTests::TestSetupFromProjectConfig()
{
   dtCore::RefPtr<dtCore::ProjectConfig> pconfig = new dtCore::ProjectConfig;
   TEST_ACCESSOR(pconfig, Name, std::string(), std::string("Grumpy"));
   TEST_ACCESSOR(pconfig, Description, std::string(), std::string("Grumpy1"));
   TEST_ACCESSOR(pconfig, Author, std::string(), std::string("Grumpy2"));
   TEST_ACCESSOR(pconfig, Comment, std::string(), std::string("Grumpy3"));
   TEST_ACCESSOR(pconfig, Copyright, std::string(), std::string("Grumpy4"));
   TEST_ACCESSOR(pconfig, ReadOnly, false, true);

   pconfig->AddContextData(dtCore::ContextData("Project"));
   pconfig->AddContextData(dtCore::ContextData("Project2"));

   dtTest::AddWorking addWorkingFunc;
   // If this fails, the SetupFromProjectConfig wil fail and the paths will be incorrect below.
   pconfig->ForEachContextData(addWorkingFunc);

   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      p.CreateContext("WorkingProject");
      p.CreateContext("WorkingProject2");

      p.SetupFromProjectConfig(*pconfig);

      // The config was set to true in the above TEST_ACCESSOR
      CPPUNIT_ASSERT(p.IsReadOnly());

      CPPUNIT_ASSERT(p.IsContextValid(0));
      CPPUNIT_ASSERT(p.IsContextValid(1));
      CPPUNIT_ASSERT(!p.IsContextValid(2));

      CPPUNIT_ASSERT_EQUAL(dtCore::Project::ContextSlot(0), p.GetContextSlotForPath(p.GetContext(0)));
      CPPUNIT_ASSERT_EQUAL(dtCore::Project::ContextSlot(1), p.GetContextSlotForPath(p.GetContext(1)));
      CPPUNIT_ASSERT_EQUAL(dtCore::Project::ContextSlot(0), p.GetContextSlotForPath(p.GetContext(0)+"/hi/joe.png"));
      CPPUNIT_ASSERT_EQUAL(dtCore::Project::ContextSlot(1), p.GetContextSlotForPath(p.GetContext(1)+"/hi/joe.png"));
      CPPUNIT_ASSERT_THROW_MESSAGE("A garbage path should not be found as being in any project context.", p.GetContextSlotForPath(TEST_PROJECT_DIR+"/hello/mom"), dtUtil::FileNotFoundException);


      // If the paths don't match here, it may be that the ForEachContextData failed above.
      CPPUNIT_ASSERT_EQUAL(fileUtils.GetAbsolutePath(pconfig->GetContextData(0).GetPath()), p.GetContext(0));
      CPPUNIT_ASSERT_EQUAL(fileUtils.GetAbsolutePath(pconfig->GetContextData(1).GetPath()), p.GetContext(1));
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex) {
   //        CPPUNIT_FAIL(ex.what());
   //    }
}

void ProjectTests::TestLoadProjectConfigFromFile()
{
   dtCore::RefPtr<dtCore::ProjectConfig> pconfig = new dtCore::ProjectConfig;
   TEST_ACCESSOR(pconfig, Name, std::string(), std::string("Grumpy"));
   TEST_ACCESSOR(pconfig, Description, std::string(), std::string("Grumpy1"));
   TEST_ACCESSOR(pconfig, Author, std::string(), std::string("Grumpy2"));
   TEST_ACCESSOR(pconfig, Comment, std::string(), std::string("Grumpy3"));
   TEST_ACCESSOR(pconfig, Copyright, std::string(), std::string("Grumpy4"));
   TEST_ACCESSOR(pconfig, ReadOnly, false, true);

   pconfig->AddContextData(dtCore::ContextData("WorkingProject"));
   pconfig->AddContextData(dtCore::ContextData("WorkingProject2"));

   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      p.CreateContext("WorkingProject");
      p.CreateContext("WorkingProject2");

      fileUtils.MakeDirectory("temp");
      pconfig->SetBasePath("temp");
      pconfig->ConvertContextDataToRelativeOfBasePath();
      CPPUNIT_ASSERT_EQUAL(std::string("../WorkingProject"), pconfig->GetContextData(0).GetPath());
      CPPUNIT_ASSERT_EQUAL(std::string("../WorkingProject2"), pconfig->GetContextData(1).GetPath());

      p.SaveProjectConfigFile(*pconfig, "temp/testConfig.dtproj");
      CPPUNIT_ASSERT(fileUtils.FileExists("temp/testConfig.dtproj"));
      CPPUNIT_ASSERT_THROW_MESSAGE("It should not allow saving over a file.", p.SaveProjectConfigFile(*pconfig, "temp/testConfig.dtproj"), dtUtil::Exception);

      dtCore::RefPtr<dtCore::ProjectConfig> loadedConfig = p.LoadProjectConfigFile("temp/testConfig.dtproj");

      // Just check the values, not change them.
      TEST_ACCESSOR(loadedConfig, Name, pconfig->GetName(), pconfig->GetName());
      TEST_ACCESSOR(loadedConfig, Description, pconfig->GetDescription(), pconfig->GetDescription());
      TEST_ACCESSOR(loadedConfig, Author, pconfig->GetAuthor(), pconfig->GetAuthor());
      TEST_ACCESSOR(loadedConfig, Comment, pconfig->GetComment(), pconfig->GetComment());
      TEST_ACCESSOR(loadedConfig, Copyright, pconfig->GetCopyright(), pconfig->GetCopyright());
      TEST_ACCESSOR(loadedConfig, ReadOnly, pconfig->GetReadOnly(), pconfig->GetReadOnly());

      CPPUNIT_ASSERT_EQUAL(loadedConfig->GetNumContextData(), pconfig->GetNumContextData());
      for (unsigned i = 0; i < loadedConfig->GetNumContextData(); ++i)
      {
         CPPUNIT_ASSERT_EQUAL(loadedConfig->GetContextData(i), pconfig->GetContextData(i));
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

void ProjectTests::TestCategories()
{
   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string projectDir2("TestProject2");

      try {
         p.CreateContext(TEST_PROJECT_DIR);
         p.CreateContext(projectDir2);
         p.SetContext(TEST_PROJECT_DIR);
         p.AddContext(projectDir2);
      } catch (const dtUtil::Exception& e) {
         CPPUNIT_FAIL(std::string(std::string("Project should have been able to set context. Exception: ") + e.ToString()).c_str());
      }

      for (std::vector<dtCore::DataType*>::const_iterator i = dtCore::DataType::EnumerateType().begin();
      i != dtCore::DataType::EnumerateType().end(); ++i) {
         dtCore::DataType& d = **i;

         //don't index the first time so it will be tested both ways.
         if (i != dtCore::DataType::EnumerateType().begin())
            p.GetAllResources();

         if (!d.IsResource()) {
            const std::string PRIM_CATEGORY_MSG("Project should not be able to create a category for a primitive type.");
            CPPUNIT_ASSERT_THROW_MESSAGE(PRIM_CATEGORY_MSG, p.CreateResourceCategory("littleFoot", d), dtUtil::Exception);
            CPPUNIT_ASSERT_THROW_MESSAGE(PRIM_CATEGORY_MSG, p.CreateResourceCategory("littleFoot", d, 0), dtUtil::Exception);
            CPPUNIT_ASSERT_THROW_MESSAGE(PRIM_CATEGORY_MSG, p.CreateResourceCategory("littleFoot", d, 1), dtUtil::Exception);
         } else {
            p.CreateResourceCategory("abomb", d);

            CPPUNIT_ASSERT_MESSAGE(
                  "attempting to remove a simple category should succeed.",
                  p.RemoveResourceCategory("abomb", d, false));

            p.CreateResourceCategory("abomb:hbomb", d);

            CPPUNIT_ASSERT_MESSAGE(
                  "attempting to remove a simple category should succeed.",
                  p.RemoveResourceCategory("abomb:hbomb", d, false));

            std::string catPath(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR
                  + d.GetName() + dtUtil::FileUtils::PATH_SEPARATOR + "abomb");

            std::string catPath2(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR
                  + d.GetName() + dtUtil::FileUtils::PATH_SEPARATOR + "abomb");

            CPPUNIT_ASSERT_MESSAGE("Static mesh category abomb should exist.",
                  fileUtils.DirExists(catPath));

            p.CreateResourceCategory("abomb:jojo:eats:hummus", d);

            std::string longPath(catPath + dtUtil::FileUtils::PATH_SEPARATOR + "jojo"
                  + dtUtil::FileUtils::PATH_SEPARATOR + "eats"
                  + dtUtil::FileUtils::PATH_SEPARATOR + "hummus");

            CPPUNIT_ASSERT_MESSAGE(std::string("Static mesh category ") + longPath + " should exist.",
                  fileUtils.DirExists(longPath));
            //printTree(p.GetAllResources());
            CPPUNIT_ASSERT(p.RemoveResourceCategory("abomb:jojo:eats:hummus", d, false));
            CPPUNIT_ASSERT_MESSAGE(std::string("Static mesh category ") + longPath + " should NOT exist.",
                  !fileUtils.DirExists(longPath));

            CPPUNIT_ASSERT_MESSAGE(
                  "Attempting to non-recursivly remove a category with contents should return false.",
                  !p.RemoveResourceCategory("abomb:jojo", d, false));

            CPPUNIT_ASSERT_MESSAGE("Static mesh category abomb should exist.",
                  fileUtils.DirExists(catPath + dtUtil::FileUtils::PATH_SEPARATOR + "jojo"
                        + dtUtil::FileUtils::PATH_SEPARATOR + "eats"));

            CPPUNIT_ASSERT(p.RemoveResourceCategory("abomb", d, true));
            CPPUNIT_ASSERT_MESSAGE("Static mesh category abomb should not exist.",
                  !fileUtils.DirExists(catPath));
         }
      }
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex) {
   //        CPPUNIT_FAIL(ex.what());
   //    }

}

void ProjectTests::TestResources()
{
   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      //Open an existing project with two directories.
      const std::string projectDir = "WorkingProject";
      const std::string projectDir2 = "WorkingProject2";

      try
      {
         p.CreateContext(projectDir);
         p.CreateContext(projectDir2);
         p.AddContext(projectDir);
         p.AddContext(projectDir2);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL((std::string("Project should have been able to Set context. Exception: ")
               + e.ToString()).c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("Project should not be read only.", !p.IsReadOnly());
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtUtil::GetDataFilePathList().find(p.GetContext(0)) != std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtUtil::GetDataFilePathList().find(p.GetContext(1)) != std::string::npos);

      const std::set<std::string>& mapNames = p.GetMapNames();

      std::vector<dtCore::RefPtr<dtCore::Map> > maps;


      const std::string& utcTime = dtUtil::DateTime::ToString(dtUtil::DateTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME),
         dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT);

      logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
            "Current time as UTC is %s", utcTime.c_str());

      std::vector<const dtCore::ResourceTypeHandler* > handlers;

      p.GetHandlersForDataType(dtCore::DataType::TERRAIN, handlers);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be 9 terrain type handlers", size_t(6),  handlers.size());

      std::string testResult;

      CPPUNIT_ASSERT_THROW_MESSAGE("The AddResource() to add a non-existent file should have failed.",
                                   p.AddResource("mojo", "../jojo.ive", "fun:bigmamajama", dtCore::DataType::STATIC_MESH, 0),
                                   dtUtil::Exception);

      CPPUNIT_ASSERT_THROW_MESSAGE("The AddResource() to add a mis-matched DataType should have failed.",
                                   p.AddResource("dirt", "../terrain_simple.ive", "fun:bigmamajama", dtCore::DataType::BOOLEAN, 1),
                                   dtUtil::Exception);

      std::string dirtCategory = "fun:bigmamajama";


      dtCore::ResourceDescriptor terrain1RD = p.AddResource("terrain1", GetExamplesDataDir() + "/StaticMeshes/exampleTerrain", "terrain",
            dtCore::DataType::TERRAIN, 0);

      //force resources to be indexed.
      p.GetAllResources();


      dtCore::ResourceDescriptor terrain2RD = p.AddResource("terrain2", GetExamplesDataDir() + "/StaticMeshes/exampleTerrain/terrain.3ds", "",
            dtCore::DataType::TERRAIN, 1);

      //printTree(p.GetAllResources());

      dtUtil::tree<dtCore::ResourceTreeNode> toFill;

      p.GetResourcesOfType(dtCore::DataType::TERRAIN, toFill);
      dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator terrainCategory =
         findTreeNodeFromCategory(toFill, NULL, "");

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"")
            + "\" should have been found in the resource tree", terrainCategory != p.GetAllResources().end());

      dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator terrain2Resource =
         terrainCategory.tree_ref().find(dtCore::ResourceTreeNode("terrain2", terrainCategory->getFullCategory(), &terrain2RD, 0));

      terrainCategory = findTreeNodeFromCategory(toFill, NULL, "terrain");
      printTree(p.GetAllResources());

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"terrain")
            + "\" should have been found in the resource tree", terrainCategory != p.GetAllResources().end());

      dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator terrain1Resource =
         terrainCategory.tree_ref().find(dtCore::ResourceTreeNode("terrain1", terrainCategory->getFullCategory(), &terrain1RD, 0));

      CPPUNIT_ASSERT_MESSAGE("The terrain2 resource should have been found.", terrain2Resource != p.GetAllResources().end());
      CPPUNIT_ASSERT_MESSAGE("The terrain1 resource should have been found.", terrain1Resource != p.GetAllResources().end());

      std::string terrainDir0(p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + dtCore::DataType::TERRAIN.GetName() + dtUtil::FileUtils::PATH_SEPARATOR);
      std::string terrainDir1(p.GetContext(1) + dtUtil::FileUtils::PATH_SEPARATOR + dtCore::DataType::TERRAIN.GetName() + dtUtil::FileUtils::PATH_SEPARATOR);

      CPPUNIT_ASSERT(fileUtils.DirExists(terrainDir1 + "terrain2.3dst" ) &&
            fileUtils.FileExists(terrainDir1 + "terrain2.3dst" + dtUtil::FileUtils::PATH_SEPARATOR + "terrain.3ds"));

      CPPUNIT_ASSERT(fileUtils.DirExists(terrainDir0 + "terrain" +
            dtUtil::FileUtils::PATH_SEPARATOR + "terrain1.3dst") &&
            fileUtils.FileExists(terrainDir0 + "terrain" + dtUtil::FileUtils::PATH_SEPARATOR + "terrain1.3dst" + dtUtil::FileUtils::PATH_SEPARATOR + "terrain.3ds"));

      //Done with the terrains

      dtCore::ResourceDescriptor rd = p.AddResource("flatdirt", std::string(GetExamplesDataDir() + "/StaticMeshes/flatdirt.ive"),
            dirtCategory, dtCore::DataType::STATIC_MESH, 0);

      CPPUNIT_ASSERT_MESSAGE("Descriptor id should not be empty.", !rd.GetResourceIdentifier().empty());

      testResult = p.GetResourcePath(rd);

      std::string expectedPath = p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + dtCore::DataType::STATIC_MESH.GetName() + dtUtil::FileUtils::PATH_SEPARATOR
            + "fun" + dtUtil::FileUtils::PATH_SEPARATOR + "bigmamajama"
            + dtUtil::FileUtils::PATH_SEPARATOR + "flatdirt.ive";
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Getting the resource path returned the wrong value",
            testResult, expectedPath);

      CPPUNIT_ASSERT(dtCore::ResourceActorProperty::GetResourcePath(dtCore::ResourceDescriptor::NULL_RESOURCE).empty());
      CPPUNIT_ASSERT_EQUAL(expectedPath, dtCore::ResourceActorProperty::GetResourcePath(rd));

#ifndef DELTA_WIN32
      std::string rdVal = rd.GetResourceIdentifier();

      dtUtil::ToUpperCase(rdVal);
      dtCore::ResourceDescriptor rdUpperCase(rdVal);

      std::string testResultUpper;
      testResultUpper = p.GetResourcePath(rdUpperCase);
      CPPUNIT_ASSERT_EQUAL(testResult, testResultUpper);
#endif

      for (std::set<std::string>::const_iterator i = mapNames.begin(); i != mapNames.end(); i++)
      {
         logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found map named %s.", i->c_str());
         //dtCore::Map& m = p.GetMap(*i);

         //maps.Push_back(dtCore::RefPtr<dtCore::Map>(&m));
      }

      p.GetResourcesOfType(dtCore::DataType::STATIC_MESH, toFill);

      CPPUNIT_ASSERT_MESSAGE("The head of the tree should be static mesh",
            toFill.data().getNodeText() == dtCore::DataType::STATIC_MESH.GetName());

      dtUtil::tree<dtCore::ResourceTreeNode>::const_iterator treeResult =
         findTreeNodeFromCategory(p.GetAllResources(),
               &dtCore::DataType::STATIC_MESH, dirtCategory);

      //printTree(toFill.tree_iterator());

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should have been found in the resource tree", treeResult != p.GetAllResources().end());


      CPPUNIT_ASSERT_MESSAGE(std::string("the resource \"") + rd.GetResourceIdentifier()
            + "\" should have been found in the resource tree",
            treeResult.tree_ref().find(dtCore::ResourceTreeNode(rd.GetDisplayName(), dirtCategory, &rd, 0))
            != p.GetAllResources().end());

      p.RemoveResource(rd);

      treeResult = findTreeNodeFromCategory(p.GetAllResources(),
            &dtCore::DataType::STATIC_MESH, dirtCategory);

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should have been found in the resource tree", treeResult != p.GetAllResources().end());

      CPPUNIT_ASSERT_MESSAGE(std::string("the resource \"") + rd.GetResourceIdentifier()
            + "\" should have NOT been found in the resource tree",
            treeResult.tree_ref().find(dtCore::ResourceTreeNode(rd.GetDisplayName(), dirtCategory,  &rd, 0))
            == p.GetAllResources().end());

      CPPUNIT_ASSERT(!p.RemoveResourceCategory("fun", dtCore::DataType::STATIC_MESH, false));
      CPPUNIT_ASSERT(p.RemoveResourceCategory("fun", dtCore::DataType::STATIC_MESH, true));

      treeResult = findTreeNodeFromCategory(p.GetAllResources(),
            &dtCore::DataType::STATIC_MESH, dirtCategory);
      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should not have been found in the resource tree", treeResult == p.GetAllResources().end());

      treeResult = findTreeNodeFromCategory(p.GetAllResources(),
            &dtCore::DataType::STATIC_MESH, "fun");
      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + "fun"
            + "\" should not have been found in the resource tree", treeResult == p.GetAllResources().end());

      rd = p.AddResource("pow", std::string(GetExamplesDataDir() + "/sounds/pow.wav"), std::string("tea:money"), dtCore::DataType::SOUND, 0);
      testResult = p.GetResourcePath(rd);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Getting the resource path returned the wrong value: ",
            testResult, p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR +
            dtCore::DataType::SOUND.GetName() + dtUtil::FileUtils::PATH_SEPARATOR
            + "tea" + dtUtil::FileUtils::PATH_SEPARATOR
            + "money" + dtUtil::FileUtils::PATH_SEPARATOR + "pow.wav");

      dtCore::ResourceDescriptor rd1 = p.AddResource("bang", std::string(GetExamplesDataDir() + "/sounds/bang.wav"),
            std::string("tee:cash"), dtCore::DataType::SOUND, 0);
      testResult = p.GetResourcePath(rd1);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Getting the resource path returned the wrong value:",
            testResult, p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR +
            dtCore::DataType::SOUND.GetName() + dtUtil::FileUtils::PATH_SEPARATOR + "tee"
            + dtUtil::FileUtils::PATH_SEPARATOR + "cash" + dtUtil::FileUtils::PATH_SEPARATOR + "bang.wav");

      CPPUNIT_ASSERT_THROW_MESSAGE("Getting the path to a resource directory should throw an exception",
               p.GetResourcePath(dtCore::ResourceDescriptor(dtCore::DataType::SOUND.GetName())), dtCore::ProjectResourceErrorException);

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Getting the path to a resource directory should work in this case.",
               p.GetContext(0) + dtUtil::FileUtils::PATH_SEPARATOR + dtCore::DataType::SOUND.GetName(),
               p.GetResourcePath(dtCore::ResourceDescriptor(dtCore::DataType::SOUND.GetName()), true));

      dtCore::ResourceDescriptor rdNoCat = p.AddResource("pow", std::string(GetExamplesDataDir() + "/sounds/pow.wav"), std::string(""), dtCore::DataType::SOUND, 1);
      CPPUNIT_ASSERT_EQUAL(dtCore::DataType::SOUND.GetName() + ":pow.wav", rdNoCat.GetDisplayName());

      p.RemoveResource(rdNoCat);

      p.Refresh();

      //const dtUtil::tree<dtCore::ResourceTreeNode>& allTree = p.GetAllResources();

      //printTree(allTree.tree_iterator());

      p.RemoveResource(rd);
      p.RemoveResource(rd1);
      p.RemoveResource(terrain1RD);
      p.RemoveResource(terrain2RD);

      fileUtils.PushDirectory(projectDir);
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtCore::DataType::SOUND.GetName() + std::string("/tea/money/pow.wav")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.DirExists(dtCore::DataType::TERRAIN.GetName() + std::string("/terrain/terrain1.3dst")));
      fileUtils.PopDirectory();

      fileUtils.PushDirectory(projectDir2);
      CPPUNIT_ASSERT_MESSAGE("Resource should have never been in the project, but the file still exists.",
            !fileUtils.FileExists(dtCore::DataType::STATIC_MESH.GetName() + std::string("/fun/bigmamajama/terrain_simple.ive")));

      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.DirExists(dtCore::DataType::TERRAIN.GetName() + std::string("/terrain2.3dst")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtCore::DataType::SOUND.GetName() + std::string("/tee/cash/bang.wav")));
      fileUtils.PopDirectory();

      //this should work fine even if the file is deleted.
      p.RemoveResource(rd);

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex) {
   //        CPPUNIT_FAIL(ex.what());
   //    }

}


void ProjectTests::TestProject()
{
   try
   {
      dtCore::Project& p = dtCore::Project::GetInstance();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      std::string originalPathList = dtUtil::GetDataFilePathList();

#ifdef DELTA3D_WIN32
      std::string crapPath("/:%**/../^^jojo/funky/\\\\/,/,.uchor");
#else
      std::string crapPath("/usr/:%**/../^^jojo/funky/\\\\/,/,.uchor");
#endif

      CPPUNIT_ASSERT_THROW(p.CreateContext(crapPath), dtCore::ProjectInvalidContextException);
      CPPUNIT_ASSERT_THROW(p.SetContext(crapPath), dtCore::ProjectInvalidContextException);


      if (fileUtils.FileExists(TEST_PROJECT_DIR))
      {
         try
         {
            fileUtils.DirDelete(TEST_PROJECT_DIR, true);
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString().c_str());
         }

         CPPUNIT_ASSERT_MESSAGE("The project Directory should not yet exist.", !fileUtils.FileExists(TEST_PROJECT_DIR));
      }

      fileUtils.MakeDirectory(TEST_PROJECT_DIR);

      try
      {
         p.SetContext(TEST_PROJECT_DIR);
      }
      catch (const dtUtil::Exception&)
      {
         CPPUNIT_FAIL("Project should have been able to Set the context because the maps dir is no longer required.");
      }

      try {
         p.CreateContext(TEST_PROJECT_DIR);
         p.SetContext(TEST_PROJECT_DIR);
      } catch (const dtUtil::Exception& e) {
         CPPUNIT_FAIL(std::string(std::string("Project should have been able to Set context. Exception: ") + e.ToString()).c_str());
      }

      CPPUNIT_ASSERT_THROW_MESSAGE("passing a non-absolute path with a file that doesn't exist throws an exception.", p.GetContextSlotForPath(TEST_PROJECT_DIR+"/hi/joe.png"), dtUtil::FileNotFoundException);
      CPPUNIT_ASSERT_THROW_MESSAGE("passing a non-absolute path with a directory that doesn't exist throws an exception.", p.GetContextSlotForPath(TEST_PROJECT_DIR+"/hello/mom"), dtUtil::FileNotFoundException);
      
      CPPUNIT_ASSERT_EQUAL(dtCore::Project::ContextSlot(0), p.GetContextSlotForPath(TEST_PROJECT_DIR));

      CPPUNIT_ASSERT_EQUAL(dtCore::Project::ContextSlot(0), p.GetContextSlotForPath(p.GetContext(0)+"/hi/joe.png"));

      CPPUNIT_ASSERT_MESSAGE("Project should not be read only.", !p.IsReadOnly());
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtUtil::GetDataFilePathList().find(p.GetContext()) != std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the original path list.",
            dtUtil::GetDataFilePathList().find(originalPathList) != std::string::npos);

      try {
         p.SetContext(TEST_PROJECT_DIR, true);
      } catch (const dtUtil::Exception& e) {
         CPPUNIT_FAIL(std::string(std::string("Project should have been able to Set context. Exception: ") + e.ToString()).c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("Project should be read only.", p.IsReadOnly());
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtUtil::GetDataFilePathList().find(p.GetContext()) != std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the original path list.",
            dtUtil::GetDataFilePathList().find(originalPathList) != std::string::npos);

      std::string projectDir2("Test2Project");
      try
      {
         p.CreateContext(projectDir2);
         p.SetContext(projectDir2);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string(std::string("Project should have been able to Set context. Exception: ") + e.ToString()).c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("Project should be read only.", !p.IsReadOnly());
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtUtil::GetDataFilePathList().find(p.GetContext()) != std::string::npos);

      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtUtil::GetDataFilePathList().find(projectDir2) != std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should NOT contain the old context.",
            dtUtil::GetDataFilePathList().find(TEST_PROJECT_DIR) == std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the original path list.",
            dtUtil::GetDataFilePathList().find(originalPathList) != std::string::npos);

      try {
         fileUtils.DirDelete(TEST_PROJECT_DIR, true);
      } catch (const dtUtil::Exception& ex) {
         CPPUNIT_FAIL(ex.ToString().c_str());
      }


      CPPUNIT_ASSERT_MESSAGE("The project Directory should have been deleted.", !fileUtils.FileExists(TEST_PROJECT_DIR));

   } catch (const dtUtil::Exception& ex) {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex) {
   //        CPPUNIT_FAIL(ex.what());
   //    }

}

////////////////////////////////////////////////////////////////////////////////
void ProjectTests::TestDeletingBackupFromReadOnlyContext()
{
   const std::string mapName("mapWithBackup");
   dtCore::Project& proj = dtCore::Project::GetInstance();

   proj.CreateContext(TEST_PROJECT_DIR);
   proj.SetContext(TEST_PROJECT_DIR, false);

   //create a Map and save a backup
   dtCore::Map& testMap = proj.CreateMap(mapName, "mapWithBackup");
   testMap.SetModified(true); //needs to be modified before creating a backup
   proj.SaveMapBackup(testMap);

   CPPUNIT_ASSERT_MESSAGE("Didn't find a Map backup, can't go on with test",
                          true == proj.HasBackup(testMap));

   proj.SetContext(TEST_PROJECT_DIR, true); //now we're read only

   //trying to delete a Map backup in a read-only ProjectContext should throw
   CPPUNIT_ASSERT_THROW(proj.ClearBackup(mapName), dtUtil::Exception);

   proj.SetContext(TEST_PROJECT_DIR, false); //now we're not read only
   proj.ClearBackup(mapName); //this should work
}
//////////////////////////////////////////////////////////////////////////
void ProjectTests::TestNonModifiedMapBackup()
{
   std::string mapName("UnmodifiedMap");
   std::string mapFileName("UnmodifiedMap");

   dtCore::Project& project = dtCore::Project::GetInstance();
   project.CreateContext(TEST_PROJECT_DIR);
   project.SetContext(TEST_PROJECT_DIR, false);

   dtCore::Map* map = &project.CreateMap(mapName, mapFileName);

   project.SaveMapBackup(*map);

   //test both versions of the call.
   CPPUNIT_ASSERT_MESSAGE("Map was not modified.  There should be no backup saves.",
      !project.HasBackup(*map) && !project.HasBackup(mapName));

   project.DeleteMap(*map);
}

////////////////////////////////////////////////////////////////////////////////
void ProjectTests::TestModifiedMapBackup()
{
   std::string mapName("ModifiedMap");
   std::string mapFileName("ModifiedMap");

   dtCore::Project& project = dtCore::Project::GetInstance();
   project.CreateContext(TEST_PROJECT_DIR);
   project.SetContext(TEST_PROJECT_DIR, false);

   dtCore::Map* map = &project.CreateMap(mapName, mapFileName);

   //modify the map
   map->SetDescription("Teague is league with a \"t\".");

   const std::string filenameBeforeBackup = map->GetFileName();

   project.SaveMapBackup(*map);

   //test both versions of the call.
   CPPUNIT_ASSERT_MESSAGE("A backup was just saved.  The map should have backups.",
                           project.HasBackup(*map) && project.HasBackup(mapName));

   project.ClearBackup(*map);

   //test both versions of the call.
   CPPUNIT_ASSERT_MESSAGE("Backups were cleared.  The map should have no backups.",
                           !project.HasBackup(*map) && !project.HasBackup(mapName));

   project.DeleteMap(*map);
}

//////////////////////////////////////////////////////////////////////////
void ProjectTests::TestMapSaveAndLoadMapName()
{
   dtCore::Project& project = dtCore::Project::GetInstance();
   project.CreateContext(TEST_PROJECT_DIR);
   project.SetContext(TEST_PROJECT_DIR, false);

   const std::string mapName("Neato Map");
   const std::string mapFileName("neatomap");

   dtCore::Map* map = &project.CreateMap(mapName, mapFileName);

   const std::string newMapName("Weirdo Map");

   //set the name to make sure it can be changed.
   map->SetName(newMapName);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map should have the new name.", newMapName, map->GetName());

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map should have the old saved name", mapName, map->GetSavedName());

   project.SaveMapBackup(*map);

   CPPUNIT_ASSERT_MESSAGE("A backup was just saved.  The map should have backups.",
                          project.HasBackup(*map) && project.HasBackup(mapName));

   project.SaveMap(*map);

   //test both versions of the call.
   CPPUNIT_ASSERT_MESSAGE("Map was saved.  The map should have no backups.",
                          !project.HasBackup(*map) && !project.HasBackup(newMapName));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map should have the new saved name", newMapName, map->GetSavedName());

   project.DeleteMap(*map);
}

////////////////////////////////////////////////////////////////////////////////
void ProjectTests::TestMapBackupFilename()
{
   dtCore::Project& project = dtCore::Project::GetInstance();
   project.CreateContext(TEST_PROJECT_DIR);
   project.SetContext(TEST_PROJECT_DIR, false);
   const std::string mapName("Neato Map");
   const std::string mapFileName("neatomap");

   dtCore::Map* map = &project.CreateMap(mapName, mapFileName);

   const std::string newAuthor("Dr. Eddie");
   map->SetAuthor(newAuthor);

   const std::string filenameBeforeBackup = map->GetFileName();;
   project.SaveMapBackup(*map);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map filename should not have changed after performing a backup",
                                filenameBeforeBackup, map->GetFileName());

   map = &project.OpenMapBackup(mapName);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map filename should not have changed after loading a backup.",
                                filenameBeforeBackup, map->GetFileName());

   project.DeleteMap(*map);
}

////////////////////////////////////////////////////////////////////////////////
void ProjectTests::TestOpenMapBackupCreatesBackups()
{
   dtCore::Project& project = dtCore::Project::GetInstance();
   project.CreateContext(TEST_PROJECT_DIR);
   project.SetContext(TEST_PROJECT_DIR, false);
   const std::string mapName("Neato Map");
   const std::string mapFileName("neatomap");

   dtCore::Map* map = &project.CreateMap(mapName, mapFileName);

   map->SetAuthor("Dr. Eddie");

   project.SaveMapBackup(*map);
   map = &project.OpenMapBackup(mapName);

   CPPUNIT_ASSERT_MESSAGE("Map was loaded as a backup, so it should have backups.",
                           project.HasBackup(*map) && project.HasBackup(mapName));

   project.SaveMapBackup(*map);
   CPPUNIT_ASSERT_MESSAGE("A backup was just saved.  The map should have a backup.",
                           project.HasBackup(*map) && project.HasBackup(mapName));

   project.DeleteMap(*map);
}

////////////////////////////////////////////////////////////////////////////////
void ProjectTests::TestMapSaveAsExceptions()
{
   dtCore::Project& project = dtCore::Project::GetInstance();
   project.CreateContext(TEST_PROJECT_DIR);
   project.SetContext(TEST_PROJECT_DIR, false);
   const std::string mapName("Neato Map");
   const std::string mapFileName("neatomap");

   dtCore::Map* map = &project.CreateMap(mapName, mapFileName);
   map->SetAuthor("unit test");

   // JPH: Changes to the map load system should allow you to overwrite the same map with different data.
   //CPPUNIT_ASSERT_THROW_MESSAGE("Calling SaveAs on a map with the same name and filename should fail.",
   //   project.SaveMapAs(*map, mapName, mapFileName),
   //   dtUtil::Exception);

   //CPPUNIT_ASSERT_THROW_MESSAGE("Calling SaveAs on a map with the same filename should fail.",
   //   project.SaveMapAs(*map, "asdf", mapFileName),
   //   dtUtil::Exception);

   CPPUNIT_ASSERT_THROW_MESSAGE("Calling SaveAs on a map with the same name should fail.",
      project.SaveMapAs(*map, mapName, "asdf"),
      dtUtil::Exception);

   project.DeleteMap(*map);
}

////////////////////////////////////////////////////////////////////////////////
void ProjectTests::TestMapSaveAsBackups()
{
   dtCore::Project& project = dtCore::Project::GetInstance();
   project.CreateContext(TEST_PROJECT_DIR);
   project.SetContext(TEST_PROJECT_DIR, false);
   const std::string mapName("Neato Map");
   const std::string newMapName("new map name");
   const std::string mapFileName("neatomap");
   const std::string newMapFileName("oo");

   dtCore::Map* map = &project.CreateMap(mapName, mapFileName);
   map->SetAuthor("unit test");

   project.SaveMapAs(*map, newMapName, newMapFileName);

   //test both versions of the call.
   CPPUNIT_ASSERT_MESSAGE("Map was just saved AS.  The map should have no backups.",
      !project.HasBackup(*map) && !project.HasBackup(mapName));

   CPPUNIT_ASSERT_MESSAGE("Map was just saved AS.  The old map should have no backups.",
      !project.HasBackup(mapName));

   CPPUNIT_ASSERT_EQUAL_MESSAGE("Map file name should have changed during a SaveAs",
      newMapFileName + "." + dtCore::Map::MAP_FILE_EXTENSION, map->GetFileName());

   CPPUNIT_ASSERT_MESSAGE("Map name didn't change during a SaveAs.",
      map->GetName() == newMapName && map->GetSavedName() == newMapName);

   std::string newMapFilePath = project.GetContext() + dtUtil::FileUtils::PATH_SEPARATOR + "maps"
      + dtUtil::FileUtils::PATH_SEPARATOR + "oo." + dtCore::Map::MAP_FILE_EXTENSION;

   CPPUNIT_ASSERT_MESSAGE(std::string("The new map file should exist: ") + newMapFilePath,
      dtUtil::FileUtils::GetInstance().FileExists(newMapFilePath));

   project.DeleteMap(*map);
}
