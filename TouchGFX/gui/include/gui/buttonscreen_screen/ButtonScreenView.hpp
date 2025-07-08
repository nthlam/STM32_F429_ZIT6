#ifndef BUTTONSCREENVIEW_HPP
#define BUTTONSCREENVIEW_HPP

#include <gui_generated/buttonscreen_screen/ButtonScreenViewBase.hpp>
#include <gui/buttonscreen_screen/ButtonScreenPresenter.hpp>
#include <cstdio>
#include "stm32f4xx_hal.h"
#include "main.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))
class ButtonScreenView: public ButtonScreenViewBase {
public:
	ButtonScreenView();
	virtual ~ButtonScreenView() {
	}
	virtual void setupScreen();
	virtual void tearDownScreen();
	virtual void handleTickEvent();
	virtual void updateCursorPositionText();
	void updateHighScoreDisplay();
	void checkAndUpdateHighScore(int newScore);

	void tickEvent();
	virtual void buttonHandle();
	int calculateScore(int cursorX);
	float calculateDynamicSpeed(int cursorX);
	virtual void pauseButtonClicked();
	void updateCursor();
	// Virtual functions từ TouchGFX Designer interactions
	virtual void readyButtonClicked();
	virtual void retryButtonClicked();

	void startCountdown();
	// Modal window control functions
	void showModal();
	void hideModal();
	void resetModalState();

	void startGame();
	void stopGame();
	void resetGame();
	static void setHighScore(int score);
	static int getHighScore();
protected:

private:
	// Game States
	enum GameState {
		GAME_PREPARING,    // Modal showing, countdown not started
		GAME_COUNTDOWN,    // Countdown active
		GAME_PLAYING,      // Game active, cursor moving
		GAME_ENDED,        // Game finished, cursor stopped
		GAME_PAUSED        // Game paused/stopped
	};
	// Countdown step counter
	enum CountdownState {
		WAITING_FOR_READY,
		COUNTDOWN_READY,
		COUNTDOWN_SET,
		COUNTDOWN_STRIKE,
		COUNTDOWN_FINISHED
	};

	// State management
	GameState gameState;
	CountdownState currentState;
	bool isModalVisible;
	bool isCountdownActive;
	bool isGameActive;  // NEW: Controls cursor movement
	// Timing using your existing tickCount
	uint32_t countdownStartTick;
	uint32_t framesPerSecond; // Adjust based on your game FPS
	bool cursorStopped;     // NEW: Simple flag to stop cursor

	uint32_t tickCount;
	uint16_t localImage1; //địa chỉ con trỏ
	int cursorDirection;    // Hướng di chuyển: 1 = phải, -1 = trái
	int minX;              // Tọa độ x tối thiểu
	int maxX;              // Tọa độ x tối đa
	int peakX;          // Vị trí đỉnh (117)
	int maxScore;       // Điểm tối đa
	int minScore;       // Điểm tối thiểu
	float sigma;        // Độ rộng đường cong Gaussian
	int baseAnimationSpeed;    // Tốc độ cơ bản (không dùng nữa)
	float minSpeed;            // Tốc độ tối thiểu
	float maxSpeed;            // Tốc độ tối đa

	// Countdown functions
	void stepCountdown(); // Function you're already calling

	// UI update functions
	void showReadyState();
	void showSetState();
	void showStrikeState();
	void hideAllCountdownTexts();
};

#endif // BUTTONSCREENVIEW_HPP
