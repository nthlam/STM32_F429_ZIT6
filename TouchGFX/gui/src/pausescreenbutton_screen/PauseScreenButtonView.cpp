#include <gui/pausescreenbutton_screen/PauseScreenButtonView.hpp>
#include <gui/common/FrontendApplication.hpp>

// Khởi tạo biến static
PauseScreenButtonView::PreviousScreen PauseScreenButtonView::previousScreen = BUTTON_SCREEN;

PauseScreenButtonView::PauseScreenButtonView()
{
}

void PauseScreenButtonView::setupScreen()
{
    PauseScreenButtonViewBase::setupScreen();
}

void PauseScreenButtonView::tearDownScreen()
{
    PauseScreenButtonViewBase::tearDownScreen();
}

void PauseScreenButtonView::setPreviousScreen(PreviousScreen screen)
{
    previousScreen = screen;
}

void PauseScreenButtonView::resumeButtonClicked()
{
    // Chuyển về màn hình trước đó dựa vào biến đã lưu
    switch(previousScreen)
    {
        case BUTTON_SCREEN:
            static_cast<FrontendApplication*>(Application::getInstance())->gotoButtonScreenScreenNoTransition();
            break;
        case LOADCELL_SCREEN:
            static_cast<FrontendApplication*>(Application::getInstance())->gotoLoadcellScreenScreenNoTransition();
            break;
        default:
            // Fallback về màn hình Button
            static_cast<FrontendApplication*>(Application::getInstance())->gotoButtonScreenScreenNoTransition();
            break;
    }
}
