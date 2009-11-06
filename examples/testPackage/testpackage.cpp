/* -*-c++-*-
 * testPackage - testpackage (.h & .cpp) - Using 'The MIT License'
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
 * Jeff Houde
 */

/// Demo application illustrating map loading and actor manipulation
#include <dtCore/globals.h>
#include <dtUtil/packager.h>

#include <iostream>

using namespace dtCore;

class TestPackage
{
public:
   TestPackage(const std::string& dir)
   : mDir(dir)
   {
   }

   void run()
   {
      // Create a new package with some test files.
      dtUtil::Packager packager;
      packager.AddFile(mDir + "file1.bmp", "");
      packager.AddFile(mDir + "file2.txt", "testDir/subDir");

      packager.PackPackage(mDir + "testPackage", true);

      // Now test the remove file functionality.
      packager.RemoveFile("testDir/subDir/nonexistant.txt");
      packager.RemoveFile("file1.bmp");
      packager.RemoveFile("testDir/subDir/file2.txt");

      packager.PackPackage(mDir + "testEmptyPack", true);

      // Now unpack the package we just made.
      packager.UnpackPackage(mDir + "testPackage", mDir + "unpacked1");

      // Now test appending data to a package.
      packager.OpenPackage(mDir + "testPackage");
      packager.AddFile(mDir + "file3.lnk", "testDir");
      packager.PackPackage(mDir + "testPackage", true);

      // Now unpack the appended package.
      packager.UnpackPackage(mDir + "testPackage", mDir + "unpacked2");
   }

protected:

   virtual ~TestPackage()
   {
   }

private:

   std::string mDir;
};

int main()
{
   std::string dataPath = dtCore::GetDeltaDataPathList();
   dtCore::SetDataFilePathList(dataPath + ";" +
                               dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   TestPackage* testPackage = new TestPackage(dtCore::GetDeltaRootPath() + "/examples/data/packager/");
   testPackage->run();

   return 0;
}
