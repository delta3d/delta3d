/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicbyteelementcontrol (.h & .cpp) - Using 'The MIT License'
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
 * Jeff P. Houde
 */
#ifndef DELTA_DYNAMICBYTEELEMENTCONTROL
#define DELTA_DYNAMICBYTEELEMENTCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamicsubwidgets.h>

namespace dtCore
{
   class BitMaskActorProperty;
}

namespace dtQt
{

   /**
    * @class DynamicByteElementControl
    * @brief This is the dynamic control for the float data type - used in the property editor
    */
   class DT_QT_EXPORT DynamicByteElementControl : public DynamicAbstractControl
   {
      Q_OBJECT
   public:
      /**
       * Constructor
       */
      DynamicByteElementControl(int byteIndex);

      /**
       * Destructor
       */
      ~DynamicByteElementControl();

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

      const QString getHexString(unsigned int value);

      unsigned int getByteValue();

      bool getBitValue(int bitIndex);

      /**
       * Gets the current byte index of the element.
       */
      virtual int GetByteIndex();

      /**
       * Sets the current byte index of the element.
       */
      virtual void SetByteIndex(int index);


   public slots:

      /**
       * Signal when a bit button is toggled.
       */
      virtual void onBitToggled(bool checked);

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

   private:

      dtCore::BitMaskActorProperty* mProperty;

      int             mIndex;
      SubQLabel*      mTextLabel;
      std::vector<SubQToolButton*> mBitButtons;
   };

} // namespace dtQt

#endif // DELTA_DYNAMICBYTEELEMENTCONTROL
