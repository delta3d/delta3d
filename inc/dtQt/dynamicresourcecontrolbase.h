/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicresourcecontrol (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Curtiss Murphy
 */
#ifndef DELTA_DYNAMICRESOURCECONTROLBASE
#define DELTA_DYNAMICRESOURCECONTROLBASE

#include <dtQt/export.h>
#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/dynamiclabelcontrol.h>

namespace dtDAL
{
   class ResourceActorProperty;
   class ResourceDescriptor;
}

class QLabel;

namespace dtQt
{

   class SubQLabel;
   class SubQPushButton;

   /**
    * @class DynamicResourceControlBase
    * @brief This is the resource actor property.  It knows how to work with the various
    * resource data types (Terrain, Character, Mesh, Texture, sound, ...) from DataTypes.h
    * This control is not editable, but has several child controls and some of them
    * are editable.
    */
   class DT_QT_EXPORT DynamicResourceControlBase : public DynamicAbstractParentControl
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      DynamicResourceControlBase();

      /**
       * Destructor
       */
      virtual ~DynamicResourceControlBase();

      /**
       * @see DynamicAbstractControl#InitializeData
       */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* property);

      /**
       * @see DynamicAbstractControl#getDisplayName
       */
      virtual const QString getDisplayName();

      /**
       * @see DynamicAbstractControl#getDescription
       */
      virtual const QString getDescription();

      /**
       * @see DynamicAbstractControl#getValueAsString
       */
      virtual const QString getValueAsString();

      /**
       * @see DynamicAbstractControl#updateEditorFromModel
       */
      virtual void updateEditorFromModel(QWidget* widget);

      /**
       * @see DynamicAbstractControl#updateModelFromEditor
       */
      virtual bool updateModelFromEditor(QWidget* widget);

      /**
       * @see DynamicAbstractControl#createEditor
       */
      virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
         const QModelIndex& index);

      /**
       * @see DynamicAbstractControl#isEditable
       */
      virtual bool isEditable();

      /**
       * @see DynamicAbstractControl#installEventFilterOnControl
       */
      virtual void installEventFilterOnControl(QObject* filterObj);

      dtDAL::ResourceActorProperty& GetProperty();

   public slots:
      virtual bool updateData(QWidget* widget);

      /**
       * The user pressed the 'Use Current' Button.  Grab the current resource.
       */
      void useCurrentPressed();

      /**
       * The user pressed the 'Clear' Button.  Clear out the resource.
       */
      void clearPressed();

      void actorPropertyChanged(dtDAL::PropertyContainer& propCon,
               dtDAL::ActorProperty& property);

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

   protected:
      /**
       * Figure out which resource descriptor  to get from EditorData and get it.
       * @return the current resource descriptor for our type, else an empty one of if type is invalid.
       */
      virtual dtDAL::ResourceDescriptor getCurrentResource() = 0;

   private:
      dtDAL::ResourceActorProperty* mProperty;

      // This pointer is not really in our control.  It is constructed in the createEditor()
      // method and destroyed whenever QT feels like it (mostly when the control looses focus).
      // We work around this by trapping the destruction of this object, it should
      // call our handleSubEditDestroy() method so we know to not hold this anymore
      QWidget*        mTemporaryWrapper;
      SubQLabel*      mTemporaryEditOnlyTextLabel;
      SubQPushButton* mTemporaryUseCurrentBtn;
      SubQPushButton* mTemporaryClearBtn;

   };

} // namespace dtQt

#endif // DELTA_DYNAMICRESOURCECONTROLBASE
