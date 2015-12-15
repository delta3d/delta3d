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
 * @author William E. Johnson II
 */

#include <prefix/unittestprefix.h>

#include <dtUtil/datapathutils.h>

#include <dtCore/datatype.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/stringutils.h>

#include <cppunit/extensions/HelperMacros.h>

#include <osg/Version>
#include <osgDB/FileNameUtils>
#include <osgDB/Registry>
#include <osgDB/AuthenticationMap>
#include <osgDB/ReadFile>

std::string GetExamplesDataDir();
std::string GetTestsDir();

class FileUtilsTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(FileUtilsTests);

      CPPUNIT_TEST(testFileIO1);
      CPPUNIT_TEST(testFileIO2);
      CPPUNIT_TEST(testFileIOCaseInsensitive);
      CPPUNIT_TEST(testRelativePath);
      CPPUNIT_TEST(testRelativePathComplex);
      CPPUNIT_TEST(testConcatPaths);
      CPPUNIT_TEST(TestAbsolutePath);
      CPPUNIT_TEST(testDirectoryContentsWithOneFilter);
      CPPUNIT_TEST(testDirectoryContentsWithTwoFilters);
      CPPUNIT_TEST(testDirectoryContentsWithDuplicateFilter);

      CPPUNIT_TEST(testLoadFromArchive);
      CPPUNIT_TEST(testArchiveRelativePath);
      CPPUNIT_TEST(testIsSameFile);
      CPPUNIT_TEST(testIsSameFileInArchive);
      CPPUNIT_TEST(testDirExistsInArchive);
      CPPUNIT_TEST(testFileExistsInArchive);
      CPPUNIT_TEST(testLoadFromPasswordProtectedArchive);
      CPPUNIT_TEST(testArchiveReadNodeFile);


   CPPUNIT_TEST_SUITE_END();

   public:

      void setUp();
      void tearDown();

      void testFileIO1();
      void testFileIO2();
      void testFileIOCaseInsensitive();
      void testRelativePath();
      void testRelativePathComplex();
      void TestAbsolutePath();
      void testConcatPaths();
      void testCopyFileOntoItself();
      //void testAbsoluteToRelativePath();
      void testDirectoryContentsWithOneFilter();
      void testDirectoryContentsWithTwoFilters();
      void testDirectoryContentsWithDuplicateFilter();

      void testLoadFromArchive();
      void testArchiveRelativePath();
      void testIsSameFile();
      void testIsSameFileInArchive();
      void testDirExistsInArchive();
      void testFileExistsInArchive();
      void testLoadFromPasswordProtectedArchive();
      void testArchiveReadNodeFile();

   private:

      void NormalizeDirectorySlashes(std::string& str)
      {
         for (size_t i = 0; i < str.size(); i++)
         {
            if (str[i] == '\\')
               str[i] = '/';
         }
      }

      dtUtil::Log* logger;
};

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(FileUtilsTests);


//////////////////////////////////////////////////////////////////////////
std::string getFileExtensionIncludingDot(const std::string& fileName)
{
#if defined(OSG_VERSION_MAJOR) && defined(OSG_VERSION_MINOR) && OSG_VERSION_MAJOR <= 2  && OSG_VERSION_MINOR <= 6
   //copied from osgDB/FileNameUtils.cpp (v2.8.0)
   std::string::size_type dot = fileName.find_last_of('.');
   if (dot==std::string::npos) return std::string("");
   return std::string(fileName.begin()+dot,fileName.end());
#else
   return osgDB::getFileExtensionIncludingDot(fileName);
#endif
}

//////////////////////////////////////////////////////////////////////////
void FileUtilsTests::setUp()
{
   try
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      GetExamplesDataDir() = fileUtils.GetAbsolutePath(GetExamplesDataDir());
      GetTestsDir() = fileUtils.GetAbsolutePath(GetTestsDir());

      dtUtil::SetDataFilePathList(dtUtil::GetDataFilePathList() + ";" +"../examples/data/;");
      std::string logName("projectTest");

      logger = &dtUtil::Log::GetInstance(logName);

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
   {
      fileUtils.DirDelete("TestProject", true);
   }
   if (fileUtils.DirExists("Test2Project"))
   {
      fileUtils.DirDelete("Test2Project", true);
   }

   if (fileUtils.DirExists("WorkingProject"))
   {
      fileUtils.DirDelete("WorkingProject", true);
   }

   std::string currentDir = fileUtils.CurrentDirectory();
   std::string projectDir("dtCore");
   if (currentDir.substr(currentDir.size() - projectDir.size()) == projectDir)
   {
      fileUtils.PopDirectory();
   }
}

void FileUtilsTests::testFileIO1()
{
   try
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      const std::string Dir1("Testing");
      const std::string Dir2Name("Testing1");
      const std::string Dir2(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2Name);

      const std::string file1("terrain_simple.ive");
      const std::string file2("flatdirt.ive");

      //cleanup
      CPPUNIT_ASSERT_EQUAL_MESSAGE("Deleting a non-existent directory returned as a failure.",
                                    true, fileUtils.DirDelete(Dir1, true));
      fileUtils.MakeDirectory(Dir1);
      fileUtils.MakeDirectory(Dir2);


      dtUtil::FileInfo file1Info = fileUtils.GetFileInfo(file1);
      dtUtil::FileInfo file2Info = fileUtils.GetFileInfo(file2);

      try
      {
         //normally doesn't throw an exception
         fileUtils.GetFileInfo(file2 + "euaoeuaiao.ao.u");
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL(ex.ToString());
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

   } catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

void FileUtilsTests::testFileIO2()
{
   try
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      const std::string Dir1("Testing");
      const std::string Dir2Name("Testing1");
      const std::string Dir2(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2Name);

      const std::string file1("terrain_simple.ive");
      const std::string file2("flatdirt.ive");

      dtUtil::FileInfo file2Info = fileUtils.GetFileInfo(file2);

      //cleanup
      try
      {
         //Shouldn't throw, even if the directory doesn't exist
         fileUtils.DirDelete(Dir1, true);
      }
      catch (const dtUtil::Exception& ex)
      {
         CPPUNIT_FAIL((ex.ToString() + ": Error deleting Directory, but file exists.").c_str());
      }

      fileUtils.MakeDirectory(Dir1);
      fileUtils.MakeDirectory(Dir2);

      CPPUNIT_ASSERT_MESSAGE("terrain_simple.ive should exist.", fileUtils.FileExists(file1));
      CPPUNIT_ASSERT_MESSAGE("flatdirt.ive should exist.", fileUtils.FileExists(file2));

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

      dtUtil::FileInfo fi = fileUtils.GetFileInfo(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + file1);

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
         //Copy a directory into the parent without contents, so that it would try to recreate the same directory.
         //Note that the directory has already been created previously in this same unit test, so this should not
         //be allowed because we'd be overwriting a pre-existing directory.
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

      if (!dc.empty())
      {
         for (dtUtil::DirectoryContents::const_iterator i = dc.begin(); i != dc.end(); ++i)
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

void FileUtilsTests::testFileIOCaseInsensitive()
{
   try
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      const std::string Dir1("Testing");
      const std::string Dir2Name("Testing1");
      const std::string Dir2(Dir1 + dtUtil::FileUtils::PATH_SEPARATOR + Dir2Name);

      std::string file1("terrain_simple.ive");
      std::string file2("flatdirt.ive");

      dtUtil::FileInfo file1Info = fileUtils.GetFileInfo(file1);
      dtUtil::FileInfo file2Info = fileUtils.GetFileInfo(file2);

      CPPUNIT_ASSERT(file1Info.fileType == dtUtil::REGULAR_FILE);
      CPPUNIT_ASSERT(file2Info.fileType == dtUtil::REGULAR_FILE);

      CPPUNIT_ASSERT_EQUAL(std::string("terrain_simple"), file1Info.extensionlessFileName);
      CPPUNIT_ASSERT_EQUAL(std::string("flatdirt"), file2Info.extensionlessFileName);

      CPPUNIT_ASSERT_EQUAL(std::string("ive"), file1Info.extension);
      CPPUNIT_ASSERT_EQUAL(std::string("ive"), file2Info.extension);

#ifndef DELTA_WIN32
      std::string file1ABS = fileUtils.GetAbsolutePath(file1);
      std::string file2ABS = fileUtils.GetAbsolutePath(file2);

      dtUtil::ToUpperCase(file1);
      dtUtil::ToUpperCase(file2);

      // mess up the whole path to make sure it all works.
      dtUtil::ToUpperCase(file1ABS);
      dtUtil::ToUpperCase(file2ABS);

      dtUtil::FileInfo file1InfoCI = fileUtils.GetFileInfo(file1);
      dtUtil::FileInfo file2InfoCI = fileUtils.GetFileInfo(file2);

      CPPUNIT_ASSERT(file1InfoCI.fileType == dtUtil::FILE_NOT_FOUND);
      CPPUNIT_ASSERT(file2InfoCI.fileType == dtUtil::FILE_NOT_FOUND);

      file1InfoCI = fileUtils.GetFileInfo(file1, true);
      file2InfoCI = fileUtils.GetFileInfo(file2, true);

      CPPUNIT_ASSERT(file1InfoCI.fileType == dtUtil::REGULAR_FILE);
      CPPUNIT_ASSERT(file2InfoCI.fileType == dtUtil::REGULAR_FILE);

      CPPUNIT_ASSERT_EQUAL(std::string("terrain_simple"), file1InfoCI.extensionlessFileName);
      CPPUNIT_ASSERT_EQUAL(std::string("flatdirt"), file2InfoCI.extensionlessFileName);

      CPPUNIT_ASSERT_EQUAL(std::string("ive"), file1InfoCI.extension);
      CPPUNIT_ASSERT_EQUAL(std::string("ive"), file2InfoCI.extension);

      file1InfoCI = fileUtils.GetFileInfo(file1ABS, true);
      file2InfoCI = fileUtils.GetFileInfo(file2ABS, true);

      CPPUNIT_ASSERT(file1InfoCI.fileType == dtUtil::REGULAR_FILE);
      CPPUNIT_ASSERT(file2InfoCI.fileType == dtUtil::REGULAR_FILE);

      CPPUNIT_ASSERT_EQUAL(std::string("terrain_simple"), file1InfoCI.extensionlessFileName);
      CPPUNIT_ASSERT_EQUAL(std::string("flatdirt"), file2InfoCI.extensionlessFileName);

      CPPUNIT_ASSERT_EQUAL(std::string("ive"), file1InfoCI.extension);
      CPPUNIT_ASSERT_EQUAL(std::string("ive"), file2InfoCI.extension);
#endif

   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}


void FileUtilsTests::testRelativePath()
{
   try
   {
      std::string file = dtUtil::FindFileInPathList("map.xsd");
      CPPUNIT_ASSERT(!file.empty());

      std::string deltaRoot = dtUtil::GetDeltaRootPath();
      CPPUNIT_ASSERT(!deltaRoot.empty());

      deltaRoot = dtUtil::FileUtils::GetInstance().GetAbsolutePath(deltaRoot);

      std::string fileabs = dtUtil::FileUtils::GetInstance().GetAbsolutePath(file);
      std::string dirOnlyABS = dtUtil::FileUtils::GetInstance().GetAbsolutePath(file, true);

      // Normalize directory separators
      NormalizeDirectorySlashes(fileabs);
      NormalizeDirectorySlashes(dirOnlyABS);
      NormalizeDirectorySlashes(file);
      NormalizeDirectorySlashes(deltaRoot);

      std::string relativePath = dtUtil::FileUtils::GetInstance().RelativePath(deltaRoot, file);
      CPPUNIT_ASSERT(!relativePath.empty());

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The relative path should be: data/map.xsd",
            std::string("data/map.xsd"), relativePath);

      CPPUNIT_ASSERT_EQUAL(
            std::string(deltaRoot + "/data/map.xsd"), fileabs);

      CPPUNIT_ASSERT_EQUAL(
            std::string(deltaRoot + "/data"), dirOnlyABS);
   }
   catch (dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   catch (...)
   {
      CPPUNIT_FAIL("Unknown exception thrown.");
   }
}

void FileUtilsTests::TestAbsolutePath()
{
   std::string deltaRoot = dtUtil::GetDeltaRootPath();
   CPPUNIT_ASSERT(!deltaRoot.empty());

   deltaRoot = dtUtil::FileUtils::GetInstance().GetAbsolutePath(deltaRoot);

   dtUtil::DirectoryPush dp(deltaRoot);

   CPPUNIT_ASSERT(dp.GetSucceeded());

   CPPUNIT_ASSERT_EQUAL(deltaRoot, dtUtil::FileUtils::GetInstance().CurrentDirectory());

   std::string abspath = dtUtil::FileUtils::GetInstance().GetAbsolutePath("data/map.xsd");

   CPPUNIT_ASSERT_EQUAL(deltaRoot + dtUtil::FileUtils::PATH_SEPARATOR + "data" + dtUtil::FileUtils::PATH_SEPARATOR + "map.xsd",
            abspath);
}


void FileUtilsTests::testRelativePathComplex()
{
   {
      std::string path1("/fun/is/a/good/thing");
      // beginning with a \\ doesn't work, but all others do.
      std::string path2("/fun\\is\\a\\bad\\time.png");

      std::string result = dtUtil::FileUtils::GetInstance().RelativePath(path1, path2);
      CPPUNIT_ASSERT_EQUAL(std::string("../../bad/time.png"), result);
   }
   {
      std::string path1("C:/We/Love/chicken/burritos");
      // beginning with a \\ doesn't work, but all others do.
      std::string path2("D:\\fun\\is\\a\\bad\\time.png");
      std::string result = dtUtil::FileUtils::GetInstance().RelativePath(path1, path2);
      // should return the original because the drive letter is different.
      CPPUNIT_ASSERT_EQUAL(path2, result);
   }
   {
      std::string path1("C:/We/Love/chicken/burritos");
      // beginning with a \\ doesn't work, but all others do.
      std::string path2("C:\\fun\\is\\a\\bad\\time.png");
      std::string result = dtUtil::FileUtils::GetInstance().RelativePath(path1, path2);
      CPPUNIT_ASSERT_EQUAL(std::string("../../../../fun/is/a/bad/time.png"), result);
   }
}

void FileUtilsTests::testConcatPaths()
{
   std::string sepStr(1U, dtUtil::FileUtils::PATH_SEPARATOR);
   CPPUNIT_ASSERT_EQUAL(std::string("test1" + sepStr + "test2.ooo"), dtUtil::FileUtils::ConcatPaths("test1", "test2.ooo"));
   CPPUNIT_ASSERT_EQUAL(std::string("test1" + sepStr + "test2.ooo"), dtUtil::FileUtils::ConcatPaths("test1/", "test2.ooo"));
   //CPPUNIT_ASSERT_EQUAL(std::string("test1" + sepStr + "test2.ooo"), dtUtil::FileUtils::ConcatPaths("test1/", "/test2.ooo"));
   //CPPUNIT_ASSERT_EQUAL(std::string("test1" + sepStr + "test2.ooo"), dtUtil::FileUtils::ConcatPaths("test1\\", "\\test2.ooo"));
   CPPUNIT_ASSERT_EQUAL(std::string("test1" + sepStr + "test2.ooo"), dtUtil::FileUtils::ConcatPaths("test1\\", "test2.ooo"));
   //CPPUNIT_ASSERT_EQUAL(std::string("test1" + sepStr + "test2.ooo"), dtUtil::FileUtils::ConcatPaths("test1", "\\test2.ooo"));
}

void FileUtilsTests::testCopyFileOntoItself()
{
   const std::string testPath = GetExamplesDataDir() + "/aNewFilePath";

   const std::string path1 = testPath + "/aFile.txt";
   const std::string path2 = testPath + "/../aNewFilePath/aFile.txt";

   dtUtil::FileUtils::GetInstance().MakeDirectory(testPath);

   FILE* fp = fopen(path1.c_str(), "w");

   CPPUNIT_ASSERT(fp != NULL);

   fputs("Text for my test file", fp);

   fclose(fp);

   dtUtil::FileUtils::GetInstance().FileCopy(path1, path2, true);

   //make sure we didn't blow the file away
   dtUtil::FileInfo fi = dtUtil::FileUtils::GetInstance().GetFileInfo(path1);

   CPPUNIT_ASSERT(fi.size > 0);

   //get rid of test file
   dtUtil::FileUtils::GetInstance().FileDelete(path1);
   //get rid of test directory
   dtUtil::FileUtils::GetInstance().DirDelete(testPath, false);
}

/*void FileUtilsTests::testAbsoluteToRelativePath()
{
   try
   {
      dtUtil::FileUtils &instance = dtUtil::FileUtils::GetInstance();

      std::string path;
      std::string mapXSDPath = dtUtil::FindFileInPathList("map.xsd");

      NormalizeDirectorySlashes(mapXSDPath);

      instance.AbsoluteToRelative(mapXSDPath, path);
      CPPUNIT_ASSERT_EQUAL(std::string("../../data/map.xsd"), path);
   }
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}*/

//////////////////////////////////////////////////////////////////////////
void FileUtilsTests::testDirectoryContentsWithOneFilter()
{
   const dtUtil::DirectoryContents allContents = dtUtil::FileUtils::GetInstance().DirGetFiles(GetTestsDir());
   const size_t numAllFiles = allContents.size();

   dtUtil::FileExtensionList extensions;
   extensions.push_back(".cpp");

   const dtUtil::DirectoryContents cppContents = dtUtil::FileUtils::GetInstance().DirGetFiles(GetTestsDir(), extensions);
   CPPUNIT_ASSERT_MESSAGE("DirGetFiles() with filtering didn't appear to filter on one extension.",
                          numAllFiles > cppContents.size());

   dtUtil::DirectoryContents::const_iterator itr = cppContents.begin();
   while (itr != cppContents.end())
   {
      CPPUNIT_ASSERT_MESSAGE("DirGetFiles() returned back a file that didn't match the supplied extension",
                             getFileExtensionIncludingDot((*itr)) == ".cpp");
      ++itr;
   }
}

//////////////////////////////////////////////////////////////////////////
void FileUtilsTests::testDirectoryContentsWithTwoFilters()
{
   const dtUtil::DirectoryContents allContents = dtUtil::FileUtils::GetInstance().DirGetFiles(GetTestsDir());

   dtUtil::FileExtensionList extensions;
   extensions.push_back(".cpp");
   const dtUtil::DirectoryContents cppContents = dtUtil::FileUtils::GetInstance().DirGetFiles(GetTestsDir(), extensions);

   extensions.push_back(".h");
   const dtUtil::DirectoryContents cppHContents = dtUtil::FileUtils::GetInstance().DirGetFiles(GetTestsDir(), extensions);

   //should be more cpp & h files than just cpp files
   CPPUNIT_ASSERT_MESSAGE("DirGetFiles() with filtering didn't appear to filter with two extensions.",
                           cppHContents.size() > cppContents.size());

   dtUtil::DirectoryContents::const_iterator itr = cppHContents.begin();
   while (itr != cppHContents.end())
   {
      CPPUNIT_ASSERT_MESSAGE("DirGetFiles() returned back a file that didn't match the supplied extensions",
         getFileExtensionIncludingDot((*itr)) == ".cpp" ||
         getFileExtensionIncludingDot((*itr)) == ".h");
      ++itr;
   }

}

//////////////////////////////////////////////////////////////////////////
void FileUtilsTests::testDirectoryContentsWithDuplicateFilter()
{
   dtUtil::FileExtensionList singleExtension;
   singleExtension.push_back(".cpp");
   const dtUtil::DirectoryContents singleFilterList = dtUtil::FileUtils::GetInstance().DirGetFiles(GetTestsDir(), singleExtension);

   dtUtil::FileExtensionList duplicateExtension;
   duplicateExtension.push_back(".cpp");
   duplicateExtension.push_back(".cpp"); //add another of the same

   const dtUtil::DirectoryContents duplicateFilter = dtUtil::FileUtils::GetInstance().DirGetFiles(GetTestsDir(), duplicateExtension);

   CPPUNIT_ASSERT_EQUAL_MESSAGE("DirGetFiles() should not count duplicate extensions",
                                 singleFilterList.size(), duplicateFilter.size());
}

void FileUtilsTests::testLoadFromArchive()
{
   std::string archivePath("./data/ProjectArchive.zip");

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   fileUtils.ChangeDirectory(GetTestsDir());

   dtUtil::FileInfo info = fileUtils.GetFileInfo(archivePath);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The file type for a zip file should be an archive", dtUtil::ARCHIVE, info.fileType);

   dtUtil::DirectoryContents zipContents = fileUtils.DirGetFiles(archivePath);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The zip should have 2 folders.", 2, int(zipContents.size()));

   fileUtils.ChangeDirectory(archivePath);
   fileUtils.ChangeDirectory("StaticMeshes");

   dtUtil::DirectoryContents meshContents = fileUtils.DirGetFiles(".");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The mesh directory should have 1 file.", 1, int(meshContents.size()));

   for(int i = 0; i < 10; ++i)
   {
      osg::Node* nodeFile = dtUtil::FileUtils::GetInstance().ReadNode(meshContents[0]);
      CPPUNIT_ASSERT_MESSAGE("Node should not be NULL", nodeFile != NULL);
   }

   fileUtils.ChangeDirectory(GetTestsDir());

}

void FileUtilsTests::testArchiveRelativePath()
{

   std::string archivePath("./data/ProjectArchive.zip");

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   fileUtils.ChangeDirectory(GetTestsDir());

   dtUtil::FileInfo info = fileUtils.GetFileInfo(archivePath);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The file type for a zip file should be an archive", dtUtil::ARCHIVE, info.fileType);

   osgDB::ArchiveExtended* a = fileUtils.FindArchive(archivePath);

   CPPUNIT_ASSERT(a != NULL);
   fileUtils.ChangeDirectory(GetTestsDir());

}

void FileUtilsTests::testIsSameFile()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   try
   {
      fileUtils.ChangeDirectory(GetExamplesDataDir());

      std::string filename("test_planes.osg");
      std::string relPath("./StaticMeshes/tests/" + filename);
      std::string absPath = fileUtils.GetAbsolutePath(relPath);

      CPPUNIT_ASSERT(fileUtils.FileExists(relPath));
      CPPUNIT_ASSERT( ! absPath.empty());
      CPPUNIT_ASSERT(fileUtils.FileExists(absPath));

      CPPUNIT_ASSERT( ! fileUtils.IsAbsolutePath(relPath));
      CPPUNIT_ASSERT(fileUtils.IsAbsolutePath(absPath));
      CPPUNIT_ASSERT(fileUtils.IsSameFile(absPath, relPath));
   }
   catch (dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   catch (...)
   {
      CPPUNIT_FAIL("Unknown exception thrown.");
   }
}

void FileUtilsTests::testIsSameFileInArchive()
{

   std::string archivePath("./data/ProjectArchive.zip");
   std::string pathToFile = GetTestsDir() + "/data/ProjectArchive.zip/StaticMeshes/articulation_test.ive";
   std::string filename("articulation_test.ive");


   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   try
   {
      fileUtils.ChangeDirectory(GetTestsDir());

      dtUtil::FileInfo info = fileUtils.GetFileInfo(archivePath);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The file type for a zip file should be an archive", dtUtil::ARCHIVE, info.fileType);

      dtUtil::DirectoryContents zipContents = fileUtils.DirGetFiles(archivePath);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The zip should have 2 folders.", 2, int(zipContents.size()));

      fileUtils.ChangeDirectory(archivePath + "/StaticMeshes");

      CPPUNIT_ASSERT_EQUAL_MESSAGE("Given a full path to the file and a relative, it should be able to compare them and find them to be the same",
                                    true, fileUtils.IsSameFile(pathToFile, filename));

      fileUtils.ChangeDirectory(GetTestsDir());
   }
   catch (dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
   catch (...)
   {
      CPPUNIT_FAIL("Unknown exception thrown.");
   }
}

void FileUtilsTests::testDirExistsInArchive()
{

   std::string archivePath("./data/ProjectArchive.zip");

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   fileUtils.ChangeDirectory(GetTestsDir());

   dtUtil::FileInfo info = fileUtils.GetFileInfo(archivePath);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The file type for a zip file should be an archive", dtUtil::ARCHIVE, info.fileType);

   dtUtil::DirectoryContents zipContents = fileUtils.DirGetFiles(archivePath);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The zip should have 2 folders.", 2, int(zipContents.size()));

   fileUtils.ChangeDirectory(archivePath);

   bool relativeTest = fileUtils.DirExists("StaticMeshes");
   bool absoluteTest = fileUtils.DirExists(GetTestsDir() + "/data/ProjectArchive.zip/StaticMeshes");

   CPPUNIT_ASSERT_EQUAL_MESSAGE("The directory in the archive should be found to exist when searching with a relative path", true, relativeTest);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The directory in the archive should be found to exist when searching with an absolute path", true, absoluteTest);

   fileUtils.ChangeDirectory(GetTestsDir());
}

void FileUtilsTests::testFileExistsInArchive()
{
   try
   {
      std::string archivePath("./data/ProjectArchive.zip");

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      fileUtils.ChangeDirectory(GetTestsDir() + "/data/ProjectArchive.zip/StaticMeshes");

      bool relativeTest = fileUtils.FileExists("articulation_test.ive");
      bool absoluteTest = fileUtils.FileExists(GetTestsDir() + "/data/ProjectArchive.zip/StaticMeshes/articulation_test.ive");

      CPPUNIT_ASSERT_EQUAL_MESSAGE("The static mesh in the archive should be found to exist when searching with a relative path", true, relativeTest);
      CPPUNIT_ASSERT_EQUAL_MESSAGE("The static mesh in the archive should be found to exist when searching with an absolute path", true, absoluteTest);

      fileUtils.ChangeDirectory(GetTestsDir());
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

void FileUtilsTests::testLoadFromPasswordProtectedArchive()
{
   std::string archivePath("./data/TestArchive_password_is_delta3d.zip");

   //for testing password on .zip
   osgDB::Registry* reg = osgDB::Registry::instance();
   if(reg != NULL)
   {
      osgDB::AuthenticationMap* auth = new osgDB::AuthenticationMap();
      auth->addAuthenticationDetails("ZipPlugin", new osgDB::AuthenticationDetails("ZipPlugin", "delta3d"));
      reg->setAuthenticationMap(auth);
   }

   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

   fileUtils.ChangeDirectory(GetTestsDir());

   dtUtil::FileInfo info = fileUtils.GetFileInfo(archivePath);
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The file type for a zip file should be an archive", dtUtil::ARCHIVE, info.fileType);

   fileUtils.ChangeDirectory(archivePath + "/StaticMeshes");

   dtUtil::DirectoryContents meshContents = fileUtils.DirGetFiles(".");
   CPPUNIT_ASSERT_EQUAL_MESSAGE("The mesh directory should have 1 file.", 1, int(meshContents.size()));

   osg::Node* nodeFile = dtUtil::FileUtils::GetInstance().ReadNode(meshContents[0]);
   CPPUNIT_ASSERT_MESSAGE("Node should not be NULL", nodeFile != NULL);


   fileUtils.ChangeDirectory(GetTestsDir());

}


void FileUtilsTests::testArchiveReadNodeFile()
{
   try
   {
      std::string archivePath("../data/ProjectArchive.zip");
      std::string originalFilePathList = dtUtil::GetDataFilePathList();
      std::string newFilePathList = originalFilePathList + ";" + archivePath;
      dtUtil::SetDataFilePathList(newFilePathList);

      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();

      std::string fileToFind("/StaticMeshes/articulation_test.ive");
      //this below should work and doesnt
      std::string fileFound = dtUtil::FindFileInPathList(fileToFind);
      osg::Node* node = osgDB::readNodeFile(archivePath + fileToFind);

      //this should work and is an osg bug, currently commented out
      //CPPUNIT_ASSERT_MESSAGE("Should be able to find a file within an archive using dtUtil::FindFileInPathList.", !fileFound.empty());

      CPPUNIT_ASSERT_MESSAGE("Should be able to call osgDB::readNodeFile() for a file within an archive.", node != NULL);

      fileUtils.ChangeDirectory(GetTestsDir());
   }
   catch (const dtUtil::Exception& ex)
   {
      CPPUNIT_FAIL(ex.ToString());
   }
}

