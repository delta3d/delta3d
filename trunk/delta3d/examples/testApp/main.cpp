
#include "dt.h"
#include "dtabc.h"


int main()
{
   dtABC::Application *app = new dtABC::Application("config.xml");

   //load something interesting
   dtCore::SetDataFilePathList("../../data/;../../../data/");
   dtCore::Object *obj = new dtCore::Object("MyObject");
   obj->LoadFile("ground.flt");
   app->AddDrawable(obj);

   //adjust the Camera position
   dtCore::Transform camPos;
   sgVec3 camXYZ={0.f, -50.f, 20.f};
   sgVec3 lookAtXYZ = {0.f, 0.f, 0.f};
   sgVec3 upVec = {0.f, 0.f, 1.f};
   camPos.SetLookAt(camXYZ, lookAtXYZ, upVec);
   app->GetCamera()->SetTransform(&camPos);

   app->GetWindow()->SetWindowTitle("Hit escape to exit");

   app->Config();

   app->Run();

	return 0;
}
