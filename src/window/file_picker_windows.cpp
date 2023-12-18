
#include "file_picker_windows.h"
#include "Windows.h"

bool file_picker_windows::tryOpenPicker(std::string& path, const std::vector<file_filter>& filters) {


    char currentDir[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, currentDir);

    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;

    //TODO add all supported formats 
    std::string filter;
    
    for (const auto & f : filters) {
        filter += f.displayName + '\0' + "*." + f.format + '\0';
    }
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFile = path.data(); 
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrTitle = m_Title.data();
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn)) // user selected an input file
    {
        SetCurrentDirectoryA(currentDir);
        return true;
    }
    else
    {
        SetCurrentDirectoryA(currentDir);
        return false;
    }
}

file_picker_windows::file_picker_windows(const std::string &title) {
    m_Title = title;
}
