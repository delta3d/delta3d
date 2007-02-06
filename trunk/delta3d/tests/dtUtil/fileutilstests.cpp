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
 * @author William E. Johnson II
 */
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>

#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtCore/globals.h>

class FileUtilsTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(FileUtilsTests);
      CPPUNIT_TEST(testFileIO);
      CPPUNIT_TEST(testRelativePath);
   CPPUNIT_TEST_SUITE_END();

   public:
   
      void setUp();
      void tearDown();

      void testFileIO();
      void testRelativePath();
   
   private:

      dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(FileUtilsTests);

const std::string DATA_DIR = dtCore::GetDeltaRootPath()+dtUtil::FileUtils::PATH_SEPARATOR+"data";
const std::string TESTS_DIR = dtCore::GetDeltaRootPath()+dtUtil::FileUtils::PATH_SEPARATOR+"tests";
const std::string MAPPROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingMapProject";
const std::string PROJECTCONTEXT = TESTS_DIR + dtUtil::FileUtils::PATH_SEPARATOR + "dtDAL" + dtUtil::FileUtils::PATH_SEPARATOR + "WorkingProject";

void FileUtilsTests::setUp() 
{
   try 
   {
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
   } 
   catch (const dtUtil::Exception& ex) 
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}


void FileUtilsTests::tearDown() 
{
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

void FileUtilsTests::testFileIO()
{
   try
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string Dir1("Testing");
      std::string Dir2Name("Testing1");
      std::string Dir2(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2Name);

      //cleanup
      try
      {
         fileUtils.DirDelete(Dir1, true);
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_ASSERT_MESSAGE((ex.ToString() + ": Error deleting Directory, but file exists.").c_str(),
            ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectFileNotFound);
      }

      fileUtils.MakeDirectory(Dir1);
      fileUtils.MakeDirectory(Dir2);

      std::string file1("terrain_simple.ive");
      std::string file2("flatdirt.ive");

      dtUtil::FileInfo file1Info = fileUtils.GetFileInfo(file1);
      dtUtil::FileInfo file2Info = fileUtils.GetFileInfo(file2);

      try
      {
         fileUtils.GetFileInfo(file2 + "euaoeuaiao.ao.u");
      }
      catch (const dtUtil::Exception& ex) 
      {
         //this should throw a file not found.
         CPPUNIT_ASSERT_MESSAGE(ex.ToString().c_str(), ex.TypeEnum() == dtDAL::ExceptionEnum::ProjectFileNotFound);
         //correct
      }

      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should exist.", fileUtils.FileExists(file1));

      fileUtils.FileCopy(file1, Dir1, false);

      CPPUNIT_ASSERT_MESSAGE("The original terrain_simple.ive should exist.", fileUtils.FileExists(file1));

      CPPUNIT_ASSERT_MESSAGE("The new terrain_simple.ive should exist.", fileUtils.FileExists(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      fileUtils.FileCopy(file1, Dir1, true);

      CPPUNIT_ASSERT_MESSAGE("The original terrain_simple.ive should exist.", fileUtils.FileExists(file1));

      CPPUNIT_ASSERT_MESSAGE("The new terrain_simple.ive should exist.", fileUtils.FileExists(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      try 
      {
         fileUtils.FileCopy(file2, Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1, false);
         CPPUNIT_FAIL("The file copy should have failed since it was attempting to overwrite the file and overwriting was disabled.");
      } 
      catch (const dtUtil::Exception&)
      {
         //correct
      }

      CPPUNIT_ASSERT_MESSAGE("The original flatdirt.ive should exist.", fileUtils.FileExists(file2));

      CPPUNIT_ASSERT_MESSAGE("The new terrain_simple.ive should still exist.", fileUtils.FileExists(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      struct dtUtil::FileInfo fi = fileUtils.GetFileInfo(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1);
      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should be the same size as the original", fi.size == file1Info.size);

      try 
      {
         fileUtils.FileCopy(file2, Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1, true);
      } 
      catch (const dtUtil::Exception& ex) 
      {
         CPPUNIT_FAIL(ex.ToString().c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("The original flatdirt.ive should exist.", fileUtils.FileExists(file2));

      CPPUNIT_ASSERT_MESSAGE("The new terrain_simple.ive, copied from flatdirt.ive, should exist.",
         fileUtils.FileExists(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      fi = fileUtils.GetFileInfo(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1);

      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should be the same size as flatdirt.ive", fi.size == file2Info.size);

      try 
      {
         fileUtils.FileMove(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1, Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1, false);
      } 
      catch (const dtUtil::Exception& ex) 
      {
         CPPUNIT_FAIL(ex.ToString().c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should not exist.",!fileUtils.FileExists(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      CPPUNIT_ASSERT_MESSAGE("The new terrain_simple.ive, copied from terrain_simple.ive, should exist.",
         fileUtils.FileExists(Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      fi = fileUtils.GetFileInfo(Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1);
      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should be the same size as flatdirt.ive", fi.size == file2Info.size);

      //copy the file back so we can try to move it again with overwriting.
      try 
      {
         fileUtils.FileCopy(Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1, Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1, false);
      } 
      catch (const dtUtil::Exception& ex) 
      {
         CPPUNIT_FAIL(ex.ToString().c_str());
      }

      try 
      {
         fileUtils.FileMove(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1, Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1, false);
         CPPUNIT_FAIL("Moving the file should have failed since overwriting was turned off.");
      } 
      catch (const dtUtil::Exception&) 
      {
         //correct
      }

      try 
      {
         fileUtils.FileMove(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1, Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1, true);
      } 
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString().c_str());
      }

      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should not exist.",
         !fileUtils.FileExists(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      CPPUNIT_ASSERT_MESSAGE("The new terrain_simple.ive, copied from terrain_simple.ive, should exist.",
         fileUtils.FileExists(Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      try 
      {
         //copy a directory into itself with "copy contents only"
         fileUtils.DirCopy(Dir1, Dir1+ dtUtil::FileUtils::PATH_SEPARATOR + ".."+ dtUtil::FileUtils::PATH_SEPARATOR + Dir1, true, true);
         CPPUNIT_FAIL("DirCopy should not be able to copy a directory onto itself.");
      } 
      catch (const dtUtil::Exception&)
      {
         //correct
      }

      try 
      {
         //copy a directory into the parent without contents, so that it would try to recreate the same directory.
         fileUtils.DirCopy(Dir1, ".", true, false);
         CPPUNIT_FAIL("DirCopy should not be able to copy a directory onto itself.");
      } 
      catch (const dtUtil::Exception&) 
      {
         //correct
      }

      try 
      {
         //copy a directory into the parent without contents, so that it would try to recreate the same directory.
         fileUtils.DirCopy(Dir1, Dir1, true, false);
         //doing it again should do nothing.
         fileUtils.DirCopy(Dir1, Dir1, true, false);
      } 
      catch (const dtUtil::Exception&)
      {
         CPPUNIT_FAIL("DirCopy should be able to copy a directory into itself as a subdirectory.");
      }

      try 
      {
         //copy a directory into the parent without contents, so that it would try to recreate the same directory.
         fileUtils.DirCopy(Dir1, Dir1, false, false);
         CPPUNIT_FAIL("DirCopy should not be able to overwrite files if overwriting is set to false.");
      } 
      catch (const dtUtil::Exception&)
      {
      }

      std::string Dir3("recursiveDir");

      fileUtils.MakeDirectory(Dir3);
      //copy with the directory existing.
      fileUtils.DirCopy(Dir1, Dir3, false);

      CPPUNIT_ASSERT_MESSAGE("The recursive file should exist.",
         fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      CPPUNIT_ASSERT_MESSAGE("The recursive file should exist.",
         fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir1
         + dtUtil::FileUtils::PATH_SEPARATOR + Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));


      fileUtils.DirDelete(Dir3, true);

      CPPUNIT_ASSERT_MESSAGE("The recursive file not should exist.",
         !fileUtils.DirExists(Dir3));

      //copy with the directory NOT existing.
      fileUtils.DirCopy(Dir1, Dir3, false);

      CPPUNIT_ASSERT_MESSAGE("The recursive file should NOT exist.",
         !fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir1
         + dtUtil::FileUtils::PATH_SEPARATOR + Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      CPPUNIT_ASSERT_MESSAGE("The recursive file should exist.",
         fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2Name + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      CPPUNIT_ASSERT_MESSAGE("The recursive file should exist.",
         fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));


      fileUtils.DirDelete(Dir3, true);

      CPPUNIT_ASSERT_MESSAGE("The recursive file not should exist.",
         !fileUtils.DirExists(Dir3));

      fileUtils.MakeDirectory(Dir3);
      //copy with the directory existing, but with copy contents only. This should be the same as the directory NOT existing before the call.
      fileUtils.DirCopy(Dir1, Dir3, false, true);
      CPPUNIT_ASSERT_MESSAGE("The recursive file should NOT exist.",
         !fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir1
         + dtUtil::FileUtils::PATH_SEPARATOR + Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      CPPUNIT_ASSERT_MESSAGE("The recursive file should exist.",
         fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2Name + dtUtil::FileUtils::PATH_SEPARATOR + file1));

      CPPUNIT_ASSERT_MESSAGE("The recursive file should exist.",
         fileUtils.FileExists(Dir3 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1));


      fi = fileUtils.GetFileInfo(Dir2 + dtUtil::FileUtils::PATH_SEPARATOR + file1);
      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should be the same size as flatdirt.ive", fi.size == file2Info.size);

      dtUtil::DirectoryContents dc = fileUtils.DirGetSubs(Dir1);

      CPPUNIT_ASSERT_MESSAGE((Dir1 + " Should only contain 2 entries.").c_str(), dc.size() == 2);

      if(!dc.empty())
      {
         for(dtUtil::DirectoryContents::const_iterator i = dc.begin(); i != dc.end(); ++i) 
         {
            const std::string& s = *i;
            CPPUNIT_ASSERT_MESSAGE((Dir1 + " Should only contain 2 entries and they should be \"Testing1\" and \"Testing\".").c_str(),
               s == "Testing1" || s == "Testing");
         }
      }

      //Testing the delete functionality tests DirGetFiles
      try 
      {
         CPPUNIT_ASSERT_MESSAGE("Deleting an nonexisten Directory should be ok.", fileUtils.DirDelete("gobbletygook", false) == true);
      } 
      catch (const dtUtil::Exception&) 
      {
         CPPUNIT_FAIL("Deleting an nonexisten Directory should be ok.");
      }

      //Testing the delete functionality tests DirGetFiles
      try 
      {
         fileUtils.DirDelete(Dir1, false);
      } 
      catch (const dtUtil::Exception&) 
      {
         CPPUNIT_FAIL("Deleting non-empty Directory with a non-recursive call should have returned false.");
      }
      CPPUNIT_ASSERT_MESSAGE(Dir1 + " should still exist.", fileUtils.DirExists(Dir1));
      try 
      {
         fileUtils.DirDelete(Dir1, true);
      } 
      catch (const dtUtil::Exception& ex) 
      {
         CPPUNIT_FAIL((ex.ToString() + ": Deleting non-empty Directory with a non-recursive call should not have generated an Exception.").c_str());
      }
      CPPUNIT_ASSERT_MESSAGE(Dir1 + " should not still exist.", !fileUtils.DirExists(Dir1));
   } 
   catch (const dtUtil::Exception& ex) 
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

void FileUtilsTests::testRelativePath()
{
   std::string file = dtCore::FindFileInPathList("map.xsd");
   CPPUNIT_ASSERT(!file.empty());

   std::string deltaRoot = dtCore::GetDeltaRootPath();
   CPPUNIT_ASSERT(!deltaRoot.empty());

   // Normalize directory separators
   for(size_t i = 0; i < file.size(); i++)
   {
      if(file[i] == '\\')
         file[i] = '/';

      if(deltaRoot[i] == '\\')
         deltaRoot[i] = '/';
   }

   std::string relativePath = dtUtil::FileUtils::GetInstance().RelativePath(deltaRoot, file);
   CPPUNIT_ASSERT(!relativePath.empty());

   CPPUNIT_ASSERT_MESSAGE("The relative path should be: data/map.xsd", 
                          relativePath == "data/map.xsd");
}