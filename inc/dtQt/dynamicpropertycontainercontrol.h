/* -*-c++-*-
 * Delta3D
 * Copyright 2010, Alion Science and Technology
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#ifndef DYNAMICPROPERTYCONTAINERCONTROL_H_
#define DYNAMICPROPERTYCONTAINERCONTROL_H_


#include <dtQt/dynamicgroupcontrol.h>

#include <dtCore/propertycontaineractorproperty.h>


namespace dtQt
{

   /**
    * @class DynamicLongControl
    * @brief This is the dynamic control for a nested property container property.
    */
   class DT_QT_EXPORT DynamicPropertyContainerControl : public DynamicGroupControl
   {
      Q_OBJECT;
   public:
      DynamicPropertyContainerControl();

      virtual ~DynamicPropertyContainerControl();

      /**
       * @see DynamicAbstractControl#InitializeData
       */
      void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtCore::PropertyContainer* newPC, dtCore::ActorProperty* property) override;

      void InitWithPropertyContainers(const dtCore::PropContRefPtrVector& propContainers, PropertyEditorModel* newModel);

      /**
       * @see DynamicAbstractControl#getDisplayName
       */
      virtual const QString getDisplayName() override;

      /**
       * @see DynamicAbstractControl#getDescription
       */
      virtual const QString getDescription() override;

      /**
       * @see DynamicAbstractControl#getValueAsString
       */
      virtual const QString getValueAsString() override;


   public slots:

      /**
       * Signal when the data is updated.
       */
      virtual bool updateData(QWidget* widget);

      ////////////////////////////////////////////////////////////////////////////////
      void PropertyAboutToChangePassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop,
               std::string oldValue, std::string newValue) override;

      ////////////////////////////////////////////////////////////////////////////////
      void PropertyChangedPassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop) override;

   private:

      dtCore::RefPtr<dtCore::BasePropertyContainerActorProperty> mProperty;
   };
}
#endif /* DYNAMICPROPERTYCONTAINERCONTROL_H_ */
