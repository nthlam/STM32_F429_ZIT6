#include "InputManager.h"

// Global variables
InputType_t currentInputType = INPUT_TYPE_NONE;

/**
 * @brief Initialize input manager system
 */
void InputManager_Init(void) {
    currentInputType = INPUT_TYPE_NONE;
    buttonTaskEnabled = 0;
    loadCellTaskEnabled = 0;
}

/**
 * @brief Set input type and manage tasks accordingly
 * @param type: New input type to set
 * @retval HAL_StatusTypeDef
 */
HAL_StatusTypeDef InputManager_SetType(InputType_t type) {
    // Disable current type first
    InputManager_DisableAll();

    // Clear queue before switching
    uint32_t dummyEvent;
    while (osMessageQueueGet(buttonQueueHandle, &dummyEvent, NULL, 0) == osOK) {
        // Empty queue
    }

    // Set new type
    currentInputType = type;

    switch (type) {
        case INPUT_TYPE_BUTTON:
            buttonTaskEnabled = 1;
            break;

        case INPUT_TYPE_LOAD_CELL:
            loadCellTaskEnabled = 1;
            break;

        case INPUT_TYPE_NONE:
        default:
            // All tasks already disabled
            break;
    }

    return HAL_OK;
}

/**
 * @brief Get current input type
 * @retval InputType_t: Current input type
 */
InputType_t InputManager_GetCurrentType(void) {
    return currentInputType;
}

/**
 * @brief Disable all input types
 */
void InputManager_DisableAll(void) {
    buttonTaskEnabled = 0;
    loadCellTaskEnabled = 0;
    currentInputType = INPUT_TYPE_NONE;
}
