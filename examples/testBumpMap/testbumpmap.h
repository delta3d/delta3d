/* -*-c++-*-
* testbumpmap - testbumpmap (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2006-2008, Alion Science and Technology Corporation
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
#ifndef DELTA_TEST_BUMPMAP
#define DELTA_TEST_BUMPMAP

#include <dtABC/application.h>
#include <dtABC/labelactor.h>
#include <dtCore/object.h>
#include <dtCore/shaderparamvec4.h>
#include <dtCore/shaderparamint.h>
#include <dtCore/orbitmotionmodel.h>

////////////////////////////////////////////////////////////////////////////////

class TestBumpMapApp : public dtABC::Application
{
public:

   TestBumpMapApp(const std::string& customObjectName, 
                  const std::string& configFilename = "config.xml",
                  bool usePrecomputedTangents = true);

protected:

   virtual ~TestBumpMapApp(){}

public:  

   void LoadGeometry(const std::string& customObjectName);
   void LoadTextures();

   virtual bool KeyPressed(const dtCore::Keyboard* keyboard, int key);

   virtual void PreFrame(const double deltaFrameTime);

private:

   dtCore::RefPtr<dtCore::Object> mSphere;
   dtCore::RefPtr<dtCore::Object> mCustomObject;
   dtCore::RefPtr<dtCore::Object> mLightObject;

   float mPitchAngle;
   float mYawAngle;
   bool mUsePrecomputedTangents;

   osg::ref_ptr<osg::Texture2D> mDiffuseTexture;
   osg::ref_ptr<osg::Texture2D> mNormalTexture;
   osg::ref_ptr<osg::Texture2D> mSpecularTexture;

   dtCore::RefPtr<dtCore::ShaderParamInt>  mCustomShaderMode;
   dtCore::RefPtr<dtCore::ShaderParamInt>  mSphereShaderMode;
   dtCore::RefPtr<dtCore::ShaderParamVec4> mLightPosition;
   dtCore::RefPtr<dtCore::OrbitMotionModel> mOrbitMotion;
   dtCore::RefPtr<dtABC::LabelActor> mLabel;

   osg::Uniform* mRotationUniform;

   void GenerateTangentsForObject(dtCore::Object* object);
   void AssignShaderToObject(dtCore::Object* object, dtCore::ShaderParamInt*& outMode);
   void CenterCameraOnObject(dtCore::Object* object);
   void CreateHelpLabel();
   std::string CreateHelpLabelText();
};

#endif // DELTA_TEST_BUMPMAP

////////////////////////////////////////////////////////////////////////////////
