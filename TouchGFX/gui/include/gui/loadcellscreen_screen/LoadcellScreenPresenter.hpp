#ifndef LOADCELLSCREENPRESENTER_HPP
#define LOADCELLSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class LoadcellScreenView;

class LoadcellScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    LoadcellScreenPresenter(LoadcellScreenView& v);

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

    virtual ~LoadcellScreenPresenter() {}

private:
    LoadcellScreenPresenter();

    LoadcellScreenView& view;
};

#endif // LOADCELLSCREENPRESENTER_HPP
