/*
BSD 2-Clause License

Copyright (c) 2024, badcoiq
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "badcoiq.h"

#ifdef BQ_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dbghelp.h>
typedef BOOL(__stdcall* tSymInitialize)(HANDLE hProcess, PSTR UserSearchPath, BOOL fInvadeProcess);
typedef BOOL(__stdcall* tSymCleanup)(HANDLE hProcess);
typedef DWORD(__stdcall* tSymGetOptions)();
typedef DWORD(__stdcall* tSymSetOptions)(DWORD SymOptions);
typedef BOOL(__stdcall* tSymFromAddr)(HANDLE hProcess, DWORD64 dwAddr, PDWORD64 pdwDisplacement, PSYMBOL_INFO Symbol);
typedef BOOL(__stdcall* tSymGetLineFromAddr64)(HANDLE hProcess, DWORD64 dwAddr, PDWORD64 pdwDisplacement, PIMAGEHLP_LINE64 Line);
typedef void(__stdcall* tRtlCaptureContext)(PCONTEXT context);
typedef USHORT(__stdcall* tRtlCaptureStackBackTrace)(ULONG FramesToSkip, ULONG FramesToCapture, PVOID* BackTrace, PULONG BackTraceHash);
#endif

class bqStackTracerImpl
{
	void _init();
public:
	bqStackTracerImpl();
	~bqStackTracerImpl();

	void DumpStackTrace(const size_t* trace, int numtrace);
	int  GetStackTrace(size_t* trace, int maxtrace, int framesToSkip = 0);

	CRITICAL_SECTION m_lock;
	bool m_lockInitialized = false;
	bool m_initialized = false;
	HINSTANCE m_DbgHelpDll = 0;
	HINSTANCE m_Kernel32Dll = 0;
	tSymInitialize m_SymInitialize = 0;
	tSymCleanup m_SymCleanup = 0;
	tSymGetOptions m_SymGetOptions = 0;
	tSymSetOptions m_SymSetOptions = 0;
	tSymFromAddr m_SymFromAddr = 0;
	tSymGetLineFromAddr64 m_SymGetLineFromAddr64 = 0;
	tRtlCaptureContext m_RtlCaptureContext = 0;
	tRtlCaptureStackBackTrace m_RtlCaptureStackBackTrace = 0;
};

#define LOAD_FUNCTION(A) if(1) { m_##A = (t##A) GetProcAddress(m_DbgHelpDll, #A); } else
#define LOAD_FUNCTION_WARN(A) if(1) { m_##A = (t##A) GetProcAddress(m_DbgHelpDll, #A); NATIVE_WARN_IF( m_##A == NULL, "Could not load symbol " #A " from dbghelp.dll, version too old, but will continue without it."); } else

bqStackTracerImpl::bqStackTracerImpl()
{
	InitializeCriticalSection(&m_lock);
	m_lockInitialized = true;
}

bqStackTracerImpl::~bqStackTracerImpl()
{
	m_lockInitialized = false;
	DeleteCriticalSection(&m_lock);
	if(m_SymCleanup)
		m_SymCleanup(GetCurrentProcess());
	if (m_DbgHelpDll) 
		FreeLibrary(m_DbgHelpDll);
	if (m_Kernel32Dll)
		FreeLibrary(m_Kernel32Dll);
}

void bqStackTracerImpl::_init()
{
	if (!m_initialized)
	{
		EnterCriticalSection(&m_lock);
		if (!m_initialized)
		{
			m_DbgHelpDll = LoadLibraryA("dbghelp.dll");
			if (m_DbgHelpDll)
			{
				m_Kernel32Dll = LoadLibraryA("kernel32.dll");
				if (m_Kernel32Dll)
				{
					LOAD_FUNCTION(SymInitialize);
					LOAD_FUNCTION(SymCleanup);
					LOAD_FUNCTION(SymGetOptions);
					LOAD_FUNCTION(SymSetOptions);
					LOAD_FUNCTION(SymFromAddr);
					LOAD_FUNCTION(SymGetLineFromAddr64);

					m_RtlCaptureContext = (tRtlCaptureContext)GetProcAddress(m_Kernel32Dll, "RtlCaptureContext");
					m_RtlCaptureStackBackTrace = (tRtlCaptureStackBackTrace)GetProcAddress(m_Kernel32Dll, "RtlCaptureStackBackTrace");

					DWORD symOptions = m_SymGetOptions();
					symOptions |= SYMOPT_LOAD_LINES | SYMOPT_DEBUG | SYMOPT_DEFERRED_LOADS;
					m_SymSetOptions(symOptions);

					UINT prevErrMode = GetErrorMode();
					SetErrorMode(SEM_FAILCRITICALERRORS);
					BOOL initsymbols = m_SymInitialize(GetCurrentProcess(), NULL, TRUE);
					if (initsymbols == TRUE)
					{
						SetErrorMode(prevErrMode);
						m_initialized = true;
					}
				}
			}
		}
		LeaveCriticalSection(&m_lock);
	}

	if (!m_initialized)
	{
		if(m_DbgHelpDll)
			FreeLibrary(m_DbgHelpDll);
		if(m_Kernel32Dll)
			FreeLibrary(m_Kernel32Dll);
		m_DbgHelpDll = 0;
		m_Kernel32Dll = 0;
	}
}

void bqStackTracerImpl::DumpStackTrace(const size_t* trace, int maxtrace)
{
	_init();

	SYMBOL_INFO_PACKAGE sym;
	sym.si.SizeOfStruct = sizeof(sym.si);
	sym.si.MaxNameLen = sizeof(sym.name);

	HANDLE curproc = GetCurrentProcess();

	for (int i = 0; i < maxtrace; ++i)
	{
		uint64_t symaddress = trace[i];

		DWORD64 displacement = 0;
		BOOL ok_symbol = m_SymFromAddr(
			curproc,
			DWORD64(symaddress),
			&displacement,
			&sym.si);


		if (!ok_symbol)
			::strcpy_s(sym.si.Name, 2000, "(unknown)");
		IMAGEHLP_LINE64 line;
		::memset(&line, 0, sizeof(line));
		line.SizeOfStruct = sizeof(line);

		m_SymGetLineFromAddr64(
			curproc,
			DWORD64(symaddress),
			&displacement,
			&line);

		char tr[2048];
		_snprintf_s(tr, 2048, "%s(%i):'%s'\n", line.FileName, line.LineNumber, sym.si.Name);
		//printf(trace);
		bqLog::Print("%s\n", tr);
	}
}

int bqStackTracerImpl::GetStackTrace(size_t* trace, int maxtrace, int framesToSkip)
{
	_init();
	if (m_RtlCaptureStackBackTrace)
		return m_RtlCaptureStackBackTrace(framesToSkip, maxtrace, (PVOID*)trace, NULL);
	return 0;
}


static bqStackTracerImpl g_stackTracer;

void bqStackTracer::Print()
{
	size_t t[100];
	g_stackTracer.DumpStackTrace(t, g_stackTracer.GetStackTrace(t, 100, 0));
}

