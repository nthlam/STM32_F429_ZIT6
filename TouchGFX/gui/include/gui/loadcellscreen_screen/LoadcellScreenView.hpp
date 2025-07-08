#ifndef LOADCELLSCREENVIEW_HPP
#define LOADCELLSCREENVIEW_HPP

#include <gui_generated/loadcellscreen_screen/LoadcellScreenViewBase.hpp>
#include <gui/loadcellscreen_screen/LoadcellScreenPresenter.hpp>

class LoadcellScreenView: public LoadcellScreenViewBase {
public:
	LoadcellScreenView();
	virtual ~LoadcellScreenView() {
	}
	virtual void setupScreen();
	virtual void tearDownScreen();
	virtual void pauseButtonClicked();
	virtual void handleTickEvent();
	void handle_loadcell();
	void update_display();

	void onRetryClicked();
protected:
	int32_t loadcell_offset;
	int tickCount;
	touchgfx::Callback<LoadcellScreenView,
			const touchgfx::AbstractButtonContainer&> retryButt2Callback;
	// Hàm callback đúng kiểu
	void retryButt2Clicked(const touchgfx::AbstractButtonContainer &btn);

	bool measuring = false;
	int32_t startTick = 0;
	int32_t maxForce = 0;
	enum PrepState {
		IDLE, READY, SET, GO, MEASURING
	};
	PrepState prepState = IDLE;
	uint32_t prepStartTick = 0;

	// Thêm biến thành viên trong class:
	int32_t currentDisplayScore = 0;
	bool doneDisplayingScore = false;
	//int32_t highScoreLoadcell = 0;

	// HX711 hx;

};

#endif // LOADCELLSCREENVIEW_HPP
