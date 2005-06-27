/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2005, BMH Associates, Inc. 
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
* details.
*
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc., 
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
*
* @author Curtiss Murphy
*/
#ifndef DELTA_DYNAMICVECTORELEMENTCONTROL
#define DELTA_DYNAMICVECTORELEMENTCONTROL

#include "dtEditQt/dynamicabstractcontrol.h"

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

    public slots: 

        virtual bool updateData(QWidget *widget);


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
    };

}

#endif
