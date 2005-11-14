/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Matthew W. Campbell and David Guthrie
 */
#ifndef DELTA_DATASTREAM
#define DELTA_DATASTREAM

#include <string>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Vec2f>
#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/Vec2d>
#include <osg/Vec3d>
#include <osg/Vec4d>

#include <dtCore/uniqueid.h>
#include <dtDAL/exceptionenum.h>
#include "dtGame/export.h"

namespace dtGame 
{
   
   class DT_GAMEMANAGER_EXPORT DataStreamException : public dtDAL::ExceptionEnum 
   {
      DECLARE_ENUM(DataStreamException);
      public:
         
         static DataStreamException BUFFER_INVALID;
         static DataStreamException BUFFER_READ_ERROR;
         static DataStreamException BUFFER_WRITE_ERROR;
         static DataStreamException BUFFER_INVALID_POS;
      
      private:
         DataStreamException(const std::string &name) : dtDAL::ExceptionEnum(name) 
         {
            AddInstance(this);
         }
   };

   class DT_GAMEMANAGER_EXPORT DataStream 
   {
      public:

         class DT_GAMEMANAGER_EXPORT SeekTypeEnum : public dtUtil::Enumeration 
         {
            DECLARE_ENUM(SeekTypeEnum);
            
            public:
               static const SeekTypeEnum SET;
               static const SeekTypeEnum CURRENT;
               static const SeekTypeEnum END;
            private:
               SeekTypeEnum(const std::string &name) : dtUtil::Enumeration(name) 
               {
                  AddInstance(this);
               }
         };

         DataStream();
         DataStream(const DataStream &rhs);
         DataStream &operator=(const DataStream &rhs);
         virtual ~DataStream();

         DataStream& operator>>(bool& value) { Read(value); return *this;}
         DataStream& operator<<(bool value) { Write(value); return *this; }

         DataStream& operator>>(unsigned char& value) { Read(value); return *this;}
         DataStream& operator<<(unsigned char value) { Write(value); return *this; }
         
         DataStream& operator>>(short& value) { Read(value); return *this; }
         DataStream& operator<<(short value) { Write(value); return *this; }
         
         DataStream& operator>>(unsigned short& value) { Read(value); return *this; }
         DataStream& operator<<(unsigned short value) { Write(value); return *this; }
         
         DataStream& operator>>(int& value) { Read(value); return *this; }
         DataStream& operator<<(int value) { Write(value); return *this; }
         
         DataStream& operator>>(unsigned& value) { Read(value); return *this; }
         DataStream& operator<<(unsigned value) { Write(value); return *this; }
         
         DataStream& operator>>(long& value) { Read(value); return *this; }
         DataStream& operator<<(long value) { Write(value); return *this; }

         DataStream& operator>>(unsigned long& value) { Read(value); return *this; }
         DataStream& operator<<(unsigned long value) { Write(value); return *this; }

         DataStream& operator>>(float& value) { Read(value); return *this; }
         DataStream& operator<<(float value) { Write(value); return *this; }
         
         DataStream& operator>>(double& value) { Read(value); return *this; }
         DataStream& operator<<(double value) { Write(value); return *this; }
         
         DataStream& operator>>(std::string& value) { Read(value); return *this; }
         DataStream& operator<<(const std::string& value) { Write(value); return *this; }

         DataStream& operator>>(dtCore::UniqueId& value) { Read(value); return *this; }
         DataStream& operator<<(const dtCore::UniqueId& value) { Write(value); return *this; }
         
         DataStream& operator>>(osg::Vec2f& value) { Read(value); return *this; }
         DataStream& operator<<(const osg::Vec2f& value) { Write(value); return *this; }

         DataStream& operator>>(osg::Vec2d& value) { Read(value); return *this; }
         DataStream& operator<<(const osg::Vec2d& value) { Write(value); return *this; }
         
         DataStream& operator>>(osg::Vec3f& value) { Read(value); return *this; }
         DataStream& operator<<(const osg::Vec3f& value) { Write(value); return *this; }

         DataStream& operator>>(osg::Vec3d& value) { Read(value); return *this; }
         DataStream& operator<<(const osg::Vec3d& value) { Write(value); return *this; }

         DataStream& operator>>(osg::Vec4f& value) { Read(value); return *this; }
         DataStream& operator<<(const osg::Vec4f& value) { Write(value); return *this; }

         DataStream& operator>>(osg::Vec4d& value) { Read(value); return *this; }
         DataStream& operator<<(const osg::Vec4d& value) { Write(value); return *this; }

         void Read(bool& c);
         void Write(bool c);

         void Read(unsigned char& c);
         void Write(unsigned char c);

         void Read(short& s);
         void Write(short s);

         void Read(unsigned short& s);
         void Write(unsigned short s);

         void Read(int& i);
         void Write(int i);

         void Read(unsigned& i);
         void Write(unsigned i);

         void Read(long& i);
         void Write(long i);

         void Read(unsigned long& i);
         void Write(unsigned long i);

         void Read(float& f);
         void Write(float f);

         void Read(double& d);
         void Write(double d);

         void Read(std::string& string);
         void Write(const std::string& string);

         void Read(dtCore::UniqueId& id);
         void Write(const dtCore::UniqueId& id);

         void Read(osg::Vec2f& vector);
         void Write(const osg::Vec2f &vector);

         void Read(osg::Vec2d& vector);
         void Write(const osg::Vec2d& vector);

         void Read(osg::Vec3f& vector);
         void Write(const osg::Vec3f& vector);

         void Read(osg::Vec3d& vector);
         void Write(const osg::Vec3d& vector);

         void Read(osg::Vec4f& vector);
         void Write(const osg::Vec4f& vector);
         
         void Read(osg::Vec4d& vector);
         void Write(const osg::Vec4d& vector);

         unsigned int GetBufferCapacity() const { return mBufferCapacity; }
        
         unsigned int GetBufferSize() const { return mBufferSize; }

         void Rewind() { mReadPos = mWritePos = 0; }

         void Seekp(unsigned int offset, const SeekTypeEnum &type);
         void Seekg(unsigned int offset, const SeekTypeEnum &type);

      private:
         bool isLittleEndian;
         void ResizeBuffer();

      private:
         char *mBuffer;
         unsigned int mBufferSize, mBufferCapacity;
         unsigned int mReadPos,mWritePos;
    };

}


#endif //DELTA_DATASTREAM
