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
#include <prefix/dtgameprefix-src.h>
#include <cppunit/extensions/HelperMacros.h>
#include <LMS/WebPackager/package_utils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/exception.h>
#include <dtCore/globals.h>
#include <osgDB/FileNameUtils>

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
   #define W32
#endif

class LMSPackageBuilderTests : public CPPUNIT_NS::TestFixture
{
   CPPUNIT_TEST_SUITE(LMSPackageBuilderTests);
     CPPUNIT_TEST(TestGetFilePath);
     CPPUNIT_TEST(TestGetFileName);
     CPPUNIT_TEST(TestGetFileNameNoExt);
     CPPUNIT_TEST(TestGetFileExtension);
     CPPUNIT_TEST(TestSetFileExtension);
     CPPUNIT_TEST(TestCleanupFileString);
     CPPUNIT_TEST(TestIsAbsolutePath);
     CPPUNIT_TEST(TestRelativeToAbsolutePath);
     CPPUNIT_TEST(TestMakeDirectoryEX);
   CPPUNIT_TEST_SUITE_END();

public:

   void setUp();
   void tearDown();
   void TestGetFilePath();
   void TestGetFileName();
   void TestGetFileNameNoExt();
   void TestGetFileExtension();
   void TestSetFileExtension();
   void TestCleanupFileString();
   void TestIsAbsolutePath();
   void TestRelativeToAbsolutePath();
   void TestMakeDirectoryEX();

   void NormalizeDirectorySlashes(std::string &str)
   {
      #ifdef W32
      for(unsigned int i = 0; i < str.size(); i++)
      {
         
         if(str[i] == '"\"' || str[i] == '\\')
            str[i] = '/';
      }
      #endif
   }
};

CPPUNIT_TEST_SUITE_REGISTRATION(LMSPackageBuilderTests);

void LMSPackageBuilderTests::setUp()
{
   dtCore::SetDataFilePathList(dtCore::GetDeltaDataPathList());
}

void LMSPackageBuilderTests::tearDown()
{

}

void LMSPackageBuilderTests::TestGetFilePath()
{
   CPPUNIT_ASSERT_MESSAGE("GetFilePath should return empty string", GetFilePath("").empty());
   
   const std::string &fileToUse = "map.xsd";

   std::string absFilePath = dtCore::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absFilePath.empty());

   std::string result = GetFilePath(absFilePath);
   CPPUNIT_ASSERT(!result.empty());

   std::string filePath = absFilePath.substr(0, absFilePath.length() - (fileToUse.length() + 1));
   NormalizeDirectorySlashes(filePath);
   CPPUNIT_ASSERT_MESSAGE("The file path should be correct", result == filePath);
}

void LMSPackageBuilderTests::TestGetFileName()
{
   CPPUNIT_ASSERT_MESSAGE("GetFileName should return empty string", GetFileName("").empty());

   const std::string &fileToUse = "map.xsd";

   std::string absFilePath = dtCore::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absFilePath.empty());

   std::string result = GetFileName(absFilePath);
   CPPUNIT_ASSERT_MESSAGE("GetFileName should be correct", result == fileToUse);
}

void LMSPackageBuilderTests::TestGetFileNameNoExt()
{
   CPPUNIT_ASSERT_MESSAGE("GetFileName should return empty string", GetFileNameNoExt("").empty());

   const std::string &fileToUse = "map.xsd";
   std::string noExtName = osgDB::getStrippedName(fileToUse);

   std::string result = GetFileNameNoExt(fileToUse);
   CPPUNIT_ASSERT_MESSAGE("GetFileNameNoExt should be correct", result == noExtName);
}

void LMSPackageBuilderTests::TestGetFileExtension()
{  
   CPPUNIT_ASSERT_MESSAGE("GetFileName should return empty string", GetFileExtension("").empty());

   const std::string &fileToUse = "map.xsd";
   std::string ext = osgDB::getFileExtension(fileToUse);

   std::string result = GetFileExtension(fileToUse);
   CPPUNIT_ASSERT_MESSAGE("GetFileExtension should be correct", result == ext);
}

void LMSPackageBuilderTests::TestSetFileExtension()
{
   std::string emptyString;
   SetFileExtension(emptyString, "wav");
   CPPUNIT_ASSERT(!emptyString.empty());
   CPPUNIT_ASSERT_MESSAGE("Empty string should have the right extension", emptyString == ".wav");

   std::string band = "Amaranthine";
   SetFileExtension(band, "rules");
   CPPUNIT_ASSERT(band == "Amaranthine.rules");
   SetFileExtension(band, "isBetterThanYourBand");
   CPPUNIT_ASSERT_MESSAGE("SetFileExtension should have replaced the current extension", band == "Amaranthine.isBetterThanYourBand");
}

void LMSPackageBuilderTests::TestCleanupFileString()
{
   std::string emptyString;
   CleanupFileString(emptyString);
   CPPUNIT_ASSERT_MESSAGE("CleanupFileString should not break on an empty string", emptyString.empty());

   std::string path = "C:/Documents and Settings/some file/";
   CleanupFileString(path);
   CPPUNIT_ASSERT_MESSAGE("CleanupFileString should be correct", path == "C:/Documents and Settings/some file");

   path = "//";
   CleanupFileString(path);
   CPPUNIT_ASSERT(path == "/");
   CleanupFileString(path);
   CPPUNIT_ASSERT(path.empty());
}

void LMSPackageBuilderTests::TestIsAbsolutePath()
{
   CPPUNIT_ASSERT_MESSAGE("IsAbsolutePath should return false for empty string", !IsAbsolutePath(""));

   bool returnValue = false;
   const std::string &fileToUse = "map.xsd";
   std::string absPath = dtCore::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absPath.empty());

   returnValue = IsAbsolutePath(absPath);
   CPPUNIT_ASSERT_MESSAGE("IsAbsolutePath should return true", returnValue);

   std::string filePath = osgDB::getFilePath(absPath);
   returnValue = IsAbsolutePath(filePath);
   CPPUNIT_ASSERT_MESSAGE("IsAbsoluePath should return true", returnValue);

   returnValue = IsAbsolutePath(fileToUse);
   CPPUNIT_ASSERT(!returnValue);

   filePath += "/";
   returnValue = IsAbsolutePath(filePath);
   CPPUNIT_ASSERT(returnValue);

   filePath += "Paris Hilton made the worst cd ever. Not that I've heard it.";
   returnValue = IsAbsolutePath(filePath);
   CPPUNIT_ASSERT_MESSAGE("IsAbsolutePath should return false", !returnValue);
}

void LMSPackageBuilderTests::TestRelativeToAbsolutePath()
{
   const std::string &fileToUse = "map.xsd";
   std::string absPath = dtCore::FindFileInPathList(fileToUse);
   CPPUNIT_ASSERT(!absPath.empty());
   std::string relativePath = "../" + fileToUse;
   // The function uses the current working directory as the parent if the 
   // parent string is empty. Test that first
   std::string result = RelativeToAbsolutePath(relativePath, "");
}

void LMSPackageBuilderTests::TestMakeDirectoryEX()
{
   try
   {
      dtUtil::FileUtils &fileUtils = dtUtil::FileUtils::GetInstance();
      MakeDirectoryEX("");
      CPPUNIT_ASSERT_MESSAGE("MakeDirectoryEX should fail on empty string", !fileUtils.DirExists(""));

      std::string deltaRoot = dtCore::GetDeltaRootPath();
      std::string dirName = deltaRoot + "/tests/dtLMS/Amaranthine";
      MakeDirectoryEX(dirName);
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
   catch(const dtUtil::Exception &e)
   {
      CPPUNIT_FAIL(e.What());
   }
}
