#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "cmsis_os.h"
#include "main.h"

// Enum cho các loại input
typedef enum {
    INPUT_TYPE_NONE = 0,
    INPUT_TYPE_BUTTON,
    INPUT_TYPE_LOAD_CELL
} InputType_t;

// Enum cho trạng thái input
typedef enum {
    INPUT_STATE_IDLE = 0,
    INPUT_STATE_ACTIVE,
    INPUT_STATE_TRIGGERED
} InputState_t;

// Struct cho input event
typedef struct {
    InputType_t type;
    InputState_t state;
    uint32_t value;      // Giá trị đo được (button: 1/0, load cell: force value)
    uint32_t timestamp;  // Timestamp của event
} InputEvent_t;

// Callback function type
typedef void (*InputEventCallback_t)(InputEvent_t* event);

// Input Manager structure
typedef struct {
    InputType_t currentInputType;
    osThreadId_t inputTaskHandle;
    osMessageQueueId_t inputQueueHandle;
    InputEventCallback_t eventCallback;
    uint8_t isActive;
} InputManager_t;

// Public API functions
HAL_StatusTypeDef InputManager_Init(void);
HAL_StatusTypeDef InputManager_StartInput(InputType_t inputType, InputEventCallback_t callback);
HAL_StatusTypeDef InputManager_StopInput(void);
HAL_StatusTypeDef InputManager_SwitchInput(InputType_t newInputType);
InputType_t InputManager_GetCurrentType(void);
uint8_t InputManager_IsActive(void);

// Internal task functions
void ButtonInputTask(void *argument);
void LoadCellInputTask(void *argument);

#endif // INPUT_MANAGER_H
