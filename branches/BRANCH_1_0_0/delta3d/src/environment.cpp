#include "environment.h"
#include "system.h"
#include "notify.h"
#include "camera.h"
#include "ephemeris.h"

using namespace dtCore;
IMPLEMENT_MANAGEMENT_LAYER(Environment)

Environment::Environment(const std::string name):
mVisibility(0.f),
mFogEnabled(true),
mFogMode(EXP2),
mFogNear(1.f),
mSunAltitude(0.f),
mSunAzimuth(0.f),
mAmbLightTable(NULL),
mDifLightTable(NULL),
mSpecLightTable(NULL),
mSkyLightTable(NULL),
mLastUpdate(0.0),
mCurrTime(0)
{
   RegisterInstance(this);

   SetName(name);
   mNode = new osg::Group();
   mEnvEffectNode = new osg::Group();
   mDrawableNode = new osg::Group();
   mNode->addChild(mEnvEffectNode.get());
   mNode->addChild(mDrawableNode.get());

   sgSetVec3(mSkyColor, 0.39f, 0.50f, 0.74f);
   sgSetVec3(mFogColor, 0.84f, 0.87f, 1.f);
   sgSetVec3(mAdvFogCtrl, 1.f, 10.f, 2.545 ); //T, E, N
   sgSetVec3(mSunColor, 1.f, 1.f, 1.f);
   sgCopyVec3(mModFogColor, mFogColor);
   
   sgSetVec2(mRefLatLong, 36.586944f, -121.842778f);

   mFog = new osg::Fog();
   osg::StateSet *state = mDrawableNode->getOrCreateStateSet();
   state->setAttributeAndModes(mFog.get());
   mDrawableNode->setStateSet(state);

   mSunlightShader = new SunlightShader();
   mSkyDomeShader = new SkyDomeShader();

   mAmbLightTable = new InterpTable();
   mAmbLightTable->AddEntry(-90.0, 0.040);
   mAmbLightTable->AddEntry(-25.0, 0.040);
   mAmbLightTable->AddEntry(-20.0, 0.048);
   mAmbLightTable->AddEntry(-15.0, 0.080);
   mAmbLightTable->AddEntry(-10.0, 0.112);
   mAmbLightTable->AddEntry(-5.0, 0.136);
   mAmbLightTable->AddEntry(-2.0, 0.168);
   mAmbLightTable->AddEntry(0.0, 0.200);
   mAmbLightTable->AddEntry(90.0, 0.200);

   mDifLightTable = new InterpTable();
   mDifLightTable->AddEntry(-90.0, 0.0);
   mDifLightTable->AddEntry(-25.0, 0.0);
   mDifLightTable->AddEntry(-20.0, 0.03);
   mDifLightTable->AddEntry(-15.0, 0.1);
   mDifLightTable->AddEntry(-10.0, 0.2);
   mDifLightTable->AddEntry(-5.0, 0.5);
   mDifLightTable->AddEntry(0.0, 0.748);
   mDifLightTable->AddEntry(5.0, 0.849);
   mDifLightTable->AddEntry(10.0, 0.914);
   mDifLightTable->AddEntry(20.0, 0.977);
   mDifLightTable->AddEntry(30.0, 0.995);
   mDifLightTable->AddEntry(40.0, 0.999);
   mDifLightTable->AddEntry(50.0, 1.0);
   mDifLightTable->AddEntry(90.0, 1.0);

   mSpecLightTable = new InterpTable();
   mSpecLightTable->AddEntry(-90.0, 0.215);
   mSpecLightTable->AddEntry(-15.0, 0.215);
   mSpecLightTable->AddEntry(-10.0, 0.257);
   mSpecLightTable->AddEntry(-5.0, 0.286);
   mSpecLightTable->AddEntry(-2.0, 0.386);
   mSpecLightTable->AddEntry(0.0, 0.315);
   mSpecLightTable->AddEntry(5.0, 0.455);
   mSpecLightTable->AddEntry(10.0, 0.5);
   mSpecLightTable->AddEntry(90.0, 0.5);

   mSkyLightTable = new InterpTable();
   mSkyLightTable->AddEntry(-90.0, 0.080);
   mSkyLightTable->AddEntry(-50.0, 0.080);
   mSkyLightTable->AddEntry(-40.0, 0.090);
   mSkyLightTable->AddEntry(-25.0, 0.090);
   mSkyLightTable->AddEntry(-20.0, 0.110);
   mSkyLightTable->AddEntry(-15.0, 0.120);
   mSkyLightTable->AddEntry(-10.0, 0.200);
   mSkyLightTable->AddEntry(-5.0, 0.350);
   mSkyLightTable->AddEntry(0.0, 0.616);
   mSkyLightTable->AddEntry(5.0, 0.806);
   mSkyLightTable->AddEntry(10.0, 0.895);
   mSkyLightTable->AddEntry(20.0, 0.962);
   mSkyLightTable->AddEntry(30.0, 0.989);
   mSkyLightTable->AddEntry(40.0, 0.997);
   mSkyLightTable->AddEntry(50.0, 1.0);
   mSkyLightTable->AddEntry(90.0, 1.0);

   SetFogColor(mFogColor);
   SetAdvFogCtrl(mAdvFogCtrl);
   SetSkyColor(mSkyColor);
   SetFogMode(EXP2);
   SetVisibility(16000.f);
   SetFogEnable(true);
   SetDateTime(-1, -1, -1, -1, -1, -1); //init to current system time/date

   Update(999.99);

   AddSender(System::GetSystem());
}

Environment::~Environment(void)
{
   DeregisterInstance(this);
   delete(mAmbLightTable);
   delete(mDifLightTable);
   delete(mSpecLightTable);
   delete(mSkyLightTable);
}

///Notifies this object that it has been added to a Scene
void Environment::AddedToScene(Scene* scene)
{
   mSkyLight = scene->GetSceneHandler()->GetSceneView()->getLight();
   mParentScene = scene;
}
      
// Add an Environmental Effect to the Environment
void Environment::AddEffect(EnvEffect *effect)
{
   if (!effect) return;

   // we add EnvEffects to our "mEnvEffectNode"
   if (std::find(mEffectList.begin(), mEffectList.end(), effect)==mEffectList.end())
   {
      //effect doesn't exit so add it to the list
      mEffectList.push_back(effect);

      if (IS_A(effect, SkyDome*)) //is a SkyDome
      {
         SkyDome *dome = (SkyDome*)effect;
         mSkyDome = dome;
         mEnvEffectNode->addChild( dome->GetNode() ); 

         //add the skydome shader to dome's stateset
         short attr = osg::StateAttribute::OFF;
         if (GetFogMode()==Environment::ADV) attr = osg::StateAttribute::ON;

         osg::StateSet *state = dome->GetNode()->getOrCreateStateSet();
         state->setAttributeAndModes(mSkyDomeShader->mLightScatterinVP, attr );
         state->setAttributeAndModes(mSkyDomeShader->mDomeFP, attr );
      }
      else
      {
         mEnvEffectNode->addChild(effect->GetNode());
      }

      this->Repaint();

   }
   else
   {
      return;
   }
}

/** Remove an EnvEffect from this Environment.  This method checks to see if 
  * has previously been added, then puts the effect into a holding bin for 
  * removal at a later time.
  */
void Environment::RemEffect(EnvEffect *effect)
{
   if (!effect) return;

   EnvEffectList::iterator it = std::find(mEffectList.begin(),mEffectList.end(),effect);
   if (it != mEffectList.end() )
   {
      //its in our list of effects, so lets mark it for removal
      mToBeRemoved.push_back(effect);
   }
   else
   {
      //not in our list
      return;
   }
}


/** Remove any EnvEffects that have been marked for removal.  This method 
  * needs to be called at a scene graph "safe" time.
  */
void Environment::RemoveEffectCache(void)
{
   for (EnvEffectList::iterator it=mToBeRemoved.begin();
      it!= mToBeRemoved.end();
      it++)
   {
      EnvEffect *effect = it->get();

      mEnvEffectNode->removeChild( effect->GetNode() );

      //also take it out of the Environment's effect list
      EnvEffectList::iterator itr = std::find(mEffectList.begin(),mEffectList.end(),effect);
      if (itr != mEffectList.end())
      {
         mEffectList.erase(itr);
      }
   }
   mToBeRemoved.clear();
}


///Add a DeltaDrawable to the Scene to be viewed.
void Environment::AddDrawable( DeltaDrawable *drawable )
{
   //we add Drawables to our mDrawableNode
   if (drawable)
   {
      mDrawableNode->addChild( drawable->GetOSGNode() );

      Physical* physical = dynamic_cast<Physical*>(drawable);

      if(physical != NULL)
      {
         if (mParentScene.valid())
         {
            mParentScene->RegisterPhysical(physical);
         }
      }
   }
}

///Remove a DeltaDrawable from the Environment Node.
void Environment::RemoveDrawable( DeltaDrawable *drawable )
{
	//we add Drawables to our mDrawableNode
	if (drawable)
	{
		mDrawableNode->removeChild( drawable->GetOSGNode() );

		Physical* physical = dynamic_cast<Physical*>(drawable);

		if(physical != NULL)
		{
			if (mParentScene.valid())
			{
				mParentScene->UnRegisterPhysical(physical);
			}
		}
	}
}


void Environment::OnMessage(MessageData *data)
{
   if (data->message == "preframe")
   {
       double *deltaFrameTime = (double*)data->userData;
      Update(*deltaFrameTime);
   }
   else if (data->message == "postframe")
   {
      //remove any EnvEffects that need removing
      if(mToBeRemoved.size()>0) RemoveEffectCache();
   }
}


/** Set the base color of the fog.  This color is then adjusted internally
  * using the time of day.  NOTE: This value is not used for the fog 
  * when the FogMode is ADV, but it still can be used by the EnvEffects.
  */
void dtCore::Environment::SetFogColor(sgVec3 color)
{
   sgCopyVec3(mFogColor, color); // base fog color

   UpdateFogColor(); //tweak color based on sun angle

   Repaint();
}

void dtCore::Environment::Repaint(void)
{
   float vis = mVisibility;
   if (!GetFogEnable()) vis = 200000.f;

   //if fog is enabled, use the modified fog color otherwise just use
   //the modified sky color
   sgVec3 fogColor;
   sgCopyVec3(fogColor, mModFogColor);
   if (!GetFogEnable()) sgCopyVec3(fogColor, mModSkyColor);

   for (EnvEffectList::iterator it=mEffectList.begin(); it!=mEffectList.end(); it++)
   {
      (*it)->Repaint(mModSkyColor, fogColor, mSunAltitude, mSunAzimuth, vis );
   }
}

void dtCore::Environment::SetSkyColor(sgVec3 color)
{
   //what does this do if there is no SkyDome in the Environment?  Clear color?
   sgCopyVec3(mSkyColor, color);
   Repaint();
}

/** Set the fog mode.  Any Drawables added to the Environment will be fogged
*   based on the FogMode.  The ADV FogMode uses a pixel shader to calculate
*   a somewhat physically correct fog/haze.  LINEAR, EXP, and EXP2 use standard
*   OpenGL Fog.
*/
void dtCore::Environment::SetFogMode(FogMode mode)
{
   if (mFogMode == mode) return;

   mFogMode = mode;
   osg::Fog::Mode fm;
   short attr = osg::StateAttribute::OFF;

   switch (mode) {
   case Environment::LINEAR:  fm = osg::Fog::LINEAR;  break;
   case Environment::EXP:     fm = osg::Fog::EXP;   	break;
   case Environment::EXP2:    fm = osg::Fog::EXP2;    break;
   case Environment::ADV:
      {
         fm = osg::Fog::LINEAR;
         if (GetFogEnable())  attr = osg::StateAttribute::ON;
         else                 attr = osg::StateAttribute::OFF;
      }
      break;
   default: fm = osg::Fog::LINEAR; break;
   }

   //when we switch to ADV mode, all we really do is enable/disable this
   //shader.  So when we SetFogMode to ADV, we have to check to see if the
   //FogEnable is on or off.
   osg::StateSet *state = mDrawableNode->getOrCreateStateSet();
   state->setAttributeAndModes(mSunlightShader->mLightScatterinVP, attr );
   state->setAttributeAndModes(mSunlightShader->mTerrainFP, attr );

   //if we're using a skyDome, turn on/off its shader
   if (mSkyDome.valid())
   {
      state = mSkyDome.get()->GetNode()->getOrCreateStateSet();
      state->setAttributeAndModes(mSkyDomeShader->mLightScatterinVP, attr );
      state->setAttributeAndModes(mSkyDomeShader->mDomeFP, attr );
   }

   mFog->setMode(fm);
}

/** Set the fog's near value.  This is only used when the SetFogMode() is 
  * LINEAR.  
  * @param val : the near value (meters)
  */
void dtCore::Environment::SetFogNear(const float val)
{
   mFogNear = val;
   if (mFogNear<0.f) mFogNear = 0.f;

   if (mFogNear > mVisibility) mFogNear = mVisibility;

   mFog->setStart(mFogNear);
   mFog->setEnd(mVisibility);

   Repaint();
}

/** Set's the distance at which the scene becomes fully "fogged".
  *
  * @param distance : the distance of "full" fog (meters)
  */
void dtCore::Environment::SetVisibility(float distance)
{
   if (mVisibility == distance) return;

   mVisibility = distance;

   double sqrt_m_log01 = sqrt( -log(0.01) );
   float density = sqrt_m_log01/mVisibility;

   mFog->setDensity(density);
   mFog->setEnd(mVisibility);
   Repaint();
}

/** Turns the fog effect on or off */
void dtCore::Environment::SetFogEnable(bool enable)
{
   if (mFogEnabled == enable) return;

   mFogEnabled = enable;
   osg::StateSet *state = mDrawableNode->getOrCreateStateSet();
   short attr = osg::StateAttribute::ON;

   if (enable)
   {
      attr = osg::StateAttribute::ON;
   }
   else 
   {
      attr = osg::StateAttribute::OFF;
   }

   state->setMode( GL_FOG, attr );

   if (GetFogMode()==Environment::ADV)
   {
      //if we're using ADV, then we turn on/off this shader which overrides
      //the standard openGL fog
      state->setAttributeAndModes(mSunlightShader->mLightScatterinVP, attr );
      state->setAttributeAndModes(mSunlightShader->mTerrainFP, attr );

      //if we're using a skyDome, turn on/off its shader
      if (mSkyDome.valid())
      {        
         state = mSkyDome.get()->GetNode()->getOrCreateStateSet();
         state->setAttributeAndModes(mSkyDomeShader->mLightScatterinVP, attr );
         state->setAttributeAndModes(mSkyDomeShader->mDomeFP, attr );
      }
   }

   Repaint();
}



/** Set the starting date and time.  Any value of -1 resets the date/time
*  to be the system time.
* @param yr Year (1900-xxxx)
* @param mo Month of the year (1-12)
* @param da Day of the Month (1-31)
* @param hr Hour since midnight (0-23)
* @param mi Minutes after the hour (0-59)
* @param sc Seconds pass the minute (0-59)
*/
void dtCore::Environment::SetDateTime( const int yr, const int mo, const int da,
                                    const int hr, const int mi, const int sc)
{
   if ((yr == -1) || (mo == -1) || (da == -1) ||
      (hr == -1) || (mi == -1) || (sc == -1) )
   {
      mCurrTime = time(NULL);
   }
   else 
   {
      mCurrTime = GetGMT(yr-1900, mo-1, da, hr, mi, sc);
   }
   Notify(DEBUG_INFO, "Sim time set to:%s", asctime(localtime(&mCurrTime)) );

   Update(999.99);
}


/** Get the current Date and Time of this Environment within a second.
* @param yr year
* @param mo month 
* @param da day
* @param hr hour
* @param mi minute
* @param sc second
*/
void dtCore::Environment::GetDateTime( int *yr,  int *mo,  int *da,
                                    int *hr,  int *mi,  int *sc)
{
   struct tm *tmp = localtime(&mCurrTime);
   *yr = tmp->tm_year+1900;  *mo = tmp->tm_mon+1; *da = tmp->tm_mday;
   *hr = tmp->tm_hour;  *mi = tmp->tm_min; *sc = tmp->tm_sec;
}


void dtCore::Environment::Update(const double deltaFrameTime)
{
   mLastUpdate += deltaFrameTime;
   if (mLastUpdate > 1.0)
   {
      mLastUpdate = 0.0;      
      double sun_alt, sun_az;
      GetSunPos(mCurrTime, mRefLatLong[0], mRefLatLong[1], 1.0, &sun_alt, &sun_az);
      mSunAltitude = sun_alt;
      mSunAzimuth = sun_az;
      UpdateSkyLight();
      UpdateSunColor();
      UpdateEnvColors();
      UpdateFogColor();
      Repaint();
   }

   if (GetFogMode()==Environment::ADV) UpdateShaders();
}

void dtCore::Environment::UpdateEnvColors(void)
{
   sgVec3 white ={1.f, 1.f, 1.f};
   sgVec3 baseFogColor = {0.84f, 0.87f, 1.f};

   float ambient = mAmbLightTable->Interpolate( mSunAltitude );
   float diffuse = mDifLightTable->Interpolate( mSunAltitude );
   float specular = mSpecLightTable->Interpolate(mSunAltitude);
   float skyBright = mSkyLightTable->Interpolate(mSunAltitude);

   //Modify the sky color
   sgScaleVec3(mModSkyColor, mSkyColor, skyBright);

   //Modify the fog color based on sky brightness
   sgScaleVec3(mModFogColor, mFogColor, skyBright);
}


Environment::InterpTable::InterpTable():
mSize(0)
{
}

Environment::InterpTable::~InterpTable()
{
}

void Environment::InterpTable::AddEntry(double ind, double dep)
{
   mTable.push_back(TableEntry(ind,dep));
   mSize++;
}

double Environment::InterpTable::Interpolate(double x) const
{
   int i = 0;
   double y;

   if (mSize == 0)
   {
      return (0.0);
   }

   while ((i<mSize) && (x>mTable[i].ind)) 
   {
      i++;
   }

   if (i <= 0)
   {
      return mTable[0].dep;
   }

   if (i >= mSize)
   {
      return mTable[mSize-1].dep;
   }

   y = mTable[i].dep +
      ((mTable[i-1].dep - mTable[i].dep) *
       (x-mTable[i].ind) ) /
       (mTable[i-1].ind - mTable[i].ind);

   return (y);
}

/**
 * Updates the sky light based on the sun angle.
 */
void dtCore::Environment::UpdateSkyLight(void)
{
   if(mSkyLight.valid())
   {
      mSkyLight->setPosition(
         osg::Vec4(
            sgSin(mSunAzimuth)*sgCos(mSunAltitude),
            sgCos(mSunAzimuth)*sgCos(mSunAltitude),
            sgSin(mSunAltitude),
            0.0f
         )
      );
   }
}

/** Update the fog color based on the sun angle and the sun color.
*
*  TODO: Adjust based on the Camera's current heading
*/
void dtCore::Environment::UpdateFogColor(void)
{
   // Calculate the fog color in the direction of the sun for
   // sunrise/sunset effects.
   float red =   (mModFogColor[0] + 2.f * mSunColor[0]*mSunColor[0]) / 3.f;
   float green = (mModFogColor[1] + 2.f * mSunColor[1]*mSunColor[1]) / 3.f;
   float blue =  (mModFogColor[2] + 2.f * mSunColor[2]) / 3.f;

   // interpolate between the sunrise/sunset color and the color
   // at the opposite direction of this effect. Take in account
   // the current visibility.
   float av = GetVisibility();
   if (av > 20000.f)   av = 20000.f;

   double sunRotation = osg::DegreesToRadians(-95.0);
   double heading = osg::DegreesToRadians(-95.0);

   double rotation = -(sunRotation+SGD_PI) - heading;

   float avf = 1.f - (20000.f - av) / 20000.f;
   float sif = 0.5f - cosf(osg::DegreesToRadians(mSunAltitude)*2.f)/2.f + 0.000001f;

   float rf1  = fabs((rotation-SGD_PI)/SGD_PI); //difference between eyepoint heading and sun heading (rad)
   float rf2 = avf * pow(rf1*rf1, 1/sif);

   float rf3  = 1.f - rf2;
   
   mModFogColor[0] = rf3 * mModFogColor[0] + rf2 * red;
   mModFogColor[1] = rf3 * mModFogColor[1] + rf2 * green;
   mModFogColor[2] = rf3 * mModFogColor[2] + rf2 * blue;

   //now apply the fog's color
   mFog->setColor(osg::Vec4(mModFogColor[0], mModFogColor[1], mModFogColor[2], 1.f) );
}

/** Update the color of the sun based on it's angle.
*/
void dtCore::Environment::UpdateSunColor(void)
{
   float sunFactor = 4.f * cosf(osg::DegreesToRadians(mSunAltitude)); //-4..4

   if (sunFactor > 1.f) sunFactor = 1.f;
   else if (sunFactor<-1.f) sunFactor = -1.f;

   sunFactor = sunFactor/2.f + 0.5f; //1..0

   sgVec3 color;
   color[1] = sqrtf(sunFactor);
   color[0] = sqrtf(color[1]);
   color[2] = sunFactor * sunFactor;
   color[2] *= color[2];
   
   sgCopyVec3(mSunColor, color);
}

/** Private method used to pass parameters to the light scattering shader */
void dtCore::Environment::UpdateShaders()
{
   sgVec2 sunDir;
   GetSunAzEl(&sunDir[0], &sunDir[1]);

   Camera *cam  = Camera::GetInstance(0);
   Transform camXform;
   cam->GetTransform( &camXform );
   sgVec3 xyz;
   camXform.GetTranslation(xyz);

   mSunlightShader->Update( sunDir,
                            xyz, mAdvFogCtrl[0], mAdvFogCtrl[1], 
                           mAdvFogCtrl[2] * 10.0e25 );

   mSkyDomeShader->Update( sunDir,
                           mAdvFogCtrl[0], mAdvFogCtrl[1], 
                           mAdvFogCtrl[2] * 10.0e25 );
}

void dtCore::Environment::SetRefLatLong(sgVec2 latLong)
{
   sgCopyVec2(mRefLatLong, latLong);
}
