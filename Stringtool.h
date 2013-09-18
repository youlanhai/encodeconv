#pragma once

#include <string>
#include <vector>

#define LAZY_DEBUG_MSG

namespace Lazy
{
    
    typedef std::vector<std::wstring> StringArray;

    ///分隔字符串
    void splitstring(StringArray & out, const std::wstring & str, wchar_t ch);
}

