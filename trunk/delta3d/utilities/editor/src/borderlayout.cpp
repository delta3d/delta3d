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
 * @author
*/

#include <QtGui>

#include "dtEditQt/borderlayout.h"

namespace dtEditQt 
{

BorderLayout::BorderLayout(QWidget *parent, int margin, int spacing)
    : QLayout(parent)
{
    setMargin(margin);
    setSpacing(spacing);
}

BorderLayout::BorderLayout(int spacing)
{
    setSpacing(spacing);
}


BorderLayout::~BorderLayout()
{
    QLayoutItem *l;
    while ((l = takeAt(0)))
        delete l;
}

void BorderLayout::addItem(QLayoutItem *item)
{
    add(item, West);
}

void BorderLayout::addWidget(QWidget *widget, Position position)
{
    add(new QWidgetItem(widget), position);
}

Qt::Orientations BorderLayout::expandingDirections() const
{
    return Qt::Horizontal | Qt::Vertical;
}

bool BorderLayout::hasHeightForWidth() const
{
    return false;
}

int BorderLayout::count() const
{
    return list.size();
}

QLayoutItem *BorderLayout::itemAt(int index) const
{
    ItemWrapper *wrapper = list.value(index);
    if (wrapper)
        return wrapper->item;
    else
        return 0;
}

QSize BorderLayout::minimumSize() const
{
    return calculateSize(MinimumSize);
}

void BorderLayout::setGeometry(const QRect &rect)
{
    ItemWrapper *center = 0;
    int eastWidth = 0;
    int westWidth = 0;
    int northHeight = 0;
    int southHeight = 0;
    int centerHeight = 0;
    int i;

    QLayout::setGeometry(rect);

    for (i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        Position position = wrapper->position;

        if (position == North) {
            item->setGeometry(QRect(rect.x(), northHeight, rect.width(),
                                    item->sizeHint().height()));

            northHeight += item->geometry().height() + spacing();
        } else if (position == South) {
            item->setGeometry(QRect(item->geometry().x(),
                                    item->geometry().y(), rect.width(),
                                    item->sizeHint().height()));

            southHeight += item->geometry().height() + spacing();

            item->setGeometry(QRect(rect.x(),
                              rect.y() + rect.height() - southHeight + spacing(),
                              item->geometry().width(),
                              item->geometry().height()));
        } else if (position == Center) {
            center = wrapper;
        }
    }

    centerHeight = rect.height() - northHeight - southHeight;

    for (i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        QLayoutItem *item = wrapper->item;
        Position position = wrapper->position;

        if (position == West) {
            item->setGeometry(QRect(rect.x() + westWidth, northHeight,
                                    item->sizeHint().width(), centerHeight));

            westWidth += item->geometry().width() + spacing();
        } else if (position == East) {
            item->setGeometry(QRect(item->geometry().x(), item->geometry().y(),
                                    item->sizeHint().width(), centerHeight));

            eastWidth += item->geometry().width() + spacing();

            item->setGeometry(QRect(
                              rect.x() + rect.width() - eastWidth + spacing(),
                              northHeight, item->geometry().width(),
                              item->geometry().height()));
        }
    }

    if (center)
        center->item->setGeometry(QRect(westWidth, northHeight,
                                        rect.width() - eastWidth - westWidth,
                                        centerHeight));
}

QSize BorderLayout::sizeHint() const
{
    return calculateSize(SizeHint);
}

QLayoutItem *BorderLayout::takeAt(int index)
{
    if (index >= 0 && index < list.size()) {
        ItemWrapper *layoutStruct = list.takeAt(index);
        return layoutStruct->item;
    }
    return 0;
}

void BorderLayout::add(QLayoutItem *item, Position position)
{
    list.append(new ItemWrapper(item, position));
}

QSize BorderLayout::calculateSize(SizeType sizeType) const
{
    QSize totalSize;

    for (int i = 0; i < list.size(); ++i) {
        ItemWrapper *wrapper = list.at(i);
        Position position = wrapper->position;
        QSize itemSize;

        if (sizeType == MinimumSize)
            itemSize = wrapper->item->minimumSize();
        else // (sizeType == SizeHint)
            itemSize = wrapper->item->sizeHint();

        if (position == North || position == South || position == Center)
            totalSize.rheight() += itemSize.height();

        if (position == West || position == East || position == Center)
            totalSize.rwidth() += itemSize.width();
    }
    return totalSize;
}

}
