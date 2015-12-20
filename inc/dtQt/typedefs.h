/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
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
 */
#ifndef DELTA_PROPERTY_TYPEDEFS
#define DELTA_PROPERTY_TYPEDEFS

#include <QtCore/QMetaType>
#include <vector>
#include <osg/Node>
#include <dtCore/refptr.h>
#include <dtCore/actorproxy.h>
#include <dtCore/actorproperty.h>

// TODO: Remove these typedefs that have "Proxy" and "Ref" in the name.
// Actor Pointers
typedef dtCore::ObserverPtr<dtCore::BaseActorObject> ActorWeakPtr;
// Actor Pointer Vectors
typedef std::vector<dtCore::ActorPtr> ActorPtrVector;
typedef std::vector<dtCore::ActorPtr> ActorRefPtrVector;
typedef std::vector<ActorWeakPtr> ActorWeakPtrVector;
// Property Pointers
typedef dtCore::RefPtr<dtCore::ActorProperty> ActorPropertyPtr;
typedef dtCore::RefPtr<dtCore::ActorProperty> ActorPropertyRefPtr;
typedef dtCore::ObserverPtr<dtCore::ActorProperty> ActorPropertyWeakPtr;

// Node Pointers
typedef dtCore::RefPtr<osg::Node> OsgNodePtr;
typedef dtCore::ObserverPtr<osg::Node> OsgNodeWeakPtr;
typedef std::vector<OsgNodePtr> OsgNodePtrVector;
typedef std::vector<OsgNodeWeakPtr> OsgNodeWeakPtrVector;

//allows storage in a QVariant
Q_DECLARE_METATYPE(dtCore::ActorPtr);
Q_DECLARE_METATYPE(dtCore::ActorRefPtrVector);
Q_DECLARE_METATYPE(dtCore::RefPtr<dtCore::ActorProperty>);
Q_DECLARE_METATYPE(OsgNodePtr);
Q_DECLARE_METATYPE(OsgNodePtrVector);

#endif // DELTA_PROPERTY_TYPEDEFS
