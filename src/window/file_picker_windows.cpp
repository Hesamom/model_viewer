
#include "file_picker_windows.h"
#include "Windows.h"

bool file_picker_windows::tryOpenPicker(std::string& path) {

    
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = "FBX\0*.fbx\0";
    ofn.lpstrFile = path.data(); 
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrTitle = m_Title.data();
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    if (GetOpenFileNameA(&ofn)) // user selected an input file
    {
        return true;
    }
    else
    {
        return false;
    }
}

file_picker_windows::file_picker_windows(const std::string &title) {
    m_Title = title;
}
