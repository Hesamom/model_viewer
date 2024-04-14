
#ifndef STRING_UTIL_H
#define STRING_UTIL_H

std::string convertWideToANSI(const std::wstring& wstr);
std::wstring convertAnsiToWide(const std::string& str);
std::string convertWideToUtf8(const std::wstring& wstr);
std::wstring convertUtf8ToWide(const std::string& str);

#endif //STRING_UTIL_H
