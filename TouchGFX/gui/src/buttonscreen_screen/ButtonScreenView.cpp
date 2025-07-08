#include <gui/buttonscreen_screen/ButtonScreenView.hpp>
#include <gui/pausescreen_screen/PauseScreenView.hpp>
#include <cmath>
#ifndef SIMULATOR
#include "cmsis_os2.h"
#include "main.h"
extern osMessageQueueId_t buttonQueueHandle;
#endif
extern int g_highScore;
ButtonScreenView::ButtonScreenView() {
	cursorDirection = 1;
	baseAnimationSpeed = 2;    // Tốc độ cơ bản
	minSpeed = 1;              // Tốc độ tối thiểu (ở 2 đầu)
	maxSpeed = 8;              // Tốc độ tối đa (ở giữa)
	minX = 20;
	maxX = 214;
	peakX = 117;
	tickCount = 0;

	// Tham số cho hàm tính điểm
	maxScore = 1000;
	minScore = 0;
	sigma = 30.0f;

	currentState = WAITING_FOR_READY;
	isModalVisible = false;
	isCountdownActive = false;
	countdownStartTick = 0;
	framesPerSecond = 60;
}

void ButtonScreenView::setupScreen() {
	ButtonScreenViewBase::setupScreen();

// Đảm bảo cursor bắt đầu trong phạm vi cho phép
	localImage1 = 117; // Bắt đầu từ vị trí x = 20
	updateHighScoreDisplay();
	resetGame();
	resetModalState();
	showModal();

}
void ButtonScreenView::buttonHandle() {
	uint32_t buttonEvent;  // Biến để nhận dữ liệu từ queue
	osStatus_t res;        // Kết quả trả về từ osMessageQueueGet

	if (osMessageQueueGetCount(buttonQueueHandle) > 0) {
		// Đọc dữ liệu từ queue
		res = osMessageQueueGet(buttonQueueHandle, &buttonEvent, NULL, 0);

		if (res == osOK) {
			// Nếu đọc thành công, cập nhật màn hình
			updateCursorPositionText();
			stopGame();
		}
	}
}

void ButtonScreenView::tearDownScreen() {
	isCountdownActive = false;
	ButtonScreenViewBase::tearDownScreen();
}
void ButtonScreenView::handleTickEvent() {
	tickCount++;

	// Always handle countdown if active
	if (isCountdownActive) {
		stepCountdown();
	}

	// Only run game logic when game is active (after countdown)
	if (isGameActive && gameState == GAME_PLAYING) {
		buttonHandle();
		updateCursor();
	}
}

void ButtonScreenView::updateCursor() {
	int oldX = localImage1;

	// Cập nhật vị trí cursor mỗi tick
	float currentSpeed = calculateDynamicSpeed(localImage1);

	localImage1 += (int) (cursorDirection * currentSpeed);

	// Kiểm tra và đảo chiều khi đạt giới hạn
	if (localImage1 >= maxX) {
		localImage1 = maxX;
		cursorDirection = -1; // Đổi chiều sang trái
	} else if (localImage1 <= minX) {
		localImage1 = minX;
		cursorDirection = 1; // Đổi chiều sang phải
	}

	// Chỉ cập nhật nếu vị trí thay đổi
	if (oldX != localImage1) {
		// Xóa vùng cũ trước
		touchgfx::Rect oldRect = image1.getRect();
		invalidateRect(oldRect);

		// Cập nhật vị trí mới
		image1.setX(localImage1);

		// Đánh dấu vùng cần vẽ lại
		image1.invalidate();
		//updateCursorPositionText();
	}
}

// Hàm tính tốc độ động theo hàm -x² (nhanh ở giữa, chậm ở hai đầu)
float ButtonScreenView::calculateDynamicSpeed(int cursorX) {
// Tính khoảng cách từ vị trí hiện tại đến đỉnh (117)
	float distanceFromPeak = abs(cursorX - peakX);
	float maxDistance = MAX(abs(minX - peakX), abs(maxX - peakX)); // = 97

// Chuẩn hóa khoảng cách về [0, 1]
	float normalizedDistance = distanceFromPeak / maxDistance;

// Áp dụng hàm -x² + 1: f(x) = 1 - x²
// Khi x = 0 (ở giữa): f(0) = 1 - 0² = 1 (tốc độ tối đa)
// Khi x = 1 (ở đầu): f(1) = 1 - 1² = 0 (tốc độ tối thiểu)
	float speedFactor = 1.0f - (normalizedDistance * normalizedDistance);

// Điều chỉnh với hệ số biến đổi

// Tính tốc độ cuối cùng: NHANH ở giữa, CHẬM ở 2 đầu
	float speed = minSpeed + (maxSpeed - minSpeed) * speedFactor;

	return speed;
}
int ButtonScreenView::calculateScore(int cursorX) {
// Công thức Gaussian: f(x) = A * e^(-((x-μ)²)/(2σ²))
// Trong đó:
// - A = maxScore (biên độ tối đa)
// - μ = peakX (vị trí đỉnh)
// - σ = sigma (độ rộng đường cong)
// - x = cursorX (vị trí hiện tại)

	float distance = (float) (cursorX - peakX);
	float exponent = -(distance * distance) / (2.0f * sigma * sigma);
	float gaussianValue = expf(exponent);

// Tính điểm số từ 0 đến maxScore
	int score = (int) (minScore + (maxScore - minScore) * gaussianValue);

// Đảm bảo điểm số nằm trong khoảng cho phép
	if (score < minScore)
		score = minScore;
	if (score > maxScore)
		score = maxScore;

	return score;
}
void ButtonScreenView::updateCursorPositionText() {
	int currentScore = calculateScore(localImage1);

// Chuyển điểm số thành chuỗi
	char scoreBuffer[16];
	snprintf(scoreBuffer, sizeof(scoreBuffer), "%d", currentScore);

// Cập nhật text hiển thị
	Unicode::strncpy(scorebuttBuffer, scoreBuffer, SCOREBUTT_SIZE);
	scorebutt.setWildcard(scorebuttBuffer);
	scorebutt.invalidate(); // Đánh dấu để vẽ lại text
}
void ButtonScreenView::pauseButtonClicked() {
// Set màn hình hiện tại trước khi chuyển
	PauseScreenView::setPreviousScreen(PauseScreenView::BUTTON_SCREEN);

// Chuyển đến màn hình pause
	static_cast<FrontendApplication*>(Application::getInstance())->gotoPauseScreenScreenNoTransition();
}
void ButtonScreenView::readyButtonClicked() {
	if (currentState == WAITING_FOR_READY && isModalVisible) {
		// Hide ready button
		readyButton.setVisible(false);
		readyButton.invalidate();

		// Start countdown sequence
		currentState = COUNTDOWN_READY;
		isCountdownActive = true;
		countdownStartTick = tickCount; // Use your existing tickCount

		// Immediately show READY state
		showReadyState();
	}
}

void ButtonScreenView::stepCountdown() {
	if (!isCountdownActive)
		return;

// Calculate elapsed time using your tickCount
	uint32_t elapsedTicks = tickCount - countdownStartTick;
	uint32_t elapsedSeconds = elapsedTicks / framesPerSecond;

// State transitions based on elapsed seconds
	switch (currentState) {
	case COUNTDOWN_READY:
		if (elapsedSeconds >= 1) // After 1 second, show SET
				{
			showSetState();
			currentState = COUNTDOWN_SET;
			countdownStartTick = tickCount; // Reset timer for next state
		}
		break;

	case COUNTDOWN_SET:
		if (elapsedSeconds >= 1) // After 1 more second, show STRIKE
				{
			showStrikeState();
			currentState = COUNTDOWN_STRIKE;
			countdownStartTick = tickCount; // Reset timer for next state
		}
		break;

	case COUNTDOWN_STRIKE:
		if (elapsedSeconds >= 1) // After 1 more second, hide modal
				{
			isCountdownActive = false;
			hideModal();
			startGame();  // This will enable cursor movement
			currentState = COUNTDOWN_FINISHED;
		}
		break;

	default:
		break;
	}
}

void ButtonScreenView::showModal() {
	if (!isModalVisible) {
		modalWindow1.setVisible(true);
		modalWindow1.invalidate();

		headerText.setVisible(true);
		readyButton.setVisible(true);

		hideAllCountdownTexts();

		headerText.invalidate();
		readyButton.invalidate();

		isModalVisible = true;
		currentState = WAITING_FOR_READY;
	}
}

void ButtonScreenView::hideModal() {
	if (isModalVisible) {
		modalWindow1.setVisible(false);
		modalWindow1.invalidate();
		isModalVisible = false;

		// Game can start now
		// You can add game start logic here
	}
}

void ButtonScreenView::resetModalState() {
	currentState = WAITING_FOR_READY;
	isCountdownActive = false;
	countdownStartTick = 0;

	headerText.setVisible(true);
	readyButton.setVisible(true);
	hideAllCountdownTexts();
}

void ButtonScreenView::hideAllCountdownTexts() {
	ready.setVisible(false);
	set.setVisible(false);
	strike.setVisible(false);

	ready.invalidate();
	set.invalidate();
	strike.invalidate();
}

void ButtonScreenView::showReadyState() {
	headerText.setVisible(false);
	hideAllCountdownTexts();

	ready.setVisible(true);
	ready.setAlpha(255);

	headerText.invalidate();
	ready.invalidate();
}

void ButtonScreenView::showSetState() {
	hideAllCountdownTexts();

	set.setVisible(true);
	set.setAlpha(255);

	set.invalidate();
}

void ButtonScreenView::showStrikeState() {
	hideAllCountdownTexts();

	strike.setVisible(true);
	strike.setAlpha(255);

	strike.invalidate();
}

void ButtonScreenView::retryButtonClicked() {
	stopGame();
	resetGame();

	resetModalState();
	showModal();
}
void ButtonScreenView::startGame() {
	gameState = GAME_PLAYING;
	isGameActive = true;  // Enable cursor movement

	// Reset cursor to starting position (optional)
	// image1.invalidate();

	// Game is now ready for player input!
}

void ButtonScreenView::stopGame() {
	gameState = GAME_PAUSED;
	isGameActive = false;  // Stop cursor movement
}

void ButtonScreenView::resetGame() {
	gameState = GAME_PREPARING;
	isGameActive = false;
	isCountdownActive = false;

	// Reset cursor to starting position
	// localImage1 = (maxX + minX) / 2;
	// cursorDirection = 1;
}
void ButtonScreenView::setHighScore(int score)
{
    g_highScore = score;
}

int ButtonScreenView::getHighScore()
{
    return g_highScore;
}
void ButtonScreenView::updateHighScoreDisplay()
{
    // Get current highscore from global variable
    int currentHighScore = getHighScore();

    // Convert highscore to string
    char highscoreBuffer[16];
    snprintf(highscoreBuffer, sizeof(highscoreBuffer), "%d", currentHighScore);

    // Update highscorebutt text display
    touchgfx::Unicode::strncpy(highscorebuttBuffer, highscoreBuffer, HIGHSCOREBUTT_SIZE);
    highscorebutt.setWildcard(highscorebuttBuffer);
    highscorebutt.invalidate();
}
void ButtonScreenView::checkAndUpdateHighScore(int newScore)
{
    int currentHighScore = getHighScore();

    if (newScore > currentHighScore)
    {
        // New highscore achieved!
        setHighScore(newScore);

        // Update display immediately
        updateHighScoreDisplay();

        // TODO: Later add flash save here
        // saveHighScoreToFlash(newScore);

        // Optional: Add visual feedback for new highscore
        // showNewHighScoreAnimation();
    }
}
