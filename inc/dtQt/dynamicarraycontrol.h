/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicarraycontrol (.h & .cpp) - Using 'The MIT License'
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



namespace dtCore
{
    class ArrayActorPropertyBase;
}

namespace dtQt
{
   /**
    * @class DynamicArrayControl
    * @brief This is the dynamic control for the float data type - used in the property editor
    */
   class DT_QT_EXPORT DynamicArrayControl : public DynamicAbstractParentControl
   {
      Q_OBJECT

   public:
      typedef DynamicAbstractParentControl BaseClass;

      /**
       * Constructor
       */
      DynamicArrayControl();

      /**
       * Destructor
       */
      ~DynamicArrayControl();

      /**
      * This method is called by the delegate between the model and the view when data should
      * be pushed to the editor from the model.  I'm not exactly sure when that happens, but
      * I assume that's at construction time of the editor.  The default does nothing.
      *
      * @NOTE - In all likelihood, this method won't need the widget that's passed in,
      * since it's tracked by the individual subclass.
      */
      virtual void updateEditorFromModel(QWidget* widget) override;

      /**
       * @see DynamicAbstractControl#InitializeData
       */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtCore::PropertyContainer* newPC, dtCore::ActorProperty* property) override;

      /**
       * @see DynamicAbstractControl#createEditor
       */
      virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) override;

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint) override;

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

      /**
       * Resize the number of children in the array based on the actual property.
       *
       * @param[in]  forceRefresh  True to force the UI to refresh.
       * @param[in]  isChild       True if a child control is calling this from its parent.
       * @param[in]  initializing  True if we are initializing the list.
       */
      void resizeChildren(bool forceRefresh = false, bool isChild = false);

      /**
       * Sets the focus on a particular property index.
       */
      void SetIndexFocus(int index);

      /**
       * Retrieves the array property.
       */
      dtCore::ArrayActorPropertyBase* GetProperty() const {return mProperty;}

      /**
       *	Tests whether the base and all linked property arrays match in array size.
       */
      bool doPropertiesMatchInArraySize();
      
      void ItemAdd(dtCore::PropertyContainer& container,
         dtCore::ArrayActorPropertyBase& prop);

      void ItemSwap(dtCore::PropertyContainer& container,
         dtCore::ArrayActorPropertyBase& prop, int itemIndex, int targetIndex);

      void ItemCopy(dtCore::PropertyContainer& container,
         dtCore::ArrayActorPropertyBase& prop, int itemIndex);

      void ItemDelete(dtCore::PropertyContainer& container,
         dtCore::ArrayActorPropertyBase& prop, int itemIndex);

      void ItemClear(dtCore::PropertyContainer& container,
         dtCore::ArrayActorPropertyBase& prop);

      void NotifyPropertyChange(dtCore::PropertyContainer& container,
         dtCore::ArrayActorPropertyBase& prop, const std::string& oldValue);

   public slots:
      /**
       * Signal when the data is updated.
       */
      virtual bool updateData(QWidget* widget) override;

      /**
       * Signal when the Add button has been clicked.
       */
      void onAddClicked();

      /**
       * Signal when the Clear button has been clicked.
       */
      void onClearClicked();

      /**
       * Signal when the Reset button has been clicked.
       */
      void onResetClicked() override;

      virtual void onItemShiftUpClicked(int itemIndex);
      virtual void onItemShiftDownClicked(int itemIndex);
      virtual void onItemCopyClicked(int itemIndex);
      virtual void onItemDeleteClicked(int itemIndex);

      void PropertyAboutToChangePassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop,
               std::string oldValue, std::string newValue) override;

      void PropertyChangedPassThrough(dtCore::PropertyContainer& pc, dtCore::ActorProperty& prop) override;

   private:
      /**
       * Updates the enabled status of the control buttons after an update.
       */
      void UpdateButtonStates() override;

      dtCore::RefPtr<dtCore::ArrayActorPropertyBase> mProperty;

      SubQLabel*      mTextLabel;
      SubQToolButton* mAddButton;
      SubQToolButton* mClearButton;
   };

} // namespace dtEditQt

#endif // DELTA_DYNAMICARRAYCONTROL
