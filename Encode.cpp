#include "StdAfx.h"
#include "Encode.h"
#include "Stringtool.h"
#include "Filetool.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include <Windows.h>


namespace Lazy
{
    
    const std::wstring & cpToCode(size_t cp)
    {
        if(cp == CP::utf7) return Encode::utf7;
        else if(cp == CP::utf8) return Encode::utf8;
        else if(cp == CP::gbk) return Encode::gbk;
        else if(cp == CP::big5) return Encode::big5;
        else if(cp == CP::ucs_2le) return Encode::ucs_2le;
        else if(cp == CP::ucs_2be) return Encode::ucs_2be;
        else if(cp == CP::ucs2) return Encode::ucs2;
        else return Encode::none;
    }

    size_t codeToCp(const std::wstring & code)
    {
        if(code == Encode::utf7) return CP::utf7;
        else if(code == Encode::utf8) return CP::utf8;
        else if(code == Encode::gbk) return CP::gbk;
        else if(code == Encode::big5) return CP::big5;
        else if(code == Encode::ucs_2le) return CP::ucs_2le;
        else if(code == Encode::ucs_2be) return CP::ucs_2be;
        else if(code == Encode::ucs2) return CP::ucs2;
        else return CP::none;
    }

    bool charToWChar(std::wstring & dest, const std::string & str, size_t code)
    {
        int lLen = MultiByteToWideChar(code, 0, str.c_str(), -1, NULL, 0); 

        dest.resize(lLen, 0);

        lLen = MultiByteToWideChar(code, 0, str.c_str(), -1, &dest[0], lLen);
        if(lLen <= 0) return false;

        dest.erase(lLen - 1);
        return true;
    }

    bool wcharToChar(std::string & dest, const std::wstring & wstr, size_t code)
    {
        int lLen = WideCharToMultiByte(code, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
        dest.resize(lLen);

        lLen = WideCharToMultiByte(code, 0, wstr.c_str(), -1, &dest[0], lLen, NULL, NULL);
        if(lLen <= 0) return false;

        dest.erase(lLen - 1);
        return true;
    }

    bool cmpEncodeMark(const char *eMark, const char *mark)
    {
        assert(eMark && mark);

        int i = 0;
        while(eMark[i] != 0)
        {
            if(0 == mark[i]) return false;
            if(eMark[i] != mark[i]) return false;

            ++i;
        }

        return true;
    }

    size_t readString(std::wstring & dest, FILE *pFile, size_t codePage)
    {
        assert(pFile && "readString");

        long curFPos = ftell(pFile);

        //尝试读取文件编码头
        char mark[EncodeMark::maxMarkLen+1];
        fread(mark, EncodeMark::maxMarkLen, 1, pFile);
        fseek(pFile, curFPos, SEEK_SET);

        //优先使用文件编码头
        if(cmpEncodeMark(EncodeMark::utf8, mark))
        {
            codePage = CP::utf8;
            fseek(pFile, strlen(EncodeMark::utf8), SEEK_CUR);
        }
        else if(cmpEncodeMark(EncodeMark::ucs_2le, mark))
        {
            codePage = CP::ucs_2le;
            fseek(pFile, strlen(EncodeMark::ucs_2le), SEEK_CUR);
        }
        else if(cmpEncodeMark(EncodeMark::ucs_2be, mark))
        {
            codePage = CP::ucs_2be;
            fseek(pFile, strlen(EncodeMark::ucs_2be), SEEK_CUR);
        }

        dest.clear();
        const long length = seekFileLength(pFile);
        if(length == 0)return Error::ok;
        
        if(codePage == CP::ucs_2le)
        {
            size_t n = (length + 1) / 2;
            dest.resize(n);
            n = fread(&dest[0], 2, n, pFile);
            dest.erase(n);

            if(n == 0) return Error::readFile;
        }
        else if(codePage == CP::ucs_2be)
        {
            std::string buffer(length, 0);
            size_t n = fread(&buffer[0], 1, length, pFile);
            buffer.erase(n);

            if(n == 0) return Error::readFile;

            wchar_t ch;
            size_t ulen = n / 2;
            dest.resize(ulen);
            for(size_t i=0; i<ulen; ++i)
            {
                ch = (unsigned char)buffer[i*2];
                ch <<= 8;
                ch |= (unsigned char)buffer[i*2+1];
                dest[i] = ch;
            }
        }
        else
        {
            std::string buffer(length, 0);
            size_t n = fread(&buffer[0], 1, length, pFile);
            buffer.erase(n);

            if(n == 0) return Error::readFile;

            if(!charToWChar(dest, buffer, codePage)) 
                return Error::decodeFailed;
        }

        return Error::ok;
    }

    size_t writeString(const std::wstring & src, FILE *pFile, size_t codePage, bool bom)
    {
        assert(pFile && "writeString");

        if(codePage == CP::ucs_2le)
        {
            //写入编码头
            if(bom) fwrite(&EncodeMark::ucs_2le, 1, strlen(EncodeMark::ucs_2le), pFile);

            if(fwrite(&src[0], 2, src.size(), pFile) != src.size())
                return Error::writeFile;
        }
        else if(codePage == CP::ucs_2be)
        {
            if(bom) fwrite(&EncodeMark::ucs_2be, 1, strlen(EncodeMark::ucs_2be), pFile);

            //转换成big endian
            size_t n = src.size();
            std::string buffer(n * 2, 0);
            
            wchar_t ch;
            for(size_t i=0; i<n; ++i)
            {
                ch = src[i];
                buffer[i*2] = (ch >> 8) & 0xff;
                buffer[i*2 + 1] = ch & 0xff;
            }

            if(fwrite(&buffer[0], 1, buffer.size(), pFile) != buffer.size())
                return Error::writeFile;
        }
        else
        {
            //写入编码头
            if(bom && codePage==CP::utf8) 
            {
                fwrite(EncodeMark::utf8, 1, strlen(EncodeMark::utf8), pFile);
            }

            std::string buffer;
            if(!wcharToChar(buffer, src, codePage))
                return Error::encodeFailed;

            if(fwrite(&buffer[0], 1, buffer.size(), pFile) != buffer.size())
                return Error::writeFile;
        }

        return Error::ok;
    }

    size_t convertFile(const std::wstring & destFile,
        const std::wstring & destEncode,
        const std::wstring & srcFile,
        const std::wstring & srcEncode,
        bool useBom)
    {
        size_t srcCP = codeToCp(srcEncode);
        size_t destCP = codeToCp(destEncode);

        if(destCP == CP::none) return Error::invalidEncode;

        std::wstring buffer;

        {
            FILE* pFile = openFile(srcFile, L"rb");
            if(pFile == nullptr) 
                return Error::open;

            size_t result = readString(buffer, pFile, srcCP);
            fclose(pFile);

            if(result != Error::ok) return result;
        }

        {
            FILE *pFile = openFile(destFile, L"wb");
            if(pFile == nullptr) 
                return Error::open;

            size_t result = writeString(buffer, pFile, destCP, useBom);
            fclose(pFile);

            if(result != Error::ok) return result;
        }

        return Error::ok;
    }

    size_t _convertPath(const std::wstring & destPath,
        const std::wstring & destEncode,
        const std::wstring & srcPath,
        const std::wstring & srcEncode,
        bool useBom,
        const StringArray & filter,
        bool rescursively)
    {
        std::wcout<<L"info: convert dir '"<<srcPath <<"' to '"<< destPath <<std::endl;

        ///先搜索目录
        StringArray files;
        if(!listdir(files, srcPath)) return Error::searchFailed;

        if(files.empty())
        {
            std::wcout<<L"info: dir is empty."<<std::endl;
            return Error::ok;
        }

        //建立目标文件夹
        if(!isdir(destPath))
        {
            std::wcout<<L"mkdir "<< destPath <<std::endl;
            if(!mkdir(destPath)) return Error::mkdir;
        }

        for(size_t i=0; i<files.size(); ++i)
        {
            std::wstring srcFile = srcPath + files[i];
            std::wstring destFile = destPath + files[i];
            size_t result = Lazy::Error::ok;

            //std::wcout<<L"trace: '"<<srcFile <<"' to '"<< destFile <<std::endl;

            if(isdir(srcFile) && rescursively)
            {
                srcFile += L'/';
                destFile += L'/';
                result = _convertPath(destFile, destEncode, srcFile, srcEncode, useBom, filter, rescursively);
                if(result != Lazy::Error::ok)
                {
                    std::wcout<<L"error: '"<<srcFile
                        <<"' to '"<< destFile <<"' failed! code:" <<result <<std::endl;
                }
            }
            else
            {
                std::wstring ext = getFileExt(srcFile);
                if(std::find(filter.begin(), filter.end(), ext) != filter.end())
                {
                    std::wstring destFile = destPath + files[i];
                    result = convertFile(destFile, destEncode, srcFile, srcEncode, useBom);

                    if(result == Lazy::Error::ok)
                    {
                        std::wcout<<L"info: '"<<srcFile
                            <<"' to '"<< destFile <<"' success." <<std::endl;
                    }
                    else
                    {
                        std::wcout<<L"error: '"<<srcFile
                            <<"' to '"<< destFile <<"' failed! code:" <<result <<std::endl;
                    }
                }
            }
        }

        return Error::ok;
    }

    size_t convertPath(const std::wstring & destPath,
        const std::wstring & destEncode,
        const std::wstring & srcPath,
        const std::wstring & srcEncode,
        bool useBom,
        const std::wstring & filter,
        bool rescursively)
    {
        StringArray filterArray;
        splitstring(filterArray, filter, L';');
        if(filterArray.empty()) return Error::invalidFilter;

        std::wstring spath = srcPath;
        std::wstring dpath = destPath;
        formatPath(spath);
        formatPath(dpath);

        return _convertPath(dpath, destEncode, spath, srcEncode, useBom, filterArray, rescursively);
    }
}