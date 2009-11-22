/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicfloatcontrol (.h & .cpp) - Using 'The MIT License'
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
 * Jeffrey Houde
 */
#ifndef DELTA_DYNAMICARRAYCONTROL
#define DELTA_DYNAMICARRAYCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicsubwidgets.h>

namespace dtDAL
{
    class ArrayActorPropertyBase;
}

namespace dtQt
{
   class DynamicArrayElementControl;

   /**
    * @class DynamicArrayControl
    * @brief This is the dynamic control for the float data type - used in the property editor
    */
   class DT_QT_EXPORT DynamicArrayControl : public DynamicAbstractParentControl
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      DynamicArrayControl();

      /**
       * Destructor
       */
      ~DynamicArrayControl();

      /**
       * @see DynamicAbstractControl#InitializeData
       */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* property);

      /**
       * @see DynamicAbstractControl#createEditor
       */
      virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index);

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

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
       * Resize the number of children in the array based on the actual property.
       *
       * @param[in]  forceRefresh  True to force the UI to refresh.
       * @param[in]  isChild       True if a child control is calling this from its parent.
       * @param[in]  initializing  True if we are initializing the list.
       */
      void resizeChildren(bool forceRefresh = false, bool isChild = false, bool initializing = false);

   public slots:
      /**
       * Signal when the data is updated.
       */
      virtual bool updateData(QWidget* widget);

      /**
       * Signal when the Add button has been clicked.
       */
      void onAddClicked();

      /**
       * Signal when the Clear button has been clicked.
       */
      void onClearClicked();

   private:
      /**
       * Updates the enabled status of the control buttons after an update.
       */
      void UpdateButtonStates();

      dtCore::RefPtr<dtDAL::ArrayActorPropertyBase> mProperty;

      QWidget*        mWrapper;
      SubQLabel*      mTextLabel;
      SubQPushButton* mAddButton;
      SubQPushButton* mClearButton;
   };

} // namespace dtEditQt

#endif // DELTA_DYNAMICARRAYCONTROL
