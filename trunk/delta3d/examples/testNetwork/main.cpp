/* -*-c++-*-
 * testNetwork - main (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2006-2008, MOVES Institute
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

// main.cpp : defines the entry point for the console application.

#include "testNetwork.h"
#include <gnelib.h>

#include <dtUtil/datapathutils.h>

using namespace dtCore;

///Supplying a host name as the first argument on the command line will create
///a client and try to connect to that server.
///No parameters on the command line will create a server.
int main(int argc, char* argv[])
{
   // set data search path to parent directory and delta3d/data
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaRootPath() + "/examples/data" + ";");

   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ALWAYS, "",
      "Usage: testNetwork.exe [hostname]");

   std::string hostName;

   if (argc > 1)
   {
      hostName = std::string(argv[1]);
   }

   dtCore::RefPtr<TestNetwork> app = new TestNetwork(hostName, "testnetworkconfig.xml");

   app->Config(); //configuring the application
   app->Run(); // running the simulation loop

   return 0;
}

