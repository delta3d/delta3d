/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: Jeff P. Houde
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

      director->CreateDebugScript();

      dtDAL::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/examples/data/director");
      director->SaveScript("test");
      director->LoadScript("test");

      director->GetGraphData()->GetEventNodes()[0]->Trigger(0);

      director->Update(0, 0);
      director->Update(0, 0);

      int resultValue = director->GetGraphData()->mSubGraphs[0]->mValueNodes[4]->GetInt();
      int extValue    = director->GetGraphData()->mSubGraphs[0]->mValueNodes[0]->GetInt();
      int outsideValue= director->GetGraphData()->mValueNodes[0]->GetInt();
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
