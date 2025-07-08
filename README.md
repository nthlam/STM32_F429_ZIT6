## GIỚI THIỆU

__Đề bài__: Game Highstriker

__Sản phẩm:__
1. Hiển thị giao diện đồ họa với màn hình TFT 240x320 thông qua bộ điều khiển LTDC của board
2. Điều khiển trò chơi bằng hai chế độ: nút nhấn vật lý và cảm biến lực (load cell)
3. Quản lý tác vụ động, chỉ kích hoạt polling nút khi cần thiết
4. Giao diện TouchGFX với nhiều màn hình: start, lựa chọn phần cứng, gameplay và tạm dừng
5. Hệ thống phản hồi thời gian thực với độ trễ dưới 50ms

![Ảnh minh họa dự án Highstriker Game với STM32](./AnhDemo.jpg)

## MÔI TRƯỜNG HOẠT ĐỘNG

- STM32F429ZIT6 (ARM Cortex-M4 @ 180MHz)
- Bộ kit STM32F429I-Discovery tích hợp màn hình LCD 240x320, cảm ứng capacitive
- Nút nhấn vật lý kết nối GPIO PA0
- Cảm biến lực (load cell) với module HX711
- Buzzer để phản hồi âm thanh

## SƠ ĐỒ SCHEMATIC
![image](./SoDoMach.jpg)

### TÍCH HỢP HỆ THỐNG

* Phần cứng
|Thành phần|Vai trò|
  |--|--|
  |STM32F429ZIT6|Bo mạch điều khiển chính, xử lý toàn bộ logic game và giao tiếp phần cứng|
  |Màn hình LCD 240x320|Hiển thị giao diện trò chơi Highstriker và nhận thao tác cảm ứng|
  |Nút nhấn vật lý|Điều khiển trò chơi trong chế độ button mode|
  |Cảm biến lực HX711|Đo lường lực trong chế độ load cell mode|
  |Buzzer|Phản hồi âm thanh khi có sự kiện trong game|
  |Breadboard + dây nối|Tạo mạch kết nối phần cứng giữa các thiết bị|

* Phần mềm
|Thành phần|Vai trò|
  |--|--|
  |Firmware chính (C/C++)|Điều khiển game logic, xử lí giao tiếp với phần cứng|
  |TouchGFX Framework|Thiết kế giao diện game và quản lý các màn hình|
  |FreeRTOS|Hệ điều hành thời gian thực để quản lý tác vụ|
  |STM32CubeIDE|Môi trường phát triển, biên dịch và nạp chương trình|
  |STM32CubeMX|Cấu hình phần cứng và tạo code khởi tạo|

### ĐẶC TẢ HÀM

*__[main.c](./Core/Src/main.c)__*
* ```C
  // Hàng đợi phục vụ cho việc xử lý sự kiện nút nhấn
  osMessageQueueId_t buttonQueueHandle;

  // Handle cho tác vụ button polling động
  osThreadId_t buttonTaskHandle = NULL;
  ```
* ```C
    /**
     StartDefaultTask - tác vụ mặc định xử lý polling nút nhấn
    */
    void StartDefaultTask(void *argument) {
        GPIO_PinState currentState;
        GPIO_PinState lastState = GPIO_PIN_RESET;
        uint32_t buttonEvent = 1;

        for (;;) {
            // Đọc trạng thái hiện tại của nút nhấn PA0
            currentState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

            // Phát hiện cạnh lên (0 -> 1) - nhấn nút
            if (currentState == GPIO_PIN_SET && lastState == GPIO_PIN_RESET) {
                // Gửi event vào queue để TouchGFX xử lý
                osMessageQueuePut(buttonQueueHandle, &buttonEvent, 0, 0);
            }

            lastState = currentState;
            osDelay(50); // Polling mỗi 50ms để chống rung
        }
    }
  ```
* ```C
    /**
     ButtonTaskFunction - tác vụ polling nút nhấn động chỉ chạy khi cần
    */
    void ButtonTaskFunction(void *argument) {
        GPIO_PinState currentState;
        GPIO_PinState lastState = GPIO_PIN_RESET;
        uint32_t buttonEvent = 1;

        for (;;) {
            currentState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
            
            if (currentState == GPIO_PIN_SET && lastState == GPIO_PIN_RESET) {
                osMessageQueuePut(buttonQueueHandle, &buttonEvent, 0, 0);
            }
            
            lastState = currentState;
            osDelay(50);
        }
    }
  ```
* ```C
    /**
     StartButtonPollingTask - tạo và khởi chạy tác vụ button polling
    */
    void StartButtonPollingTask() {
        if (buttonTaskHandle == NULL) {
            buttonTaskHandle = osThreadNew(ButtonTaskFunction, NULL, &buttonTask_attributes);
        }
    }
  ```
* ```C
    /**
     StopButtonPollingTask - dừng và xóa tác vụ button polling
    */
    void StopButtonPollingTask() {
        if (buttonTaskHandle != NULL) {
            osThreadTerminate(buttonTaskHandle);
            buttonTaskHandle = NULL;
        }
    }
  ```

*__[Model.hpp](TouchGFX/gui/include/gui/model/Model.hpp)__*
* ```C
   /**
    Enum định nghĩa các chế độ game
    */
    enum class GameMode {
        NONE,         // Không có chế độ nào được chọn
        BUTTON_MODE,  // Chế độ nút nhấn
        LOADCELL_MODE // Chế độ cảm biến lực
    };
  ```
* ```C
   /**
    Class Model quản lý trạng thái game toàn cục
    */
    class Model {
    private:
        GameMode currentGameMode;
        ModelListener* modelListener;
        
    public:
        Model();
        void setGameMode(GameMode mode);
        GameMode getGameMode() const;
        void bind(ModelListener* listener);
        void tick();
    };
  ```

*__[StartScreenPresenter.cpp](TouchGFX/gui/src/startscreen_screen/StartScreenPresenter.cpp)__*
* ```C
   /**
    StartScreenPresenter::activate - được gọi khi màn hình start được kích hoạt
    */
    void StartScreenPresenter::activate() {
        // Reset game mode về NONE khi vào màn hình start
        SetGameModeAndManageButtonTask(GameMode::NONE);
    }
  ```

*__[ButtonScreenPresenter.cpp](TouchGFX/gui/src/buttonscreen_screen/ButtonScreenPresenter.cpp)__*
* ```C
   /**
    ButtonScreenPresenter::activate - kích hoạt chế độ button mode
    */
    void ButtonScreenPresenter::activate() {
        // Chuyển sang chế độ button và bắt đầu polling nút
        SetGameModeAndManageButtonTask(GameMode::BUTTON_MODE);
    }
  ```
* ```C
   /**
    ButtonScreenPresenter::deactivate - hủy kích hoạt chế độ button mode
    */
    void ButtonScreenPresenter::deactivate() {
        // Reset về chế độ NONE và dừng polling nút
        SetGameModeAndManageButtonTask(GameMode::NONE);
    }
  ```

*__[ButtonScreenView.cpp](TouchGFX/gui/src/buttonscreen_screen/ButtonScreenView.cpp)__*
* ```C
   /**
    ButtonScreenView::setupScreen - khởi tạo màn hình button
    */
    void ButtonScreenView::setupScreen() {
        // Khởi tạo các thành phần UI cho màn hình button game
        cursorPosition = 0;
        isGameActive = true;
        updateCursorPositionText();
    }
  ```
* ```C
   /**
    ButtonScreenView::buttonHandle - xử lý sự kiện nhấn nút từ queue
    */
    void ButtonScreenView::buttonHandle() {
        uint32_t buttonEvent;
        // Kiểm tra queue có sự kiện nút nhấn không
        if (osMessageQueueGet(buttonQueueHandle, &buttonEvent, NULL, 0) == osOK) {
            if (isGameActive) {
                cursorPosition += 10; // Tăng vị trí cursor
                updateCursorPositionText();
                
                // Kiểm tra điều kiện thắng
                if (cursorPosition >= 100) {
                    // Người chơi thắng
                    isGameActive = false;
                    // Cập nhật UI hiển thị thắng
                }
            }
        }
    }
  ```
* ```C
   /**
    ButtonScreenView::handleTickEvent - được gọi liên tục 60 lần/giây
    */
    void ButtonScreenView::handleTickEvent() {
        // Xử lý sự kiện nút nhấn
        buttonHandle();
        
        // Cập nhật animation và hiệu ứng visual khác
        if (isGameActive) {
            // Logic game animation
        }
    }
  ```

*__[LoadcellScreenPresenter.cpp](TouchGFX/gui/src/loadcellscreen_screen/LoadcellScreenPresenter.cpp)__*
* ```C
   /**
    LoadcellScreenPresenter::activate - kích hoạt chế độ load cell
    */
    void LoadcellScreenPresenter::activate() {
        // Chuyển sang chế độ load cell (không cần polling nút)
        SetGameModeAndManageButtonTask(GameMode::LOADCELL_MODE);
    }
  ```

*__[LoadcellScreenView.cpp](TouchGFX/gui/src/loadcellscreen_screen/LoadcellScreenView.cpp)__*
* ```C
   /**
    LoadcellScreenView::handle_loadcell - xử lý dữ liệu từ cảm biến lực
    */
    void LoadcellScreenView::handle_loadcell() {
        // Đọc giá trị từ cảm biến HX711
        int32_t forceValue = readLoadCellValue();
        
        if (forceValue > threshold) {
            // Cập nhật vị trí cursor dựa trên lực
            cursorPosition = map(forceValue, threshold, maxForce, 0, 100);
            update_display();
        }
    }
  ```
* ```C
   /**
    LoadcellScreenView::update_display - cập nhật hiển thị lực đo được
    */
    void LoadcellScreenView::update_display() {
        // Cập nhật thanh tiến trình hiển thị lực
        Unicode::snprintf(forceValueBuffer, FORCE_VALUE_SIZE, "%d", currentForce);
        forceValueText.invalidate();
        
        // Cập nhật vị trí cursor trên màn hình
        updateCursorPosition();
    }
  ```

*__[TouchGFXHAL.cpp](TouchGFX/target/TouchGFXHAL.cpp)__*
* ```C
   /**
    TouchGFXHAL::initialize - khởi tạo hardware abstraction layer
    */
    void TouchGFXHAL::initialize() {
        // Cấu hình frame buffer và DMA2D
        setFrameBufferStartAddress((uint16_t*)frameBuf);
        
        // Khởi tạo touch controller
        touchController.init();
        
        // Cấu hình interrupt cho LTDC
        configureInterrupts();
    }
  ```
* ```C
   /**
    TouchGFXHAL::flushFrameBuffer - cập nhật frame buffer lên LCD
    */
    void TouchGFXHAL::flushFrameBuffer(const touchgfx::Rect& rect) {
        // Sử dụng DMA2D để copy frame buffer lên LCD
        // với hardware acceleration
        if (DMA2D->CR & DMA2D_CR_START) {
            // DMA2D đang bận, chờ hoàn thành
            return;
        }
        
        // Cấu hình DMA2D cho memory-to-memory transfer
        // và bắt đầu transfer
    }
  ```

*__[STM32TouchController.cpp](TouchGFX/target/STM32TouchController.cpp)__*
* ```C
   /**
    STM32TouchController::init - khởi tạo touch controller STMPE811
    */
    void STM32TouchController::init() {
        // Khởi tạo I2C để giao tiếp với STMPE811
        IOE_Init();
        
        // Cấu hình touch controller parameters
        IOE_Write(STMPE811_ADDR, STMPE811_TSC_CTRL, STMPE811_TSC_CTRL_EN);
    }
  ```
* ```C
   /**
    BSP_TS_GetState - đọc trạng thái cảm ứng
    */
    void BSP_TS_GetState(TS_StateTypeDef* TsState) {
        uint8_t dataXY[4];
        
        // Đọc dữ liệu X, Y từ STMPE811 qua I2C
        IOE_ReadMultiple(STMPE811_ADDR, STMPE811_TSC_DATA_X, dataXY, 4);
        
        // Chuyển đổi raw data thành tọa độ màn hình
        TsState->TouchDetected = (dataXY[0] & 0x80) ? 1 : 0;
        if (TsState->TouchDetected) {
            TsState->X = ((dataXY[0] & 0x0F) << 8) | dataXY[1];
            TsState->Y = ((dataXY[2] & 0x0F) << 8) | dataXY[3];
        }
    }
  ```

### QUẢN LÝ TÁC VỤ ĐỘNG

* ```C
   /**
    SetGameModeAndManageButtonTask - quản lý tác vụ dựa trên game mode
    */
    void SetGameModeAndManageButtonTask(GameMode mode) {
        gameModel.setGameMode(mode);
        
        if (mode == GameMode::BUTTON_MODE) {
            StartButtonPollingTask();  // Tạo và khởi chạy button task
        } else {
            StopButtonPollingTask();   // Dừng và xóa button task
        }
    }
  ```

### LUỒNG HOẠT ĐỘNG GAME

```
1. Khởi động hệ thống:
   main() → HAL_Init() → SystemClock_Config() → FreeRTOS_Start()

2. Màn hình Start:
   - Game mode = NONE
   - Không có button polling task
   - Chỉ có TouchGFX task chạy

3. Chọn chế độ Button:
   - Game mode = BUTTON_MODE  
   - Tạo button polling task
   - Button task polling GPIO PA0 mỗi 50ms

4. Chơi game Button mode:
   - Button task gửi event vào queue
   - TouchGFX task xử lý event và cập nhật UI
   - Phản hồi thời gian thực < 50ms

5. Chọn chế độ Load cell:
   - Game mode = LOADCELL_MODE
   - Dừng button polling task  
   - Sử dụng ADC/SPI để đọc HX711

6. Thoát game:
   - Game mode = NONE
   - Dừng tất cả polling task
   - Trở về màn hình start
```
