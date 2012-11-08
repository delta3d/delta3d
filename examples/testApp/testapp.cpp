/* -*-c++-*-
* testApp - testApp(.h & .cpp) - Using 'The MIT License'
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
#include <dtCore/camera.h>
#include <dtCore/object.h>
#include <dtCore/transform.h>
#include <dtABC/application.h>
#include <dtUtil/datapathutils.h>

int main()
{

   std::string dataPath = dtUtil::GetDeltaDataPathList();
   dtUtil::SetDataFilePathList(dataPath + ";" + 
      dtUtil::GetDeltaRootPath() + "/examples/data" + ";" +
      dtUtil::GetDeltaRootPath() + "/examples/testApp/;");

    dtCore::RefPtr<dtABC::Application> app = new dtABC::Application("config.xml");


   //load some terrain
   dtCore::RefPtr<dtCore::Object> terrain = new dtCore::Object("Terrain");
   terrain->LoadFile("models/terrain_simple.ive");
   app->AddDrawable(terrain.get());

   //load an object
   dtCore::RefPtr<dtCore::Object> brdm = new dtCore::Object("BRDM");
   brdm->LoadFile("models/brdm.ive");
   app->AddDrawable(brdm.get());
   osg::Vec3 brdmPosition(0.0f, 0.0f, 3.5f);
   osg::Vec3 brdmRotation(90.0f, 0.0f, 0.0f);
   dtCore::Transform trans;
   trans.SetTranslation(brdmPosition);
   trans.SetRotation(brdmRotation);
   brdm->SetTransform(trans);
   
   //adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ(0.f, -30.f, 15.f);
   osg::Vec3 lookAtXYZ(brdmPosition);
   osg::Vec3 upVec(0.f, 0.f, 1.f);
   camPos.Set(camXYZ, lookAtXYZ, upVec);
   app->GetCamera()->SetTransform(camPos);

   app->Config();
   app->Run();

   return 0;
}

/** \page testApp Application Tutorial
*  \dontinclude testApp.cpp
* This tutorial will decompose the testApp example, found in the Delta3D/Examples folder. It
* is meant as a very simple introduction to the dtABC::Application class.  When you run
* testApp.exe, this is what you should see:
* \image html testApp.jpg
* 
* The first thing we need to do is include some header files.  In this example, we'll include
* all the header files which are required for this example.  We could have also included the main
* headers dt.h and dtABC.h, which correspond to namespaces and subsequently, the libraries we need.
* <b>When you include these header files, the library files get automatically linked in</b>.  Since
* we're not using the main header files, we have to setup our project file to link in the additional libraries
* and their dependencies.
* \skip camera.h
* \until application.h
*
* Now we can set up our search paths to find the data files we'll use in the application.  In
* this case, we append a couple of strings together, separated by a semi-colon:
* \skip main
* \until );
*
* Now we can create a new dtABC::Application.  Notice we're storing it as a dtCore::RefPtr.  This
* will allow it to be delete automatrically when the variable goes out of scope.  We're also
* passing in the name of a config file which will allow us to change some aspects of this application.
* \skipline RefPtr

* Then we create a couple of dtCore::Object and give them files to load.  Don't forget to add it to the Scene,
* or else it won't get rendered. To do this, we can just use dtABC::Application::AddDrawable().
* \skipline "Terrain"
* \until brdm.get
*
* On the second dtCore::Object, lets set its position using the dtCore::Transform
* class.  In this case, the Transform just changes the heading  by 90 degrees.
* \until SetTransform
*
* Now we can set up the dtCore::Camera position by giving it a position and a place to look at.
* \until app->GetCamera()
* 
* And finally, tell the dtABC::Application to configure and to run.  The dtABC::Application::Run() method
* is blocking, meaning it won't return control back until the the Application has exited.  To quit the 
* app, just hit the escape key.
* \until return
*/
