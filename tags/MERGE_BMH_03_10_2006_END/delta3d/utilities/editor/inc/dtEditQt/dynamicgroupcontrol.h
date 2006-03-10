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
#ifndef DELTA_DYNAMICGROUPCONTROL
#define DELTA_DYNAMICGROUPCONTROL

#include "dtEditQt/dynamicabstractparentcontrol.h"
#include <osg/ref_ptr>
#include <vector>


namespace dtDAL 
{
    class StringActorProperty;
}

namespace dtEditQt 
{

    class PropertyEditorModel;

    /**
    * @class DynamicGroupControl
    * @brief This is the dynamic control for the Group data type - used in the property editor
    * The primary purpose of the group control is to provide a visual grouping of property types
    * so that they aren't all laid out together.  
    */
    class DynamicGroupControl : public DynamicAbstractParentControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicGroupControl(const std::string &newName);

        /**
         * Destructor

         */
        virtual ~DynamicGroupControl();

        /**
         * Attempt to find a group control with the passed in name.  This is used primarily
         * on the root object to find an existing group.  However, it could easily be used 
         * for nested groups once that is supported.
         */
        DynamicGroupControl *getChildGroupControl(QString name);

        /**
         * Groups can have children.  This is how you add children to the group. Note that you can't 
         * remove a child once it's added.
         */
        void addChildControl(DynamicAbstractControl *child, PropertyEditorModel *model);

        // OVERRIDDEN METHODS FROM ABSTRACT BASE

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

    public slots: 

        /**
         * @see DynamicAbstractControl#updateData
         */
        virtual bool updateData(QWidget *widget);

    protected:

    private:
        QString name;


    };

}

#endif
