/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicgameeventcontrol (.h & .cpp) - Using 'The MIT License'
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
 * William E. Johnson II
 */
#ifndef DELTA_DYNAMIC_GAME_EVENT_CONTROL
#define DELTA_DYNAMIC_GAME_EVENT_CONTROL

#include <dtEditQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamicsubwidgets.h>

// Forward References
namespace dtDAL
{
   class GameEventActorProperty;
}

namespace dtEditQt
{

   class PropertyEditorModel;

   class DT_EDITQT_EXPORT DynamicGameEventControl : public dtQt::DynamicAbstractControl
   {
      Q_OBJECT

      public:

         /// Constructor
         DynamicGameEventControl();

         /// Destructor
         virtual ~DynamicGameEventControl();

         /**
          * @see DynamicAbstractControl#InitializeData
          */
         void InitializeData(dtQt::DynamicAbstractControl* newParent, dtQt::PropertyEditorModel* newModel,
            dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty);

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
         virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index);

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
         /**
          * @see DynamicAbstractControl#handleSubEditDestroy
          */
         void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint)
         {
            // we have to check - sometimes the destructor won't get called before the
            // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
            if (widget == mTemporaryEditControl)
            {
               mTemporaryEditControl = NULL;
            }
         }

         void actorPropertyChanged(dtDAL::PropertyContainer& propCon,
                  dtDAL::ActorProperty& property);

         /**
          * Called when the user selects an item in the combo box
          */
         void itemSelected(int index);

      private:
         dtDAL::GameEventActorProperty* mProperty;

         // This pointer is not really in our control.  It is constructed in the createEditor()
         // method and destroyed whenever QT feels like it (mostly when the control looses focus).
         // We work around this by trapping the destruction of this object, it should
         // call our handleSubEditDestroy() method so we know to not hold this anymore
         dtQt::SubQComboBox* mTemporaryEditControl;
   };

} // namespace dtEditQt

#endif // DELTA_DYNAMIC_GAME_EVENT_CONTROL
