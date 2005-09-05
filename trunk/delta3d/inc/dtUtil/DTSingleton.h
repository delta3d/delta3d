
#ifndef __DELTA_SINGLETON_H__
#define __DELTA_SINGLETON_H__


/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2004 MOVES Institute 
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
* @author Bradley Anderegg
*/




////////////////////////////////////////////////////////////////////////////////////////////
// CLASS: DTSingleton
//
// DESCRIPTION: A template singleton 
//
// USAGE: DTSingleton<SingletonClass> singleton.... singleton.CreateInstance()
//			..singleton.GetInstance()->SomeFancyFunction().. singleton.DestroyInstance()
//
// AUTHOR: Bradley Anderegg
//
// VERSION: 100
//
////////////////////////////////////////////////////////////////////////////////////////////

namespace dtUtil
{

	template <class T>
	class DTSingleton
	{

		////////////////////////////////////////////////////////////////////////////////////////////
		//Data Structures
		////////////////////////////////////////////////////////////////////////////////////////////
	public:


		/////////////////////////////////////////////////////////////////////////////////////////////
		//Functions
		/////////////////////////////////////////////////////////////////////////////////////////////
	public:

		DTSingleton(){}
		~DTSingleton(){}

		static void CreateInstance();
		static void DestroyInstance();

		static T* GetInstance();


		//operators
		T& operator*()  { return *mInstance; }
		const T& operator*() const { return *mInstance; }
		T* operator->() { return mInstance; }
		const T* operator->() const   { return mInstance; }
		bool operator!() const	{ return mInstance == 0; }
		bool Valid() const	{ return mInstance != 0; }


	private:


		/////////////////////////////////////////////////////////////////////////////////////////////
		//Member Variables                                                                         
		/////////////////////////////////////////////////////////////////////////////////////////////
	public:



	private:

		static T* mInstance;


	};


}//dtUtil

#endif

