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
 * Jeffrey P. Houde
 */
#ifndef DELTA_EDITOR_VIEWPORT_CONTAINER
#define DELTA_EDITOR_VIEWPORT_CONTAINER

#include <QtGui/QWidget>

class QBoxLayout;
class QFrame;
class QLineEdit;
class QCheckBox;

namespace dtEditQt
{

   /**
    * The viewport is a higher level container widget used to house viewport widgets and
    * control their behavior.  The container has a toolbar in addition to the viewport
    * which contains actions that determine for example, how the editor gizmos work.
    * @note
    *  The viewports themselves have methods to control their behavior, therefore, this
    *  class acts as a UI interface to that functionality.
    */
   class EditorViewportContainer : public QWidget
   {
      Q_OBJECT

   public:
      /**
       * Constructs the viewport container.  Note, there is no viewport currently assigned
       * to it.
       * @param child  The child widget this container contains.
       * @param parent The parent widget to assign to this container.
       */
      EditorViewportContainer(QWidget* child = NULL, QWidget* parent = NULL);

      /**
      * Updates all snap settings to all viewports.
      */
      virtual void updateSnaps();

      /**
      * Sets the current child of the container.
      */
      void setChild(QWidget* child);

   public slots:

      void setSnapTranslation();
      void setSnapRotation();
      void setSnapScale();

      void setSnapEnabled(int state);

      void onSetSnapEnabled(bool translation, bool rotation, bool scale);
      void onSetSnapTranslation(float value);
      void onSetSnapRotation(float value);
      void onSetSnapScale(float value);

   protected:

      ///Adds the action objects to this containers toolbar.
      void createToolBar();

   private:
      QBoxLayout*       mLayout;
      QFrame*           mToolBar;
      QLineEdit*        mEditTrans;
      QLineEdit*        mEditAngle;
      QLineEdit*        mEditScale;
      QCheckBox*        mCheckBoxTrans;
      QCheckBox*        mCheckBoxAngle;
      QCheckBox*        mCheckBoxScale;

      void SetupSnapWidgets(QBoxLayout* layout);
   };

} // namespace dtEditQt

#endif // DELTA_EDITOR_VIEWPORT_CONTAINER
