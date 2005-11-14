/* 
* Delta3D Open Source Game and Simulation Engine 
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free 
* Software Foundation; either version 2 of the License, or (at your option) 
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
* details.
*
* You should have received a copy of the GNU General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* @author Curtiss Murphy
*/
#ifndef DELTA_DYNAMICSUBWIDGETS
#define DELTA_DYNAMICSUBWIDGETS

#include "dtEditQt/dynamicabstractcontrol.h"
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPalette>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>
#include <vector>


namespace dtEditQt 
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
    class SubQLineEdit : public QLineEdit
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        SubQLineEdit(QWidget *parent, DynamicAbstractControl *newControl)
            : QLineEdit(parent), control(newControl) {}

        /**
         * Destructor
         */
        virtual ~SubQLineEdit()
        {  
            if (control != NULL) {
                control->updateData(this);  
                control->handleSubEditDestroy(this);
            }
        }

    private:
        DynamicAbstractControl *control;
    };

    /**
     * Subclass of QComboBox. Destructor updates the parent DynamicAbstractControl.
     * If you use this callback to clear out any handles to this control, then
     * it will be safe to hold onto a control created in the createEditor() method.
     */
    class SubQComboBox : public QComboBox
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        SubQComboBox(QWidget *parent, DynamicAbstractControl *newControl)
            : QComboBox(parent), control(newControl) {}

        /**
         * Destructor
         */
        virtual ~SubQComboBox()
        {  
            if (control != NULL) {
                control->updateData(this);  
                control->handleSubEditDestroy(this);
            }
        }

    private:
        DynamicAbstractControl *control;
    };

    /**
     * Subclass of QSpinBox. Destructor updates the parent DynamicAbstractControl.
     * If you use this callback to clear out any handles to this control, then
     * it will be safe to hold onto a control created in the createEditor() method.
     */
    class SubQSpinBox : public QSpinBox
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        SubQSpinBox(QWidget *parent, DynamicAbstractControl *newControl)
            : QSpinBox(parent), control(newControl) {}

        /**
         * Destructor
         */
        virtual ~SubQSpinBox()
        {  
            if (control != NULL) {
                control->updateData(this);  
                control->handleSubEditDestroy(this);
            }
        }

    private:
        DynamicAbstractControl *control;
    };



    /**
     * Subclass of QLabel. Destructor calls the handleSubEditDestroy.
     * If you use this callback to clear out any handles to this control, then
     * it will be safe to hold onto a control created in the createEditor() method.
     */
    class SubQLabel : public QLabel
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        SubQLabel(QString name, QWidget *parent, DynamicAbstractControl *newControl)
            : QLabel(name, parent), control(newControl) {}

        /**
         * Destructor
         */
        virtual ~SubQLabel()
        {  
            if (control != NULL) {
                control->updateData(this);  
                control->handleSubEditDestroy(this);
            }
        }

    private:
        DynamicAbstractControl *control;
    };


    /**
     * Subclass of QPushButton. Destructor calls the handleSubEditDestroy().  
     * If you use this callback to clear out any handles to this control, then
     * it will be safe to hold onto a control created in the createEditor() method.
     */
    class SubQPushButton : public QPushButton
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        SubQPushButton(QString name, QWidget *parent, DynamicAbstractControl *newControl)
            : QPushButton(name, parent), control(newControl) {}

        /**
         * Destructor
         */
        virtual ~SubQPushButton()
        {  
            if (control != NULL) {
                control->updateData(this);  
                control->handleSubEditDestroy(this);
            }
        }

    private:
        DynamicAbstractControl *control;
    };


    /**
     * Subclass of QWidget. Used to trap multiple children if needed.
     * Unlike the other SubXYZ classes, this one does NOT work with the parent control
     */
    class SubQWidget : public QWidget
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        SubQWidget(QWidget *parent, DynamicAbstractControl *newControl)
            : QWidget(parent), control(newControl) {}

        /**
         * Destructor
         */
        virtual ~SubQWidget()   
        {  
            childWidgets.clear();
        }
        
        /**
         * Add a child widget that we should track for palette changes.
         */
        void addManagedChild(QWidget *child) 
        {
            childWidgets.push_back(child);
        }

    private:
        DynamicAbstractControl *control;
        std::vector<QWidget *> childWidgets;
    };

}

#endif
