/* 
* Delta3D Open Source Game and Simulation Engine Level Editor 
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
#ifndef DELTA_DYNAMICCOLORRGBCONTROL
#define DELTA_DYNAMICCOLORRGBCONTROL

#include "dtEditQt/dynamicabstractparentcontrol.h"

class QLabel;
class QColorDialog;

namespace dtDAL 
{
    class ColorRgbaActorProperty;
}

namespace dtEditQt 
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
    class DynamicColorRGBAControl : public DynamicAbstractParentControl
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
         * @see DynamicAbstractControl#initializeData
         */
        virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
            dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);

        /**
         * @see DynamicAbstractControl#addSelfToParentWidget
         */
        void addSelfToParentWidget(QWidget &parent, QGridLayout &layout, int row);

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
        virtual void updateEditorFromModel(QWidget *widget);

        /**
         * @see DynamicAbstractControl#updateModelFromEditor
         */
        virtual bool updateModelFromEditor(QWidget *widget);

        /**
         * @see DynamicAbstractControl#createEditor
         */
        virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
            const QModelIndex &index);

        /**
         * @see DynamicAbstractControl#isEditable
         */
        virtual bool isEditable();

        /**
         * @see DynamicAbstractControl#isNeedsPersistentEditor
         */
        virtual bool isNeedsPersistentEditor();

        /**
         * @see DynamicAbstractControl#handleSubEditDestroy
         */
        virtual void handleSubEditDestroy(QWidget *widget);

        /** 
         * @see DynamicAbstractControl#installEventFilterOnControl
         */
        virtual void installEventFilterOnControl(QObject *filterObj);

    public slots: 

        virtual bool updateData(QWidget *widget);

        /**
        * Slot - color button is pressed
        */
        void colorPickerPressed();

        void actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property);

    protected:

    private: 
 
        DynamicColorElementControl *rElement;
        DynamicColorElementControl *gElement;
        DynamicColorElementControl *bElement;
        DynamicColorElementControl *aElement;

        dtDAL::ColorRgbaActorProperty *myProperty;

        // This pointer is not really in our control.  It is constructed in the createEditor() 
        // method and destroyed whenever QT feels like it (mostly when the control looses focus). 
        // We work around this by trapping the destruction of this object, it should
        // call our handleSubEditDestroy() method so we know to not hold this anymore
        SubQLabel *temporaryEditOnlyTextLabel;
        SubQPushButton *temporaryColorPicker;

        QColorDialog *colorDialog;
    };

}

#endif
