/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicgrouppropertycontrol (.h & .cpp) - Using 'The MIT License'
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
 * David Guthrie
 */
#ifndef STAGE_DYNAMICGROUPPROPERTYCONTROL
#define STAGE_DYNAMICGROUPPROPERTYCONTROL

#include <dtEditQt/export.h>
#include <dtQt/dynamicabstractparentcontrol.h>
#include <vector>


namespace dtCore
{
   class GroupActorProperty;
}

namespace dtEditQt
{
   class GroupUIPlugin;
   class PropertyEditorModel;

   /**
    * @class DynamicGroupPropertyControl
    * @brief This is the dynamic control for the Group data type - used in the property editor
    * The primary purpose of the group control is to provide a visual grouping of property types
    * so that they aren't all laid out together.
    */
   class DT_EDITQT_EXPORT STAGEDynamicGroupPropertyControl : public dtQt::DynamicAbstractParentControl
   {
         Q_OBJECT
      public:
         /**
          * Constructor
          */
         STAGEDynamicGroupPropertyControl();

         /**
          * Destructor
          */
         virtual ~STAGEDynamicGroupPropertyControl();

         /**
          * Groups can have children.  This is how you add children to the group. Note that you can't
          * remove a child once it's added.
          */
         void addChildControl(dtQt::DynamicAbstractControl* child, dtQt::PropertyEditorModel* model);

         // OVERRIDDEN METHODS FROM ABSTRACT BASE

         /**
          * @see DynamicAbstractControl#InitializeData
          */
         virtual void InitializeData(dtQt::DynamicAbstractControl* newParent, dtQt::PropertyEditorModel* model,
                                     dtCore::PropertyContainer* pc, dtCore::ActorProperty* property);

         /**
          * @see DynamicAbstractControl#addSelfToParentWidget
          */
         void addSelfToParentWidget(QWidget& parent, QGridLayout& layout, int row);

         /**
          * @see DynamicAbstractControl#createEditor
          */
         virtual QWidget* createEditor(QWidget* parent,
                                       const QStyleOptionViewItem& option,
                                       const QModelIndex &index);

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

      public slots:

         /**
          * @see DynamicAbstractControl#updateData
          */
         virtual bool updateData(QWidget* widget);

         ///Slot called when the edit button on the editor control is clicked.
         void EditClicked();

      protected:

      private:
         //helper method for getting the appropriate plugin for the property.
         GroupUIPlugin* GetPlugin();

         dtCore::GroupActorProperty* mGroupProperty;

   };

} // namespace dtEditQt

#endif // STAGE_DYNAMICGROUPPROPERTYCONTROL
