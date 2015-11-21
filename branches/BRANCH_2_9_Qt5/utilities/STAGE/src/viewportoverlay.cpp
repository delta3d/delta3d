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
#include <prefix/stageprefix.h>
#include <QtWidgets/QAction>

#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Group>
#include <osg/Program>
#include <osg/Shader>

#include <dtUtil/log.h>
#include <dtUtil/mswinmacros.h> //for snprintf definition
#include <dtActors/volumeeditactor.h>

#include <dtCore/actorproxyicon.h>

#include <dtEditQt/editorevents.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/viewportoverlay.h>
#include <dtEditQt/viewportmanager.h>

#include <dtCore/map.h>

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
   void ViewportOverlay::onActorsSelected(std::vector< dtCore::RefPtr<dtCore::BaseActorObject> >& actors)
   {
      ViewportManager::GetInstance().refreshActorSelection(actors);

      //ensure that the Brush outline is on (if it turns out the brush is selected it
      //will get disabled later)
      EditorData::GetInstance().getMainWindow()->GetVolumeEditActor()->EnableOutline(true);

      clearCurrentSelection();
      if (actors.empty())
      {
         EditorActions::GetInstance().mActionFileExportPrefab->setEnabled(false);
         EditorActions::GetInstance().mActionGroupActors->setEnabled(false);
         EditorActions::GetInstance().mActionUngroupActors->setEnabled(false);
         EditorActions::GetInstance().mActionEditDeleteActor->setEnabled(false);
         EditorActions::GetInstance().mActionEditDuplicateActor->setEnabled(false);
         EditorActions::GetInstance().mActionEditGroundClampActors->setEnabled(false);
         EditorActions::GetInstance().mActionEditGotoActor->setEnabled(false);
         EditorActions::GetInstance().mActionEditResetTranslation->setEnabled(false);
         EditorActions::GetInstance().mActionEditResetRotation->setEnabled(false);
         EditorActions::GetInstance().mActionEditResetScale->setEnabled(false);
         ViewportManager::GetInstance().refreshAllViewports();
         return;
      }

      //if (!mMultiSelectMode)
      //{
      //   clearCurrentSelection();
      //}

      for (unsigned int i = 0; i < actors.size(); ++i)
      {
         const dtCore::BaseActorObject::RenderMode& renderMode = actors[i]->GetRenderMode();
         dtCore::ActorProxyIcon* billBoardIcon = NULL;

         if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
         {
            billBoardIcon = actors[i]->GetBillBoardIcon();
            if (billBoardIcon != NULL)
            {
               select(actors[i]->GetBillBoardIcon()->GetDrawable());
            }
            else
            {
               LOG_ERROR("BaseActorObject: " + actors[i]->GetName() + " has NULL billboard.");
            }
         }
         else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR)
         {
            select(actors[i]->GetDrawable());
         }
         else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
         {
            billBoardIcon = actors[i]->GetBillBoardIcon();
            if (billBoardIcon != NULL)
            {
               select(actors[i]->GetBillBoardIcon()->GetDrawable());
            }
            else
            {
               LOG_ERROR("BaseActorObject: " + actors[i]->GetName() + " has NULL billboard.");
            }

            select(actors[i]->GetDrawable());
         }
         else
         {
            select(actors[i]->GetDrawable());
         }

         mCurrentActorSelection.push_back(actors[i]);
      }

      dtCore::Map* map = EditorData::GetInstance().getCurrentMap();

      if (mCurrentActorSelection.size() <= 1)
      {
         EditorActions::GetInstance().mActionGroupActors->setEnabled(false);
      }
      else
      {
         bool enabled = true;
         if (map)
         {
            // Check the selection and make sure they are not already in the same group.
            enabled = false;
            int groupIndex = -2;
            for (int index = 0; index < (int)mCurrentActorSelection.size(); index++)
            {
               int testGroupIndex = map->FindGroupForActor(*mCurrentActorSelection[index].get());

               if (testGroupIndex != groupIndex)
               {
                  if (groupIndex == -2)
                  {
                     groupIndex = testGroupIndex;
                  }
                  else
                  {
                     enabled = true;
                     break;
                  }
               }
            }

            // If all the actors are not in a group, then we can group them.
            if (groupIndex == -1)
            {
               enabled = true;
            }
         }
         EditorActions::GetInstance().mActionGroupActors->setEnabled(enabled);
      }

      // Now add all the selected actions into a new group.
      bool canUngroup = false;
      if (map)
      {
         for (int index = 0; index < (int)mCurrentActorSelection.size(); index++)
         {
            dtCore::BaseActorObject* actor = mCurrentActorSelection[index].get();
            if (map->FindGroupForActor(*actor) != -1)
            {
               canUngroup = true;
               break;
            }
         }
      }

      EditorActions::GetInstance().mActionFileExportPrefab->setEnabled(true);
      EditorActions::GetInstance().mActionUngroupActors->setEnabled(canUngroup);
      EditorActions::GetInstance().mActionEditDeleteActor->setEnabled(true);
      EditorActions::GetInstance().mActionEditDuplicateActor->setEnabled(true);
      EditorActions::GetInstance().mActionEditGroundClampActors->setEnabled(true);
      EditorActions::GetInstance().mActionEditGotoActor->setEnabled(true);
      EditorActions::GetInstance().mActionEditResetTranslation->setEnabled(true);
      EditorActions::GetInstance().mActionEditResetRotation->setEnabled(true);
      EditorActions::GetInstance().mActionEditResetScale->setEnabled(true);
      ViewportManager::GetInstance().refreshAllViewports();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::select(dtCore::DeltaDrawable* drawable)
   {
      // HACK: Temporarily disable object highlighting until
      // it can be fixed to work well with actor hierarchies.
      if (true) return;

      //osg::StateSet* ss = mSelectionDecorator->getOrCreateStateSet();

      if (drawable == NULL || drawable->GetOSGNode() == NULL)
      {
         return;
      }

      if (mSelectionDecorator->containsNode(drawable->GetOSGNode()))
      {
         return;
      }

      //if this is the Brush, then make the selectionDecorator visible through
      //other objects
      dtActors::VolumeEditActor* volEditActTest = dynamic_cast<dtActors::VolumeEditActor*>(drawable);
      if (volEditActTest)
      {
         //ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
         volEditActTest->EnableOutline(false);
      }
      //else
      //{
      //   ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
      //   EditorData::GetInstance().getMainWindow()->GetVolumeEditActor()->EnableOutline(true);
      //}

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
   bool ViewportOverlay::isActorSelected(dtCore::BaseActorObject* proxy) const
   {
      for (int index = 0; index < (int)mCurrentActorSelection.size(); index++)
      {
         if (mCurrentActorSelection[index] == proxy)
         {
            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ViewportOverlay::isActorSelectedFirst(dtCore::BaseActorObject* proxy) const
   {
      if (mCurrentActorSelection.size() > 0)
      {
         if (mCurrentActorSelection[0] == proxy)
         {
            return true;
         }
      }

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::removeActorFromCurrentSelection(dtCore::BaseActorObject* proxy, bool clearAll)
   {
      int foundIndex = -1;
      for (int index = 0; index < (int)mCurrentActorSelection.size(); index++)
      {
         if (mCurrentActorSelection[index] == proxy)
         {
            foundIndex = index;
            break;
         }
      }

      if (foundIndex == -1)
      {
         return;
      }

      const dtCore::BaseActorObject::RenderMode& renderMode = proxy->GetRenderMode();
      dtCore::ActorProxyIcon* billBoardIcon = NULL;

      //Make sure we remove the correct drawable from the selection list depending
      //on the render mode of the actor.
      if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON)
      {
         billBoardIcon = proxy->GetBillBoardIcon();
         if (billBoardIcon != NULL)
         {
            unSelect(proxy->GetBillBoardIcon()->GetDrawable());
         }
         else
         {
            LOG_ERROR("BaseActorObject: " + proxy->GetName() + " has NULL billboard.");
         }
      }
      else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR)
      {
         unSelect(proxy->GetDrawable());
      }
      else if (renderMode == dtCore::BaseActorObject::RenderMode::DRAW_ACTOR_AND_BILLBOARD_ICON)
      {
         billBoardIcon = proxy->GetBillBoardIcon();
         if (billBoardIcon != NULL)
         {
            unSelect(proxy->GetBillBoardIcon()->GetDrawable());
         }
         else
         {
            LOG_ERROR("BaseActorObject: " + proxy->GetName() + " has NULL billboard.");
         }

         unSelect(proxy->GetDrawable());
      }
      else
      {
         unSelect(proxy->GetDrawable());
      }

      //Finally remove the actor proxy from the selection list.
      if (clearAll)
      {
         mCurrentActorSelection.erase(mCurrentActorSelection.begin() + foundIndex);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ViewportOverlay::clearCurrentSelection()
   {
      for (int index = 0; index < (int)mCurrentActorSelection.size(); index++)
      {
         removeActorFromCurrentSelection(const_cast<dtCore::BaseActorObject*>(mCurrentActorSelection[index].get()), false);
      }

      mCurrentActorSelection.clear();
   }

   ///////////////////////////////////////////////////////////////////////////////
   static const std::string COLOR_SHADER_FORMAT =
      "uniform sampler2D baseTexture;\
      void main (void)\
      {\
         vec4 finalColor = gl_Color * texture2D(baseTexture, gl_TexCoord[0].st);\
         gl_FragColor = vec4(%f, %f, %f, 1.0) * finalColor;\
      }";
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

         ss->setMode(GL_BLEND, osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
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

      connect(&ge,   SIGNAL(selectedActors(ActorRefPtrVector&)),
               this, SLOT(onActorsSelected(ActorRefPtrVector&)));

      connect(&ge,   SIGNAL(editorPreferencesChanged()),
               this, SLOT(onEditorPreferencesChanged()));
   }

} // namespace dtEditQt
