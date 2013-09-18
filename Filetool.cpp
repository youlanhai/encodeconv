#include "StdAfx.h"
#include "Filetool.h"

#include <cassert>
#include <algorithm>
#include <iostream>

#include <Windows.h>

namespace Lazy
{
    
    FILE* openFile(const std::wstring & fname, const std::wstring & mode)
    {
        FILE *pFile;
        errno_t eno = _wfopen_s(&pFile, fname.c_str(), mode.c_str());
        if(eno == 0) return pFile;

#ifdef LAZY_DEBUG_MSG
        std::wcout<<L"error: open file '"<<fname <<L"' failed! code:"<<eno <<std::endl;
#endif

        return NULL;
    }

    long seekFileLength(FILE *pFile)
    {
        assert(pFile && "seekFileLength");

        long cur = ftell(pFile);
        fseek(pFile, 0, SEEK_END);
        long length = ftell(pFile) - cur;
        fseek(pFile, cur, SEEK_SET);

        return length;
    }

    bool isdir(const std::wstring & fname)
    {
        DWORD attr = ::GetFileAttributes(fname.c_str());
        if(attr == INVALID_FILE_ATTRIBUTES) return false;

        return 0 != (attr & FILE_ATTRIBUTE_DIRECTORY);
    }

    bool isfile(const std::wstring & fname)
    {
        DWORD attr = ::GetFileAttributes(fname.c_str());
        if(attr == INVALID_FILE_ATTRIBUTES) return false;

        return 0 != (attr & FILE_ATTRIBUTE_ARCHIVE);
    }

    bool mkdir(const std::wstring & path)
    {
        return !!::CreateDirectory(path.c_str(), NULL);
    }

    bool listdir(StringArray & out, const std::wstring & path)
    {
        out.clear();

        std::wstring fname = path;
        formatPath(fname);
        fname += L'*';

        WIN32_FIND_DATA findData;
        HANDLE hFind = ::FindFirstFile(fname.c_str(), &findData);
        if(hFind == INVALID_HANDLE_VALUE) return false;

        do
        {
            const std::wstring & str = findData.cFileName;
            if(str != L"." && str != L"..")
            {
                out.push_back(str);
            }
        }while(::FindNextFile(hFind, &findData));

        ::FindClose(hFind);
        return true;
    }

    
    void formatPath(std::wstring & fname)
    {
        if(fname.empty()) return;
        
        std::replace(fname.begin(), fname.end(), L'\\', L'/');
        if(fname.back() != L'/') fname += L'/';
    }

    std::wstring getFileExt(const std::wstring & fname)
    {
        size_t pos = fname.find_last_of(L'.');
        if(pos != fname.npos) return fname.substr(pos + 1);

        return L"";
    }

}