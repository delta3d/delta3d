
template<typename T>
CreateDestroyPolicy<T>::CreateDestroyPolicy()
   : mCreateName("CreateComponent")
   , mDestroyName("DestroyComponent")
{
}

template<typename T>
bool CreateDestroyPolicy<T>::LoadSymbols(const dtUtil::LibrarySharingManager::LibraryHandle* handle,
                                         LibraryInterface& pi )
{
   dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS csa = handle->FindSymbol( mCreateName );
   dtUtil::LibrarySharingManager::LibraryHandle::SYMBOL_ADDRESS dsa = handle->FindSymbol( mDestroyName );

   bool result(true);

   if( csa == NULL )
   {
      LOG_WARNING("Could not find symbol: " + mCreateName)
      pi.mCreator = NULL;
      result = false;
   }
   else
   {
      pi.mCreator = (typename LibraryInterface::CREATE_FUNCTION)csa;
   }

   if( dsa == NULL )
   {
      LOG_WARNING("Could not find symbol: " + mDestroyName)
      pi.mDestroyer = NULL;
      result = false;
   }
   else
   {
      pi.mDestroyer = (typename LibraryInterface::DESTROY_FUNCTION)dsa;
   }

   return result;
}

