/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * David Guthrie
 */
#ifndef DELTA_HLACOMPONENTCONFIG
#define DELTA_HLACOMPONENTCONFIG

#include <dtCore/refptr.h>

#include <xercesc/sax2/SAX2XMLReader.hpp>
namespace xercesc_dt = XERCES_CPP_NAMESPACE;

#include <dtHLAGM/export.h>

namespace dtUtil
{
   class Log;
}

namespace dtHLAGM
{

   class HLAFOMConfigContentHandler;
   class HLAComponent;

   class DT_HLAGM_EXPORT HLAComponentConfig
   {
      public:
         /**
          * @throw dtUtil::Exception if creating or configuring the configurator fails.
          */
         HLAComponentConfig();
      	~HLAComponentConfig();
         
         /**
          * Populates the configuration of a translator based on the given file
          * @param translator the translator to populate
          * @param dataFilePath the path to the configuration file inside the delta3d data path.
          */
         void LoadConfiguration(HLAComponent& translator, const std::string& dataFilePath);
         
      private:
         dtCore::RefPtr<HLAFOMConfigContentHandler> mHandler;
         xercesc_dt::SAX2XMLReader* mXercesParser;
         dtCore::RefPtr<dtUtil::Log> mLogger;
         //The name of the logger
         static const std::string LOG_NAME;
   };

}

#endif /*DELTA_HLAFOMTRANSLATORCONFIG*/
