/* -*-c++-*-
* testprerender - testprerender (.h & .cpp) - Using 'The MIT License'
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
* Michael Guerrero
*/
#ifndef DELTA_TEST_PRERENDER
#define DELTA_TEST_PRERENDER

#include <dtABC/application.h>
#include <dtCore/object.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/refptr.h>
#include <osg/Texture2D>

////////////////////////////////////////////////////////////////////////////////

class TestPreRender : public dtABC::Application
{
public:

   TestPreRender(const std::string& mainSceneObjectName,
                 const std::string& configFilename = "config.xml");

protected:

   virtual ~TestPreRender();

public:

   void LoadGeometry(const std::string& customObjectName);

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

   virtual void PreFrame(const double deltaFrameTime);

private:

   dtCore::RefPtr<dtCore::Object> mBoxObject;

   dtCore::RefPtr<dtCore::OrbitMotionModel> mOrbitMotion;

   // Offscreen rendering resources
   dtCore::RefPtr<dtCore::Scene>  mTextureScene;
   dtCore::RefPtr<dtCore::Camera> mTextureCamera;
   dtCore::RefPtr<dtCore::View>   mTextureView;

   // The texture to render into
   osg::ref_ptr<osg::Texture2D> mTextureTarget;

   void CreateTextureScene();
   void CreateTextureTarget(int width, int height);
   void CreateTextureCamera(int width, int height);

   void CenterCameraOnObject(dtCore::Object* object);
};

#endif // DELTA_TEST_PRERENDER

////////////////////////////////////////////////////////////////////////////////
