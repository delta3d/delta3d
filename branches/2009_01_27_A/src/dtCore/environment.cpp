#include <prefix/dtcoreprefix-src.h>
#include <dtCore/environment.h>

#include <dtCore/camera.h>
#include <dtCore/enveffect.h>
#include <dtCore/ephemeris.h>
#include <dtCore/infinitelight.h>
#include <dtCore/scene.h>
#include <dtCore/keyboardmousehandler.h> // due to including scene.h
#include <dtCore/skydome.h>
#include <dtCore/skydomeshader.h>
#include <dtCore/sunlightshader.h>
#include <dtCore/system.h>
#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>
#include <osg/Fog>
#include <osg/FragmentProgram>
#include <osg/Group>
#include <osg/LightSource>
#include <osg/VertexProgram>

#include <cmath>

using namespace dtCore;
using namespace dtUtil;

IMPLEMENT_MANAGEMENT_LAYER(Environment)

////////////////////////////////////////////////////////////////////////////////
Environment::Environment(const std::string& name)
   : mAmbLightTable(new InterpTable())
   , mDifLightTable(new InterpTable())
   , mSpecLightTable(new InterpTable())
   , mSkyLightTable(new InterpTable())
   , mEnvEffectNode(new osg::Group())
   , mDrawableNode(new osg::Group())
   , mSkyLight(NULL)
   , mFog(new osg::Fog())
   , mVisibility(0)
   , mFogEnabled(true)
   , mFogMode(EXP2)
   , mFogNear(1.0f)
   , mSunAltitude(0)
   , mSunAzimuth(0)
   , mCurrTime(dtUtil::DateTime::TimeOrigin::LOCAL_TIME)
   , mLastUpdate(0)
   , mSunlightShader(new SunlightShader())
   , mSkyDomeShader(new SkyDomeShader())
   , mSkyDome(NULL)
{
   RegisterInstance(this);

   SetName(name);
   osg::Node *rootNode = new osg::Group();
   rootNode->setName(name);
   SetOSGNode(rootNode);
   GetOSGNode()->asGroup()->addChild(mEnvEffectNode.get());
   GetOSGNode()->asGroup()->addChild(mDrawableNode.get());

   mSkyColor.set(0.39f, 0.50f, 0.74f);
   mFogColor.set(0.84f, 0.87f, 1.f);
   mAdvFogCtrl.set(1.f, 10.f, 2.545); // T, E, N
   mSunColor.set(1.f, 1.f, 1.f);
   mModFogColor.set(mFogColor);

   mRefLatLong.set(36.586944f, -121.842778f);

   osg::StateSet* state = mDrawableNode->getOrCreateStateSet();
   state->setAttributeAndModes(mFog.get());
   mDrawableNode->setStateSet(state);

   mAmbLightTable->AddEntry(-90.0, 0.040);
   mAmbLightTable->AddEntry(-25.0, 0.040);
   mAmbLightTable->AddEntry(-20.0, 0.048);
   mAmbLightTable->AddEntry(-15.0, 0.080);
   mAmbLightTable->AddEntry(-10.0, 0.112);
   mAmbLightTable->AddEntry(-5.0, 0.136);
   mAmbLightTable->AddEntry(-2.0, 0.168);
   mAmbLightTable->AddEntry(0.0, 0.200);
   mAmbLightTable->AddEntry(90.0, 0.200);

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

   mSpecLightTable->AddEntry(-90.0, 0.215);
   mSpecLightTable->AddEntry(-15.0, 0.215);
   mSpecLightTable->AddEntry(-10.0, 0.257);
   mSpecLightTable->AddEntry(-5.0, 0.286);
   mSpecLightTable->AddEntry(-2.0, 0.386);
   mSpecLightTable->AddEntry(0.0, 0.315);
   mSpecLightTable->AddEntry(5.0, 0.455);
   mSpecLightTable->AddEntry(10.0, 0.5);
   mSpecLightTable->AddEntry(90.0, 0.5);

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

   Update(999.99);

   AddSender(&System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
Environment::~Environment()
{
   while (GetNumEffects() > 0)
   {
      if (EnvEffect* e = GetEffect(0))
      {
         RemEffect(e);
         RemoveEffectCache();
      }
   }

   while (GetNumChildren() > 0)
   {
      DeltaDrawable* d = GetChild(0);
      RemoveChild(d);
   }

   DeregisterInstance(this);

   delete mAmbLightTable;
   delete mDifLightTable;
   delete mSpecLightTable;
   delete mSkyLightTable;
   delete mSunlightShader;
   delete mSkyDomeShader;

   RemoveSender(&System::GetInstance());
}

////////////////////////////////////////////////////////////////////////////////
void Environment::AddedToScene(Scene* scene)
{
   DeltaDrawable::AddedToScene(scene);

   if (scene != 0)
   {
      mSkyLight = scene->GetLight(0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Environment::AddEffect(EnvEffect* effect)
{
   if (!effect) 
   {
      return; 
   }

   // we add EnvEffects to our "mEnvEffectNode"
   if (std::find(mEffectList.begin(), mEffectList.end(), effect) == mEffectList.end())
   {
      // effect doesn't exit so add it to the list
      mEffectList.push_back(effect);

      if (SkyDome* dome = dynamic_cast<SkyDome*>(effect)) //is a SkyDome
      {
         mSkyDome = dome;
         mEnvEffectNode->addChild(dome->GetOSGNode());

         // add the skydome shader to dome's stateset
         short attr = osg::StateAttribute::OFF;
         if (GetFogMode() == Environment::ADV) attr = osg::StateAttribute::ON;

         osg::StateSet* state = dome->GetOSGNode()->getOrCreateStateSet();
         state->setAttributeAndModes(mSkyDomeShader->GetLightScatterinVP(), attr);
         state->setAttributeAndModes(mSkyDomeShader->GetDomeFP(), attr);
      }
      else
      {
         mEnvEffectNode->addChild(effect->GetOSGNode());
      }

      this->Repaint();
   }
}

////////////////////////////////////////////////////////////////////////////////
void Environment::RemEffect(EnvEffect* effect)
{
   if (!effect) 
   {
      return; 
   }

   EnvEffectList::iterator it = std::find(mEffectList.begin(), mEffectList.end(), effect);
   if (it != mEffectList.end())
   {
      //its in our list of effects, so lets mark it for removal
      mToBeRemoved.push_back(effect);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Environment::RemoveEffectCache()
{
   for (EnvEffectList::iterator it = mToBeRemoved.begin();
        it != mToBeRemoved.end();
        ++it)
   {
      EnvEffect* effect = it->get();

      mEnvEffectNode->removeChild(effect->GetOSGNode());

      // Also take it out of the Environment's effect list
      mEffectList.erase(std::remove(mEffectList.begin(), mEffectList.end(), effect),
                        mEffectList.end());
   }

   mToBeRemoved.clear();
}

////////////////////////////////////////////////////////////////////////////////
bool Environment::AddChild(DeltaDrawable* child)
{
   // we add Drawables to our mDrawableNode
   if (DeltaDrawable::AddChild(child))
   {
      mDrawableNode->addChild(child->GetOSGNode());
      return true;
   }
   else
   {
      return false;
   }
}

////////////////////////////////////////////////////////////////////////////////
void Environment::RemoveChild(DeltaDrawable* child)
{
   // we add Drawables to our mDrawableNode
   if (child)
   {
      mDrawableNode->removeChild(child->GetOSGNode());
      DeltaDrawable::RemoveChild(child);
   }
}

////////////////////////////////////////////////////////////////////////////////
void Environment::OnMessage(MessageData* data)
{
   if (data->message == dtCore::System::MESSAGE_PRE_FRAME)
   {
      double deltaFrameTime = *static_cast<double*>(data->userData);
      Update(deltaFrameTime);
   }
   else if (data->message == dtCore::System::MESSAGE_POST_FRAME)
   {
      // remove any EnvEffects that need removing
      if (mToBeRemoved.size() > 0) 
      {
         RemoveEffectCache(); 
      }
   }
   else if (data->message == dtCore::System::MESSAGE_EXIT)
   {
      // time to get rid of any added children
      while (GetNumChildren() > 0)
      {
         DeltaDrawable* d = GetChild(0);
         RemoveChild(d);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetFogColor(const osg::Vec3& color)
{
   mFogColor = color; // base fog color

   UpdateFogColor(); // tweak color based on sun angle

   Repaint();
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::Repaint()
{
   float vis = mVisibility;
   if (!GetFogEnable()) { vis = 200000.f; }

   // if fog is enabled, use the modified fog color otherwise just use
   // the modified sky color
   osg::Vec3 fogColor = mModFogColor;
   if (!GetFogEnable()) { fogColor = mModSkyColor; }

   for (EnvEffectList::iterator it = mEffectList.begin(); it != mEffectList.end(); ++it)
   {
      (*it)->Repaint(mModSkyColor, fogColor, mSunAltitude, mSunAzimuth, vis);
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetSkyColor(const osg::Vec3& color)
{
   // what does this do if there is no SkyDome in the Environment?  Clear color?
   mSkyColor = color;
   Repaint();
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetFogMode(FogMode mode)
{
   if (mFogMode == mode) { return; }

   mFogMode = mode;
   osg::Fog::Mode fm;
   short attr = osg::StateAttribute::OFF;

   switch (mode)
   {
   case Environment::LINEAR:  fm = osg::Fog::LINEAR; break;
   case Environment::EXP:     fm = osg::Fog::EXP;    break;
   case Environment::EXP2:    fm = osg::Fog::EXP2;   break;
   case Environment::ADV:
      {
         fm = osg::Fog::LINEAR;
         if (GetFogEnable())  { attr = osg::StateAttribute::ON;  }
         else                 { attr = osg::StateAttribute::OFF; }
      }
      break;
   default: fm = osg::Fog::LINEAR; break;
   }

   // when we switch to ADV mode, all we really do is enable/disable this
   // shader.  So when we SetFogMode to ADV, we have to check to see if the
   // FogEnable is on or off.
   osg::StateSet* state = mDrawableNode->getOrCreateStateSet();
   state->setAttributeAndModes(mSunlightShader->GetLightScatterinVP(), attr);
   state->setAttributeAndModes(mSunlightShader->GetTerrainFP(), attr);

   // if we're using a skyDome, turn on/off its shader
   if (mSkyDome.valid())
   {
      state = mSkyDome->GetOSGNode()->getOrCreateStateSet();
      state->setAttributeAndModes(mSkyDomeShader->GetLightScatterinVP(), attr);
      state->setAttributeAndModes(mSkyDomeShader->GetDomeFP(), attr);
   }

   mFog->setMode(fm);
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetFogNear(float val)
{
   mFogNear = val;

   if (mFogNear < 0.f)
   {
      mFogNear = 0.f;
   }
   if (mFogNear > mVisibility)
   {
      mFogNear = mVisibility;
   }

   mFog->setStart(mFogNear);
   mFog->setEnd(mVisibility);

   Repaint();
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetVisibility(float distance)
{
   if (mVisibility == distance)
   {
      return;
   }

   mVisibility = distance;

   double sqrt_m_log01 = sqrt(-log(0.01));
   float density = sqrt_m_log01 / mVisibility;

   mFog->setDensity(density);
   mFog->setEnd(mVisibility);
   Repaint();
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetFogEnable(bool enable)
{
   if (mFogEnabled == enable)
   {
      return;
   }

   mFogEnabled = enable;
   osg::StateSet* state = mDrawableNode->getOrCreateStateSet();
   short attr = osg::StateAttribute::ON;

   if (enable)
   {
      attr = osg::StateAttribute::ON;
   }
   else
   {
      attr = osg::StateAttribute::OFF;
   }

   state->setMode(GL_FOG, attr);

   if (GetFogMode() == Environment::ADV)
   {
      // if we're using ADV, then we turn on/off this shader which overrides
      // the standard openGL fog
      state->setAttributeAndModes(mSunlightShader->GetLightScatterinVP(), attr);
      state->setAttributeAndModes(mSunlightShader->GetTerrainFP(), attr);

      // if we're using a skyDome, turn on/off its shader
      if (mSkyDome.valid())
      {
         state = mSkyDome.get()->GetOSGNode()->getOrCreateStateSet();
         state->setAttributeAndModes(mSkyDomeShader->GetLightScatterinVP(), attr);
         state->setAttributeAndModes(mSkyDomeShader->GetDomeFP(), attr);
      }
   }

   Repaint();
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetFogDensity(float density)
{
   mFog->setDensity(density);
}

////////////////////////////////////////////////////////////////////////////////
float dtCore::Environment::GetFogDensity()
{
   return mFog->getDensity();
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetDateTime(unsigned yr, unsigned mo, unsigned da,
                                      unsigned hr, unsigned mi, unsigned sc)
{

   mCurrTime.SetTime(yr, mo, da, hr, mi, sc);

   Log::GetInstance().LogMessage(Log::LOG_DEBUG, __FILE__, "Sim time set to:%s",
      (mCurrTime.ToString(dtUtil::DateTime::TimeFormat::CALENDAR_DATE_AND_TIME_FORMAT)).c_str());

   Update(999.99);
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetDateTime(const DateTime& dateTime)
{
   mCurrTime = dateTime;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::GetDateTime(unsigned& yr, unsigned& mo, unsigned& da, unsigned& hr, unsigned& mi, unsigned& sc) const
{
   mCurrTime.GetTime(yr, mo, da, hr, mi, sc);
}

////////////////////////////////////////////////////////////////////////////////
const dtUtil::DateTime& dtCore::Environment::GetDateTime() const
{
   return mCurrTime;
}

////////////////////////////////////////////////////////////////////////////////
dtUtil::DateTime& dtCore::Environment::GetDateTime()
{
   return mCurrTime;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::Update(const double deltaFrameTime)
{
   mLastUpdate += deltaFrameTime;

   if (mLastUpdate > 1.0)
   {
      mLastUpdate = 0.0;
      GetSunPos(mCurrTime.GetGMTTime(), mRefLatLong[0], mRefLatLong[1], 1.0, &mSunAltitude, &mSunAzimuth);
      UpdateSkyLight();
      UpdateSunColor();
      UpdateEnvColors();
      UpdateFogColor();
      Repaint();
   }

   if (GetFogMode() == Environment::ADV)
   {
      UpdateShaders();
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::UpdateEnvColors()
{
   float skyBright = mSkyLightTable->Interpolate(mSunAltitude);

   // Modify the sky color
   mModSkyColor = mSkyColor * skyBright;

   // Modify the fog color based on sky brightness
   mModFogColor = mFogColor * skyBright;
}

////////////////////////////////////////////////////////////////////////////////

Environment::InterpTable::InterpTable()
   : mSize(0)
{
}

////////////////////////////////////////////////////////////////////////////////
Environment::InterpTable::~InterpTable()
{
}

////////////////////////////////////////////////////////////////////////////////
void Environment::InterpTable::AddEntry(double ind, double dep)
{
   mTable.push_back(TableEntry(ind,dep));
   mSize++;
}

////////////////////////////////////////////////////////////////////////////////
double Environment::InterpTable::Interpolate(double x) const
{
   int i = 0;
   double y;

   if (mSize == 0)
   {
      return (0.0);
   }

   while ((i < mSize) && (x > mTable[i].ind))
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

   y =   mTable[i].dep +
         ((mTable[i-1].dep - mTable[i].dep) *
         (x - mTable[i].ind) ) /
         (mTable[i-1].ind - mTable[i].ind);

   return y;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::UpdateSkyLight()
{
   if (mSkyLight.valid())
   {
      if (InfiniteLight* sun = static_cast<InfiniteLight*>(mSkyLight.get()))
      {
         sun->SetAzimuthElevation(mSunAzimuth, mSunAltitude);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::UpdateFogColor()
{
   // Calculate the fog color in the direction of the sun for
   // sunrise/sunset effects.
   float red   = (mModFogColor[0] + 2.f * mSunColor[0] * mSunColor[0]) / 3.f;
   float green = (mModFogColor[1] + 2.f * mSunColor[1] * mSunColor[1]) / 3.f;
   float blue  = (mModFogColor[2] + 2.f * mSunColor[2]) / 3.f;

   // interpolate between the sunrise/sunset color and the color
   // at the opposite direction of this effect. Take in account
   // the current visibility.
   float vis = GetVisibility();

   const float MAX_VISIBILITY = 20000;

   // Clamp visibility
   if (vis > MAX_VISIBILITY)
   {
      vis = MAX_VISIBILITY;
   }

   double sunRotation = osg::DegreesToRadians(-95.0);
   double heading     = osg::DegreesToRadians(-95.0);

   double rotation = -(sunRotation + osg::PI) - heading;

   float inverseVis = 1.f - (MAX_VISIBILITY - vis) / MAX_VISIBILITY;
   float sif = 0.5f - cosf(osg::DegreesToRadians(mSunAltitude) * 2.f) / 2.f + 0.000001f;

   float rf1  = std::abs((rotation-osg::PI) / osg::PI); // difference between eyepoint heading and sun heading (rad)
   float rf2 = inverseVis * pow(rf1 * rf1, 1.0f / sif);

   float rf3  = 1.f - rf2;

   mModFogColor[0] = rf3 * mModFogColor[0] + rf2 * red;
   mModFogColor[1] = rf3 * mModFogColor[1] + rf2 * green;
   mModFogColor[2] = rf3 * mModFogColor[2] + rf2 * blue;

   // now apply the fog's color
   mFog->setColor(osg::Vec4(mModFogColor[0], mModFogColor[1], mModFogColor[2], 1.f));
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::UpdateSunColor()
{
   // magic light color code borrowed from osgEphemeris, courtesy of Don Burns

   double red   = mSunAltitude * 0.5;
   double green = mSunAltitude * 0.25;
   double blue  = mSunAltitude * 0.125;

   dtUtil::Clamp<double>(red,   0.0, 1.0);
   dtUtil::Clamp<double>(green, 0.0, 1.0);
   dtUtil::Clamp<double>(blue,  0.0, 1.0);
   osg::Vec3 diff(red, green, blue);

   red   = (mSunAltitude + 10.0) * 0.04;
   green = (mSunAltitude + 10.0) * 0.02;
   blue  = (mSunAltitude + 10.0) * 0.01;
   dtUtil::Clamp<double>(red,   0.01, 0.3);
   dtUtil::Clamp<double>(green, 0.01, 0.3);
   dtUtil::Clamp<double>(blue,  0.01, 0.3);
   osg::Vec3 amb(red, green, blue);

   mSunColor = diff;

   if (mSkyLight.valid())
   {
      mSkyLight->SetDiffuse(diff[0], diff[1], diff[2], 1.f);
      mSkyLight->SetAmbient(amb[0],  amb[1],  amb[2],  1.f);
   }

}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::UpdateShaders()
{
   osg::Vec2 sunDir;
   GetSunAzEl(sunDir[0], sunDir[1]);

   Camera* cam  = Camera::GetInstance(0);
   Transform camXform;
   cam->GetTransform(camXform);
   osg::Vec3 xyz;
   camXform.GetTranslation(xyz);

   mSunlightShader->Update(sunDir,
                           xyz, mAdvFogCtrl[0], mAdvFogCtrl[1],
                           mAdvFogCtrl[2] * 10.0e25);

   mSkyDomeShader->Update(sunDir,
                          mAdvFogCtrl[0], mAdvFogCtrl[1],
                          mAdvFogCtrl[2] * 10.0e25);
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetRefLatLong(const osg::Vec2& latLong)
{
   mRefLatLong = latLong;
}

////////////////////////////////////////////////////////////////////////////////
void dtCore::Environment::SetOSGNode(osg::Node* pNode)
{
   mNode = pNode;
}

////////////////////////////////////////////////////////////////////////////////
