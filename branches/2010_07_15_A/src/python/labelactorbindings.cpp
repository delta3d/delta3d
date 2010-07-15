// LabelActorBindingds.cpp: LabelActor binding implementations.
//  Brett Lindberg and Michael Day, 14 July 2009
//  LabelActor allows you to put 3D text into the scene as an actor
//////////////////////////////////////////////////////////////////////

#include <python/dtpython.h>
#include <dtABC/baseabc.h>
#include <dtABC/labelactor.h>
#include <dtCore/transformable.h>
#include <osgText/Text>

using namespace boost::python;
using namespace dtABC;

class LabelActorWrapper : public LabelActor
{
public:
   enum AlignEnumWrapper
   {
      LEFT_TOP = 0,
      LEFT_CENTER,
      LEFT_BOTTOM,
      CENTER_TOP,
      CENTER_CENTER,
      CENTER_BOTTOM,
      RIGHT_TOP,
      RIGHT_CENTER,
      RIGHT_BOTTOM,
      LEFT_BASE_LINE,
      CENTER_BASE_LINE,
      RIGHT_BASE_LINE,
      LEFT_BOTTOM_BASE_LINE,
      CENTER_BOTTOM_BASE_LINE,
      RIGHT_BOTTOM_BASE_LINE,
      BASE_LINE,
      NO_ALIGNMENT
   };

   AlignEnumWrapper GetTextAlignment2()
   {
      if (GetTextAlignment() == LabelActor::AlignmentEnum::BASE_LINE)
      {
         return LabelActorWrapper::BASE_LINE;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::CENTER_BASE_LINE)
      {
         return LabelActorWrapper::CENTER_BASE_LINE;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::CENTER_BOTTOM)
      {
         return LabelActorWrapper::CENTER_BOTTOM;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::CENTER_BOTTOM_BASE_LINE)
      {
         return LabelActorWrapper::CENTER_BOTTOM_BASE_LINE;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::CENTER_CENTER)
      {
         return LabelActorWrapper::CENTER_CENTER;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::CENTER_TOP)
      {
         return LabelActorWrapper::CENTER_TOP;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::LEFT_BASE_LINE)
      {
         return LabelActorWrapper::LEFT_BASE_LINE;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::LEFT_BOTTOM)
      {
         return LabelActorWrapper::LEFT_BOTTOM;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::LEFT_BOTTOM_BASE_LINE)
      {
         return LabelActorWrapper::LEFT_BOTTOM_BASE_LINE;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::LEFT_CENTER)
      {
         return LabelActorWrapper::LEFT_CENTER;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::LEFT_TOP)
      {
         return LabelActorWrapper::LEFT_TOP;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::RIGHT_BASE_LINE)
      {
         return LabelActorWrapper::RIGHT_BASE_LINE;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::RIGHT_BOTTOM)
      {
         return LabelActorWrapper::RIGHT_BOTTOM;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::RIGHT_BOTTOM_BASE_LINE)
      {
         return LabelActorWrapper::RIGHT_BOTTOM_BASE_LINE;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::RIGHT_CENTER)
      {
         return LabelActorWrapper::RIGHT_CENTER;
      }
      else if (GetTextAlignment() == LabelActor::AlignmentEnum::RIGHT_TOP)
      {
         return LabelActorWrapper::RIGHT_TOP;
      }

      return LabelActorWrapper::NO_ALIGNMENT;
   }

   void SetTextAlignment2(AlignEnumWrapper aew)
   {
      switch (aew)
      {
         case LabelActorWrapper::BASE_LINE:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::BASE_LINE);
            break;

         case LabelActorWrapper::CENTER_BASE_LINE:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::CENTER_BASE_LINE);
            break;

         case LabelActorWrapper::CENTER_BOTTOM:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::CENTER_BOTTOM);
            break;

         case LabelActorWrapper::CENTER_BOTTOM_BASE_LINE:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::CENTER_BOTTOM_BASE_LINE);
            break;

         case LabelActorWrapper::CENTER_CENTER:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::CENTER_CENTER);
            break;

         case LabelActorWrapper::CENTER_TOP:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::CENTER_TOP);
            break;

         case LabelActorWrapper::LEFT_BASE_LINE:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::LEFT_BASE_LINE);
            break;

         case LabelActorWrapper::LEFT_BOTTOM:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::LEFT_BOTTOM);
            break;

         case LabelActorWrapper::LEFT_BOTTOM_BASE_LINE:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::LEFT_BOTTOM_BASE_LINE);
            break;

         case LabelActorWrapper::LEFT_CENTER:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::LEFT_CENTER);
            break;

         case LabelActorWrapper::LEFT_TOP:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::LEFT_TOP);
            break;

         case LabelActorWrapper::RIGHT_BASE_LINE:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::RIGHT_BASE_LINE);
            break;

         case LabelActorWrapper::RIGHT_BOTTOM:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::RIGHT_BOTTOM);
            break;

         case LabelActorWrapper::RIGHT_BOTTOM_BASE_LINE:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::RIGHT_BOTTOM_BASE_LINE);
            break;

         case LabelActorWrapper::RIGHT_CENTER:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::RIGHT_CENTER);
            break;

         case LabelActorWrapper::RIGHT_TOP:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::RIGHT_TOP);
            break;

         default:
            LabelActor::SetTextAlignment(LabelActor::AlignmentEnum::BASE_LINE);
            break;
      }
   }
};

void initLabelActorBindings()
{
  class_<LabelActor, bases<dtCore::Transformable>, dtCore::RefPtr<LabelActor>, boost::noncopyable>("LabelActor", init<optional<const std::string&> >())
    //GetText allows you to get the text value of the label
    .def("GetText", &LabelActor::GetText)
    .def("SetText", &LabelActor::SetText)
    .def("SetFont", &LabelActor::SetFont)
    .def("GetFont", &LabelActor::GetFont)
    .def("SetFontSize", &LabelActor::SetFontSize)
    .def("GetFontSize", &LabelActor::GetFontSize)
    .def("SetTextColor", &LabelActor::SetTextColor)
    .def("GetTextColor", &LabelActor::GetTextColor, return_internal_reference<>())
    .def("SetBackColor", &LabelActor::SetBackColor)
    .def("GetBackColor",&LabelActor::GetBackColor)
    .def("SetBackWidth",&LabelActor::SetBackWidth)
    .def("GetBackWidth", &LabelActor::GetBackWidth)
    .def("GetBackHeight", &LabelActor::GetBackHeight)
    .def("SetBackHeight", &LabelActor::SetBackHeight)
    .def("SetBackSize", &LabelActor::SetBackSize)
    .def("GetBackSize", &LabelActor::GetBackSize, return_internal_reference<>())
    .def("SetBackVisible", &LabelActor::SetBackVisible)
    .def("GetBackVisible", &LabelActor::IsBackVisible)
    .def("SetEnableLighting", &LabelActor::SetEnableLighting)
    .def("GetEnableLighting", &LabelActor::GetEnableLighting)
    .def("SetEnableDepthTesting", &LabelActor::SetEnableDepthTesting)
    .def("GetEnableDepthTesting", &LabelActor::GetEnableDepthTesting)
    .def("GetTextAlignment", &LabelActorWrapper::GetTextAlignment2)
    .def("SetTextAlignment", &LabelActorWrapper::SetTextAlignment2)
    ;

  enum_<LabelActorWrapper::AlignEnumWrapper>("AlignmentEnum")
    .value("BASE_LINE", LabelActorWrapper::BASE_LINE)
    .value("CENTER_BASE_LINE", LabelActorWrapper::CENTER_BASE_LINE)
    .value("CENTER_BOTTOM", LabelActorWrapper::CENTER_BOTTOM)
    .value("CENTER_BOTTOM_BASE_LINE", LabelActorWrapper::CENTER_BOTTOM_BASE_LINE)
    .value("CENTER_CENTER", LabelActorWrapper::CENTER_CENTER)
    .value("CENTER_TOP", LabelActorWrapper::CENTER_TOP)
    .value("LEFT_BASE_LINE", LabelActorWrapper::LEFT_BASE_LINE)
    .value("LEFT_BOTTOM", LabelActorWrapper::LEFT_BOTTOM)
    .value("LEFT_BOTTOM_BASE_LINE", LabelActorWrapper::LEFT_BOTTOM_BASE_LINE)
    .value("LEFT_CENTER", LabelActorWrapper::LEFT_CENTER)
    .value("LEFT_TOP", LabelActorWrapper::LEFT_TOP)
    .value("RIGHT_BASE_LINE", LabelActorWrapper::RIGHT_BASE_LINE)
    .value("RIGHT_BOTTOM", LabelActorWrapper::RIGHT_BOTTOM)
    .value("RIGHT_BOTTOM_BASE_LIN", LabelActorWrapper::RIGHT_BOTTOM_BASE_LINE)
    .value("RIGHT_CENTER", LabelActorWrapper::RIGHT_CENTER)
    .value("RIGHT_TOP", LabelActorWrapper::RIGHT_TOP)
    .export_values();
}
