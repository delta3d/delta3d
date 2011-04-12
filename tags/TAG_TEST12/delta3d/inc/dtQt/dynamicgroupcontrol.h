/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - dynamicgroupcontrol (.h & .cpp) - Using 'The MIT License'
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
 * Curtiss Murphy
 */
#ifndef DELTA_DYNAMICGROUPCONTROL
#define DELTA_DYNAMICGROUPCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractparentcontrol.h>
#include <vector>


namespace dtDAL
{
    class StringActorProperty;
}

namespace dtQt
{

    class PropertyEditorModel;

    /**
    * @class DynamicGroupControl
    * @brief This is the dynamic control for the Group data type - used in the property editor
    * The primary purpose of the group control is to provide a visual grouping of property types
    * so that they aren't all laid out together.
    */
    class DT_QT_EXPORT DynamicGroupControl : public DynamicAbstractParentControl
    {
        Q_OBJECT
    public:
        /**
         * Constructor
         */
        DynamicGroupControl(const std::string& newName);

        /**
         * Destructor

         */
        virtual ~DynamicGroupControl();

        /**
         * Attempt to find a group control with the passed in name.  This is used primarily
         * on the root object to find an existing group.  However, it could easily be used
         * for nested groups once that is supported.
         */
        DynamicGroupControl* getChildGroupControl(QString name);

        /**
         * Groups can have children.  This is how you add children to the group. Note that you can't
         * remove a child once it's added.
         */
        void addChildControl(DynamicAbstractControl* child, PropertyEditorModel* model);

        // OVERRIDDEN METHODS FROM ABSTRACT BASE

        /**
         * @see DynamicAbstractControl#addSelfToParentWidget
         */
        void addSelfToParentWidget(QWidget& parent, QGridLayout& layout, int row);

        /**
         * @see DynamicAbstractControl#getDisplayName
         */
        virtual const QString getDisplayName();

    public slots:

        /**
         * @see DynamicAbstractControl#updateData
         */
        virtual bool updateData(QWidget* widget);

    protected:

    private:
        QString mName;
    };

} // namespace dtQt

#endif // DELTA_DYNAMICGROUPCONTROL
