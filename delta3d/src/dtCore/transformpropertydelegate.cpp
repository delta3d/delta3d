/*
 * dofpropertydelegate.cpp
 *
 *  Created on: May 29, 2015
 *      Author: david
 */

#include <dtCore/transformpropertydelegate.h>
#include <osg/MatrixTransform>
#include <osgSim/DOFTransform>
#include <dtUtil/matrixutil.h>
#include <dtCore/transformableactorproxy.h>

namespace dtCore
{

   TransformPropertyDelegate::TransformPropertyDelegate(osg::Transform& transformNode)
   : mTransformNode(&transformNode)
   , mIsDOF(dynamic_cast<osgSim::DOFTransform*>(&transformNode) != nullptr)
   {
   }

   TransformPropertyDelegate::~TransformPropertyDelegate()
   {
   }

   dtCore::RefPtr<dtCore::Vec3ActorProperty> TransformPropertyDelegate::CreateTranslationProperty() const
   {
      dtUtil::RefString propName(GetTranslationPropertyName());
      return new dtCore::Vec3ActorProperty(propName, propName,
            dtCore::Vec3ActorProperty::SetFuncType(this, &TransformPropertyDelegate::SetTranslation),
            dtCore::Vec3ActorProperty::GetFuncType(this, &TransformPropertyDelegate::GetTranslation),
            "Transform subnode control property for rotation");
   }
   dtCore::RefPtr<dtCore::Vec3ActorProperty> TransformPropertyDelegate::CreateRotationProperty() const
   {
      dtUtil::RefString propName(GetRotationPropertyName());
      return new dtCore::Vec3ActorProperty(propName, propName,
            dtCore::Vec3ActorProperty::SetFuncType(this, &TransformPropertyDelegate::SetRotation),
            dtCore::Vec3ActorProperty::GetFuncType(this, &TransformPropertyDelegate::GetRotation),
            "Transform subnode control property for rotation.");
   }

   void TransformPropertyDelegate::AddProperties(dtCore::PropertyContainer& pc) const
   {
      pc.AddProperty(CreateTranslationProperty().get());
      pc.AddProperty(CreateRotationProperty().get());
   }

   void TransformPropertyDelegate::RemoveProperties(dtCore::PropertyContainer& pc) const
   {
      pc.RemoveProperty(GetTranslationPropertyName());
      pc.RemoveProperty(GetRotationPropertyName());
   }

   dtUtil::RefString TransformPropertyDelegate::GetTranslationPropertyName() const
   { return mTransformNode->getName() + dtCore::TransformableActorProxy::PROPERTY_TRANSLATION; }
   dtUtil::RefString TransformPropertyDelegate::GetRotationPropertyName() const
   { return mTransformNode->getName() + dtCore::TransformableActorProxy::PROPERTY_ROTATION; }

   osg::Vec3 TransformPropertyDelegate::GetTranslation() const
   {
       osg::Vec3 result;
       if (mIsDOF)
       {
          osgSim::DOFTransform* doft = static_cast<osgSim::DOFTransform*>(mTransformNode.get());
          result = doft->getCurrentTranslate();
       }
       else if (osg::MatrixTransform* mt = mTransformNode->asMatrixTransform())
       {
          result = mt->getMatrix().getTrans();
       }
       return result;
   }

   void TransformPropertyDelegate::SetTranslation(const osg::Vec3& translation) const
   {
      if (mIsDOF)
      {
         osgSim::DOFTransform* doft = static_cast<osgSim::DOFTransform*>(mTransformNode.get());
         doft->setCurrentTranslate(translation);
      }
      else if (osg::MatrixTransform* mt = mTransformNode->asMatrixTransform())
      {
         osg::Matrix m = mt->getMatrix();
         m.setTrans(translation);
         mt->setMatrix(m);
      }
   }

   osg::Vec3 TransformPropertyDelegate::GetRotation() const
   {
       osg::Vec3 result;
       if (mIsDOF)
       {
          osgSim::DOFTransform* doft = static_cast<osgSim::DOFTransform*>(mTransformNode.get());
          result = doft->getCurrentHPR();
          for (unsigned i = 0; i < 3; ++i)
          {
             result[i] = osg::RadiansToDegrees(result[i]);
          }
       }
       else if (osg::MatrixTransform* mt = mTransformNode->asMatrixTransform())
       {
          dtUtil::MatrixUtil::MatrixToHpr(result, mt->getMatrix());
       }
       return result;
   }

   void TransformPropertyDelegate::SetRotation(const osg::Vec3& hpr) const
   {
      if (mIsDOF)
      {
         osgSim::DOFTransform* doft = static_cast<osgSim::DOFTransform*>(mTransformNode.get());
         osg::Vec3 hprRad = hpr;
         for (unsigned i = 0; i < 3; ++i)
         {
            hprRad[i] = osg::DegreesToRadians(hprRad[i]);
         }
         doft->setCurrentHPR(hprRad);
      }
      else if (osg::MatrixTransform* mt = mTransformNode->asMatrixTransform())
      {
         osg::Matrix mat = mt->getMatrix();
         dtUtil::MatrixUtil::HprToMatrix(mat, hpr, false);
         mt->setMatrix(mat);
         return;
      }
   }


} /* namespace dtCore */
