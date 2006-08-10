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
#ifndef DELTA_DYNAMICVEC2CONTROL
#define DELTA_DYNAMICVEC2CONTROL

#include "dtEditQt/dynamicabstractparentcontrol.h"

namespace dtDAL 
{
    class Vec2ActorProperty;
    class Vec2fActorProperty;
    class Vec2dActorProperty;
}

namespace dtEditQt 
{

    class DynamicVectorElementControl;

    /**
    * @class DynamicVec2Control
    * @brief This is the dynamic control for the 2 dimensional vector data type - 
    * used in the property editor
    * @Note It adds a group of child elements to the tree, since you can't edit 3 things
    * in one control easily.
    */
    class DynamicVec2Control : public DynamicAbstractParentControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicVec2Control();

        /**
         * Destructor
         */
        ~DynamicVec2Control();


        /**
         * @see DynamicAbstractControl#initializeData
         */
        virtual void initializeData(DynamicAbstractControl *newParent, PropertyEditorModel *model,
            dtDAL::ActorProxy *proxy, dtDAL::ActorProperty *property);

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

    public slots: 

        virtual bool updateData(QWidget *widget);

    protected:

    private: 
        DynamicVectorElementControl *xElement;
        DynamicVectorElementControl *yElement;

        bool isVecFloat;
        // the tool tip type label indicates that the vector is a float or a double
        std::string toolTipTypeLabel;

        dtCore::RefPtr<dtDAL::Vec2ActorProperty>  myVec2Property;
        dtCore::RefPtr<dtDAL::Vec2fActorProperty> myVec2fProperty;
        dtCore::RefPtr<dtDAL::Vec2dActorProperty> myVec2dProperty;
    };

}

#endif
