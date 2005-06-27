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

#include "dtEditQt/dynamicabstractparentcontrol.h"

namespace dtEditQt 
{

    ///////////////////////////////////////////////////////////////////////////////
    DynamicAbstractParentControl::DynamicAbstractParentControl()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicAbstractParentControl::~DynamicAbstractParentControl()
    {
        removeAllChildren(NULL);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void DynamicAbstractParentControl::removeAllChildren(PropertyEditorModel *model)
    {
        // Note that we no longer use hte model.  it remains for future potential issues.

        std::vector<DynamicAbstractControl *>::iterator childIter;

        // remove all the controls from the inner layout
        for (childIter = children.begin(); childIter != children.end(); ++childIter) {
            DynamicAbstractControl *control = (*childIter);    

            if (control != NULL) {
                // Groups delete their children in the destructor.
                delete control;              
            }
        }
        children.clear();
    }


    /////////////////////////////////////////////////////////////////////////////////
    int DynamicAbstractParentControl::getChildIndex(DynamicAbstractControl *child)
    {
        std::vector <DynamicAbstractControl *>::const_iterator iter;
        int index = 0;
        bool found = false;

        // walk through our list to find the child.
        for(iter = children.begin(); iter != children.end(); ++iter, index++){
            if ((*iter) == child) {
                found = true;
                break;
            }
        }

        // found it, so return result
        if (found)
            return index;
        else 
            return 0;
    }

    /////////////////////////////////////////////////////////////////////////////////
    DynamicAbstractControl *DynamicAbstractParentControl::getChild(int index)
    {
        if (children.size() > (unsigned) index)
            return children[index];
        else 
            return NULL;
    }

    /////////////////////////////////////////////////////////////////////////////////
    int DynamicAbstractParentControl::getChildCount()
    {
        return children.size();
    }

}
