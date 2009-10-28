#include <particleviewer.h>

#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PrimitiveSet>
#include <osg/StateAttribute>
#include <osg/Image>
#include <osg/Material>
#include <osg/MatrixTransform>
#include <osg/Texture2D>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>
#include <osgParticle/ForceOperator>
#include <osgParticle/MultiSegmentPlacer>
#include <osgParticle/PointPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/SegmentPlacer>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/CompositeViewer>

#include <QtGui/QFileDialog>
#include <QtCore/QtDebug>
#include <QtGui/QAction>

#include <dtCore/refptr.h>
#include <dtCore/system.h>
#include <dtCore/globals.h>
#include <dtCore/transform.h>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtCore/light.h>
#include <dtCore/compass.h>

////////////////////////////////////////////////////////////////////////////////
ParticleViewer::ParticleViewer()
: mpParticleSystemGroup(NULL)
, mpReferenceModel(NULL)
, mpParticleSystemUpdater(NULL)
, mLayerIndex(0)
, mMultiSegmentVertexIndex(0)
, mOperatorsIndex(0)
{
}

////////////////////////////////////////////////////////////////////////////////
ParticleViewer::~ParticleViewer()
{
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::Config()
{
   dtABC::Application::Config();
   GetCompositeViewer()->setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

   mMotion = new OrbitMotionModel(GetMouse(), GetCamera());

   mpSceneGroup = GetScene()->GetSceneNode();

   MakeCompass();
   MakeGrids();

   GetScene()->GetSceneNode()->addChild(mpXYGridTransform);
   GetScene()->GetSceneNode()->addChild(mpXZGridTransform);
   GetScene()->GetSceneNode()->addChild(mpYZGridTransform);

   mMotion->SetCompassTransform(mpCompassTransform);

   CreateNewParticleSystem();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::LoadFile(QString filename, bool import/* = false*/)
{
   if (!filename.isEmpty())
   {
      QFileInfo fileName = filename;

      osg::Node* node = osgDB::readNodeFile(filename.toStdString());
      if (node == NULL || !IS_A(node, osg::Group*))
      {
         qWarning() << "Invalid particle system:" << filename;
         return;
      }

      osg::Group* newParticleSystemGroup = static_cast<osg::Group*>(node);
      std::vector<ParticleSystemLayer> newLayers;
      osgParticle::ParticleSystemUpdater* newParticleSystemUpdater = NULL;
      unsigned int i;
      unsigned int OldWay = 0;

      for(i = 0; i < newParticleSystemGroup->getNumChildren(); ++i)
      {
         node = newParticleSystemGroup->getChild(i);

         // Old particle code here
         if(IS_A(node, osg::Group*))
         {
            ++OldWay;
         }
      }

      // Recreate the particle system so its children aren't all groups
      if(newParticleSystemGroup->getNumChildren() == OldWay)
      {
         for(unsigned int j = 0; j < OldWay; ++j)
         {
            node = newParticleSystemGroup->getChild(j);
            osg::Group* newerParticleSystemGroup = (osg::Group*)node;
            for(i = 0; i < newerParticleSystemGroup->getNumChildren(); ++i)
            {
               node = newerParticleSystemGroup->getChild(i);
               if(IS_A(node, osg::Geode*))
               {
                  ParticleSystemLayer layer;
                  layer.mGeode = (osg::Geode*)node;
                  for(unsigned int j = 0; j < layer.mGeode->getNumDrawables(); ++j)
                  {
                     osg::Drawable* drawable = layer.mGeode->getDrawable(j);
                     if(IS_A(drawable, osgParticle::ParticleSystem*))
                     {
                        layer.mParticleSystem = static_cast<osgParticle::ParticleSystem*>(drawable);
                        layer.mpParticle = new osgParticle::Particle(layer.mParticleSystem->getDefaultParticleTemplate());
                     }
                  }

                  if(layer.mGeode->getName() == "")
                  {
                     char buf[256];
                     sprintf(buf, "Layer %u", unsigned(newLayers.size() + (import ? mLayers.size() : 0)));
                     layer.mGeode->setName(buf);
                  }
                  newLayers.push_back(layer);
               }
               else if(IS_A(node, osgParticle::ParticleSystemUpdater*))
               {
                  newParticleSystemUpdater = static_cast<osgParticle::ParticleSystemUpdater*>(node);
               }
            }

            for(i = 0; i < newerParticleSystemGroup->getNumChildren(); ++i)
            {
               node = newerParticleSystemGroup->getChild(i);

               if(IS_A(node, osg::MatrixTransform*))
               {
                  osg::MatrixTransform* newEmitterTransform = static_cast<osg::MatrixTransform*>(node);
                  for(unsigned int j = 0; j < newEmitterTransform->getNumChildren(); ++j)
                  {
                     osg::Node* childNode = newEmitterTransform->getChild(j);
                     if(IS_A(childNode, osgParticle::ModularEmitter*))
                     {
                        osgParticle::ModularEmitter* newModularEmitter = static_cast<osgParticle::ModularEmitter*>(childNode);
                        for(unsigned int k = 0; k < newLayers.size(); ++k)
                        {
                           if(newLayers[k].mParticleSystem == newModularEmitter->getParticleSystem())
                           {
                              newLayers[k].mEmitterTransform = newEmitterTransform;
                              newLayers[k].mModularEmitter = newModularEmitter;
                              break;
                           }
                        }
                     }
                  }
               }
               else if(IS_A(node, osgParticle::ModularProgram*))
               {
                  osgParticle::ModularProgram* newModularProgram = static_cast<osgParticle::ModularProgram*>(node);
                  for(unsigned int j = 0; j < newLayers.size(); ++j)
                  {
                     if(newLayers[j].mParticleSystem == newModularProgram->getParticleSystem())
                     {
                        newLayers[j].mModularProgram = newModularProgram;
                        break;
                     }
                  }
               }
            }
         }

         SetTexturePaths(fileName.path(), false);
      }
      // end old way
      else
      {
         for(i = 0; i < newParticleSystemGroup->getNumChildren(); ++i)
         {
            node = newParticleSystemGroup->getChild(i);
            if (IS_A(node, osg::Geode*))
            {
               ParticleSystemLayer layer;
               layer.mGeode = (osg::Geode*)node;

               for(unsigned int j = 0; j < layer.mGeode->getNumDrawables(); ++j)
               {
                  osg::Drawable* drawable = layer.mGeode->getDrawable(j);
                  if (IS_A(drawable, osgParticle::ParticleSystem*))
                  {
                     layer.mParticleSystem = static_cast<osgParticle::ParticleSystem*>(drawable);
                     layer.mpParticle = new osgParticle::Particle(layer.mParticleSystem->getDefaultParticleTemplate());
                  }
               }

               if(layer.mGeode->getName() == "")
               {
                  char buf[256];
                  sprintf(buf, "Layer %u", unsigned(newLayers.size() + (import ? mLayers.size() : 0)));
                  layer.mGeode->setName(buf);
               }
               newLayers.push_back(layer);
            }
            else if(IS_A(node, osgParticle::ParticleSystemUpdater*))
            {
               newParticleSystemUpdater = static_cast<osgParticle::ParticleSystemUpdater*>(node);
            }
         }

         for(i = 0; i < newParticleSystemGroup->getNumChildren(); ++i)
         {
            node = newParticleSystemGroup->getChild(i);
            if (IS_A(node, osg::MatrixTransform*))
            {
               osg::MatrixTransform* newEmitterTransform = static_cast<osg::MatrixTransform*>(node);
               for (unsigned int j = 0; j < newEmitterTransform->getNumChildren(); ++j)
               {
                  osg::Node* childNode = newEmitterTransform->getChild(j);
                  if (IS_A(childNode, osgParticle::ModularEmitter*))
                  {
                     osgParticle::ModularEmitter* newModularEmitter = static_cast<osgParticle::ModularEmitter*>(childNode);
                     for (unsigned int k = 0; k < newLayers.size(); ++k)
                     {
                        if (newLayers[k].mParticleSystem == newModularEmitter->getParticleSystem())
                        {
                           newLayers[k].mEmitterTransform = newEmitterTransform;
                           newLayers[k].mModularEmitter = newModularEmitter;
                           break;
                        }
                     }
                  }
               }
            }
            else if(IS_A(node, osgParticle::ModularProgram*))
            {
               osgParticle::ModularProgram* newModularProgram = static_cast<osgParticle::ModularProgram*>(node);
               for(unsigned int j = 0; j < newLayers.size(); ++j)
               {
                  if(newLayers[j].mParticleSystem == newModularProgram->getParticleSystem())
                  {
                     newLayers[j].mModularProgram = newModularProgram;
                     break;
                  }
               }
            }
         }
      }
      if(newParticleSystemUpdater != NULL)
      {
         int newLayer = 0;
         if(import)
         {
            newLayer = mLayers.size();
            for (i = 0; i < newLayers.size(); ++i)
            {
               mpParticleSystemGroup->addChild(newLayers[i].mGeode.get());
               mpParticleSystemGroup->addChild(newLayers[i].mEmitterTransform.get());
               mpParticleSystemGroup->addChild(newLayers[i].mModularProgram.get());
               mpParticleSystemUpdater->addParticleSystem(newLayers[i].mParticleSystem.get());
               mLayers.push_back(newLayers[i]);
            }
            ResetEmitters();
         }
         else
         {
            mpSceneGroup->removeChild(mpParticleSystemGroup);
            mpParticleSystemGroup = newParticleSystemGroup;
            mLayers = newLayers;
            mpParticleSystemUpdater = newParticleSystemUpdater;
            mpSceneGroup->addChild(mpParticleSystemGroup);
         }
         SetTexturePaths(fileName.path(), false);
         UpdateLayersList();
         emit SelectIndexOfLayersList(newLayer);
         SetParticleSystemFilename(filename);
      }
      else
      {
         qWarning() << "Invalid particle system:" << filename;
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::CreateNewParticleSystem()
{
   if(mpParticleSystemGroup != NULL)
   {
      GetScene()->GetSceneNode()->removeChild(mpParticleSystemGroup);
   }

   mpParticleSystemGroup = new osg::Group();
   mpParticleSystemUpdater = new osgParticle::ParticleSystemUpdater();
   mpParticleSystemGroup->addChild(mpParticleSystemUpdater);
   GetScene()->GetSceneNode()->addChild(mpParticleSystemGroup);
   SetParticleSystemFilename("");
   mLayers.clear();
   UpdateLayersList();
   CreateNewParticleLayer();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OpenParticleSystem()
{
   QString filename = QFileDialog::getOpenFileName(NULL, tr("Open"),
      mParticleSystemFilename, tr("Particle Systems (*.osg)"));

   if(filename != "")
   {
      LoadFile(filename);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OpenRecentParticleSystem()
{
   QAction* action = qobject_cast<QAction*>(sender());

   if (action)
   {
      LoadFile(action->data().toString());
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ImportParticleSystem()
{
   QString filename = QFileDialog::getOpenFileName(NULL, tr("Import"),
      mParticleSystemFilename, tr("Particle Systems (*.osg)"));

   if(filename != "")
   {
      LoadFile(filename, true);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::LoadReferenceObject()
{
   QString filename = QFileDialog::getOpenFileName(NULL, tr("Load Reference"),
      mParticleSystemFilename, tr("Geometry Files (*.{osg,ive})\tOSG Files (*.osg)\tIVE Files (*.ive)"));

   bool visible = true;

   if (filename != "")
   {
      if (mpReferenceModel.valid())
      {
         //remove the existing one from the scene
         mpSceneGroup->removeChild(mpReferenceModel.get());

         //carry over the visibility of the old file to the new one
         visible = mpReferenceModel->getNodeMask() ? true : false;
      }
      mpReferenceModel = osgDB::readNodeFile(filename.toStdString());
      if (mpReferenceModel.valid())
      {
         mpSceneGroup->addChild(mpReferenceModel.get());
         emit ReferenceObjectLoaded(filename);
         
         ToggleReferenceObject(visible);
      }
      else
      {
         qWarning() << "Can't load geometry file:" << filename;
      }
   }

}

//////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleReferenceObject(bool enabled)
{
   if (mpReferenceModel.valid() == false) { return; }

   if (enabled)
   {
      mpReferenceModel->setNodeMask(0xffffffff);
   }
   else
   {
      mpReferenceModel->setNodeMask(0x0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SaveParticleToFile()
{
   if (mParticleSystemFilename.isEmpty())
   {
      SaveParticleAs();
   }
   else
   {
      QFileInfo filename = mParticleSystemFilename;

      SetTexturePaths(filename.path(), true);
      ResetEmitters();
      osgDB::writeNodeFile(*mpParticleSystemGroup, mParticleSystemFilename.toStdString());
      SetTexturePaths(filename.path(), false);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SaveParticleAs()
{
   QString filename = QFileDialog::getSaveFileName(NULL, tr("Save As"),
      mParticleSystemFilename, tr("Particle Systems (*.osg)"));

   if (!filename.isEmpty())
   {
      SetParticleSystemFilename(filename);
      SaveParticleToFile();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleCompass(bool enabled)
{
   if(enabled)
   {
      mpCompassTransform->setNodeMask(0xFFFFFFFF);
   }
   else
   {
      mpCompassTransform->setNodeMask(0x0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleXYGrid(bool enabled)
{
   if(enabled)
   {
      mpXYGridTransform->setNodeMask(0xFFFFFFFF);
   }
   else
   {
      mpXYGridTransform->setNodeMask(0x0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleXZGrid(bool enabled)
{
   if(enabled)
   {
      mpXZGridTransform->setNodeMask(0xFFFFFFFF);
   }
   else
   {
      mpXZGridTransform->setNodeMask(0x0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleYZGrid(bool enabled)
{
   if(enabled)
   {
      mpYZGridTransform->setNodeMask(0xFFFFFFFF);
   }
   else
   {
      mpYZGridTransform->setNodeMask(0x0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::CreateNewParticleLayer()
{
   ParticleSystemLayer layer;
   layer.mParticleSystem = new osgParticle::ParticleSystem();
   layer.mParticleSystem->setDefaultAttributes("", true, false);
   layer.mpParticle = new osgParticle::Particle();
   layer.mParticleSystem->setDefaultParticleTemplate(*layer.mpParticle);
   layer.mGeode = new osg::Geode();

   char buf[256];
   sprintf(buf, "Layer %u", unsigned(mLayers.size()));
   layer.mGeode->setName(buf);
   layer.mGeode->addDrawable(layer.mParticleSystem.get());
   mpParticleSystemGroup->addChild(layer.mGeode.get());

   layer.mEmitterTransform = new osg::MatrixTransform();
   layer.mModularEmitter = new osgParticle::ModularEmitter();
   layer.mModularEmitter->setParticleSystem(layer.mParticleSystem.get());
   osgParticle::RandomRateCounter* rrc = new osgParticle::RandomRateCounter();
   rrc->setRateRange(20, 30);
   layer.mModularEmitter->setCounter(rrc);
   layer.mModularEmitter->setPlacer(new osgParticle::PointPlacer());
   layer.mModularEmitter->setShooter(new osgParticle::RadialShooter());
   layer.mModularEmitter->setLifeTime(5.0);
   layer.mEmitterTransform->addChild(layer.mModularEmitter.get());
   mpParticleSystemGroup->addChild(layer.mEmitterTransform.get());

   layer.mModularProgram = new osgParticle::ModularProgram();
   layer.mModularProgram->setParticleSystem(layer.mParticleSystem.get());
   mpParticleSystemGroup->addChild(layer.mModularProgram.get());
   mpParticleSystemUpdater->addParticleSystem(layer.mParticleSystem.get());
   mLayers.push_back(layer);

   ResetEmitters();

   // Update UI
   int rowCount = mLayers.size() - 1;
   QString newLayerLabel;
   newLayerLabel.sprintf("Layer %d", rowCount);
   emit AddLayerToLayerList(newLayerLabel);
   emit SelectIndexOfLayersList(rowCount);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::DeleteSelectedLayer()
{
   mpParticleSystemGroup->removeChild(mLayers[mLayerIndex].mGeode.get());
   mpParticleSystemGroup->removeChild(mLayers[mLayerIndex].mEmitterTransform.get());
   mpParticleSystemGroup->removeChild(mLayers[mLayerIndex].mModularProgram.get());

   mpParticleSystemUpdater->removeParticleSystem(mLayers[mLayerIndex].mParticleSystem.get());

   mLayers.erase(mLayers.begin() + mLayerIndex);

   UpdateLayersList();

   if(mLayerIndex >= mLayers.size())
   {
      mLayerIndex = mLayers.size() - 1;
   }
   emit SelectIndexOfLayersList(mLayerIndex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleSelectedLayerHidden()
{
   if(mLayers[mLayerIndex].mModularEmitter->isEnabled())
   {
      mLayers[mLayerIndex].mModularEmitter->setEnabled(false);
   }
   else
   {
      mLayers[mLayerIndex].mModularEmitter->setEnabled(true);
   }
}

//////////////////////////////////////////////////////////////////////////
void ParticleViewer::RenameParticleLayer(const QString& name)
{
   if (mLayerIndex < mLayers.size())
   {
      mLayers[mLayerIndex].mGeode->setName(name.toStdString());
   }
}


////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ResetEmitters()
{
   for (unsigned int i = 0; i < mLayers.size(); ++i)
   {
      mLayers[i].mModularEmitter->setCurrentTime(0.0);
      mLayers[i].mModularProgram->setCurrentTime(0.0);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateSelectionIndex(int newIndex)
{
   if(0 <= newIndex && newIndex < static_cast<int>(mLayers.size()))
   {
      mLayerIndex = newIndex;
      UpdateParticleTabsValues();
      UpdateCounterTabsValues();
      UpdatePlacerTabsValues();
      UpdateShooterTabsValues();
      UpdateProgramTabsValues();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::AlignmentChanged(int newAlignment)
{
   mLayers[mLayerIndex].mParticleSystem->setParticleAlignment(
      (osgParticle::ParticleSystem::Alignment)newAlignment);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ShapeChanged(int newShape)
{
   mLayers[mLayerIndex].mpParticle->setShape((osgParticle::Particle::Shape)newShape);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleEmissive(bool enabled)
{
   std::string textureFile = "";
   osg::StateSet* ss = mLayers[mLayerIndex].mParticleSystem->getStateSet();
   osg::StateAttribute* sa = ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
   if(IS_A(sa, osg::Texture2D*))
   {
      osg::Texture2D* t2d = (osg::Texture2D*)sa;
      osg::Image* image = t2d->getImage();
      if (image != NULL)
      {
         textureFile = image->getFileName();
      }
   }
   osg::Material* material = (osg::Material*)ss->getAttribute(osg::StateAttribute::MATERIAL);
   bool lighting = material->getColorMode() == osg::Material::AMBIENT_AND_DIFFUSE;

   mLayers[mLayerIndex].mParticleSystem->setDefaultAttributes(textureFile, enabled, lighting);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ToggleLighting(bool enabled)
{
   std::string textureFile = "";
   osg::StateSet* ss = mLayers[mLayerIndex].mParticleSystem->getStateSet();
   osg::StateAttribute* sa = ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
   if(IS_A(sa, osg::Texture2D*))
   {
      osg::Texture2D* t2d = (osg::Texture2D*)sa;
      osg::Image* image = t2d->getImage();
      if (image != NULL)
      {
         textureFile = image->getFileName();
      }
   }
   osg::BlendFunc* blend = (osg::BlendFunc*)ss->getAttribute(osg::StateAttribute::BLENDFUNC);
   bool emissive = blend->getDestination() == osg::BlendFunc::ONE;

   mLayers[mLayerIndex].mParticleSystem->setDefaultAttributes(textureFile, emissive, enabled);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::LifeValueChanged(double newValue)
{
   mLayers[mLayerIndex].mpParticle->setLifeTime(newValue);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadiusValueChanged(double newValue)
{
   mLayers[mLayerIndex].mpParticle->setRadius(newValue);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MassValueChanged(double newValue)
{
   mLayers[mLayerIndex].mpParticle->setMass(newValue);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SizeFromValueChanged(double newValue)
{
   osgParticle::rangef sizeRange = mLayers[mLayerIndex].mpParticle->getSizeRange();
   sizeRange.minimum = newValue;
   mLayers[mLayerIndex].mpParticle->setSizeRange(sizeRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SizeToValueChanged(double newValue)
{
   osgParticle::rangef sizeRange = mLayers[mLayerIndex].mpParticle->getSizeRange();
   sizeRange.maximum = newValue;
   mLayers[mLayerIndex].mpParticle->setSizeRange(sizeRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::TextureChanged(QString filename)
{
   osg::StateSet* ss = mLayers[mLayerIndex].mParticleSystem->getStateSet();
   osg::BlendFunc* blend = (osg::BlendFunc*)ss->getAttribute(osg::StateAttribute::BLENDFUNC);
   bool emissive = blend->getDestination() == osg::BlendFunc::ONE;
   osg::Material* material = (osg::Material*)ss->getAttribute(osg::StateAttribute::MATERIAL);
   bool lighting = material->getColorMode() == osg::Material::AMBIENT_AND_DIFFUSE;

   mLayers[mLayerIndex].mParticleSystem->setDefaultAttributes(filename.toStdString(), emissive, lighting);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RFromValueChanged(double newValue)
{
   osgParticle::rangev4 colorRange = mLayers[mLayerIndex].mpParticle->getColorRange();
   colorRange.minimum[0] = newValue;
   mLayers[mLayerIndex].mpParticle->setColorRange(colorRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RToValueChanged(double newValue)
{
   osgParticle::rangev4 colorRange = mLayers[mLayerIndex].mpParticle->getColorRange();
   colorRange.maximum[0] = newValue;
   mLayers[mLayerIndex].mpParticle->setColorRange(colorRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::GFromValueChanged(double newValue)
{
   osgParticle::rangev4 colorRange = mLayers[mLayerIndex].mpParticle->getColorRange();
   colorRange.minimum[1] = newValue;
   mLayers[mLayerIndex].mpParticle->setColorRange(colorRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::GToValueChanged(double newValue)
{
   osgParticle::rangev4 colorRange = mLayers[mLayerIndex].mpParticle->getColorRange();
   colorRange.maximum[1] = newValue;
   mLayers[mLayerIndex].mpParticle->setColorRange(colorRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::BFromValueChanged(double newValue)
{
   osgParticle::rangev4 colorRange = mLayers[mLayerIndex].mpParticle->getColorRange();
   colorRange.minimum[2] = newValue;
   mLayers[mLayerIndex].mpParticle->setColorRange(colorRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::BToValueChanged(double newValue)
{
   osgParticle::rangev4 colorRange = mLayers[mLayerIndex].mpParticle->getColorRange();
   colorRange.maximum[2] = newValue;
   mLayers[mLayerIndex].mpParticle->setColorRange(colorRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::AFromValueChanged(double newValue)
{
   osgParticle::rangef sizeRange = mLayers[mLayerIndex].mpParticle->getAlphaRange();
   sizeRange.minimum = newValue;
   mLayers[mLayerIndex].mpParticle->setAlphaRange(sizeRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::AToValueChanged(double newValue)
{
   osgParticle::rangef sizeRange = mLayers[mLayerIndex].mpParticle->getAlphaRange();
   sizeRange.maximum = newValue;
   mLayers[mLayerIndex].mpParticle->setAlphaRange(sizeRange);
   mLayers[mLayerIndex].mParticleSystem->setDefaultParticleTemplate(*mLayers[mLayerIndex].mpParticle);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::EmitterLifeValueChanged(double newValue)
{
   mLayers[mLayerIndex].mModularEmitter->setLifeTime(newValue);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::EmitterStartValueChanged(double newValue)
{
   mLayers[mLayerIndex].mModularEmitter->setStartTime(newValue);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::EmitterResetValueChanged(double newValue)
{
   mLayers[mLayerIndex].mModularEmitter->setResetTime(newValue);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::EndlessLifetimeChanged(bool endless)
{
   mLayers[mLayerIndex].mModularEmitter->setEndless(endless);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::CounterTypeBoxValueChanged(int newCounter)
{
   osgParticle::Counter* counter = mLayers[mLayerIndex].mModularEmitter->getCounter();

   switch(newCounter)
   {
   case 0:
      if(!IS_A(counter, osgParticle::RandomRateCounter*))
      {
         mLayers[mLayerIndex].mModularEmitter->setCounter(new osgParticle::RandomRateCounter());
         UpdateRandomRatesValues();
      }
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RandomRateMinRateValueChanged(double newValue)
{
   osgParticle::RandomRateCounter* rrc =
      (osgParticle::RandomRateCounter*)mLayers[mLayerIndex].mModularEmitter->getCounter();
   osgParticle::rangef rateRange = rrc->getRateRange();
   rateRange.minimum = newValue;
   rrc->setRateRange(rateRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RandomRateMaxRateValueChanged(double newValue)
{
   osgParticle::RandomRateCounter* rrc =
      (osgParticle::RandomRateCounter*)mLayers[mLayerIndex].mModularEmitter->getCounter();
   osgParticle::rangef rateRange = rrc->getRateRange();
   rateRange.maximum = newValue;
   rrc->setRateRange(rateRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::PlacerTypeBoxValueChanged(int newCounter)
{
   osgParticle::Placer* placer = mLayers[mLayerIndex].mModularEmitter->getPlacer();

   switch(newCounter)
   {
   case 0:
      if(!IS_A(placer, osgParticle::PointPlacer*))
      {
         mLayers[mLayerIndex].mModularEmitter->setPlacer(new osgParticle::PointPlacer());
         UpdatePointPlacerValues();
      }
      break;

   case 1:
      if (!IS_A(placer, osgParticle::SectorPlacer*))
      {
         mLayers[mLayerIndex].mModularEmitter->setPlacer(new osgParticle::SectorPlacer());
         UpdateSectorPlacerValues();
      }
      break;

   case 2:
      if (!IS_A(placer, osgParticle::SegmentPlacer*))
      {
         mLayers[mLayerIndex].mModularEmitter->setPlacer(new osgParticle::SegmentPlacer());
         UpdateSegmentPlacerValues();
      }
      break;

   case 3:
      if (!IS_A(placer, osgParticle::MultiSegmentPlacer*))
      {
         osgParticle::MultiSegmentPlacer* msp = new osgParticle::MultiSegmentPlacer();
         msp->addVertex(-1, 0, 0);
         msp->addVertex(1, 0, 0);
         mLayers[mLayerIndex].mModularEmitter->setPlacer(msp);
         UpdateMultiSegmentPlacerVertexList();
      }
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::PointPlacerXValueChanged(double newValue)
{
   osgParticle::PointPlacer* pp =(osgParticle::PointPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = pp->getCenter();
   center[0] = newValue;
   pp->setCenter(center);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::PointPlacerYValueChanged(double newValue)
{
   osgParticle::PointPlacer* pp =(osgParticle::PointPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = pp->getCenter();
   center[1] = newValue;
   pp->setCenter(center);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::PointPlacerZValueChanged(double newValue)
{
   osgParticle::PointPlacer* pp =(osgParticle::PointPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = pp->getCenter();
   center[2] = newValue;
   pp->setCenter(center);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SectorPlacerXValueChanged(double newValue)
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = sp->getCenter();
   center[0] = newValue;
   sp->setCenter(center);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SectorPlacerYValueChanged(double newValue)
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = sp->getCenter();
   center[1] = newValue;
   sp->setCenter(center);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SectorPlacerZValueChanged(double newValue)
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = sp->getCenter();
   center[2] = newValue;
   sp->setCenter(center);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SectorPlacerMinRadiusValueChanged(double newValue)
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osgParticle::rangef radiusRange = sp->getRadiusRange();
   radiusRange.minimum = newValue;
   sp->setRadiusRange(radiusRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SectorPlacerMaxRadiusValueChanged(double newValue)
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osgParticle::rangef radiusRange = sp->getRadiusRange();
   radiusRange.maximum = newValue;
   sp->setRadiusRange(radiusRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SectorPlacerMinPhiValueChanged(double newValue)
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osgParticle::rangef phiRange = sp->getPhiRange();
   phiRange.minimum = newValue;
   sp->setPhiRange(phiRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SectorPlacerMaxPhiValueChanged(double newValue)
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osgParticle::rangef phiRange = sp->getPhiRange();
   phiRange.maximum = newValue;
   sp->setPhiRange(phiRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SegmentPlacerVertexAXValueChanged(double newValue)
{
   osgParticle::SegmentPlacer* sp =(osgParticle::SegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertexA = sp->getVertexA();
   vertexA[0] = newValue;
   sp->setVertexA(vertexA);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SegmentPlacerVertexAYValueChanged(double newValue)
{
   osgParticle::SegmentPlacer* sp =(osgParticle::SegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertexA = sp->getVertexA();
   vertexA[1] = newValue;
   sp->setVertexA(vertexA);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SegmentPlacerVertexAZValueChanged(double newValue)
{
   osgParticle::SegmentPlacer* sp =(osgParticle::SegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertexA = sp->getVertexA();
   vertexA[2] = newValue;
   sp->setVertexA(vertexA);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SegmentPlacerVertexBXValueChanged(double newValue)
{
   osgParticle::SegmentPlacer* sp =(osgParticle::SegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertexB = sp->getVertexB();
   vertexB[0] = newValue;
   sp->setVertexB(vertexB);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SegmentPlacerVertexBYValueChanged(double newValue)
{
   osgParticle::SegmentPlacer* sp =(osgParticle::SegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertexB = sp->getVertexB();
   vertexB[1] = newValue;
   sp->setVertexB(vertexB);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SegmentPlacerVertexBZValueChanged(double newValue)
{
   osgParticle::SegmentPlacer* sp =(osgParticle::SegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertexB = sp->getVertexB();
   vertexB[2] = newValue;
   sp->setVertexB(vertexB);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateMultiSegmentPlacerSelectionIndex(int newIndex)
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   if(0 <= newIndex && newIndex < msp->numVertices())
   {
      mMultiSegmentVertexIndex = newIndex;
      UpdateMultiSegmentPlacerValues();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MultiSegmentPlacerAddVertex()
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   msp->addVertex(0.0, 0.0, 0.0);
   mMultiSegmentVertexIndex = msp->numVertices() - 1;
   UpdateMultiSegmentPlacerVertexList();
   emit SelectIndexOfMultiSegmentPlacerVertexList(mMultiSegmentVertexIndex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MultiSegmentPlacerDeleteVertex()
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   msp->removeVertex(mMultiSegmentVertexIndex);
   UpdateMultiSegmentPlacerVertexList();
   if(mMultiSegmentVertexIndex >= msp->numVertices())
   {
      mMultiSegmentVertexIndex = msp->numVertices() - 1;
   }
   emit SelectIndexOfMultiSegmentPlacerVertexList(mMultiSegmentVertexIndex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MultiSegmentPlacerXValueChanged(double newValue)
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertex = msp->getVertex(mMultiSegmentVertexIndex);
   vertex[0] = newValue;
   msp->setVertex(mMultiSegmentVertexIndex, vertex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MultiSegmentPlacerYValueChanged(double newValue)
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertex = msp->getVertex(mMultiSegmentVertexIndex);
   vertex[1] = newValue;
   msp->setVertex(mMultiSegmentVertexIndex, vertex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MultiSegmentPlacerZValueChanged(double newValue)
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertex = msp->getVertex(mMultiSegmentVertexIndex);
   vertex[2] = newValue;
   msp->setVertex(mMultiSegmentVertexIndex, vertex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::ShooterTypeBoxValueChanged(int newShooter)
{
   osgParticle::Shooter* shooter = mLayers[mLayerIndex].mModularEmitter->getShooter();

   switch(newShooter)
   {
   case 0:
      if(!IS_A(shooter, osgParticle::RadialShooter*))
      {
         mLayers[mLayerIndex].mModularEmitter->setShooter(new osgParticle::RadialShooter());
         UpdateRadialShooterValues();
      }
      break;
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterElevationMinValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangef elevationRange = rs->getThetaRange();
   elevationRange.minimum = newValue;
   rs->setThetaRange(elevationRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterElevationMaxValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangef elevationRange = rs->getThetaRange();
   elevationRange.maximum = newValue;
   rs->setThetaRange(elevationRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterAzimuthMinValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangef azimuthRange = rs->getPhiRange();
   azimuthRange.minimum = newValue;
   rs->setPhiRange(azimuthRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterAzimuthMaxValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangef azimuthRange = rs->getPhiRange();
   azimuthRange.maximum = newValue;
   rs->setPhiRange(azimuthRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialVelocityMinValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangef initialVelocityRange = rs->getInitialSpeedRange();
   initialVelocityRange.minimum = newValue;
   rs->setInitialSpeedRange(initialVelocityRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialVelocityMaxValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangef initialVelocityRange = rs->getInitialSpeedRange();
   initialVelocityRange.maximum = newValue;
   rs->setInitialSpeedRange(initialVelocityRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialMinRotationXValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangev3 rotationalRange = rs->getInitialRotationalSpeedRange();
   rotationalRange.minimum[0] = newValue;
   rs->setInitialRotationalSpeedRange(rotationalRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialMinRotationYValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangev3 rotationalRange = rs->getInitialRotationalSpeedRange();
   rotationalRange.minimum[1] = newValue;
   rs->setInitialRotationalSpeedRange(rotationalRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialMinRotationZValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangev3 rotationalRange = rs->getInitialRotationalSpeedRange();
   rotationalRange.minimum[2] = newValue;
   rs->setInitialRotationalSpeedRange(rotationalRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialMaxRotationXValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangev3 rotationalRange = rs->getInitialRotationalSpeedRange();
   rotationalRange.maximum[0] = newValue;
   rs->setInitialRotationalSpeedRange(rotationalRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialMaxRotationYValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangev3 rotationalRange = rs->getInitialRotationalSpeedRange();
   rotationalRange.maximum[1] = newValue;
   rs->setInitialRotationalSpeedRange(rotationalRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::RadialShooterInitialMaxRotationZValueChanged(double newValue)
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangev3 rotationalRange = rs->getInitialRotationalSpeedRange();
   rotationalRange.maximum[2] = newValue;
   rs->setInitialRotationalSpeedRange(rotationalRange);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateOperatorsSelectionIndex(int newIndex, const QString &operatorType)
{
   if(0 <= newIndex && newIndex < mLayers[mLayerIndex].mModularProgram->numOperators())
   {
      mOperatorsIndex = newIndex;
   }

   if(operatorType == "Force")
   {
      UpdateForceValues();
   }
   else if(operatorType == "Acceleration")
   {
      UpdateAccelerationValues();
   }
   else if(operatorType == "Fluid Friction")
   {
      UpdateFluidFrictionValues();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsAddNewForce()
{
   osgParticle::ForceOperator* ao = new osgParticle::ForceOperator();
   mLayers[mLayerIndex].mModularProgram->addOperator(ao);
   UpdateOperatorsList();
   emit SelectIndexOfOperatorsList(mLayers[mLayerIndex].mModularProgram->numOperators() - 1);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsAddNewAcceleration()
{
   osgParticle::AccelOperator* ao = new osgParticle::AccelOperator();
   mLayers[mLayerIndex].mModularProgram->addOperator(ao);
   UpdateOperatorsList();
   emit SelectIndexOfOperatorsList(mLayers[mLayerIndex].mModularProgram->numOperators() - 1);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsAddNewFluidFriction()
{
   osgParticle::FluidFrictionOperator* ao = new osgParticle::FluidFrictionOperator();
   mLayers[mLayerIndex].mModularProgram->addOperator(ao);
   UpdateOperatorsList();
   emit SelectIndexOfOperatorsList(mLayers[mLayerIndex].mModularProgram->numOperators() - 1);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsDeleteCurrentOperator()
{
   mLayers[mLayerIndex].mModularProgram->removeOperator(mOperatorsIndex);
   UpdateOperatorsList();
   if(mOperatorsIndex >= mLayers[mLayerIndex].mModularProgram->numOperators())
   {
      mOperatorsIndex = mLayers[mLayerIndex].mModularProgram->numOperators() - 1;
   }
   emit SelectIndexOfOperatorsList(mOperatorsIndex);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsForceXValueChanged(double newValue)
{
   osgParticle::ForceOperator* fo = (osgParticle::ForceOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 force = fo->getForce();
   force[0] = newValue;
   fo->setForce(force);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsForceYValueChanged(double newValue)
{
   osgParticle::ForceOperator* fo = (osgParticle::ForceOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 force = fo->getForce();
   force[1] = newValue;
   fo->setForce(force);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsForceZValueChanged(double newValue)
{
   osgParticle::ForceOperator* fo = (osgParticle::ForceOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 force = fo->getForce();
   force[2] = newValue;
   fo->setForce(force);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsAccelerationXValueChanged(double newValue)
{
   osgParticle::AccelOperator* ao = (osgParticle::AccelOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 acceleration = ao->getAcceleration();
   acceleration[0] = newValue;
   ao->setAcceleration(acceleration);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsAccelerationYValueChanged(double newValue)
{
   osgParticle::AccelOperator* ao = (osgParticle::AccelOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 acceleration = ao->getAcceleration();
   acceleration[1] = newValue;
   ao->setAcceleration(acceleration);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsAccelerationZValueChanged(double newValue)
{
   osgParticle::AccelOperator* ao = (osgParticle::AccelOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 acceleration = ao->getAcceleration();
   acceleration[2] = newValue;
   ao->setAcceleration(acceleration);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::FluidFrictionAirButtonPressed()
{
   osgParticle::FluidFrictionOperator* ffo = (osgParticle::FluidFrictionOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   ffo->setFluidToAir();
   UpdateFluidFrictionValues();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::FluidFrictionWaterButtonPressed()
{
   osgParticle::FluidFrictionOperator* ffo = (osgParticle::FluidFrictionOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   ffo->setFluidToWater();
   UpdateFluidFrictionValues();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsFluidFrictionDensityValueChanged(double newValue)
{
   osgParticle::FluidFrictionOperator* ffo = (osgParticle::FluidFrictionOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   ffo->setFluidDensity(newValue);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsFluidFrictionViscosityValueChanged(double newValue)
{
   osgParticle::FluidFrictionOperator* ffo = (osgParticle::FluidFrictionOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   ffo->setFluidViscosity(newValue);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::OperatorsFluidFrictionOverrideRadiusValueChanged(double newValue)
{
   osgParticle::FluidFrictionOperator* ffo = (osgParticle::FluidFrictionOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   ffo->setOverrideRadius(newValue);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MakeCompass()
{
   dtCore::Compass* compass = new dtCore::Compass(GetCamera());
   GetScene()->AddDrawable(compass);
   mpCompassTransform = (osg::MatrixTransform*)compass->GetOSGNode();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::MakeGrids()
{
   const int numVertices = 2 * 2 * GRID_LINE_COUNT;
   osg::Vec3 vertices[numVertices];
   float length = (GRID_LINE_COUNT - 1) * GRID_LINE_SPACING;
   int ptr = 0;

   for(int i = 0; i < GRID_LINE_COUNT; ++i)
   {
      vertices[ptr++].set(-length / 2 + i * GRID_LINE_SPACING, length / 2, 0.0f);
      vertices[ptr++].set(-length / 2 + i * GRID_LINE_SPACING, -length / 2, 0.0f);
   }

   for (int i = 0; i < GRID_LINE_COUNT; ++i)
   {
      vertices[ptr++].set(length / 2, -length / 2 + i * GRID_LINE_SPACING, 0.0f);
      vertices[ptr++].set(-length / 2, -length / 2 + i * GRID_LINE_SPACING, 0.0f);
   }

   osg::Geometry* geometry = new osg::Geometry;
   geometry->setVertexArray(new osg::Vec3Array(numVertices, vertices));
   geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, numVertices));

   osg::Geode* geode = new osg::Geode;
   geode->addDrawable(geometry);
   geode->getOrCreateStateSet()->setMode(GL_LIGHTING, 0);

   mpXYGridTransform = new osg::MatrixTransform;
   mpXYGridTransform->addChild(geode);

   mpXZGridTransform = new osg::MatrixTransform;
   mpXZGridTransform->setMatrix(osg::Matrix::rotate(osg::PI_2, 1, 0, 0));

   mpXZGridTransform->addChild(geode);
   mpXZGridTransform->setNodeMask(0x0);

   mpYZGridTransform = new osg::MatrixTransform;
   mpYZGridTransform->setMatrix(osg::Matrix::rotate(osg::PI_2, 0, 1, 0));

   mpYZGridTransform->addChild(geode);
   mpYZGridTransform->setNodeMask(0x0);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SetParticleSystemFilename(QString filename)
{
   mParticleSystemFilename = filename;

   if(mParticleSystemFilename == "")
   {
      emit UpdateWindowTitle("Particle System Editor");
   }
   else
   {
      QString newWindowTitle("Particle System Editor - ");
      newWindowTitle.append(filename);
      emit UpdateWindowTitle(newWindowTitle);
      emit UpdateHistory(filename);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateLayersList()
{
   emit ClearLayerList();
   for(uint i = 0; i < mLayers.size(); ++i)
   {
      emit AddLayerToLayerList(QString::fromStdString(mLayers[i].mGeode->getName()));
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateParticleTabsValues()
{
   // Particle UI
   emit LayerHiddenChanged(mLayers[mLayerIndex].mModularEmitter->isEnabled());
   emit LayerRenderBinChanged(mLayers[mLayerIndex].mParticleSystem->getOrCreateStateSet()->getBinNumber());
   emit AlignmentUpdated((int)mLayers[mLayerIndex].mParticleSystem->getParticleAlignment());
   emit ShapeUpdated(mLayers[mLayerIndex].mpParticle->getShape());
   emit LifeUpdated(mLayers[mLayerIndex].mpParticle->getLifeTime());
   emit RadiusUpdated(mLayers[mLayerIndex].mpParticle->getRadius());
   emit MassUpdated(mLayers[mLayerIndex].mpParticle->getMass());
   osgParticle::rangef sizeRange = mLayers[mLayerIndex].mpParticle->getSizeRange();
   emit SizeFromUpdated(sizeRange.minimum);
   emit SizeToUpdated(sizeRange.maximum);

   // Texture UI
   std::string textureFile = "";
   osg::StateSet* ss = mLayers[mLayerIndex].mParticleSystem->getStateSet();
   osg::StateAttribute* sa = ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
   if(IS_A(sa, osg::Texture2D*))
   {
      osg::Texture2D* t2d = (osg::Texture2D*)sa;
      osg::Image* image = t2d->getImage();
      if (image != NULL)
      {
         textureFile = image->getFileName();
      }
   }
   osg::BlendFunc* blend = (osg::BlendFunc*)ss->getAttribute(osg::StateAttribute::BLENDFUNC);
   bool emissive = (blend->getDestination() == osg::BlendFunc::ONE);
   osg::Material* material = (osg::Material*)ss->getAttribute(osg::StateAttribute::MATERIAL);
   bool lighting = (material->getColorMode() == osg::Material::AMBIENT_AND_DIFFUSE);
   emit TextureUpdated(QString::fromStdString(textureFile));
   emit EmissiveUpdated(emissive);
   emit LightingUpdated(lighting);

   // Color UI
   osgParticle::rangev4 colorRange = mLayers[mLayerIndex].mpParticle->getColorRange();
   emit RFromUpdated(colorRange.minimum[0]);
   emit RToUpdated(colorRange.maximum[0]);
   emit GFromUpdated(colorRange.minimum[1]);
   emit GToUpdated(colorRange.maximum[1]);
   emit BFromUpdated(colorRange.minimum[2]);
   emit BToUpdated(colorRange.maximum[2]);
   osgParticle::rangef alphaRange = mLayers[mLayerIndex].mpParticle->getAlphaRange();
   emit AFromUpdated(alphaRange.minimum);
   emit AToUpdated(alphaRange.maximum);

   // Emitter UI
   emit EmitterLifeUpdated(mLayers[mLayerIndex].mModularEmitter->getLifeTime());
   emit EmitterStartUpdated(mLayers[mLayerIndex].mModularEmitter->getStartTime());
   emit EmitterResetUpdated(mLayers[mLayerIndex].mModularEmitter->getResetTime());
   emit EndlessLifetimeUpdated(mLayers[mLayerIndex].mModularEmitter->isEndless());
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateCounterTabsValues()
{
   osgParticle::Counter* counter = mLayers[mLayerIndex].mModularEmitter->getCounter();
   if(IS_A(counter, osgParticle::RandomRateCounter*))
   {
      emit CounterTypeBoxUpdated(0);
      UpdateRandomRatesValues();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateRandomRatesValues()
{
   osgParticle::RandomRateCounter* rrc =
      (osgParticle::RandomRateCounter*)mLayers[mLayerIndex].mModularEmitter->getCounter();
   osgParticle::rangef rateRange = rrc->getRateRange();
   emit RandomRateMinRateUpdated(rateRange.minimum);
   emit RandomRateMaxRateUpdated(rateRange.maximum);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdatePlacerTabsValues()
{
   osgParticle::Placer* placer = mLayers[mLayerIndex].mModularEmitter->getPlacer();
   if(IS_A(placer, osgParticle::PointPlacer*))
   {
      emit PlacerTypeBoxUpdated(0);
      UpdatePointPlacerValues();
   }
   else if(IS_A(placer, osgParticle::SectorPlacer*))
   {
      emit PlacerTypeBoxUpdated(1);
      UpdateSectorPlacerValues();
   }
   else if(IS_A(placer, osgParticle::SegmentPlacer*))
   {
      emit PlacerTypeBoxUpdated(2);
      UpdateSegmentPlacerValues();
   }
   else if(IS_A(placer, osgParticle::MultiSegmentPlacer*))
   {
      emit PlacerTypeBoxUpdated(3);
      UpdateMultiSegmentPlacerVertexList();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdatePointPlacerValues()
{
   osgParticle::PointPlacer* pp =(osgParticle::PointPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = pp->getCenter();
   emit PointPlacerXUpdated(center[0]);
   emit PointPlacerYUpdated(center[1]);
   emit PointPlacerZUpdated(center[2]);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateSectorPlacerValues()
{
   osgParticle::SectorPlacer* sp =(osgParticle::SectorPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 center = sp->getCenter();
   emit SectorPlacerXUpdated(center[0]);
   emit SectorPlacerYUpdated(center[1]);
   emit SectorPlacerZUpdated(center[2]);
   osgParticle::rangef radiusRange = sp->getRadiusRange();
   emit SectorPlacerMinRadiusUpdated(radiusRange.minimum);
   emit SectorPlacerMaxRadiusUpdated(radiusRange.maximum);
   osgParticle::rangef phiRange = sp->getPhiRange();
   emit SectorPlacerMinPhiUpdated(phiRange.minimum);
   emit SectorPlacerMaxPhiUpdated(phiRange.maximum);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateSegmentPlacerValues()
{
   osgParticle::SegmentPlacer* sp =(osgParticle::SegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertexA = sp->getVertexA();
   emit SegmentPlacerVertexAXUpdated(vertexA[0]);
   emit SegmentPlacerVertexAYUpdated(vertexA[1]);
   emit SegmentPlacerVertexAZUpdated(vertexA[2]);
   osg::Vec3 vertexB = sp->getVertexB();
   emit SegmentPlacerVertexBXUpdated(vertexB[0]);
   emit SegmentPlacerVertexBYUpdated(vertexB[1]);
   emit SegmentPlacerVertexBZUpdated(vertexB[2]);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateMultiSegmentPlacerVertexList()
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   emit ClearMultiSegmentPlacerVertexList();
   for(int i = 0; i < msp->numVertices(); ++i)
   {
      osg::Vec3 vertex = msp->getVertex(i);
      emit AddVertexToMultiSegmentPlacerVertexList(vertex[0], vertex[1], vertex[2]);
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateMultiSegmentPlacerValues()
{
   osgParticle::MultiSegmentPlacer* msp =
      (osgParticle::MultiSegmentPlacer*)mLayers[mLayerIndex].mModularEmitter->getPlacer();
   osg::Vec3 vertex = msp->getVertex(mMultiSegmentVertexIndex);
   emit MultiSegmentPlacerXUpdated(vertex[0]);
   emit MultiSegmentPlacerYUpdated(vertex[1]);
   emit MultiSegmentPlacerZUpdated(vertex[2]);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateShooterTabsValues()
{
   osgParticle::Shooter* shooter = mLayers[mLayerIndex].mModularEmitter->getShooter();
   if(IS_A(shooter, osgParticle::RadialShooter*))
   {
      emit ShooterTypeBoxUpdated(0);
      UpdateRadialShooterValues();
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateRadialShooterValues()
{
   osgParticle::RadialShooter* rs =(osgParticle::RadialShooter*)mLayers[mLayerIndex].mModularEmitter->getShooter();
   osgParticle::rangef elevationRange = rs->getThetaRange();
   emit RadialShooterElevationMinUpdated(elevationRange.minimum);
   emit RadialShooterElevationMaxUpdated(elevationRange.maximum);
   osgParticle::rangef azimuthRange = rs->getPhiRange();
   emit RadialShooterAzimuthMinUpdated(azimuthRange.minimum);
   emit RadialShooterAzimuthMaxUpdated(azimuthRange.maximum);
   osgParticle::rangef initialVelocityRange = rs->getInitialSpeedRange();
   emit RadialShooterInitialVelocityMinUpdated(initialVelocityRange.minimum);
   emit RadialShooterInitialVelocityMaxUpdated(initialVelocityRange.maximum);
   osgParticle::rangev3 rotationalRange = rs->getInitialRotationalSpeedRange();
   emit RadialShooterInitialMinRotationXUpdated(rotationalRange.minimum[0]);
   emit RadialShooterInitialMinRotationYUpdated(rotationalRange.minimum[1]);
   emit RadialShooterInitialMinRotationZUpdated(rotationalRange.minimum[2]);
   emit RadialShooterInitialMaxRotationXUpdated(rotationalRange.maximum[0]);
   emit RadialShooterInitialMaxRotationYUpdated(rotationalRange.maximum[1]);
   emit RadialShooterInitialMaxRotationZUpdated(rotationalRange.maximum[2]);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateProgramTabsValues()
{
   UpdateOperatorsList();
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateOperatorsList()
{
   emit ClearOperatorsList();
   for(int i = 0; i < mLayers[mLayerIndex].mModularProgram->numOperators(); ++i)
   {
      osgParticle::Operator* op = mLayers[mLayerIndex].mModularProgram->getOperator(i);
      if(IS_A(op, osgParticle::ForceOperator*))
      {
         emit AddOperatorToOperatorsList("Force");
      }
      else if(IS_A(op, osgParticle::AccelOperator*))
      {
         emit AddOperatorToOperatorsList("Acceleration");
      }
      else if(IS_A(op, osgParticle::FluidFrictionOperator*))
      {
         emit AddOperatorToOperatorsList("Fluid Friction");
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateForceValues()
{
   osgParticle::ForceOperator* fo = (osgParticle::ForceOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 force = fo->getForce();
   emit OperatorsForceXUpdated(force[0]);
   emit OperatorsForceYUpdated(force[1]);
   emit OperatorsForceZUpdated(force[2]);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateAccelerationValues()
{
   osgParticle::AccelOperator* ao = (osgParticle::AccelOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   osg::Vec3 acceleration = ao->getAcceleration();
   emit OperatorsAccelerationXUpdated(acceleration[0]);
   emit OperatorsAccelerationYUpdated(acceleration[1]);
   emit OperatorsAccelerationZUpdated(acceleration[2]);
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::UpdateFluidFrictionValues()
{
   osgParticle::FluidFrictionOperator* ffo = (osgParticle::FluidFrictionOperator*)mLayers[mLayerIndex].mModularProgram->getOperator(mOperatorsIndex);
   emit OperatorsFluidFrictionDensityUpdated(ffo->getFluidDensity());
   emit OperatorsFluidFrictionViscosityUpdated(ffo->getFluidViscosity());
   emit OperatorsFluidFrictionOverrideRadiusUpdated(ffo->getOverrideRadius());
}

////////////////////////////////////////////////////////////////////////////////
void ParticleViewer::SetTexturePaths(QString path, bool relativePath)
{
   QDir dir(path);

   for (int layerIndex = 0; layerIndex < (int)mLayers.size(); layerIndex++)
   {
      osgParticle::ParticleSystem* particleSystem = mLayers[layerIndex].mParticleSystem.get();
      if (particleSystem)
      {
         osg::StateSet* stateSet = particleSystem->getStateSet();
         if (stateSet)
         {
            osg::StateAttribute* stateAttribute = stateSet->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
            if (stateAttribute)
            {
               if (IS_A(stateAttribute, osg::Texture2D*))
               {
                  osg::Texture2D* texture = (osg::Texture2D*)stateAttribute;
                  osg::Image* image = texture->getImage();
                  if (image)
                  {
                     QString fileName;
                     if (relativePath)
                     {
                        QString textureFile = image->getFileName().c_str();
                        fileName = dir.relativeFilePath(textureFile);
                     }
                     else
                     {
                        QString textureFile = image->getFileName().c_str();
                        fileName = dir.absoluteFilePath(textureFile);
                        fileName = QDir::cleanPath(fileName);

                        //QFileInfo textureFile = image->getFileName().c_str();
                        //fileName = path;
                        //fileName += "/";
                        //fileName += textureFile.fileName();
                     }
                     image->setFileName(fileName.toStdString());
                  }
               }
            }
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void ParticleViewer::SetParticleLayerRenderBin(int value)
{
   if (mLayerIndex < mLayers.size())
   {
      mLayers[mLayerIndex].mParticleSystem->getOrCreateStateSet()->setBinNumber(value);
   }
}

bool ParticleViewer::KeyPressed( const dtCore::Keyboard* keyboard, int kc )
{
   //Nothing to do here, but we want to overwrite the inherited 
   //Quit-on-Escape behavior.
   return false;
}
////////////////////////////////////////////////////////////////////////////////
