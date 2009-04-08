/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#ifndef DELTA_OBJECT_VIEWER
#define DELTA_OBJECT_VIEWER

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/object.h>
#include <dtCore/compass.h>

#include <dtABC/application.h>

#include <dtDAL/actorproxy.h>

#include <dtAnim/posemeshdatabase.h>
#include <dtAnim/posemeshutility.h>
#include <dtAnim/posemesh.h>
#include <dtAnim/attachmentcontroller.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////

class QColor;

struct LightInfo;

namespace dtCore
{
   class ObjectMotionModel;
   class OrbitMotionModel;
   class Light;
}

namespace osg
{
   class Group;
}

namespace dtAnim
{
   class CharDrawable;
   class Cal3DDatabase;
   class PoseMeshDatabase;
}

///////////////////////////////////////////////////////////////////////////////

class ObjectViewer: public QObject, public dtABC::Application
{
   Q_OBJECT

public:

   ObjectViewer();
   ~ObjectViewer();

   virtual void Config();

public slots:

   void OnLoadShaderFile(const QString& filename);
   void OnLoadMapFile(const std::string& filename);
   void OnLoadGeometryFile(const std::string& filename);
   void OnUnloadGeometryFile();
   void OnApplyShader(const std::string& groupName, const std::string& programName);
   void OnRemoveShader();
   void OnSetShaded();
   void OnSetWireframe();
   void OnSetShadedWireframe();
   void OnSetGenerateTangentAttribute(bool shouldGenerate);
   void OnToggleGrid(bool shouldDisplay);

   // Lighting slots
   void OnAddLight(int id);
   void OnFixLights();
   void OnSetCurrentLight(int id);
   void OnSetLightEnabled(int id, bool enabled);
   void OnSetLightType(int id, int type);
   void OnSetLightPosition(int id, const osg::Vec3& position);
   void OnSetLightRotation(int id, const osg::Vec3& rotation);
   void OnSetAmbient(int id, const osg::Vec4& color);
   void OnSetDiffuse(int id, const osg::Vec4& color);
   void OnSetSpecular(int id, const osg::Vec4& color);
   void OnSetLightCutoff(int id, float cutoff);
   void OnSetLightExponent(int id, float exponent);
   void OnSetLightConstant(int id, float constant);
   void OnSetLightLinear(int id, float linear);
   void OnSetLightQuadratic(int id, float quadratic);

   // Usage mode slots
   void OnEnterObjectMode();
   void OnEnterLightMode();

   // Editing mode slots
   void OnWorldSpaceMode();
   void OnLocalSpaceMode();

signals:

   void ShaderLoaded(const std::string& filename, const std::string& shaderGroup, const std::string& shaderName);
   void ErrorOccured(const QString& msg);
   void LightUpdate(const LightInfo& lightInfo);

protected:
   void InitWireDecorator();
   void InitGridPlanes();
   void InitLights();
   void ReInitLights();

   void ClearLights();

   void clearProxies( const std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::ActorProxy> >& proxies);

   virtual void PostFrame(const double deltaFrameTime);

   void GenerateTangentsForObject(dtCore::Object* object);

private:

   dtCore::RefPtr<dtDAL::Map>      mMap;
   dtCore::RefPtr<dtCore::Object>  mObject;
   dtCore::RefPtr<dtCore::Compass> mCompass;

   dtCore::RefPtr<dtCore::OrbitMotionModel> mModelMotion;
   std::vector<dtCore::RefPtr<dtCore::ObjectMotionModel> > mLightMotion;

   dtCore::RefPtr<osg::Group> mShadedScene;
   dtCore::RefPtr<osg::Group> mUnShadedScene;
   dtCore::RefPtr<osg::Group> mWireDecorator;
   dtCore::RefPtr<osg::Group> mShadeDecorator;
   dtCore::RefPtr<osg::Geode> mGridGeode;

   std::vector<dtCore::RefPtr<dtCore::Transformable> > mLightArrowTransformable;
   std::vector<dtCore::RefPtr<dtCore::Object> >        mLightArrow;

   int   mCurrentLight;
   bool  mShouldGenerateTangents;
   //float mLightScale;
};

#endif // DELTA_OBJECT_VIEWER
