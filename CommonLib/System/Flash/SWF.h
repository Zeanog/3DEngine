#pragma once

#import "C:\\Windows\\System32\\Macromed\\Flash\\Flash.ocx" rename_namespace("Flash")

#include "System/Functors/MultiFunctor.h"
#include "System/StaticString.h"
#include "System/StackString.h"
#include "System/Map.h"

#include "System/Flash/Dispatcher.h"

#include <atlbase.h>
#include <atlwin.h>
#include <comdef.h>
#include <stdio.h>

using namespace tinyxml2;

#define HANDLERNAME( fc ) _##fc##Handler

#define DEFINE_CALL_HANDLER( retType, name )	\
	Dispatcher<retType>	name;	\
	retType HANDLERNAME(name)()

#define DEFINE_CALL_HANDLER_1( retType, name, param1Type )	\
	Dispatcher<retType, TYPELIST_1(param1Type)>	name;	\
	retType HANDLERNAME(name)(typename ParamType<param1Type>::Type p1)

#define DEFINE_CALL_HANDLER_2( retType, name, param1Type, param2Type )	\
	Dispatcher<retType, TYPELIST_2(param1Type, param2Type)>	name;	\
	retType HANDLERNAME(name)(typename ParamType<param1Type>::Type p1, typename ParamType<param2Type>::Type p2)

#define DEFINE_CALL_HANDLER_3( retType, name, param1Type, param2Type, param3Type )	\
	Dispatcher<retType, TYPELIST_3(param1Type, param2Type, param3Type)>	name;	\
	retType HANDLERNAME(name)(typename ParamType<param1Type>::Type p1, typename ParamType<param2Type>::Type p2, typename ParamType<param3Type>::Type p3)

template< typename TData >
class TypeConvert;

template<>
class TypeConvert<void> {
public:
	typedef void	TData;

	static TData	Parse(const Char* encodedData) {
	}

	static void	Format(String& out) {
		out = "undefined";
	}

	static _bstr_t	Format() {
		return "undefined";
	}
};

template<>
class TypeConvert<String> {
public:
	typedef String	TData;

	static const StaticString& Type() {
		static StaticString local("string");
		return local;
	}

	static TData	Parse(const Char* encodedData) {
		return encodedData;
	}

	static void	Format(const TData& data, String& out) {
		Int32 l = Type().Length();
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, data.Length() + Type().Length() * 2 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%s</%s>", Type().CStr(), data.CStr(), Type().CStr());
		out = encodedData.CStr();
	}

	static _bstr_t	Format(const TData& data) {
		Int32 l = Type().Length();
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, data.Length() + Type().Length() * 2 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%s</%s>", Type().CStr(), data.CStr(), Type().CStr());
		return encodedData.CStr();
	}
};

template<>
class TypeConvert<const Char*> {
public:
	typedef const Char*	TData;

	static const StaticString& Type() {
		static StaticString local("string");
		return local;
	}

	static TData	Parse(const Char* encodedData) {
		return encodedData;
	}

	static void	Format(TData data, String& out) {
		Int32 l = Type().Length();
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, String::StrLen(data) + Type().Length() * 2 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%s</%s>", Type().CStr(), data, Type().CStr());
		out = encodedData.CStr();
	}

	static _bstr_t	Format(TData data) {
		Int32 l = Type().Length();
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, String::StrLen(data) + Type().Length() * 2 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%s</%s>", Type().CStr(), data, Type().CStr());
		return encodedData.CStr();
	}
};

template<>
class TypeConvert<Int32> {
public:
	typedef Int32	TData;

	static const StaticString& Type() {
		static StaticString local("number");
		return local;
	}

	static TData	Parse(const Char* encodedData) {
		TData data;
		sscanf_s(encodedData, "%d", &data);
		return data;
	}

	static void	Format(const TData& data, String& out) {
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, Type().Length() * 2 + 16 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%d</%s>", Type().CStr(), data, Type().CStr());
		out = encodedData.CStr();
	}

	static _bstr_t	Format(const TData& data) {
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, Type().Length() * 2 + 16 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%d</%s>", Type().CStr(), data, Type().CStr());
		return encodedData.CStr();
	}
};

template<>
class TypeConvert<Float32> {
public:
	typedef Float32	TData;

	static const StaticString& Type() {
		static StaticString local("number");
		return local;
	}

	static TData	Parse(const Char* encodedData) {
		TData data;
		sscanf_s(encodedData, "%f", &data);
		return data;
	}

	static void	Format(const TData& data, String& out) {
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, Type().Length() * 2 + 32 + 1 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%f</%s>", Type().CStr(), data, Type().CStr());
		out = encodedData.CStr();
	}

	static _bstr_t	Format(const TData& data) {
		Int32 numExtraChars = 6;//<%s>%s</%s>
		STACK_STRING(encodedData, Type().Length() * 2 + 32 + 1 + numExtraChars);

		sprintf_s(encodedData.CStr(), encodedData.Allocated(), "<%s>%f</%s>", Type().CStr(), data, Type().CStr());
		return encodedData.CStr();
	}
};

class FlashEngine : public Flash::_IShockwaveFlashEvents {
protected:
	Flash::IShockwaveFlash* m_pFlash = NULL;

	IConnectionPointContainer *m_lpConCont;
	IConnectionPoint *m_lpConPoint;
	DWORD			m_dwConPointID;

	Int32			m_iRef = 0;

protected:
	void		RegisterHandlers();

public:
	FlashEngine() {
		RegisterHandlers();
	}

	Bool	AttachTo( HWND hWnd ) {
		HRESULT hr = CoCreateInstance(__uuidof(Flash::ShockwaveFlash), NULL, CLSCTX_ALL, __uuidof(Flash::IShockwaveFlash), (void**)&m_pFlash);
		if (FAILED(hr)) {
			return false;
		}

		if (FAILED(AtlAxAttachControl(m_pFlash, hWnd, NULL))) {
			m_pFlash->Release();
			m_pFlash = NULL;
			return false;
		}

		//m_lVersion = m_pFlash->FlashVersion();
		/*if ((m_lVersion & 0x00FF0000) == 0x00080000)
			m_bFixTransparency = TRUE;
		else
			m_bFixTransparency = FALSE;*/

		hr = m_pFlash->QueryInterface(IID_IConnectionPointContainer, (void**)&m_lpConCont);
		if (FAILED(hr))
			return FALSE;

		hr = m_lpConCont->FindConnectionPoint(__uuidof(Flash::_IShockwaveFlashEvents), &m_lpConPoint);
		if (FAILED(hr))
			return FALSE;

		hr = m_lpConPoint->Advise((Flash::_IShockwaveFlashEvents *)this, &m_dwConPointID);
		if (FAILED(hr))
			return FALSE;

		//if (m_bTransparent)
			//m_pFlash->PutWMode(L"transparent");

		//m_pFlash->PutScale(L"showAll");
		//m_pFlash->PutBackgroundColor(0x00000000);

		return true;
	}

	virtual ~FlashEngine() {
		if(m_lpConPoint)
		{
			if (m_dwConPointID) {
				m_lpConPoint->Unadvise(m_dwConPointID);
				m_dwConPointID = 0;
			}
			m_lpConPoint->Release();
			m_lpConPoint = NULL;
		}

		if (m_lpConCont) {
			m_lpConCont->Release();
			m_lpConCont = NULL;
		}
	}

	HRESULT	SetReturnValue(_bstr_t encodedRet) {
		return m_pFlash->SetReturnValue(encodedRet);
	}

	Bool PlayMovie(const StaticString& path) {
		return PlayMovie(path.CStr());
	}

	Bool PlayMovie(const Char* path) {
		return PlayMovie(_bstr_t(path));
	}

	Bool PlayMovie(bstr_t path) {
		if (!m_pFlash) {
			return false;
		}

		m_pFlash->put_BackgroundColor(RGB(255, 255, 255));
		if( FAILED(m_pFlash->LoadMovie(0, path)) ) {
			return false;
		}

		m_pFlash->PutLoop(VARIANT_FALSE);
		m_pFlash->Rewind();
		m_pFlash->Play();
		return true;
	}

	Bool	StopMovie() {
		return SUCCEEDED(m_pFlash->Stop());
	}

	bstr_t	CallFunction(bstr_t request) {
		//_bstr_t fu(L"<invoke name=\"foo1\" returntype=\"xml\"><arguments><string>BLEH!!</string></arguments></invoke>");
		//_bstr_t rs = p->CallFunction(fu);
		return m_pFlash->CallFunction(request);
	}

	static const Char*	BuildFuncBlock(const Char* funcName) {
		return String::Format("name=\"%s\"", funcName);
	}

	static const Char* BuildInvokeBlock( const Char* funcName, const Char* encodedArgs ) {
		return String::Format("<invoke %s returntype=\"xml\"><arguments>%s</arguments></invoke>", BuildFuncBlock(funcName), encodedArgs);
	}

	template< typename TReturn >
	TReturn	CallFunction(const Char* funcName) {
		return TReturn();
	}

	template< typename TReturn, typename TParam1 >
	TReturn	CallFunction(const Char* funcName, TParam1 param1 ) {
		static String encodedArgs;

		TypeConvert<TParam1>::Format(param1, encodedArgs);

		const Char* encodedCall = BuildInvokeBlock(funcName, encodedArgs.CStr());

		//_bstr_t fu(L"<invoke name=\"foo1\" returntype=\"xml\"><arguments><string>BLEH!!!</string></arguments></invoke>");
		bstr_t ret = CallFunction(encodedCall);

		return TypeConvert<TReturn>::Parse(ret);
	}

	template< typename TReturn, typename TParam1, typename TParam2 >
	TReturn	CallFunction(const Char* funcName, TParam1 param1, TParam2 param2) {
		static String encodedArgs;
		static String encodedArg;

		TypeConvert<TParam1>::Format(param1, encodedArg);
		encodedArgs = encodedArg;
		TypeConvert<TParam2>::Format(param2, encodedArg);
		encodedArgs += encodedArg;

		const Char* encodedCall = BuildInvokeBlock(funcName, encodedArgs.CStr());

		//_bstr_t fu(L"<invoke name=\"foo1\" returntype=\"xml\"><arguments><string>BLEH!!!</string></arguments></invoke>");
		bstr_t ret = CallFunction(encodedCall);

		return TypeConvert<TReturn>::Parse(ret);
	}

	template< typename TReturn, typename TParam1, typename TParam2, typename TParam3 >
	TReturn	CallFunction(const Char* funcName, TParam1 param1, TParam2 param2, TParam3 param3) {
		static String encodedArgs;
		static String encodedArg;

		TypeConvert<TParam1>::Format(param1, encodedArg);
		encodedArgs = encodedArg;
		TypeConvert<TParam2>::Format(param2, encodedArg);
		encodedArgs += encodedArg;
		TypeConvert<TParam3>::Format(param3, encodedArg);
		encodedArgs += encodedArg;

		const Char* encodedCall = BuildInvokeBlock(funcName, encodedArgs.CStr());

		//_bstr_t fu(L"<invoke name=\"foo1\" returntype=\"xml\"><arguments><string>BLEH!!!</string></arguments></invoke>");
		bstr_t ret = CallFunction(encodedCall);

		return TypeConvert<TReturn>::Parse(ret);
	}

	typedef Map<StaticString, const IFunctorImpl<void, TYPELIST_2(FlashEngine*, XMLHandle*)>*>	THandlerContainer;
	THandlerContainer		m_HandlerMap;

	void	AddHandler(const Char* handlerName, THandlerContainer::TValue handler) {
		m_HandlerMap.Add(StaticString(handlerName), handler);
	}

	HRESULT	InvokeCall(XMLHandle* hInvoke) {
		try {
			const char* functionName = STACK_ALLOC(char, 512);
			int result = hInvoke->ToElement()->QueryStringAttribute("name", &functionName);
			if (result != 0)
			{
				return S_FALSE;
			}

			THandlerContainer::TValue handler = m_HandlerMap[functionName];
			if (!handler) {
				return S_FALSE;
			}

			(*handler)(this, hInvoke);
			return S_OK;
		}
		catch (...) {
			assert(0);
		}
	}

	HRESULT STDMETHODCALLTYPE Invoke(
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
		/* [out] */ VARIANT __RPC_FAR *pVarResult,
		/* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
		/* [out] */ UINT __RPC_FAR *puArgErr)
	{
		if (wFlags == DISPATCH_METHOD)
		{
			switch (dispIdMember)
			{
			case 0xc5: // FlashCall (from ActionScript)
				if (pDispParams->cArgs != 1 || pDispParams->rgvarg[0].vt != VT_BSTR)
					return E_INVALIDARG;
				return FlashCall(pDispParams->rgvarg[0].bstrVal);

			case 0x96: // FSCommand (from ActionScript)
				if (pDispParams->cArgs != 2 || pDispParams->rgvarg[0].vt != VT_BSTR || pDispParams->rgvarg[1].vt != VT_BSTR)
					return E_INVALIDARG;
				return FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
				//break;

			case 0x7a6: // OnProgress                  
				return OnProgress(pDispParams->rgvarg[0].intVal);
				//break;

			case DISPID_READYSTATECHANGE:
				return E_NOTIMPL;
				break;
			}
		}

		return E_NOTIMPL;
	}

	//
	// Methods for interface 'IDispatch'
	//
	HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT __RPC_FAR *pctinfo)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetTypeInfo(
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE GetIDsOfNames( REFIID riid, LPOLESTR __RPC_FAR *rgszNames, UINT cNames, LCID lcid,/*[out]*/ DISPID __RPC_FAR *rgDispId)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void ** ppvObject)
	{
		if (IsEqualGUID(riid, __uuidof(Flash::_IShockwaveFlashEvents)))
		{
			*ppvObject = (void*)static_cast<Flash::_IShockwaveFlashEvents *>(this);
		}
		/*else if (IsEqualGUID(riid, Flash::IID_IServiceProvider))
		{
			*ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::IServiceProvider *>(this);
		}*/
		//else if (IsEqualGUID(riid, ShockwaveFlashObjects::IID_ICanHandleException))
		//{
		//	//*ppvObject = (void*)dynamic_cast<ShockwaveFlashObjects::ICanHandleException *>(this);
		//}
		else
		{
			*ppvObject = 0;
			return E_NOINTERFACE;
		}
		if (!(*ppvObject))
			return E_NOINTERFACE; //if dynamic_cast returned 0

		m_iRef++;
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{
		m_iRef++;
		return m_iRef;
	}

	ULONG STDMETHODCALLTYPE Release()
	{
		m_iRef--;

		assert(m_iRef >= 0);

		return m_iRef;
	}

	HRESULT	STDMETHODCALLTYPE FlashCall( bstr_t request ) {
		HRESULT hr = S_FALSE;

		if (!m_pFlash)
		{
			return hr;
		}

		tinyxml2::XMLDocument doc;
		const char *c_str = request;
		doc.Parse(c_str);

		XMLHandle hDoc(&doc);
		XMLHandle hInvoke = hDoc.FirstChildElement("invoke");
		hr = InvokeCall(&hInvoke);
		return hr;
	}

	HRESULT STDMETHODCALLTYPE FSCommand( _bstr_t command, _bstr_t args ) {
		HRESULT hr = S_FALSE;

		return hr;
	}

	HRESULT STDMETHODCALLTYPE OnReadyStateChange( long newState ) {
		HRESULT hr = S_FALSE;

		return hr;
	}

	HRESULT STDMETHODCALLTYPE OnProgress(long percentDone) {
		HRESULT hr = S_FALSE;

		return hr;
	}

	DEFINE_CALL_HANDLER_1(void, ext2, Int32) {
		OutputDebugString(String::Format("ext2: %d\n", p1));
	}
};