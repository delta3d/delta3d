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
 * William E. Johnson II
 */

#include <prefix/unittestprefix.h>
#include <cppunit/extensions/HelperMacros.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>
#include <dtUtil/mswinmacros.h>
#include <dtUtil/datapathutils.h>
#include <osgDB/FileNameUtils>


class LMSPackageBuilderTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(LMSPackageBuilderTests);
     CPPUNIT_TEST(TestGetFilePath);
     CPPUNIT_TEST(TestGetFileName);
     CPPUNIT_TEST(TestGetFileNameNoExt);
     CPPUNIT_TEST(TestGetFileExtension);
     CPPUNIT_TEST(TestCleanupFileString);
     CPPUNIT_TEST(TestIsAbsolutePath);
     CPPUNIT_TEST(TestMakeDirectoryEX);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestGetFilePath();
   void TestGetFileName();
   void TestGetFileNameNoExt();
   void TestGetFileExtension();
   void TestCleanupFileString();
   void TestIsAbsolutePath();
   void TestMakeDirectoryEX();
};

CPPUNIT_TEST_SUITE_REGISTRATION(LMSPackageBuilderTests);

void LMSPackageBuilderTests::setUp()
{
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaDataPathList());
}

void LMSPackageBuilderTests::tearDown()
{

}

void LMSPackageBuilderTests::TestGetFilePath()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   CPPUNIT_ASSERT_MESSAGE("GetFilePath should return empty string", fileUtils.GetFileInfo("").path.empty());

   const std::string& fileToUse = "map.xsd";

   std::string absFilePath = dtUtil::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absFilePath.empty());

   std::string result = fileUtils.GetFileInfo(absFilePath).path;
   CPPUNIT_ASSERT(!result.empty());

   std::string filePath = osgDB::convertFileNameToUnixStyle(absFilePath.substr(0, absFilePath.length() - (fileToUse.length() + 1)));
   CPPUNIT_ASSERT_MESSAGE("The file path should be correct", result == filePath);
}

void LMSPackageBuilderTests::TestGetFileName()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   CPPUNIT_ASSERT_MESSAGE("GetFileName should return empty string", fileUtils.GetFileInfo("").fileName.empty());

   const std::string& fileToUse = "map.xsd";

   std::string absFilePath = dtUtil::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absFilePath.empty());

   std::string result = fileUtils.GetFileInfo(absFilePath).baseName;
   CPPUNIT_ASSERT_MESSAGE("GetFileName should be correct", result == fileToUse);
}

void LMSPackageBuilderTests::TestGetFileNameNoExt()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   CPPUNIT_ASSERT_MESSAGE("GetFileName should return empty string", fileUtils.GetFileInfo("").baseName.empty());

   const std::string& fileToUse = "map.xsd";

   std::string absFilePath = dtUtil::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absFilePath.empty());

   std::string noExtName = osgDB::getStrippedName(fileToUse);

   std::string result = fileUtils.GetFileInfo(absFilePath).extensionlessFileName;
   CPPUNIT_ASSERT_MESSAGE("GetFileNameNoExt should be correct", result == noExtName);
}

void LMSPackageBuilderTests::TestGetFileExtension()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   CPPUNIT_ASSERT_MESSAGE("GetFileName should return empty string", fileUtils.GetFileInfo("").extension.empty());

   const std::string& fileToUse = "map.xsd";

   std::string absFilePath = dtUtil::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absFilePath.empty());

   std::string ext = osgDB::getFileExtension(fileToUse);

   std::string result = fileUtils.GetFileInfo(absFilePath).extension;
   CPPUNIT_ASSERT_MESSAGE("GetFileExtension should be correct", result == ext);
}

void LMSPackageBuilderTests::TestCleanupFileString()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   std::string emptyString;
   fileUtils.CleanupFileString(emptyString);
   CPPUNIT_ASSERT_MESSAGE("CleanupFileString should not break on an empty string", emptyString.empty());

   std::string path = "C:/Documents and Settings/some file/";
   fileUtils.CleanupFileString(path);
   CPPUNIT_ASSERT_MESSAGE("CleanupFileString should be correct", path == "C:/Documents and Settings/some file");

   path = "//";
   fileUtils.CleanupFileString(path);
   CPPUNIT_ASSERT(path.empty());
}

void LMSPackageBuilderTests::TestIsAbsolutePath()
{
   dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
   CPPUNIT_ASSERT_MESSAGE("IsAbsolutePath should return false for empty string", !fileUtils.IsAbsolutePath(""));

   bool returnValue = false;
   const std::string& fileToUse = "map.xsd";
   std::string absPath = dtUtil::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absPath.empty());

   returnValue = fileUtils.IsAbsolutePath(absPath);
   CPPUNIT_ASSERT_MESSAGE("IsAbsolutePath should return true", returnValue);

   std::string filePath = osgDB::getFilePath(absPath);
   returnValue = fileUtils.IsAbsolutePath(filePath);
   CPPUNIT_ASSERT_MESSAGE("IsAbsoluePath should return true", returnValue);

   returnValue = fileUtils.IsAbsolutePath(fileToUse);
   CPPUNIT_ASSERT(!returnValue);

   filePath += "/";
   returnValue = fileUtils.IsAbsolutePath(filePath);
   CPPUNIT_ASSERT(returnValue);
}

void LMSPackageBuilderTests::TestMakeDirectoryEX()
{
   try
   {
      dtUtil::FileUtils& fileUtils = dtUtil::FileUtils::GetInstance();
      fileUtils.MakeDirectoryEX("");
      CPPUNIT_ASSERT_MESSAGE("MakeDirectoryEX should fail on empty string", !fileUtils.DirExists(""));

      std::string deltaRoot = dtUtil::GetDeltaRootPath();
      std::string dirName = deltaRoot + "/tests/dtLMS/Amaranthine";
      fileUtils.MakeDirectoryEX(dirName);
      CPPUNIT_ASSERT_MESSAGE("The directory should exist", fileUtils.DirExists(dirName));

      CPPUNIT_ASSERT(fileUtils.DirDelete(dirName, false));

      /*std::string subDirName = "Amaranthine/Judd";
      MakeDirectoryEX(subDirName);
      CPPUNIT_ASSERT(fileUtils.DirExists(dirName));
      fileUtils.PushDirectory(dirName);
      CPPUNIT_ASSERT_MESSAGE("MakeDirectory should have created the sub directory", fileUtils.DirExists("Judd"));
      fileUtils.PopDirectory();
      fileUtils.DirDelete(dirName, true);*/
   }
   catch(const dtUtil::Exception& e)
   {
      CPPUNIT_FAIL(e.What());
   }
}
