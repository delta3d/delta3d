/* -*-c++-*-
 * testNetwork - testphysics (.h & .cpp) - Using 'The MIT License'
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
#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtCore/object.h>
#include <dtCore/odebodywrap.h>
#include <dtCore/odecontroller.h>
#include <dtCore/globals.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtUtil/mathdefines.h>
#include <ode/ode.h>
#include <cassert>
#include <queue>
#include <osgGA/GUIEventAdapter>

using namespace dtCore;
using namespace dtABC;
using namespace dtUtil;

class Updater : public Base
{

public:
   Updater(Scene* scene)
      : mScene(scene)
   {
      assert(mScene.valid());
      AddSender(mScene.get());
   }
protected:
   virtual ~Updater()
   {
      RemoveSender(mScene.get());
   }

public:

   virtual void OnMessage(MessageData* data)
   {
      if (data->message == "collision")
      {

         //Scene::CollisionData* cd = static_cast< Scene::CollisionData* >( data->userData );

         //Log::GetInstance().LogMessage( Log::LOG_INFO, __FUNCTION__,
         //                               "Collision detected between %s and %s at (%f, %f, %f)",
         //                               dynamic_cast<Base*>(cd->mBodies[0])->GetName().c_str(),
         //                               dynamic_cast<Base*>(cd->mBodies[1])->GetName().c_str(),
         //                               cd->mLocation[0],
         //                               cd->mLocation[1],
         //                               cd->mLocation[2] );
         //
      }
   }
private:
   dtCore::RefPtr<Scene> mScene;
};

class TestPhysicsApp : public Application
{

public:

   TestPhysicsApp(const std::string& configFile = "config.xml")
      : Application(configFile)
      , mLimit(50)
   {
      //setup some default physics values
      GetScene()->SetGravity(0.0f, 0.0f, -15.0f);
      GetScene()->GetPhysicsController()->GetWorldWrapper()->SetDamping(0.01f, 0.01f);

      RefPtr<Object> obj1 = new Object("Ground");
      RefPtr<Object> obj2 = new Object("FallingCrate");
      RefPtr<Object> obj3 = new Object("GroundCrate");

      obj1->LoadFile("models/terrain_simple.ive");
      obj2->LoadFile("models/physics_crate.ive");
      obj3->LoadFile("models/physics_crate.ive");

      //position the camera
      Transform position;
      position.Set(0.0f, -20.0f, 7.0f, 0.0f, 0.0f, 0.0f);
      GetCamera()->SetTransform(position);

      //position first falling crate
      position.Set(0.55f, 0.0f, 7.0f, 0.0f, 40.0f, 0.0f);
      obj2->SetTransform(position);

      //position the crate on the ground
      position.Set(0.0f, 0.0f, 4.f, 0.0f, 0.0f, 0.0f);
      obj3->SetTransform(position);

      double lx = 1.0;
      double ly = 1.0;
      double lz = 1.0;

      //create collision meshes
      obj1->SetCollisionMesh();
      obj2->SetCollisionBox();
      obj3->SetCollisionBox();

      //set the mass for objects
      dMass mass;
      dMassSetBox(&mass, 1.0, lx, ly, lz);
      obj2->SetMass(&mass);
      obj3->SetMass(&mass);

      //turn on the physics
      obj2->EnableDynamics();
      obj3->EnableDynamics();

      //Add the objects to the Scene to be rendered
      GetScene()->AddDrawable(obj1.get());
      GetScene()->AddDrawable(obj2.get());
      GetScene()->AddDrawable(obj3.get());

      //put the falling crate in the vector of dropped objects
      mObjects.push_back(obj2);

      updater = new Updater(GetScene());

      omm = new OrbitMotionModel(GetKeyboard(), GetMouse());
      omm->SetTarget(GetCamera());

      //calculate and set focal distance for orbit motion model (origin -> camera)
      Transform trans;
      GetCamera()->GetTransform(trans);

      osg::Vec3 camLoc;
      trans.GetTranslation(camLoc);

      osg::Vec3 origin(0.0f, 0.0f, 0.0f);
      omm->SetDistance((camLoc - origin).length());

      CreateHelpLabel();
   }

protected:

   virtual ~TestPhysicsApp() {}

   virtual void PreFrame(const double deltaFrameTiime)
   {
      while (!mToAdd.empty())
      {
         GetScene()->AddDrawable(mToAdd.front().get());
         mObjects.push_back(mToAdd.front());
         mToAdd.pop();
      }

      while (!mToRemove.empty())
      {
         GetScene()->RemoveDrawable(mToRemove.front().get());
         mObjects.pop_front();
         mToRemove.pop();
      }
   }

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key)
   {
     // SetCollision____ must be called before setTransform so
     // that a dGeomID will exist before a prePhysicsUpdate, otherwise
     // the scale will never be applied to the physics geometry
      bool verdict(false);
      switch (key)
      {
         case 'p':
         {
            System::GetInstance().SetPause(!System::GetInstance().GetPause());
            verdict = true;
            break;
         }
         case osgGA::GUIEventAdapter::KEY_Escape:
         {
            while (!mToAdd.empty())
            {
               mToAdd.pop();
            }

            while (!mToRemove.empty())
            {
               mToRemove.pop();
            }

            while (!mObjects.empty())
            {
               mObjects.pop_front();
            }

            Quit();
            verdict = true;
            break;
         }
         case 'b':
            {
               if (mObjects.size() < mLimit)
               {
                  RefPtr<Object> box = new Object("box");
                  box->LoadFile("models/physics_crate.ive");

                  float randomScale = RandFloat(0.5f, 2.0f);
                  box->SetScale(osg::Vec3(randomScale, randomScale, randomScale));

                  box->SetCollisionBox();
                  box->SetTransform(GetStartTransform());
                  //box->RenderCollisionGeometry();

                  double lx = 1.0;
                  double ly = 1.0;
                  double lz = 1.0;

                  dMass mass;
                  dMassSetBox(&mass, 1.0, lx, ly, lz);
                  box->SetMass(&mass);

                  box->EnableDynamics();

                  mToAdd.push(box);
               }
               else
               {
                  mToRemove.push(mObjects.front());
               }
               verdict = true;
               break;
         }
         case 's':
         {
            if (mObjects.size() < mLimit)
            {
               RefPtr<Object> sphere = new Object("sphere");
               sphere->LoadFile("models/physics_happy_sphere.ive");

               float randomScale = RandFloat(0.5f, 2.0f);
               sphere->SetScale(osg::Vec3(randomScale, randomScale, randomScale));

               sphere->SetCollisionSphere();
               sphere->SetTransform(GetStartTransform());
               //sphere->RenderCollisionGeometry(true);

               double radius = 0.5;

               dMass mass;
               dMassSetSphere(&mass, 1.0, radius);
               sphere->SetMass(&mass);
               sphere->EnableDynamics();

               mToAdd.push(sphere);
            }
            else
            {
               mToRemove.push(mObjects.front());
            }
            verdict = true;
            break;
         }
         case 'c':
         {
            if (mObjects.size() < mLimit)
            {
               RefPtr<Object> cyl = new Object("cylinder");
               cyl->LoadFile("models/physics_barrel.ive");

               float randomScale = RandFloat(0.5f, 2.0f);
               cyl->SetScale(osg::Vec3(randomScale, randomScale, randomScale));

               cyl->SetCollisionCappedCylinder();
               cyl->SetTransform(GetStartTransform());
               //cyl->RenderCollisionGeometry(true);

               double radius = 0.321;
               double length = 1.0;

               dMass mass;
               dMassSetCappedCylinder(&mass, 1.0, 2, radius, length);
               cyl->SetMass(&mass);

               cyl->EnableDynamics();

               mToAdd.push(cyl);
            }
            else
            {
               mToRemove.push(mObjects.front());
            }
            verdict = true;
            break;
         }
         case osgGA::GUIEventAdapter::KEY_F1:
         {
            mLabel->SetActive(!mLabel->GetActive());
            break;
         }
         default:
         {
            break;
         }
      }

      return verdict;
   }

   //If a physical object is picked, apply a little spinning force to it
   virtual bool MouseButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      if (button != 0) { return false; }

      dtCore::DeltaDrawable* drawable = GetView()->GetMousePickedObject();
      if (drawable == NULL) { return false; } //nothing picked

      dtCore::Physical* phys = dynamic_cast<dtCore::Physical*>(drawable);
      if (phys == NULL) { return false; } //didn't pick a Physical

      if (phys->DynamicsEnabled() == false) { return false; } //not enabled

      phys->GetBodyWrapper()->ApplyForce(osg::Vec3(0.f, 0.f, 120.f));
      phys->GetBodyWrapper()->ApplyTorque(osg::Vec3(0.f, 0.f, 60.f));
      return true;
   }

   ///Use the mouse pick point to calculate the starting location
   Transform GetStartTransform()
   {
      osg::Vec3 xyz;
      if (GetView()->GetMousePickPosition(xyz))
      {
         Transform xform;
         xyz[2] += 1.f; //bump up the z so objects aren't buried underground
         xform.SetTranslation(xyz);
         xform.SetRotation(RandFloat(0.0f, 180.0f),
                           RandFloat(0.0f,  90.0f),
                           RandFloat(0.0f,  90.0f));

         return xform;
      }

      return Transform();
   }

   private:
      void CreateHelpLabel()
      {
         mLabel = new dtABC::LabelActor();
         osg::Vec2 testSize(24.0f, 5.5f);
         mLabel->SetBackSize(testSize);
         mLabel->SetFontSize(0.8f);
         mLabel->SetTextAlignment(dtABC::LabelActor::AlignmentEnum::LEFT_CENTER);
         mLabel->SetText(CreateHelpLabelText());
         mLabel->SetEnableDepthTesting(false);
         mLabel->SetEnableLighting(false);

         GetCamera()->AddChild(mLabel.get());
         dtCore::Transform labelOffset(-17.0f, 50.0f, 10.5f, 0.0f, 90.0f, 0.0f);
         mLabel->SetTransform(labelOffset, dtCore::Transformable::REL_CS);
         AddDrawable(GetCamera());
      }

      std::string CreateHelpLabelText()
      {
         std::string testString("");
         testString += "F1: Toggle Help Screen\n";
         testString += "\n";
         testString += "b: Add box at mouse position\n";
         testString += "c: Add cylinder at mouse position\n";
         testString += "s: Add sphere at mouse position\n";
         testString += "p: Pause System\n";

         return testString;
      }

   const unsigned int mLimit;

   std::queue< RefPtr<Object> > mToAdd;
   std::queue< RefPtr<Object> > mToRemove;
   std::deque< RefPtr<Object> > mObjects;

   RefPtr<Updater> updater;
   RefPtr<OrbitMotionModel> omm;
   dtCore::RefPtr<dtABC::LabelActor> mLabel;
};


int main(int argc, char** argv)
{
   dtCore::SetDataFilePathList( dtCore::GetDeltaRootPath() + "/examples/data" + ";");

   RefPtr<TestPhysicsApp> app = new TestPhysicsApp("config.xml");

   app->Config();
   app->Run();

   return 0;
}
