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
#ifndef DELTA_DYNAMICRESOURCECONTROL
#define DELTA_DYNAMICRESOURCECONTROL

#include <dtEditQt/export.h>
#include <dtQt/dynamicresourcecontrolbase.h>

namespace dtQt
{
   class SubQLabel;
   class SubQPushButton;
}

namespace dtDAL
{
   class ResourceActorProperty;
   class ResourceDescriptor;
}

class QLabel;

namespace dtEditQt
{
   class ResourceUIPlugin;

   /**
    * @class DynamicResourceControl
    * @brief This is the resource actor property.  It knows how to work with the various
    * resource data types (Terrain, Character, Mesh, Texture, sound, ...) from DataTypes.h
    * This control is not editable, but has several child controls and some of them
    * are editable.
    */
   class DT_EDITQT_EXPORT DynamicResourceControl : public dtQt::DynamicResourceControlBase
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      DynamicResourceControl();

      /**
       * Destructor
       */
      virtual ~DynamicResourceControl();

      /**
       * @see DynamicAbstractControl#createEditor
       */
      virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
         const QModelIndex& index);

   public slots:

      /**
       * The user pressed the "Edit" Button.
       */
      void editPressed();

      /**
       * @see DynamicAbstractControl#handleSubEditDestroy
       */
      virtual void handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint = QAbstractItemDelegate::NoHint);

   protected:

      ResourceUIPlugin* GetPlugin();

      /**
       * Figure out which resource descriptor  to get from EditorData and get it.
       * @return the current resource descriptor for our type, else an empty one of if type is invalid.
       */
      virtual dtDAL::ResourceDescriptor getCurrentResource();

      dtQt::SubQPushButton* mTemporaryEditBtn;
   };

} // namespace dtEditQt

#endif // DELTA_DYNAMICRESOURCECONTROL
