/* -*-c++-*-
 * testQt  - Using 'The MIT License'
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
#include "App.h"

#include <dtCore/deltawin.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <QtOpenGL/QGLWidget>
#include <dtQt/osggraphicswindowqt.h>

using namespace dtCore;
using namespace dtABC;

App::App(const std::string& configFilename)
: Application(configFilename)
{
}

//////////////////////////////////////////////////////////////////////////
App::~App()
{
   //The widget's about to be destroyed so we need to tell the GraphicsWindow it's gone (to avoid crash on exit)
   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(GetWindow()->GetOsgViewerGraphicsWindow());
   if (osgGraphWindow)
   {
      osgGraphWindow->SetQGLWidget(NULL);
   }
}

//////////////////////////////////////////////////////////////////////////
void App::Config()
{
   Application::Config();

   connect(&mMainWindow, SIGNAL(LoadFile(const QString&)), this, SLOT(OnLoadFile(const QString&)));

   connect(this, SIGNAL(FileLoaded(bool)), &mMainWindow, SLOT(OnFileLoaded(bool)));

   mCameraMotion = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
   mCameraMotion->SetTarget(GetCamera());
   mCameraMotion->SetFocalPoint(osg::Vec3(0.0f, 0.0f, 0.0f));

   mObject1 = new dtCore::Object("Object1");
   AddDrawable(mObject1.get());

   //hook up the Qt widgeting stuff
   dtQt::OSGGraphicsWindowQt* osgGraphWindow = dynamic_cast<dtQt::OSGGraphicsWindowQt*>(GetWindow()->GetOsgViewerGraphicsWindow());
   mMainWindow.SetGraphicsWidget(osgGraphWindow->GetQGLWidget());
   mMainWindow.show();
}

////////////////////////////////////////////////////////////////////////////////
void App::OnLoadFile(const QString& filename)
{
   bool valid = mObject1->LoadFile(filename.toStdString());
   emit FileLoaded(valid);
}
