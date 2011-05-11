/// Included above ObjectViewer.h to avoid a compile error in linux
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#include "ObjectViewer.h"
#include "ObjectViewerData.h"

#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/objectmotionmodel.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/light.h>
#include <dtCore/infinitelight.h>
#include <dtCore/positionallight.h>
#include <dtCore/spotlight.h>
#include <dtCore/deltawin.h>
#include <dtCore/shadermanager.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/geometrycollector.h>
#include <dtUtil/librarysharingmanager.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>

#include <dtAnim/chardrawable.h>
#include <dtAnim/cal3ddatabase.h>

#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osgUtil/TangentSpaceGenerator>

#include <osgViewer/CompositeViewer>

#include <assert.h>

////////////////////////////////////////////////////////////////////////////////
ObjectViewer::ObjectViewer()
   : mCurrentLight(0)
   , mShouldGenerateTangents(true)
{
   mShadedScene   = new osg::Group;
   mUnShadedScene = new osg::Group;
}

////////////////////////////////////////////////////////////////////////////////
ObjectViewer::~ObjectViewer()
{
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::Config()
{
   dtABC::Application::Config();
   GetCompositeViewer()->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

   std::string exampleDataPath = dtUtil::GetEnvironment("DELTA_ROOT");
   std::string rootDataPath    = dtUtil::GetEnvironment("DELTA_DATA");
   exampleDataPath += ";" + rootDataPath;

   dtUtil::SetDataFilePathList(dtUtil::GetDataFilePathList() + ";" + exampleDataPath);

   //adjust the Camera position
   dtCore::Transform camPos;
   osg::Vec3 camXYZ(0.f, -5.f, 1.f);
   osg::Vec3 lookAtXYZ (0.f, 0.f, 1.f);
   osg::Vec3 upVec (0.f, 0.f, 1.f);
   camPos.Set(camXYZ, lookAtXYZ, upVec);

   GetCamera()->SetTransform(camPos);
   GetCamera()->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

   double vfov, aspect, nearClip, farClip;
   GetCamera()->GetPerspectiveParams(vfov, aspect, nearClip, farClip);
   GetCamera()->SetPerspectiveParams(vfov, aspect, 0.1, farClip);

   // Add the compass (3d basis axes) to the bottom left of the screen
   mCompass = new dtCore::Compass(GetCamera());
   GetScene()->GetSceneNode()->addChild(mCompass->GetOSGNode());

   mModelMotion = new dtCore::OrbitMotionModel(GetKeyboard(), GetMouse());
   mModelMotion->SetTarget(GetCamera());
   mModelMotion->SetDistance(5.f);

   mWireDecorator  = new osg::Group;
   mShadeDecorator = new osg::Group;

   InitLights();

   GetScene()->GetSceneNode()->addChild(mShadedScene.get());
   GetScene()->GetSceneNode()->addChild(mUnShadedScene.get());

   InitWireDecorator();
   InitGridPlanes();

   OnSetShaded();
   OnToggleGrid(true);

   OnEnterObjectMode();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadShaderFile(const QString& filename)
{
   try
   {
      dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();

      // Since the shader manager cannot deal with duplicate shader names,
      // we clear it out before we load each file.  This means that in order
      // to reference shaders later, the file just be reloaded.
      shaderManager.Clear();

      shaderManager.LoadShaderDefinitions(filename.toStdString());

      std::vector<dtCore::RefPtr<dtCore::ShaderGroup> > shaderGroupList;
      shaderManager.GetAllShaderGroupPrototypes(shaderGroupList);

      // Emit all shader groups and their individual shaders
      for (size_t groupIndex = 0; groupIndex < shaderGroupList.size(); ++groupIndex)
      {
         std::vector<dtCore::RefPtr<dtCore::ShaderProgram> > programList;
         shaderGroupList[groupIndex]->GetAllShaders(programList);

         const std::string& groupName = shaderGroupList[groupIndex]->GetName();

         for (size_t programIndex = 0; programIndex < programList.size(); ++programIndex)
         {
            emit ShaderLoaded(filename.toStdString(), groupName, programList[programIndex]->GetName());
         }
      }
   }
   catch (dtUtil::Exception& e)
   {
      QMessageBox::critical(NULL, "Error", e.ToString().c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadMapFile(const std::string& filename)
{
   ClearLights();

   OnUnloadGeometryFile();

   dtCore::RefPtr<dtDAL::Map> map;

   // Attempt to open the map.
   try
   {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      // We'll be creating the scene by hand using decorators
      // and since the load function needs a scene, give it temp
      dtCore::RefPtr<dtCore::Scene> dummy = new dtCore::Scene;
      map = &dtDAL::Project::GetInstance().LoadMapIntoScene(filename, *dummy);
   }
   catch (dtUtil::Exception &e)
   {
      QApplication::restoreOverrideCursor();
      QString error = "An error occured while opening the map. ";
      error += e.What().c_str();
      LOG_ERROR(error.toStdString());
      QMessageBox::critical(NULL, tr("Map Open Error"),error,tr("OK"));
      return;
   }

   // Load the new map into the current scene.
   if (map.valid())
   {
      const std::vector<std::string>& missingLibs= map->GetMissingLibraries();

      if (!missingLibs.empty())
      {
         QString
            errors(tr("The following libraries listed in the map could not be loaded:\n\n"));
         for (unsigned i = 0; i < missingLibs.size(); ++i)
         {
            std::string
               nativeName = dtUtil::LibrarySharingManager::GetPlatformSpecificLibraryName(missingLibs[i]);
            errors.append(nativeName.c_str());
            errors.append("\n");
         }

         errors.append("\nThis could happen for a number of reasons. Please ensure that the name is correct, ");
         errors.append("the library is in the path (or the working directory), the library can load correctly, and dependent libraries are available.");
         errors.append("If you save this map, the library and any actors referenced by the library will be lost.");

         QApplication::restoreOverrideCursor();
         QMessageBox::warning(NULL, tr("Missing Libraries"), errors, tr("OK"));
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
      }
   }

   QApplication::restoreOverrideCursor();
   mMap = map;

   if (mMap.valid())
   {
      mMap->SetModified(false);

      InitLights();

      bool bFirstBoundSet = false;
      osg::Vec3 minBounds;
      osg::Vec3 maxBounds;

      const std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::BaseActorObject> >& proxies =
         mMap->GetAllProxies();

      std::map<dtCore::UniqueId,dtCore::RefPtr<dtDAL::BaseActorObject> >::const_iterator itor;

      for (itor = proxies.begin(); itor != proxies.end(); ++itor)
      {
         dtDAL::BaseActorObject *proxy = const_cast<dtDAL::BaseActorObject*>(itor->second.get());

         dtCore::DeltaDrawable* drawable = proxy->GetActor();
         if (drawable)
         {
            mShadeDecorator->addChild(drawable->GetOSGNode());
            mWireDecorator->addChild(drawable->GetOSGNode());

            // Get the min and max bounding area.
            osg::Vec3 center;
            float radius;
            drawable->GetBoundingSphere(center, radius);

            if (bFirstBoundSet)
            {
               for (int axis = 0; axis < 3; axis++)
               {
                  if (minBounds[axis] > center[axis] - radius)
                  {
                     minBounds[axis] = center[axis] - radius;
                  }

                  if (maxBounds[axis] < center[axis] + radius)
                  {
                     maxBounds[axis] = center[axis] + radius;
                  }
               }
            }
            else
            {
               for (int axis = 0; axis < 3; axis++)
               {
                  minBounds[axis] = center[axis] - radius;
                  maxBounds[axis] = center[axis] + radius;
               }
               bFirstBoundSet = true;
            }
         }
      }

      // Now update the camera positions.
      if (bFirstBoundSet)
      {
         osg::Vec3 center;
         float radius = 0.0f;

         for (int axis = 0; axis < 3; axis++)
         {
            center[axis] = minBounds[axis] + ((maxBounds[axis] - minBounds[axis]) * 0.5f);

            float testRadius = (maxBounds[axis] - minBounds[axis]) * 0.5f;
            if (testRadius > radius)
            {
               radius = testRadius;
            }
         }

         // Reset the camera outside the bounding sphere.
         mModelMotion->SetDistance(radius * 2.0f);
         mModelMotion->SetFocalPoint(center);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadGeometryFile(const std::string& filename)
{
   OnUnloadGeometryFile();

   QString qtFilename(filename.c_str());

   // If this is a static mesh
   if (!qtFilename.endsWith(".dtMap") && !qtFilename.endsWith(".dtChar"))
   {
      mObject = new dtCore::Object;
      mObject->LoadFile(filename);

      // set up the ObjectViewer's scene graph
      mShadeDecorator->addChild(mObject->GetOSGNode());
      mWireDecorator->addChild(mObject->GetOSGNode());

      osg::Vec3 center;

      if (mObject.valid())
      {
         float radius;
         mObject->GetBoundingSphere(center, radius);

         // Reset the camera outside the bounding sphere.
         mModelMotion->SetDistance(radius * 2.0f);
         mModelMotion->SetFocalPoint(center);

         if (mShouldGenerateTangents)
         {
            GenerateTangentsForObject(mObject.get());
         }
      }
   }
   else
   {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      dtCore::RefPtr<dtAnim::Cal3DModelWrapper> wrapper =
         dtAnim::Cal3DDatabase::GetInstance().Load(filename);

      if (wrapper.valid())
      {
         mCharacter = new dtAnim::CharDrawable(wrapper.get());

         // set up the ObjectViewer's scene graph
         mShadeDecorator->addChild(mCharacter->GetOSGNode());
         mWireDecorator->addChild(mCharacter->GetOSGNode());
      }
      else
      {
         emit ErrorOccured(QString("Unable to load %1").arg(filename.c_str()));
      }

      QApplication::restoreOverrideCursor();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnUnloadGeometryFile()
{
   if (mObject.valid())
   {
      mShadeDecorator->removeChild(mObject->GetOSGNode());
      mWireDecorator->removeChild(mObject->GetOSGNode());

      mObject = NULL;
   }

   if (mCharacter.valid())
   {
      mShadeDecorator->removeChild(mCharacter->GetOSGNode());
      mWireDecorator->removeChild(mCharacter->GetOSGNode());

      mCharacter = NULL;
   }

   // Remove the old map.
   if (mMap.valid())
   {
      // Remove all proxies from the scene.
      clearProxies(mMap->GetAllProxies());

      try
      {
         dtDAL::Project::GetInstance().CloseMap(*mMap, true);
         mMap = NULL;
      }
      catch (const dtUtil::Exception &e)
      {
         QMessageBox::critical(NULL, tr("Error"), e.What().c_str(), tr("OK"));
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnApplyShader(const std::string& groupName, const std::string& programName)
{
   dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();

   dtCore::ShaderProgram* program = shaderManager.FindShaderPrototype(programName, groupName);
   assert(program);

   try
   {
      dtCore::ShaderProgram* deltaProgram = shaderManager.AssignShaderFromPrototype(*program, *mShadedScene);
      osg::Program* osgProgram = deltaProgram->GetShaderProgram();

      // Bind the default location for tangents
      osgProgram->addBindAttribLocation("tangentAttrib", 6);
   }
   catch (const dtUtil::Exception &e)
   {
      LOG_ALWAYS(e.ToString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnRemoveShader()
{
   dtCore::ShaderManager &shaderManager = dtCore::ShaderManager::GetInstance();
   shaderManager.UnassignShaderFromNode(*mShadedScene);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetShaded()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mShadeDecorator.get());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetWireframe()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mWireDecorator.get());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetShadedWireframe()
{
   mShadedScene->removeChild(mWireDecorator.get());
   mShadedScene->removeChild(mShadeDecorator.get());

   mShadedScene->addChild(mWireDecorator.get());
   mShadedScene->addChild(mShadeDecorator.get());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetGenerateTangentAttribute(bool shouldGenerate)
{
   mShouldGenerateTangents = shouldGenerate;
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnToggleGrid(bool shouldDisplay)
{
   if (shouldDisplay)
   {
      mUnShadedScene->addChild(mGridGeode.get());
   }
   else
   {
      mUnShadedScene->removeChild(mGridGeode.get());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnFixLights()
{
   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; lightIndex++)
   {
      dtCore::Light* light = GetScene()->GetLight(lightIndex);

      if (!light)
      {
         OnSetLightType(lightIndex, 0);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetCurrentLight(int id)
{
   mCurrentLight = id;

   if (!mModelMotion->IsEnabled())
   {
      for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; lightIndex++)
      {
         if (id != lightIndex)
         {
            mLightMotion[lightIndex]->SetEnabled(false);
         }
         else
         {
            mLightMotion[lightIndex]->SetEnabled(true);
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightEnabled(int id, bool enabled)
{
   dtCore::Light* light = GetScene()->GetLight(id);
   light->SetEnabled(enabled);
   mLightArrow[id]->SetActive(enabled);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightType(int id, int type)
{
   QString lightName = QString("Light%1").arg(id);

   dtCore::Light* light = GetScene()->GetLight(id);
   dtCore::Light* newLight = NULL;

   switch (type)
   {
   case 0: // Infinite
      {
         newLight = new dtCore::InfiniteLight(id, lightName.toStdString());
         //newLight->GetLightSource()->getLight()->setPosition(osg::Vec4(-osg::Y_AXIS, 0.0f));
         break;
      }

   case 1: // Positional
      newLight = new dtCore::PositionalLight(id, lightName.toStdString());
      break;

   case 2: // Spot
      newLight = new dtCore::SpotLight(id, lightName.toStdString());
      break;
   }

   // Copy the previous light data to the new light.
   if (newLight)
   {
      if (light)
      {
         newLight->SetEnabled(light->GetEnabled());
         newLight->SetAmbient(light->GetAmbient());
         newLight->SetDiffuse(light->GetDiffuse());
         newLight->SetSpecular(light->GetSpecular());
      }
      else
      {
         if (id == 0)
         {
            newLight->SetEnabled(true);
         }
         else
         {
            newLight->SetEnabled(false);
         }
      }

      // Remove the current light from the transform.
      dtCore::RefPtr<dtCore::Transformable> lightArrowTransformable = mLightArrowTransformable[id];
      for (int childIndex = 0; childIndex < (int)lightArrowTransformable->GetNumChildren(); childIndex++)
      {
         dtCore::DeltaDrawable* drawable = lightArrowTransformable->GetChild(childIndex);
         if (drawable && dynamic_cast<dtCore::Light*>(drawable))
         {
            lightArrowTransformable->RemoveChild(drawable);
            break;
         }
      }

      lightArrowTransformable->AddChild(newLight);

      dtCore::RefPtr<dtCore::Object> lightArrow = mLightArrow[id];
      if (lightArrow.valid())
      {
         dtCore::Transform transform;

         if (type == 0)
         {
            // Infinite lights are pointing down the Z axis.
            transform.SetRotation(osg::Matrix::rotate(osg::DegreesToRadians(-90.0f), osg::X_AXIS));
         }

         lightArrow->SetTransform(transform, dtCore::Transformable::REL_CS);
      }

      GetScene()->RegisterLight(newLight);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightPosition(int id, const osg::Vec3& position)
{
   dtCore::Transform transform;
   mLightArrowTransformable[id]->GetTransform(transform);
   transform.SetTranslation(position);
   mLightArrowTransformable[id]->SetTransform(transform);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightRotation(int id, const osg::Vec3& rotation)
{
   dtCore::Transform transform;
   mLightArrowTransformable[id]->GetTransform(transform);
   transform.SetRotation(rotation);
   mLightArrowTransformable[id]->SetTransform(transform);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetAmbient(int id, const osg::Vec4& color)
{
   dtCore::Light* light = GetScene()->GetLight(id);
   light->SetAmbient(color);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetDiffuse(int id, const osg::Vec4& color)
{
   dtCore::Light* light = GetScene()->GetLight(id);
   light->SetDiffuse(color);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetSpecular(int id, const osg::Vec4& color)
{
   dtCore::Light* light = GetScene()->GetLight(id);
   light->SetSpecular(color);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightCutoff(int id, float cutoff)
{
   dtCore::SpotLight* light = dynamic_cast<dtCore::SpotLight*>(GetScene()->GetLight(id));
   if (light)
   {
      light->SetSpotCutoff(cutoff);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightExponent(int id, float exponent)
{
   dtCore::SpotLight* light = dynamic_cast<dtCore::SpotLight*>(GetScene()->GetLight(id));
   if (light)
   {
      light->SetSpotExponent(exponent);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightConstant(int id, float constant)
{
   dtCore::PositionalLight* light = dynamic_cast<dtCore::PositionalLight*>(GetScene()->GetLight(id));
   if (light)
   {
      osg::Vec3 attenuation = light->GetAttenuation();
      attenuation[0] = constant;
      light->SetAttenuation(attenuation);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightLinear(int id, float linear)
{
   dtCore::PositionalLight* light = dynamic_cast<dtCore::PositionalLight*>(GetScene()->GetLight(id));
   if (light)
   {
      osg::Vec3 attenuation = light->GetAttenuation();
      attenuation[1] = linear;
      light->SetAttenuation(attenuation);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightQuadratic(int id, float quadratic)
{
   dtCore::PositionalLight* light = dynamic_cast<dtCore::PositionalLight*>(GetScene()->GetLight(id));
   if (light)
   {
      osg::Vec3 attenuation = light->GetAttenuation();
      attenuation[2] = quadratic;
      light->SetAttenuation(attenuation);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnEnterObjectMode()
{
   mModelMotion->SetEnabled(true);

   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; lightIndex++)
   {
      mLightMotion[lightIndex]->SetEnabled(false);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnEnterLightMode()
{
   mModelMotion->SetEnabled(false);

   if (mCurrentLight > -1 && mCurrentLight < dtCore::MAX_LIGHTS)
   {
      mLightMotion[mCurrentLight]->SetEnabled(true);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnWorldSpaceMode()
{
   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; lightIndex++)
   {
      mLightMotion[lightIndex]->SetCoordinateSpace(dtCore::ObjectMotionModel::WORLD_SPACE);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLocalSpaceMode()
{
   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; lightIndex++)
   {
      mLightMotion[lightIndex]->SetCoordinateSpace(dtCore::ObjectMotionModel::LOCAL_SPACE);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::InitWireDecorator()
{
   osg::StateSet* stateset = new osg::StateSet;
   osg::PolygonOffset* polyOffset = new osg::PolygonOffset;
   polyOffset->setFactor(-1.0f);
   polyOffset->setUnits(-1.0f);

   osg::PolygonMode* polyMode = new osg::PolygonMode;
   polyMode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
   stateset->setAttributeAndModes(polyOffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   stateset->setAttributeAndModes(polyMode, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);

   osg::Material* material = new osg::Material;
   stateset->setAttributeAndModes(material, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
   stateset->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);

   stateset->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF );

   mWireDecorator->setStateSet(stateset);

   stateset->setMode(GL_BLEND,osg::StateAttribute::OVERRIDE |
         osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::InitGridPlanes()
{
   const int GRID_LINE_COUNT = 49;
   const float GRID_LINE_SPACING = 2.0f;

   const int numVerts(2 * 2 * GRID_LINE_COUNT);
   const float length(((GRID_LINE_COUNT - 1) * GRID_LINE_SPACING) / 2.f);

   osg::Vec3 verts[numVerts];
   int indx(0L);

   for (int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      verts[indx++].set(-length + ii * GRID_LINE_SPACING,  length, 0.0f);
      verts[indx++].set(-length + ii * GRID_LINE_SPACING, -length, 0.0f);
   }

   for (int ii(0L); ii < GRID_LINE_COUNT; ii++ )
   {
      verts[indx++].set(length, -length + ii * GRID_LINE_SPACING, 0.0f);
      verts[indx++].set(-length, -length + ii * GRID_LINE_SPACING, 0.0f);
   }

   osg::Geometry* geometry = new osg::Geometry;
   assert(geometry);

   geometry->setVertexArray(new osg::Vec3Array(numVerts, verts));
   geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numVerts));

   assert(!mGridGeode.valid());
   mGridGeode = new osg::Geode;

   mGridGeode->addDrawable(geometry);
   mGridGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, 0);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::InitLights()
{
   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; ++lightIndex)
   {
      dtCore::RefPtr<dtCore::Light> light = GetScene()->GetLight(lightIndex);

      if (!light.valid())
      {
         QString lightName = QString("Light%1").arg(lightIndex);

         light = new dtCore::InfiniteLight(lightIndex, lightName.toStdString());
         GetScene()->RegisterLight(light);
         light->SetEnabled(false);

         light->SetAmbient(0.2f, 0.2f, 0.2f, 1.f);
         light->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
         light->SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);

         // Infinite lights must start here, point light from the positive y axis
         //light->GetLightSource()->getLight()->setPosition(osg::Vec4(-osg::Y_AXIS, 0.0f));
      }

      bool enabled = light->GetEnabled();

      dtCore::RefPtr<dtCore::Object> lightArrow = new dtCore::Object;
      lightArrow->LoadFile("examples/data/models/LightArrow.ive");
      lightArrow->SetActive(enabled);

      dtCore::Transform transform;

      // Infinite lights point in a different direction than others.
      if (dynamic_cast<dtCore::InfiniteLight*>(light.get()))
      {
         transform.SetRotation(osg::Matrix::rotate(osg::DegreesToRadians(-90.0f), osg::X_AXIS));
         lightArrow->SetTransform(transform);
      }

      dtCore::RefPtr<dtCore::Transformable> lightArrowTransformable = new dtCore::Transformable;
      lightArrowTransformable->AddChild(lightArrow.get());
      light->Emancipate();
      lightArrowTransformable->AddChild(light);

      // Copy the transform from the light to the attached transformable.
      light->GetTransform(transform);
      light->SetTransform(dtCore::Transform());
      lightArrowTransformable->SetTransform(transform);

      dtCore::RefPtr<dtCore::ObjectMotionModel> lightMotion = new dtCore::ObjectMotionModel(GetView());
      lightMotion->SetEnabled(false);
      lightMotion->SetTarget(lightArrowTransformable.get());
      lightMotion->SetScale(0.5f);

      GetScene()->AddChild(lightArrowTransformable.get());

      mLightMotion.push_back(lightMotion);
      mLightArrowTransformable.push_back(lightArrowTransformable);
      mLightArrow.push_back(lightArrow);
      light->SetEnabled(enabled);
   }

   // The built in scene skylight has problems with being updated
   // by the motion model so reset it with a custom one here.
   OnSetLightType(0, 0);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::ReInitLights()
{
   for (int lightIndex = 0; lightIndex < dtCore::MAX_LIGHTS; ++lightIndex)
   {
      dtCore::Light* light = GetScene()->GetLight(lightIndex);

      if (!light)
      {
         continue;
      }

      bool enabled = light->GetEnabled();
      dtCore::RefPtr<dtCore::Transformable> lightArrowTransformable = mLightArrowTransformable[lightIndex];

      // Copy the transform from the light to the attached transformable.
      dtCore::Transform transform;
      light->GetTransform(transform);
      light->SetTransform(dtCore::Transform());
      lightArrowTransformable->SetTransform(transform);

      dtCore::RefPtr<dtCore::ObjectMotionModel> lightMotion = mLightMotion[lightIndex];
      lightMotion->SetEnabled(false);

      light->SetEnabled(enabled);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::ClearLights()
{
   for (int lightIndex = 0; lightIndex < (int)mLightMotion.size(); lightIndex++)
   {
      mLightMotion[lightIndex]->SetEnabled(false);
   }

   for (int lightIndex = 0; lightIndex < (int)mLightArrowTransformable.size(); lightIndex++)
   {
      GetScene()->RemoveChild(mLightArrowTransformable[lightIndex].get());
   }

   mLightMotion.clear();
   mLightArrowTransformable.clear();
   mLightArrow.clear();
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::clearProxies(const std::map<dtCore::UniqueId, dtCore::RefPtr<dtDAL::BaseActorObject> >& proxies)
{
   std::map<dtCore::UniqueId,dtCore::RefPtr<dtDAL::BaseActorObject> >::const_iterator itor;

   for (itor = proxies.begin(); itor != proxies.end(); ++itor)
   {
      dtDAL::BaseActorObject* proxy = const_cast<dtDAL::BaseActorObject*>(itor->second.get());

      dtCore::DeltaDrawable* drawable = proxy->GetActor();
      if (drawable)
      {
         mShadeDecorator->removeChild(drawable->GetOSGNode());
         mWireDecorator->removeChild(drawable->GetOSGNode());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::PostFrame(const double)
{
   // Broadcast the current state of all the lights in the scene
   for (int lightIndex = 0; lightIndex < (int)mLightMotion.size(); ++lightIndex)
   {
      // Update the scale of the light arrow to match the current scale of the motion model.
      float lightScale = mLightMotion[lightIndex]->GetAutoScaleSize();
      mLightArrow[lightIndex]->SetScale(osg::Vec3(lightScale, lightScale, lightScale));

      dtCore::Light* light = GetScene()->GetLight(lightIndex);
      if (light)
      {
         dtCore::Transform arrowTransform;
         mLightArrowTransformable[lightIndex]->GetTransform(arrowTransform);

         LightInfo lightInfo;
         lightInfo.light = light;
         lightInfo.transform = &arrowTransform;

         emit LightUpdate(lightInfo);
         continue;
      }

      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::GenerateTangentsForObject(dtCore::Object* object)
{
   // Get all geometry in the graph to apply the shader to
   osg::ref_ptr<dtUtil::GeometryCollector> geomCollector = new dtUtil::GeometryCollector;
   object->GetOSGNode()->accept(*geomCollector);

   // Calculate tangent vectors for all faces and store them as vertex attributes
   for (size_t geomIndex = 0; geomIndex < geomCollector->mGeomList.size(); ++geomIndex)
   {
      osg::Geometry* geom = geomCollector->mGeomList[geomIndex];

      // Force display lists to OFF and VBO's to ON so that vertex
      // attributes can be set without disturbing the graphics driver
      geom->setSupportsDisplayList(false);
      geom->setUseDisplayList(false);
      geom->setUseVertexBufferObjects(true);

      osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
      tsg->generate(geom, 0);

      if (!geom->getVertexAttribArray(6))
      {
         geom->setVertexAttribData(6, osg::Geometry::ArrayData(tsg->getTangentArray(), osg::Geometry::BIND_PER_VERTEX, GL_FALSE));
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnNextStatistics()
{
   SetNextStatisticsType();
}

