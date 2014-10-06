/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

#ifndef NODEREGISTRY_H_
#define NODEREGISTRY_H_

#include "../export.h"
#include <dtDirector/nodepluginregistry.h>

namespace dtExample
{
   class TEST_APP_EXPORT DirectorNodePluginRegistry: public dtDirector::NodePluginRegistry
   {
   public:

      static dtCore::RefPtr<dtDirector::NodeType> SELECT_DESTINATION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> MOVE_TO_DESTINATION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> SELECT_TARGET_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> TURN_TO_FACE_DIRECTION_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> LOOK_AT_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> CHANGE_STANCE_NODE_TYPE;

      static dtCore::RefPtr<dtDirector::NodeType> CIVILIAN_AVATAR_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> MARINE_AVATAR_NODE_TYPE;
      static dtCore::RefPtr<dtDirector::NodeType> APPLICATION_NODE_TYPE;

      DirectorNodePluginRegistry();
      virtual ~DirectorNodePluginRegistry();

      virtual std::string GetNodeLibraryType() const;

      virtual void RegisterNodeTypes();

   };
}
#endif /* NODEREGISTRY_H_ */
