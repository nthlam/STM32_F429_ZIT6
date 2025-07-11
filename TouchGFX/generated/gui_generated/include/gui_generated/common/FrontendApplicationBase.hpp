/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#ifndef FRONTENDAPPLICATIONBASE_HPP
#define FRONTENDAPPLICATIONBASE_HPP

#include <mvp/MVPApplication.hpp>
#include <gui/model/Model.hpp>

class FrontendHeap;

class FrontendApplicationBase : public touchgfx::MVPApplication
{
public:
    FrontendApplicationBase(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplicationBase() { }

    virtual void changeToStartScreen()
    {
        gotoStartScreenScreenNoTransition();
    }

    // StartScreen
    void gotoStartScreenScreenNoTransition();

    // SelectHardwareScreen
    void gotoSelectHardwareScreenScreenNoTransition();

    // ButtonScreen
    void gotoButtonScreenScreenNoTransition();

    // LoadcellScreen
    void gotoLoadcellScreenScreenNoTransition();

    // PauseScreen
    void gotoPauseScreenScreenNoTransition();

protected:
    touchgfx::Callback<FrontendApplicationBase> transitionCallback;
    FrontendHeap& frontendHeap;
    Model& model;

    // StartScreen
    void gotoStartScreenScreenNoTransitionImpl();

    // SelectHardwareScreen
    void gotoSelectHardwareScreenScreenNoTransitionImpl();

    // ButtonScreen
    void gotoButtonScreenScreenNoTransitionImpl();

    // LoadcellScreen
    void gotoLoadcellScreenScreenNoTransitionImpl();

    // PauseScreen
    void gotoPauseScreenScreenNoTransitionImpl();
};

#endif // FRONTENDAPPLICATIONBASE_HPP
