#pragma once
#include "Stringtool.h"

namespace Lazy
{
    
    ///打开文件
    FILE* openFile(const std::wstring & fname, const std::wstring & mode);

    ///查询文件长度。从当前位置到末尾的长度。
    long seekFileLength(FILE *pFile);

    ///是否是文件夹
    bool isdir(const std::wstring & fname);

    ///是否是文件
    bool isfile(const std::wstring & fname);

    ///创建文件夹
    bool mkdir(const std::wstring & path);

    ///列举目录文件
    bool listdir(StringArray & out, const std::wstring & path);
    
    ///格式化路径。将'\'格式化为'/'，尾部追加一个'/'
    void formatPath(std::wstring & fname);

    ///获取文件扩展名。不含'.'
    std::wstring getFileExt(const std::wstring & fname);
}

