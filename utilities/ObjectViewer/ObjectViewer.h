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

#ifndef DELTA_OBJECT_VIEWER_H
#define DELTA_OBJECT_VIEWER_H

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/object.h>
#include <dtCore/compass.h>

#include <dtABC/application.h>
#include <dtCore/actorproxy.h>

#include <dtQt/nodetreepanel.h>

#include <vector>

#include "MotionModelToolbar.h"
#include "NodeHighlighter.h"
#include "Typedefs.h"

///////////////////////////////////////////////////////////////////////////////

struct LightInfo;
class QColor;

namespace dtCore
{
   class ObjectMotionModel;
   class MotionModel;
   class Light;
}

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
}
/// @endcond

namespace dtAnim
{
   class CharDrawable;
}



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class ObjectViewer: public QObject, public dtABC::Application
{
   Q_OBJECT

public:

   ObjectViewer();
   ~ObjectViewer();

   virtual void Config();

   void ResetMotionModel(float radius, const osg::Vec3& center);

   dtCore::Object* GetDeltaObject() {return mObject.get();}

public slots:

   void OnMotionModelSpeedChanged(MotionModelTypeE motionModelType, float speed);
   void OnMotionModelSelected(MotionModelTypeE motionModelType);

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
   void OnNextStatistics();
   void OnNodesSelected(OsgNodePtrVector nodes);

   // Lighting slots
   void OnFixLights();
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

   // Editing mode slots
   void OnWorldSpaceMode();
   void OnLocalSpaceMode();

signals:

   void SignalShaderApplied(ShaderProgramPtr shaderProgram);
   void ShaderLoaded(const std::string& filename, const std::string& shaderGroup, const std::string& shaderName);
   void ErrorOccured(const QString& msg);
   void LightUpdate(const LightInfo& lightInfo);

protected:
   void CountPrimitives ( osg::Node& mesh, const std::string& filename );
   void InitWireDecorator();
   void InitGridPlanes();
   void InitLights();
   void ReInitLights();

   void ClearLights();

   void clearProxies( const std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >& proxies);

   virtual void EventTraversal(const double deltaSimTime);
   virtual void PostFrame(const double deltaFrameTime);

   void GenerateTangentsForObject(dtCore::Object* object);

private:
   dtCore::RefPtr<dtCore::Map> mMap;
   dtCore::RefPtr<dtCore::Object> mObject;
   dtCore::RefPtr<dtAnim::CharDrawable> mCharacter;
   dtCore::RefPtr<dtCore::Compass> mCompass;
   dtCore::RefPtr<NodeHighlighter> mNodeHighlighter;

   dtCore::RefPtr<dtCore::MotionModel> mMotionModel;
   std::vector<dtCore::RefPtr<dtCore::ObjectMotionModel> > mLightMotion;

   dtCore::RefPtr<osg::Group> mShadedScene;
   dtCore::RefPtr<osg::Group> mUnShadedScene;
   dtCore::RefPtr<osg::Group> mWireDecorator;
   dtCore::RefPtr<osg::Group> mShadeDecorator;
   dtCore::RefPtr<osg::Geode> mGridGeode;

   std::vector<dtCore::RefPtr<dtCore::Transformable> > mLightArrowTransformable;
   std::vector<dtCore::RefPtr<dtCore::Object> > mLightArrow;

   bool  mShouldGenerateTangents;
};

#endif // DELTA_OBJECT_VIEWER_H
