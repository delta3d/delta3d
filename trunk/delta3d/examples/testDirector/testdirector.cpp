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

      dtDAL::Project::GetInstance().SetContext(dtUtil::GetDeltaRootPath() + "/tests/data/ProjectContext");
      director->LoadScript("test");

      std::vector<dtDirector::Node*> nodes;
      director->GetNodes("Remote Event", "Core", nodes);
      int count = (int)nodes.size();
      for (int index = 0; index < count; index++)
      {
         dtDirector::EventNode* event = dynamic_cast<dtDirector::EventNode*>(nodes[index]);
         if (event)
         {
            dtDAL::ActorProperty* prop = event->GetProperty("EventName");
            if (prop && prop->ToString() == "First")
            {
               event->Trigger();
               break;
            }
         }
      }

      while (director->IsRunning())
      {
         director->Update(0.5f, 0.5f);
      }

      dtDirector::ValueNode* result = director->GetValueNode("Result");
      float resultValue = result->GetDouble();
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
