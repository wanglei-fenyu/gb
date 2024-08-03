#include <Windows.h>
#include <corecrt_io.h>
#include <fstream>
#include <string>


std::string utf8_to_string(const std::string& str) {
    int      nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    memset(pwBuf, 0, (size_t)nwLen * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)str.c_str(), static_cast<int>(str.length()), pwBuf, nwLen);

    int   nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);
    WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string ret = pBuf;
    delete[] pBuf;
    delete[] pwBuf;

    return ret;
}
std::string string_to_utf8(const std::string& str) {
    int      nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    memset(pwBuf, 0, nwLen * 2 + 2);
    MultiByteToWideChar(CP_ACP, 0, (LPCSTR)str.c_str(), static_cast<int>(str.length()), (LPWSTR)pwBuf, nwLen);

    int   nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);
    WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string ret = pBuf;
    delete[] pwBuf;
    delete[] pBuf;

    return ret;
}
std::string utf8_to_string(const char* str) { return utf8_to_string(std::string(str)); }
std::string string_to_utf8(const char* str) { return string_to_utf8(std::string(str)); }

#define U8(str)  string_to_utf8(str)
