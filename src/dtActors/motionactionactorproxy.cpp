#include <dtActors/motionactionactorproxy.h>
#include <dtABC/motionaction.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>

using namespace dtCore;
using namespace dtABC;
using namespace dtDAL;

namespace dtActors
{

   IMPLEMENT_ENUM(MotionActionActorProxy::ParentRelationEnum);

   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::NO_RELATION("NO_RELATION");
   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::TRACK_PARENT("TRACK_PARENT");
   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::FOLLOW_PARENT("FOLLOW_PARENT");
   MotionActionActorProxy::ParentRelationEnum MotionActionActorProxy::ParentRelationEnum::TRACK_AND_FOLLOW("TRACK_AND_FOLLOW");

   bool MotionActionActorProxy::IsPlaceable()
   {
      return false;
   }

   void MotionActionActorProxy::BuildPropertyMap()
   {
      ActionActorProxy::BuildPropertyMap();

      //MotionAction* mo = static_cast<MotionAction*>(GetActor());

      AddProperty(new ActorActorProperty(*this, "Target Object", "Target Object",
         MakeFunctor(*this, &MotionActionActorProxy::SetActorTargetObject),
         MakeFunctorRet(*this, &MotionActionActorProxy::GetActorTargetObject),
         "dtCore::Transformable",
         "Sets the object to be moved when this action is triggered"));

      AddProperty(new dtDAL::ActorActorProperty(*this, "Parent Object", "Parent Object",
         MakeFunctor(*this, &MotionActionActorProxy::SetActorParentObject),
         MakeFunctorRet(*this, &MotionActionActorProxy::GetActorParentObject),
         "dtCore::Transformable",
         "Sets the parent for this Actor."));

      AddProperty(new dtDAL::EnumActorProperty<MotionActionActorProxy::ParentRelationEnum>("Parent Relation","Parent Relation",
         dtDAL::MakeFunctor(*this,&MotionActionActorProxy::SetActorParentRelation),
         dtDAL::MakeFunctorRet(*this,&MotionActionActorProxy::GetActorParentRelation),
         "Sets this actors relation to its parent.", "MotionAction"));


      AddProperty(new Vec3ActorProperty("Rotation", "Rotation",
         MakeFunctor(*this, &MotionActionActorProxy::SetRotation),
         MakeFunctorRet(*this, &MotionActionActorProxy::GetRotation),
         "Sets the amount of rotation on a transformable. Represented with pitch, yaw, and roll.",
         "LocalTransform"));

      AddProperty(new Vec3ActorProperty("Translation", "Translation",
         MakeFunctor(*this, &MotionActionActorProxy::SetTranslation),
         MakeFunctorRet(*this, &MotionActionActorProxy::GetTranslation),
         "Sets the location of a transformable in 3D space.",
         "LocalTransform"));

      AddProperty(new Vec3ActorProperty("Scale", "Scale",
         MakeFunctor(*this, &MotionActionActorProxy::SetScale),
         MakeFunctorRet(*this, &MotionActionActorProxy::GetScale),
         "Sets the scale of a transformable.","LocalTransform"));

   }

   DeltaDrawable* MotionActionActorProxy::GetActorTargetObject()
   {
      MotionAction* mo = static_cast< MotionAction* >( GetActor() );
      if( mo == 0 )
      {
         throw dtUtil::Exception( ExceptionEnum::InvalidActorException,
            "Actor should be type dtABC::MotionAction", __FILE__, __LINE__);
      }

      return mo->GetTargetObject();
   }

   DeltaDrawable* MotionActionActorProxy::GetActorParentObject()
   {
      MotionAction * mo = static_cast< MotionAction* >( GetActor() );
      if( mo == 0 )
      {
         throw dtUtil::Exception( ExceptionEnum::InvalidActorException,
            "Actor should be type dtABC::MotionAction" , __FILE__, __LINE__);
      }

      return mo->GetParent();
   }

   void MotionActionActorProxy::SetActorTargetObject(ActorProxy* node)
   {
      SetLinkedActor("Target Object", node);

      dtABC::MotionAction* ma = static_cast<dtABC::MotionAction*>(GetActor());

      dtCore::Transformable* trans  = NULL;

      if(node)
      {
          trans = static_cast<dtCore::Transformable*>(node->GetActor());
      }
      
      ma->SetTargetObject(trans);      
   }

   void MotionActionActorProxy::SetActorParentObject(ActorProxy* proxy)
   {
      SetLinkedActor("Parent Object", proxy);

      dtABC::MotionAction* ma = static_cast<dtABC::MotionAction*>(GetActor());

      dtCore::Transformable* trans  = NULL;

      if(proxy)
      {
         trans = static_cast<dtCore::Transformable*>(proxy->GetActor());
      }

      ma->SetParent(trans);
   }

   MotionActionActorProxy::ParentRelationEnum& MotionActionActorProxy::GetActorParentRelation() 
   {
      dtABC::MotionAction* ma = static_cast<dtABC::MotionAction*>(GetActor());

      MotionAction::PARENT_RELATION pr = ma->GetParentRelation();

      if(pr == dtABC::MotionAction::FOLLOW_PARENT)
         return ParentRelationEnum::FOLLOW_PARENT;

      else if(pr == dtABC::MotionAction::TRACK_PARENT)
         return ParentRelationEnum::TRACK_PARENT;

      else if(pr == dtABC::MotionAction::TRACK_AND_FOLLOW)
         return ParentRelationEnum::TRACK_AND_FOLLOW;

      return ParentRelationEnum::NO_RELATION;
   }

   void MotionActionActorProxy::SetActorParentRelation(ParentRelationEnum& e)
   {
      dtABC::MotionAction* ma = static_cast<dtABC::MotionAction*>(GetActor());
      if(e == ParentRelationEnum::FOLLOW_PARENT)
         ma->SetParentRelation(dtABC::MotionAction::FOLLOW_PARENT);

      else if(e == ParentRelationEnum::TRACK_PARENT)
         ma->SetParentRelation(dtABC::MotionAction::TRACK_PARENT);
         
      else if(e == ParentRelationEnum::TRACK_AND_FOLLOW)
         ma->SetParentRelation(dtABC::MotionAction::TRACK_AND_FOLLOW);

      else ma->SetParentRelation(dtABC::MotionAction::NO_RELATION);
   }




   ///////////////////////////////////////////////////////////////////////////////
   void MotionActionActorProxy::SetRotation(const osg::Vec3 &rotation)
   {
      dtABC::MotionAction *t = static_cast<dtABC::MotionAction*>(GetActor());

      osg::Vec3 hpr = rotation;

      //Normalize the rotation.
      if (hpr.x() < 0.0f)
         hpr.x() += 360.0f;
      if (hpr.x() > 360.0f)
         hpr.x() -= 360.0f;

      if (hpr.y() < 0.0f)
         hpr.y() += 360.0f;
      if (hpr.y() > 360.0f)
         hpr.y() -= 360.0f;

      if (hpr.z() < 0.0f)
         hpr.z() += 360.0f;
      if (hpr.z() > 360.0f)
         hpr.z() -= 360.0f;

      dtCore::Transform trans = t->GetLocalTransform();
      
      trans.SetRotation(hpr);
      t->SetLocalTransform(trans);
   }


   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 MotionActionActorProxy::GetRotation()
   {
      dtABC::MotionAction *t = static_cast<dtABC::MotionAction*>(GetActor());

      osg::Vec3 hpr;
      t->GetLocalTransform().GetRotation(hpr);

      return hpr;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MotionActionActorProxy::SetTranslation(const osg::Vec3 &translation)
   {
      dtABC::MotionAction *t = static_cast<dtABC::MotionAction*>(GetActor());

      dtCore::Transform trans = t->GetLocalTransform();
     
      osg::Vec3 oldTrans;
      trans.GetTranslation(oldTrans);
      trans.SetTranslation(translation[0], translation[1], translation[2]);
      t->SetLocalTransform(trans);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 MotionActionActorProxy::GetTranslation()
   {
      dtABC::MotionAction *t = static_cast<dtABC::MotionAction*>(GetActor());
      if (t == NULL)
         throw dtUtil::Exception(dtDAL::ExceptionEnum::InvalidActorException, "Actor should be type "
         " dtABC::MotionAction", __FILE__, __LINE__);

      osg::Vec3 trans;
      t->GetLocalTransform().GetTranslation(trans);
      return trans;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void MotionActionActorProxy::SetScale(const osg::Vec3 &scale)
   {
      dtABC::MotionAction *t = static_cast<dtABC::MotionAction*>(GetActor());

      dtCore::Transform trans = t->GetLocalTransform();
      
      osg::Vec3 oldScale;
      trans.GetScale(oldScale);
      trans.SetScale(scale);
      t->SetLocalTransform(trans);

   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 MotionActionActorProxy::GetScale()
   {
      dtABC::MotionAction *t = static_cast<dtABC::MotionAction*>(GetActor());

      osg::Vec3 scale;
      t->GetLocalTransform().GetScale(scale);
      return scale;
   }




}//namespace dtActors


