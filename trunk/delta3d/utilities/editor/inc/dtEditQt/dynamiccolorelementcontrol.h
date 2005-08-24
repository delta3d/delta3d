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
#ifndef DELTA_DYNAMICCOLORELEMENTCONTROL
#define DELTA_DYNAMICCOLORELEMENTCONTROL

#include "dtEditQt/dynamicabstractcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"

namespace dtDAL 
{
    //class Vec3ActorProperty;
    class ColorRgbaActorProperty;
}

namespace dtEditQt 
{

    /**
    * @class DynamicColorElementControl
    * @brief This is the a sub control used by the various color property classes.  
    * In order to draw a vector 3 in the property tree, you actually have 
    * an X, Y, and Z entry.  This class is for that.  And, this class actually supports
    * the ColorRgbaActorProperty, and ColorRgbActorProperty with separate contructors.  
    * It provides a get and set method to get at the data.
    */
    class DynamicColorElementControl : public DynamicAbstractControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor - For the ColorRgbaActorProperty property
         *
         * @Note - We can put data in the constructor because aren't using the factory for this.
         */
        DynamicColorElementControl(dtDAL::ColorRgbaActorProperty *colorRGBA, int whichIndex, 
            const std::string &newLabel);

        /**
         * Constructor - For the ColorRgbAActorProperty property
         *
         * @Note - We can put data in the constructor because aren't using the factory for this.
         */
        //DynamicColorElementControl(dtDAL::Vec4ActorProperty *newVectorProp, int whichIndex, 
        //    const std::string &newLabel);

        /**
         * Destructor
         */
        virtual ~DynamicColorElementControl();

        /**
         * @see DynamicAbstractControl#initializeData
         */
        virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
            dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);

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

        /**
         * A convenience method to get the value from the associated 
         * vector.  Which element is the 0 based index into the vector.
         */
        int getValue();

        /**
         * Puts the passed in value into the appropriate vector at whichElement index
         */
        void setValue(int value);

        /**
         * Convert the 0.0 to 1.0 float color value to the 0 to 255 int display range.
         */
        static int convertColorFloatToInt(float value);

        /**
         * Convert the 0 to 255 int display range to 0.0 to a 1.0 float value
         */
        static float convertColorIntToFloat(int value);

        /**
         * @see DynamicAbstractControl#handleSubEditDestroy
         */
        void handleSubEditDestroy(QWidget *widget)
        {
            // we have to check - sometimes the destructor won't get called before the 
            // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
            if (widget == temporaryEditControl)
                temporaryEditControl = NULL;
        }

    public slots: 

        virtual bool updateData(QWidget *widget);

        void actorPropertyChanged(proxyRefPtr proxy,
            propertyRefPtr property);

    protected:

    private: 
        enum WHICHTYPE {RGBA, RGB} whichType;

        std::string label;

        //dtDAL::ColorRgbActorProperty *colorRGB;
        dtDAL::ColorRgbaActorProperty *colorRGBA;
        int elementIndex;

        // This pointer is not really in our control.  It is constructed in the createEditor() 
        // method and destroyed whenever QT feels like it (mostly when the control looses focus). 
        // We work around this by trapping the destruction of this object, it should
        // call our handleSubEditDestroy() method so we know to not hold this anymore
        SubQSpinBox *temporaryEditControl;
    };

}

#endif
