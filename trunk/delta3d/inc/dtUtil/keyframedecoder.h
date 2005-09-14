#ifndef DTUTIL_KEYFRAME_DECODER_INC
#define DTUTIL_KEYFRAME_DECODER_INC

#include "dtUtil/log.h"
#include "dtUtil/stringutils.h"
#include <xercesc/dom/DOMTreeWalker.hpp>

namespace dtUtil
{
   /** \brief A class that fills key frame data with DOM parsing.
     * Developed for the dtCore::Recorder class, it assumes a DOM
     * tree's structure, and uses existing recordable instances
     * to "decode", or deserialize, the XML hierarchy into a data package,
     * known here as a FrameDataType.
     *
     * Requires the following layout in XML:
     * Top
     *   FrameStamp,  Attr="Time"
     *      Source
     *
     * "Time" is required to an attribute name, and its value is assumed to be a double precision floating point number.
     * A 'Source' tag is given to a RecordableType instance and deserialized into usable data.
     *
     */
   template<typename RecordableT,typename FrameDataT>
   class KeyFrameDecoder
   {
   public:
      typedef RecordableT                                    RecordableType;         /// The type of object of interest.  RecordableTypes know how to create, serialize, and deserialize FrameDataTypes.
      typedef FrameDataT                                     FrameDataType;          /// The data to be saved from the object of interest.
      typedef std::vector< dtCore::RefPtr<FrameDataType> >   FrameDataPtrContainer;  /// A container to hold each source's frame data.
      typedef std::pair<double,FrameDataPtrContainer>        KeyFrame;               /// The time stamp applied to the entire container of frame data.
      typedef std::vector<KeyFrame>                          KeyFrameContainer;      /// The container of KeyFrame data.
      typedef std::vector< dtCore::RefPtr<RecordableType> >  RecordablePtrContainer; /// The container of sources of frame data.

      /** \brief The constructor.
        * @param sources The RecordableType instances that are needed for deserializing the XML information.
        * @param kfc The KeyFrameContainer provided will be appended during loading.  If only loaded data
        * is wished to be contained, then the container should be cleared before executing the Walk.
        */
      KeyFrameDecoder(const RecordablePtrContainer& sources, KeyFrameContainer& kfc): mSources(sources), mKFC(kfc), mSourceIndex(0), mSourcesSize(sources.size())
      {
      }

      ~KeyFrameDecoder()
      {
      }

      void Walk(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc)
      {
         if( mSources.empty() )
         {
            LOG_INFO("No recordable instances are available for loading from XML.")
            return;
         }

         // --- build a vector of frame data --- //
         // tree walking assumes a schema
         // root
         //   framestamp
         //     source
         //       framedata
         //     source
         //       framedata
         //     source (etc)
         //       framedata
         // ------------------------------------ //
         XERCES_CPP_NAMESPACE_QUALIFIER DOMTreeWalker* framestampwalker = doc->createTreeWalker( doc->getDocumentElement(), XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeFilter::SHOW_ELEMENT, 0, true );

         float timeStamp(0.0);
         // start at the root
         // assume the first child is a frame stamp
         for(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* framestamp=framestampwalker->firstChild();
            framestamp != 0;
            framestamp = framestampwalker->nextSibling() )
         {
            std::string val( dtUtil::FindAttributeValueFor("TimeCode", framestamp->getAttributes()) );
            timeStamp = dtUtil::ToFloat( val );

            FrameDataPtrContainer dataToBeFilled;
            DecodeFrameStamp( doc, framestamp , dataToBeFilled);
            mKFC.push_back( typename KeyFrameContainer::value_type( timeStamp, dataToBeFilled ) );
         }
      }

   protected:
      /** Walks the frame stamp tag to find each source's data and
        * asks the source to deserialize the XML tag into a data package.
        */
      void DecodeFrameStamp(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,
                            XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* fs,
                            FrameDataPtrContainer& fdc)
      {
         XERCES_CPP_NAMESPACE_QUALIFIER DOMTreeWalker* sourcewalker = doc->createTreeWalker( fs, XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeFilter::SHOW_ELEMENT, 0, true );

         // move the walker to the first source
         for(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode* child=sourcewalker->firstChild();
             child != 0;
             child=sourcewalker->nextSibling())
         {
            XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* element = static_cast<XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*>( child );
            FrameDataType* data = DecodeSourceData( doc, element );
            fdc.push_back( data );
         }
      }

      FrameDataType* DecodeSourceData(XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc,
                                      XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* e)
      {
         return mSources[mSourceIndex]->Deserialize( doc, e );
      }

   private:
      const RecordablePtrContainer& mSources;
      KeyFrameContainer& mKFC;
      unsigned int mSourceIndex, mSourcesSize;
   };

};

#endif // DTUTIL_KEYFRAME_DECODER_INC

