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
#ifndef DELTA_DYNAMICVECTORELEMENTCONTROL
#define DELTA_DYNAMICVECTORELEMENTCONTROL

#include "dtEditQt/dynamicabstractcontrol.h"
#include "dtEditQt/dynamicsubwidgets.h"

namespace dtDAL 
{
    class Vec3ActorProperty;
    class Vec2ActorProperty;
    class Vec4ActorProperty;
}

namespace dtEditQt 
{

    /**
    * @class DynamicVectorElementControl
    * @brief This is the a sub control used by the various vector property classes.  
    * In order to draw a vector 3 in the property tree, you actually have 
    * an X, Y, and Z entry.  This class is for that.  And, this class actually supports
    * the Vec2, Vec3, and Vec4 with separate contructors.  It provides a get and set
    * method to get at the data.
    */
    class DynamicVectorElementControl : public DynamicAbstractControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor - For the Vec2 property
         *
         * @Note - We can put data in the constructor because aren't using the factory for this.
         */
        DynamicVectorElementControl(dtDAL::Vec2ActorProperty *newVectorProp, int whichIndex, 
            const std::string &newLabel);

        /**
         * Constructor - For the Vec3 property
         *
         * @Note - We can put data in the constructor because aren't using the factory for this.
         */
        DynamicVectorElementControl(dtDAL::Vec3ActorProperty *newVectorProp, int whichIndex, 
            const std::string &newLabel);

        /**
         * Constructor - For the Vec3 property
         *
         * @Note - We can put data in the constructor because aren't using the factory for this.
         */
        DynamicVectorElementControl(dtDAL::Vec4ActorProperty *newVectorProp, int whichIndex, 
            const std::string &newLabel);

        /**
         * Destructor
         */
        virtual ~DynamicVectorElementControl();

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
        double getValue();

        /**
         * Puts the passed in value into the appropriate vector at whichElement index
         */
        void setValue(double value);

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
        void actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
            osg::ref_ptr<dtDAL::ActorProperty> property);

    protected:

    private: 
        enum WHICHTYPE {VEC2, VEC3, VEC4} whichType;

        bool isVecFloat;
        // the tool tip type label indicates that the vector is a float or a double
        std::string toolTipTypeLabel;
        std::string label;

        dtDAL::Vec2ActorProperty *vec2Prop;
        dtDAL::Vec3ActorProperty *vec3Prop;
        dtDAL::Vec4ActorProperty *vec4Prop;
        int elementIndex;

        // This pointer is not really in our control.  It is constructed in the createEditor() 
        // method and destroyed whenever QT feels like it (mostly when the control looses focus). 
        // We work around this by trapping the destruction of this object, it should
        // call our handleSubEditDestroy() method so we know to not hold this anymore
        SubQLineEdit *temporaryEditControl;
    };

}

#endif
