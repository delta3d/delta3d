#include "dt.h"
#include "dtabc.h"
#include "soarx/dtsoarx.h"

#include <osg/CullSettings>
#include <osg/PolygonMode>
#include <osg/StateAttribute>
#include <osg/StateSet>

#include <iostream>

using namespace dtCore;
using namespace dtABC;
using namespace dtSOARX;
using namespace std;

/**
 * The terrain test application.
 */
class TestTerrainApplication : public Application
{
   DECLARE_MANAGEMENT_LAYER(TestTerrainApplication)
   
   public:
   
      /**
       * Constructor.
       */
      TestTerrainApplication()
         : Application("config.xml"),
           mWalkMode(false),
           mWireframe(false),
           mTimeOfDay(-1.0f)
      {
         SetDataFilePathList("../../data/;./data/;" + GetDeltaDataPathList());
         
         mEnvironment = new Environment;
         
         mEnvironment->SetVisibility(500000.0f);
         
         mEnvironment->AddEffect(new SkyDome);
         
         GetScene()->AddDrawable(mEnvironment.get());
         
         mSOARXTerrain = new SOARXTerrain;

         mSOARXTerrain->LoadConfiguration("data/soarxterrain.xml");
         
         mEnvironment->AddDrawable(mSOARXTerrain.get());
         
         GetScene()->GetSceneHandler()->GetSceneView()->setComputeNearFarMode(
            osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR
         );
         
         GetCamera()->GetLens()->setPerspective(
            60.0f, 60.0f, 1.0f, 100000.0f
         );
         
         Transform transform(0, 0, 2000.0);
         
         GetCamera()->SetTransform(&transform);
      }
      
      /**
       * Pre-frame callback.
       *
       * @param deltaFrameTime the amount of time elapsed since the last frame
       */
      virtual void PreFrame(const double deltaFrameTime)
      {
         if(GetKeyboard()->GetKeyState(Producer::Key_minus))
         {
            mSOARXTerrain->SetThreshold(
               clamp((float)(mSOARXTerrain->GetThreshold() - deltaFrameTime*5.0), 1.0f, 10.0f)
            );
         }
         
         if(GetKeyboard()->GetKeyState(Producer::Key_equal))
         {
            mSOARXTerrain->SetThreshold(
               clamp((float)(mSOARXTerrain->GetThreshold() + deltaFrameTime*5.0), 1.0f, 10.0f)
            );
         }
         
         if(GetKeyboard()->GetKeyState(Producer::Key_bracketleft))
         {
            mSOARXTerrain->SetDetailMultiplier(
               clamp((float)(mSOARXTerrain->GetDetailMultiplier() - deltaFrameTime*5.0), 1.0f, 20.0f)
            );
         }
         
         if(GetKeyboard()->GetKeyState(Producer::Key_bracketright))
         {
            mSOARXTerrain->SetDetailMultiplier(
               clamp((float)(mSOARXTerrain->GetDetailMultiplier() + deltaFrameTime*5.0), 1.0f, 20.0f)
            );
         }
         
         if(GetKeyboard()->GetKeyState(Producer::Key_semicolon))
         {
            int year, month, day, hour, minute, second;
            
            mEnvironment->GetDateTime(&year, &month, &day, &hour, &minute, &second);
          
            if(mTimeOfDay < 0.0f)
            {
               mTimeOfDay = hour*100.0f+minute*100.0f/60.0f;
            }
            
            mTimeOfDay -= (float)(deltaFrameTime*100);
            
            if(mTimeOfDay < 0.0f)
            {
               mTimeOfDay += 2400.0f;
            }
            
            hour = (int)mTimeOfDay/100;
            
            minute = (int)(fmod(mTimeOfDay, 100.0f)*60/100);
            
            mEnvironment->SetDateTime(
               year, month, day, hour, minute, second
            );
         }
         
         if(GetKeyboard()->GetKeyState(Producer::Key_apostrophe))
         {
            int year, month, day, hour, minute, second;
            
            mEnvironment->GetDateTime(&year, &month, &day, &hour, &minute, &second);
            
            if(mTimeOfDay < 0.0f)
            {
               mTimeOfDay = hour*100.0f+minute*100.0f/60.0f;
            }
            
            mTimeOfDay += (float)(deltaFrameTime*100);
            
            if(mTimeOfDay > 2400.0f)
            {
               mTimeOfDay -= 2400.0f;
            }
            
            hour = (int)mTimeOfDay/100;
            
            minute = (int)(fmod(mTimeOfDay, 100.0f)*60/100);
            
            mEnvironment->SetDateTime(
               year, month, day, hour, minute, second
            );
         }
         
         Transform transform;
            
         GetCamera()->GetTransform(&transform);
         
         sgVec3 xyz, hpr;
         
         transform.Get(xyz, hpr);
         
         if(GetKeyboard()->GetKeyState(Producer::Key_Left))
         {
            hpr[0] += (float)(deltaFrameTime*90.0);
         }
         
         if(GetKeyboard()->GetKeyState(Producer::Key_Right))
         {
            hpr[0] -= (float)(deltaFrameTime*90.0);
         }
         
         float speed = mWalkMode ? 100.0 : 10000.0;
         
         if(GetKeyboard()->GetKeyState(Producer::Key_Up))
         {
            if(GetKeyboard()->GetKeyState(Producer::Key_Shift_R) ||
               GetKeyboard()->GetKeyState(Producer::Key_Shift_L))
            {
               xyz[2] += (float)(deltaFrameTime*speed);
            }
            else
            {
               sgVec3 vec = { 0, deltaFrameTime*speed, 0 };
               
               sgMat4 mat;
               
               sgMakeRotMat4(mat, hpr[0], 0, 0);
               
               sgXformVec3(vec, mat);
               
               sgAddVec3(xyz, vec);
            }
         }
         
         if(GetKeyboard()->GetKeyState(Producer::Key_Down))
         {
            if(GetKeyboard()->GetKeyState(Producer::Key_Shift_R) ||
               GetKeyboard()->GetKeyState(Producer::Key_Shift_L))
            {
               xyz[2] -= (float)(deltaFrameTime*speed);
            }
            else
            {
               sgVec3 vec = { 0, -deltaFrameTime*speed, 0 };
               
               sgMat4 mat;
               
               sgMakeRotMat4(mat, hpr[0], 0, 0);
               
               sgXformVec3(vec, mat);
               
               sgAddVec3(xyz, vec);
            }
         }
         
         if(mWalkMode)
         {
            // Clamp to ground
            
            xyz[2] = mSOARXTerrain->GetHeight(xyz[0], xyz[1]) + 2.0f;
         }
         
         transform.Set(xyz, hpr);
         
         GetCamera()->SetTransform(&transform);
      }
      
      /**
       * KeyboardListener override
       * Called when a key is pressed.
       *
       * @param keyboard the source of the event
       * @param key the key pressed
       * @param character the corresponding character
       */
      virtual void KeyPressed(dtCore::Keyboard* keyboard, 
                              Producer::KeyboardKey key,
                              Producer::KeyCharacter character)
      {
         Application::KeyPressed(keyboard, key, character);
         
         osg::StateSet* ss;
         osg::PolygonMode* pm;
         
         switch(key)
         {
            case Producer::Key_W:
               mWalkMode = !mWalkMode;
               break;
               
            case Producer::Key_space:
               
               mWireframe = !mWireframe;
               
               ss = mSOARXTerrain->GetOSGNode()->getOrCreateStateSet();
               
               pm = (osg::PolygonMode*)ss->getAttribute(osg::StateAttribute::POLYGONMODE);
               
               if(pm == NULL)
               {
                  pm = new osg::PolygonMode;
                  
                  ss->setAttributeAndModes(pm);
               }
               
               pm->setMode(
                  osg::PolygonMode::FRONT_AND_BACK,
                  mWireframe ? osg::PolygonMode::LINE : osg::PolygonMode::FILL
               );
               
               break;
               
            case Producer::Key_Return:
               GetScene()->SetNextStatisticsType();
               break;
         }
      }
                                
      
   private:
      
      /**
       * The camera motion model.
       */
      osg::ref_ptr<UFOMotionModel> mCameraMotionModel;
      
      /**
       * The environment.
       */
      osg::ref_ptr<Environment> mEnvironment;
      
      /**
       * The SOARX terrain object.
       */
      osg::ref_ptr<SOARXTerrain> mSOARXTerrain;
      
      /**
       * The navigation mode (walk or magic carpet).
       */
      bool mWalkMode;
      
      /**
       * The time of day, in minutes, from 0000 to 2359.
       */
      float mTimeOfDay;
      
      /**
       * Wireframe flag.
       */
      bool mWireframe;
};

IMPLEMENT_MANAGEMENT_LAYER(TestTerrainApplication)

int main( int argc, char **argv )
{  
   TestTerrainApplication* testTerrainApp = 
      new TestTerrainApplication;

   testTerrainApp->Config();
   testTerrainApp->Run();
   
   return 0;
}