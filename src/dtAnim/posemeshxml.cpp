#include <dtAnim/PoseMeshXML.h>
#include <dtAnim/PoseMesh.h>

#include <dtUtil/xercesutils.h>
#include <cstddef>
#include <cassert>
#include <sstream>

using namespace dtAnim;

const char PoseMeshFileHandler::POSE_NODE_NAME[]         = { "PoseMesh\0" };
const char PoseMeshFileHandler::BONE_ATTRIBUTE_NAME[]    = { "bone\0" };
const char PoseMeshFileHandler::NAME_ATTRIBUTE_NAME[]    = { "name\0" };
const char PoseMeshFileHandler::FORWARD_ATTRIBUTE_NAME[] = { "forward\0" };
const char PoseMeshFileHandler::DEFAULT_VALUE[]          = { "default\0" };
const char PoseMeshFileHandler::TRIANGLE_NODE_NAME[]     = { "Triangle\0" };
const char PoseMeshFileHandler::ANIMATION_NODE_NAME[]    = { "Animation\0" };

/////////////////////////////////////////////////////////////////////////////////////////
PoseMeshFileHandler::PoseMeshFileHandler()
: mMeshDataList()
, mCurrentData()
, mNodeStack()
, mTriangleAnimations()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
PoseMeshFileHandler::~PoseMeshFileHandler()
{
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshFileHandler::startElement( const XMLCh* const uri,const XMLCh* const localname,
                                             const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs )
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   dtUtil::AttributeSearch search;
   dtUtil::AttributeSearch::ResultMap results;
   dtUtil::AttributeSearch::ResultMap::iterator resultIter;

   PoseNode currentNode(NODE_UNKNOWN);

   if( elementStr == POSE_NODE_NAME )
   {
      currentNode = NODE_CELESTIAL_MESH;      

      results    = search(attrs);
      resultIter = results.find(NAME_ATTRIBUTE_NAME);      

      // Get the name attribute
      if( resultIter != results.end())
      {
         mCurrentData.mName = resultIter->second;
      }
      else
      {
         mCurrentData.mName = DEFAULT_VALUE;
      }
      
      // Get the bone attribute
      resultIter = results.find(BONE_ATTRIBUTE_NAME);
      assert( resultIter != results.end() );

      if( resultIter != results.end() )
      {
         mCurrentData.mBoneName = resultIter->second;
      }     

      // Get the forward direction
      resultIter = results.find(FORWARD_ATTRIBUTE_NAME);
      assert( resultIter != results.end() );

      if( resultIter != results.end() )
      {
         std::istringstream iss(resultIter->second);
         float xAxis, yAxis, zAxis;

         iss >> xAxis >> yAxis >> zAxis;
         mCurrentData.mForward.set(xAxis, yAxis, zAxis);         
      }     
   }
   else if( elementStr == TRIANGLE_NODE_NAME )
   {      
      currentNode = NODE_TRIANGLE;
   }
   else if( elementStr == ANIMATION_NODE_NAME )
   {
      currentNode = NODE_ANIMATION;
   }   

   mNodeStack.push( currentNode );
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshFileHandler::endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname)
{
   dtUtil::XMLStringConverter elementName(localname);
   const std::string elementStr = elementName.ToString();

   if( elementStr == POSE_NODE_NAME )
   {
      mMeshDataList.push_back( mCurrentData );
      mCurrentData.mAnimations.clear();
   }
   else if ( elementStr == TRIANGLE_NODE_NAME )
   {
      assert((mCurrentData.mAnimations.size() % 3) == 0);      
   }

   mNodeStack.pop();
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshFileHandler::characters(const XMLCh* const chars, const unsigned int length)
{
   switch( mNodeStack.top() )
   { 
      case NODE_ANIMATION:
      {         
         dtUtil::XMLStringConverter data(chars);
         mCurrentData.mAnimations.push_back(data.ToString());        

      } break;

      case NODE_UNKNOWN:
      default:
      {
      } break;
   }
}


