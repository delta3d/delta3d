#include <delta3dthread.h>
#include <mainwindow.h>

#include <dtCore/camera.h>
#include <dtCore/globals.h>
#include <dtCore/scene.h>
#include <dtCore/compass.h>
#include <dtCore/system.h>
#include <dtUtil/fileutils.h>
//#include <dtUtil/log.h>
//#include <dtUtil/exception.h>

//#include <osg/BlendFunc>
//#include <osg/Geode>
//#include <osg/Geometry>
//#include <osg/PrimitiveSet>
//#include <osg/StateAttribute>
//#include <osg/Image>
//#include <osg/Material>
//#include <osg/Texture2D>
//
//#include <osgDB/ReadFile>
//#include <osgDB/WriteFile>
//
//#include <osgParticle/AccelOperator>
//#include <osgParticle/FluidFrictionOperator>
//#include <osgParticle/ForceOperator>
//#include <osgParticle/MultiSegmentPlacer>
//#include <osgParticle/PointPlacer>
//#include <osgParticle/RadialShooter>
//#include <osgParticle/RandomRateCounter>
//#include <osgParticle/SectorPlacer>
//#include <osgParticle/SegmentPlacer>
//
//#include <osgText/Text>

#include <QtGui/QApplication>
#include <QtGui/QMessageBox>

#include <sstream>
#include <string.h>
#include <math.h>

//using namespace dtCore;
/// @cond DOXYGEN_SHOULD_SKIP_THIS
//using namespace osgParticle;
/// @endcond

////////////////////////////////////////////////////////////////////////////////

#ifdef DELTA_WIN32
#include <Windows.h>
#define SLEEP(milliseconds) Sleep((milliseconds))
#else
#include <unistd.h>
#define SLEEP(milliseconds) usleep(((milliseconds) * 1000))
#endif

/////The Editor preferences - gets saved automatically when app exits
//Fl_Preferences appPrefs (Fl_Preferences::USER, "Delta3D", "ParticleEditor/preferences");

// Previous loaded file history
char absoluteHistory[5][1024];
char relativeHistory[5][1024];

////////////////////////////////////////////////////////////////////////////////
//
///**
// * Activates and updates the random rate counter parameters.
// */
//void updateRandomRateCounterParameters()
//{
//   int layer = app->Layers->value() - 1;
//
//   app->RandomRateCounterParameters->show();
//
//   RandomRateCounter* rrc =
//      (RandomRateCounter*)layers[layer].mModularEmitter->getCounter();
//
//   app->RandomRateCounter_MinRate->value(rrc->getRateRange().minimum);
//   app->RandomRateCounter_MaxRate->value(rrc->getRateRange().maximum);
//}
//
///**
// * Activates and updates the point placer parameters.
// */
//void updatePointPlacerParameters()
//{
//   int layer = app->Layers->value() - 1;
//
//   app->Placer_Type->value(0);
//
//   app->PointPlacerParameters->show();
//   app->SectorPlacerParameters->hide();
//   app->SegmentPlacerParameters->hide();
//   app->MultiSegmentPlacerParameters->hide();
//
//   PointPlacer* pp =
//      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   app->PointPlacer_X->value(pp->getCenter()[0]);
//   app->PointPlacer_Y->value(pp->getCenter()[1]);
//   app->PointPlacer_Z->value(pp->getCenter()[2]);
//}
//
///**
// * Activates and updates the sector placer parameters.
// */
//void updateSectorPlacerParameters()
//{
//   int layer = app->Layers->value() - 1;
//
//   app->Placer_Type->value(1);
//
//   app->PointPlacerParameters->hide();
//   app->SectorPlacerParameters->show();
//   app->SegmentPlacerParameters->hide();
//   app->MultiSegmentPlacerParameters->hide();
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   app->SectorPlacer_X->value(sp->getCenter()[0]);
//   app->SectorPlacer_Y->value(sp->getCenter()[1]);
//   app->SectorPlacer_Z->value(sp->getCenter()[2]);
//
//   app->SectorPlacer_MinRadius->value(sp->getRadiusRange().minimum);
//   app->SectorPlacer_MaxRadius->value(sp->getRadiusRange().maximum);
//
//   app->SectorPlacer_MinPhi->value(sp->getPhiRange().minimum);
//   app->SectorPlacer_MaxPhi->value(sp->getPhiRange().maximum);
//}
//
///**
// * Activates and updates the segment placer parameters.
// */
//void updateSegmentPlacerParameters()
//{
//   int layer = app->Layers->value() - 1;
//
//   app->Placer_Type->value(2);
//
//   app->PointPlacerParameters->hide();
//   app->SectorPlacerParameters->hide();
//   app->SegmentPlacerParameters->show();
//   app->MultiSegmentPlacerParameters->hide();
//
//   SegmentPlacer* sp =
//      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   app->SegmentPlacer_A_X->value(sp->getVertexA()[0]);
//   app->SegmentPlacer_A_Y->value(sp->getVertexA()[1]);
//   app->SegmentPlacer_A_Z->value(sp->getVertexA()[2]);
//
//   app->SegmentPlacer_B_X->value(sp->getVertexB()[0]);
//   app->SegmentPlacer_B_Y->value(sp->getVertexB()[1]);
//   app->SegmentPlacer_B_Z->value(sp->getVertexB()[2]);
//}
//
///**
// * Activates and updates the multi-segment placer parameters.
// */
//void updateMultiSegmentPlacerParameters()
//{
//   int layer = app->Layers->value() - 1;
//
//   app->Placer_Type->value(3);
//
//   app->PointPlacerParameters->hide();
//   app->SectorPlacerParameters->hide();
//   app->SegmentPlacerParameters->hide();
//   app->MultiSegmentPlacerParameters->show();
//
//   MultiSegmentPlacer* msp =
//      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   app->MultiSegmentPlacer_Vertices->clear();
//
//   char buf[80];
//
//   for (int i = 0; i < msp->numVertices(); ++i)
//   {
//      sprintf(buf, "%g, %g, %g",
//         msp->getVertex(i)[0],
//         msp->getVertex(i)[1],
//         msp->getVertex(i)[2]
//      );
//
//      app->MultiSegmentPlacer_Vertices->add(buf);
//   }
//
//   app->MultiSegmentPlacer_VertexParameters->hide();
//   app->MultiSegmentPlacer_DeleteVertex->deactivate();
//}
//
///**
// * Updates the state of the parameter tabs.
// */
//void updateParameterTabs()
//{
//   int layer = app->Layers->value() - 1;
//
//   if (layer >= 0)
//   {
//      app->ParameterTabs->activate();
//
//      app->Particles_Alignment->value(
//         layers[layer].mParticleSystem->getParticleAlignment()
//      );
//
//      app->Particles_Shape->value(
//         layers[layer].mParticle->getShape()
//      );
//
//      app->Particles_Lifetime->value(
//         layers[layer].mParticle->getLifeTime()
//      );
//
//      app->Particles_Radius->value(
//         layers[layer].mParticle->getRadius()
//      );
//
//      app->Particles_Mass->value(
//         layers[layer].mParticle->getMass()
//      );
//
//      app->Particles_MinSize->value(
//         layers[layer].mParticle->getSizeRange().minimum
//      );
//
//      app->Particles_MaxSize->value(
//         layers[layer].mParticle->getSizeRange().maximum
//      );
//
//      app->Particles_MinAlpha->value(
//         layers[layer].mParticle->getAlphaRange().minimum
//      );
//
//      app->Particles_MaxAlpha->value(
//         layers[layer].mParticle->getAlphaRange().maximum
//      );
//
//      app->Particles_MinR->value(
//         layers[layer].mParticle->getColorRange().minimum[0]
//      );
//
//      app->Particles_MinG->value(
//         layers[layer].mParticle->getColorRange().minimum[1]
//      );
//
//      app->Particles_MinB->value(
//         layers[layer].mParticle->getColorRange().minimum[2]
//      );
//
//      app->Particles_MinColor->color(
//         fl_rgb_color(
//            (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
//         )
//      );
//
//      app->Particles_MinColor->redraw();
//
//      app->Particles_MaxR->value(
//         layers[layer].mParticle->getColorRange().maximum[0]
//      );
//
//      app->Particles_MaxG->value(
//         layers[layer].mParticle->getColorRange().maximum[1]
//      );
//
//      app->Particles_MaxB->value(
//         layers[layer].mParticle->getColorRange().maximum[2]
//      );
//
//      app->Particles_MaxColor->color(
//         fl_rgb_color(
//            (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
//         )
//      );
//
//      app->Particles_MaxColor->redraw();
//
//      app->Particles_Emitter_Endless->value(layers[layer].mModularEmitter->isEndless());
//
//      app->Particles_Emitter_Lifetime->value(layers[layer].mModularEmitter->getLifeTime());
//
//      if (layers[layer].mModularEmitter->isEndless())
//      {
//         app->Particles_Emitter_Lifetime->deactivate();
//      }
//      else
//      {
//         app->Particles_Emitter_Lifetime->activate();
//      }
//
//      app->Particles_Emitter_StartTime->value(layers[layer].mModularEmitter->getStartTime());
//
//      app->Particles_Emitter_ResetTime->value(layers[layer].mModularEmitter->getResetTime());
//
//      std::string textureFile = "";
//
//      osg::StateSet* ss =
//         layers[layer].mParticleSystem->getStateSet();
//
//      osg::StateAttribute* sa =
//         ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE);
//
//      if (IS_A(sa, osg::Texture2D*))
//      {
//         osg::Texture2D* t2d = (osg::Texture2D*)sa;
//
//         osg::Image* image = t2d->getImage();
//
//         if (image != NULL)
//         {
//            textureFile = image->getFileName();
//         }
//      }
//
//      app->Particles_Texture->value(
//         textureFile.c_str()
//      );
//
//      if (!textureFile.empty())
//      {
//         app->Particles_TexturePreview->SetTexture(textureFile);
//      }
//
//      osg::BlendFunc* blend =
//         (osg::BlendFunc*)ss->getAttribute(osg::StateAttribute::BLENDFUNC);
//
//      app->Particles_Emissive->value(
//         blend->getDestination() == osg::BlendFunc::ONE
//      );
//
//      osg::Material* material =
//         (osg::Material*)ss->getAttribute(osg::StateAttribute::MATERIAL);
//
//      app->Particles_Lighting->value(
//         material->getColorMode() == osg::Material::AMBIENT_AND_DIFFUSE
//      );
//
//
//      Counter* counter = layers[layer].mModularEmitter->getCounter();
//
//      if (IS_A(counter, RandomRateCounter*))
//      {
//         updateRandomRateCounterParameters();
//      }
//
//
//      Placer* placer = layers[layer].mModularEmitter->getPlacer();
//
//      if (IS_A(placer, PointPlacer*))
//      {
//         updatePointPlacerParameters();
//      }
//      else if (IS_A(placer, SectorPlacer*))
//      {
//         updateSectorPlacerParameters();
//      }
//      else if (IS_A(placer, SegmentPlacer*))
//      {
//         updateSegmentPlacerParameters();
//      }
//      else if (IS_A(placer, MultiSegmentPlacer*))
//      {
//         updateMultiSegmentPlacerParameters();
//      }
//
//
//      Shooter* shooter = layers[layer].mModularEmitter->getShooter();
//
//      if (IS_A(shooter, RadialShooter*))
//      {
//         app->RadialShooterParameters->show();
//
//         RadialShooter* rs = (RadialShooter*)shooter;
//
//         app->RadialShooter_MinTheta->value(rs->getThetaRange().minimum);
//         app->RadialShooter_MaxTheta->value(rs->getThetaRange().maximum);
//
//         app->RadialShooter_MinPhi->value(rs->getPhiRange().minimum);
//         app->RadialShooter_MaxPhi->value(rs->getPhiRange().maximum);
//
//         app->RadialShooter_MinInitialSpeed->value(rs->getInitialSpeedRange().minimum);
//         app->RadialShooter_MaxInitialSpeed->value(rs->getInitialSpeedRange().maximum);
//
//         app->RadialShooter_MinInitialRotationalSpeedX->value(
//            rs->getInitialRotationalSpeedRange().minimum[0]
//         );
//
//         app->RadialShooter_MinInitialRotationalSpeedY->value(
//            rs->getInitialRotationalSpeedRange().minimum[1]
//         );
//
//         app->RadialShooter_MinInitialRotationalSpeedZ->value(
//            rs->getInitialRotationalSpeedRange().minimum[2]
//         );
//
//         app->RadialShooter_MaxInitialRotationalSpeedX->value(
//            rs->getInitialRotationalSpeedRange().maximum[0]
//         );
//
//         app->RadialShooter_MaxInitialRotationalSpeedY->value(
//            rs->getInitialRotationalSpeedRange().maximum[1]
//         );
//
//         app->RadialShooter_MaxInitialRotationalSpeedZ->value(
//            rs->getInitialRotationalSpeedRange().maximum[2]
//         );
//      }
//
//
//      app->Program_Operators->clear();
//
//      psEditor_Program_OperatorsChanged(NULL, NULL);
//
//      for (int i = 0; i < layers[layer].mModularProgram->numOperators(); ++i)
//      {
//         Operator* op = layers[layer].mModularProgram->getOperator(i);
//
//         if (IS_A(op, FluidFrictionOperator*))
//         {
//            app->Program_Operators->add("Fluid Friction");
//         }
//         else if (IS_A(op, ForceOperator*))
//         {
//            app->Program_Operators->add("Force");
//         }
//         else if (IS_A(op, AccelOperator*))
//         {
//            app->Program_Operators->add("Acceleration");
//         }
//      }
//   }
//   else
//   {
//      app->ParameterTabs->deactivate();
//   }
//}
//
//// Update file history from preferences...
//void UpdateHistory(const std::string filename)
//{
//   int   i;      // Looping var
//   char  absolute[1024];
//
//   fl_filename_absolute(absolute, sizeof(absolute), filename.c_str());
//
//   for (i = 0; i < 5; ++i)
//   {
//      if (!strcmp(absolute, absoluteHistory[i])) break;
//   }
//
//   if (i == 0) { return; }
//
//   if (i >= 5) { i = 4;  }
//
//   // Move the other filenames down in the list...
//   memmove(absoluteHistory + 1, absoluteHistory, i * sizeof(absoluteHistory[0]));
//   memmove(relativeHistory + 1, relativeHistory, i * sizeof(relativeHistory[0]));
//
//   // Put the new file at the top...
//   strncpy(absoluteHistory[0], absolute, sizeof(absoluteHistory[0]));
//
//   fl_filename_relative(relativeHistory[0], sizeof(relativeHistory[0]),
//      absoluteHistory[0]);
//
//   // Update the menu items as needed...
//   for (i = 0; i < 5; ++i)
//   {
//      appPrefs.set( Fl_Preferences::Name("file%d", i), absoluteHistory[i]);
//      if (absoluteHistory[i][0])
//      {
//         UserInterface::menu_MainMenu[i + 4].flags = 0;
//         UserInterface::menu_MainMenu[i + 4].label( );
//      }
//      else
//      {
//         UserInterface::menu_MainMenu[i + 4].flags = FL_MENU_INVISIBLE;
//      }
//   }
//
//   UserInterface::menu_MainMenu[3].flags &= ~FL_MENU_INACTIVE;
//}
//
//void setParticleSystemFilename(const std::string& newFilename)
//{
//   particleSystemFilename = newFilename;
//
//   if (particleSystemFilename == "")
//   {
//      app->UIMainWindow->label("Particle System Editor");
//   }
//   else
//   {
//      char buf[256];
//
//      sprintf(buf, "Particle System Editor (%s)", newFilename.c_str());
//
//      app->UIMainWindow->label(buf);
//      UpdateHistory(newFilename);
//   }
//}
//
//
//void psEditorGUI_LayerSelect(Fl_Browser*, void*)
//{
//   if (app->Layers->value() == 0)
//   {
//      app->Layers_DeleteButton->deactivate();
//      app->Layers_RenameButton->deactivate();
//      app->Layers_HideButton->deactivate();
//   }
//   else
//   {
//      int layer = app->Layers->value() - 1;
//
//      app->Layers_DeleteButton->activate();
//      app->Layers_RenameButton->activate();
//      app->Layers_HideButton->activate();
//
//      if (layers[layer].mModularEmitter->isEnabled())
//      {
//         //Layers_HideButton->label("Hide");
//         app->Layers_HideButton->value(1);
//      }
//      else
//      {
//         //Layers_HideButton->label("Show");
//         app->Layers_HideButton->value(0);
//      }
//   }
//
//   updateParameterTabs();
//}
//
//
/////Load the given filename
//void LoadFile( std::string filename, bool import = false )
//{
//   if (!filename.empty())
//   {
//      osg::Node* node = osgDB::readNodeFile(filename.c_str());
//
//      if (node == NULL || !IS_A(node, osg::Group*))
//      {
//         fl_alert("Invalid particle system: %s", filename.c_str());
//
//         return;
//      }
//
//      osg::Group* newParticleSystemGroup = (osg::Group*)node;
//
//      std::vector<ParticleSystemLayer> newLayers;
//
//      ParticleSystemUpdater* newParticleSystemUpdater = NULL;
//
//      unsigned int i;
//
//      unsigned int OldWay = 0;
//
//      for (i = 0; i < newParticleSystemGroup->getNumChildren(); ++i)
//      {
//         node = newParticleSystemGroup->getChild(i);
//
//         // OLD PARTICLE CODE HERE
//         if (IS_A(node, osg::Group*))
//         {
//            ++OldWay;
//         }
//      }
//
//      // OLD SCHOOLZ LOLLERZDUCK, QUACK QUACK
//      if (newParticleSystemGroup->getNumChildren() == OldWay)
//      {
//         for (unsigned int j = 0; j < OldWay; ++j)
//         {
//            node = newParticleSystemGroup->getChild(j);
//            osg::Group* newerParticleSystemGroup = (osg::Group*)node;
//
//            for (i = 0; i < newerParticleSystemGroup->getNumChildren(); ++i)
//            {
//               node = newerParticleSystemGroup->getChild(i);
//               if (IS_A(node, osg::Geode*))
//               {
//                  ParticleSystemLayer layer;
//
//                  layer.mGeode = (osg::Geode*)node;
//
//                  for (unsigned int j = 0; j < layer.mGeode->getNumDrawables(); ++j)
//                  {
//                     osg::Drawable* drawable = layer.mGeode->getDrawable(j);
//
//                     if (IS_A(drawable, osgParticle::ParticleSystem*))
//                     {
//                        layer.mParticleSystem = (osgParticle::ParticleSystem*)drawable;
//
//                        layer.mParticle = new Particle(
//                           layer.mParticleSystem->getDefaultParticleTemplate()
//                        );
//                     }
//                  }
//
//                  if (layer.mGeode->getName() == "")
//                  {
//                     char buf[256];
//
//                     sprintf(buf, "Layer %u", unsigned(newLayers.size() + (import ? layers.size() : 0)));
//
//                     layer.mGeode->setName(buf);
//                  }
//
//                  newLayers.push_back(layer);
//               }
//               else if (IS_A(node, ParticleSystemUpdater*))
//               {
//                  newParticleSystemUpdater = static_cast<ParticleSystemUpdater*>(node);
//               }
//            }
//
//            for (i = 0; i < newerParticleSystemGroup->getNumChildren(); ++i)
//            {
//               node = newerParticleSystemGroup->getChild(i);
//
//               if (IS_A(node, osg::MatrixTransform*))
//               {
//                  osg::MatrixTransform* newEmitterTransform = static_cast<osg::MatrixTransform*>(node);
//
//                  for (unsigned int j = 0; j < newEmitterTransform->getNumChildren(); ++j)
//                  {
//                     osg::Node* childNode = newEmitterTransform->getChild(j);
//
//                     if (IS_A(childNode, ModularEmitter*))
//                     {
//                        osgParticle::ModularEmitter* newModularEmitter = (ModularEmitter*)childNode;
//
//                        for (unsigned int k = 0; k < newLayers.size(); ++k)
//                        {
//                           if (newLayers[k].mParticleSystem == newModularEmitter->getParticleSystem())
//                           {
//                              newLayers[k].mEmitterTransform = newEmitterTransform;
//                              newLayers[k].mModularEmitter = newModularEmitter;
//                              break;
//                           }
//                        }
//                     }
//                  }
//               }
//               else if (IS_A(node, ModularProgram*))
//               {
//                  osgParticle::ModularProgram* newModularProgram = static_cast<ModularProgram*>(node);
//
//                  for (unsigned int j = 0; j < newLayers.size(); ++j)
//                  {
//                     if (newLayers[j].mParticleSystem == newModularProgram->getParticleSystem())
//                     {
//                        newLayers[j].mModularProgram = newModularProgram;
//                        break;
//                     }
//                  }
//               }
//            }
//         }
//      }
//      // end old way
//      else
//      {
//         for (i = 0; i < newParticleSystemGroup->getNumChildren(); ++i)
//         {
//            node = newParticleSystemGroup->getChild(i);
//            if (IS_A(node, osg::Geode*))
//            {
//               ParticleSystemLayer layer;
//
//               layer.mGeode = (osg::Geode*)node;
//
//               for (unsigned int j = 0; j < layer.mGeode->getNumDrawables(); ++j)
//               {
//                  osg::Drawable* drawable = layer.mGeode->getDrawable(j);
//
//                  if (IS_A(drawable, osgParticle::ParticleSystem*))
//                  {
//                     layer.mParticleSystem = (osgParticle::ParticleSystem*)drawable;
//
//                     layer.mParticle = new Particle(
//                        layer.mParticleSystem->getDefaultParticleTemplate()
//                     );
//                  }
//               }
//
//               if (layer.mGeode->getName() == "")
//               {
//                  char buf[256];
//
//                  sprintf(buf, "Layer %u", unsigned(newLayers.size() + (import ? layers.size() : 0)));
//
//                  layer.mGeode->setName(buf);
//               }
//
//               newLayers.push_back(layer);
//            }
//            else if (IS_A(node, ParticleSystemUpdater*))
//            {
//               newParticleSystemUpdater = static_cast<ParticleSystemUpdater*>(node);
//            }
//         }
//
//         for (i = 0; i < newParticleSystemGroup->getNumChildren(); ++i)
//         {
//            node = newParticleSystemGroup->getChild(i);
//
//            if (IS_A(node, osg::MatrixTransform*))
//            {
//               osg::MatrixTransform* newEmitterTransform = static_cast<osg::MatrixTransform*>(node);
//
//               for (unsigned int j = 0; j < newEmitterTransform->getNumChildren(); ++j)
//               {
//                  osg::Node* childNode = newEmitterTransform->getChild(j);
//
//                  if (IS_A(childNode, ModularEmitter*))
//                  {
//                     osgParticle::ModularEmitter* newModularEmitter = (ModularEmitter*)childNode;
//
//                     for (unsigned int k = 0; k < newLayers.size(); ++k)
//                     {
//                        if (newLayers[k].mParticleSystem == newModularEmitter->getParticleSystem())
//                        {
//                           newLayers[k].mEmitterTransform = newEmitterTransform;
//                           newLayers[k].mModularEmitter = newModularEmitter;
//                           break;
//                        }
//                     }
//                  }
//               }
//            }
//            else if (IS_A(node, ModularProgram*))
//            {
//               osgParticle::ModularProgram* newModularProgram = static_cast<ModularProgram*>(node);
//
//               for (unsigned int j = 0; j < newLayers.size(); ++j)
//               {
//                  if (newLayers[j].mParticleSystem == newModularProgram->getParticleSystem())
//                  {
//                     newLayers[j].mModularProgram = newModularProgram;
//                     break;
//                  }
//               }
//            }
//         }
//      }
//      if (newParticleSystemUpdater != NULL)
//      {
//         int newLayer = 1;
//
//         if (import)
//         {
//            newLayer = layers.size() + 1;
//
//            for (i = 0; i < newLayers.size(); ++i)
//            {
//               particleSystemGroup->addChild(newLayers[i].mGeode.get());
//               particleSystemGroup->addChild(newLayers[i].mEmitterTransform.get());
//               particleSystemGroup->addChild(newLayers[i].mModularProgram.get());
//
//               particleSystemUpdater->addParticleSystem(newLayers[i].mParticleSystem.get());
//
//               layers.push_back(newLayers[i]);
//            }
//
//            resetEmitters();
//         }
//         else
//         {
//            sceneGroup->removeChild(particleSystemGroup);
//
//            particleSystemGroup = newParticleSystemGroup;
//
//            layers = newLayers;
//
//            particleSystemUpdater = newParticleSystemUpdater;
//
//            sceneGroup->addChild(particleSystemGroup);
//         }
//
//         updateLayers();
//
//         app->Layers->value(newLayer);
//
//         psEditorGUI_LayerSelect(NULL, NULL);
//
//         setParticleSystemFilename(filename);
//      }
//      else
//      {
//         fl_alert("Invalid particle system: %s", filename.c_str());
//      }
//   }
//}
//
//void psEditorGUI_Open(Fl_Menu_*, void*)
//{
//   char* filename =
//      fl_file_chooser(
//      "Open",
//      "Particle Systems (*.osg)",
//      particleSystemFilename.c_str(),
//      1
//      );
//
//   if (filename != NULL)
//   {
//      std::string name = filename;
//      LoadFile(name);
//   }
//}
//
//void psEditorGUI_Import(Fl_Menu_*, void*)
//{
//   char* filename =
//      fl_file_chooser(
//      "Import",
//      "Particle Systems (*.osg)",
//      particleSystemFilename.c_str(),
//      1
//      );
//
//   if (filename != NULL)
//   {
//      std::string name = filename;
//      LoadFile(name, true);
//   }
//}
//
///// Load a file to display as a reference 3D object
//void psEditorGUI_LoadReference(Fl_Menu_*, void*)
//{
//   char* filename =
//      fl_file_chooser(
//      "Load",
//      "Geometry Files (*.{osg,ive})\tOSG Files (*.osg)\tIVE Files (*.ive)",
//      particleSystemFilename.c_str(),
//      1
//      );
//
//   if (filename != NULL)
//   {
//      std::string name = filename;
//      if (referenceModel.valid())
//      {
//         //remove the existing one from the scene
//        sceneGroup->removeChild(referenceModel.get());
//      }
//
//      referenceModel = osgDB::readNodeFile(name);
//
//      if (referenceModel.valid())
//      {
//         sceneGroup->addChild( referenceModel.get() );
//      }
//      else
//      {
//         fl_alert("Can't load geometry file: %s", name.c_str());
//      }
//   }
//
//}
//
//void psEditorGUI_Save(Fl_Menu_*, void*)
//{
//   if (particleSystemFilename == "")
//   {
//      psEditorGUI_SaveAs(NULL, NULL);
//   }
//   else
//   {
//      resetEmitters();
//
//      osgDB::writeNodeFile(*particleSystemGroup, particleSystemFilename);
//   }
//}
//
//void psEditorGUI_SaveAs(Fl_Menu_*, void*)
//{
//   char* filename =
//      fl_file_chooser(
//         "Save As",
//         "Particle Systems (*.osg)",
//         particleSystemFilename.c_str(),
//         1
//      );
//
//   if (filename != NULL)
//   {
//      setParticleSystemFilename(filename);
//
//      psEditorGUI_Save(NULL, NULL);
//   }
//}
//
//void psEditorGUI_Quit(Fl_Menu_*, void*)
//{
//   exit(0);
//}
//
//void psEditorGUI_Compass(Fl_Menu_*, void*)
//{
//   compassTransform->setNodeMask(
//      compassTransform->getNodeMask() ?
//         0x0 : 0xFFFFFFFF
//   );
//}
//
//void psEditorGUI_XYGrid(Fl_Menu_*, void*)
//{
//   xyGridTransform->setNodeMask(
//      xyGridTransform->getNodeMask() ?
//         0x0 : 0xFFFFFFFF
//   );
//}
//
//void psEditorGUI_YZGrid(Fl_Menu_*, void*)
//{
//   yzGridTransform->setNodeMask(
//      yzGridTransform->getNodeMask() ?
//         0x0 : 0xFFFFFFFF
//   );
//}
//
//void psEditorGUI_XZGrid(Fl_Menu_*, void*)
//{
//   xzGridTransform->setNodeMask(
//      xzGridTransform->getNodeMask() ?
//         0x0 : 0xFFFFFFFF
//   );
//}
//
//void psEditorGUI_Particles_SetAlignment(Fl_Choice*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticleSystem->setParticleAlignment(
//      (osgParticle::ParticleSystem::Alignment)app->Particles_Alignment->value()
//   );
//}
//
//void psEditorGUI_Particles_SetShape(Fl_Choice*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setShape(
//      (Particle::Shape)app->Particles_Shape->value()
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetLifetime(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setLifeTime(
//      app->Particles_Lifetime->value()
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMinSize(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setSizeRange(
//      rangef(
//         app->Particles_MinSize->value(),
//         app->Particles_MaxSize->value()
//      )
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMaxSize(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setSizeRange(
//      rangef(
//         app->Particles_MinSize->value(),
//         app->Particles_MaxSize->value()
//      )
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMinAlpha(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setAlphaRange(
//      rangef(
//         app->Particles_MinAlpha->value(),
//         app->Particles_MaxAlpha->value()
//      )
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMaxAlpha(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setAlphaRange(
//      rangef(
//         app->Particles_MinAlpha->value(),
//         app->Particles_MaxAlpha->value()
//      )
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetRadius(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setRadius(
//      app->Particles_Radius->value()
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMass(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setMass(
//      app->Particles_Mass->value()
//   );
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetTexture(Fl_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticleSystem->setDefaultAttributes(
//      app->Particles_Texture->value(),
//      app->Particles_Emissive->value(),
//      app->Particles_Lighting->value()
//   );
//
//   app->Particles_TexturePreview->SetTexture(app->Particles_Texture->value());
//}
//
//void psEditorGUI_Particles_ChooseTexture(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   char* filename =
//      fl_file_chooser(
//         "Choose Texture",
//         "Image Files (*.{bmp,dds,gif,jpg,pic,png,rgb,tga,tiff})",
//         app->Particles_Texture->value(),
//         0
//      );
//
//   if (filename != NULL)
//   {
//      app->Particles_Texture->value(filename);
//
//      app->Particles_TexturePreview->SetTexture(filename);
//
//      layers[layer].mParticleSystem->setDefaultAttributes(
//         app->Particles_Texture->value(),
//         app->Particles_Emissive->value(),
//         app->Particles_Lighting->value()
//      );
//   }
//}
//
//void psEditorGUI_Particles_SetEmissive(Fl_Check_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticleSystem->setDefaultAttributes(
//      app->Particles_Texture->value(),
//      app->Particles_Emissive->value(),
//      app->Particles_Lighting->value()
//   );
//}
//
//void psEditorGUI_Particles_SetLighting(Fl_Check_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticleSystem->setDefaultAttributes(
//      app->Particles_Texture->value(),
//      app->Particles_Emissive->value(),
//      app->Particles_Lighting->value()
//   );
//}
//
//void psEditorGUI_Particles_SetMinR(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setColorRange(
//      rangev4(
//         osg::Vec4(
//            app->Particles_MinR->value(),
//            app->Particles_MinG->value(),
//            app->Particles_MinB->value(),
//            1.0
//         ),
//         osg::Vec4(
//            app->Particles_MaxR->value(),
//            app->Particles_MaxG->value(),
//            app->Particles_MaxB->value(),
//            1.0
//         )
//      )
//   );
//
//   app->Particles_MinColor->color(
//      fl_rgb_color(
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
//      )
//   );
//
//   app->Particles_MinColor->redraw();
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMinG(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setColorRange(
//      rangev4(
//         osg::Vec4(
//            app->Particles_MinR->value(),
//            app->Particles_MinG->value(),
//            app->Particles_MinB->value(),
//            1.0
//         ),
//         osg::Vec4(
//            app->Particles_MaxR->value(),
//            app->Particles_MaxG->value(),
//            app->Particles_MaxB->value(),
//            1.0
//         )
//      )
//   );
//
//   app->Particles_MinColor->color(
//      fl_rgb_color(
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
//      )
//   );
//
//   app->Particles_MinColor->redraw();
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMinB(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setColorRange(
//      rangev4(
//         osg::Vec4(
//            app->Particles_MinR->value(),
//            app->Particles_MinG->value(),
//            app->Particles_MinB->value(),
//            1.0
//         ),
//         osg::Vec4(
//            app->Particles_MaxR->value(),
//            app->Particles_MaxG->value(),
//            app->Particles_MaxB->value(),
//            1.0
//         )
//      )
//   );
//
//   app->Particles_MinColor->color(
//      fl_rgb_color(
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
//      )
//   );
//
//   app->Particles_MinColor->redraw();
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_ChooseMinColor(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   double r = app->Particles_MinR->value(),
//          g = app->Particles_MinG->value(),
//          b = app->Particles_MinB->value();
//
//   if (fl_color_chooser("Choose Min. Color", r, g, b))
//   {
//      app->Particles_MinR->value(r);
//      app->Particles_MinG->value(g);
//      app->Particles_MinB->value(b);
//
//      layers[layer].mParticle->setColorRange(
//         rangev4(
//            osg::Vec4(
//               app->Particles_MinR->value(),
//               app->Particles_MinG->value(),
//               app->Particles_MinB->value(),
//               1.0
//            ),
//            osg::Vec4(
//               app->Particles_MaxR->value(),
//               app->Particles_MaxG->value(),
//               app->Particles_MaxB->value(),
//               1.0
//            )
//         )
//      );
//
//      app->Particles_MinColor->color(
//         fl_rgb_color(
//            (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
//         )
//      );
//
//      app->Particles_MinColor->redraw();
//
//      layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//   }
//}
//
//void psEditorGUI_Particles_SetMaxR(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setColorRange(
//      rangev4(
//         osg::Vec4(
//            app->Particles_MinR->value(),
//            app->Particles_MinG->value(),
//            app->Particles_MinB->value(),
//            1.0
//         ),
//         osg::Vec4(
//            app->Particles_MaxR->value(),
//            app->Particles_MaxG->value(),
//            app->Particles_MaxB->value(),
//            1.0
//         )
//      )
//   );
//
//   app->Particles_MaxColor->color(
//      fl_rgb_color(
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
//      )
//   );
//
//   app->Particles_MaxColor->redraw();
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMaxG(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setColorRange(
//      rangev4(
//         osg::Vec4(
//            app->Particles_MinR->value(),
//            app->Particles_MinG->value(),
//            app->Particles_MinB->value(),
//            1.0
//         ),
//         osg::Vec4(
//            app->Particles_MaxR->value(),
//            app->Particles_MaxG->value(),
//            app->Particles_MaxB->value(),
//            1.0
//         )
//      )
//   );
//
//   app->Particles_MaxColor->color(
//      fl_rgb_color(
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
//      )
//   );
//
//   app->Particles_MaxColor->redraw();
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_SetMaxB(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mParticle->setColorRange(
//      rangev4(
//         osg::Vec4(
//            app->Particles_MinR->value(),
//            app->Particles_MinG->value(),
//            app->Particles_MinB->value(),
//            1.0
//         ),
//         osg::Vec4(
//            app->Particles_MaxR->value(),
//            app->Particles_MaxG->value(),
//            app->Particles_MaxB->value(),
//            1.0
//         )
//      )
//   );
//
//   app->Particles_MaxColor->color(
//      fl_rgb_color(
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
//         (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
//      )
//   );
//
//   app->Particles_MaxColor->redraw();
//
//   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//}
//
//void psEditorGUI_Particles_ChooseMaxColor(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   double r = app->Particles_MaxR->value(),
//          g = app->Particles_MaxG->value(),
//          b = app->Particles_MaxB->value();
//
//   if (fl_color_chooser("Choose Max. Color", r, g, b))
//   {
//      app->Particles_MaxR->value(r);
//      app->Particles_MaxG->value(g);
//      app->Particles_MaxB->value(b);
//
//      layers[layer].mParticle->setColorRange(
//         rangev4(
//            osg::Vec4(
//               app->Particles_MinR->value(),
//               app->Particles_MinG->value(),
//               app->Particles_MinB->value(),
//               1.0
//            ),
//            osg::Vec4(
//               app->Particles_MaxR->value(),
//               app->Particles_MaxG->value(),
//               app->Particles_MaxB->value(),
//               1.0
//            )
//         )
//      );
//
//      app->Particles_MaxColor->color(
//         fl_rgb_color(
//            (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
//            (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
//         )
//      );
//
//      app->Particles_MaxColor->redraw();
//
//      layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
//   }
//}
//
//void psEditorGUI_Particles_SetEmitterEndless(Fl_Check_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mModularEmitter->setEndless(
//      app->Particles_Emitter_Endless->value()
//   );
//
//   if (layers[layer].mModularEmitter->isEndless())
//   {
//      app->Particles_Emitter_Lifetime->deactivate();
//   }
//   else
//   {
//      app->Particles_Emitter_Lifetime->activate();
//   }
//}
//
//void psEditorGUI_Particles_SetEmitterLifetime(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mModularEmitter->setLifeTime(
//      app->Particles_Emitter_Lifetime->value()
//   );
//}
//
//void psEditorGUI_Particles_SetEmitterStartTime(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mModularEmitter->setStartTime(
//      app->Particles_Emitter_StartTime->value()
//   );
//}
//
//void psEditorGUI_Particles_SetEmitterResetTime(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   layers[layer].mModularEmitter->setResetTime(
//      app->Particles_Emitter_ResetTime->value()
//   );
//}
//
//void psEditorGUI_RandomRateCounter_SetMinRate(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RandomRateCounter* rrc =
//      (RandomRateCounter*)layers[layer].mModularEmitter->getCounter();
//
//   rrc->setRateRange(
//      rangef(
//         app->RandomRateCounter_MinRate->value(),
//         app->RandomRateCounter_MaxRate->value()
//      )
//   );
//}
//
//void psEditorGUI_RandomRateCounter_SetMaxRate(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RandomRateCounter* rrc =
//      (RandomRateCounter*)layers[layer].mModularEmitter->getCounter();
//
//   rrc->setRateRange(
//      rangef(
//         app->RandomRateCounter_MinRate->value(),
//         app->RandomRateCounter_MaxRate->value()
//      )
//   );
//}
//
//void psEditorGUI_Counter_SetType(Fl_Choice* counterType, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   Counter* counter = layers[layer].mModularEmitter->getCounter();
//
//   switch (counterType->value())
//   {
//      case 0:
//         if (!IS_A(counter, RandomRateCounter*))
//         {
//            layers[layer].mModularEmitter->setCounter(
//               new RandomRateCounter()
//            );
//
//            updateRandomRateCounterParameters();
//         }
//         break;
//   }
//}
//
//void psEditor_SegmentPlacer_VertexA_SetX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SegmentPlacer* sp =
//      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setVertexA(
//      app->SegmentPlacer_A_X->value(),
//      app->SegmentPlacer_A_Y->value(),
//      app->SegmentPlacer_A_Z->value()
//   );
//}
//
//void psEditor_SegmentPlacer_VertexA_SetY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SegmentPlacer* sp =
//      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setVertexA(
//      app->SegmentPlacer_A_X->value(),
//      app->SegmentPlacer_A_Y->value(),
//      app->SegmentPlacer_A_Z->value()
//   );
//}
//
//void psEditor_SegmentPlacer_VertexA_SetZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SegmentPlacer* sp =
//      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setVertexA(
//      app->SegmentPlacer_A_X->value(),
//      app->SegmentPlacer_A_Y->value(),
//      app->SegmentPlacer_A_Z->value()
//   );
//}
//
//void psEditor_SegmentPlacer_VertexB_SetX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SegmentPlacer* sp =
//      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setVertexB(
//      app->SegmentPlacer_B_X->value(),
//      app->SegmentPlacer_B_Y->value(),
//      app->SegmentPlacer_B_Z->value()
//   );
//}
//
//void psEditor_SegmentPlacer_VertexB_SetY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SegmentPlacer* sp =
//      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setVertexB(
//      app->SegmentPlacer_B_X->value(),
//      app->SegmentPlacer_B_Y->value(),
//      app->SegmentPlacer_B_Z->value()
//   );
//}
//
//void psEditor_SegmentPlacer_VertexB_SetZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SegmentPlacer* sp =
//      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setVertexB(
//      app->SegmentPlacer_B_X->value(),
//      app->SegmentPlacer_B_Y->value(),
//      app->SegmentPlacer_B_Z->value()
//   );
//}
//
//void psEditorGUI_SectorPlacer_Center_SetX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setCenter(
//      app->SectorPlacer_X->value(),
//      app->SectorPlacer_Y->value(),
//      app->SectorPlacer_Z->value()
//   );
//}
//
//void psEditorGUI_SectorPlacer_Center_SetY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setCenter(
//      app->SectorPlacer_X->value(),
//      app->SectorPlacer_Y->value(),
//      app->SectorPlacer_Z->value()
//   );
//}
//
//void psEditorGUI_SectorPlacer_Center_SetZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setCenter(
//      app->SectorPlacer_X->value(),
//      app->SectorPlacer_Y->value(),
//      app->SectorPlacer_Z->value()
//   );
//}
//
//void psEditorGUI_SectorPlacer_SetMinRadius(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setRadiusRange(
//      app->SectorPlacer_MinRadius->value(),
//      app->SectorPlacer_MaxRadius->value()
//   );
//}
//
//void psEditorGUI_SectorPlacer_SetMaxRadius(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setRadiusRange(
//      app->SectorPlacer_MinRadius->value(),
//      app->SectorPlacer_MaxRadius->value()
//   );
//}
//
//void psEditorGUI_SectorPlacer_SetMinPhi(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setPhiRange(
//      app->SectorPlacer_MinPhi->value(),
//      app->SectorPlacer_MaxPhi->value()
//   );
//}
//
//void psEditorGUI_SectorPlacer_SetMaxPhi(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   SectorPlacer* sp =
//      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   sp->setPhiRange(
//      app->SectorPlacer_MinPhi->value(),
//      app->SectorPlacer_MaxPhi->value()
//   );
//}
//
//void psEditorGUI_PointPlacer_SetX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   PointPlacer* pp =
//      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   pp->setCenter(
//      app->PointPlacer_X->value(),
//      app->PointPlacer_Y->value(),
//      app->PointPlacer_Z->value()
//   );
//}
//
//void psEditorGUI_PointPlacer_SetY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   PointPlacer* pp =
//      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   pp->setCenter(
//      app->PointPlacer_X->value(),
//      app->PointPlacer_Y->value(),
//      app->PointPlacer_Z->value()
//   );
//}
//
//void psEditorGUI_PointPlacer_SetZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   PointPlacer* pp =
//      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   pp->setCenter(
//      app->PointPlacer_X->value(),
//      app->PointPlacer_Y->value(),
//      app->PointPlacer_Z->value()
//   );
//}
//
//void psEditorGUI_MultiSegmentPlacer_VerticesChanged(Fl_Browser*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   MultiSegmentPlacer* msp =
//      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   int i = app->MultiSegmentPlacer_Vertices->value() - 1;
//
//   if (i >= 0)
//   {
//      app->MultiSegmentPlacer_VertexParameters->show();
//      app->MultiSegmentPlacer_DeleteVertex->activate();
//
//      app->MultiSegmentPlacer_X->value(msp->getVertex(i)[0]);
//      app->MultiSegmentPlacer_Y->value(msp->getVertex(i)[1]);
//      app->MultiSegmentPlacer_Z->value(msp->getVertex(i)[2]);
//   }
//   else
//   {
//      app->MultiSegmentPlacer_VertexParameters->hide();
//      app->MultiSegmentPlacer_DeleteVertex->deactivate();
//   }
//}
//
//void psEditorGUI_MultiSegmentPlacer_AddVertex(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   MultiSegmentPlacer* msp =
//      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   app->MultiSegmentPlacer_Vertices->add("0, 0, 0");
//
//   msp->addVertex(0, 0, 0);
//
//   app->MultiSegmentPlacer_Vertices->value(msp->numVertices());
//
//   app->MultiSegmentPlacer_Vertices->do_callback();
//}
//
//void psEditorGUI_MultiSegmentPlacer_DeleteVertex(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   MultiSegmentPlacer* msp =
//      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   int i = app->MultiSegmentPlacer_Vertices->value() - 1;
//
//   msp->removeVertex(i);
//
//   app->MultiSegmentPlacer_Vertices->remove(i+1);
//
//   if (i < app->MultiSegmentPlacer_Vertices->size())
//   {
//      app->MultiSegmentPlacer_Vertices->value(i+1);
//   }
//
//   app->MultiSegmentPlacer_Vertices->do_callback();
//}
//
//void psEditorGUI_MultiSegmentPlacer_Vertex_SetX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   MultiSegmentPlacer* msp =
//      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   int i = app->MultiSegmentPlacer_Vertices->value() - 1;
//
//   msp->setVertex(
//      i,
//      app->MultiSegmentPlacer_X->value(),
//      app->MultiSegmentPlacer_Y->value(),
//      app->MultiSegmentPlacer_Z->value()
//   );
//
//   char buf[80];
//
//   sprintf(buf, "%g, %g, %g",
//            app->MultiSegmentPlacer_X->value(),
//            app->MultiSegmentPlacer_Y->value(),
//            app->MultiSegmentPlacer_Z->value()
//   );
//
//   app->MultiSegmentPlacer_Vertices->text(i+1, buf);
//}
//
//void psEditorGUI_MultiSegmentPlacer_Vertex_SetY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   MultiSegmentPlacer* msp =
//      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   int i = app->MultiSegmentPlacer_Vertices->value() - 1;
//
//   msp->setVertex(
//      i,
//      app->MultiSegmentPlacer_X->value(),
//      app->MultiSegmentPlacer_Y->value(),
//      app->MultiSegmentPlacer_Z->value()
//   );
//
//   char buf[80];
//
//   sprintf(buf, "%g, %g, %g",
//            app->MultiSegmentPlacer_X->value(),
//            app->MultiSegmentPlacer_Y->value(),
//            app->MultiSegmentPlacer_Z->value()
//   );
//
//   app->MultiSegmentPlacer_Vertices->text(i+1, buf);
//}
//
//void psEditorGUI_MultiSegmentPlacer_Vertex_SetZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   MultiSegmentPlacer* msp =
//      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();
//
//   int i = app->MultiSegmentPlacer_Vertices->value() - 1;
//
//   msp->setVertex(
//      i,
//      app->MultiSegmentPlacer_X->value(),
//      app->MultiSegmentPlacer_Y->value(),
//      app->MultiSegmentPlacer_Z->value()
//   );
//
//   char buf[80];
//
//   sprintf(buf, "%g, %g, %g",
//            app->MultiSegmentPlacer_X->value(),
//            app->MultiSegmentPlacer_Y->value(),
//            app->MultiSegmentPlacer_Z->value()
//   );
//
//   app->MultiSegmentPlacer_Vertices->text(i+1, buf);
//}
//
//void psEditorGUI_Placer_SetType(Fl_Choice* placerType, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   Placer* placer = layers[layer].mModularEmitter->getPlacer();
//
//   switch (placerType->value())
//   {
//   case 0:
//      if (!IS_A(placer, PointPlacer*))
//      {
//         layers[layer].mModularEmitter->setPlacer(
//            new PointPlacer()
//         );
//
//         updatePointPlacerParameters();
//      }
//      break;
//
//   case 1:
//      if (!IS_A(placer, SectorPlacer*))
//      {
//         layers[layer].mModularEmitter->setPlacer(
//            new SectorPlacer()
//         );
//
//         updateSectorPlacerParameters();
//      }
//      break;
//
//   case 2:
//      if (!IS_A(placer, SegmentPlacer*))
//      {
//         layers[layer].mModularEmitter->setPlacer(
//            new SegmentPlacer()
//         );
//
//         updateSegmentPlacerParameters();
//      }
//      break;
//
//   case 3:
//      if (!IS_A(placer, MultiSegmentPlacer*))
//      {
//         MultiSegmentPlacer* msp = new MultiSegmentPlacer();
//
//         msp->addVertex(-1, 0, 0);
//         msp->addVertex(1, 0, 0);
//
//         layers[layer].mModularEmitter->setPlacer(msp);
//
//         updateMultiSegmentPlacerParameters();
//      }
//      break;
//   }
//}
//
//void psEditorGUI_RadialShooter_SetMinTheta(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setThetaRange(
//      app->RadialShooter_MinTheta->value(),
//      app->RadialShooter_MaxTheta->value()
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMaxTheta(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setThetaRange(
//      app->RadialShooter_MinTheta->value(),
//      app->RadialShooter_MaxTheta->value()
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMinPhi(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setPhiRange(
//      app->RadialShooter_MinPhi->value(),
//      app->RadialShooter_MaxPhi->value()
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMaxPhi(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setPhiRange(
//      app->RadialShooter_MinPhi->value(),
//      app->RadialShooter_MaxPhi->value()
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMinInitialSpeed(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialSpeedRange(
//      app->RadialShooter_MinInitialSpeed->value(),
//      app->RadialShooter_MaxInitialSpeed->value()
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMaxInitialSpeed(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialSpeedRange(
//      app->RadialShooter_MinInitialSpeed->value(),
//      app->RadialShooter_MaxInitialSpeed->value()
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialRotationalSpeedRange(
//      osg::Vec3(
//         app->RadialShooter_MinInitialRotationalSpeedX->value(),
//         app->RadialShooter_MinInitialRotationalSpeedY->value(),
//         app->RadialShooter_MinInitialRotationalSpeedZ->value()
//      ),
//      osg::Vec3(
//         app->RadialShooter_MaxInitialRotationalSpeedX->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedY->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedZ->value()
//      )
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialRotationalSpeedRange(
//      osg::Vec3(
//         app->RadialShooter_MinInitialRotationalSpeedX->value(),
//         app->RadialShooter_MinInitialRotationalSpeedY->value(),
//         app->RadialShooter_MinInitialRotationalSpeedZ->value()
//      ),
//      osg::Vec3(
//         app->RadialShooter_MaxInitialRotationalSpeedX->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedY->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedZ->value()
//      )
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialRotationalSpeedRange(
//      osg::Vec3(
//         app->RadialShooter_MinInitialRotationalSpeedX->value(),
//         app->RadialShooter_MinInitialRotationalSpeedY->value(),
//         app->RadialShooter_MinInitialRotationalSpeedZ->value()
//      ),
//      osg::Vec3(
//         app->RadialShooter_MaxInitialRotationalSpeedX->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedY->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedZ->value()
//      )
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialRotationalSpeedRange(
//      osg::Vec3(
//         app->RadialShooter_MinInitialRotationalSpeedX->value(),
//         app->RadialShooter_MinInitialRotationalSpeedY->value(),
//         app->RadialShooter_MinInitialRotationalSpeedZ->value()
//      ),
//      osg::Vec3(
//         app->RadialShooter_MaxInitialRotationalSpeedX->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedY->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedZ->value()
//      )
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialRotationalSpeedRange(
//      osg::Vec3(
//         app->RadialShooter_MinInitialRotationalSpeedX->value(),
//         app->RadialShooter_MinInitialRotationalSpeedY->value(),
//         app->RadialShooter_MinInitialRotationalSpeedZ->value()
//      ),
//      osg::Vec3(
//         app->RadialShooter_MaxInitialRotationalSpeedX->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedY->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedZ->value()
//      )
//   );
//}
//
//void psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   RadialShooter* rs =
//      (RadialShooter*)layers[layer].mModularEmitter->getShooter();
//
//   rs->setInitialRotationalSpeedRange(
//      osg::Vec3(
//         app->RadialShooter_MinInitialRotationalSpeedX->value(),
//         app->RadialShooter_MinInitialRotationalSpeedY->value(),
//         app->RadialShooter_MinInitialRotationalSpeedZ->value()
//      ),
//      osg::Vec3(
//         app->RadialShooter_MaxInitialRotationalSpeedX->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedY->value(),
//         app->RadialShooter_MaxInitialRotationalSpeedZ->value()
//      )
//   );
//}
//
//void psEditorGUI_Shooter_SetType(Fl_Choice*, void*)
//{
//}
//
//void psEditor_Program_OperatorsChanged(Fl_Browser*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   if (i >= 0)
//   {
//      app->Program_DeleteOperator->activate();
//
//      Operator* op = layers[layer].mModularProgram->getOperator(i);
//
//      if (IS_A(op, FluidFrictionOperator*))
//      {
//         app->FluidFrictionParameters->show();
//         app->ForceParameters->hide();
//         app->AccelerationParameters->hide();
//
//         FluidFrictionOperator* ffo = (FluidFrictionOperator*)op;
//
//         app->FluidFriction_Density->value(
//            ffo->getFluidDensity()
//         );
//
//         app->FluidFriction_Viscosity->value(
//            ffo->getFluidViscosity()
//         );
//
//         app->FluidFriction_OverrideRadius->value(
//            ffo->getOverrideRadius()
//         );
//      }
//      else if (IS_A(op, ForceOperator*))
//      {
//         app->FluidFrictionParameters->hide();
//         app->ForceParameters->show();
//         app->AccelerationParameters->hide();
//
//         ForceOperator* fo = (ForceOperator*)op;
//
//         app->Force_X->value(
//            fo->getForce()[0]
//         );
//
//         app->Force_Y->value(
//            fo->getForce()[1]
//         );
//
//         app->Force_Z->value(
//            fo->getForce()[2]
//         );
//      }
//      else if (IS_A(op, AccelOperator*))
//      {
//         app->FluidFrictionParameters->hide();
//         app->ForceParameters->hide();
//         app->AccelerationParameters->show();
//
//         AccelOperator* ao = (AccelOperator*)op;
//
//         app->Acceleration_X->value(
//            ao->getAcceleration()[0]
//         );
//
//         app->Acceleration_Y->value(
//            ao->getAcceleration()[1]
//         );
//
//         app->Acceleration_Z->value(
//            ao->getAcceleration()[2]
//         );
//      }
//   }
//   else
//   {
//      app->FluidFrictionParameters->hide();
//      app->ForceParameters->hide();
//      app->AccelerationParameters->hide();
//      app->Program_DeleteOperator->deactivate();
//   }
//}
//
//void psEditorGUI_Program_NewForce(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   ForceOperator* fo = new ForceOperator();
//
//   layers[layer].mModularProgram->addOperator(fo);
//
//   app->Program_Operators->add("Force");
//
//   app->Program_Operators->value(app->Program_Operators->size());
//
//   app->Program_Operators->do_callback();
//}
//
//void psEditorGUI_Program_NewAcceleration(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   AccelOperator* ao = new AccelOperator();
//
//   layers[layer].mModularProgram->addOperator(ao);
//
//   app->Program_Operators->add("Acceleration");
//
//   app->Program_Operators->value(app->Program_Operators->size());
//
//   app->Program_Operators->do_callback();
//}
//
//void psEditorGUI_Program_NewFluidFriction(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   FluidFrictionOperator* ffo = new FluidFrictionOperator();
//
//   layers[layer].mModularProgram->addOperator(ffo);
//
//   app->Program_Operators->add("Fluid Friction");
//
//   app->Program_Operators->value(app->Program_Operators->size());
//
//   app->Program_Operators->do_callback();
//}
//
//void psEditorGUI_Program_DeleteOperator(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   layers[layer].mModularProgram->removeOperator(i);
//
//   app->Program_Operators->remove(i + 1);
//
//   if (i < app->Program_Operators->size())
//   {
//      app->Program_Operators->value(i+1);
//   }
//
//   app->Program_Operators->do_callback();
//}
//
//void psEditorGUI_FluidFriction_Air(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   FluidFrictionOperator* ffo =
//      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ffo->setFluidToAir();
//
//   app->FluidFriction_Density->value(
//      ffo->getFluidDensity()
//   );
//
//   app->FluidFriction_Viscosity->value(
//      ffo->getFluidViscosity()
//   );
//}
//
//void psEditorGUI_FluidFriction_Water(Fl_Button*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   FluidFrictionOperator* ffo =
//      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ffo->setFluidToWater();
//
//   app->FluidFriction_Density->value(
//      ffo->getFluidDensity()
//   );
//
//   app->FluidFriction_Viscosity->value(
//      ffo->getFluidViscosity()
//   );
//}
//
//void psEditorGUI_FluidFriction_SetDensity(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   FluidFrictionOperator* ffo =
//      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ffo->setFluidDensity(
//      app->FluidFriction_Density->value()
//   );
//}
//
//void psEditorGUI_FluidFriction_SetViscosity(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   FluidFrictionOperator* ffo =
//      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ffo->setFluidViscosity(
//      app->FluidFriction_Viscosity->value()
//   );
//}
//
//void psEditorGUI_FluidFriction_SetOverrideRadius(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   FluidFrictionOperator* ffo =
//      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ffo->setOverrideRadius(
//      app->FluidFriction_OverrideRadius->value()
//   );
//}
////
//void psEditorGUI_Force_SetX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   ForceOperator* fo =
//      (ForceOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   fo->setForce(
//      osg::Vec3(
//         app->Force_X->value(),
//         app->Force_Y->value(),
//         app->Force_Z->value()
//      )
//   );
//}
//
//void psEditorGUI_Force_SetY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   ForceOperator* fo =
//      (ForceOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   fo->setForce(
//      osg::Vec3(
//         app->Force_X->value(),
//         app->Force_Y->value(),
//         app->Force_Z->value()
//      )
//   );
//}
//
//void psEditorGUI_Force_SetZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   ForceOperator* fo =
//      (ForceOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   fo->setForce(
//      osg::Vec3(
//         app->Force_X->value(),
//         app->Force_Y->value(),
//         app->Force_Z->value()
//      )
//   );
//}
//
//void psEditorGUI_Acceleration_SetX(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   AccelOperator* ao =
//      (AccelOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ao->setAcceleration(
//      osg::Vec3(
//         app->Acceleration_X->value(),
//         app->Acceleration_Y->value(),
//         app->Acceleration_Z->value()
//      )
//   );
//}
//
//void psEditorGUI_Acceleration_SetY(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   AccelOperator* ao =
//      (AccelOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ao->setAcceleration(
//      osg::Vec3(
//         app->Acceleration_X->value(),
//         app->Acceleration_Y->value(),
//         app->Acceleration_Z->value()
//      )
//   );
//}
//
//void psEditorGUI_Acceleration_SetZ(Fl_Value_Input*, void*)
//{
//   int layer = app->Layers->value() - 1;
//
//   int i = app->Program_Operators->value() - 1;
//
//   AccelOperator* ao =
//      (AccelOperator*)layers[layer].mModularProgram->getOperator(i);
//
//   ao->setAcceleration(
//      osg::Vec3(
//         app->Acceleration_X->value(),
//         app->Acceleration_Y->value(),
//         app->Acceleration_Z->value()
//      )
//   );
//}
//
//
/////Called from the Open Previous menu items
//void OpenHistoryCB(Fl_Widget *, void *v)
//{
//   LoadFile((char*)v);
//}
//
/////Read the preference file and populate the particular widgets
//void ReadPrefs(void)
//{
//   for (int i = 0; i < 5; ++i)
//   {
//      appPrefs.get(Fl_Preferences::Name("file%d", i), absoluteHistory[i],
//                     "", sizeof(absoluteHistory[i]));
//
//      if (absoluteHistory[i][0])
//      {
//         fl_filename_relative(relativeHistory[i], sizeof(relativeHistory[i]), absoluteHistory[i]);
//         app->menu_MainMenu[i+4].flags = 0;
//      }
//      else
//      {
//         app->menu_MainMenu[i+4].flags = FL_MENU_INVISIBLE;
//      }
//   }
//
//   if (!absoluteHistory[0][0]) app->menu_MainMenu[3].flags |= FL_MENU_INACTIVE;
//
//   //populate the menu items with the file paths
//   for (int i = 0; i < 5; ++i)
//   {
//      app->menu_MainMenu[i+4].label(relativeHistory[i]);
//      app->menu_MainMenu[i+4].callback(OpenHistoryCB, absoluteHistory[i]);
//   }
//}

int main(int argc, char **argv)
{
   //Fl::visual(FL_DOUBLE | FL_RGB8);

   //app = new UserInterface();

   //app->Show();

   //sceneGroup = app->viewWidget->GetScene()->GetSceneNode();

   //app->viewWidget->Config();

   //makeGrids();

   //Compass* compass = new Compass(app->viewWidget->GetCamera());

   //app->viewWidget->GetScene()->AddDrawable(compass);

   //compassTransform = (osg::MatrixTransform*)compass->GetOSGNode();

   //::OrbitMotionModel obm(app->viewWidget->GetMouse(), app->viewWidget->GetCamera());

   //psEditorGUI_New(NULL, NULL);

   //ReadPrefs();

   ////load the first filename passed in on the command line
   //if (argc>1)
   //{
   //   std::string filename = argv[1];
   //   LoadFile(filename);
   //}

   //app->viewWidget->SetEvent(FL_PUSH);
   //app->viewWidget->SetEvent(FL_RELEASE);
   //app->viewWidget->SetEvent(FL_ENTER);
   //app->viewWidget->SetEvent(FL_LEAVE);
   //app->viewWidget->SetEvent(FL_DRAG);
   //app->viewWidget->SetEvent(FL_MOVE);
   //app->viewWidget->SetEvent(FL_MOUSEWHEEL);
   //app->viewWidget->SetEvent(FL_FOCUS);
   //app->viewWidget->SetEvent(FL_UNFOCUS);
   //app->viewWidget->SetEvent(FL_KEYDOWN);
   //app->viewWidget->SetEvent(FL_KEYUP);


   //Fl::run();

   //delete app;
   //return 0;
   QApplication qapp(argc, argv);  

   Delta3DThread *thread = new Delta3DThread(&qapp);

   QObject::connect(QApplication::instance(), SIGNAL(lastWindowClosed()), thread, SLOT(quit()));

   psEditor::MainWindow win;
   win.show();

   thread->SetMainWindow(&win);
   thread->run();

   //if (argc >= 2)
   //{
   //   win.LoadCharFile(QCoreApplication::arguments().at(1));
   //}

   qapp.exec();

   delete thread;

   return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
