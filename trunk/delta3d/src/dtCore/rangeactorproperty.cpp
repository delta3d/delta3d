/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
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
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/dtcoreprefix.h>
#include <dtCore/rangeactorproperty.h>
#include <dtUtil/stringutils.h>



namespace dtCore
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   RangeActorProperty::RangeActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      const dtUtil::RefString& desc /*= ""*/, const dtUtil::RefString& groupName /*= ""*/)
      : BaseClass(DataType::RANGE, name, label, set, get, desc, groupName)
   {
      SetNumberPrecision(3 * sizeof(Range::ValueType) + 1);
   }

   ////////////////////////////////////////////////////////////////////////////
   bool RangeActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      osg::Vec3 newValue;

      if (dtUtil::ParseVec<osg::Vec3>(value, newValue, 3))
      {
         Range range(newValue.x(), newValue.y(), newValue.z());
         SetValue(range);
         return true;
      }
      else return false;

   }

   ////////////////////////////////////////////////////////////////////////////
   const std::string RangeActorProperty::ToString() const
   {
      std::ostringstream stream;
      stream.precision(GetNumberPrecision());

      Range range = GetValue();
      osg::Vec3 vec(range.GetMin(), range.GetMax(), range.GetValue());

      stream << vec;
      return stream.str();
   }

}

