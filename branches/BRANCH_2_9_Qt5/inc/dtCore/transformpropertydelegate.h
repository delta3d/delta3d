/*
 * dofpropertydelegate.h
 *
 *  Created on: May 29, 2015
 *      Author: david
 */

#ifndef DELTA_TRANSFORMPROPERTYDELEGATE_H_
#define DELTA_TRANSFORMPROPERTYDELEGATE_H_

#include <dtCore/export.h>
#include <dtCore/observerptr.h>
#include <dtCore/vectoractorproperties.h>
#include <dtCore/propertycontainer.h>
#include <dtUtil/refstring.h>
#include <osg/Referenced>
#include <osg/Transform>

namespace dtCore
{

   class DT_CORE_EXPORT TransformPropertyDelegate : public osg::Referenced
   {
   public:
      TransformPropertyDelegate(osg::Transform& transformNode);

      osg::Transform* GetTransformNode() { return mTransformNode.get(); }

      osg::Vec3 GetTranslation() const;

      void SetTranslation(const osg::Vec3& translation) const;

      osg::Vec3 GetRotation() const;

      void SetRotation(const osg::Vec3& hpr) const;

      dtCore::RefPtr<dtCore::Vec3ActorProperty> CreateTranslationProperty(bool partialUpdateFlag = false) const;
      dtCore::RefPtr<dtCore::Vec3ActorProperty> CreateRotationProperty(bool partialUpdateFlag = false) const;
      dtUtil::RefString GetTranslationPropertyName() const;
      dtUtil::RefString GetRotationPropertyName() const;

      virtual void AddProperties(dtCore::PropertyContainer& pc, bool partialUpdateFlag = false) const;
      virtual void RemoveProperties(dtCore::PropertyContainer& pc) const;

   protected:
      virtual ~TransformPropertyDelegate();
   private:

      dtCore::ObserverPtr<osg::Transform> mTransformNode;
      // This is to avoid the dynamic cast
      bool mIsDOF;
   };

   typedef dtCore::RefPtr<TransformPropertyDelegate> TransformPropertyDelegatePtr;

} /* namespace dtCore */

#endif /* DELTA_TRANSFORMPROPERTYDELEGATE_H_ */
