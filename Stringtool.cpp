#include "StdAfx.h"
#include "Stringtool.h"

namespace Lazy
{
    void splitstring(StringArray & out, const std::wstring & str, wchar_t ch)
    {
        size_t last = 0;
        size_t cur = 0;

        while(cur < str.size())
        {
            cur = str.find(ch, last);
            out.push_back(str.substr(last, cur-last));
            last = cur + 1;
        }
    }

}