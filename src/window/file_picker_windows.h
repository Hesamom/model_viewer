
#ifndef MODEL_VIEWER_FILEPICKERWINDOWS_H
#define MODEL_VIEWER_FILEPICKERWINDOWS_H


struct file_filter {
    std::string displayName;
    std::string format;
};

class file_picker_windows {

private:
    std::string m_Title;
public:
    explicit file_picker_windows(const std::string& title);
    bool tryOpenPicker(std::string& path, const std::vector<file_filter>& filters);
};


#endif //MODEL_VIEWER_FILEPICKERWINDOWS_H
