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
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtUtil/mathdefines.h>
#include <dtUtil/datapathutils.h>
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
      , mGround(NULL)
      , mRenderCollisionGeometry(false)
   {
      // setup some default physics values
      GetScene()->SetGravity(0.0f, 0.0f, -15.0f);
      GetScene()->GetPhysicsController()->GetWorldWrapper()->SetDamping(0.01f, 0.01f);

      // position the camera
      Transform position;
      position.Set(0.0f, -20.0f, 7.0f, 0.0f, 0.0f, 0.0f);
      GetCamera()->SetTransform(position);

      double lx = 1.0;
      double ly = 1.0;
      double lz = 1.0;

      // configure the mass for the crates
      dMass mass;
      dMassSetBox(&mass, 1.0, lx, ly, lz);

      // load ground
      {
         mGround = new Object("Ground");
         mGround->LoadFile("StaticMeshes/terrain_simple.ive");

         // create collision mesh
         mGround->SetCollisionMesh();

         // Add the object to the Scene to be rendered
         GetScene()->AddChild(mGround.get());

         // render collision geometry (for debugging)
         mGround->RenderCollisionGeometry(mRenderCollisionGeometry);
      }

      // load falling crate
      {
         RefPtr<Object> obj2 = new Object("FallingCrate");
         obj2->LoadFile("StaticMeshes/physics_crate.ive");

         // position first falling crate
         position.Set(0.55f, 0.0f, 7.0f, 0.0f, 40.0f, 0.0f);
         obj2->SetTransform(position);

         // create collision mesh
         obj2->SetCollisionBox();

         // set the mass
         obj2->SetMass(&mass);

         // turn on the physics
         obj2->EnableDynamics();

         // Add the object to the Scene to be rendered
         GetScene()->AddChild(obj2.get());

         // put the falling crate in the vector of dropped objects
         mObjects.push_back(obj2);

         // render collision geometry
         obj2->RenderCollisionGeometry(mRenderCollisionGeometry);
      }

      // load ground crate
      {
         RefPtr<Object> obj3 = new Object("GroundCrate");
         obj3->LoadFile("StaticMeshes/physics_crate.ive");

         // position the crate on the ground
         position.Set(0.0f, 0.0f, 4.0f, 0.0f, 0.0f, 0.0f);
         obj3->SetTransform(position);

         // create collision mesh
         obj3->SetCollisionBox();

         // set the mass
         obj3->SetMass(&mass);

         // turn on the physics
         obj3->EnableDynamics();

         // Add the object to the Scene to be rendered
         GetScene()->AddChild(obj3.get());

         // put the ground crate in the vector of dropped objects
         mObjects.push_back(obj3);

         // render collision geometry
         obj3->RenderCollisionGeometry(mRenderCollisionGeometry);
      }

      updater = new Updater(GetScene());

      omm = new OrbitMotionModel(GetKeyboard(), GetMouse());
      omm->SetTarget(GetCamera());

      // calculate and set focal distance for orbit motion model (origin -> camera)
      {
         Transform trans;
         GetCamera()->GetTransform(trans);

         osg::Vec3 camLoc;
         trans.GetTranslation(camLoc);

         osg::Vec3 origin(0.0f, 0.0f, 0.0f);
         omm->SetDistance((camLoc - origin).length());
      }

      CreateHelpLabel();
   }

protected:
   virtual ~TestPhysicsApp() {}

   virtual void PreFrame(const double deltaFrameTiime)
   {
      while (!mToAdd.empty())
      {
         GetScene()->AddChild(mToAdd.front().get());
         mObjects.push_back(mToAdd.front());
         mToAdd.pop();
      }

      while (!mToRemove.empty())
      {
         GetScene()->RemoveChild(mToRemove.front().get());
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
         System::GetInstance().SetPause(!System::GetInstance().GetPause());
         verdict = true;
         break;

      case osgGA::GUIEventAdapter::KEY_Escape:
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

      case 'b':
         if (mObjects.size() < mLimit)
         {
            RefPtr<Object> box = new Object("box");
            box->LoadFile("StaticMeshes/physics_crate.ive");

            float randomScale = RandFloat(0.5f, 2.0f);
            box->SetScale(osg::Vec3(randomScale, randomScale, randomScale));

            box->SetCollisionBox();
            box->SetTransform(GetStartTransform());
            box->RenderCollisionGeometry(mRenderCollisionGeometry);

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

      case 's':
         if (mObjects.size() < mLimit)
         {
            RefPtr<Object> sphere = new Object("sphere");
            sphere->LoadFile("StaticMeshes/physics_happy_sphere.ive");

            float randomScale = RandFloat(0.5f, 2.0f);
            sphere->SetScale(osg::Vec3(randomScale, randomScale, randomScale));

            sphere->SetCollisionSphere();
            sphere->SetTransform(GetStartTransform());
            sphere->RenderCollisionGeometry(mRenderCollisionGeometry);

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

      case 'c':
         if (mObjects.size() < mLimit)
         {
            RefPtr<Object> cyl = new Object("cylinder");
            cyl->LoadFile("StaticMeshes/physics_barrel.ive");

            float randomScale = RandFloat(0.5f, 2.0f);
            cyl->SetScale(osg::Vec3(randomScale, randomScale, randomScale));

            cyl->SetCollisionCappedCylinder();
            cyl->SetTransform(GetStartTransform());
            cyl->RenderCollisionGeometry(mRenderCollisionGeometry);

            double radius = 0.321;
            double length = 1.0;

            dMass mass;
            dMassSetCapsule(&mass, 1.0, 2, radius, length);
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

      case 'd':
         // toggle drawing of all collision geometry
         mRenderCollisionGeometry = !mRenderCollisionGeometry;
         mGround->RenderCollisionGeometry(mRenderCollisionGeometry);
         for (size_t i = 0; i < mObjects.size(); ++i)
         {
            mObjects[i]->RenderCollisionGeometry(mRenderCollisionGeometry);
         }
         break;

      case osgGA::GUIEventAdapter::KEY_F1:
         mLabel->SetActive(!mLabel->GetActive());
         break;

      default:
         break;
      }

      return verdict;
   }

   // If a physical object is picked, apply a little spinning force to it
   virtual bool MouseButtonPressed(const dtCore::Mouse* mouse, dtCore::Mouse::MouseButton button)
   {
      if (button != 0) { return false; }

      dtCore::DeltaDrawable* drawable = GetView()->GetMousePickedObject();
      if (drawable == NULL) { return false; } // nothing picked

      dtCore::Physical* phys = dynamic_cast<dtCore::Physical*>(drawable);
      if (phys == NULL) { return false; } // didn't pick a Physical

      if (phys->DynamicsEnabled() == false) { return false; } // not enabled

      phys->GetBodyWrapper()->ApplyForce(osg::Vec3(0.0f, 0.0f, 120.0f));
      phys->GetBodyWrapper()->ApplyTorque(osg::Vec3(0.0f, 0.0f, 60.0f));
      return true;
   }

   ///Use the mouse pick point to calculate the starting location
   Transform GetStartTransform()
   {
      osg::Vec3 xyz;
      if (GetView()->GetMousePickPosition(xyz))
      {
         Transform xform;
         xyz[2] += 1.f; // bump up the z so objects aren't buried underground
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
   RefPtr<Object> mGround;
   bool mRenderCollisionGeometry;
};


int main(int argc, char** argv)
{
   dtUtil::SetDataFilePathList(dtUtil::GetDeltaRootPath() + "/examples/data" + ";");

   RefPtr<TestPhysicsApp> app = new TestPhysicsApp("config.xml");

   app->Config();
   app->Run();

   return 0;
}
