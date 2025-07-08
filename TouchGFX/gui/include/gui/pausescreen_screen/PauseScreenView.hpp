#ifndef PAUSESCREENVIEW_HPP
#define PAUSESCREENVIEW_HPP

#include <gui_generated/pausescreen_screen/PauseScreenViewBase.hpp>
#include <gui/pausescreen_screen/PauseScreenPresenter.hpp>

class PauseScreenView : public PauseScreenViewBase
{
public:
    PauseScreenView();
    virtual ~PauseScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    // Enum để định nghĩa các màn hình trước đó
    enum PreviousScreen
    {
        BUTTON_SCREEN,
        LOADCELL_SCREEN
    };

    // Phương thức để set màn hình trước đó
    static void setPreviousScreen(PreviousScreen screen);

    // Handler cho nút Cancel
    virtual void resumeButtonClicked();

protected:
    static PreviousScreen previousScreen; // Biến static để lưu màn hình trước
};

#endif // PAUSESCREENVIEW_HPP
