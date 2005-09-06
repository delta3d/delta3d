#ifndef DELTA_KEY_FRAME_HANDLER_INC
#define DELTA_KEY_FRAME_HANDLER_INC

#include <xercesc/sax2/ContentHandler.hpp>
#include <vector>
#include <string>
#include <map>

namespace dtUtil
{
   /** \brief A class used for filling KeyFrames, enforces a schema.
      * KeyFrames
      *    KeyFrame
      *       Source
      *          FrameData
      *             InternalData
      *                PotentiallyMoreInternalData
      */
   template<typename RecordableT,typename FrameDataT>
   class KeyFrameHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
   public:
      typedef RecordableT                                   RecordableType;
      typedef FrameDataT                                    FrameDataType;
      typedef std::vector< dtCore::RefPtr<RecordableType> > RecordablePtrContainer;
      typedef std::vector<FrameDataType>                    FrameDataContainer;     /// A container to hold each source's frame data.
      typedef std::pair<double,FrameDataContainer>          KeyFrame;               /// The time stamp applied to the entire container of frame data.
      typedef std::vector<KeyFrame>                         KeyFrameContainer;      /// The container of KeyFrame data.

   private:
      /** A base interface class for the state of the parser.
        * Derived classes can be used to control that actions of the parser.
        */
      class ActionState : public osg::Referenced
      {
      public:
         ActionState() {}
         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)=0;
         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)=0;

      protected:
         virtual ~ActionState() {}
      };

      /** Used to control the parsing for sets of key frames.
        */
      class KeyFramesState : public ActionState
      {
      public:
         KeyFramesState() {}

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
         }

      protected:
         virtual ~KeyFramesState() {}
      };

      /** Used to control the parsing for of key frames.
        */
      class KeyFrameState : public ActionState
      {
      public:
         KeyFrameState() {}

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
         }

      protected:
         virtual ~KeyFrameState() {}
      };

      /** Used to control the parsing for a source.
        */
      class SourceState : public ActionState
      {
      public:
         SourceState() {}

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
         }

      protected:
         virtual ~SourceState() {}
      };

      /** Used to control the parsing for the source's frame data.
        */
      class FrameDataState : public ActionState
      {
      public:
         FrameDataState() {}

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
         }

      protected:
         virtual ~FrameDataState() {}
      };

   public:
      typedef XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler BaseClass;
      KeyFrameHandler(const RecordablePtrContainer& sources,KeyFrameContainer& kfc): BaseClass(),
         mSources(sources), mKFC(kfc), mCurrentState(0), mActionStateMap()
      {
         mActionStateMap.insert( ActionStateMap::value_type( "KeyFrames", new KeyFramesState() ) );
         mActionStateMap.insert( ActionStateMap::value_type( "KeyFrame", new KeyFrameState() ) );
         mActionStateMap.insert( ActionStateMap::value_type( "Source", new SourceState() ) );
         mActionStateMap.insert( ActionStateMap::value_type( "FrameData", new FrameDataState() ) );
      }

      virtual ~KeyFrameHandler()
      {
      }

      // inherited pure virtual functions
      virtual void startDocument() {}
      virtual void endDocument() {}
      virtual void characters(const XMLCh* const chars, const unsigned int length) {}
      virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {}
      virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data) {}
      virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {}
      virtual void startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri) {}
      virtual void endPrefixMapping(const XMLCh* const prefix) {}
      virtual void skippedEntity(const XMLCh* const name) {}

      virtual void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
      {
         mLevel++;
         SetActionState( mLevel );
         if( mCurrentState )
            mCurrentState->HandleStart( uri, localname, qname, attrs );
      }

      virtual void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname)
      {
         if( mCurrentState )
            mCurrentState->HandleEnd( uri, localname, qname );
         mLevel--;
      }

   protected:
      void SetActionState(unsigned int action)
      {
         switch( mLevel )
         {
         case 0:  // new document
            {
               ///\todo log the new document parsing started
               mCurrentState = 0;
            } break;

         case 1:  // top level tag, keyframes
            {
               ActionStateMap::iterator iter = mActionStateMap.find("KeyFrames");
               if( iter != mActionStateMap.end() )
                  mCurrentState = (*iter).second.get();
            } break;

         case 2:  // a single keyframe tag
            {
               ActionStateMap::iterator iter = mActionStateMap.find("KeyFrame");
               if( iter != mActionStateMap.end() )
                  mCurrentState = (*iter).second.get();
            } break;

         case 3:
            {
               ActionStateMap::iterator iter = mActionStateMap.find("Source");
               if( iter != mActionStateMap.end() )
                  mCurrentState = (*iter).second.get();
            } break;

         case 4:
         default:
            {
               ActionStateMap::iterator iter = mActionStateMap.find("FrameData");
               if( iter != mActionStateMap.end() )
                  mCurrentState = (*iter).second.get();
            } break;
         };
      }

   private:
      const RecordablePtrContainer& mSources;
      KeyFrameContainer mKFC;
      unsigned int mLevel;
      typedef std::map<std::string, osg::ref_ptr<ActionState> > ActionStateMap;
      ActionStateMap mActionStateMap;
      ActionState* mCurrentState;
   }; // end KeyFrameHandler

}; // end namespace dtUtil

#endif // DELTA_KEY_FRAME_HANDLER_INC
