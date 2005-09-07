#ifndef DELTA_KEY_FRAME_HANDLER_INC
#define DELTA_KEY_FRAME_HANDLER_INC

#include <xercesc/sax2/ContentHandler.hpp>
#include <vector>
#include <string>
#include <map>
#include <stack>

namespace dtUtil
{
   /** \brief A class used for filling KeyFrames, enforces a schema.
     * Typically used by the Recorder class.
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
        * Each derived class will probably have behavior very unique from the other derived classes.
        */
      class ParserState : public osg::Referenced
      {
      public:
         ParserState() {}
         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)=0;
         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)=0;

      protected:
         virtual ~ParserState() {}
      };

      /** Used to control the parsing for sets of key frames.
        */
      class FrameContainerState : public ParserState
      {
      public:
         FrameContainerState(KeyFrameContainer& kfc): mKFC(kfc)
         {
         }

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
            LOG_ALWAYS( std::string("uri = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(uri)) )
            LOG_ALWAYS( std::string("local name = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname)) )
            LOG_ALWAYS( std::string("qname = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(qname)) )
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
         }

         KeyFrameContainer& GetContainer() { return mKFC; }

      protected:
         virtual ~FrameContainerState() {}

      private:
         FrameContainerState(); /// not implemented by design
         KeyFrameContainer& mKFC;
      };

      /** Used to control the parsing for of key frames.
        */
      class KeyFrameState : public ParserState
      {
      public:
         KeyFrameState(FrameContainerState* fcs): mFCS(fcs), mTimeCodeName("TimeCode"), mTimeStamp(0.0)
         {
         }

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
            ///\todo clear the mFDC ? (voting yes)
            mFDC.clear();

            LOG_ALWAYS( std::string("uri = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(uri)) )
            LOG_ALWAYS( std::string("local name = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname)) )
            LOG_ALWAYS( std::string("qname = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(qname)) )

            // try to find the time stamp
            bool FOUND_TIMESTAMP(false);
            unsigned int asize = attrs.getLength();
            for(unsigned int i=0; i<asize; i++)
            {
               std::string an = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode( attrs.getLocalName(i) );
               if( an == mTimeCodeName )
               {
                  FOUND_TIMESTAMP = true;
                  std::string av = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode( attrs.getValue(i) );
                  mTimeStamp = dtUtil::ToFloat( av.c_str() );
                  LOG_INFO("Time stamp = " + av)
               }
            }

            if( !FOUND_TIMESTAMP )
            {
               LOG_WARNING("Time stamp attribute by name," + mTimeCodeName + " was not found.")
            }
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
            // do something with the mTimeStamp
            mFCS->GetContainer().push_back( KeyFrameContainer::value_type(mTimeStamp,mFDC) );
         }

         FrameDataContainer& GetContainer() { return mFDC; }

      protected:
         virtual ~KeyFrameState() {}

      private:
         KeyFrameState();  /// not implemented by design
         osg::ref_ptr<FrameContainerState> mFCS;
         std::string mTimeCodeName;
         double mTimeStamp;
         FrameDataContainer mFDC;
      };

      /** Used to control the parsing for a source.
        */
      class SourceState : public ParserState
      {
      public:
         SourceState(KeyFrameState* kfs): mKFS(kfs), mSource(0)
         {
         }

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
            ///\todo find a way to clear off the dom element, maybe make a new one and leak the old one?
            //mDOMElement->clear();

            LOG_ALWAYS( std::string("uri = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(uri)) )
            LOG_ALWAYS( std::string("local name = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname)) )
            LOG_ALWAYS( std::string("qname = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(qname)) )
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
            mKFS->GetContainer().push_back( RecordableType::DeSerialize( mDOMElement ) );
         }

         void SetSource(RecordableT* s) { mSource = s; }

      protected:
         virtual ~SourceState() {}

      private:
         SourceState();  /// not implemented by design
         osg::ref_ptr<KeyFrameState> mKFS;
         RecordableT* mSource;
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* mDOMElement;
      };

      /** Used to control the parsing for the source's frame data.
        * It builds a DOMElement consisting of FrameData contents, to be de-serialized by the higher level node.
        */
      class FrameDataState : public ParserState
      {
      public:
         FrameDataState(SourceState* ss): mSS(ss)
         {
         }

         virtual void HandleStart(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs)
         {
            ///\todo modify mParent

            LOG_ALWAYS( std::string("uri = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(uri)) )
            LOG_ALWAYS( std::string("local name = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(localname)) )
            LOG_ALWAYS( std::string("qname = ") + std::string(XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(qname)) )
         }

         virtual void HandleEnd(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
         {
         }

      protected:
         virtual ~FrameDataState() {}

      private:
         FrameDataState();  /// not implemented by design
         osg::ref_ptr<SourceState> mSS;
         XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* mParent;
      };

   public:
      typedef XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler BaseClass;
      KeyFrameHandler(const RecordablePtrContainer& sources,KeyFrameContainer& kfc): BaseClass(),
         mSources(sources), mLevel(0), mCurrentState(0), mParserStateMap()
      {
         // allocate the different parser states
         osg::ref_ptr<FrameContainerState> fcs = new FrameContainerState(kfc);
         osg::ref_ptr<KeyFrameState> kfs = new KeyFrameState( fcs.get() );
         osg::ref_ptr<SourceState> ss = new SourceState( kfs.get() );
         osg::ref_ptr<FrameDataState> fds = new FrameDataState( ss.get() );

         // stuff the states into a map
         mParserStateMap.insert( ParserStateMap::value_type( "FrameContainer", fcs.get() ) );
         mParserStateMap.insert( ParserStateMap::value_type( "KeyFrame", kfs.get() ) );
         mParserStateMap.insert( ParserStateMap::value_type( "Source", ss.get() ) );
         mParserStateMap.insert( ParserStateMap::value_type( "FrameData", fds.get() ) );
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
         SetParserState( mLevel );
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
      void SetParserState(unsigned int action)
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
               ParserStateMap::iterator iter = mParserStateMap.find("FrameContainer");
               if( iter != mParserStateMap.end() )
                  mCurrentState = (*iter).second.get();
            } break;

         case 2:  // a single keyframe tag
            {
               ParserStateMap::iterator iter = mParserStateMap.find("KeyFrame");
               if( iter != mParserStateMap.end() )
                  mCurrentState = (*iter).second.get();
            } break;

         case 3:
            {
               ParserStateMap::iterator iter = mParserStateMap.find("Source");
               if( iter != mParserStateMap.end() )
               {
                  ///\todo HUGE! set the source!
                  mCurrentState = (*iter).second.get();
               }
            } break;

         case 4:
         default:
            {
               ParserStateMap::iterator iter = mParserStateMap.find("FrameData");
               if( iter != mParserStateMap.end() )
                  mCurrentState = (*iter).second.get();
            } break;
         };
      }

   private:
      const RecordablePtrContainer& mSources;
      KeyFrameContainer mKFC;
      unsigned int mLevel;
      ParserState* mCurrentState;
      typedef std::map<std::string, osg::ref_ptr<ParserState> > ParserStateMap;
      ParserStateMap mParserStateMap;
   }; // end KeyFrameHandler

}; // end namespace dtUtil

#endif // DELTA_KEY_FRAME_HANDLER_INC
