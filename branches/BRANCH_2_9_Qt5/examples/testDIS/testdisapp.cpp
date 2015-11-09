/* -*-c++-*-
* Using 'The MIT License'
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

// Must be first because of a hawknl conflict with osg.  This is not a directly required include, but indirectly
#include <osgDB/Serializer>

#include "testdisapp.h"
#include "disactorcomponents.h"
#include <dtDIS/mastercomponent.h>
#include <dtDIS/sharedstate.h>
#include <dtGame/gamemanager.inl>
#include <dtGame/gamemanager.h>
#include <dtGame/defaultmessageprocessor.h>
#include <dtGame/defaultnetworkpublishingcomponent.h>
#include <dtUtil/datapathutils.h>
#include <dtUtil/coordinates.h>
#include <dtCore/project.h>
#include <dtCore/rtsmotionmodel.h>

////////////////////////////////////////////////////////////////////////////////
TestDISApp::TestDISApp(const std::string& connectionXml,
                       const std::string& actorTypeMappingXml)
: mMotion(NULL)
, mConnectionXml(connectionXml)
, mActorTypeMapping(actorTypeMappingXml)
{
}

////////////////////////////////////////////////////////////////////////////////
TestDISApp::~TestDISApp()
{
   mGameManager->RemoveComponent(*mDISComponent);
   mGameManager->RemoveComponent(*mMessageProc);
   mGameManager->RemoveComponent(*mNetworkingRouter);
   mMotion->SetTarget(NULL);
   mActorsToPublish.clear();
}

////////////////////////////////////////////////////////////////////////////////
void TestDISApp::Config()
{
   dtABC::Application::Config();

   mMotion = new dtCore::RTSMotionModel(GetKeyboard(), GetMouse());
   mMotion->SetTarget(GetCamera());

   dtDIS::SharedState* disConfig = NULL;

   disConfig = new dtDIS::SharedState(mConnectionXml, mActorTypeMapping);

   //Assuming that incoming positions are lat/lon and the local terrain is
   //centered around 0,0,0.  Note, this doesn't conform to DIS specs.
   disConfig->GetCoordinateConverter().SetIncomingCoordinateType(dtUtil::IncomingCoordinateType::GEODETIC);
   disConfig->GetCoordinateConverter().SetFlatEarthOrigin(osg::Vec2(0.0f, 0.0f));

   mDISComponent = new dtDIS::MasterComponent(disConfig);
   mMessageProc = new dtGame::DefaultMessageProcessor(); //need this to get messages routed around

   //need this to get messages send out to the DIS component, for sending to the network
   mNetworkingRouter = new dtGame::DefaultNetworkPublishingComponent(); 

   mGameManager = new dtGame::GameManager(*this->GetScene());

   //need to set a ProjectContext so the ResouceActorProperty can find the StaticMesh resources
   const std::string context = dtUtil::FindFileInPathList("examples/data");
   dtCore::Project::GetInstance().SetContext(context);
   mGameManager->ChangeMap("MyCoolMap");  //just for something to see

   mGameManager->AddComponent(*mDISComponent);
   mGameManager->AddComponent(*mMessageProc, dtGame::GameManager::ComponentPriority::HIGHEST);
   mGameManager->AddComponent(*mNetworkingRouter);
}


///a little class used to find a GameActor named "helo"
class FindDISActor
{
public:
   FindDISActor() {};
   ~FindDISActor() {};

   bool operator()(dtCore::BaseActorObject& actor)
   {
      if (actor.GetName().find("helo") != std::string::npos)
      {
         if (actor.IsGameActor())
         {
            return static_cast<dtGame::GameActorProxy&>(actor).IsPublished();
         }
         else return false;
      }
      else {return false;}
   }
};

////////////////////////////////////////////////////////////////////////////////
void TestDISApp::FindActorsAndAddComponents()
{
   FindDISActor findDisActor;
   mGameManager->FindActorsIf(findDisActor, mActorsToPublish);
   dtCore::ActorPtrVector::iterator itr = mActorsToPublish.begin();
   while (itr != mActorsToPublish.end())
   {
      EntityTypeActorComponent* entityTypeComp = new EntityTypeActorComponent(1,1,222,1,2,2);
      static_cast<dtGame::GameActorProxy*>((*itr))->AddComponent(*entityTypeComp);

      ++itr;
   }    
}

////////////////////////////////////////////////////////////////////////////////
void TestDISApp::PostFrame(const double deltaSimTime)
{
   static double kTimeToSend = 0.0;
   kTimeToSend += deltaSimTime;

   if (kTimeToSend > 2.0)
   {
      //find any published GameActors and add some new ActorComponents to them
      if (mActorsToPublish.empty()) { FindActorsAndAddComponents(); }

      dtCore::ActorPtrVector::iterator itr = mActorsToPublish.begin();
      while (itr != mActorsToPublish.end())
      {
         //Tell the GameActorProxy to send an ActorUpdateMessage with all of it's ActorProperties
         static_cast<dtGame::GameActorProxy*>((*itr))->NotifyFullActorUpdate();
         ++itr;
      }

      kTimeToSend = 0.0;
   }
}
