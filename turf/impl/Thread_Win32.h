/*------------------------------------------------------------------------
  Turf: Configurable C++ platform adapter
  Copyright (c) 2016 Jeff Preshing

  Distributed under the Simplified BSD License.
  Original location: https://github.com/preshing/turf

  This software is distributed WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the LICENSE file for more information.
------------------------------------------------------------------------*/

#ifndef TURF_IMPL_THREAD_WIN32_H
#define TURF_IMPL_THREAD_WIN32_H

#include <turf/Core.h>
#include <turf/Assert.h>

namespace turf {

#define TURF_THREAD_STARTCALL WINAPI

class Thread_Win32 {
private:
    HANDLE m_handle;

public:
    typedef DWORD ReturnType;
    typedef PTHREAD_START_ROUTINE StartRoutine;

    Thread_Win32() : m_handle(INVALID_HANDLE_VALUE) {
    }

    Thread_Win32(StartRoutine startRoutine, void* arg = NULL) {
        m_handle = CreateThread(NULL, 0, startRoutine, arg, 0, NULL);
    }

    Thread_Win32(void* (*startRoutine)(void*), void *arg=NULL)
        : Thread_Win32((PTHREAD_START_ROUTINE)startRoutine, arg) {
    }

    ~Thread_Win32() {
        if (m_handle != INVALID_HANDLE_VALUE)
            CloseHandle(m_handle);
    }

    bool isValid() const {
        return m_handle != INVALID_HANDLE_VALUE;
    }

    void join() {
        TURF_ASSERT(m_handle != INVALID_HANDLE_VALUE);
        WaitForSingleObject(m_handle, INFINITE);
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }

    void setName(const char *name) {
        TURF_ASSERT(strlen(name) < 16);
        // sucks, having to do this. why does windows not have an ascii API?
        wchar_t namebuf[16];
        MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, name, (int)strlen(name), namebuf, 16);
        SetThreadDescription(m_handle, namebuf);
    }

    void getName(char *buf, size_t len) {
        TURF_ASSERT(strlen(name) >= 16);
        // sucks, having to do this. why does windows not have an ascii API?
        wchar_t *wbuf = nullptr;
        HRESULT hr = GetThreadDescription(m_handle, &wbuf);
        if (SUCCEEDED(hr) && wbuf) {   
            WideCharToMultiByte(CP_UTF8, MB_PRECOMPOSED,
                wbuf, lstrlenW(wbuf),
                buf, (int)len, nullptr, nullptr);
            LocalFree(wbuf);
        }
    }

    void run(StartRoutine startRoutine, void* arg = NULL) {
        TURF_ASSERT(m_handle == INVALID_HANDLE_VALUE);
        m_handle = CreateThread(NULL, 0, startRoutine, arg, 0, NULL);
    }

    void run(void* (*startRoutine)(void*), void *arg=NULL) {
        run((PTHREAD_START_ROUTINE)startRoutine, arg);
    }

    static void sleepMillis(ureg millis) {
        Sleep((DWORD) millis);
    }
};

} // namespace turf

#endif // TURF_IMPL_THREAD_WIN32_H
