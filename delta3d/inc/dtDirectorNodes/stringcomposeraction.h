/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author: MG
 */

#ifndef STRING_COMPOSER_ACTION_H
#define STRING_COMPOSER_ACTION_H

////////////////////////////////////////////////////////////////////////////////

#include <dtDirector/actionnode.h>
#include <dtDirectorNodes/nodelibraryexport.h>

#include <dtCore/stringactorproperty.h>

namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   class NODE_LIBRARY_EXPORT StringComposerAction : public ActionNode
   {
   public:

      StringComposerAction();

      virtual const std::string& GetName();

      virtual void Init(const NodeType& nodeType, DirectorGraph* graph);
      virtual void BuildPropertyMap();
      virtual bool Update(float simDelta, float delta, int input, bool firstUpdate);

      void SetText(const std::string& newText);
      std::string GetText() const;

   protected:

      ~StringComposerAction();

   private:

      void SetArgument(const std::string&) {}
      std::string GetArgument() const { return std::string(); }

      int GetNumberOfArgumentSequences(const std::string& sourceText);
      std::string GetComposedString();

      void SetResultText(const std::string& newText);
      std::string GetResultText() const;

      std::string mSourceText;
      std::string mResultText;
      dtCore::RefPtr<dtCore::StringActorProperty> mTextProperty;
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // STRING_COMPOSER_ACTION_H
