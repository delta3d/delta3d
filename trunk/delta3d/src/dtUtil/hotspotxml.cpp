#include <prefix/dtutilprefix-src.h>
#include <dtUtil/hotspotxml.h>

#include <dtUtil/xercesutils.h>
#include <cstddef>
#include <sstream>

namespace dtUtil
{

const osg::Vec3 HotSpotFileHandler::HEADING_VEC(0,0,1);
const osg::Vec3 HotSpotFileHandler::PITCH_VEC(1,0,0);
const osg::Vec3 HotSpotFileHandler::ROLL_VEC(0,1,0);

const char HotSpotFileHandler::HOT_SPOT_NODE_NAME[] = { "HotSpot\0" };
const char HotSpotFileHandler::NAME_ATTRIBUTE_NAME[] = { "name\0" };
const char HotSpotFileHandler::DEFAULT_VALUE[] = { "default\0" };
const char HotSpotFileHandler::HOT_SPOT_PARENT_NODE_NAME[] = { "Parent\0" };
const char HotSpotFileHandler::LOCAL_TRANSLATION_NODE_NAME[] = { "LocalTranslation\0" };
const char HotSpotFileHandler::LOCAL_ROTATION_NODE_NAME[] = { "LocalRotation\0" };

void HotSpotFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
                                       const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   dtUtil::AttributeSearch::ResultMap::iterator resultIter;

   HotSpotNode currentNode(NODE_UNKNOWN);

   if( elementStr == HOT_SPOT_NODE_NAME )
   {
      currentNode = NODE_HOT_SPOT;

      mCurrentData = HotSpotDefinition();
      results = search(attrs);
      resultIter = results.find(NAME_ATTRIBUTE_NAME);
      if( resultIter != results.end() )
      {
         mCurrentData.mName = resultIter->second;
      }
      else
      {
         mCurrentData.mName = DEFAULT_VALUE;
      }
   }
   else if( elementStr == HOT_SPOT_PARENT_NODE_NAME )
   {
      currentNode = NODE_HOT_SPOT_PARENT;
   }
   else if( elementStr == LOCAL_TRANSLATION_NODE_NAME )
   {
      currentNode = NODE_HOT_SPOT_LOCAL_TRANSLATION;
   }
   else if( elementStr == LOCAL_ROTATION_NODE_NAME )
   {
      currentNode = NODE_HOT_SPOT_LOCAL_ROTATION;
   }

   mNodeStack.push( currentNode );
}

void HotSpotFileHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname)
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   if( elementStr == HOT_SPOT_NODE_NAME )
   {
      mHotSpots.push_back( mCurrentData );
   }

   mNodeStack.pop();
}

void HotSpotFileHandler::characters(const XMLCh* const chars, const unsigned int length)
{
   switch( mNodeStack.top() )
   {
   case NODE_HOT_SPOT_PARENT:
      {
         dtUtil::XMLStringConverter data(chars);
         mCurrentData.mParentName = data.ToString();
      } break;

   case NODE_HOT_SPOT_LOCAL_TRANSLATION:
      {
         dtUtil::XMLStringConverter data(chars);
         std::istringstream iss;
         iss.str( data.ToString() );
         float x, y, z;
         iss >> x >> y >> z;
         mCurrentData.mLocalTranslation.set(x,y,z);
      } break;

   case NODE_HOT_SPOT_LOCAL_ROTATION:
      {
         dtUtil::XMLStringConverter data(chars);
         std::istringstream iss;
         iss.str( data.ToString() );
         float heading, pitch, roll;
         iss >> heading >> pitch >> roll;

         mCurrentData.mLocalRotation = osg::Quat(osg::DegreesToRadians(heading), HEADING_VEC,
                                                 osg::DegreesToRadians(pitch), PITCH_VEC,
                                                 osg::DegreesToRadians(roll), ROLL_VEC);
      } break;

   case NODE_UNKNOWN:
   default:
      {
      } break;
   }
}

HotSpotFileHandler::HotSpotFileHandler()
   : mCurrentData()
   , mHotSpots()
   , mNodeStack()
{
}

HotSpotFileHandler::~HotSpotFileHandler()
{
}

HotSpotFileHandler::HotSpotDefinitionVector& HotSpotFileHandler::GetData()
{
   return mHotSpots;
}

}

