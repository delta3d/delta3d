/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_DYNAMICVECTORELEMENTCONTROL
#define DELTA_DYNAMICVECTORELEMENTCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamicsubwidgets.h>

namespace dtDAL
{
   class Vec3fActorProperty;
   class Vec2fActorProperty;
   class Vec4fActorProperty;
   class Vec3dActorProperty;
   class Vec2dActorProperty;
   class Vec4dActorProperty;
}

namespace dtQt
{

   /**
    * @class DynamicVectorElementControl
    * @brief This is the a sub control used by the various vector property classes.
    * In order to draw a vector 3 in the property tree, you actually have
    * an X, Y, and Z entry.  This class is for that.  And, this class actually supports
    * the Vec2, Vec3, and Vec4 with separate contructors.  It provides a get and set
    * method to get at the data.
    */
   class DT_QT_EXPORT DynamicVectorElementControl : public DynamicAbstractControl
   {
      Q_OBJECT

   public:

      /**
       * Constructor - For the Vec2f property
       *
       * @Note - We can put data in the constructor because aren't using the factory for this.
       */
      DynamicVectorElementControl(dtDAL::Vec2fActorProperty* newVectorProp, int whichIndex,
         const std::string& newLabel);

      /**
       * Constructor - For the Vec2d property
       *
       * @Note - We can put data in the constructor because aren't using the factory for this.
       */
      DynamicVectorElementControl(dtDAL::Vec2dActorProperty* newVectorProp, int whichIndex,
         const std::string& newLabel);

      /**
       * Constructor - For the Vec3f property
       *
       * @Note - We can put data in the constructor because aren't using the factory for this.
       */
      DynamicVectorElementControl(dtDAL::Vec3fActorProperty* newVectorProp, int whichIndex,
         const std::string& newLabel);

      /**
       * Constructor - For the Vec3d property
       *
       * @Note - We can put data in the constructor because aren't using the factory for this.
       */
      DynamicVectorElementControl(dtDAL::Vec3dActorProperty* newVectorProp, int whichIndex,
         const std::string& newLabel);


      /**
       * Constructor - For the Vec4f property
       *
       * @Note - We can put data in the constructor because aren't using the factory for this.
       */
      DynamicVectorElementControl(dtDAL::Vec4fActorProperty* newVectorProp, int whichIndex,
         const std::string& newLabel);

      /**
       * Constructor - For the Vec4d property
       *
       * @Note - We can put data in the constructor because aren't using the factory for this.
       */
      DynamicVectorElementControl(dtDAL::Vec4dActorProperty* newVectorProp, int whichIndex,
         const std::string& newLabel);

      /**
       * Destructor
       */
      virtual ~DynamicVectorElementControl();

      /**
       * @see DynamicAbstractControl#InitializeData
       */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtDAL::PropertyContainer* pc, dtDAL::ActorProperty* property);

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
       * @see DynamicAbstractControl#isEditable
       */
      virtual bool isEditable();

      /**
       * A convenience method to get the value from the associated
       * vector.  Which element is the 0 based index into the vector.
       */
      double getValue();

      /**
       * Puts the passed in value into the appropriate vector at whichElement index
       */
      void setValue(double value);

   public slots:

      virtual bool updateData(QWidget* widget);

      void actorPropertyChanged(dtDAL::PropertyContainer& propCon,
               dtDAL::ActorProperty& property);

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint)
      {
         // we have to check - sometimes the destructor won't get called before the
         // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
         if (widget != NULL && widget == mTemporaryEditControl)
         {
            mTemporaryEditControl = NULL;
         }
      }

   protected:

   private:
      enum WHICHTYPE
      {
         VEC2F,
         VEC2D,
         VEC3F,
         VEC3D,
         VEC4F,
         VEC4D
      } mWhichType;

      bool mIsVecFloat;
      // the tool tip type label indicates that the vector is a float or a double
      std::string mToolTipTypeLabel;
      std::string mLabel;

      dtDAL::ActorProperty*  mActiveProp;

      dtDAL::Vec2fActorProperty* mVec2fProp;
      dtDAL::Vec3fActorProperty* mVec3fProp;
      dtDAL::Vec4fActorProperty* mVec4fProp;
      dtDAL::Vec2dActorProperty* mVec2dProp;
      dtDAL::Vec3dActorProperty* mVec3dProp;
      dtDAL::Vec4dActorProperty* mVec4dProp;
      int mElementIndex;

      // This pointer is not really in our control.  It is constructed in the createEditor()
      // method and destroyed whenever QT feels like it (mostly when the control looses focus).
      // We work around this by trapping the destruction of this object, it should
      // call our handleSubEditDestroy() method so we know to not hold this anymore
      SubQLineEdit* mTemporaryEditControl;
   };

} // namespace dtQt

#endif // DELTA_DYNAMICVECTORELEMENTCONTROL
