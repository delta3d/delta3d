#include <dtActors/backdropactor.h>

#include <dtCore/resourceactorproperty.h>
#include <dtCore/vectoractorproperties.h>

#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/Vec3>

#include <osgDB/ReadFile>

namespace dtActors
{

////////////////////////////////////////////////////////////////////////////////
BackdropActor::BackdropActor(const std::string& name /* = "Backdrop Actor" */)
   : dtCore::Transformable(name)
   , mFrontQuad(new osg::Geode())
   , mBackQuad(new osg::Geode())
   , mModel(new dtCore::Model())
{
   this->SetName("Backdrop");

   this->GetOSGNode()->asGroup()->addChild(&mModel->GetMatrixTransform());

   mModel->GetMatrixTransform().asGroup()->addChild(mFrontQuad);
   mModel->GetMatrixTransform().asGroup()->addChild(mBackQuad);

   CreateBackdropGeometry();
}

////////////////////////////////////////////////////////////////////////////////
BackdropActor::~BackdropActor()
{
}

osg::Vec3 BackdropActor::GetScale() const
{
   osg::Vec3 scale;
   mModel->GetScale(scale);
   return scale;
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActor::SetScale(const osg::Vec3& xyz)
{
   mModel->SetScale(xyz);
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActor::SetFrontTexture(const std::string& path)
{
   dtCore::RefPtr<osg::Texture2D> txtr = LoadTexture(path);

   osg::StateSet* ss = mFrontQuad->getOrCreateStateSet();

   if (! txtr)
   {   
      ss->removeTextureAttribute(0, osg::StateAttribute::TEXTURE);
      return;
   }
   
   ss->setTextureAttributeAndModes(0, txtr.get(), osg::StateAttribute::ON);    
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActor::SetBackTexture(const std::string& path)
{
   dtCore::RefPtr<osg::Texture2D> txtr = LoadTexture(path);
   osg::StateSet* ss = mBackQuad->getOrCreateStateSet();

   if (! txtr)
   {
      ss->removeTextureAttribute(0, osg::StateAttribute::TEXTURE);
      return;
   }
   
   ss->setTextureAttributeAndModes(0, txtr.get(), osg::StateAttribute::ON);    
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActor::CreateBackdropGeometry()
{   
   osg::Geometry* quadGeometry = new osg::Geometry();
   osg::Vec3Array* quadVertexArray = new osg::Vec3Array();
   osg::Vec2Array* quadTexCoords = new osg::Vec2Array(4);
   osg::DrawElementsUInt* quadPrimSet = 
      new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);

   osg::StateSet* frontSS = mFrontQuad->getOrCreateStateSet();
   osg::PolygonMode* frontPolygonMode = new osg::PolygonMode(osg::PolygonMode::FRONT,
      osg::PolygonMode::FILL);   

   osg::StateSet* backSS = mBackQuad->getOrCreateStateSet();
   osg::PolygonMode* backPolygonMode = new osg::PolygonMode(osg::PolygonMode::BACK,
                                                        osg::PolygonMode::FILL);   

   quadVertexArray->push_back(osg::Vec3(-10.0f, 0.0f, 10.0f));
   quadVertexArray->push_back(osg::Vec3(-10.0f, 0.0f, -10.0f));
   quadVertexArray->push_back(osg::Vec3(10.0f, 0.0f, -10.0f));
   quadVertexArray->push_back(osg::Vec3(10.0f, 0.0f, 10.0f));
   
   quadPrimSet->push_back(0);
   quadPrimSet->push_back(1);
   quadPrimSet->push_back(2);
   quadPrimSet->push_back(3);

   (*quadTexCoords)[0].set(0.0f, 1.0f);
   (*quadTexCoords)[1].set(0.0f, 0.0f);
   (*quadTexCoords)[2].set(1.0f, 0.0f);
   (*quadTexCoords)[3].set(1.0f, 1.0f);

   mFrontQuad->addDrawable(quadGeometry);
   mBackQuad->addDrawable(quadGeometry);
   
   quadGeometry->setUseDisplayList(false);
   quadGeometry->setVertexArray(quadVertexArray);
   quadGeometry->setTexCoordArray(0, quadTexCoords);
   quadGeometry->addPrimitiveSet(quadPrimSet);

   frontSS->setAttributeAndModes(frontPolygonMode, 
      osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

   backSS->setAttributeAndModes(backPolygonMode,
      osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
   backSS->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);   
}

////////////////////////////////////////////////////////////////////////////////
dtCore::RefPtr<osg::Texture2D> BackdropActor::LoadTexture(const std::string& path)
{
   osg::Image *im = NULL;

   if (path != "")
   {
      im = osgDB::readImageFile(path);
   }
   else
   {
      return NULL;
   }

   if (!im)
   {      
      LOG_ERROR("Couldn't find texture.");
      return NULL;
   }

   osg::Texture2D* txtr = new osg::Texture2D();
   txtr->setDataVariance(osg::Object::DYNAMIC);
   txtr->setImage(im);

   return txtr;
}

////////////////////////////////////////////////////////////////////////////////
// PROXY Methods follow:
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
BackdropActorProxy::BackdropActorProxy()
   : TransformableActorProxy()
{   
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActorProxy::BuildPropertyMap()   
{
   dtCore::TransformableActorProxy::BuildPropertyMap();

   static const dtUtil::RefString GROUPNAME = "Backdrop";

   // Make sure our actor is valid
   BackdropActor *actor = dynamic_cast<BackdropActor*> (GetDrawable());
   if(!actor)
   {
      LOG_ERROR("BackdropActor was initialized incorrectly");
      return;
   }

   //Volume actors need to be scalable
   AddProperty(new dtCore::Vec3ActorProperty("Scale", "Scale",
            dtCore::Vec3ActorProperty::SetFuncType(actor, &BackdropActor::SetScale),
            dtCore::Vec3ActorProperty::GetFuncType(actor, &BackdropActor::GetScale),
            "Scales", "Transformable"));

   AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
            "Front Texture", "Front Texture",
            dtCore::ResourceActorProperty::SetFuncType(this, &BackdropActorProxy::SetFrontTexture),
            "Sets the texture on the front of the backdrop", GROUPNAME));

   AddProperty(new dtCore::ResourceActorProperty(dtCore::DataType::TEXTURE,
            "Back Texture", "Back Texture",
            dtCore::ResourceActorProperty::SetFuncType(this, &BackdropActorProxy::SetBackTexture),
            "Sets the texture on the back of the backdrop", GROUPNAME));
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActorProxy::SetFrontTexture(const std::string &path)
{
   dtActors::BackdropActor *backdropA =
      dynamic_cast<dtActors::BackdropActor *>(GetDrawable());

   if (backdropA == NULL)
   {
      LOG_ERROR("Expected a BackdropActor.");
      return;
   }

   backdropA->SetFrontTexture(path);
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActorProxy::SetBackTexture(const std::string &path)
{
   dtActors::BackdropActor *backdropA =
      dynamic_cast<dtActors::BackdropActor *>(GetDrawable());

   if (backdropA == NULL)
   {
      LOG_ERROR("Expected a BackdropActor.");
      return;
   }

   backdropA->SetBackTexture(path);
}

////////////////////////////////////////////////////////////////////////////////
void BackdropActorProxy::CreateDrawable()
{
   BackdropActor* actor = new BackdropActor();
   SetDrawable(*actor);   
}

}//dtActors
