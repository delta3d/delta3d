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
 * Jeff P. Houde
 */

/// Demo application illustrating the use of dtDirector.
#include <dtUtil/packager.h>
#include <dtUtil/datapathutils.h>

#include <dtDirector/director.h>

#include <dtDAL/project.h>

#include <iostream>

class TestDirector
{
public:
   TestDirector(const std::string& dir)
      : mDir(dir)
   {
   }

   void run()
   {
      dtCore::RefPtr<dtDirector::Director> director = new dtDirector::Director();
      director->Init();

      dtDAL::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data/director");
      director->SaveScript("test");
      director->LoadScript("test", NULL);

      director->GetEventNodes()[0]->Trigger(0);

      director->Update(0, 0);
      director->Update(0, 0);
      director->Update(0, 0);
      director->Update(0, 0);

      int firstResult = director->GetValueNodes()[2]->GetPropertyValue<int>();
      int secondResult = director->GetValueNodes()[3]->GetPropertyValue<int>();
      int thirdResult = director->GetValueNodes()[4]->GetPropertyValue<int>();
   }

protected:

   virtual ~TestDirector()
   {
   }

private:

   std::string mDir;
};

int main()
{
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" +
                               dtUtil::GetDeltaRootPath() + "/examples/data" + ";");

   TestDirector* testDirector = new TestDirector(dtUtil::GetDeltaRootPath() + "/examples/data/director/");
   testDirector->run();

   return 0;
}
