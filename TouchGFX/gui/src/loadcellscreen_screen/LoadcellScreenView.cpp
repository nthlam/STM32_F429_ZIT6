#include <gui/loadcellscreen_screen/LoadcellScreenView.hpp>
#include <gui/pausescreen_screen/PauseScreenView.hpp>
#include "cmsis_os2.h"
extern osMessageQueueId_t loadcell_queueHandle;
#include <cmath>
#include "main.h"
#include "stm32f4xx_hal.h"
#include <cstring>
#include <cstdio>
#include "main.h"

extern int32_t highScoreLoadcell;

LoadcellScreenView::LoadcellScreenView() :
		retryButt2Callback(this, &LoadcellScreenView::retryButt2Clicked) {
}

void LoadcellScreenView::setupScreen() // gọi 1 lần khi màn hình hiển thị
{
	LoadcellScreenViewBase::setupScreen();
	retryButt2.setAction(retryButt2Callback); // gán hàm retryButt2Callback với nút bấm
}

void LoadcellScreenView::tearDownScreen() // được dùng 1 lần khi rời màn hình => dùng giair phóng biến, clear trạng thái
{
	LoadcellScreenViewBase::tearDownScreen();
}

void LoadcellScreenView::handle_loadcell() {
	// Biến để nhận dữ liệu từ queue
	osStatus_t res;        // Kết quả trả về từ osMessageQueueGet

	if (osMessageQueueGetCount(loadcell_queueHandle) > 0) {
		// Đọc dữ liệu từ queue
		char msg[64];
		res = osMessageQueueGet(loadcell_queueHandle, &loadcell_offset, NULL,
				0);
		//sprintf(msg, "Gia tri sau queue-----: %d \r\n", loadcell_offset);
		//HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
		if (res == osOK) {
			// Nếu đọc thành công, cập nhật màn hình
			update_display();
		}
	}

}
//
//void LoadcellScreenView::update_display() {
//	int currentScore = loadcell_offset;
//
//		// Chuyển điểm số thành chuỗi
//		char scoreBuffer[16];
//		snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", currentScore);
//
//		// Cập nhật text hiển thị
//		Unicode::strncpy(scorelcBuffer, scoreBuffer, SCORELC_SIZE);
//		scorelc.setWildcard(scorelcBuffer);
//		scorelc.invalidate(); // Đánh dấu để vẽ lại text
////	if (measuring) {
////		int force = loadcell_offset;
////		if (force > maxForce)
////			maxForce = force;
////
////		if (HAL_GetTick() - startTick >= 2000)  // hết 3 giây
////				{
////			measuring = false;
////
////			// Cập nhật điểm hiển thị
////			char scoreBuffer[16];
////			snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", maxForce);
////			Unicode::strncpy(scorelcBuffer, scoreBuffer, SCORELC_SIZE);
////			scorelc.setWildcard(scorelcBuffer);
////			scorelc.invalidate();  // cập nhật UI
////		}
////	}
//
//}
void LoadcellScreenView::update_display() {
	if (measuring) {
		int32_t force = loadcell_offset;
		if (force > maxForce)
			maxForce = force;
		// Nếu đã hết 5 giây thì dừng
		if (HAL_GetTick() - startTick >= 2000) {
			measuring = false;

			// Hiển thị điểm số cuối cùng
			char scoreBuffer[16];
			snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", maxForce);
			Unicode::strncpy(scorelcBuffer, scoreBuffer, SCORELC_SIZE);
			scorelc.setWildcard(scorelcBuffer);
			scorelc.invalidate();
			if (maxForce > highScoreLoadcell) {
				highScoreLoadcell = maxForce;
				char highScoreBuffer[16];
				snprintf(highScoreBuffer, sizeof(highScoreBuffer), "%d",
						maxForce);
				Unicode::strncpy(highscorelcBuffer, highScoreBuffer,
						HIGHSCORELC_SIZE);
				highscorelc.setWildcard(highscorelcBuffer);
				highscorelc.invalidate();
			}

		}
	}
}

//void LoadcellScreenView::retryButt2Clicked(
//		const touchgfx::AbstractButtonContainer &btn) {
//	// mỗi lần bấm thì sẽ set chữ về 0
//	Unicode::strncpy(scorelcBuffer, "0", SCORELC_SIZE);
//	scorelc.setWildcard(scorelcBuffer);
//	scorelc.invalidate();
//	// Bắt đầu đo lực ở đây
//	measuring = true;
//	maxForce = 0;
//	startTick = HAL_GetTick();
//}

/// HÀM READY SET GO
void LoadcellScreenView::retryButt2Clicked(
		const touchgfx::AbstractButtonContainer &btn) {
	// Reset hiển thị
	Unicode::strncpy(scorelcBuffer, "READY", SCORELC_SIZE);
	scorelc.setWildcard(scorelcBuffer);
	scorelc.invalidate();

	prepState = READY;
	prepStartTick = HAL_GetTick();
}

void LoadcellScreenView::handleTickEvent() { // hàm được gọi lặp lại liên tục (60hz)
	tickCount++;
	handle_loadcell();

	// XỬ KÍ LẦN LƯỢT CÁC STATES
	uint32_t now = HAL_GetTick();

	switch (prepState) {
	case READY:
		if (now - prepStartTick >= 1000) {
			Unicode::strncpy(scorelcBuffer, "SET", SCORELC_SIZE);
			scorelc.setWildcard(scorelcBuffer);
			scorelc.invalidate();
			prepState = SET;
			prepStartTick = now;
		}
		break;
	case SET:
		if (now - prepStartTick >= 1000) {
			char scoreBuffer[16];
			snprintf(scoreBuffer, sizeof(scoreBuffer), "%s", "GO!!!");
			Unicode::strncpy(scorelcBuffer, scoreBuffer, SCORELC_SIZE);
			scorelc.setWildcard(scorelcBuffer);
			scorelc.invalidate();
			prepState = GO;
			prepStartTick = now;
		}
		break;
	case GO:
		if (now - prepStartTick >= 500) {
			// Bắt đầu đo lực
			measuring = true;
			maxForce = 0;
			startTick = HAL_GetTick();
			prepState = MEASURING;
		}
		break;
	default:
		break;
	}
}

/*
 CÁC VẤN ĐỀ:
 + high Score
 + điểm chưa scale

 handleTickEvent
 retryButt2Clicked
 update_display
 handle_loadcell
 -----------
 hàm handleTickEvent -> handle_loadcell (lấy dữ liệu từ queue)
 -> update_display (nếu measuring = true thì sẽ tính max_value, khi hết 3s thì set
 measuring = false)
 --------
 gán hàm CallBack cho butt2
 khi bấm thì sẽ set measuring = true -> khi đo sẽ so sánh



 */
void LoadcellScreenView::pauseButtonClicked() {
	// Set màn hình hiện tại trước khi chuyển
	PauseScreenView::setPreviousScreen(PauseScreenView::LOADCELL_SCREEN);

	// Chuyển đến màn hình pause
	static_cast<FrontendApplication*>(Application::getInstance())->gotoPauseScreenScreenNoTransition();
}
