#pragma once

#include <Wbemidl.h>
#include <tchar.h>
#include <comdef.h>
#include <exception>
class WMIWBEMINFO
{
private:
	static WMIWBEMINFO* currentInstance;
	HRESULT hres;
	IWbemServices* pSvc;
	IWbemLocator* pLoc;
	WMIWBEMINFO() {

		// Step 1: --------------------------------------------------
		// Initialize COM. ------------------------------------------

		hres = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(hres)) {
			throw std::exception("Failed to initialize COM library");
		}

		// Step 2: --------------------------------------------------
		// Set general COM security levels --------------------------

		hres = CoInitializeSecurity(
			NULL,
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities 
			NULL                         // Reserved
		);


		if (FAILED(hres)) {
			CoUninitialize();
			throw std::exception("Failed to initialize COM security levels");
		}

		//obtain the initial locator to WMI

		this->pLoc = NULL;

		hres = CoCreateInstance(
			CLSID_WbemLocator,
			0,
			CLSCTX_INPROC_SERVER,
			IID_IWbemLocator, (LPVOID*)& pLoc);

		if (FAILED(hres)) {
			CoUninitialize();
			throw std::exception("Failed to create IWbemLocator object");
		}

		// Step 4: -----------------------------------------------------
		// Connect to WMI through the IWbemLocator::ConnectServer method

		this->pSvc = NULL;

		// Connect to the root\cimv2 namespace with
		// the current user and obtain pointer pSvc
		// to make IWbemServices calls.
		hres = pLoc->ConnectServer(
			_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
			NULL,                    // User name. NULL = current user
			NULL,                    // User password. NULL = current
			0,                       // Locale. NULL indicates current
			NULL,                    // Security flags.
			0,                       // Authority (for example, Kerberos)
			0,                       // Context object 
			&pSvc                    // pointer to IWbemServices proxy
		);

		if (FAILED(hres)) {
			pLoc->Release();
			CoUninitialize();
			throw std::exception("Could not connect to root/cimv2 namespace");
		}


		// Step 5: --------------------------------------------------
		// Set security levels on the proxy -------------------------

		hres = CoSetProxyBlanket(
			pSvc,                        // Indicates the proxy to set
			RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
			RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
			NULL,                        // Server principal name 
			RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
			RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
			NULL,                        // client identity
			EOAC_NONE                    // proxy capabilities 
		);

		if (FAILED(hres)) {
			pSvc->Release();
			pLoc->Release();
			CoUninitialize();
			throw std::exception("Could not set proxy blanket");
		}
	}

public:
	static WMIWBEMINFO* getWMIWBEMINFOInstance();
	IWbemLocator *getWbemLocator();
	IWbemServices* getWbemServices();
	HRESULT getHres();
	~WMIWBEMINFO();
};

