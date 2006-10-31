/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * William E. Johnson II
 */
#ifndef DELTA_TEST_AAR_EXCEPTION_ENUM
#define DELTA_TEST_AAR_EXCEPTION_ENUM

#include <dtUtil/exception.h>
#include "export.h"

/**
 * Exceptions that may be thrown by the demo.
 */
class TEST_AAR_EXPORT AppException : public dtUtil::Enumeration
{
   DECLARE_ENUM(AppException);
  
   public:
     
      static AppException INIT_ERROR;
   
   private:
      AppException(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
};

/**
 * Exception that may be thrown by the HUD.
 */
class TEST_AAR_EXPORT ARRHUDException : public dtUtil::Enumeration
{
   DECLARE_ENUM(ARRHUDException);
   
   public:
      
      static ARRHUDException INIT_ERROR;
      static ARRHUDException RUNTIME_ERROR;

   private:
      
      ARRHUDException(const std::string &name) : dtUtil::Enumeration(name)
      {
         AddInstance(this);
      }
};

#endif
