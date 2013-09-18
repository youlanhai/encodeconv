#pragma once

#include <string>

namespace Lazy
{
    ///编码描述
    namespace Encode
    {
        const std::wstring none = L"";
        const std::wstring gbk = L"gbk";
        const std::wstring big5 = L"big5";
        const std::wstring utf7 = L"utf-7";
        const std::wstring utf8 = L"utf-8";
        const std::wstring ucs2 = L"ucs2";
        const std::wstring ucs_2le = L"ucs-2le";
        const std::wstring ucs_2be = L"ucs-2be";
    }

    ///code page.代码页
    namespace CP
    {
        const size_t none = 0;
        const size_t gbk = 936;
        const size_t big5 = 950;
        const size_t utf7 = 65000;
        const size_t utf8 = 65001;
        const size_t ucs_2le = 1200;
        const size_t ucs_2be = 1201;
        const size_t ucs2 = ucs_2le;
    }

    ///编码标志
    namespace EncodeMark
    {
        const char ucs_2le[] = "\xFF\xFE";
        const char ucs_2be[] = "\xFE\xFF";
        const char utf8[] = "\xEF\xBB\xBF";
        const size_t maxMarkLen = 4;
    }

    ///错误编码
    namespace Error
    {
        const size_t ok = 0;///<没有错误
        const size_t encodeFailed = 1;///<编码(转成多字节)失败
        const size_t decodeFailed = 2;///<解码(转成unicode)失败
        const size_t open = 3;///<打开文件失败
        const size_t readFile = 4;///<读取文件失败
        const size_t writeFile = 5;///<写入文件失败
        const size_t invalidEncode = 6;///<无效的编码
        const size_t invalidFilter = 7;///<无效的过滤器
        const size_t searchFailed = 8;///<搜索文件失败
        const size_t mkdir = 9;///<创建文件夹
    }

    ///编码描述转换到编码页
    const std::wstring & cpToCode(size_t cp);

    ///编码页转换到编码描述
    size_t codeToCp(const std::wstring & code);


    ///窄字符转换到宽字符
    bool charToWChar(std::wstring & dest, const std::string & str, size_t code);

    ///宽字符转换到窄字符
    bool wcharToChar(std::string & dest, const std::wstring & wstr, size_t code);


    ///用指定编码读取出字符串
    size_t readString(std::wstring & dest, FILE *pFile, size_t codePage);

    ///用指定编码写入字符串
    size_t writeString(const std::wstring & src, FILE *pFile, size_t codePage, bool bom);


    ///转换文件编码
    size_t convertFile(const std::wstring & destFile,
        const std::wstring & destEncode,
        const std::wstring & srcFile,
        const std::wstring & srcEncode,
        bool useBom);

    ///转换目录中所有文件的编码
    ///注意：递归模式下，destPath不能为srcPath的子目录。当然两者可以相等。
    size_t convertPath(const std::wstring & destPath,
        const std::wstring & destEncode,
        const std::wstring & srcPath,
        const std::wstring & srcEncode,
        bool useBom,
        const std::wstring & filter,
        bool rescursively);

}//end namespace Lazy