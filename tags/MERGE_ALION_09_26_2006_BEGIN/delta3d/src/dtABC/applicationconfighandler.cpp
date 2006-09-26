/* -*-c++-*- 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
 * John K. Grant
 * David Guthrie
 */
#include <dtABC/applicationconfighandler.h>

#include <dtABC/applicationconfigschema.h>
#include <dtUtil/stringutils.h>
#include <dtUtil/log.h>
#include <dtUtil/xercesutils.h>

#include <xercesc/sax2/XMLReaderFactory.hpp>

// includes necessary to use dtABC::Application
#include <dtABC/application.h>
#include <dtCore/keyboard.h>
#include <dtCore/generickeyboardlistener.h>
#include <dtCore/mouse.h>

#include <dtCore/camera.h>
#include <dtCore/scene.h>

XERCES_CPP_NAMESPACE_USE;
namespace dtABC
{

   ApplicationConfigHandler::ApplicationConfigHandler()
   {
   }
   
   ApplicationConfigHandler::~ApplicationConfigHandler()
   {
   }
   
   void ApplicationConfigHandler::characters(const XMLCh* const chars, const unsigned int length) 
   {
      if (mCurrentElement == ApplicationConfigSchema::LIBRARY_PATH)
      {
         std::string path(dtUtil::XMLStringConverter(chars).ToString());
         mConfigData.LIBRARY_PATHS.push_back(path);
      }
   }
   
   void ApplicationConfigHandler::endDocument() {}
   void ApplicationConfigHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname) {}
   void ApplicationConfigHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
   void ApplicationConfigHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
   void ApplicationConfigHandler::setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
   void ApplicationConfigHandler::startDocument() {}
   void ApplicationConfigHandler::startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri) {}
   void ApplicationConfigHandler::endPrefixMapping(const XMLCh* const prefix) {}
   void ApplicationConfigHandler::skippedEntity(const XMLCh* const name) {}
   
   ///\todo use ApplicationConfigSchema for attribute and node name searches.
   ///\todo optimize out string and data copies by just using the default applicatioinconfigdata struct
   void ApplicationConfigHandler::startElement(const XMLCh* const uri,
                                               const XMLCh* const localname,
                                               const XMLCh* const qname,
                                               const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
   {
      dtUtil::XMLStringConverter stringConv(localname);
      
      mCurrentElement = stringConv.ToString();
         
      if( mCurrentElement == ApplicationConfigSchema::WINDOW )
      {
         // set up some defaults
         std::string name("DefaultApp"),
                     xstring("100"), ystring("100"),
                     wstring("640"), hstring("480"),
                     showstring("1"), fullstring("0"),
                     changeres("0"), bitdepth("24"), refresh("60");
   
         // search for specific named attributes, append the list of searched strings
         dtUtil::AttributeSearch windowattrs;
   
         // perform the search
         typedef dtUtil::AttributeSearch::ResultMap RMap;
         RMap results = windowattrs( attrs );
   
         // if the named attributes were found, use them, else use defaults
         RMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if( iter != results.end() )
            name = iter->second;
   
         iter = results.find(ApplicationConfigSchema::X);
         if( iter != results.end() )
            xstring = iter->second;
   
         iter = results.find(ApplicationConfigSchema::Y);
         if( iter != results.end() )
            ystring = iter->second;
   
         iter = results.find(ApplicationConfigSchema::WIDTH);
         if( iter != results.end() )
            wstring = iter->second;
   
         iter = results.find(ApplicationConfigSchema::HEIGHT);
         if( iter != results.end() )
            hstring = iter->second;
   
         iter = results.find(ApplicationConfigSchema::SHOWCURSOR);
         if( iter != results.end() )
            showstring = iter->second;
   
         iter = results.find(ApplicationConfigSchema::FULLSCREEN);
         if( iter != results.end() )
            fullstring = iter->second;
   
         iter = results.find(ApplicationConfigSchema::CHANGEDISPLAYRESOLUTION);
         if( iter != results.end() )
            changeres = iter->second;
            
         iter = results.find(ApplicationConfigSchema::PIXELDEPTH);
         if( iter != results.end() )
            bitdepth = iter->second;
   
         iter = results.find(ApplicationConfigSchema::REFRESHRATE);
         if( iter != results.end() )
            refresh = iter->second;
            
         int winX = dtUtil::ToType<int>(xstring.c_str());
         int winY = dtUtil::ToType<int>(ystring.c_str());
         int width = dtUtil::ToType<int>(wstring.c_str());
         int height = dtUtil::ToType<int>(hstring.c_str());
         bool showCursor = dtUtil::ToType<bool>(showstring.c_str());
         bool fullScreen = dtUtil::ToType<bool>(fullstring.c_str());
         bool resolution_changed = dtUtil::ToType<bool>(changeres.c_str());
         int depth = dtUtil::ToType<int>(bitdepth.c_str());
         int rate = dtUtil::ToType<int>(refresh.c_str());
   
         mConfigData.WINDOW_NAME = name;
         mConfigData.RESOLUTION.width = width;
         mConfigData.RESOLUTION.height = height;
         mConfigData.RESOLUTION.bitDepth = depth;
         mConfigData.RESOLUTION.refresh = rate;
         mConfigData.WINDOW_X = winX;
         mConfigData.WINDOW_Y = winY;
         mConfigData.SHOW_CURSOR = showCursor;
         mConfigData.FULL_SCREEN = fullScreen;
         mConfigData.CHANGE_RESOLUTION = resolution_changed;
      }
      else if( mCurrentElement == ApplicationConfigSchema::SCENE )
      {
         dtUtil::AttributeSearch sceneattrs;
         dtUtil::AttributeSearch::ResultMap results = sceneattrs( attrs );
   
         dtUtil::AttributeSearch::ResultMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if( iter != results.end() )
         {
            mConfigData.SCENE_NAME = iter->second;
         }
      }
      else if( mCurrentElement == ApplicationConfigSchema::CAMERA )
      {
         // push some keys
         dtUtil::AttributeSearch camattrs;
   
         // do the attribute search, catch the results
         dtUtil::AttributeSearch::ResultMap results = camattrs( attrs );
   
         dtUtil::AttributeSearch::ResultMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if( iter != results.end() )
         {
            mConfigData.CAMERA_NAME = iter->second;
         }
   
         iter = results.find(ApplicationConfigSchema::WINDOWINSTANCE);
         if( iter != results.end() )
         {
            mConfigData.WINDOW_INSTANCE = iter->second;
         }
         else
         {
            LOG_ERROR("No DeltaWin instance speficied for Camera, " + mConfigData.CAMERA_NAME)
         }
   
         iter = results.find(ApplicationConfigSchema::SCENEINSTANCE);
         if( iter != results.end() )
         {
            mConfigData.SCENE_INSTANCE = iter->second;
         }
         else
         {
            LOG_ERROR("No Scene Instance specified for Camera, " + mConfigData.CAMERA_NAME)
         }
      }
      else if ( mCurrentElement == ApplicationConfigSchema::LOG )
      {
         // push some keys
         dtUtil::AttributeSearch logAttrs;
   
         // do the attribute search, catch the results
         dtUtil::AttributeSearch::ResultMap results = logAttrs( attrs );
         
         dtUtil::AttributeSearch::ResultMap::iterator iter;
   
         iter = results.find(ApplicationConfigSchema::NAME);
         std::string name;
         if( iter != results.end() )
         {
            name = iter->second;
         }
         else
         {
            name = dtUtil::Log::GetInstance().GetName();
         }
         
         iter = results.find(ApplicationConfigSchema::LOG_LEVEL);
         if ( iter != results.end() )
         {
            mConfigData.LOG_LEVELS.insert(std::make_pair(name, iter->second));
         }
         else
         {
            LOG_WARNING("No level defined for log named \"" + name + "\", the default value will be used.");
         }  
      }     
   }
}
