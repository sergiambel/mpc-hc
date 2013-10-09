#include "StdAfx.h"
#include "NullStorage.h"

#define NOTIMPLEMENTED __asm{ int 3 }; return E_NOTIMPL


STDMETHODIMP NullStorage::QueryInterface(REFIID riid,void ** ppvObject)
{

	NOTIMPLEMENTED;
}

STDMETHODIMP_(ULONG) NullStorage::AddRef(void)
{
	return 1;
}

STDMETHODIMP_(ULONG) NullStorage::Release(void)
{
	return 1;
}

// IStorage
STDMETHODIMP NullStorage::CreateStream(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStream ** ppstm)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::OpenStream(const WCHAR * pwcsName,void * reserved1,DWORD grfMode,DWORD reserved2,IStream ** ppstm)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::CreateStorage(const WCHAR * pwcsName,DWORD grfMode,DWORD reserved1,DWORD reserved2,IStorage ** ppstg)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::OpenStorage(const WCHAR * pwcsName,IStorage * pstgPriority,DWORD grfMode,SNB snbExclude,DWORD reserved,IStorage ** ppstg)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::CopyTo(DWORD ciidExclude,IID const * rgiidExclude,SNB snbExclude,IStorage * pstgDest)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::MoveElementTo(const OLECHAR * pwcsName,IStorage * pstgDest,const OLECHAR* pwcsNewName,DWORD grfFlags)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::Commit(DWORD grfCommitFlags)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::Revert(void)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::EnumElements(DWORD reserved1,void * reserved2,DWORD reserved3,IEnumSTATSTG ** ppenum)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::DestroyElement(const OLECHAR * pwcsName)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::RenameElement(const WCHAR * pwcsOldName,const WCHAR * pwcsNewName)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::SetElementTimes(const WCHAR * pwcsName,FILETIME const * pctime,FILETIME const * patime,FILETIME const * pmtime)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::SetClass(REFCLSID clsid)
{
	return S_OK;
}

STDMETHODIMP NullStorage::SetStateBits(DWORD grfStateBits,DWORD grfMask)
{
	NOTIMPLEMENTED;
}

STDMETHODIMP NullStorage::Stat(STATSTG * pstatstg,DWORD grfStatFlag)
{
	NOTIMPLEMENTED;
}
