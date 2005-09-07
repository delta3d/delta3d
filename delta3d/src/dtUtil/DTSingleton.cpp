
#include <dtUtil/DTSingleton.h>


namespace dtUtil
{


template<class T> T* DTSingleton<T>::mInstance = 0;


template<class T>
void DTSingleton<T>::CreateInstance()
{
	if(!mInstance)
	{
		mInstance = new T();
	}
	return mInstance;
}

template<class T>
void DTSingleton<T>::DestroyInstance()
{
	delete mInstance;
	mInstance = 0;
}

template<class T>
T* DTSingleton<T>::Instance()
{
	return mInstance;
}


}//dtUtil

