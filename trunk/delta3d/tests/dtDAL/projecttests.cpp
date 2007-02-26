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
 * @author David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <vector>
#include <set>
#include <string>

#include <cstdio>
#include <ctime>

#include <dtCore/dt.h>
#include <dtCore/scene.h>
#include <dtCore/globals.h>
#include <dtABC/application.h>

#include <dtUtil/stringutils.h>

#include <dtUtil/tree.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
#include <dtDAL/mapxml.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/map.h>


#include <cppunit/extensions/HelperMacros.h>

namespace dtDAL 
{
   class ResourceTreeNode;
   class DataType;
}

class ProjectTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE( ProjectTests );
   CPPUNIT_TEST( testReadonlyFailure );
   CPPUNIT_TEST( testProject );
   CPPUNIT_TEST( testCategories );
   CPPUNIT_TEST( testResources );
   CPPUNIT_TEST_SUITE_END();

   public:
      void setUp();
      void tearDown();

      void testProject();
      void testFileIO();
      void testCategories();
      void testReadonlyFailure();
      void testResources();
   private:
      dtUtil::Log* logger;
      void printTree(const dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator& iter);
      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator findTreeNodeFromCategory(
            const dtUtil::tree<dtDAL::ResourceTreeNode>& currentTree,
            const dtDAL::DataType* dt, const std::string& category) const;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ProjectTests );

const std::string DATA_DIR = dtCore::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR+"examples/data";
const std::string TESTS_DIR = dtCore::GetDeltaRootPath() + dtUtil::FileUtils::PATH_SEPARATOR+"tests";
const std::string MAPPROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingMapProject";
const std::string PROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingProject";


void ProjectTests::setUp() {
   try {
      dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
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
      fileUtils.ChangeDirectory(TESTS_DIR);
      fileUtils.PushDirectory("dtDAL");

      fileUtils.PushDirectory("WorkingProject");
      fileUtils.DirDelete(dtDAL::DataType::STATIC_MESH.GetName(), true);
      fileUtils.DirDelete(dtDAL::DataType::TERRAIN.GetName(), true);
      fileUtils.PopDirectory();

      fileUtils.FileDelete("terrain_simple.ive");
      fileUtils.FileDelete("flatdirt.ive");
      fileUtils.DirDelete("Testing", true);
      fileUtils.DirDelete("recursiveDir", true);

      fileUtils.FileCopy(DATA_DIR + "/models/terrain_simple.ive", ".", false);
      fileUtils.FileCopy(DATA_DIR + "/models/flatdirt.ive", ".", false);
   } catch (const dtUtil::Exception& ex) {
      CPPUNIT_FAIL(ex.ToString());
   }
}


void ProjectTests::tearDown() {
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   fileUtils.FileDelete("terrain_simple.ive");
   fileUtils.FileDelete("flatdirt.ive");
   fileUtils.DirDelete("Testing", true);
   fileUtils.DirDelete("recursiveDir", true);

   //Delete a couple other projects
   if (fileUtils.DirExists("TestProject"))
      fileUtils.DirDelete("TestProject", true);
   if (fileUtils.DirExists("Test2Project"))
      fileUtils.DirDelete("Test2Project", true);

   std::string currentDir = fileUtils.CurrentDirectory();
   std::string projectDir("dtDAL");
   if (currentDir.substr(currentDir.size() - projectDir.size()) == projectDir)
      fileUtils.PopDirectory();
}

dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator ProjectTests::findTreeNodeFromCategory(
      const dtUtil::tree<dtDAL::ResourceTreeNode>& currentTree,
      const dtDAL::DataType* dt, const std::string& category) const {

   if (dt != NULL && !dt->IsResource())
      return currentTree.end();

   std::vector<std::string> tokens;
   dtUtil::StringTokenizer<dtDAL::IsCategorySeparator>::tokenize(tokens, category);
   //if dt == NULL, assume that the datatype name is at the front of the category.
   if (dt != NULL)
      //Push the name of the datetype because it's the top level of the tree.
      tokens.insert(tokens.begin(), dt->GetName());

   std::string currentCategory;

   dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator ti = currentTree.tree_iterator();

   for (std::vector<std::string>::const_iterator i = tokens.begin(); i != tokens.end(); ++i) {
      if (ti == currentTree.end())
         return currentTree.end();

      //std::cout << *i << std::endl;

      //keep a full category string running
      //to create an accurate tree node to compare against.
      //Skip the first token because it is the datatype, not the category.
      if (i != tokens.begin())
         if (currentCategory == "")
            currentCategory += *i;
         else
            currentCategory += dtDAL::ResourceDescriptor::DESCRIPTOR_SEPARATOR + *i;

      ti = ti.tree_ref().find(dtDAL::ResourceTreeNode(*i, currentCategory));
   }
   return ti;
}

void ProjectTests::printTree(const dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator& iter)
{
   for (unsigned tabs = 0; tabs < iter.level(); ++tabs)
      std::cout << "\t";

   std::cout << iter->getNodeText();
   if (!iter->isCategory())
      std::cout << " -- " << iter->getResource().GetResourceIdentifier();
      else
         std::cout << " -> " << iter->getFullCategory();

         std::cout << std::endl;

         for (dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator i = iter.tree_ref().in();
         i != iter.tree_ref().end();
         ++i) {
            printTree(i);

         }
}

void ProjectTests::testReadonlyFailure()
{
   try 
   {
      dtDAL::Project& p = dtDAL::Project::GetInstance();

      std::string projectDir("TestProject");

      p.CreateContext(projectDir);

      try 
      {
         p.SetContext(projectDir);
      } 
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL(std::string("Project should have been able to set context. Exception: ") + e.ToString());
      }

      try 
      {
         p.SetContext(projectDir, true);
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
         dtUtil::tree<dtDAL::ResourceTreeNode> toFill;
         p.GetResourcesOfType(dtDAL::DataType::STATIC_MESH, toFill);
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

      try 
      {
         p.DeleteMap("mojo");
         CPPUNIT_FAIL("deleteMap should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }

      try 
      {
         p.SaveMap("mojo");
         CPPUNIT_FAIL("deleteMap should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }

      try 
      {
         p.SaveMapAs("mojo", "a", "b");
         CPPUNIT_FAIL("deleteMap should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }


      try 
      {
         p.AddResource("mojo", std::string("../jojo.ive"),
               std::string("fun:bigmamajama"), dtDAL::DataType::STATIC_MESH);
         CPPUNIT_FAIL("addResource should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }

      try 
      {
         p.RemoveResource(dtDAL::ResourceDescriptor("",""));
         CPPUNIT_FAIL("removeResource should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }

      try 
      {
         p.CreateResourceCategory("name-o", dtDAL::DataType::STRING);
         CPPUNIT_FAIL("createResourceCategory should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }

      try 
      {
         p.RemoveResourceCategory("name-o", dtDAL::DataType::SOUND, true);
         CPPUNIT_FAIL("removeResourceCategory should not be allowed on a readoly context.");
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }

      try 
      {
         p.CreateMap("name-o", "testFile");
         CPPUNIT_FAIL("createMap should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }

      try 
      {
         p.ClearBackup("name-o");
         CPPUNIT_FAIL("clearBackup should not be allowed on a readoly context.");
      } 
      catch (const dtUtil::Exception& e) 
      {
         CPPUNIT_ASSERT_MESSAGE("Exception should have been ExceptionEnum::ProjectReadOnly",
               e.TypeEnum() == dtDAL::ExceptionEnum::ProjectReadOnly);
      }
   } 
   catch (const dtUtil::Exception& ex) 
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception &e) {
   //       CPPUNIT_FAIL(std::string("Caught an exception of type") + typeid(e).name() + " with message " + e.what());
   //    }

}

void ProjectTests::testCategories()
{
   try
   {
      dtDAL::Project& p = dtDAL::Project::GetInstance();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string projectDir("TestProject");

      try {
         p.CreateContext(projectDir);
         p.SetContext(projectDir);
      } catch (const dtUtil::Exception& e) {
         CPPUNIT_FAIL(std::string(std::string("Project should have been able to set context. Exception: ") + e.ToString()).c_str());
      }

      for (std::vector<dtDAL::DataType*>::const_iterator i = dtDAL::DataType::EnumerateType().begin();
      i != dtDAL::DataType::EnumerateType().end(); ++i) {
         dtDAL::DataType& d = **i;

         //don't index the first time so it will be tested both ways.
         if (i != dtDAL::DataType::EnumerateType().begin())
            p.GetAllResources();

         if (!d.IsResource()) {
            try {
               p.CreateResourceCategory("littleFoot", d);
               CPPUNIT_FAIL("Project should not be able to create a category for a primitive type.");
            } catch (const dtUtil::Exception&) {
               //correct
            }
         } else {
            p.CreateResourceCategory("abomb", d);

            CPPUNIT_ASSERT_MESSAGE(
                  "attempting to remove a simple category should succeed.",
                  p.RemoveResourceCategory("abomb", d, false));

            p.CreateResourceCategory("abomb:hbomb", d);

            CPPUNIT_ASSERT_MESSAGE(
                  "attempting to remove a simple category should succeed.",
                  p.RemoveResourceCategory("abomb:hbomb", d, false));

            std::string catPath(p.GetContext() + dtUtil::FileUtils::PATH_SEPARATOR
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

void ProjectTests::testResources()
{
   try
   {
      dtDAL::Project& p = dtDAL::Project::GetInstance();

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      //Open an existing project.
      std::string projectDir = "WorkingProject";

      try
      {
         p.CreateContext(projectDir);
         p.SetContext(projectDir);
      }
      catch (const dtUtil::Exception& e)
      {
         CPPUNIT_FAIL((std::string("Project should have been able to Set context. Exception: ")
               + e.ToString()).c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("Project should not be read only.", !p.IsReadOnly());
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtCore::GetDataFilePathList().find(p.GetContext()) != std::string::npos);

      const std::set<std::string>& mapNames = p.GetMapNames();

      std::vector<dtCore::RefPtr<dtDAL::Map> > maps;

      time_t currentTime;
      time(&currentTime);
      const std::string& utcTime = dtUtil::TimeAsUTC(currentTime);

      logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
            "Current time as UTC is %s", utcTime.c_str());

      std::vector<const dtDAL::ResourceTypeHandler* > handlers;

      p.GetHandlersForDataType(dtDAL::DataType::TERRAIN, handlers);
      CPPUNIT_ASSERT_MESSAGE("There should be 4 terrain type handlers",  handlers.size() == 4);

      std::string testResult;

      try {
         p.AddResource("mojo", std::string("../jojo.ive"),
               std::string("fun:bigmamajama"), dtDAL::DataType::STATIC_MESH);
         CPPUNIT_FAIL("The add resource call to add a non-existent file should have failed.");
      } catch (const dtUtil::Exception& ex) {
         CPPUNIT_ASSERT_MESSAGE(ex.ToString().c_str(), ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectFileNotFound);
         //correct otherwise
      }

      try {
         p.AddResource("dirt", std::string("../terrain_simple.ive"),
               std::string("fun:bigmamajama"), dtDAL::DataType::BOOLEAN);
         CPPUNIT_FAIL("The add resource call to add boolean should have failed.");
      } catch (const dtUtil::Exception& ex) {
         //should not allow a boolean resource to be added.
         CPPUNIT_ASSERT_MESSAGE(ex.ToString().c_str(), ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectResourceError);
         //correct otherwise
      }

      std::string dirtCategory = "fun:bigmamajama";

      //add one marine before indexing
      dtDAL::ResourceDescriptor marineRD = p.AddResource("marine", DATA_DIR + "/marine/marine.rbody", "",
            dtDAL::DataType::CHARACTER);

      dtDAL::ResourceDescriptor terrain1RD = p.AddResource("terrain1", DATA_DIR + "/models/exampleTerrain", "terrain",
            dtDAL::DataType::TERRAIN);

      //force resources to be indexed.
      p.GetAllResources();

      //add one marine after indexing
      dtDAL::ResourceDescriptor marine2RD = p.AddResource("marine2", DATA_DIR + "/marine/marine.rbody", "marine",
            dtDAL::DataType::CHARACTER);


      dtDAL::ResourceDescriptor terrain2RD = p.AddResource("terrain2", DATA_DIR + "/models/exampleTerrain/terrain.3ds", "",
            dtDAL::DataType::TERRAIN);

      //printTree(p.GetAllResources());

      dtUtil::tree<dtDAL::ResourceTreeNode> toFill;
      p.GetResourcesOfType(dtDAL::DataType::CHARACTER, toFill);
      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator marineCategory =
         findTreeNodeFromCategory(toFill, NULL, "");

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"")
            + "\" should have been found in the resource tree", marineCategory != p.GetAllResources().end());

      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator marineResource =
         marineCategory.tree_ref().find(dtDAL::ResourceTreeNode("marine.rbody", marineCategory->getFullCategory(), &marineRD));


      marineCategory = findTreeNodeFromCategory(toFill, NULL, "marine");

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"marine")
            + "\" should have been found in the resource tree", marineCategory != p.GetAllResources().end());

      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator marine2Resource =
         marineCategory.tree_ref().find(dtDAL::ResourceTreeNode("marine2.rbody", marineCategory->getFullCategory(), &marine2RD));

      CPPUNIT_ASSERT_MESSAGE("The marine resource should have been found.", marineResource != p.GetAllResources().end());
      CPPUNIT_ASSERT_MESSAGE("The second marine resource should have been found.", marine2Resource != p.GetAllResources().end());

      std::string characterDir(p.GetContext() + dtUtil::FileUtils::PATH_SEPARATOR + dtDAL::DataType::CHARACTER.GetName() + dtUtil::FileUtils::PATH_SEPARATOR);

      CPPUNIT_ASSERT(fileUtils.DirExists(characterDir + "marine.rbody") &&
            fileUtils.FileExists(characterDir + "marine.rbody" + dtUtil::FileUtils::PATH_SEPARATOR + "marine.rbody"));

      CPPUNIT_ASSERT(fileUtils.DirExists(characterDir + "marine" +
            dtUtil::FileUtils::PATH_SEPARATOR + "marine2.rbody") &&
            fileUtils.FileExists(characterDir + "marine" + dtUtil::FileUtils::PATH_SEPARATOR + "marine2.rbody" + dtUtil::FileUtils::PATH_SEPARATOR + "marine2.rbody"));

      //Done with the marines

      p.GetResourcesOfType(dtDAL::DataType::TERRAIN, toFill);
      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator terrainCategory =
         findTreeNodeFromCategory(toFill, NULL, "");

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"")
            + "\" should have been found in the resource tree", terrainCategory != p.GetAllResources().end());

      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator terrain2Resource =
         terrainCategory.tree_ref().find(dtDAL::ResourceTreeNode("terrain2", terrainCategory->getFullCategory(), &terrain2RD));


      terrainCategory = findTreeNodeFromCategory(toFill, NULL, "terrain");

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"terrain")
            + "\" should have been found in the resource tree", terrainCategory != p.GetAllResources().end());

      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator terrain1Resource =
         terrainCategory.tree_ref().find(dtDAL::ResourceTreeNode("terrain1", terrainCategory->getFullCategory(), &terrain1RD));

      CPPUNIT_ASSERT_MESSAGE("The terrain2 resource should have been found.", terrain2Resource != p.GetAllResources().end());
      CPPUNIT_ASSERT_MESSAGE("The terrain1 resource should have been found.", terrain1Resource != p.GetAllResources().end());

      std::string terrainDir(p.GetContext() + dtUtil::FileUtils::PATH_SEPARATOR + dtDAL::DataType::TERRAIN.GetName() + dtUtil::FileUtils::PATH_SEPARATOR);

      CPPUNIT_ASSERT(fileUtils.DirExists(terrainDir + "terrain2.3dst" ) &&
            fileUtils.FileExists(terrainDir + "terrain2.3dst" + dtUtil::FileUtils::PATH_SEPARATOR + "terrain.3ds"));

      CPPUNIT_ASSERT(fileUtils.DirExists(terrainDir + "terrain" +
            dtUtil::FileUtils::PATH_SEPARATOR + "terrain1.3dst") &&
            fileUtils.FileExists(terrainDir + "terrain" + dtUtil::FileUtils::PATH_SEPARATOR + "terrain1.3dst" + dtUtil::FileUtils::PATH_SEPARATOR + "terrain.3ds"));

      //Done with the terrains

      dtDAL::ResourceDescriptor rd = p.AddResource("flatdirt", std::string(DATA_DIR + "/models/flatdirt.ive"),
            dirtCategory, dtDAL::DataType::STATIC_MESH);

      CPPUNIT_ASSERT_MESSAGE("Descriptor id should not be empty.", !rd.GetResourceIdentifier().empty());

      testResult = p.GetResourcePath(rd);

      CPPUNIT_ASSERT_MESSAGE("Getting the resource path returned the wrong value: " + testResult,
            testResult == dtDAL::DataType::STATIC_MESH.GetName() + dtUtil::FileUtils::PATH_SEPARATOR
            + "fun" + dtUtil::FileUtils::PATH_SEPARATOR + "bigmamajama"
            + dtUtil::FileUtils::PATH_SEPARATOR + "flatdirt.ive");


      for (std::set<std::string>::const_iterator i = mapNames.begin(); i != mapNames.end(); i++) {
         logger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Found map named %s.", i->c_str());
         //dtDAL::Map& m = p.GetMap(*i);

         //maps.Push_back(dtCore::RefPtr<dtDAL::Map>(&m));
      }

      p.GetResourcesOfType(dtDAL::DataType::STATIC_MESH, toFill);

      CPPUNIT_ASSERT_MESSAGE("The head of the tree should be static mesh",
            toFill.data().getNodeText() == dtDAL::DataType::STATIC_MESH.GetName());

      dtUtil::tree<dtDAL::ResourceTreeNode>::const_iterator treeResult =
         findTreeNodeFromCategory(p.GetAllResources(),
               &dtDAL::DataType::STATIC_MESH, dirtCategory);

      //printTree(toFill.tree_iterator());

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should have been found in the resource tree", treeResult != p.GetAllResources().end());


      CPPUNIT_ASSERT_MESSAGE(std::string("the resource \"") + rd.GetResourceIdentifier()
            + "\" should have been found in the resource tree",
            treeResult.tree_ref().find(dtDAL::ResourceTreeNode(rd.GetDisplayName(), dirtCategory, &rd))
            != p.GetAllResources().end());

      p.RemoveResource(rd);

      treeResult = findTreeNodeFromCategory(p.GetAllResources(),
            &dtDAL::DataType::STATIC_MESH, dirtCategory);

      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should have been found in the resource tree", treeResult != p.GetAllResources().end());

      CPPUNIT_ASSERT_MESSAGE(std::string("the resource \"") + rd.GetResourceIdentifier()
            + "\" should have NOT been found in the resource tree",
            treeResult.tree_ref().find(dtDAL::ResourceTreeNode(rd.GetDisplayName(), dirtCategory,  &rd))
            == p.GetAllResources().end());

      CPPUNIT_ASSERT(!p.RemoveResourceCategory("fun", dtDAL::DataType::STATIC_MESH, false));
      CPPUNIT_ASSERT(p.RemoveResourceCategory("fun", dtDAL::DataType::STATIC_MESH, true));

      treeResult = findTreeNodeFromCategory(p.GetAllResources(),
            &dtDAL::DataType::STATIC_MESH, dirtCategory);
      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + dirtCategory
            + "\" should not have been found in the resource tree", treeResult == p.GetAllResources().end());

      treeResult = findTreeNodeFromCategory(p.GetAllResources(),
            &dtDAL::DataType::STATIC_MESH, "fun");
      CPPUNIT_ASSERT_MESSAGE(std::string("the category \"") + "fun"
            + "\" should not have been found in the resource tree", treeResult == p.GetAllResources().end());

      rd = p.AddResource("pow", std::string(DATA_DIR + "/sounds/pow.wav"), std::string("tea:money"), dtDAL::DataType::SOUND);
      testResult = p.GetResourcePath(rd);

      CPPUNIT_ASSERT_MESSAGE("Getting the resource path returned the wrong value: " + testResult ,
            testResult == dtDAL::DataType::SOUND.GetName() + dtUtil::FileUtils::PATH_SEPARATOR
            + "tea" + dtUtil::FileUtils::PATH_SEPARATOR
            + "money" + dtUtil::FileUtils::PATH_SEPARATOR + "pow.wav");

      dtDAL::ResourceDescriptor rd1 = p.AddResource("bang", std::string(DATA_DIR + "/sounds/bang.wav"),
            std::string("tee:cash"), dtDAL::DataType::SOUND);
      testResult = p.GetResourcePath(rd1);

      CPPUNIT_ASSERT_MESSAGE("Getting the resource path returned the wrong value: " + testResult,
            testResult == dtDAL::DataType::SOUND.GetName() + dtUtil::FileUtils::PATH_SEPARATOR + "tee"
            + dtUtil::FileUtils::PATH_SEPARATOR + "cash" + dtUtil::FileUtils::PATH_SEPARATOR + "bang.wav");

      p.Refresh();

      //const dtUtil::tree<dtDAL::ResourceTreeNode>& allTree = p.GetAllResources();

      //printTree(allTree.tree_iterator());

      p.RemoveResource(rd);
      p.RemoveResource(rd1);
      p.RemoveResource(marineRD);
      p.RemoveResource(marine2RD);
      p.RemoveResource(terrain1RD);
      p.RemoveResource(terrain2RD);

      fileUtils.PushDirectory(projectDir);
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::STATIC_MESH.GetName() + std::string("/fun/bigmamajama/terrain_simple.ive")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::SOUND.GetName() + std::string("/tea/money/pow.wav")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::SOUND.GetName() + std::string("/tee/cash/bang.wav")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::CHARACTER.GetName() + std::string("/marine.rbody")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.FileExists(dtDAL::DataType::CHARACTER.GetName() + std::string("/marine/marine2.rbody")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.DirExists(dtDAL::DataType::TERRAIN.GetName() + std::string("/terrain2.3dst")));
      CPPUNIT_ASSERT_MESSAGE("Resource should have been deleted, but the file still exists.",
            !fileUtils.DirExists(dtDAL::DataType::TERRAIN.GetName() + std::string("/terrain/terrain1.3dst")));
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


void ProjectTests::testProject()
{
   try
   {
      dtDAL::Project& p = dtDAL::Project::GetInstance();
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      std::string originalPathList = dtCore::GetDataFilePathList();

      std::string crapPath("/usr/:**/../^^jojo/funky/\\\\/,/,.uchor");
      
      CPPUNIT_ASSERT_THROW(p.CreateContext(crapPath), dtUtil::Exception);
      CPPUNIT_ASSERT_THROW(p.SetContext(crapPath), dtUtil::Exception);

      std::string projectDir("TestProject");

      if (fileUtils.FileExists(projectDir))
      {
         try
         {
            fileUtils.DirDelete(projectDir, true);
         }
         catch (const dtUtil::Exception& ex)
         {
            CPPUNIT_FAIL(ex.ToString().c_str());
         }

         CPPUNIT_ASSERT_MESSAGE("The project Directory should not yet exist.", !fileUtils.FileExists(projectDir));
      }

      fileUtils.MakeDirectory(projectDir);

      try
      {
         p.SetContext(projectDir);
         CPPUNIT_FAIL("Project should not have been able to Set the context because it is empty.");
      }
      catch (const dtUtil::Exception&)
      {
         //correct
      }

      try {
         p.CreateContext(projectDir);
         p.SetContext(projectDir);
      } catch (const dtUtil::Exception& e) {
         CPPUNIT_FAIL(std::string(std::string("Project should have been able to Set context. Exception: ") + e.ToString()).c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("Project should not be read only.", !p.IsReadOnly());
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtCore::GetDataFilePathList().find(p.GetContext()) != std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the original path list.",
            dtCore::GetDataFilePathList().find(originalPathList) != std::string::npos);

      try {
         p.SetContext(projectDir, true);
      } catch (const dtUtil::Exception& e) {
         CPPUNIT_FAIL(std::string(std::string("Project should have been able to Set context. Exception: ") + e.ToString()).c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("Project should be read only.", p.IsReadOnly());
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtCore::GetDataFilePathList().find(p.GetContext()) != std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the original path list.",
            dtCore::GetDataFilePathList().find(originalPathList) != std::string::npos);

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
            dtCore::GetDataFilePathList().find(p.GetContext()) != std::string::npos);

      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the context.",
            dtCore::GetDataFilePathList().find(projectDir2) != std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should NOT contain the old context.",
            dtCore::GetDataFilePathList().find(projectDir) == std::string::npos);
      CPPUNIT_ASSERT_MESSAGE("Delta3D search path should contain the original path list.",
            dtCore::GetDataFilePathList().find(originalPathList) != std::string::npos);

      try {
         fileUtils.DirDelete(projectDir, true);
      } catch (const dtUtil::Exception& ex) {
         CPPUNIT_FAIL(ex.ToString().c_str());
      }


      CPPUNIT_ASSERT_MESSAGE("The project Directory should have been deleted.", !fileUtils.FileExists(projectDir));

   } catch (const dtUtil::Exception& ex) {
      CPPUNIT_FAIL(ex.ToString());
   }
   //    catch (const std::exception& ex) {
   //        CPPUNIT_FAIL(ex.what());
   //    }

}



