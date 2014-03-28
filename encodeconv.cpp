// encodeconv.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Encode.h"

#include <iostream>


std::wstring usage = \
    L"encodeconv -f fname -s src encode [-d][dest encode] [-o][output file] [-nb]\n"
    L"encodeconv -p path -e extensions -s src encode [-d][dest encode] [-o][output path] [-r] [-nb]\n\n"
    L"-e: extension of target files. eg. txt;h;cpp\n"
    L"-s: the source file encode. eg. gbk\n"
    L"-d: the dest file encode. default is utf-8\n"
    L"-o: the output file or path. default is same as the input file or path."
    L"-r: convert rescursively.\n"
    L"-nb: don't add bom.\n\n"
    L"eg. convert a file: encodeconv -f test.txt -s gbk -d utf-8 -o test2.txt\n"
    L"eg. convert files in path: encodeconv -p c:/test/ -e txt;h;cpp -s gbk -d utf-8 -o d:/test/ \n"
    ;

///查找参数名称
int findArgN(const wchar_t *cmd, int argc, wchar_t* argv[])
{
    for(int i=1; i<argc; ++i)
    {
        if(wcscmp(cmd, argv[i]) == 0) return i;
    }
    return -1;
}

///查找参数值
int findArgV(const wchar_t *cmd, int argc, wchar_t* argv[])
{
    int index = findArgN(cmd, argc, argv);
    if(index < 0) return -1;
    
    index += 1;
    if(index >= argc) return -1;

    return index;
}

int wmain(int argc, wchar_t* argv[])
{
    if(argc < 3)
    {
        std::wcout<< usage <<std::endl;
        return 0;
    }

    std::wstring filter;

    bool isForFile;
    if(wcscmp(L"-f", argv[1]) == 0)
    {
        isForFile = true;
    }
    else if(wcscmp(L"-p", argv[1]) == 0)
    {
        isForFile = false;
    }
    else
    {
        std::wcout<< usage <<std::endl;
        return 0;
    }
    
    std::wstring srcName = argv[2];
    std::wstring destName;

    //find output file name.
    int index = findArgV(L"-o", argc, argv);
    if(index >= 0) destName = argv[index];
    else destName = srcName;

    //find src file encode.
    std::wstring srcEncode;
    index = findArgV(L"-s", argc, argv);
    if(index < 0)
    {
        std::wcout << "please input the source encoding with '-r encoding'!" << std::endl;
        return 0;
    }
    srcEncode = argv[index];

    //find dest file encode.
    std::wstring destEncode = L"utf-8";
    index = findArgV(L"-d", argc, argv);
    if(index >= 0)
    {
        destEncode = argv[index];
    }

    bool rescursively = false;
    if(findArgN(L"-r", argc, argv) >= 0)
    {
        rescursively = true;
    }

    bool useBom = true;
    if(findArgN(L"-nb", argc, argv) >= 0)
    {
        useBom = false;
    }

    size_t result;
    if(isForFile)
    {

#ifdef LAZY_DEBUG_MSG
        std::wcout<<L"convertFile: "
            <<destName <<L", "
            <<destEncode <<L", "
            <<srcName <<L", "
            <<srcEncode <<L", "
            <<std::endl;
#endif

        result = Lazy::convertFile(destName, destEncode, srcName, srcEncode, useBom);

    }
    else
    {
        //parse the target files extension
        index = findArgV(L"-e", argc, argv);
        if(index < 0)
        {
            std::wcout << "please input extensions of the taget files with '-e extesions'." << std::endl;
            return 0;
        }
        filter = argv[index];

#ifdef LAZY_DEBUG_MSG
        std::wcout<<L"convertPath: "
            <<destName <<L", "
            <<destEncode <<L", "
            <<srcName <<L", "
            <<srcEncode <<L", "
            <<filter <<L", "
            <<rescursively <<L", "
            <<std::endl;
#endif

        result = Lazy::convertPath(destName, destEncode, srcName, srcEncode, useBom, filter, rescursively);
    }

    if(result == Lazy::Error::ok)
    {
        std::wcout<<L"info: convert '"<<srcName
            <<"' to '"<< destName <<"' success." <<std::endl;
    }
    else
    {
        std::wcout<<L"error: convert '"<<srcName
            <<"' to '"<< destName <<"' failed! code:" <<result <<std::endl;
    }
    

	return 0;
}

