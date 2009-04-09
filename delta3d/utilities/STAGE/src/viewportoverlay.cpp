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
 *
 * Matthew W. Campbell
 */
#include <prefix/dtstageprefix-src.h>
#include <QtGui/QAction>

#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Material>
#include <osg/CullFace>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/NodeVisitor>
#include <osgDB/WriteFile>
#include <dtCore/transformable.h>

#include <osg/Program>
#include <osg/Shader>
#include <dtUtil/macros.h>
#include <dtUtil/log.h>
#include <cstdio>

#include <dtDAL/actorproxyicon.h>

#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ViewportOverlay::ViewportOverlay()
   {
      mMultiSelectMode  = false;
      mOverlayGroup = new osg::Group();
      mSelectionDecorator = NULL;
      setupSelectionDecorator();
      if (!mOverlayGroup->containsNode(mSelectionDecorator.get()))
      {
         mOverlayGroup->addChild(mSelectionDecorator.get());
      }
      listenForEvents();
   }

   ///////////////////////////////////////////////////////////////////////////////
   ViewportOverlay::~ViewportOverlay()
   {

   }

   ///////////////////////////////////////////////////////////////////////////////
   ViewportOverlay::ViewportOverlay(const ViewportOverlay& rhs)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ViewportOverlay& ViewportOverlay::operator=(const ViewportOverlay& rhs)
   {
      return *this;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::onActorsSelected(std::vector< dtCore::RefPtr<dtDAL::ActorProxy> >& actors)
   {
      if (actors.empty())
      {
         clearCurrentSelection();
         EditorActions::GetInstance().actionEditDeleteActor->setEnabled(false);
         EditorActions::GetInstance().actionEditDuplicateActor->setEnabled(false);
         EditorActions::GetInstance().actionEditGroundClampActors->setEnabled(false);
         EditorActions::GetInstance().actionEditGotoActor->setEnabled(false);
         ViewportManager::GetInstance().refreshAllViewports();
         return;
      }

      if (!mMultiSelectMode)
      {
         clearCurrentSelection();
      }

      for (unsigned int i = 0; i < actors.size(); ++i)
      {
         const dtDAL::ActorProxy::RenderMode& renderMode = actors[i]->GetRenderMode();
         dtDAL::ActorProxyIcon* billBoardIcon = NULL;

         if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
         {
            billBoardIcon = actors[i]->GetBillBoardIcon();
            if (billBoardIcon != NULL)
            {
               select(actors[i]->GetBillBoardIcon()->GetDrawable());
            }
            else
            {
               LOG_ERROR("ActorProxy: " + actors[i]->GetName() + " has NULL billboard.");
            }
         }
         else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
         {
            select(actors[i]->GetActor());
         }
         else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
         {
            billBoardIcon = actors[i]->GetBillBoardIcon();
            if (billBoardIcon != NULL)
            {
               select(actors[i]->GetBillBoardIcon()->GetDrawable());
            }
            else
            {
               LOG_ERROR("ActorProxy: " + actors[i]->GetName() + " has NULL billboard.");
            }

            select(actors[i]->GetActor());
         }
         else
         {
            select(actors[i]->GetActor());
         }

         currentActorSelection.insert(actors[i]);
      }

      EditorActions::GetInstance().actionEditDeleteActor->setEnabled(true);
      EditorActions::GetInstance().actionEditDuplicateActor->setEnabled(true);
      EditorActions::GetInstance().actionEditGroundClampActors->setEnabled(true);
      EditorActions::GetInstance().actionEditGotoActor->setEnabled(true);
      ViewportManager::GetInstance().refreshAllViewports();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::select(dtCore::DeltaDrawable* drawable)
   {
      if (drawable == NULL || drawable->GetOSGNode() == NULL)
      {
         return;
      }

      if (mSelectionDecorator->containsNode(drawable->GetOSGNode()))
      {
         return;
      }

      mSelectionDecorator->addChild(drawable->GetOSGNode());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::unSelect(dtCore::DeltaDrawable* drawable)
   {
      if (drawable == NULL || drawable->GetOSGNode() == NULL)
      {
         return;
      }

      mSelectionDecorator->removeChild(drawable->GetOSGNode());
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ViewportOverlay::isActorSelected(dtDAL::ActorProxy* proxy) const
   {
      ActorProxyList::const_iterator itor = this->currentActorSelection.find(proxy);
      return (itor != this->currentActorSelection.end());
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::removeActorFromCurrentSelection(dtDAL::ActorProxy* proxy, bool clearAll)
   {
      ActorProxyList::iterator itor = this->currentActorSelection.find(proxy);
      if (itor == this->currentActorSelection.end())
      {
         return;
      }

      const dtDAL::ActorProxy::RenderMode& renderMode = proxy->GetRenderMode();
      dtDAL::ActorProxyIcon* billBoardIcon = NULL;

      //Make sure we remove the correct drawable from the selection list depending
      //on the render mode of the actor.
      if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_BILLBOARD_ICON)
      {
         billBoardIcon = proxy->GetBillBoardIcon();
         if (billBoardIcon != NULL)
         {
            unSelect(proxy->GetBillBoardIcon()->GetDrawable());
         }
         else
         {
            LOG_ERROR("ActorProxy: " + proxy->GetName() + " has NULL billboard.");
         }
      }
      else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR)
      {
         unSelect(proxy->GetActor());
      }
      else if (renderMode == dtDAL::ActorProxy::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
      {
         billBoardIcon = proxy->GetBillBoardIcon();
         if (billBoardIcon != NULL)
         {
            unSelect(proxy->GetBillBoardIcon()->GetDrawable());
         }
         else
         {
            LOG_ERROR("ActorProxy: " + proxy->GetName() + " has NULL billboard.");
         }

         unSelect(proxy->GetActor());
      }
      else
      {
         unSelect(proxy->GetActor());
      }

      //Finally remove the actor proxy from the selection list.
      if (clearAll)
      {
         this->currentActorSelection.erase(itor);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::clearCurrentSelection()
   {
      ActorProxyList::iterator itor = this->currentActorSelection.begin();
      while (itor != this->currentActorSelection.end())
      {
         removeActorFromCurrentSelection(const_cast<dtDAL::ActorProxy*>(itor->get()), false);
         ++itor;
      }

      this->currentActorSelection.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   static const std::string COLOR_SHADER_FORMAT =
      "void main (void) { gl_FragColor = vec4(%f, %f, %f, 1.0); }";
   ///////////////////////////////////////////////////////////////////////////////

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::setupSelectionDecorator()
   {
      bool createDecorator = !mSelectionDecorator.valid();
      if (createDecorator)
      {
         mSelectionDecorator = new osg::Group();
      }

      osg::StateSet* ss = mSelectionDecorator->getOrCreateStateSet();

      osg::StateAttribute::GLModeValue turnOn =
         osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON;

      //Set the material color using the editor's current preference.
      QColor qtColor = EditorData::GetInstance().getSelectionColor();

      dtCore::RefPtr<osg::Program> program = new osg::Program();
      dtCore::RefPtr<osg::Shader> fragShader = new osg::Shader(osg::Shader::FRAGMENT);

      char shaderText[255];
      snprintf(shaderText, 255, COLOR_SHADER_FORMAT.c_str(), qtColor.redF(), qtColor.greenF(), qtColor.blueF());
      fragShader->setShaderSource(std::string(shaderText));
      program->addShader(fragShader.get());
      ss->setAttributeAndModes(program.get(), turnOn);

      if (createDecorator)
      {
         //Create the required state attributes for wireframe overlay selection.
         osg::PolygonOffset* po = new osg::PolygonOffset;
         osg::PolygonMode* pm = new osg::PolygonMode();

         pm->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
         po->setFactor(-1.0f);
         po->setUnits(-1.0f);

         ss->setAttributeAndModes(pm, turnOn);
         ss->setAttributeAndModes(po, turnOn);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::onEditorPreferencesChanged()
   {
      setupSelectionDecorator();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::listenForEvents()
   {
      EditorEvents& ge = EditorEvents::GetInstance();

      connect(&ge,   SIGNAL(selectedActors(ActorProxyRefPtrVector&)),
               this, SLOT(onActorsSelected(ActorProxyRefPtrVector&)));

      connect(&ge,   SIGNAL(editorPreferencesChanged()),
               this, SLOT(onEditorPreferencesChanged()));
   }

} // namespace dtEditQt
