#ifndef SELECTHARDWARESCREENPRESENTER_HPP
#define SELECTHARDWARESCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class SelectHardwareScreenView;

class SelectHardwareScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    SelectHardwareScreenPresenter(SelectHardwareScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~SelectHardwareScreenPresenter() {}

private:
    SelectHardwareScreenPresenter();

    SelectHardwareScreenView& view;
};

#endif // SELECTHARDWARESCREENPRESENTER_HPP
