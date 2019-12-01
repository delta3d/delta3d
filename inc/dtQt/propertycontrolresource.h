/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef DELTA_PROPERTY_CONTROL_RESOURCE_H
#define DELTA_PROPERTY_CONTROL_RESOURCE_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtCore/resourceactorproperty.h>
#include <dtCore/resourcedescriptor.h>
#include <dtQt/basepropertycontrol.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace Ui
{
   class PropertyControlResource;
}



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class DT_QT_EXPORT PropertyControlResource : public BasePropertyControl
   {
      Q_OBJECT
   public:
      typedef BasePropertyControl BaseClass;

      typedef dtCore::ResourceActorProperty ResourceProperty;

      PropertyControlResource(QWidget* parent, dtCore::DataType& dataType);

      virtual ~PropertyControlResource();

      /*virtual*/ QObject* GetControlUI();

      /*virtual*/ QLabel* GetLabel() const;
      
      void SetResource(const dtCore::ResourceDescriptor& res);
      dtCore::ResourceDescriptor GetResource() const;

      /**
       * Update the UI values from the referenced data.
       */
      /*virtual*/ void UpdateUI(const PropertyType& prop);

      /**
       * Update the referenced data with values from the UI.
       */
      /*virtual*/ void UpdateData(PropertyType& propToUpdate);

      ResourceProperty* GetResourceProperty() const;

   public slots:
      virtual void OnButtonClicked();

   protected:
      /*virtual*/ void CreateConnections();

      Ui::PropertyControlResource* mUI;
   };


   /////////////////////////////////////////////////////////////////////////////
   // MACRO CODE
   /////////////////////////////////////////////////////////////////////////////
#ifndef DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS
#define DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(subclassName, datatype) \
   class DT_QT_EXPORT PropertyControl ## subclassName : public PropertyControlResource\
   {\
   public:\
      typedef PropertyControlResource BaseClass;\
\
      PropertyControl ## subclassName(QWidget* parent = NULL)\
         : BaseClass(parent, datatype)\
      {}\
\
   protected:\
      virtual ~PropertyControl ## subclassName()\
      {}\
   };\

#endif



   /////////////////////////////////////////////////////////////////////////////
   // SUBCLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(StaticMesh, dtCore::DataType::STATIC_MESH)
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(SkeletalMesh, dtCore::DataType::SKELETAL_MESH)
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(Terrain, dtCore::DataType::TERRAIN)
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(Sound, dtCore::DataType::SOUND)
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(ParticleSystem, dtCore::DataType::PARTICLE_SYSTEM)
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(Prefab, dtCore::DataType::PREFAB)
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(Shader, dtCore::DataType::SHADER)
   DECLARE_PROPERTY_CONTROL_RESOURCE_SUBCLASS(Director, dtCore::DataType::DIRECTOR)
}

#endif
