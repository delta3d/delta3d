#include "dtCore/dt.h"
#include "dtCore/camera.h"
#include "dtCore/globals.h"
#include "dtCore/system.h"
#include "dtCore/mouse.h"
#include "dtUtil/matrixutil.h"

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

#include "FL/Fl_Color_Chooser.H"
#include "FL/Fl_File_Chooser.H"
#include "FL/fl_ask.H"

#include "psEditorGUI.h"

#include <string.h>
#include <math.h>

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
 * A single particle system in the particle system group.
 */
struct ParticleSystemLayer
{
   /**
    * The geode that holds the drawable particle system, and whose name is
    * the name of the layer.
    */
   RefPtr<osg::Geode> mGeode;
   
   /**
    * The active particle system.
    */
   RefPtr<osgParticle::ParticleSystem> mParticleSystem;
   
   /**
    * The active particle template.
    */
   Particle* mParticle;
   
   /**
    * The transform that controls the position of the emitter.
    */
   RefPtr<osg::MatrixTransform> mEmitterTransform;
   
   /**
    * The active emitter.
    */
   RefPtr<osgParticle::ModularEmitter> mModularEmitter;
   
   /**
    * The active program.
    */
   RefPtr<osgParticle::ModularProgram> mModularProgram;
};

/**
 * The particle system layers.
 */
static std::vector<ParticleSystemLayer> layers;

/**
 * The particle system updater.
 */
static osgParticle::ParticleSystemUpdater* particleSystemUpdater = NULL;

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
   int layer = Layers->value() - 1;
   
   RandomRateCounterParameters->show();

   RandomRateCounter* rrc = 
      (RandomRateCounter*)layers[layer].mModularEmitter->getCounter();

   RandomRateCounter_MinRate->value(rrc->getRateRange().minimum);
   RandomRateCounter_MaxRate->value(rrc->getRateRange().maximum);
}

/**
 * Activates and updates the point placer parameters.
 */
static void updatePointPlacerParameters()
{
   int layer = Layers->value() - 1;
   
   Placer_Type->value(0);
   
   PointPlacerParameters->show();
   SectorPlacerParameters->hide();
   SegmentPlacerParameters->hide();
   MultiSegmentPlacerParameters->hide();

   PointPlacer* pp = 
      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();

   PointPlacer_X->value(pp->getCenter()[0]);
   PointPlacer_Y->value(pp->getCenter()[1]);
   PointPlacer_Z->value(pp->getCenter()[2]);
}

/**
 * Activates and updates the sector placer parameters.
 */
static void updateSectorPlacerParameters()
{
   int layer = Layers->value() - 1;
   
   Placer_Type->value(1);
   
   PointPlacerParameters->hide();
   SectorPlacerParameters->show();
   SegmentPlacerParameters->hide();
   MultiSegmentPlacerParameters->hide();

   SectorPlacer* sp = 
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

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
   int layer = Layers->value() - 1;
   
   Placer_Type->value(2);
   
   PointPlacerParameters->hide();
   SectorPlacerParameters->hide();
   SegmentPlacerParameters->show();
   MultiSegmentPlacerParameters->hide();

   SegmentPlacer* sp = 
      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

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
   int layer = Layers->value() - 1;
   
   Placer_Type->value(3);
   
   PointPlacerParameters->hide();
   SectorPlacerParameters->hide();
   SegmentPlacerParameters->hide();
   MultiSegmentPlacerParameters->show();

   MultiSegmentPlacer* msp = 
      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

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
 * Updates the state of the parameter tabs.
 */
static void updateParameterTabs()
{
   int layer = Layers->value() - 1;
   
   if(layer >= 0)
   {
      ParameterTabs->activate();
      
      Particles_Alignment->value(
         layers[layer].mParticleSystem->getParticleAlignment()
      );

      Particles_Shape->value(
         layers[layer].mParticle->getShape()
      );

      Particles_Lifetime->value(
         layers[layer].mParticle->getLifeTime()
      );

      Particles_Radius->value(
         layers[layer].mParticle->getRadius()
      );

      Particles_Mass->value(
         layers[layer].mParticle->getMass()
      );

      Particles_MinSize->value(
         layers[layer].mParticle->getSizeRange().minimum
      );

      Particles_MaxSize->value(
         layers[layer].mParticle->getSizeRange().maximum
      );

      Particles_MinAlpha->value(
         layers[layer].mParticle->getAlphaRange().minimum
      );

      Particles_MaxAlpha->value(
         layers[layer].mParticle->getAlphaRange().maximum
      );

      Particles_MinR->value(
         layers[layer].mParticle->getColorRange().minimum[0]
      );

      Particles_MinG->value(
         layers[layer].mParticle->getColorRange().minimum[1]
      );

      Particles_MinB->value(
         layers[layer].mParticle->getColorRange().minimum[2]
      );

      Particles_MinColor->color(
         fl_rgb_color(
            (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
            (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
            (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
         )
      );

      Particles_MinColor->redraw();

      Particles_MaxR->value(
         layers[layer].mParticle->getColorRange().maximum[0]
      );

      Particles_MaxG->value(
         layers[layer].mParticle->getColorRange().maximum[1]
      );

      Particles_MaxB->value(
         layers[layer].mParticle->getColorRange().maximum[2]
      );

      Particles_MaxColor->color(
         fl_rgb_color(
            (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
            (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
            (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
         )
      );

      Particles_MaxColor->redraw();

      Particles_Emitter_Endless->value(layers[layer].mModularEmitter->isEndless());
      
      Particles_Emitter_Lifetime->value(layers[layer].mModularEmitter->getLifeTime());
      
      if(layers[layer].mModularEmitter->isEndless())
      {
         Particles_Emitter_Lifetime->deactivate();
      }
      else
      {
         Particles_Emitter_Lifetime->activate();
      }
      
      Particles_Emitter_StartTime->value(layers[layer].mModularEmitter->getStartTime());
      
      Particles_Emitter_ResetTime->value(layers[layer].mModularEmitter->getResetTime());
      
      std::string textureFile = "";

      osg::StateSet* ss =
         layers[layer].mParticleSystem->getStateSet();

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

      Particles_TexturePreview->SetTexture(textureFile);
      
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


      Counter* counter = layers[layer].mModularEmitter->getCounter();

      if(IS_A(counter, RandomRateCounter*))
      {
         updateRandomRateCounterParameters();
      }


      Placer* placer = layers[layer].mModularEmitter->getPlacer();

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


      Shooter* shooter = layers[layer].mModularEmitter->getShooter();

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

      for(int i=0;i<layers[layer].mModularProgram->numOperators();i++)
      {
         Operator* op = layers[layer].mModularProgram->getOperator(i);

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
   else
   {
      ParameterTabs->deactivate(); 
   }
}

/**
 * Updates the state of the layers widget.
 */
static void updateLayers()
{
   Layers->clear();
   
   for(unsigned int i=0;i<layers.size();i++)
   {
      Layers->add(layers[i].mGeode->getName().c_str());
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


/**
 * Synchronizes all emitters by resetting them to the zero time.
 */
static void resetEmitters()
{
   for(unsigned int i=0;i<layers.size();i++)
   {
      layers[i].mModularEmitter->setCurrentTime(0.0);
      layers[i].mModularProgram->setCurrentTime(0.0);
   }
}


void psEditorGUI_NewLayer(Fl_Button*, void*)
{
   ParticleSystemLayer layer;
   
   
   layer.mParticleSystem = new osgParticle::ParticleSystem();

   layer.mParticleSystem->setDefaultAttributes("", true, false);

   layer.mParticle = new Particle();

   layer.mParticleSystem->setDefaultParticleTemplate(*layer.mParticle);


   layer.mGeode = new osg::Geode();

   char buf[256];
   
   sprintf(buf, "Layer %d", layers.size());
   
   layer.mGeode->setName(buf);
   
   layer.mGeode->addDrawable(layer.mParticleSystem.get());

   particleSystemGroup->addChild(layer.mGeode.get());


   layer.mEmitterTransform = new osg::MatrixTransform();

   layer.mModularEmitter = new ModularEmitter();

   layer.mModularEmitter->setParticleSystem(layer.mParticleSystem.get());

   RandomRateCounter* rrc = new RandomRateCounter();

   rrc->setRateRange(20, 30);

   layer.mModularEmitter->setCounter(
      rrc
   );

   layer.mModularEmitter->setPlacer(
      new PointPlacer()
   );

   layer.mModularEmitter->setShooter(
      new RadialShooter()
   );

   layer.mEmitterTransform->addChild(layer.mModularEmitter.get());

   particleSystemGroup->addChild(layer.mEmitterTransform.get());


   layer.mModularProgram = new ModularProgram();
 
   layer.mModularProgram->setParticleSystem(layer.mParticleSystem.get());

   particleSystemGroup->addChild(layer.mModularProgram.get());


   particleSystemUpdater->addParticleSystem(layer.mParticleSystem.get());


   layers.push_back(layer);
   
   resetEmitters();
   
   Layers->add(layer.mGeode->getName().c_str());
   
   Layers->value(layers.size());
   
   
   psEditorGUI_LayerSelect(NULL, NULL);
}

void psEditorGUI_DeleteLayer(Fl_Button*, void*)
{
   unsigned int layer = Layers->value() - 1;
   
   particleSystemGroup->removeChild(layers[layer].mGeode.get());
   particleSystemGroup->removeChild(layers[layer].mEmitterTransform.get());
   particleSystemGroup->removeChild(layers[layer].mModularProgram.get());
   
   particleSystemUpdater->removeParticleSystem(layers[layer].mParticleSystem.get());
   
   layers.erase(layers.begin() + layer);
   
   Layers->remove(Layers->value());
   
   if(layers.size() > layer)
   {
      Layers->value(layer + 1);
   }
   else
   {
      Layers->value(layer);
   }
   
   psEditorGUI_LayerSelect(NULL, NULL);
}

void psEditorGUI_RenameLayer(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   const char* input = fl_input("Rename Layer", layers[layer].mGeode->getName().c_str());
   
   if(input != NULL)
   {
      layers[layer].mGeode->setName(input);
      Layers->text(Layers->value(), input);
   }
}

void psEditorGUI_ResetLayers(Fl_Button*, void*)
{
   resetEmitters();
}

void psEditorGUI_LayerSelect(Fl_Browser*, void*)
{
   if(Layers->value() == 0)
   {
      Layers_DeleteButton->deactivate();
      Layers_RenameButton->deactivate();
      Layers_HideButton->deactivate();
   }
   else
   {
      int layer = Layers->value() - 1;

      Layers_DeleteButton->activate();
      Layers_RenameButton->activate();   
      Layers_HideButton->activate();

      if (layers[layer].mModularEmitter->isEnabled() )      
         //Layers_HideButton->label("Hide");
         Layers_HideButton->value(1);
      else
         //Layers_HideButton->label("Show");
         Layers_HideButton->value(0);
   }
   
   updateParameterTabs();
}


void psEditorGUI_New(Fl_Menu_*, void*)
{
   if(particleSystemGroup != NULL)
   {
      sceneGroup->removeChild(particleSystemGroup);
   }

   particleSystemGroup = new osg::Group();


   particleSystemUpdater = new ParticleSystemUpdater();

   particleSystemGroup->addChild(particleSystemUpdater);


   sceneGroup->addChild(particleSystemGroup);


   setParticleSystemFilename("");
   
   
   layers.clear();
   
   updateLayers();
   
   psEditorGUI_NewLayer(NULL, NULL);
}

void psEditorGUI_HideLayer( Fl_Light_Button*, void*)
{
   int layer = Layers->value() - 1;

   if (layers[layer].mModularEmitter->isEnabled() )
   {
      layers[layer].mModularEmitter->setEnabled(false);
      //Layers_HideButton->label("Show");
      Layers_HideButton->value(0);
   }
   else
   {
      layers[layer].mModularEmitter->setEnabled(true);
      //Layers_HideButton->label("Hide");
      Layers_HideButton->value(1);
   }
}

///Load the given filename
void LoadFile( std::string filename, bool import = false )
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

      std::vector<ParticleSystemLayer> newLayers;
      
      ParticleSystemUpdater* newParticleSystemUpdater = NULL;

      unsigned int i;
      
      for(i=0;i<newParticleSystemGroup->getNumChildren();i++)
      {
         node = newParticleSystemGroup->getChild(i);

         if(IS_A(node, osg::Geode*))
         {
            ParticleSystemLayer layer;
            
            layer.mGeode = (osg::Geode*)node;

            for(unsigned int j=0;j<layer.mGeode->getNumDrawables();j++)
            {
               osg::Drawable* drawable = layer.mGeode->getDrawable(j);

               if(IS_A(drawable, osgParticle::ParticleSystem*))
               {
                  layer.mParticleSystem = (osgParticle::ParticleSystem*)drawable;

                  layer.mParticle = new Particle(
                     layer.mParticleSystem->getDefaultParticleTemplate()
                  );
               }
            }
            
            if(layer.mGeode->getName() == "")
            {
               char buf[256];
               
               sprintf(buf, "Layer %d", newLayers.size() + (import ? layers.size() : 0));
               
               layer.mGeode->setName(buf);
            }
            
            newLayers.push_back(layer);
         }
         else if(IS_A(node, ParticleSystemUpdater*))
         {
            newParticleSystemUpdater = (ParticleSystemUpdater*)node;
         }
      }
      
      for(i=0;i<newParticleSystemGroup->getNumChildren();i++)
      {
         node = newParticleSystemGroup->getChild(i);
         
         if(IS_A(node, osg::MatrixTransform*))
         {
            osg::MatrixTransform* newEmitterTransform = (osg::MatrixTransform*)node;

            for(unsigned int j=0;j<newEmitterTransform->getNumChildren();j++)
            {
               osg::Node* childNode = newEmitterTransform->getChild(j);

               if(IS_A(childNode, ModularEmitter*))
               {
                  osgParticle::ModularEmitter* newModularEmitter = (ModularEmitter*)childNode;
                  
                  for(unsigned int k=0;k<newLayers.size();k++)
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
         else if(IS_A(node, ModularProgram*))
         {
            osgParticle::ModularProgram* newModularProgram = (ModularProgram*)node;
            
            for(unsigned int j=0;j<newLayers.size();j++)
            {
               if(newLayers[j].mParticleSystem == newModularProgram->getParticleSystem())
               {
                  newLayers[j].mModularProgram = newModularProgram;
                  break;
               }
            }
         }
      }
      
      if(newParticleSystemUpdater != NULL)
      {
         int newLayer = 1;
         
         if(import)
         {
            newLayer = layers.size() + 1;
            
            for(i=0;i<newLayers.size();i++)
            {
               particleSystemGroup->addChild(newLayers[i].mGeode.get());
               particleSystemGroup->addChild(newLayers[i].mEmitterTransform.get());
               particleSystemGroup->addChild(newLayers[i].mModularProgram.get());
               
               particleSystemUpdater->addParticleSystem(newLayers[i].mParticleSystem.get());
               
               layers.push_back(newLayers[i]);
            }
            
            resetEmitters();
         }
         else
         {
            sceneGroup->removeChild(particleSystemGroup);         

            particleSystemGroup = newParticleSystemGroup;
            
            layers = newLayers;
            
            particleSystemUpdater = newParticleSystemUpdater;

            sceneGroup->addChild(particleSystemGroup);
         }
         
         updateLayers();
         
         Layers->value(newLayer);
         
         psEditorGUI_LayerSelect(NULL, NULL);

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

void psEditorGUI_Import(Fl_Menu_*, void*)
{
   char* filename =
      fl_file_chooser(
      "Import",
      "Particle Systems (*.osg)",
      particleSystemFilename.c_str(),
      1
      );

   if (filename != NULL)
   {
      std::string name = filename;
      LoadFile(name, true);
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
      resetEmitters();
      
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
   int layer = Layers->value() - 1;
   
   layers[layer].mParticleSystem->setParticleAlignment(
      (osgParticle::ParticleSystem::Alignment)Particles_Alignment->value()
   );
}

void psEditorGUI_Particles_SetShape(Fl_Choice*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setShape(
      (Particle::Shape)Particles_Shape->value()
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetLifetime(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setLifeTime(
      Particles_Lifetime->value()
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMinSize(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setSizeRange(
      rangef(
         Particles_MinSize->value(),
         Particles_MaxSize->value()
      )
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMaxSize(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setSizeRange(
      rangef(
         Particles_MinSize->value(),
         Particles_MaxSize->value()
      )
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMinAlpha(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setAlphaRange(
      rangef(
         Particles_MinAlpha->value(),
         Particles_MaxAlpha->value()
      )
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMaxAlpha(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setAlphaRange(
      rangef(
         Particles_MinAlpha->value(),
         Particles_MaxAlpha->value()
      )
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetRadius(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setRadius(
      Particles_Radius->value()
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMass(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setMass(
      Particles_Mass->value()
   );

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetTexture(Fl_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticleSystem->setDefaultAttributes(
      Particles_Texture->value(),
      Particles_Emissive->value(),
      Particles_Lighting->value()
   );
   
   Particles_TexturePreview->SetTexture(Particles_Texture->value());
}

void psEditorGUI_Particles_ChooseTexture(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
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

      Particles_TexturePreview->SetTexture(filename);
      
      layers[layer].mParticleSystem->setDefaultAttributes(
         Particles_Texture->value(),
         Particles_Emissive->value(),
         Particles_Lighting->value()
      );
   }
}

void psEditorGUI_Particles_SetEmissive(Fl_Check_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticleSystem->setDefaultAttributes(
      Particles_Texture->value(),
      Particles_Emissive->value(),
      Particles_Lighting->value()
   );
}

void psEditorGUI_Particles_SetLighting(Fl_Check_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticleSystem->setDefaultAttributes(
      Particles_Texture->value(),
      Particles_Emissive->value(),
      Particles_Lighting->value()
   );
}

void psEditorGUI_Particles_SetMinR(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setColorRange(
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
         (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
         (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
         (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
      )
   );  

   Particles_MinColor->redraw();

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMinG(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setColorRange(
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
         (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
         (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
         (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
      )
   );  

   Particles_MinColor->redraw();

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMinB(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setColorRange(
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
         (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
         (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
         (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
      )
   );  

   Particles_MinColor->redraw();

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_ChooseMinColor(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   double r = Particles_MinR->value(),
          g = Particles_MinG->value(),
          b = Particles_MinB->value();

   if(fl_color_chooser("Choose Min. Color", r, g, b))
   {
      Particles_MinR->value(r);
      Particles_MinG->value(g);
      Particles_MinB->value(b);

      layers[layer].mParticle->setColorRange(
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
            (uchar)(layers[layer].mParticle->getColorRange().minimum[0]*255),
            (uchar)(layers[layer].mParticle->getColorRange().minimum[1]*255),
            (uchar)(layers[layer].mParticle->getColorRange().minimum[2]*255)
         )
      );

      Particles_MinColor->redraw();

      layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
   }
}

void psEditorGUI_Particles_SetMaxR(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setColorRange(
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
         (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
         (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
         (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
      )
   );   

   Particles_MaxColor->redraw();

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMaxG(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setColorRange(
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
         (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
         (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
         (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
      )
   );   

   Particles_MaxColor->redraw();

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_SetMaxB(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mParticle->setColorRange(
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
         (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
         (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
         (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
      )
   );   

   Particles_MaxColor->redraw();

   layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
}

void psEditorGUI_Particles_ChooseMaxColor(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   double r = Particles_MaxR->value(),
          g = Particles_MaxG->value(),
          b = Particles_MaxB->value();

   if(fl_color_chooser("Choose Max. Color", r, g, b))
   {
      Particles_MaxR->value(r);
      Particles_MaxG->value(g);
      Particles_MaxB->value(b);

      layers[layer].mParticle->setColorRange(
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
            (uchar)(layers[layer].mParticle->getColorRange().maximum[0]*255),
            (uchar)(layers[layer].mParticle->getColorRange().maximum[1]*255),
            (uchar)(layers[layer].mParticle->getColorRange().maximum[2]*255)
         )
      );

      Particles_MaxColor->redraw();

      layers[layer].mParticleSystem->setDefaultParticleTemplate(*layers[layer].mParticle);
   }
}

void psEditorGUI_Particles_SetEmitterEndless(Fl_Check_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mModularEmitter->setEndless(
      Particles_Emitter_Endless->value()
   );
   
   if(layers[layer].mModularEmitter->isEndless())
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
   int layer = Layers->value() - 1;
   
   layers[layer].mModularEmitter->setLifeTime(
      Particles_Emitter_Lifetime->value()
   );
}

void psEditorGUI_Particles_SetEmitterStartTime(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mModularEmitter->setStartTime(
      Particles_Emitter_StartTime->value()
   );
}

void psEditorGUI_Particles_SetEmitterResetTime(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   layers[layer].mModularEmitter->setResetTime(
      Particles_Emitter_ResetTime->value()
   );
}

void psEditorGUI_RandomRateCounter_SetMinRate(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RandomRateCounter* rrc = 
      (RandomRateCounter*)layers[layer].mModularEmitter->getCounter();

   rrc->setRateRange(
      rangef(
         RandomRateCounter_MinRate->value(),
         RandomRateCounter_MaxRate->value()
      )
   );
}

void psEditorGUI_RandomRateCounter_SetMaxRate(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RandomRateCounter* rrc = 
      (RandomRateCounter*)layers[layer].mModularEmitter->getCounter();

   rrc->setRateRange(
      rangef(
         RandomRateCounter_MinRate->value(),
         RandomRateCounter_MaxRate->value()
      )
   );
}

void psEditorGUI_Counter_SetType(Fl_Choice* counterType, void*)
{
   int layer = Layers->value() - 1;
   
   Counter* counter = layers[layer].mModularEmitter->getCounter();

   switch(counterType->value())
   {
      case 0:
         if(!IS_A(counter, RandomRateCounter*))
         {
            layers[layer].mModularEmitter->setCounter(
               new RandomRateCounter()
            );

            updateRandomRateCounterParameters();
         }
         break;
   }
}

void psEditor_SegmentPlacer_VertexA_SetX(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SegmentPlacer* sp = 
      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setVertexA(
      SegmentPlacer_A_X->value(),
      SegmentPlacer_A_Y->value(),
      SegmentPlacer_A_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexA_SetY(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SegmentPlacer* sp = 
      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setVertexA(
      SegmentPlacer_A_X->value(),
      SegmentPlacer_A_Y->value(),
      SegmentPlacer_A_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexA_SetZ(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SegmentPlacer* sp = 
      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setVertexA(
      SegmentPlacer_A_X->value(),
      SegmentPlacer_A_Y->value(),
      SegmentPlacer_A_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexB_SetX(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SegmentPlacer* sp = 
      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setVertexB(
      SegmentPlacer_B_X->value(),
      SegmentPlacer_B_Y->value(),
      SegmentPlacer_B_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexB_SetY(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SegmentPlacer* sp = 
      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setVertexB(
      SegmentPlacer_B_X->value(),
      SegmentPlacer_B_Y->value(),
      SegmentPlacer_B_Z->value()
   );
}

void psEditor_SegmentPlacer_VertexB_SetZ(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SegmentPlacer* sp = 
      (SegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setVertexB(
      SegmentPlacer_B_X->value(),
      SegmentPlacer_B_Y->value(),
      SegmentPlacer_B_Z->value()
   );
}

void psEditorGUI_SectorPlacer_Center_SetX(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SectorPlacer* sp =
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setCenter(
      SectorPlacer_X->value(),
      SectorPlacer_Y->value(),
      SectorPlacer_Z->value()
   );
}

void psEditorGUI_SectorPlacer_Center_SetY(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SectorPlacer* sp =
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setCenter(
      SectorPlacer_X->value(),
      SectorPlacer_Y->value(),
      SectorPlacer_Z->value()
   );
}

void psEditorGUI_SectorPlacer_Center_SetZ(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SectorPlacer* sp =
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setCenter(
      SectorPlacer_X->value(),
      SectorPlacer_Y->value(),
      SectorPlacer_Z->value()
   );
}

void psEditorGUI_SectorPlacer_SetMinRadius(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SectorPlacer* sp =
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setRadiusRange(
      SectorPlacer_MinRadius->value(),
      SectorPlacer_MaxRadius->value()
   );
}

void psEditorGUI_SectorPlacer_SetMaxRadius(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SectorPlacer* sp =
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setRadiusRange(
      SectorPlacer_MinRadius->value(),
      SectorPlacer_MaxRadius->value()
   );
}

void psEditorGUI_SectorPlacer_SetMinPhi(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SectorPlacer* sp =
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setPhiRange(
      SectorPlacer_MinPhi->value(),
      SectorPlacer_MaxPhi->value()
   );
}

void psEditorGUI_SectorPlacer_SetMaxPhi(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   SectorPlacer* sp =
      (SectorPlacer*)layers[layer].mModularEmitter->getPlacer();

   sp->setPhiRange(
      SectorPlacer_MinPhi->value(),
      SectorPlacer_MaxPhi->value()
   );
}

void psEditorGUI_PointPlacer_SetX(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   PointPlacer* pp =
      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();

   pp->setCenter(
      PointPlacer_X->value(),
      PointPlacer_Y->value(),
      PointPlacer_Z->value()
   );
}

void psEditorGUI_PointPlacer_SetY(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   PointPlacer* pp =
      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();

   pp->setCenter(
      PointPlacer_X->value(),
      PointPlacer_Y->value(),
      PointPlacer_Z->value()
   );
}

void psEditorGUI_PointPlacer_SetZ(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   PointPlacer* pp =
      (PointPlacer*)layers[layer].mModularEmitter->getPlacer();

   pp->setCenter(
      PointPlacer_X->value(),
      PointPlacer_Y->value(),
      PointPlacer_Z->value()
   );
}

void psEditorGUI_MultiSegmentPlacer_VerticesChanged(Fl_Browser*, void*)
{
   int layer = Layers->value() - 1;
   
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

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
   int layer = Layers->value() - 1;
   
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

   MultiSegmentPlacer_Vertices->add("0, 0, 0");

   msp->addVertex(0, 0, 0);

   MultiSegmentPlacer_Vertices->value(msp->numVertices());

   MultiSegmentPlacer_Vertices->do_callback();
}

void psEditorGUI_MultiSegmentPlacer_DeleteVertex(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

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
   int layer = Layers->value() - 1;
   
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

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
   int layer = Layers->value() - 1;
   
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

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
   int layer = Layers->value() - 1;
   
   MultiSegmentPlacer* msp =
      (MultiSegmentPlacer*)layers[layer].mModularEmitter->getPlacer();

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
   int layer = Layers->value() - 1;
   
   Placer* placer = layers[layer].mModularEmitter->getPlacer();

   switch(placerType->value())
   {
      case 0:
         if(!IS_A(placer, PointPlacer*))
         {
            layers[layer].mModularEmitter->setPlacer(
               new PointPlacer()
            );

            updatePointPlacerParameters();
         }
         break;

      case 1:
         if(!IS_A(placer, SectorPlacer*))
         {
            layers[layer].mModularEmitter->setPlacer(
               new SectorPlacer()
            );

            updateSectorPlacerParameters();
         }
         break;

      case 2:
         if(!IS_A(placer, SegmentPlacer*))
         {
            layers[layer].mModularEmitter->setPlacer(
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

            layers[layer].mModularEmitter->setPlacer(msp);

            updateMultiSegmentPlacerParameters();
         }
         break;
   }
}

void psEditorGUI_RadialShooter_SetMinTheta(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

   rs->setThetaRange(
      RadialShooter_MinTheta->value(),
      RadialShooter_MaxTheta->value()
   );
}

void psEditorGUI_RadialShooter_SetMaxTheta(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

   rs->setThetaRange(
      RadialShooter_MinTheta->value(),
      RadialShooter_MaxTheta->value()
   );
}

void psEditorGUI_RadialShooter_SetMinPhi(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

   rs->setPhiRange(
      RadialShooter_MinPhi->value(),
      RadialShooter_MaxPhi->value()
   );
}

void psEditorGUI_RadialShooter_SetMaxPhi(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

   rs->setPhiRange(
      RadialShooter_MinPhi->value(),
      RadialShooter_MaxPhi->value()
   );
}

void psEditorGUI_RadialShooter_SetMinInitialSpeed(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

   rs->setInitialSpeedRange(
      RadialShooter_MinInitialSpeed->value(),
      RadialShooter_MaxInitialSpeed->value()
   );
}

void psEditorGUI_RadialShooter_SetMaxInitialSpeed(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

   rs->setInitialSpeedRange(
      RadialShooter_MinInitialSpeed->value(),
      RadialShooter_MaxInitialSpeed->value()
   );
}

void psEditorGUI_RadialShooter_SetMinInitialRotationalSpeedX(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

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
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

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
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

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
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

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
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

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
   int layer = Layers->value() - 1;
   
   RadialShooter* rs = 
      (RadialShooter*)layers[layer].mModularEmitter->getShooter();

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   if(i >= 0)
   {
      Program_DeleteOperator->activate();

      Operator* op = layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   ForceOperator* fo = new ForceOperator();

   layers[layer].mModularProgram->addOperator(fo);

   Program_Operators->add("Force");

   Program_Operators->value(Program_Operators->size());

   Program_Operators->do_callback();
}

void psEditorGUI_Program_NewAcceleration(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   AccelOperator* ao = new AccelOperator();

   layers[layer].mModularProgram->addOperator(ao);

   Program_Operators->add("Acceleration");

   Program_Operators->value(Program_Operators->size());

   Program_Operators->do_callback();
}

void psEditorGUI_Program_NewFluidFriction(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   FluidFrictionOperator* ffo = new FluidFrictionOperator();

   layers[layer].mModularProgram->addOperator(ffo);

   Program_Operators->add("Fluid Friction");

   Program_Operators->value(Program_Operators->size());

   Program_Operators->do_callback();
}

void psEditorGUI_Program_DeleteOperator(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   layers[layer].mModularProgram->removeOperator(i);

   Program_Operators->remove(i + 1);

   if(i < Program_Operators->size())
   {
      Program_Operators->value(i+1);
   }

   Program_Operators->do_callback();
}

void psEditorGUI_FluidFriction_Air(Fl_Button*, void*)
{
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);

   ffo->setFluidDensity(
      FluidFriction_Density->value()
   );
}

void psEditorGUI_FluidFriction_SetViscosity(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);

   ffo->setFluidViscosity(
      FluidFriction_Viscosity->value()
   );
}

void psEditorGUI_FluidFriction_SetOverrideRadius(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   FluidFrictionOperator* ffo = 
      (FluidFrictionOperator*)layers[layer].mModularProgram->getOperator(i);

   ffo->setOverrideRadius(
      FluidFriction_OverrideRadius->value()
   );
}

void psEditorGUI_Force_SetX(Fl_Value_Input*, void*)
{
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   ForceOperator* fo =
      (ForceOperator*)layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   ForceOperator* fo =
      (ForceOperator*)layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   ForceOperator* fo =
      (ForceOperator*)layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   AccelOperator* ao =
      (AccelOperator*)layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   AccelOperator* ao =
      (AccelOperator*)layers[layer].mModularProgram->getOperator(i);

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
   int layer = Layers->value() - 1;
   
   int i = Program_Operators->value() - 1;

   AccelOperator* ao =
      (AccelOperator*)layers[layer].mModularProgram->getOperator(i);

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

         mCenter.set(0, 0, 0);
         
         UpdateTargetTransform();
      }

      virtual void ButtonPressed(Mouse* mouse, MouseButton button)
      {
         mouse->GetPosition(mLastX, 
            mLastY);
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
            
            osg::Matrix mat;
            
            transform.Get(mat);
            
            osg::Vec3 offset (-dX*mDistance*0.25f, 0, -dY*mDistance*0.25f);
            
            //sgXformVec3(offset, mat);
            osg::Matrix::transform3x3(offset, mat);
            
            //sgAddVec3(mCenter, offset);
            mCenter += offset;
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
            mCenter[0] + mDistance * cosf(osg::DegreesToRadians(mElevation)) * sinf(osg::DegreesToRadians(mAzimuth)),
            mCenter[1] + mDistance * cosf(osg::DegreesToRadians(mElevation)) * -cosf(osg::DegreesToRadians(mAzimuth)),
            mCenter[2] + mDistance * sinf(osg::DegreesToRadians(mElevation))
         );
         
         transform.SetRotation(
            mAzimuth, 
            -mElevation, 
            0.0f
         );
         
         mTarget->SetTransform(&transform);
         
         osg::Matrix mat;
         
         transform.Get(mat);
         
         osg::Vec3 vec (-0.3, 1, -0.225);
         
         //sgXformPnt3(vec, mat);
         dtUtil::MatrixUtil::TransformVec3(vec, mat);
         
         osg::Matrix osgMat;
         
         osgMat.makeTranslate(vec[0], vec[1], vec[2]);
         
         compassTransform->setMatrix(osgMat);
      }

      Mouse* mMouse;

      Transformable* mTarget;

      float mAzimuth, mElevation, mDistance;

      osg::Vec3 mCenter;
      
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

   for(int i=0;i<gridLineCount;i++)
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
