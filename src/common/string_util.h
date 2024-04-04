
#ifndef STRING_UTIL_H
#define STRING_UTIL_H

std::string ConvertWideToANSI(const std::wstring& wstr);
std::wstring ConvertAnsiToWide(const std::string& str);
std::string ConvertWideToUtf8(const std::wstring& wstr);
std::wstring ConvertUtf8ToWide(const std::string& str);

#endif //STRING_UTIL_H
