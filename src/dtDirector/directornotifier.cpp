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
 * Author: Jeff P. Houde
 */

#include <dtDirector/directornotifier.h>


namespace dtDirector
{
   ////////////////////////////////////////////////////////////////////////////////
   DirectorNotifier::DirectorNotifier()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   DirectorNotifier::~DirectorNotifier()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorNotifier::Update(bool /*pause*/, bool /*step*/)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorNotifier::OnNodeExecution(Node* /*node*/, const std::string& /*input*/, const std::vector<std::string>& /*outputs*/)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorNotifier::OnValueChanged(Node* /*node*/)
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorNotifier::OnBeginDebugging()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorNotifier::OnEndDebugging()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void DirectorNotifier::OnStepDebugging()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DirectorNotifier::ShouldBreak(Node* /*node*/)
   {
      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DirectorNotifier::BreakNode(Node* /*node*/, bool /*shouldFocus*/)
   {
   }
}
