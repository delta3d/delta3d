#include "dtCore/system.h"
#include "dtCore/skybox.h"
#include "dtCore/scene.h"

#include <osg/ShapeDrawable>
#include <osg/Depth>
#include <osg/PolygonMode> ///for wireframe rendering
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/Texture>

using namespace dtCore;

IMPLEMENT_MANAGEMENT_LAYER(SkyBox)

SkyBox::SkyBox(std::string name):
EnvEffect(name),
mGeode(NULL)
{
   RegisterInstance(this);

   AddSender(System::GetSystem()); //hook us up to the System

   //need this made so the SkyBox can be added to the Environment
   mNode = new osg::Group(); 
}

SkyBox::~SkyBox(void)
{
   RemoveSender(System::GetSystem());
   DeregisterInstance(this);
}


/** Private method that actually creates the SkyBox geometry.  It is assumed 
 *  that the textures have already been assigned.
 */
void dtCore::SkyBox::Config(void)
{
   osg::Group *group = new osg::Group();

   mXform = new MoveEarthySkyWithEyePointTransform();

   mXform->setCullingActive(false);
   mXform->addChild( MakeBox() );

   group->addChild(mXform);
   dynamic_cast<osg::Group*>(mNode.get())->addChild(group);
}

/** Make the box and load the textures */
osg::Node* dtCore::SkyBox::MakeBox(void)
{
   mGeode = new osg::Geode();

   float x = 1.f;
   float y = 1.f;
   float z = 1.f;
   osg::Vec3 coords0[] = //front
   {
      osg::Vec3(-x, y, -z),
         osg::Vec3(x, y,-z),
         osg::Vec3(x, y, +z),
         osg::Vec3(-x, y, +z),
   };

   osg::Vec3 coords1[] = //right
   {
         osg::Vec3(x, y, -z),
         osg::Vec3(x, -y, -z),
         osg::Vec3(x, -y, z),
         osg::Vec3(x, y, z)
   };

   osg::Vec3 coords2[] = //back
   {
      osg::Vec3(x, -y, -z),
         osg::Vec3(-x, -y, -z),
         osg::Vec3(-x, -y, z),
         osg::Vec3(x, -y, z)
   };

   osg::Vec3 coords3[] = //left
   {
      osg::Vec3(-x, -y, -z),
         osg::Vec3(-x, y, -z),
         osg::Vec3(-x, y, z),
         osg::Vec3(-x, -y, z)
   };

   osg::Vec3 coords4[] = //top
   {
      osg::Vec3(-x, y, z),
         osg::Vec3(x, y, z),
         osg::Vec3(x, -y, z),
         osg::Vec3(-x, -y, z)
   };
   osg::Vec3 coords5[] = //bottom
   {
      osg::Vec3(-x, y, -z),
         osg::Vec3(-x, -y, -z),
         osg::Vec3(x, -y, -z),
         osg::Vec3(x, y, -z)
   };

   osg::Vec2 tCoords[] =
   {
      osg::Vec2(0,0),
         osg::Vec2(1,0),
         osg::Vec2(1,1),
         osg::Vec2(0,1)
   };
   osg::Geometry *polyGeom[6];

   osg::Image *image[6];
   osg::Texture2D *texture[6];

   osg::Vec3Array *vArray[6];
   vArray[0] = new osg::Vec3Array(4, coords0);
   vArray[1] = new osg::Vec3Array(4, coords1);
   vArray[2] = new osg::Vec3Array(4, coords2);
   vArray[3] = new osg::Vec3Array(4, coords3);
   vArray[4] = new osg::Vec3Array(4, coords4);
   vArray[5] = new osg::Vec3Array(4, coords5);

   for (int side=0; side<6; side++)
   {
      polyGeom[side] = new osg::Geometry();

      polyGeom[side]->setVertexArray(vArray[side]);
      polyGeom[side]->setTexCoordArray(0, new osg::Vec2Array(4, tCoords));
      polyGeom[side]->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

      osg::StateSet *dstate = new osg::StateSet;
      dstate->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
      dstate->setMode( GL_CULL_FACE, osg::StateAttribute::ON );

      // clear the depth to the far plane.
      osg::Depth* depth = new osg::Depth;
      depth->setFunction(osg::Depth::ALWAYS);
      depth->setRange(1.0,1.0);   
      dstate->setAttributeAndModes(depth,osg::StateAttribute::ON );
      dstate->setMode(GL_FOG, osg::StateAttribute::OFF );
      dstate->setRenderBinDetails(-2,"RenderBin");

      //for wireframe rendering
      //   osg::PolygonMode *polymode = new osg::PolygonMode;
      //   polymode->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
      //   dstate->setAttributeAndModes(polymode, osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
      if (!mTextureFilenameMap[side].empty())
      {
         image[side] = osgDB::readImageFile(mTextureFilenameMap[side].c_str());

         if (image[side])
         {
            texture[side] = new osg::Texture2D;
            texture[side]->setImage(image[side]);
            texture[side]->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
            texture[side]->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
            dstate->setTextureAttributeAndModes(0, texture[side], osg::StateAttribute::ON);
         }
      }

      polyGeom[side]->setStateSet( dstate );
      mGeode->addDrawable(polyGeom[side]);
   }
   return mGeode.get();
}


/// Must override this to supply the repainting routine
void SkyBox::Repaint(sgVec3 skyColor, sgVec3 fogColor,
                     double sunAngle, double sunAzimuth,
                     double visibility)
{
   //need to recolor anything?
}

void SkyBox::OnMessage(MessageData *data)
{
   if (data->message == "configure")
   {
      Config();
   }
}

/** Pass in the filenames for the textures to be applied to the SkyBox.*/
void dtCore::SkyBox::SetTextureFilename(SkyBoxSideEnum side, std::string filename)
{
   mTextureFilenameMap[side] = filename;
}
