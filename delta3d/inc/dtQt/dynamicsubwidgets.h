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
#ifndef DELTA_DYNAMICSUBWIDGETS
#define DELTA_DYNAMICSUBWIDGETS

#include <dtQt/export.h>
#include <dtQt/dynamicabstractcontrol.h>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtCore/QObject>
#include <QtGui/QPalette>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>
#include <vector>

/*
 * WARNING:  You cannot add signals and slots to the classes in this file.  The reason is that
 *           moc will only parse one Q_OBJECT per header.  The rest will be ignored.
 *           If you wish to add signals and slots, you will have to put these subclasses into separate files.
 */

namespace dtQt
{

   /**
    * This is collection of subclasses of QWidgets that updates a dynamic abstract control
    * on destruction.  It's necessary because when you QT's Model View architecture,
    * it has a bug.  The original bug occured when you selected a new object in a viewport
    * while you had changed a value in the property editor.  To cause it, select an object,
    * and edit one of the values.  While you're editing, go back to the viewport and select
    * a new actor.  This generates an event to change the property editor, but also causes
    * the Tree/View architecture to delete your editor QWidget. We loose any changes we made
    * right before we did the new selection.
    *
    * So, to fix it, we trap the destruction event of our QWidget objects and tell them
    * to update the model data.  Nasty, and class heavy, but it was the only way sure
    * fire way around the problem.
    */

   /**
    * Subclass of QLineEdit. Destructor updates the parent DynamicAbstractControl.
    * If you use this callback to clear out any handles to this control, then
    * it will be safe to hold onto a control created in the createEditor() method.
    */
   class DT_QT_EXPORT SubQLineEdit : public QLineEdit
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SubQLineEdit(QWidget* parent, DynamicAbstractControl* newControl)
         : QLineEdit(parent)
         , mControl(newControl)
      {
      }

      /**
       * Destructor
       */
      virtual ~SubQLineEdit()
      {
      }

   private:
      DynamicAbstractControl* mControl;
   };

   /**
    * Subclass of QComboBox. Destructor updates the parent DynamicAbstractControl.
    * If you use this callback to clear out any handles to this control, then
    * it will be safe to hold onto a control created in the createEditor() method.
    */
   class DT_QT_EXPORT SubQComboBox : public QComboBox
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SubQComboBox(QWidget* parent, DynamicAbstractControl* newControl)
         : QComboBox(parent)
         , mControl(newControl)
      {
      }

      /**
       * Destructor
       */
      virtual ~SubQComboBox()
      {
      }

   private:
      DynamicAbstractControl* mControl;
   };

   /**
    * Subclass of QSpinBox. Destructor updates the parent DynamicAbstractControl.
    * If you use this callback to clear out any handles to this control, then
    * it will be safe to hold onto a control created in the createEditor() method.
    */
   class DT_QT_EXPORT SubQSpinBox : public QSpinBox
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SubQSpinBox(QWidget* parent, DynamicAbstractControl* newControl)
         : QSpinBox(parent)
         , mControl(newControl)
      {
      }

      /**
       * Destructor
       */
      virtual ~SubQSpinBox()
      {
      }

   private:
      DynamicAbstractControl* mControl;
   };

   /**
    * Subclass of QLabel. Destructor calls the handleSubEditDestroy.
    * If you use this callback to clear out any handles to this control, then
    * it will be safe to hold onto a control created in the createEditor() method.
    */
   class DT_QT_EXPORT SubQLabel : public QLabel
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SubQLabel(QString name, QWidget* parent, DynamicAbstractControl* newControl)
         : QLabel(name, parent)
         , mControl(newControl)
      {
      }

      /**
       * Destructor
       */
      virtual ~SubQLabel()
      {
      }

   private:
      DynamicAbstractControl* mControl;
   };

   /**
    * Subclass of QPushButton. Destructor calls the handleSubEditDestroy().
    * If you use this callback to clear out any handles to this control, then
    * it will be safe to hold onto a control created in the createEditor() method.
    */
   class DT_QT_EXPORT SubQPushButton : public QPushButton
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SubQPushButton(QString name, QWidget* parent, DynamicAbstractControl* newControl)
         : QPushButton(name, parent)
         , mControl(newControl)
      {
         setFlat(true);
      }

      /**
       * Destructor
       */
      virtual ~SubQPushButton()
      {
      }

   private:
      DynamicAbstractControl* mControl;
   };

   /**
    * Subclass of QToolButton. Destructor calls the handleSubEditDestroy().
    * If you use this callback to clear out any handles to this control, then
    * it will be safe to hold onto a control created in the createEditor() method.
    */
   class DT_QT_EXPORT SubQToolButton : public QToolButton
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SubQToolButton(QWidget* parent, DynamicAbstractControl* newControl)
         : QToolButton(parent)
         , mControl(newControl)
      {
      }

      /**
       * Destructor
       */
      virtual ~SubQToolButton()
      {
      }

   private:
      DynamicAbstractControl* mControl;
   };

   /**
    * Subclass of QWidget. Used to trap multiple children if needed.
    * Unlike the other SubXYZ classes, this one does NOT work with the parent control
    */
   class DT_QT_EXPORT SubQWidget : public QWidget
   {
      Q_OBJECT

   public:
      /**
       * Constructor
       */
      SubQWidget(QWidget* parent, DynamicAbstractControl* newControl)
         : QWidget(parent)
         , mControl(newControl)
      {
      }

      /**
       * Destructor
       */
      virtual ~SubQWidget()
      {
         mChildWidgets.clear();
      }

      /**
       * Add a child widget that we should track for palette changes.
       */
      void addManagedChild(QWidget* child)
      {
         mChildWidgets.push_back(child);
      }

   private:
      DynamicAbstractControl* mControl;
      std::vector<QWidget*>   mChildWidgets;
   };

} // namespace dtEditQt

#endif // DELTA_DYNAMICSUBWIDGETS
