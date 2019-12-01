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
#include <dtUtil/mathdefines.h>

#include <xercesc/sax2/XMLReaderFactory.hpp>

XERCES_CPP_NAMESPACE_USE

namespace dtABC
{

   ApplicationConfigHandler::ApplicationConfigHandler()
   {
   }

   ApplicationConfigHandler::~ApplicationConfigHandler()
   {
   }
#if XERCES_VERSION_MAJOR < 3
   void ApplicationConfigHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
   void ApplicationConfigHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
   {
      if (mCurrentElement == ApplicationConfigSchema::LIBRARY_PATH)
      {
         std::string path(dtUtil::XMLStringConverter(chars).ToString());
         mConfigData.LIBRARY_PATHS.push_back(path);
      }
      else if (mCurrentElement == ApplicationConfigSchema::APP_PROPERTY)
      {
         mConfigData.mProperties.insert(std::make_pair(mPropertyName, dtUtil::XMLStringConverter(chars).ToString()));
      }
   }

   void ApplicationConfigHandler::endDocument() {}
   void ApplicationConfigHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname)
   {
      mPropertyName.clear();
   }

#if XERCES_VERSION_MAJOR < 3
   void ApplicationConfigHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
#else
   void ApplicationConfigHandler::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length) {}
#endif

   void ApplicationConfigHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
   void ApplicationConfigHandler::setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
   void ApplicationConfigHandler::startDocument() {}
   void ApplicationConfigHandler::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri) {}
   void ApplicationConfigHandler::endPrefixMapping(const XMLCh* const prefix) {}
   void ApplicationConfigHandler::skippedEntity(const XMLCh* const name) {}

   ///\todo optimize out string and data copies by just using the default applicatioinconfigdata struct
   void ApplicationConfigHandler::startElement(const XMLCh* const uri,
                                               const XMLCh* const localname,
                                               const XMLCh* const qname,
                                               const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
   {
      dtUtil::XMLStringConverter stringConv(localname);

      mCurrentElement = stringConv.ToString();

      if (mCurrentElement == ApplicationConfigSchema::WINDOW)
      {
         // set up some defaults
         std::string name("DefaultApp"),
                     xstring("100"), ystring("100"),
                     wstring("640"), hstring("480"),
                     showstring("1"), fullstring("0"), realizestring("1"),
                     changeres("0"), bitdepth("24"), refresh("60"), vsyncString("true"), hideConsole("false"), multiSampleString("0");

         // search for specific named attributes, append the list of searched strings
         dtUtil::AttributeSearch windowattrs;

         // perform the search
         typedef dtUtil::AttributeSearch::ResultMap RMap;
         RMap results = windowattrs(attrs);

         // if the named attributes were found, use them, else use defaults
         RMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if (iter != results.end())
            name = iter->second;

         iter = results.find(ApplicationConfigSchema::X);
         if (iter != results.end())
            xstring = iter->second;

         iter = results.find(ApplicationConfigSchema::Y);
         if (iter != results.end())
            ystring = iter->second;

         iter = results.find(ApplicationConfigSchema::WIDTH);
         if (iter != results.end())
            wstring = iter->second;

         iter = results.find(ApplicationConfigSchema::HEIGHT);
         if (iter != results.end())
            hstring = iter->second;

         iter = results.find(ApplicationConfigSchema::SHOWCURSOR);
         if (iter != results.end())
            showstring = iter->second;

         iter = results.find(ApplicationConfigSchema::FULLSCREEN);
         if (iter != results.end())
            fullstring = iter->second;

         iter = results.find(ApplicationConfigSchema::REALIZE_UPON_CREATE);
         if (iter != results.end())
            realizestring = iter->second;

         iter = results.find(ApplicationConfigSchema::CHANGEDISPLAYRESOLUTION);
         if ( iter != results.end() )
            changeres = iter->second;

         iter = results.find(ApplicationConfigSchema::PIXELDEPTH);
         if (iter != results.end())
            bitdepth = iter->second;

         iter = results.find(ApplicationConfigSchema::REFRESHRATE);
         if (iter != results.end())
            refresh = iter->second;

         iter = results.find(ApplicationConfigSchema::VSYNC);
         if (iter != results.end())
            vsyncString = iter->second;

         iter = results.find(ApplicationConfigSchema::HIDE_WINDOWS_CONSOLE);
         if (iter != results.end())
            hideConsole = iter->second;

         iter = results.find(ApplicationConfigSchema::MULTI_SAMPLE);
         if (iter != results.end())
            multiSampleString = iter->second;

         int winX = dtUtil::ToType<int>(xstring);
         int winY = dtUtil::ToType<int>(ystring);
         int width = dtUtil::ToType<int>(wstring);
         int height = dtUtil::ToType<int>(hstring);
         bool showCursor = dtUtil::ToType<bool>(showstring);
         bool fullScreen = dtUtil::ToType<bool>(fullstring);
         bool realizeUponCreate = dtUtil::ToType<bool>(realizestring);
         bool resolution_changed = dtUtil::ToType<bool>(changeres);
         int depth = dtUtil::ToType<int>(bitdepth);
         int rate = dtUtil::ToType<int>(refresh);
         bool vsync = dtUtil::ToType<bool>(vsyncString);
         bool hideCon = dtUtil::ToType<bool>(hideConsole);
         int multiSample = dtUtil::ToType<int>(multiSampleString);

         dtUtil::Clamp(multiSample, 0, 128);

         mConfigData.WINDOW_NAME = name;
         mConfigData.RESOLUTION.width = width;
         mConfigData.RESOLUTION.height = height;
         mConfigData.RESOLUTION.bitDepth = depth;
         mConfigData.RESOLUTION.refresh = rate;
         mConfigData.WINDOW_X = winX;
         mConfigData.WINDOW_Y = winY;
         mConfigData.SHOW_CURSOR = showCursor;
         mConfigData.FULL_SCREEN = fullScreen;
         mConfigData.REALIZE_UPON_CREATE = realizeUponCreate;
         mConfigData.CHANGE_RESOLUTION = resolution_changed;
         mConfigData.VSYNC = vsync;
         mConfigData.HIDE_WINDOWS_CONSOLE = hideCon;
         mConfigData.MULTI_SAMPLE = multiSample;
      }
      else if (mCurrentElement == ApplicationConfigSchema::SCENE)
      {
         dtUtil::AttributeSearch sceneattrs;
         dtUtil::AttributeSearch::ResultMap results = sceneattrs(attrs);

         dtUtil::AttributeSearch::ResultMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if (iter != results.end())
         {
            mConfigData.SCENE_NAME = iter->second;
         }
      }
      else if (mCurrentElement == ApplicationConfigSchema::VIEW)
      {
         dtUtil::AttributeSearch sceneattrs;
         dtUtil::AttributeSearch::ResultMap results = sceneattrs(attrs);

         dtUtil::AttributeSearch::ResultMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if (iter != results.end())
         {
            mConfigData.VIEW_NAME = iter->second;
         }

         iter = results.find(ApplicationConfigSchema::CAMERAINSTANCE);
         if (iter != results.end())
         {
             mConfigData.CAMERA_INSTANCE = iter->second;
         }
         else
         {
             LOG_ERROR("No Camera Instance specified for View, " + mConfigData.VIEW_NAME)
         }

         iter = results.find(ApplicationConfigSchema::SCENEINSTANCE);
         if (iter != results.end())
         {
             mConfigData.SCENE_INSTANCE = iter->second;
         }
         else
         {
             LOG_ERROR("No Scene Instance specified for View, " + mConfigData.VIEW_NAME)
         }
      }
      else if (mCurrentElement == ApplicationConfigSchema::CAMERA)
      {
         // push some keys
         dtUtil::AttributeSearch camattrs;

         // do the attribute search, catch the results
         dtUtil::AttributeSearch::ResultMap results = camattrs(attrs);

         dtUtil::AttributeSearch::ResultMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if (iter != results.end())
         {
            mConfigData.CAMERA_NAME = iter->second;
         }

         iter = results.find(ApplicationConfigSchema::WINDOWINSTANCE);
         if (iter != results.end())
         {
            mConfigData.WINDOW_INSTANCE = iter->second;
         }
         else
         {
            LOG_ERROR("No DeltaWin instance speficied for Camera, " + mConfigData.CAMERA_NAME)
         }

         iter = results.find(ApplicationConfigSchema::SCENEINSTANCE);
         if (iter != results.end())
         {
//            mConfigData.SCENE_INSTANCE = iter->second;
            LOG_ERROR("DEPRECIATED : Scene Instance specified for Camera " + mConfigData.CAMERA_NAME + "must be specified for View")
         }
//         else
//         {
//            LOG_ERROR("No Scene Instance specified for Camera, " + mConfigData.CAMERA_NAME)
//         }
      }
      else if (mCurrentElement == ApplicationConfigSchema::VIEWPORT)
      {
         // push some keys
         dtUtil::AttributeSearch vpAttrs;

         // do the attribute search, catch the results
         dtUtil::AttributeSearch::ResultMap results = vpAttrs(attrs);

         dtUtil::AttributeSearch::ResultMap::iterator iter = results.find(ApplicationConfigSchema::VIEWPORT_X);
         if (iter != results.end())
         {
            mConfigData.VIEWPORT_X = dtUtil::ToType<int>(iter->second);
         }

         iter = results.find(ApplicationConfigSchema::VIEWPORT_Y);
         if (iter != results.end())
         {
            mConfigData.VIEWPORT_Y = dtUtil::ToType<int>(iter->second);
         }

         iter = results.find(ApplicationConfigSchema::VIEWPORT_WIDTH);
         if (iter != results.end())
         {
            mConfigData.VIEWPORT_W = dtUtil::ToType<int>(iter->second);
         }

         iter = results.find(ApplicationConfigSchema::VIEWPORT_HEIGHT);
         if (iter != results.end())
         {
            mConfigData.VIEWPORT_H = dtUtil::ToType<int>(iter->second);
         }
      }
      else if (mCurrentElement == ApplicationConfigSchema::LOG)
      {
         // push some keys
         dtUtil::AttributeSearch logAttrs;

         // do the attribute search, catch the results
         dtUtil::AttributeSearch::ResultMap results = logAttrs( attrs );

         dtUtil::AttributeSearch::ResultMap::iterator iter;

         iter = results.find(ApplicationConfigSchema::NAME);
         std::string name;
         if ( iter != results.end() )
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
      else if (mCurrentElement == ApplicationConfigSchema::GLOBAL_LOG_LEVEL)
      {
         // push some keys
         dtUtil::AttributeSearch logAttrs;

         // do the attribute search, catch the results
         dtUtil::AttributeSearch::ResultMap results = logAttrs(attrs);

         dtUtil::AttributeSearch::ResultMap::iterator iter;
         iter = results.find(ApplicationConfigSchema::LOG_LEVEL);
         if (iter != results.end())
         {
            mConfigData.GLOBAL_LOG_LEVEL = iter->second;
         }
         else
         {
            LOG_WARNING("No global log level defined, the default value will be used.");
         }
      }
      else if (mCurrentElement == ApplicationConfigSchema::APP_PROPERTY)
      {
         // push some keys
         dtUtil::AttributeSearch propAttrs;

         // do the attribute search, catch the results
         dtUtil::AttributeSearch::ResultMap results = propAttrs(attrs);

         dtUtil::AttributeSearch::ResultMap::iterator iter = results.find(ApplicationConfigSchema::NAME);
         if (iter != results.end())
         {
            mPropertyName = iter->second;
         }

      }
   }
}
