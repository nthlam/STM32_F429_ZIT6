#include <gui/pausescreen_screen/PauseScreenView.hpp>
#include <gui/common/FrontendApplication.hpp>

// Khởi tạo biến static
PauseScreenView::PreviousScreen PauseScreenView::previousScreen = BUTTON_SCREEN;

PauseScreenView::PauseScreenView()
{
}

void PauseScreenView::setupScreen()
{
    PauseScreenViewBase::setupScreen();
}

void PauseScreenView::tearDownScreen()
{
    PauseScreenViewBase::tearDownScreen();
}

void PauseScreenView::setPreviousScreen(PreviousScreen screen)
{
    previousScreen = screen;
}

void PauseScreenView::resumeButtonClicked()
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
