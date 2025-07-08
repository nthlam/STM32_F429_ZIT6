#ifndef BUTTONSCREENPRESENTER_HPP
#define BUTTONSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ButtonScreenView;

class ButtonScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    ButtonScreenPresenter(ButtonScreenView& v);

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

    virtual ~ButtonScreenPresenter() {}

private:
    ButtonScreenPresenter();

    ButtonScreenView& view;
};

#endif // BUTTONSCREENPRESENTER_HPP
