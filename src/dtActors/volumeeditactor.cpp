#include <dtActors/volumeeditactor.h>
#include <dtCore/model.h>
#include <dtCore/transform.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>

#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/Shape>
#include <osg/ShapeDrawable>

namespace dtActors
{
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_ENUM(VolumeEditActor::VolumeShapeType)
VolumeEditActor::VolumeShapeType VolumeEditActor::VolumeShapeType::BOX("BOX");
VolumeEditActor::VolumeShapeType VolumeEditActor::VolumeShapeType::SPHERE("SPHERE");
VolumeEditActor::VolumeShapeType VolumeEditActor::VolumeShapeType::CYLINDER("CYLINDER");
VolumeEditActor::VolumeShapeType VolumeEditActor::VolumeShapeType::CAPSULE("CAPSULE");
VolumeEditActor::VolumeShapeType VolumeEditActor::VolumeShapeType::CONE("CONE");

////////////////////////////////////////////////////////////////////////////////
VolumeEditActor::VolumeEditActor()
   : dtCore::Transformable("VolumeEditActor")
   , mVolumeGroup(new osg::Group())
   , mShaderGroup(new osg::Group())
   , mVolumeGeode(new osg::Geode())
   , mModel(new dtCore::Model())
   , mBaseRadius(10.0)
   , mBaseLength(10.0)
{
   //For volume to get added to scene, make the Geode a child of the
   //Transformable's OSGNode (Transformable Actor is already in scene)
   osg::Group *g = this->GetOSGNode()->asGroup();
   g->addChild(&mModel->GetMatrixTransform());   

   g = mModel->GetMatrixTransform().asGroup();
   g->addChild(mVolumeGroup.get());

   mVolumeGroup->addChild(mVolumeGeode.get());   

   //setup wireframe outline
   SetupWireOutline();
   mVolumeGroup->addChild(mShaderGroup.get());
   mShaderGroup->addChild(mVolumeGeode.get());

   //default shape is box
   SetShape(VolumeShapeType::BOX);
}

////////////////////////////////////////////////////////////////////////////////
VolumeEditActor::~VolumeEditActor()
{
}

////////////////////////////////////////////////////////////////////////////////
double VolumeEditActor::GetBaseLength()
{
   return mBaseLength;
}

////////////////////////////////////////////////////////////////////////////////
double VolumeEditActor::GetBaseRadius()
{
   return mBaseRadius;
}

////////////////////////////////////////////////////////////////////////////////
osg::Vec3 VolumeEditActor::GetScale() const
{
   osg::Vec3 scale;
   mModel->GetScale(scale);
   return scale;
}

////////////////////////////////////////////////////////////////////////////////
VolumeEditActor::VolumeShapeType& VolumeEditActor::GetShape()
{
   if (mVolumeShape == NULL)
   {
      //Defaults back to BOX
      return VolumeShapeType::BOX;
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
         __FUNCTION__, __LINE__, "No Shape Set");
   }

   if (strcmp(mVolumeShape->className(), "Box") == 0)
   {
      return VolumeShapeType::BOX;
   }

   if (strcmp(mVolumeShape->className(), "Sphere") == 0)
   {
      return VolumeShapeType::SPHERE;
   }

   if (strcmp(mVolumeShape->className(), "Cylinder") == 0)
   {
      return VolumeShapeType::CYLINDER;
   }

   if (strcmp(mVolumeShape->className(), "Capsule") == 0)
   {
      return VolumeShapeType::CAPSULE;
   }

   if (strcmp(mVolumeShape->className(), "Cone") == 0)
   {
      return VolumeShapeType::CONE;
   }

   dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
      __FUNCTION__, __LINE__, "No Shape Set");
   //defaults back to box
   return VolumeShapeType::BOX;
}

////////////////////////////////////////////////////////////////////////////////
void VolumeEditActor::SetScale(const osg::Vec3& xyz)
{
   mModel->SetScale(xyz);
}

////////////////////////////////////////////////////////////////////////////////
void VolumeEditActor::SetShape(VolumeShapeType& shape)
{
   if (shape == VolumeShapeType::BOX)
   {
      mVolumeShape = new osg::Box(osg::Vec3(0.0, 0.0, 0.0), mBaseLength, 
                                                            mBaseLength,
                                                            mBaseLength);
   }
   else if (shape == VolumeShapeType::SPHERE)
   {
      mVolumeShape = new osg::Sphere(osg::Vec3(0.0, 0.0, 0.0), mBaseRadius);
   }
   else if (shape == VolumeShapeType::CYLINDER)
   {
      mVolumeShape = new osg::Cylinder(osg::Vec3(0.0, 0.0, 0.0), mBaseRadius,
                                                                 mBaseLength);
   }
   else if (shape == VolumeShapeType::CAPSULE)
   {
      mVolumeShape = new osg::Capsule(osg::Vec3(0.0, 0.0, 0.0), mBaseRadius, 
                                                                mBaseLength);
   }
   else if (shape == VolumeShapeType::CONE)
   {
      mVolumeShape = new osg::Cone(osg::Vec3(0.0, 0.0, 0.0), mBaseRadius,
                                                             mBaseLength);
   }
   else
   {
      dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_ERROR,
            __FUNCTION__, __LINE__, "Unrecognized Shape");
      return;
   }

   dtCore::RefPtr<osg::TessellationHints> tessHints = new osg::TessellationHints();

   mVolumeDrawable = new osg::ShapeDrawable(mVolumeShape.get(), tessHints.get());   
   mVolumeDrawable->setColor(osg::Vec4(0.3f, 0.3f, 0.3f, 0.7f));

   //Volume should only ever contain one shape drawable,
   //so remove the old one, if there is one
   mVolumeGeode->removeDrawables(0, 1);
   mVolumeGeode->addDrawable(mVolumeDrawable.get());

   osg::StateSet* stateSet = mVolumeGeode->getOrCreateStateSet();   
   if (stateSet)
   {
      stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);

      //stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

      osg::PolygonMode* polygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT, osg::PolygonMode::FILL);
      stateSet->setAttribute(polygonMode, osg::StateAttribute::OVERRIDE);
    
      osg::BlendFunc* blend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
      stateSet->setAttribute(blend, osg::StateAttribute::ON);
      
      stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
   }
}

void VolumeEditActor::EnableOutline(bool doEnable)
{
   if (doEnable)
   {
      SetupWireOutline();
   }
   else
   {
      mShaderGroup->setStateSet(NULL);
   }
}

////////////////////////////////////////////////////////////////////////////////
void VolumeEditActor::SetupWireOutline()
{
   osg::StateSet* ss = mShaderGroup->getOrCreateStateSet();

   osg::StateAttribute::GLModeValue turnOn =
      osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON;

   dtCore::RefPtr<osg::Program> program = new osg::Program();
   dtCore::RefPtr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);

   fragShader->setShaderSource("void main (void) { gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0); }");
   program->addShader(fragShader.get());   

   ss->setAttributeAndModes(program.get(), turnOn); 
   ss->setRenderBinDetails(80, "RenderBin");
 
   //Create the required state attributes for wireframe overlay selection.
   osg::PolygonOffset* po = new osg::PolygonOffset;
   osg::PolygonMode* pm = new osg::PolygonMode();

   pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
   po->setFactor(-1.0f);

   po->setUnits(-1.0f);
   ss->setAttributeAndModes(pm, turnOn);
   ss->setAttributeAndModes(po, turnOn);

   ss->setMode(GL_BLEND,osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);

   ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
}

////////////////////////////////////////////////////////////////////////////////
VolumeEditActorProxy::VolumeEditActorProxy()
   : dtDAL::TransformableActorProxy()
{
}

////////////////////////////////////////////////////////////////////////////////
VolumeEditActorProxy::~VolumeEditActorProxy()
{
}

////////////////////////////////////////////////////////////////////////////////
void VolumeEditActorProxy::CreateActor()
{
   //defaults to box, but the properties should allow a switch to other shapes
   SetActor(*new dtActors::VolumeEditActor());
}

////////////////////////////////////////////////////////////////////////////////
void VolumeEditActorProxy::BuildPropertyMap()
{
   //The property group
   const std::string groupName = "Volume Editor";

   // Make sure to build the base class properties
   TransformableActorProxy::BuildPropertyMap();

   // Make sure our actor is valid
   VolumeEditActor *actor = dynamic_cast<VolumeEditActor*> (GetActor());
   if(!actor)
   {
      printf("Actor was initialized incorrectly\n");
      return;
   }

   AddProperty(new dtDAL::EnumActorProperty<VolumeEditActor::VolumeShapeType>(
      "Shape", "Shape",
      dtDAL::EnumActorProperty<VolumeEditActor::VolumeShapeType>::SetFuncType(this, &VolumeEditActorProxy::SetShape),
      dtDAL::EnumActorProperty<VolumeEditActor::VolumeShapeType>::GetFuncType(this, &VolumeEditActorProxy::GetShape),
      "Sets the shape for this Volume Editor", groupName));

   //Volume actors need to be scalable
   AddProperty(new dtDAL::Vec3ActorProperty("Scale", "Scale",
      dtDAL::MakeFunctor(*actor, &VolumeEditActor::SetScale),
      dtDAL::MakeFunctorRet(*actor, &VolumeEditActor::GetScale),
      "Scales", "Transformable"));
}

////////////////////////////////////////////////////////////////////////////////
VolumeEditActor::VolumeShapeType& VolumeEditActorProxy::GetShape()
{
   return dynamic_cast<VolumeEditActor*>(GetActor())->GetShape();
}

////////////////////////////////////////////////////////////////////////////////
void VolumeEditActorProxy::SetShape(VolumeEditActor::VolumeShapeType& shape)
{
   dynamic_cast<VolumeEditActor*>(GetActor())->SetShape(shape);
}

} //end namespace dtActors
