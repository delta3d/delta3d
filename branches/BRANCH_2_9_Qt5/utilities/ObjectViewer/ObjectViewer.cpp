/**
 * \file ObjectViewer.cpp
 *
 * \page objectviewer Object Viewer
 *
 * The object viewer allows a user to view OpenSceneGraph mesh files,
 * shaders, and Delta3D maps interactively.
 */

/// Included above ObjectViewer.h to avoid a compile error in linux
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>

#include "ObjectViewer.h"
#include "ObjectViewerData.h"
#include "TextureVisitor.h"

#include <dtCore/system.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/flymotionmodel.h>
#include <dtCore/objectmotionmodel.h>
#include <dtCore/orbitmotionmodel.h>
#include <dtCore/ufomotionmodel.h>
#include <dtCore/light.h>
#include <dtCore/infinitelight.h>
#include <dtCore/positionallight.h>
#include <dtCore/spotlight.h>
#include <dtCore/deltawin.h>
#include <dtCore/shadermanager.h>
#include <dtCore/pointaxis.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/log.h>
#include <dtUtil/exception.h>
#include <dtUtil/geometrycollector.h>
#include <dtUtil/librarysharingmanager.h>

#include <dtCore/project.h>
#include <dtCore/map.h>

#include <dtAnim/chardrawable.h>
#include <dtAnim/modeldatabase.h>

#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/PrimitiveSet>
#include <osgUtil/TangentSpaceGenerator>
#include <osgUtil/Statistics>

#include <osgViewer/CompositeViewer>

#include <assert.h>

#include <osg/AnimationPath>



/**
 * \brief Visitor to count triangles as simply as possible.
 */
class CountTrianglesVisitor : public osg::NodeVisitor
{
public:
    typedef osg::NodeVisitor BaseClass;

    CountTrianglesVisitor() :
        BaseClass ( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
        numTriangles(0),
        numDrawablesSkipped(0),
        numPrimitivesSkipped(0),
        numEmptyPrimitiveSets(0)
    {}

    CountTrianglesVisitor ( TraversalMode tm ) :
        BaseClass ( tm ),
        numTriangles(0),
        numDrawablesSkipped(0),
        numPrimitivesSkipped(0),
        numEmptyPrimitiveSets(0)
    {}

    virtual ~CountTrianglesVisitor() {};

    virtual const char* className() { return "CountTrianglesVisitor"; }

    virtual const char* libraryName() { return "Delta3D Object Viewer"; }

    virtual void apply ( osg::Node &node )
    {
        traverse ( node );
        return;
    }

    virtual void apply ( osg::LOD &node )
    {
        // Count only the nearest (presumably most complex) level of detail.
        const osg::LOD::RangeList& ranges = node.getRangeList();
        // Find which range is closest
        int closeIndex = -1;
        float minDist = FLT_MAX;
        for ( unsigned int i=0; i<ranges.size(); i++ )
        {
            if ( ranges[i].first < minDist )
            {
                minDist = ranges[i].first;
                closeIndex = int(i);
            }
        }

        if ( closeIndex >= 0 )
        {
            traverse ( *( node.getChild ( closeIndex ) ) );
        }
        return;
    }

    virtual void apply ( osg::Geode &node )
    {
        // Get all drawables and count their triangles.
        for ( unsigned int i=0; i<node.getNumDrawables(); i++ )
        {
            const osg::Drawable* d = node.getDrawable ( i );
            const osg::Geometry* geom = d->asGeometry();
            if ( geom != nullptr )
            {
                // Count triangles
                const osg::Geometry::PrimitiveSetList& pList ( geom->getPrimitiveSetList() );
                for ( unsigned int j=0; j<pList.size(); j++ )
                {
                    osg::ref_ptr<const osg::PrimitiveSet> pSet ( geom->getPrimitiveSet ( j ) );
                    GLenum mode ( pSet->getMode() );
                    unsigned int numIndices ( pSet->getNumIndices() );

                    switch ( mode )
                    {
                        // Can't do anything with these types.
                    case osg::PrimitiveSet::POINTS:
                    case osg::PrimitiveSet::LINES:
                    case osg::PrimitiveSet::LINE_STRIP:
                    case osg::PrimitiveSet::LINE_LOOP:
                        numPrimitivesSkipped++;
                        break;

                    case osg::PrimitiveSet::TRIANGLES:
                        if ( numIndices >= 3 )
                        {
                            numTriangles += numIndices/3;
                        }
                        else
                        {
                            numEmptyPrimitiveSets++;
                        }
                        break;

                    case osg::PrimitiveSet::TRIANGLE_STRIP:
                        if ( numIndices >= 3 )
                        {
                            numTriangles += numIndices - 2;
                        }
                        else
                        {
                            numEmptyPrimitiveSets++;
                        }
                        break;

                    case osg::PrimitiveSet::QUADS:
                        if ( numIndices >= 4 )
                        {
                            numTriangles += numIndices / 2;
                        }
                        else
                        {
                            numEmptyPrimitiveSets++;
                        }
                        break;

                    case osg::PrimitiveSet::QUAD_STRIP:
                        if ( numIndices >= 4 )
                        {
                            numTriangles += numIndices - 3;
                        }
                        else
                        {
                            numEmptyPrimitiveSets++;
                        }
                        break;

                    case osg::PrimitiveSet::POLYGON:
                    case osg::PrimitiveSet::TRIANGLE_FAN:
                        if ( numIndices >= 3 )
                        {
                            numTriangles += numIndices - 2;
                        }
                        else
                        {
                            numEmptyPrimitiveSets++;
                        }
                        break;
                    }
                }

            }
            else
            {
                numDrawablesSkipped++;
            }
        }
        traverse ( node );
    }

public:
    unsigned int numTriangles;
    unsigned int numDrawablesSkipped;
    unsigned int numPrimitivesSkipped;
    unsigned int numEmptyPrimitiveSets;
};

////////////////////////////////////////////////////////////////////////////////
ObjectViewer::ObjectViewer()
   : mShouldGenerateTangents(true)
{
   mShadedScene   = new osg::Group;
   mUnShadedScene = new osg::Group;

   mNodeHighlighter = new NodeHighlighter;
   mNodeHighlighter->SetCacheEnabled(true);
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
   osg::Vec3 camXYZ(0.0f, -5.0f, 1.0f);
   osg::Vec3 lookAtXYZ (0.0f, 0.0f, 1.0f);
   osg::Vec3 upVec (0.0f, 0.0f, 1.0f);
   camPos.Set(camXYZ, lookAtXYZ, upVec);

   GetCamera()->SetTransform(camPos);
   GetCamera()->SetNearFarCullingMode(dtCore::Camera::NO_AUTO_NEAR_FAR);

   double vfov, aspect, nearClip, farClip;
   GetCamera()->GetPerspectiveParams(vfov, aspect, nearClip, farClip);
   GetCamera()->SetPerspectiveParams(vfov, aspect, 0.1, farClip);

   // Add the compass (3d basis axes) to the bottom left of the screen
   mCompass = new dtCore::Compass(GetCamera());
   GetScene()->GetSceneNode()->addChild(mCompass->GetOSGNode());

   OnMotionModelSelected(MotionModelType::ORBIT.GetValue());

   mWireDecorator  = new osg::Group;
   mShadeDecorator = new osg::Group;

   InitLights();

   GetScene()->GetSceneNode()->addChild(mShadedScene.get());
   GetScene()->GetSceneNode()->addChild(mUnShadedScene.get());

   InitWireDecorator();
   InitGridPlanes();

   OnSetShaded();
   OnToggleGrid(true);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnMotionModelSpeedChanged(MotionModelTypeE motionModelType, float speed)
{
   if (motionModelType == MM_FLY)
   {
      dtCore::FlyMotionModel* fly
         = dynamic_cast<dtCore::FlyMotionModel*>(mMotionModel.get());
      
      if (fly != nullptr)
      {
         fly->SetMaximumFlySpeed(speed);
      }
   }
   else if (motionModelType == MM_UFO)
   {
      dtCore::UFOMotionModel* ufo
         = dynamic_cast<dtCore::UFOMotionModel*>(mMotionModel.get());
      
      if (ufo != nullptr)
      {
         ufo->SetMaximumFlySpeed(speed);
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnMotionModelSelected(MotionModelTypeE motionModelType)
{
   dtCore::Keyboard* keyboard = GetKeyboard();
   dtCore::Mouse* mouse = GetMouse();

   switch(motionModelType)
   {
   case MM_UFO:
      mMotionModel = new dtCore::UFOMotionModel(keyboard, mouse);
      break;

   case MM_FLY:
      mMotionModel = new dtCore::FlyMotionModel(keyboard, mouse);
      break;

   default: // ORBIT
      dtCore::RefPtr<dtCore::OrbitMotionModel> orbit = new dtCore::OrbitMotionModel(keyboard, mouse);
      orbit->SetDistance(5.0f);
      mMotionModel = orbit.get();
      break;
   }

   mMotionModel->SetTarget(GetCamera());
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::ResetMotionModel(float radius, const osg::Vec3& center)
{
   dtCore::OrbitMotionModel* orbit = dynamic_cast<dtCore::OrbitMotionModel*>(mMotionModel.get());

   if (orbit != nullptr)
   {
      dtCore::Camera* cam = GetCamera();

      dtCore::Transform camPos;
      cam->GetTransform(camPos);
      osg::Vec3 pos(camPos.GetTranslation());

      pos += camPos.GetForwardVector() * radius;

      osg::Vec3 lookAtXYZ = camPos.GetForwardVector() * (radius * 2.0f);
      osg::Vec3 upVec (0.0f, 0.0f, 1.0f);

      camPos.Set(pos, lookAtXYZ, upVec);

      GetCamera()->SetTransform(camPos);

      orbit->SetDistance(radius);
      orbit->SetFocalPoint(center);
   }
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
      QMessageBox::critical(nullptr, "Error", e.ToString().c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadMapFile(const std::string& filename)
{
   ClearLights();

   OnUnloadGeometryFile();

   dtCore::RefPtr<dtCore::Map> map;

   // Attempt to open the map.
   try
   {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      // We'll be creating the scene by hand using decorators
      // and since the load function needs a scene, give it temp
      dtCore::RefPtr<dtCore::Scene> dummy = new dtCore::Scene;
      map = &dtCore::Project::GetInstance().LoadMapIntoScene(filename, *dummy);
   }
   catch (dtUtil::Exception& e)
   {
      QApplication::restoreOverrideCursor();
      QString error = "An error occured while opening the map. ";
      error += e.What().c_str();
      LOG_ERROR(error.toStdString());
      QMessageBox::critical(nullptr, tr("Map Open Error"),error,tr("OK"));
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
         QMessageBox::warning(nullptr, tr("Missing Libraries"), errors, tr("OK"));
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

      const std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >& proxies =
         mMap->GetAllProxies();

      std::map<dtCore::UniqueId,dtCore::RefPtr<dtCore::BaseActorObject> >::const_iterator itor;

      for (itor = proxies.begin(); itor != proxies.end(); ++itor)
      {
         dtCore::BaseActorObject *proxy = const_cast<dtCore::BaseActorObject*>(itor->second.get());

         dtCore::DeltaDrawable* drawable = proxy->GetDrawable();
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
         ResetMotionModel(radius, center);
      }
   }
}

void ObjectViewer::CountPrimitives ( osg::Node& mesh, const std::string& filename )
{
    // Count triangles in model.
    CountTrianglesVisitor visit;
    mesh.accept(visit);

    std::cout << filename << " contains" <<std::endl;
    osgUtil::StatsVisitor statVis;
    (mObject->GetOSGNode())->accept(statVis);
    unsigned int numTriangles  = 0;
    unsigned int numTriStrips  = 0;
    unsigned int numQuads      = 0;
    unsigned int numQuadStrips = 0;
    unsigned int numPolygons   = 0;
    unsigned int numTriFans    = 0;
    unsigned int numOthers     = 0;
    unsigned int numPrimSets   = 0;
    const osgUtil::Statistics::PrimitiveCountMap& pcMap ( statVis._instancedStats.getPrimitiveCountMap() );
    osgUtil::Statistics::PrimitiveCountMap::const_iterator mapEntry = pcMap.begin();
    for ( ; mapEntry != pcMap.end(); mapEntry++ )
    {
        numPrimSets += mapEntry->second;
        switch ( mapEntry->first )
        {
        case osg::PrimitiveSet::POINTS:
        case osg::PrimitiveSet::LINES:
        case osg::PrimitiveSet::LINE_STRIP:
        case osg::PrimitiveSet::LINE_LOOP:
            numOthers += mapEntry->second;
            break;

        case osg::PrimitiveSet::TRIANGLES:
            numTriangles  += mapEntry->second;
            break;

        case osg::PrimitiveSet::TRIANGLE_STRIP:
            numTriStrips  += mapEntry->second;
            break;

        case osg::PrimitiveSet::QUADS:
            numQuads      += mapEntry->second;
            break;

        case osg::PrimitiveSet::QUAD_STRIP:
            numQuadStrips += mapEntry->second;
            break;

        case osg::PrimitiveSet::POLYGON:
            numPolygons   += mapEntry->second;
            break;

        case osg::PrimitiveSet::TRIANGLE_FAN:
            numTriFans    += mapEntry->second;
            break;

        default:
            numOthers     += mapEntry->second;
            break;
        }
    }

    //unsigned int primCount = statVis._instancedStats._primitiveCount.size();

    std::cout << statVis._instancedStats._vertexCount << " vertices, " << std::endl;
    std::cout << numPrimSets << " total primitives." << std::endl;

    if ( numTriangles > 0 )                std::cout << numTriangles  << " triangle primitives" << std::endl;
    if ( numTriStrips > 0 )                std::cout << numTriStrips  << " triangles in strips" << std::endl;
    if ( numQuads > 0 )                    std::cout << numQuads      << " quads"               << std::endl;
    if ( numQuadStrips > 0 )               std::cout << numQuadStrips << " quads in strips"     << std::endl;
    if ( numPolygons > 0 )                 std::cout << numPolygons   << " polygons"            << std::endl;
    if ( numTriFans > 0 )                  std::cout << numTriFans    << " triangles in fans"   << std::endl;
    if ( numOthers > 0 )                   std::cout << numOthers     << " skipped"             << std::endl;

    if ( visit.numDrawablesSkipped > 0 )   std::cout << visit.numDrawablesSkipped << " drawables skipped"<< std::endl;
    if ( visit.numPrimitivesSkipped > 0 )  std::cout << visit.numPrimitivesSkipped <<" primitive sets skipped"<< std::endl;
    if ( visit.numEmptyPrimitiveSets > 0 ) std::cout << visit.numEmptyPrimitiveSets << " empty PrimitiveSets" << std::endl;

    std::cout << visit.numTriangles << " triangles in closest LOD." << std::endl;

}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnLoadGeometryFile(const std::string& filename)
{
   OnUnloadGeometryFile();

   QString qtFilename(filename.c_str());

   if (qtFilename.endsWith(".dtMap") || qtFilename.endsWith(".dtChar"))
   {
      QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

      dtCore::RefPtr<dtAnim::BaseModelWrapper> wrapper;
      //if (dtAnim::ModelDatabase::GetInstance().Load(filename))
      //{
      //   dtCore::RefPtr<dtAnim::BaseModelWrapper> wrapper =
      //}


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
   else
   {
       // If this is a static mesh
      mObject = new dtCore::Object;
      mObject->LoadFile(filename, false);

      // Ensure image data for textures remains available in case
      // model and textures need to be written back out at some
      // later time.
      dtCore::RefPtr<TextureVisitor> visitor = new TextureVisitor;
      mObject->GetOSGNode()->accept(*visitor);
      visitor->SetUnRefImageData(false);

      //print out stats
      CountPrimitives ( *(mObject->GetOSGNode()), filename );

      // set up the ObjectViewer's scene graph
      mShadeDecorator->addChild(mObject->GetOSGNode());
      mWireDecorator->addChild(mObject->GetOSGNode());

      osg::Vec3 center;

      if (mObject.valid())
      {
         float radius;
         mObject->GetBoundingSphere(center, radius);

         // Reset the camera outside the bounding sphere.
         ResetMotionModel(radius, center);

         if (mShouldGenerateTangents)
         {
            GenerateTangentsForObject(mObject.get());
         }
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnUnloadGeometryFile()
{
   // Remove node highlights.
   mNodeHighlighter->SetHighlightsDisabled();

   if (mObject.valid())
   {
      mShadeDecorator->removeChild(mObject->GetOSGNode());
      mWireDecorator->removeChild(mObject->GetOSGNode());

      mObject = nullptr;
   }

   if (mCharacter.valid())
   {
      mShadeDecorator->removeChild(mCharacter->GetOSGNode());
      mWireDecorator->removeChild(mCharacter->GetOSGNode());

      mCharacter = nullptr;
   }

   // Remove the old map.
   if (mMap.valid())
   {
      // Remove all proxies from the scene.
      clearProxies(mMap->GetAllProxies());

      try
      {
         dtCore::Project::GetInstance().CloseMap(*mMap, true);
         mMap = nullptr;
      }
      catch (const dtUtil::Exception &e)
      {
         QMessageBox::critical(nullptr, tr("Error"), e.What().c_str(), tr("OK"));
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnApplyShader(const std::string& groupName, const std::string& programName)
{
   dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();

   ShaderProgramPtr program = shaderManager.FindShaderPrototype(programName, groupName);
   assert(program);

   try
   {
      ShaderProgramPtr deltaProgram = shaderManager.AssignShaderFromPrototype(*program, *mShadedScene);
      osg::Program* osgProgram = deltaProgram->GetShaderProgram();

      // Bind the default location for tangents
      osgProgram->addBindAttribLocation("tangentAttrib", 6);

      emit SignalShaderApplied(deltaProgram);
   }
   catch (const dtUtil::Exception &e)
   {
      QMessageBox::critical(nullptr, "Error", e.ToString().c_str());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnRemoveShader()
{
   dtCore::ShaderManager& shaderManager = dtCore::ShaderManager::GetInstance();
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
void ObjectViewer::OnSetLightEnabled(int id, bool enabled)
{
   dtCore::Light* light = GetScene()->GetLight(id);
   light->SetEnabled(enabled);
   mLightArrow[id]->SetActive(enabled);
   mLightMotion[id]->SetEnabled(enabled);
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnSetLightType(int id, int type)
{
   QString lightName = QString("Light%1").arg(id);

   dtCore::Light* light = GetScene()->GetLight(id);
   dtCore::Light* newLight = nullptr;

   switch (type)
   {
   case 0: // Infinite
      {
         newLight = new dtCore::InfiniteLight(id, lightName.toStdString());
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

      // Copy the transform from the light to the attached transformable.
      dtCore::Transform transform;
      newLight->GetTransform(transform);
      newLight->SetTransform(dtCore::Transform());
      lightArrowTransformable->SetTransform(transform);

      lightArrowTransformable->AddChild(newLight);

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
   const float GRID_LINE_SPACING = 1.0f;

   const int numVerts(2 * 2 * GRID_LINE_COUNT * 3);
   const float length(((GRID_LINE_COUNT - 1) * GRID_LINE_SPACING) / 2.0f);

   osg::Vec3 verts[numVerts];
   int indx(0L);

   for (int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      float a = -length + ii * GRID_LINE_SPACING, b = length;
      verts[indx++].set(a,  b, 0.0f);
      verts[indx++].set(a, -b, 0.0f);
   }

   for (int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      float a = -length + ii * GRID_LINE_SPACING, b = length;
      verts[indx++].set(b,  a, 0.0f);
      verts[indx++].set(-b, a, 0.0f);
   }
   for (int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      float a = -length + ii * GRID_LINE_SPACING, b = length;
      verts[indx++].set(a, 0.0f, b);
      verts[indx++].set(a, 0.0f, -b);
   }
   for (int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      float a = -length + ii * GRID_LINE_SPACING, b = length;
      verts[indx++].set(b, 0.0f,  a);
      verts[indx++].set(-b, 0.0f, a);
   }
   for (int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      float a = -length + ii * GRID_LINE_SPACING, b = length;
      verts[indx++].set(0.0f, a, b);
      verts[indx++].set(0.0f, a, -b);
   }

   for (int ii(0L); ii < GRID_LINE_COUNT; ii++)
   {
      float a = -length + ii * GRID_LINE_SPACING, b = length;
      verts[indx++].set(0.0f, b,  a);
      verts[indx++].set(0.0f,-b,  a);
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

         light->SetAmbient(0.2f, 0.2f, 0.2f, 1.0f);
         light->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
         light->SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);

         // Infinite lights must start here, point light from the positive y axis
         //light->GetLightSource()->getLight()->setPosition(osg::Vec4(-osg::Y_AXIS, 0.0f));
      }

      bool enabled = light->GetEnabled();
      dtCore::RefPtr<dtCore::Object> lightArrow = new dtCore::Object;
      lightArrow->LoadFile("examples/data/StaticMeshes/LightArrow.ive");
      lightArrow->SetActive(enabled);

      dtCore::RefPtr<dtCore::Transformable> lightArrowTransformable = new dtCore::Transformable;
      lightArrowTransformable->AddChild(lightArrow.get());
      light->Emancipate();
      lightArrowTransformable->AddChild(light);

      int halfMaxLights = dtCore::MAX_LIGHTS / 2;

      // Set the lights to default position that do not overlap each so that
      // the object motion model can be discenerible for each of them
      float xOffset = (lightIndex % halfMaxLights) * 0.5f;

      if (lightIndex >= halfMaxLights)
      {
         // Since light 0 is in the middle already, start with a -0.5f offset
         xOffset = -0.5f + -1.0f * xOffset;
      }

      // Copy the transform from the light to the attached transformable and include offset mentioned above;
      dtCore::Transform transform;
      light->GetTransform(transform);
      light->SetTransform(dtCore::Transform());

      transform.SetTranslation(xOffset, 0.0f, 0.0f);
      lightArrowTransformable->SetTransform(transform);

      dtCore::RefPtr<dtCore::ObjectMotionModel> lightMotion = new dtCore::ObjectMotionModel(GetView());
      lightMotion->SetEnabled(enabled);
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
   //OnSetLightType(0, 0);
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
void ObjectViewer::clearProxies(const std::map<dtCore::UniqueId, dtCore::RefPtr<dtCore::BaseActorObject> >& proxies)
{
   std::map<dtCore::UniqueId,dtCore::RefPtr<dtCore::BaseActorObject> >::const_iterator itor;

   for (itor = proxies.begin(); itor != proxies.end(); ++itor)
   {
      dtCore::BaseActorObject* proxy = const_cast<dtCore::BaseActorObject*>(itor->second.get());

      dtCore::DeltaDrawable* drawable = proxy->GetDrawable();
      if (drawable)
      {
         mShadeDecorator->removeChild(drawable->GetOSGNode());
         mWireDecorator->removeChild(drawable->GetOSGNode());
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ObjectViewer::EventTraversal(const double deltaSimTime)
{
   dtABC::Application::EventTraversal(deltaSimTime);

   // Are we trying to manipulate an object motion model for the lights?
   size_t lightIndex = 0;
   while (lightIndex < mLightMotion.size())
   {
      if (mLightMotion[lightIndex]->IsEnabled() &&
          mLightMotion[lightIndex]->GetMotionType() != dtCore::ObjectMotionModel::MOTION_TYPE_MAX)
      {
         break;
      }

      ++lightIndex;
   }

   // If we're manipulating lights, disable orbit, otherwise enable
   mMotionModel->SetEnabled(lightIndex == mLightMotion.size());
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
      osg::Array* tangentArray = tsg->getTangentArray();
      tangentArray->setName("Tangents");

      if (!geom->getVertexAttribArray(6))
      {
         if (tangentArray != nullptr)
         {
            //geom->setVertexAttribData(6, osg::Geometry::ArrayData(tsg->getTangentArray(), osg::Geometry::BIND_PER_VERTEX, GL_FALSE));
            geom->setVertexAttribArray(6, tangentArray);
         }
         else
         {
            LOG_WARNING("Could not generate tangent space for object: " + object->GetName());
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnNextStatistics()
{
   SetNextStatisticsType();
}

//////////////////////////////////////////////////////////////////////////
void ObjectViewer::OnNodesSelected(OsgNodePtrVector nodes)
{
   mNodeHighlighter->SetHighlightsDisabled();

   if ( ! nodes.empty())
   {
      mNodeHighlighter->SetHighlightsEnabled(nodes, true);
   }
}


