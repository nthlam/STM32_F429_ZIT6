#ifndef SELECTHARDWARESCREENVIEW_HPP
#define SELECTHARDWARESCREENVIEW_HPP

#include <gui_generated/selecthardwarescreen_screen/SelectHardwareScreenViewBase.hpp>
#include <gui/selecthardwarescreen_screen/SelectHardwareScreenPresenter.hpp>

class SelectHardwareScreenView : public SelectHardwareScreenViewBase
{
public:
    SelectHardwareScreenView();
    virtual ~SelectHardwareScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SELECTHARDWARESCREENVIEW_HPP
