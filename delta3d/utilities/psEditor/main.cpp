#include "dt.h"
#include "camera.h"
#include "globals.h"
#include "notify.h"
#include "system.h"
#include "mouse.h"

#include "osg/BlendFunc"
#include "osg/Geode"
#include "osg/Geometry"
#include "osg/PrimitiveSet"
#include "osg/StateAttribute"
#include "osg/Image"
#include "osg/Material"
#include "osg/MatrixTransform"
#include "osg/Texture2D"

#include "osgDB/ReadFile"
#include "osgDB/WriteFile"

#include "osgParticle/AccelOperator"
#include "osgParticle/FluidFrictionOperator"
#include "osgParticle/ForceOperator"
#include "osgParticle/ModularEmitter"
#include "osgParticle/ModularProgram"
#include "osgParticle/MultiSegmentPlacer"
#include "osgParticle/Particle"
#include "osgParticle/ParticleSystem"
#include "osgParticle/ParticleSystemUpdater"
#include "osgParticle/PointPlacer"
#include "osgParticle/RadialShooter"
#include "osgParticle/RandomRateCounter"
#include "osgParticle/SectorPlacer"
#include "osgParticle/SegmentPlacer"

#include "osgText/Text"

#include "FL/FL_Color_Chooser.h"
#include "FL/FL_File_Chooser.h"
#include "FL/fl_ask.h"

#include "psEditorGUI.h"

#include <string.h>

using namespace dtCore;
using namespace osgParticle;


/**
 * The editor window.
 */
static Fl_Double_Window* editorWindow;

/**
 * The group that holds the entire scene.
 */
static osg::Group* sceneGroup;

/**
 * The transform that controls the position of the compass.
 */
static osg::MatrixTransform* compassTransform;

/**
 * The transform that contains the XY grid.
 */
static osg::MatrixTransform* xyGridTransform;

/**
 * The transform that contains the YZ grid.
 */
static osg::MatrixTransform* yzGridTransform;

/**
 * The transform that contains the XZ grid.
 */
static osg::MatrixTransform* xzGridTransform;

/**
 * The group that holds the active particle system.
 */
static osg::Group* particleSystemGroup = NULL;

/**
 * The geode that holds the drawable particle system.
 */
static osg::Geode* geode;

/**
 * The active particle system.
 */
static osgParticle::ParticleSystem* particleSystem;

/**
 * The active particle template.
 */
static Particle* particle;

/**
 * The transform that controls the position of the emitter.
 */
static osg::MatrixTransform* emitterTransform;

/**
 * The active emitter.
 */
static ModularEmitter* modularEmitter;

/**
 * The active program.
 */
static ModularProgram* modularProgram;

/**
 * The particle system updater.
 */
static ParticleSystemUpdater* particleSystemUpdater;

/**
 * The filename of the active particle system.
 */
static std::string particleSystemFilename;

///The Editor preferences - gets saved automatically when app exits
Fl_Preferences appPrefs (Fl_Preferences::USER, "Delta3D", "ParticleEditor/preferences");

// Previous loaded file history
char absoluteHistory[5][1024];
char relativeHistory[5][1024];

/**
 * Activates and updates the random rate counter parameters.
 */
static void updateRandomRateCounterParameters()
{
   RandomRateCounterParameters->show();

   RandomRateCounter* rrc = 
      (RandomRateCounter*)modularEmitter->getCounter();

   RandomRateCounter_MinRate->value(rrc->getRateRange().minimum);
   RandomRateCounter_MaxRate->value(rrc->getRateRange().maximum);
}

/**
 * Activates and updates the point placer parameters.
 */
static void updatePointPlacerParameters()
{
   PointPlacerParameters->show();
   SectorPlacerParameters->hide();
   SegmentPlacerParameters->hide();
   MultiSegmentPlacerParameters->hide();

   PointPlacer* pp = 
      (PointPlacer*)modularEmitter->getPlacer();

   PointPlacer_X->value(pp->getCenter()[0]);
   PointPlacer_Y->value(pp->getCenter()[1]);
   PointPlacer_Z->value(pp->getCenter()[2]);
}

/**
 * Activates and updates the sector placer parameters.
 */
static void updateSectorPlacerParameters()
{
   PointPlacerParameters->hide();
   SectorPlacerParameters->show();
   SegmentPlacerParameters->hide();
   MultiSegmentPlacerParameters->hide();

   SectorPlacer* sp = 
      (SectorPlacer*)modularEmitter->getPlacer();

   SectorPlacer_X->value(sp->getCenter()[0]);
   SectorPlacer_Y->value(sp->getCenter()[1]);
   SectorPlacer_Z->value(sp->getCenter()[2]);

   SectorPlacer_MinRadius->value(sp->getRadiusRange().minimum);
   SectorPlacer_MaxRadius->value(sp->getRadiusRange().maximum);

   SectorPlacer_MinPhi->value(sp->getPhiRange().minimum);
   SectorPlacer_MaxPhi->value(sp->getPhiRange().maximum);
}

/**
 * Activates and updates the segment placer parameters.
 */
static void updateSegmentPlacerParameters()
{
   PointPlacerParameters->hide();
   SectorPlacerParameters->hide();
   SegmentPlacerParameters->show();
   MultiSegmentPlacerParameters->hide();

   SegmentPlacer* sp = 
      (SegmentPlacer*)modularEmitter->getPlacer();

   SegmentPlacer_A_X->value(sp->getVertexA()[0]);
   SegmentPlacer_A_Y->value(sp->getVertexA()[1]);
   SegmentPlacer_A_Z->value(sp->getVertexA()[2]);

   SegmentPlacer_B_X->value(sp->getVertexB()[0]);
   SegmentPlacer_B_Y->value(sp->getVertexB()[1]);
   SegmentPlacer_B_Z->value(sp->getVertexB()[2]);
}

/**
 * Activates and updates the multi-segment placer parameters.
 */
static void updateMultiSegmentPlacerParameters()
{
   PointPlacerParameters->hide();
   SectorPlacerParameters->hide();
   SegmentPlacerParameters->hide();
   MultiSegmentPlacerParameters->show();

   MultiSegmentPlacer* msp = 
      (MultiSegmentPlacer*)modularEmitter->getPlacer();

   MultiSegmentPlacer_Vertices->clear();

   char buf[80];

   for(int i=0;i<msp->numVertices();i++)
   {
      sprintf(buf, "%g, %g, %g", 
         msp->getVertex(i)[0], 
         msp->getVertex(i)[1], 
         msp->getVertex(i)[2]
      );

      MultiSegmentPlacer_Vertices->add(buf);
   }

   MultiSegmentPlacer_VertexParameters->hide();
   MultiSegmentPlacer_DeleteVertex->deactivate();
}

/**
 * Updates the state of all GUI elements in response to a new or
 * open request.
 */
static void updateGUIElements()
{
   Particles_Alignment->value(
      particleSystem->getParticleAlignment()
   );

   Particles_Shape->value(
      particle->getShape()
   );

   Particles_Lifetime->value(
      particle->getLifeTime()
   );

   Particles_Radius->value(
      particle->getRadius()
   );

   Particles_Mass->value(
      particle->getMass()
   );

   Particles_MinSize->value(
      particle->getSizeRange().minimum
   );

   Particles_MaxSize->value(
      particle->getSizeRange().maximum
   );

   Particles_MinAlpha->value(
      particle->getAlphaRange().minimum
   );

   Particles_MaxAlpha->value(
      particle->getAlphaRange().maximum
   );

   Particles_MinR->value(
      particle->getColorRange().minimum[0]
   );

   Particles_MinG->value(
      particle->getColorRange().minimum[1]
   );

   Particles_MinB->value(
      particle->getColorRange().minimum[2]
   );

   Particles_MinColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().minimum[0]*255),
         (uchar)(particle->getColorRange().minimum[1]*255),
         (uchar)(particle->getColorRange().minimum[2]*255)
      )
   );

   Particles_MinColor->redraw();

   Particles_MaxR->value(
      particle->getColorRange().maximum[0]
   );

   Particles_MaxG->value(
      particle->getColorRange().maximum[1]
   );

   Particles_MaxB->value(
      particle->getColorRange().maximum[2]
   );

   Particles_MaxColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().maximum[0]*255),
         (uchar)(particle->getColorRange().maximum[1]*255),
         (uchar)(particle->getColorRange().maximum[2]*255)
      )
   );

   Particles_MaxColor->redraw();

   Particles_Emitter_Endless->value(modularEmitter->isEndless());
   
   Particles_Emitter_Lifetime->value(modularEmitter->getLifeTime());
   
   if(modularEmitter->isEndless())
   {
      Particles_Emitter_Lifetime->deactivate();
   }
   else
   {
      Particles_Emitter_Lifetime->activate();
   }
   
   Particles_Emitter_StartTime->value(modularEmitter->getStartTime());
   
   Particles_Emitter_ResetTime->value(modularEmitter->getResetTime());
   
   std::string textureFile = "";

   osg::StateSet* ss =
      particleSystem->getStateSet();

   osg::StateAttribute* sa = 
      ss->getTextureAttribute(0, osg::StateAttribute::TEXTURE);

   if(IS_A(sa, osg::Texture2D*))
   {
      osg::Texture2D* t2d = (osg::Texture2D*)sa;

      osg::Image* image = t2d->getImage();

      if(image != NULL)
      {
         textureFile = image->getFileName();
      }
   }

   Particles_Texture->value(
      textureFile.c_str()
   );

   osg::BlendFunc* blend = 
      (osg::BlendFunc*)ss->getAttribute(osg::StateAttribute::BLENDFUNC);

   Particles_Emissive->value(
      blend->getDestination() == osg::BlendFunc::ONE
   );

   osg::Material* material = 
      (osg::Material*)ss->getAttribute(osg::StateAttribute::MATERIAL);

   Particles_Lighting->value(
      material->getColorMode() == osg::Material::AMBIENT_AND_DIFFUSE
   );


   Counter* counter = modularEmitter->getCounter();

   if(IS_A(counter, RandomRateCounter*))
   {
      updateRandomRateCounterParameters();
   }


   Placer* placer = modularEmitter->getPlacer();

   if(IS_A(placer, PointPlacer*))
   {
      updatePointPlacerParameters();
   }
   else if(IS_A(placer, SectorPlacer*))
   {
      updateSectorPlacerParameters();
   }
   else if(IS_A(placer, SegmentPlacer*))
   {
      updateSegmentPlacerParameters();
   }
   else if(IS_A(placer, MultiSegmentPlacer*))
   {
      updateMultiSegmentPlacerParameters();
   }


   Shooter* shooter = modularEmitter->getShooter();

   if(IS_A(shooter, RadialShooter*))
   {
      RadialShooterParameters->show();

      RadialShooter* rs = (RadialShooter*)shooter;

      RadialShooter_MinTheta->value(rs->getThetaRange().minimum);
      RadialShooter_MaxTheta->value(rs->getThetaRange().maximum);

      RadialShooter_MinPhi->value(rs->getPhiRange().minimum);
      RadialShooter_MaxPhi->value(rs->getPhiRange().maximum);

      RadialShooter_MinInitialSpeed->value(rs->getInitialSpeedRange().minimum);
      RadialShooter_MaxInitialSpeed->value(rs->getInitialSpeedRange().maximum);

      RadialShooter_MinInitialRotationalSpeedX->value(
         rs->getInitialRotationalSpeedRange().minimum[0]
      );

      RadialShooter_MinInitialRotationalSpeedY->value(
         rs->getInitialRotationalSpeedRange().minimum[1]
      );

      RadialShooter_MinInitialRotationalSpeedZ->value(
         rs->getInitialRotationalSpeedRange().minimum[2]
      );

      RadialShooter_MaxInitialRotationalSpeedX->value(
         rs->getInitialRotationalSpeedRange().maximum[0]
      );

      RadialShooter_MaxInitialRotationalSpeedY->value(
         rs->getInitialRotationalSpeedRange().maximum[1]
      );

      RadialShooter_MaxInitialRotationalSpeedZ->value(
         rs->getInitialRotationalSpeedRange().maximum[2]
      );
   }


   Program_Operators->clear();

   for(int i=0;i<modularProgram->numOperators();i++)
   {
      Operator* op = modularProgram->getOperator(i);

      if(IS_A(op, FluidFrictionOperator*))
      {
         Program_Operators->add("Fluid Friction");
      }
      else if(IS_A(op, ForceOperator*))
      {
         Program_Operators->add("Force");
      }
      else if(IS_A(op, AccelOperator*))
      {
         Program_Operators->add("Acceleration");
      }
   }
}

// Update file history from preferences...
void UpdateHistory(const std::string filename) 
{
   int	i;		// Looping var
   char	absolute[1024];

   fl_filename_absolute(absolute, sizeof(absolute), filename.c_str());

   for (i = 0; i < 5; i ++)
   {
      if (!strcmp(absolute, absoluteHistory[i])) break;
   }

   if (i == 0) return;

   if (i >= 5) i = 4;

   // Move the other filenames down in the list...
   memmove(absoluteHistory + 1, absoluteHistory, i * sizeof(absoluteHistory[0]));
   memmove(relativeHistory + 1, relativeHistory, i * sizeof(relativeHistory[0]));

   // Put the new file at the top...
   strncpy(absoluteHistory[0], absolute, sizeof(absoluteHistory[0]));

   fl_filename_relative(relativeHistory[0], sizeof(relativeHistory[0]),
      absoluteHistory[0]);

   // Update the menu items as needed...
   for (i = 0; i < 5; i ++) 
   {
      appPrefs.set( Fl_Preferences::Name("file%d", i), absoluteHistory[i]);
      if (absoluteHistory[i][0])
      {
         menu_MainMenu[i + 4].flags = 0;
         menu_MainMenu[i + 4].label( );
      }
      else  menu_MainMenu[i + 4].flags = FL_MENU_INVISIBLE;
   }

   menu_MainMenu[3].flags &= ~FL_MENU_INACTIVE;
}

static void setParticleSystemFilename(std::string newFilename)
{
   particleSystemFilename = newFilename;

   if(particleSystemFilename == "")
   {
      editorWindow->label("Particle System Editor");
   }
   else
   {
      char buf[256];

      sprintf(buf, "Particle System Editor (%s)", newFilename.c_str());

      editorWindow->label(buf);
      UpdateHistory(newFilename);
   }
}



void psEditorGUI_New(Fl_Menu_*, void*)
{
   if(particleSystemGroup != NULL)
   {
      particleSystemUpdater->removeParticleSystem(particleSystem);

      sceneGroup->removeChild(particleSystemGroup);
   }

   particleSystemGroup = new osg::Group();


   particleSystem = new osgParticle::ParticleSystem();

   particleSystem->setDefaultAttributes("", true, false);

   particle = new Particle();

   particleSystem->setDefaultParticleTemplate(*particle);


   geode = new osg::Geode();

   geode->addDrawable(particleSystem);

   particleSystemGroup->addChild(geode);


   emitterTransform = new osg::MatrixTransform();

   modularEmitter = new ModularEmitter();

   modularEmitter->setParticleSystem(particleSystem);

   RandomRateCounter* rrc = new RandomRateCounter();

   rrc->setRateRange(20, 30);

   modularEmitter->setCounter(
      rrc
   );

   modularEmitter->setPlacer(
      new PointPlacer()
   );

   modularEmitter->setShooter(
      new RadialShooter()
   );

   emitterTransform->addChild(modularEmitter);

   particleSystemGroup->addChild(emitterTransform);


   modularProgram = new ModularProgram();
 
   modularProgram->setParticleSystem(particleSystem);

   particleSystemGroup->addChild(modularProgram);

   
   particleSystemUpdater = new ParticleSystemUpdater();

   particleSystemUpdater->addParticleSystem(particleSystem);

   particleSystemGroup->addChild(particleSystemUpdater);


   sceneGroup->addChild(particleSystemGroup);


   setParticleSystemFilename("");
   

   updateGUIElements();
}

///Load the given filename
void LoadFile( std::string filename )
{
   if(!filename.empty())
   {
      osg::Node* node = osgDB::readNodeFile(filename.c_str());

      if(node == NULL || !IS_A(node, osg::Group*))
      {
         fl_alert("Invalid particle system: %s", filename.c_str());

         return;
      }

      osg::Group* newParticleSystemGroup = (osg::Group*)node;

      osg::Geode* newGeode = NULL;
      osgParticle::ParticleSystem* newParticleSystem = NULL;
      Particle* newParticle = NULL;
      osg::MatrixTransform* newEmitterTransform = NULL;
      ModularEmitter* newModularEmitter = NULL;
      ModularProgram* newModularProgram = NULL;
      ParticleSystemUpdater* newParticleSystemUpdater = NULL;

      for(unsigned int i=0;i<newParticleSystemGroup->getNumChildren();i++)
      {
         node = newParticleSystemGroup->getChild(i);

         if(IS_A(node, osg::Geode*))
         {
            newGeode = (osg::Geode*)node;

            for(unsigned int j=0;j<newGeode->getNumDrawables();j++)
            {
               osg::Drawable* drawable = newGeode->getDrawable(j);

               if(IS_A(drawable, osgParticle::ParticleSystem*))
               {
                  newParticleSystem = (osgParticle::ParticleSystem*)drawable;

                  newParticle = new Particle(
                     newParticleSystem->getDefaultParticleTemplate()
                  );
               }
            }
         }
         else if(IS_A(node, osg::MatrixTransform*))
         {
            newEmitterTransform = (osg::MatrixTransform*)node;

            for(unsigned int j=0;j<newEmitterTransform->getNumChildren();j++)
            {
               osg::Node* childNode = newEmitterTransform->getChild(j);

               if(IS_A(childNode, ModularEmitter*))
               {
                  newModularEmitter = (ModularEmitter*)childNode;
               }
            }
         }
         else if(IS_A(node, ModularProgram*))
         {
            newModularProgram = (ModularProgram*)node;
         }
         else if(IS_A(node, ParticleSystemUpdater*))
         {
            newParticleSystemUpdater = (ParticleSystemUpdater*)node;
         }
      }

      if(newGeode != NULL &&
         newParticleSystem != NULL &&
         newParticle != NULL &&
         newEmitterTransform != NULL &&
         newModularEmitter != NULL &&
         newModularProgram != NULL &&
         newParticleSystemUpdater != NULL)
      {
         sceneGroup->removeChild(particleSystemGroup);         

         particleSystemGroup = newParticleSystemGroup;
         geode = newGeode;
         particleSystem = newParticleSystem;
         particle = newParticle;
         emitterTransform = newEmitterTransform;
         modularEmitter = newModularEmitter;
         modularProgram = newModularProgram;
         particleSystemUpdater = newParticleSystemUpdater;

         sceneGroup->addChild(particleSystemGroup);

         updateGUIElements();

         setParticleSystemFilename(filename);
      }
      else
      {
         fl_alert("Invalid particle system: %s", filename.c_str());
      }
   }
}

void psEditorGUI_Open(Fl_Menu_*, void*)
{
   char* filename =
      fl_file_chooser(
      "Open",
      "Particle Systems (*.osg)",
      particleSystemFilename.c_str(),
      1
      );

   if (filename != NULL)
   {
      std::string name = filename;
      LoadFile(name);
   }
}

void psEditorGUI_Save(Fl_Menu_*, void*)
{
   if(particleSystemFilename == "")
   {
      psEditorGUI_SaveAs(NULL, NULL);
   }
   else
   {
      modularEmitter->setCurrentTime(0.0);
      
      osgDB::writeNodeFile(*particleSystemGroup, particleSystemFilename);
   }
}

void psEditorGUI_SaveAs(Fl_Menu_*, void*)
{
   char* filename = 
      fl_file_chooser(
         "Save As",
         "Particle Systems (*.osg)",
         particleSystemFilename.c_str(),
         1
      );

   if(filename != NULL)
   {
      setParticleSystemFilename(filename);

      psEditorGUI_Save(NULL, NULL);
   }
}

void psEditorGUI_Quit(Fl_Menu_*, void*)
{
   exit(0);
}

void psEditorGUI_Compass(Fl_Menu_*, void*)
{
   compassTransform->setNodeMask(
      compassTransform->getNodeMask() ?
         0x0 : 0xFFFFFFFF
   );
}

void psEditorGUI_XYGrid(Fl_Menu_*, void*)
{
   xyGridTransform->setNodeMask(
      xyGridTransform->getNodeMask() ?
         0x0 : 0xFFFFFFFF
   );
}

void psEditorGUI_YZGrid(Fl_Menu_*, void*)
{
   yzGridTransform->setNodeMask(
      yzGridTransform->getNodeMask() ?
         0x0 : 0xFFFFFFFF
   );
}

void psEditorGUI_XZGrid(Fl_Menu_*, void*)
{
   xzGridTransform->setNodeMask(
      xzGridTransform->getNodeMask() ?
         0x0 : 0xFFFFFFFF
   );
}

void psEditorGUI_Particles_SetAlignment(Fl_Choice*, void*)
{
   particleSystem->setParticleAlignment(
      (osgParticle::ParticleSystem::Alignment)Particles_Alignment->value()
   );
}

void psEditorGUI_Particles_SetShape(Fl_Choice*, void*)
{
   particle->setShape(
      (Particle::Shape)Particles_Shape->value()
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetLifetime(Fl_Value_Input*, void*)
{
   particle->setLifeTime(
      Particles_Lifetime->value()
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMinSize(Fl_Value_Input*, void*)
{
   particle->setSizeRange(
      rangef(
         Particles_MinSize->value(),
         Particles_MaxSize->value()
      )
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMaxSize(Fl_Value_Input*, void*)
{
   particle->setSizeRange(
      rangef(
         Particles_MinSize->value(),
         Particles_MaxSize->value()
      )
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMinAlpha(Fl_Value_Input*, void*)
{
   particle->setAlphaRange(
      rangef(
         Particles_MinAlpha->value(),
         Particles_MaxAlpha->value()
      )
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMaxAlpha(Fl_Value_Input*, void*)
{
   particle->setAlphaRange(
      rangef(
         Particles_MinAlpha->value(),
         Particles_MaxAlpha->value()
      )
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetRadius(Fl_Value_Input*, void*)
{
   particle->setRadius(
      Particles_Radius->value()
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMass(Fl_Value_Input*, void*)
{
   particle->setMass(
      Particles_Mass->value()
   );

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetTexture(Fl_Input*, void*)
{
   particleSystem->setDefaultAttributes(
      Particles_Texture->value(),
      Particles_Emissive->value(),
      Particles_Lighting->value()
   );
}

void psEditorGUI_Particles_ChooseTexture(Fl_Button*, void*)
{
   char* filename = 
      fl_file_chooser(
         "Choose Texture",
         "Image Files (*.{bmp,dds,gif,jpg,pic,png,rgb,tga,tiff})",
         Particles_Texture->value(),
         0
      );

   if(filename != NULL)
   {
      Particles_Texture->value(filename);

      particleSystem->setDefaultAttributes(
         Particles_Texture->value(),
         Particles_Emissive->value(),
         Particles_Lighting->value()
      );
   }
}

void psEditorGUI_Particles_SetEmissive(Fl_Check_Button*, void*)
{
   particleSystem->setDefaultAttributes(
      Particles_Texture->value(),
      Particles_Emissive->value(),
      Particles_Lighting->value()
   );
}

void psEditorGUI_Particles_SetLighting(Fl_Check_Button*, void*)
{
   particleSystem->setDefaultAttributes(
      Particles_Texture->value(),
      Particles_Emissive->value(),
      Particles_Lighting->value()
   );
}

void psEditorGUI_Particles_SetMinR(Fl_Value_Input*, void*)
{
   particle->setColorRange(
      rangev4(
         osg::Vec4(
            Particles_MinR->value(),
            Particles_MinG->value(),
            Particles_MinB->value(),
            1.0
         ),
         osg::Vec4(
            Particles_MaxR->value(),
            Particles_MaxG->value(),
            Particles_MaxB->value(),
            1.0
         )
      )
   );

   Particles_MinColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().minimum[0]*255),
         (uchar)(particle->getColorRange().minimum[1]*255),
         (uchar)(particle->getColorRange().minimum[2]*255)
      )
   );  

   Particles_MinColor->redraw();

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMinG(Fl_Value_Input*, void*)
{
   particle->setColorRange(
      rangev4(
         osg::Vec4(
            Particles_MinR->value(),
            Particles_MinG->value(),
            Particles_MinB->value(),
            1.0
         ),
         osg::Vec4(
            Particles_MaxR->value(),
            Particles_MaxG->value(),
            Particles_MaxB->value(),
            1.0
         )
      )
   );

   Particles_MinColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().minimum[0]*255),
         (uchar)(particle->getColorRange().minimum[1]*255),
         (uchar)(particle->getColorRange().minimum[2]*255)
      )
   );  
   
   Particles_MinColor->redraw();

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMinB(Fl_Value_Input*, void*)
{
   particle->setColorRange(
      rangev4(
         osg::Vec4(
            Particles_MinR->value(),
            Particles_MinG->value(),
            Particles_MinB->value(),
            1.0
         ),
         osg::Vec4(
            Particles_MaxR->value(),
            Particles_MaxG->value(),
            Particles_MaxB->value(),
            1.0
         )
      )
   );

   Particles_MinColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().minimum[0]*255),
         (uchar)(particle->getColorRange().minimum[1]*255),
         (uchar)(particle->getColorRange().minimum[2]*255)
      )
   );  

   Particles_MinColor->redraw();

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_ChooseMinColor(Fl_Button*, void*)
{
   double r = Particles_MinR->value(),
          g = Particles_MinG->value(),
          b = Particles_MinB->value();

   if(fl_color_chooser("Choose Min. Color", r, g, b))
   {
      Particles_MinR->value(r);
      Particles_MinG->value(g);
      Particles_MinB->value(b);

      particle->setColorRange(
         rangev4(
            osg::Vec4(
               Particles_MinR->value(),
               Particles_MinG->value(),
               Particles_MinB->value(),
               1.0
            ),
            osg::Vec4(
               Particles_MaxR->value(),
               Particles_MaxG->value(),
               Particles_MaxB->value(),
               1.0
            )
         )
      );

      Particles_MinColor->color(
         fl_rgb_color(
            (uchar)(particle->getColorRange().minimum[0]*255),
            (uchar)(particle->getColorRange().minimum[1]*255),
            (uchar)(particle->getColorRange().minimum[2]*255)
         )
      );

      Particles_MinColor->redraw();

      particleSystem->setDefaultParticleTemplate(*particle);
   }
}

void psEditorGUI_Particles_SetMaxR(Fl_Value_Input*, void*)
{
   particle->setColorRange(
      rangev4(
         osg::Vec4(
            Particles_MinR->value(),
            Particles_MinG->value(),
            Particles_MinB->value(),
            1.0
         ),
         osg::Vec4(
            Particles_MaxR->value(),
            Particles_MaxG->value(),
            Particles_MaxB->value(),
            1.0
         )
      )
   );

   Particles_MaxColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().maximum[0]*255),
         (uchar)(particle->getColorRange().maximum[1]*255),
         (uchar)(particle->getColorRange().maximum[2]*255)
      )
   );   

   Particles_MaxColor->redraw();

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMaxG(Fl_Value_Input*, void*)
{
   particle->setColorRange(
      rangev4(
         osg::Vec4(
            Particles_MinR->value(),
            Particles_MinG->value(),
            Particles_MinB->value(),
            1.0
         ),
         osg::Vec4(
            Particles_MaxR->value(),
            Particles_MaxG->value(),
            Particles_MaxB->value(),
            1.0
         )
      )
   );

   Particles_MaxColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().maximum[0]*255),
         (uchar)(particle->getColorRange().maximum[1]*255),
         (uchar)(particle->getColorRange().maximum[2]*255)
      )
   );   

   Particles_MaxColor->redraw();

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_SetMaxB(Fl_Value_Input*, void*)
{
   particle->setColorRange(
      rangev4(
         osg::Vec4(
            Particles_MinR->value(),
            Particles_MinG->value(),
            Particles_MinB->value(),
            1.0
         ),
         osg::Vec4(
            Particles_MaxR->value(),
            Particles_MaxG->value(),
            Particles_MaxB->value(),
            1.0
         )
      )
   );

   Particles_MaxColor->color(
      fl_rgb_color(
         (uchar)(particle->getColorRange().maximum[0]*255),
         (uchar)(particle->getColorRange().maximum[1]*255),
         (uchar)(particle->getColorRange().maximum[2]*255)
      )
   );   

   Particles_MaxColor->redraw();

   particleSystem->setDefaultParticleTemplate(*particle);
}

void psEditorGUI_Particles_ChooseMaxColor(Fl_Button*, void*)
{
   double r = Particles_MaxR->value(),
          g = Particles_MaxG->value(),
          b = Particles_MaxB->value();

   if(fl_color_chooser("Choose Max. Color", r, g, b))
   {
      Particles_MaxR->value(r);
      Particles_MaxG->value(g);
      Particles_MaxB->value(b);

      particle->setColorRange(
         rangev4(
            osg::Vec4(
               Particles_MinR->value(),
               Particles_MinG->value(),
               Particles_MinB->value(),
               1.0
            ),
            osg::Vec4(
               Particles_MaxR->value(),
               Particles_MaxG->value(),
               Particles_MaxB->value(),
               1.0
            )
         )
      );

      Particles_MaxColor->color(
         fl_rgb_color(
            (uchar)(particle->getColorRange().maximum[0]*255),
            (uchar)(particle->getColorRange().maximum[1]*255),
            (uchar)(particle->getColorRange().maximum[2]*255)
         )
      );

      Particles_MaxColor->redraw();

      particleSystem->setDefaultParticleTemplate(*particle);
   }
}

void psEditorGUI_Particles_SetEmitterEndless(Fl_Check_Button*, void*)
{
   modularEmitter->setEndless(
      Particles_Emitter_Endless->value()
   );
   
   if(modularEmitter->isEndless())
   {
      Particles_Emitter_Lifetime->deactivate();
   }
   else
   {
      Particles_Emitter_Lifetime->activate();
   }
}

void psEditorGUI_Particles_SetEmitterLifetime(Fl_Value_Input*, void*)
{
   modularEmitter->setLifeTime(
      Particles_Emitter_Lifetime->value()
   );
}

void psEditorGUI_Particles_SetEmitterStartTime(Fl_Value_Input*, void*)
{
   modularEmitter->setStartTime(
      Particles_Emitter_StartTime->value()
   );
}

void psEditorGUI_Particles_SetEmitterResetTime(Fl_Value_Input*, void*)
{
   modularEmitter->setResetTime(
      Particles_Emitter_ResetTime->value()
   );
}

void psEditorGUI_RandomRateCounter_SetMinRate(Fl_Value_Input*, void*)
{
   RandomRateCounter* rrc = 
      (RandomRateCounter*)modularEmitter->getCounter();

   rrc->setRateRange(
      rangef(
         RandomRateCounter_MinRate->value(),
         RandomRateCounter_MaxRate->value()
      )
   );
}

void psEditorGUI_RandomRateCounter_SetMaxRate(Fl_Value_Input*, void*)
{
   RandomRateCounter* rrc = 
      (RandomRateCounter*)modularEmitter->getCounter();

   rrc->setRateRange(
      rangef(
         RandomRateCounter_MinRate->value(),
         RandomRateCounter_MaxRate->value()
      )
   );
}

void psEditorGUI_Counter_SetType(Fl_Choice* counterType, void*)
{
   Counter* counter = modularEmitter->getCounter();

   switch(counterType->value())
   {
      case 0:
         if(!IS_A(counter, RandomRateCounter*))
         {
            modularEmitter->setCounter(
               new RandomRateCounter()
            );

            updateRandomRateCounterParameters();
         }
         break;
   }
}

void psEditor_SegmentPlacer_VertexA_SetX(Fl_Value_Input*, void*)
{
   SegmentPlacer* sp = 
      (SegmentPlacer*)modularEmitter->getPlacer();

   sp->setVertexA(
      SegmentPlacer_A_X->value(),
      SegmentPlacer_A_Y->value(),
      SegmentPlacer_A_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexA_SetY(Fl_Value_Input*, void*)
{
   SegmentPlacer* sp = 
      (SegmentPlacer*)modularEmitter->getPlacer();

   sp->setVertexA(
      SegmentPlacer_A_X->value(),
      SegmentPlacer_A_Y->value(),
      SegmentPlacer_A_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexA_SetZ(Fl_Value_Input*, void*)
{
   SegmentPlacer* sp = 
      (SegmentPlacer*)modularEmitter->getPlacer();

   sp->setVertexA(
      SegmentPlacer_A_X->value(),
      SegmentPlacer_A_Y->value(),
      SegmentPlacer_A_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexB_SetX(Fl_Value_Input*, void*)
{
   SegmentPlacer* sp = 
      (SegmentPlacer*)modularEmitter->getPlacer();

   sp->setVertexB(
      SegmentPlacer_B_X->value(),
      SegmentPlacer_B_Y->value(),
      SegmentPlacer_B_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexB_SetY(Fl_Value_Input*, void*)
{
   SegmentPlacer* sp = 
      (SegmentPlacer*)modularEmitter->getPlacer();

   sp->setVertexB(
      SegmentPlacer_B_X->value(),
      SegmentPlacer_B_Y->value(),
      SegmentPlacer_B_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexB_SetZ(Fl_Value_Input*, void*)
{
   SegmentPlacer* sp = 
      (SegmentPlacer*)modularEmitter->getPlacer();

   sp->setVertexB(
      SegmentPlacer_B_X->value(),
      SegmentPlacer_B_Y->value(),
      SegmentPlacer_B_Z->value()
   );
}

void psEditorGUI_SectorPlacer_Center_SetX(Fl_Value_Input*, void*)
{
   SectorPlacer* sp =
      (SectorPlacer*)modularEmitter->getPlacer();

   sp->setCenter(
      SectorPlacer_X->value(),
      SectorPlacer_Y->value(),
      SectorPlacer_Z->value()
   );
}

void psEditorGUI_SectorPlacer_Center_SetY(Fl_Value_Input*, void*)
{
   SectorPlacer* sp =
      (SectorPlacer*)modularEmitter->getPlacer();

   sp->setCenter(
      SectorPlacer_X->value(),
      SectorPlacer_Y->value(),
      SectorPlacer_Z->value()
   );
}

void psEditorGUI_SectorPlacer_Center_SetZ(Fl_Value_Input*, void*)
{
   SectorPlacer* sp =
      (SectorPlacer*)modularEmitter->getPlacer();

   sp->setCenter(
      SectorPlacer_X->value(),
      SectorPlacer_Y->value(),
      SectorPlacer_Z->value()
   );
}

void psEditorGUI_SectorPlacer_SetMinRadius(Fl_Value_Input*, void*)
{
   SectorPlacer* sp =
      (SectorPlacer*)modularEmitter->getPlacer();

   sp->setRadiusRange(
      SectorPlacer_MinRadius->value(),
      SectorPlacer_MaxRadius->value()
   );
}

void psEditorGUI_SectorPlacer_SetMaxRadius(Fl_Value_Input*, void*)
{
   SectorPlacer* sp =
      (SectorPlacer*)modularEmitter->getPlacer();

   sp->setRadiusRange(
      SectorPlacer_MinRadius->value(),
      SectorPlacer_MaxRadius->value()
   );
}

void psEditorGUI_SectorPlacer_SetMinPhi(Fl_Value_Input*, void*)
{
   SectorPlacer* sp =
      (SectorPlacer*)modularEmitter->getPlacer();

   sp->setPhiRange(
      SectorPlacer_MinPhi->value(),
      SectorPlacer_MaxPhi->value()
   );
}

void psEditorGUI_SectorPlacer_SetMaxPhi(Fl_Value_Input*, void*)
{
   SectorPlacer* sp =
      (SectorPlacer*)modularEmitter->getPlacer();

   sp->setPhiRange(
      SectorPlacer_MinPhi->value(),
      SectorPlacer_MaxPhi->value()
   );
}

void psEditorGUI_PointPlacer_SetX(Fl_Value_Input*, void*)
{
   PointPlacer* pp =
      (PointPlacer*)modularEmitter->getPlacer();

   pp->setCenter(
      PointPlacer_X->value(),
      PointPlacer_Y->value(),
      PointPlacer_Z->value()
   );
}

void psEditorGUI_PointPlacer_SetY(Fl_Value_Input*, void*)
{
   PointPlacer* pp =
      (PointPlacer*)modularEmitter->getPlacer();

   pp->setCenter(
      PointPlacer_X->value(),
      PointPlacer_Y->value(),
      PointPlacer_Z->value()
   );
}

void psEditorGUI_PointPlacer_SetZ(Fl_Value_Input*, void*)
{
   PointPlacer* pp =
      (PointPlacer*)modularEmitter->getPlacer();

   pp->setCenter(
      PointPlacer_X->value(),
      PointPlacer_Y->value(),
      PointPlacer_Z->value()
   );
}

void psEditorGUI_MultiSegmentPlacer_VerticesChanged(Fl_Browser*, void*)
{
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)modularEmitter->getPlacer();

   int i = MultiSegmentPlacer_Vertices->value() - 1;

   if(i >= 0)
   {
      MultiSegmentPlacer_VertexParameters->show();
      MultiSegmentPlacer_DeleteVertex->activate();

      MultiSegmentPlacer_X->value(msp->getVertex(i)[0]);
      MultiSegmentPlacer_Y->value(msp->getVertex(i)[1]);
      MultiSegmentPlacer_Z->value(msp->getVertex(i)[2]);
   }
   else
   {
      MultiSegmentPlacer_VertexParameters->hide();
      MultiSegmentPlacer_DeleteVertex->deactivate();
   }
}

void psEditorGUI_MultiSegmentPlacer_AddVertex(Fl_Button*, void*)
{
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)modularEmitter->getPlacer();

   MultiSegmentPlacer_Vertices->add("0, 0, 0");

   msp->addVertex(0, 0, 0);

   MultiSegmentPlacer_Vertices->value(msp->numVertices());

   MultiSegmentPlacer_Vertices->do_callback();
}

void psEditorGUI_MultiSegmentPlacer_DeleteVertex(Fl_Button*, void*)
{
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)modularEmitter->getPlacer();

   int i = MultiSegmentPlacer_Vertices->value() - 1;

   msp->removeVertex(i);

   MultiSegmentPlacer_Vertices->remove(i+1);

   if(i < MultiSegmentPlacer_Vertices->size())
   {
      MultiSegmentPlacer_Vertices->value(i+1);
   }

   MultiSegmentPlacer_Vertices->do_callback();
}

void psEditorGUI_MultiSegmentPlacer_Vertex_SetX(Fl_Value_Input*, void*)
{
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)modularEmitter->getPlacer();

   int i = MultiSegmentPlacer_Vertices->value() - 1;

   msp->setVertex(
      i,
      MultiSegmentPlacer_X->value(),
      MultiSegmentPlacer_Y->value(),
      MultiSegmentPlacer_Z->value()
   );

   char buf[80];

   sprintf(buf, "%g, %g, %g",
            MultiSegmentPlacer_X->value(),
            MultiSegmentPlacer_Y->value(),
            MultiSegmentPlacer_Z->value()
   );

   MultiSegmentPlacer_Vertices->text(i+1, buf);
}

void psEditorGUI_MultiSegmentPlacer_Vertex_SetY(Fl_Value_Input*, void*)
{
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)modularEmitter->getPlacer();

   int i = MultiSegmentPlacer_Vertices->value() - 1;

   msp->setVertex(
      i,
      MultiSegmentPlacer_X->value(),
      MultiSegmentPlacer_Y->value(),
      MultiSegmentPlacer_Z->value()
   );

   char buf[80];

   sprintf(buf, "%g, %g, %g",
            MultiSegmentPlacer_X->value(),
            MultiSegmentPlacer_Y->value(),
            MultiSegmentPlacer_Z->value()
   );

   MultiSegmentPlacer_Vertices->text(i+1, buf);
}

void psEditorGUI_MultiSegmentPlacer_Vertex_SetZ(Fl_Value_Input*, void*)
{
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)modularEmitter->getPlacer();

   int i = MultiSegmentPlacer_Vertices->value() - 1;

   msp->setVertex(
      i,
      MultiSegmentPlacer_X->value(),
      MultiSegmentPlacer_Y->value(),
      MultiSegmentPlacer_Z->value()
   );

   char buf[80];

   sprintf(buf, "%g, %g, %g",
            MultiSegmentPlacer_X->value(),
            MultiSegmentPlacer_Y->value(),
            MultiSegmentPlacer_Z->value()
   );

   MultiSegmentPlacer_Vertices->text(i+1, buf);
}

void psEditorGUI_Placer_SetType(Fl_Choice* placerType, void*)
{
   Placer* placer = modularEmitter->getPlacer();

   switch(placerType->value())
   {
      case 0:
         if(!IS_A(placer, PointPlacer*))
         {
            modularEmitter->setPlacer(
               new PointPlacer()
            );

            updatePointPlacerParameters();
         }
         break;

      case 1:
         if(!IS_A(placer, SectorPlacer*))
         {
            modularEmitter->setPlacer(
               new SectorPlacer()
            );

            updateSectorPlacerParameters();
         }
         break;

      case 2:
         if(!IS_A(placer, SegmentPlacer*))
         {
            modularEmitter->setPlacer(
               new SegmentPlacer()
            );

            updateSegmentPlacerParameters();
         }
         break;

      case 3:
         if(!IS_A(placer, MultiSegmentPlacer*))
         {
            MultiSegmentPlacer* msp = new MultiSegmentPlacer();

            msp->addVertex(-1, 0, 0);
            msp->addVertex(1, 0, 0);

            modularEmitter->setPlacer(msp);

            updateMultiSegmentPlacerParameters();
         }
         break;
   }
}

void psEditorGUI_RadialShooter_SetMinTheta(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setThetaRange(
      RadialShooter_MinTheta->value(),
      RadialShooter_MaxTheta->value()
   );
}

void psEditorGUI_RadialShooter_SetMaxTheta(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setThetaRange(
      RadialShooter_MinTheta->value(),
      RadialShooter_MaxTheta->value()
   );
}

void psEditorGUI_RadialShooter_SetMinPhi(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setPhiRange(
      RadialShooter_MinPhi->value(),
      RadialShooter_MaxPhi->value()
   );
}

void psEditorGUI_RadialShooter_SetMaxPhi(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setPhiRange(
      RadialShooter_MinPhi->value(),
      RadialShooter_MaxPhi->value()
   );
}

void psEditorGUI_RadialShooter_SetMinInitialSpeed(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialSpeedRange(
      RadialShooter_MinInitialSpeed->value(),
      RadialShooter_MaxInitialSpeed->value()
   );
}

void psEditorGUI_RadialShooter_SetMaxInitialSpeed(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialSpeedRange(
      RadialShooter_MinInitialSpeed->value(),
      RadialShooter_MaxInitialSpeed->value()
   );
}

void psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedX(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialRotationalSpeedRange(
      osg::Vec3(
         RadialShooter_MinInitialRotationalSpeedX->value(),
         RadialShooter_MinInitialRotationalSpeedY->value(),
         RadialShooter_MinInitialRotationalSpeedZ->value()
      ),
      osg::Vec3(
         RadialShooter_MaxInitialRotationalSpeedX->value(),
         RadialShooter_MaxInitialRotationalSpeedY->value(),
         RadialShooter_MaxInitialRotationalSpeedZ->value()
      )
   );
}

void psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedY(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialRotationalSpeedRange(
      osg::Vec3(
         RadialShooter_MinInitialRotationalSpeedX->value(),
         RadialShooter_MinInitialRotationalSpeedY->value(),
         RadialShooter_MinInitialRotationalSpeedZ->value()
      ),
      osg::Vec3(
         RadialShooter_MaxInitialRotationalSpeedX->value(),
         RadialShooter_MaxInitialRotationalSpeedY->value(),
         RadialShooter_MaxInitialRotationalSpeedZ->value()
      )
   );
}

void psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedZ(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialRotationalSpeedRange(
      osg::Vec3(
         RadialShooter_MinInitialRotationalSpeedX->value(),
         RadialShooter_MinInitialRotationalSpeedY->value(),
         RadialShooter_MinInitialRotationalSpeedZ->value()
      ),
      osg::Vec3(
         RadialShooter_MaxInitialRotationalSpeedX->value(),
         RadialShooter_MaxInitialRotationalSpeedY->value(),
         RadialShooter_MaxInitialRotationalSpeedZ->value()
      )
   );
}

void psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedX(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialRotationalSpeedRange(
      osg::Vec3(
         RadialShooter_MinInitialRotationalSpeedX->value(),
         RadialShooter_MinInitialRotationalSpeedY->value(),
         RadialShooter_MinInitialRotationalSpeedZ->value()
      ),
      osg::Vec3(
         RadialShooter_MaxInitialRotationalSpeedX->value(),
         RadialShooter_MaxInitialRotationalSpeedY->value(),
         RadialShooter_MaxInitialRotationalSpeedZ->value()
      )
   );
}

void psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedY(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialRotationalSpeedRange(
      osg::Vec3(
         RadialShooter_MinInitialRotationalSpeedX->value(),
         RadialShooter_MinInitialRotationalSpeedY->value(),
         RadialShooter_MinInitialRotationalSpeedZ->value()
      ),
      osg::Vec3(
         RadialShooter_MaxInitialRotationalSpeedX->value(),
         RadialShooter_MaxInitialRotationalSpeedY->value(),
         RadialShooter_MaxInitialRotationalSpeedZ->value()
      )
   );
}

void psEditorGUI_RadialShooter_SetMaxInitialRotationalSpeedZ(Fl_Value_Input*, void*)
{
   RadialShooter* rs = 
      (RadialShooter*)modularEmitter->getShooter();

   rs->setInitialRotationalSpeedRange(
      osg::Vec3(
         RadialShooter_MinInitialRotationalSpeedX->value(),
         RadialShooter_MinInitialRotationalSpeedY->value(),
         RadialShooter_MinInitialRotationalSpeedZ->value()
      ),
      osg::Vec3(
         RadialShooter_MaxInitialRotationalSpeedX->value(),
         RadialShooter_MaxInitialRotationalSpeedY->value(),
         RadialShooter_MaxInitialRotationalSpeedZ->value()
      )
   );
}

void psEditorGUI_Shooter_SetType(Fl_Choice*, void*)
{
}

void psEditor_Program_OperatorsChanged(Fl_Browser*, void*)
{
   int i = Program_Operators->value() - 1;

   if(i >= 0)
   {
      Program_DeleteOperator->activate();

      Operator* op = modularProgram->getOperator(i);

      if(IS_A(op, FluidFrictionOperator*))
      {
         FluidFrictionParameters->show();
         ForceParameters->hide();
         AccelerationParameters->hide();

         FluidFrictionOperator* ffo = (FluidFrictionOperator*)op;
         
         FluidFriction_Density->value(
            ffo->getFluidDensity()
         );

         FluidFriction_Viscosity->value(
            ffo->getFluidViscosity()
         );

         FluidFriction_OverrideRadius->value(
            ffo->getOverrideRadius()
         );
      }
      else if(IS_A(op, ForceOperator*))
      {
         FluidFrictionParameters->hide();
         ForceParameters->show();
         AccelerationParameters->hide();

         ForceOperator* fo = (ForceOperator*)op;

         Force_X->value(
            fo->getForce()[0]
         );

         Force_Y->value(
            fo->getForce()[1]
         );

         Force_Z->value(
            fo->getForce()[2]
         );
      }
      else if(IS_A(op, AccelOperator*))
      {
         FluidFrictionParameters->hide();
         ForceParameters->hide();
         AccelerationParameters->show();

         AccelOperator* ao = (AccelOperator*)op;

         Acceleration_X->value(
            ao->getAcceleration()[0]
         );

         Acceleration_Y->value(
            ao->getAcceleration()[1]
         );

         Acceleration_Z->value(
            ao->getAcceleration()[2]
         );
      }
   }
   else
   {
      FluidFrictionParameters->hide();
      ForceParameters->hide();
      AccelerationParameters->hide();
      Program_DeleteOperator->deactivate();
   }
}

void psEditorGUI_Program_NewForce(Fl_Button*, void*)
{
   ForceOperator* fo = new ForceOperator();

   modularProgram->addOperator(fo);

   Program_Operators->add("Force");

   Program_Operators->value(Program_Operators->size());

   Program_Operators->do_callback();
}

void psEditorGUI_Program_NewAcceleration(Fl_Button*, void*)
{
   AccelOperator* ao = new AccelOperator();

   modularProgram->addOperator(ao);

   Program_Operators->add("Acceleration");

   Program_Operators->value(Program_Operators->size());

   Program_Operators->do_callback();
}

void psEditorGUI_Program_NewFluidFriction(Fl_Button*, void*)
{
   FluidFrictionOperator* ffo = new FluidFrictionOperator();

   modularProgram->addOperator(ffo);

   Program_Operators->add("Fluid Friction");

   Program_Operators->value(Program_Operators->size());

   Program_Operators->do_callback();
}

void psEditorGUI_Program_DeleteOperator(Fl_Button*, void*)
{
   int i = Program_Operators->value() - 1;

   modularProgram->removeOperator(i);

   Program_Operators->remove(i + 1);

   if(i < Program_Operators->size())
   {
      Program_Operators->value(i+1);
   }

   Program_Operators->do_callback();
}

void psEditorGUI_FluidFriction_Air(Fl_Button*, void*)
{
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)modularProgram->getOperator(i);

   ffo->setFluidToAir();

   FluidFriction_Density->value(
      ffo->getFluidDensity()
   );

   FluidFriction_Viscosity->value(
      ffo->getFluidViscosity()
   );
}

void psEditorGUI_FluidFriction_Water(Fl_Button*, void*)
{
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)modularProgram->getOperator(i);

   ffo->setFluidToWater();

   FluidFriction_Density->value(
      ffo->getFluidDensity()
   );

   FluidFriction_Viscosity->value(
      ffo->getFluidViscosity()
   );
}

void psEditorGUI_FluidFriction_SetDensity(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)modularProgram->getOperator(i);

   ffo->setFluidDensity(
      FluidFriction_Density->value()
   );
}

void psEditorGUI_FluidFriction_SetViscosity(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)modularProgram->getOperator(i);

   ffo->setFluidViscosity(
      FluidFriction_Viscosity->value()
   );
}

void psEditorGUI_FluidFriction_SetOverrideRadius(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)modularProgram->getOperator(i);

   ffo->setOverrideRadius(
      FluidFriction_OverrideRadius->value()
   );
}

void psEditorGUI_Force_SetX(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   ForceOperator* fo =
      (ForceOperator*)modularProgram->getOperator(i);

   fo->setForce(
      osg::Vec3(
         Force_X->value(),
         Force_Y->value(),
         Force_Z->value()
      )
   );
}

void psEditorGUI_Force_SetY(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   ForceOperator* fo =
      (ForceOperator*)modularProgram->getOperator(i);

   fo->setForce(
      osg::Vec3(
         Force_X->value(),
         Force_Y->value(),
         Force_Z->value()
      )
   );
}

void psEditorGUI_Force_SetZ(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   ForceOperator* fo =
      (ForceOperator*)modularProgram->getOperator(i);

   fo->setForce(
      osg::Vec3(
         Force_X->value(),
         Force_Y->value(),
         Force_Z->value()
      )
   );
}

void psEditorGUI_Acceleration_SetX(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   AccelOperator* ao =
      (AccelOperator*)modularProgram->getOperator(i);

   ao->setAcceleration(
      osg::Vec3(
         Acceleration_X->value(),
         Acceleration_Y->value(),
         Acceleration_Z->value()
      )
   );
}

void psEditorGUI_Acceleration_SetY(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   AccelOperator* ao =
      (AccelOperator*)modularProgram->getOperator(i);

   ao->setAcceleration(
      osg::Vec3(
         Acceleration_X->value(),
         Acceleration_Y->value(),
         Acceleration_Z->value()
      )
   );
}

void psEditorGUI_Acceleration_SetZ(Fl_Value_Input*, void*)
{
   int i = Program_Operators->value() - 1;

   AccelOperator* ao =
      (AccelOperator*)modularProgram->getOperator(i);

   ao->setAcceleration(
      osg::Vec3(
         Acceleration_X->value(),
         Acceleration_Y->value(),
         Acceleration_Z->value()
      )
   );
}

class OrbitMotionModel : public MouseListener
{
   public:

      OrbitMotionModel(Mouse* mouse, Transformable* target)
      {
         mMouse = mouse;
         mTarget = target;

         mMouse->AddMouseListener(this);

         mAzimuth = 0.0f;
         mElevation = 22.5f;
         mDistance = 50.0f;

         sgSetVec3(mCenter, 0, 0, 0);
         
         UpdateTargetTransform();
      }

      virtual void ButtonPressed(Mouse* mouse, MouseButton button)
      {
         mouse->GetPosition(&mLastX, &mLastY);
      }

      virtual void MouseDragged(Mouse* mouse, float x, float y)
      {
         float dX = x - mLastX, dY = y - mLastY;

         if(mouse->GetButtonState(LeftButton))
         {
            mAzimuth -= dX*90;
            mElevation -= dY*90;

            if(mElevation < -90.0f) mElevation = -90.0f;
            else if(mElevation > 90.0f) mElevation = 90.0f;
         }
         
         if(mouse->GetButtonState(MiddleButton))
         {
            mDistance -= dY*20.0f;
            
            if(mDistance < 1.0f) mDistance = 1.0f;
         }
         
         if(mouse->GetButtonState(RightButton))
         {
            Transform transform;
            
            mTarget->GetTransform(&transform);
            
            sgMat4 mat;
            
            transform.Get(mat);
            
            sgVec3 offset = {-dX*mDistance*0.25f, 0, -dY*mDistance*0.25f};
            
            sgXformVec3(offset, mat);
            
            sgAddVec3(mCenter, offset);
         }
         
         UpdateTargetTransform();
         
         mLastX = x;
         mLastY = y;
      }

      virtual void MouseScrolled(Mouse* mouse, int delta)
      {
         mDistance -= delta*5;

         if(mDistance < 1.0f) mDistance = 1.0f;

         UpdateTargetTransform();
      }

   private:

      void UpdateTargetTransform()
      {
         Transform transform;
         
         transform.SetTranslation(
            mCenter[0] + mDistance * sgCos(mElevation) * sgSin(mAzimuth),
            mCenter[1] + mDistance * sgCos(mElevation) * -sgCos(mAzimuth),
            mCenter[2] + mDistance * sgSin(mElevation)
         );
         
         transform.SetRotation(
            mAzimuth, 
            -mElevation, 
            0.0f
         );
         
         mTarget->SetTransform(&transform);
         
         sgMat4 mat;
         
         transform.Get(mat);
         
         sgVec3 vec = {-0.3, 1, -0.225};
         
         sgXformPnt3(vec, mat);
         
         osg::Matrix osgMat;
         
         osgMat.makeTranslate(vec[0], vec[1], vec[2]);
         
         compassTransform->setMatrix(osgMat);
      }

      Mouse* mMouse;

      Transformable* mTarget;

      float mAzimuth, mElevation, mDistance;

      sgVec3 mCenter;
      
      float mLastX, mLastY;
};

/**
 * The number of lines in each direction.
 */
const int gridLineCount = 49;

/**
 * The amount of space between each line.
 */
const float gridLineSpacing = 1.0f;

static void makeGrids()
{
   const int numVertices = 2*2*gridLineCount;

   osg::Vec3 vertices[numVertices];

   float length = (gridLineCount-1)*gridLineSpacing;
   
   int ptr = 0;
   
   for(int i=0;i<gridLineCount;i++)
   {
      vertices[ptr++].set(
         -length/2 + i*gridLineSpacing,
         length/2,
         0.0f
      );
      
      vertices[ptr++].set(
         -length/2 + i*gridLineSpacing,
         -length/2,
         0.0f
      );
   }

   for(i=0;i<gridLineCount;i++)
   {
      vertices[ptr++].set(
         length/2,
         -length/2 + i*gridLineSpacing,
         0.0f
      );
      
      vertices[ptr++].set(
         -length/2,
         -length/2 + i*gridLineSpacing,
         0.0f
      );
   }
   
   osg::Geometry* geometry = new osg::Geometry;

   geometry->setVertexArray(
      new osg::Vec3Array(numVertices, vertices)
   );

   geometry->addPrimitiveSet(
      new osg::DrawArrays(
         osg::PrimitiveSet::LINES, 0, numVertices
      )
   );

   osg::Geode* geode = new osg::Geode;

   geode->addDrawable(geometry);

   geode->getOrCreateStateSet()->setMode(GL_LIGHTING, 0);

   xyGridTransform = new osg::MatrixTransform;
   
   xyGridTransform->addChild(geode);
   
   yzGridTransform = new osg::MatrixTransform;
   
   yzGridTransform->setMatrix(
      osg::Matrix::rotate(
         osg::PI_2, 0, 1, 0
      )
   );
   
   yzGridTransform->addChild(geode);
   
   yzGridTransform->setNodeMask(0x0);
   
   xzGridTransform = new osg::MatrixTransform;
   
   xzGridTransform->setMatrix(
      osg::Matrix::rotate(
         osg::PI_2, 1, 0, 0
      )
   );
   
   xzGridTransform->addChild(geode);
   
   xzGridTransform->setNodeMask(0x0);
   
   sceneGroup->addChild(xyGridTransform);
   sceneGroup->addChild(yzGridTransform);
   sceneGroup->addChild(xzGridTransform);
}

///Called from the Open Previous menu items
static void OpenHistoryCB(Fl_Widget *, void *v)
{
   LoadFile( (char*)v );
}

///Read the preference file and populate the particular widgets
void ReadPrefs(void)
{
   for (int i=0; i<5; i++)
   {
      appPrefs.get( Fl_Preferences::Name("file%d", i), absoluteHistory[i],
                     "", sizeof(absoluteHistory[i]));

      if (absoluteHistory[i][0])
      {
         fl_filename_relative(relativeHistory[i], sizeof(relativeHistory[i]), absoluteHistory[i]);
         menu_MainMenu[i+4].flags = 0;
      }
      else
      {
         menu_MainMenu[i+4].flags = FL_MENU_INVISIBLE;
      }
   }

   if (!absoluteHistory[0][0]) menu_MainMenu[3].flags |= FL_MENU_INACTIVE;

   //populate the menu items with the file paths
   for (int i=0; i<5; i++)
   {
      menu_MainMenu[i+4].label(relativeHistory[i]);
      menu_MainMenu[i+4].callback(OpenHistoryCB, absoluteHistory[i]);
   }
}

int main( int argc, char **argv )
{
   editorWindow = make_window();

   editorWindow->show();

   sceneGroup = viewWidget->GetScene()->GetSceneNode();

   viewWidget->Config();
   
   makeGrids();

   Compass* compass = new Compass(viewWidget->GetCamera());
   
   viewWidget->GetScene()->AddDrawable(compass);
   
   compassTransform = (osg::MatrixTransform*)compass->GetOSGNode();
   
   ::OrbitMotionModel obm(viewWidget->GetMouse(), viewWidget->GetCamera());

   psEditorGUI_New(NULL, NULL);

   ReadPrefs();

   //load the first filename passed in on the command line
   if (argc>1)
   {
      std::string filename = argv[1];
      LoadFile(filename);
   }

   viewWidget->SetEvent( FL_PUSH );
   viewWidget->SetEvent( FL_RELEASE );
   viewWidget->SetEvent( FL_ENTER );
   viewWidget->SetEvent( FL_LEAVE );
   viewWidget->SetEvent( FL_DRAG );
   viewWidget->SetEvent( FL_MOVE );
   viewWidget->SetEvent( FL_MOUSEWHEEL );
   viewWidget->SetEvent( FL_FOCUS );
   viewWidget->SetEvent( FL_UNFOCUS );
   viewWidget->SetEvent( FL_KEYDOWN );
   viewWidget->SetEvent( FL_KEYUP );
   
   Fl::run();
}