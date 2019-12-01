/* -*-c++-*-
 * testAI - main (.h & .cpp) - Using 'The MIT License'
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
 */
#include <fstream>
#include <iostream>
#include "testai.h"

#include <dtUtil/datapathutils.h>

// we attempt to take a map name from the command line argument
// if none is present we call the default constructor which has a default map name
int main (int argc, char* argv[])
{
   dtUtil::RemovePSNCommandLineOption(argc, argv);

   // set data search path to parent delta3d/data
   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" + 
      dtUtil::GetDeltaRootPath() + "/examples/data/;");

   dtCore::RefPtr<TestAI> app;

   if (argc < 2)
   {
      app = new TestAI();
   }
   else 
   {
      std::string mapName(argv[1]);
      app = new TestAI(mapName);
   }

   app->Config(); // configuring the application
   app->Run(); // running the simulation loop
}
