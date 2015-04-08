/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2012, MOVES Institute
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
 * Jeff P. Houde
 */
#ifndef DELTA_DYNAMICCONTAINERSELECTORCONTROL
#define DELTA_DYNAMICCONTAINERSELECTORCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicgroupcontrol.h>
#include <dtQt/dynamicsubwidgets.h>

namespace dtCore
{
   class ContainerSelectorActorProperty;
}

namespace dtQt
{
   /**
    * @class DynamicContainerSelectorControl
    * @brief This is the dynamic control for the String data type - used in the property editor
    */
   class DT_QT_EXPORT DynamicContainerSelectorControl : public DynamicAbstractParentControl
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      DynamicContainerSelectorControl();

      /**
       * Destructor
       */
      virtual ~DynamicContainerSelectorControl();

      /**
       * @see DynamicAbstractControl#InitializeData
       */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtCore::PropertyContainer* newPC, dtCore::ActorProperty* property);

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
       *	Returns true if the base property as well as all linked properties match.
       */
      virtual bool doPropertiesMatch();

      /**
       *	Refreshes the state of the child controls.
       */
      void refreshChildren();

   public slots:
      /**
       * @see DynamicAbstractControl#updateData
       */
      virtual bool updateData(QWidget* widget);

      /**
       * Called when the user selects an item in the combo box
       */
      void itemSelected(int index);

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

   protected:

      /**
       * Attempt to find a group control with the passed in name.  This is used primarily
       * on the root object to find an existing group.  However, it could easily be used
       * for nested groups once that is supported.
       */
      DynamicGroupControl* getChildGroupControl(QString name);

      /**
       * Adds a child to this group sorted in ABC order.
       */
      void addChildControlSorted(DynamicAbstractControl* child, PropertyEditorModel* model);

   private:
      dtCore::ContainerSelectorActorProperty* mProperty;

      // This pointer is not really in our control.  It is constructed in the createEditor()
      // method and destroyed whenever QT feels like it (mostly when the control looses focus).
      // We work around this by trapping the destruction of this object, it should
      // call our handleSubEditDestroy() method so we know to not hold this anymore
      SubQComboBox* mTemporaryComboControl;
   };

} // namespace dtQt

#endif // DELTA_DYNAMICCONTAINERSELECTORCONTROL
