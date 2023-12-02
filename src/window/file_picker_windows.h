
#ifndef MODEL_VIEWER_FILEPICKERWINDOWS_H
#define MODEL_VIEWER_FILEPICKERWINDOWS_H


class file_picker_windows {

private:
    std::string m_Title;
public:
    explicit file_picker_windows(const std::string& title);
    bool tryOpenPicker(std::string& path);
};


#endif //MODEL_VIEWER_FILEPICKERWINDOWS_H
