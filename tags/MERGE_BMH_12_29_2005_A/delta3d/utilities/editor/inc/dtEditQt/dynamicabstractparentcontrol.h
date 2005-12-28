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
#ifndef DELTA_DYNAMICABSTRACTPARENTCONTROL
#define DELTA_DYNAMICABSTRACTPARENTCONTROL

#include "dtEditQt/dynamicabstractcontrol.h"
#include <vector>

namespace dtEditQt 
{

    //class DynamicVectorElementControl;

    /**
    * @class DynamicAbstractParentControl
    * @brief This is a base class for any dynamic control that has children.
    * It keeps a vector of DynamicAbstractControl *'s and knows how to work 
    * with it.  This is common behavior in the dynamic controls that was pulled
    * to this class.  
    */
    class DynamicAbstractParentControl : public DynamicAbstractControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicAbstractParentControl();

        /**
         * Destructor
         */
        virtual ~DynamicAbstractParentControl();


        /**
         * A clean up method you should use when you are planning to reuse this control.
         */
        void removeAllChildren(PropertyEditorModel *model) ;

        /**
         * @see DynamicAbstractControl#getChildIndex
         */
        virtual int getChildIndex(DynamicAbstractControl *child);

        /**
         * @see DynamicAbstractControl#getChild
         */
        virtual DynamicAbstractControl *getChild(int index);

        /**
         * @see DynamicAbstractControl#getChildCount
         */
        virtual int getChildCount();

    protected: 
        std::vector <DynamicAbstractControl *> children;
    };

}

#endif
