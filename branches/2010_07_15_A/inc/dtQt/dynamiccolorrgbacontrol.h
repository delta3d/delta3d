/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamiccolorrgbacontrol (.h & .cpp) - Using 'The MIT License'
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
#ifndef DELTA_DYNAMICCOLORRGBCONTROL
#define DELTA_DYNAMICCOLORRGBCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractparentcontrol.h>

class QLabel;
class QColorDialog;

namespace dtDAL
{
   class ColorRgbaActorProperty;
}

namespace dtQt
{
   class DynamicColorElementControl;
   class SubQLabel;
   class SubQPushButton;

   /**
     * @class DynamicColorRGBAControl
     * @brief This is the dynamic control for the an RGBA Color picker - used in the property editor
     * @Note It adds a group of child elements to the tree, since you can't edit 3 things
     * in one control easily.
     */
   class DT_QT_EXPORT DynamicColorRGBAControl : public DynamicAbstractParentControl
   {
      Q_OBJECT
   public:
      /**
       * Constructor
       */
      DynamicColorRGBAControl();

      /**
       * Destructor
       */
      virtual ~DynamicColorRGBAControl();


      /**
       * @see DynamicAbstractControl#InitializeData
       */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
               dtDAL::PropertyContainer* pc, dtDAL::ActorProperty* property);

      /**
       * @see DynamicAbstractControl#addSelfToParentWidget
       */
      void addSelfToParentWidget(QWidget& parent, QGridLayout& layout, int row);

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
       * @see DynamicAbstractControl#NeedsPersistentEditor
       */
      virtual bool NeedsPersistentEditor();

      /**
       * @see DynamicAbstractControl#installEventFilterOnControl
       */
      virtual void installEventFilterOnControl(QObject* filterObj);

   public slots:

      virtual bool updateData(QWidget* widget);

      /**
       * Slot - color button is pressed
       */
      void colorPickerPressed();

      void actorPropertyChanged(dtDAL::PropertyContainer& propCon,
               dtDAL::ActorProperty& property);

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

   protected:

      DynamicColorElementControl* CreateElementControl(int index, const std::string& label,
               PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC);

   private:

      DynamicColorElementControl* rElement;
      DynamicColorElementControl* gElement;
      DynamicColorElementControl* bElement;
      DynamicColorElementControl* aElement;

      dtDAL::ColorRgbaActorProperty* mProperty;

      // This pointer is not really in our control.  It is constructed in the createEditor()
      // method and destroyed whenever QT feels like it (mostly when the control looses focus).
      // We work around this by trapping the destruction of this object, it should
      // call our handleSubEditDestroy() method so we know to not hold this anymore
      QWidget* mTemporaryWrapper;

      SubQLabel *mTemporaryEditOnlyTextLabel;
      SubQPushButton *mTemporaryColorPicker;

      QColorDialog *colorDialog;
   };

}

#endif // DELTA_DYNAMICCOLORRGBCONTROL
