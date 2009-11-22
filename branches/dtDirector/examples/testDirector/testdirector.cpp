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

#include <dtDirector/nodemanager.h>
#include <dtDirector/actionnode.h>
#include <dtDirector/eventnode.h>
#include <dtDirector/valuenode.h>

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
       dtDirector::NodeManager& nodeManager = dtDirector::NodeManager::GetInstance();

       std::vector<const dtDirector::NodeType*> nodeTypes;
       nodeManager.GetNodeTypes(nodeTypes);

       for (int i = 0; i < (int)nodeTypes.size(); i++)
       {
       }

       dtCore::RefPtr<dtDirector::EventNode> eventNode =
           dynamic_cast<dtDirector::EventNode*>(nodeManager.CreateNode("Named Event", "General").get());

       dtCore::RefPtr<dtDirector::ActionNode> actionNode = 
           dynamic_cast<dtDirector::ActionNode*>(nodeManager.CreateNode("Binary Operation", "General").get());

       dtCore::RefPtr<dtDirector::ValueNode> firstValNode =
           dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());
       dtCore::RefPtr<dtDirector::ValueNode> secondValNode =
           dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());
       dtCore::RefPtr<dtDirector::ValueNode> firstResultNode =
           dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());
       dtCore::RefPtr<dtDirector::ValueNode> secondResultNode =
           dynamic_cast<dtDirector::ValueNode*>(nodeManager.CreateNode("Int", "General").get());

       if (eventNode.valid() && actionNode.valid() &&
           firstValNode.valid() && secondValNode.valid() &&
           firstResultNode.valid() && secondResultNode.valid())
       {
           firstValNode->SetPropertyValue(10);
           secondValNode->SetPropertyValue(15);

           // Connect all the nodes together.
           eventNode->GetOutputs()[0].Connect(&actionNode->GetInputs()[2]);

           actionNode->GetValues()[0].Connect(firstValNode.get());
           actionNode->GetValues()[1].Connect(secondValNode.get());
           actionNode->GetValues()[2].Connect(firstResultNode.get());
           actionNode->GetValues()[2].Connect(secondResultNode.get());

           // Trigger the event.
           eventNode->Trigger(0);

           // Now update stuff.
           eventNode->Update(0, 0);
           actionNode->Update(0, 0);

           int firstResult = firstResultNode->GetPropertyValue<int>();
           int secondResult = secondResultNode->GetPropertyValue<int>();
       }
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
