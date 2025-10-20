#include "InputUtil.h"

#include "logger/logger.h"

static const char *styleNames[] = {
        "Pro Controller",
        "Joy-Con controller in handheld mode",
        "Joy-Con controller in dual mode",
        "Joy-Con left controller in single mode",
        "Joy-Con right controller in single mode",
        "GameCube controller",
        "Poké Ball Plus controller",
        "NES/Famicom controller",
        "NES/Famicom controller in handheld mode",
        "SNES controller",
        "N64 controller",
        "Sega Genesis controller",
        "generic external controller",
        "generic controller",
};

nn::hid::NpadBaseState InputUtil::prevControllerState{};
nn::hid::NpadBaseState InputUtil::curControllerState{};

nn::hid::KeyboardState InputUtil::curKeyboardState{};
nn::hid::KeyboardState InputUtil::prevKeyboardState{};

nn::hid::MouseState InputUtil::curMouseState{};
nn::hid::MouseState InputUtil::prevMouseState{};

nn::hid::TouchScreenState<1> InputUtil::curTouchState{};
nn::hid::TouchScreenState<1> InputUtil::prevTouchState{};

ulong InputUtil::selectedPort = -1;
bool InputUtil::isReadInput = true;
bool InputUtil::toggleInput = false;

const char *getStyleName(nn::hid::NpadStyleSet style) {

    s32 index = -1;

    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleFullKey)) { index = 0; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleHandheld)) { index = 1; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleJoyDual)) { index = 2; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleJoyLeft)) { index = 3; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleJoyRight)) { index = 4; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleGc)) { index = 5; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStylePalma)) { index = 6; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleLark)) { index = 7; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleHandheldLark)) { index = 8; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleLucia)) { index = 9; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleLagon)) { index = 10; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleLager)) { index = 11; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleSystemExt)) { index = 12; }
    if (style.Test((int) nn::hid::NpadStyleTag::NpadStyleSystem)) { index = 13; }

    if (index != -1) {
        return styleNames[index];
    } else {
        return "Unknown";
    }
}

void InputUtil::initKBM() {
    nn::hid::InitializeKeyboard();
    nn::hid::InitializeMouse();
}

void InputUtil::updatePadState() {
    prevControllerState = curControllerState;

    tryGetContState(&curControllerState, selectedPort);

    prevKeyboardState = curKeyboardState;
    nn::hid::GetKeyboardState(&curKeyboardState);

    prevMouseState = curMouseState;
    nn::hid::GetMouseState(&curMouseState);

    prevTouchState = curTouchState;
    // nn::hid::GetTouchScreenState(&curTouchState);

    if (isHoldZL() && isPressR()) {
        toggleInput = !toggleInput;
    }
}

bool InputUtil::tryGetContState(nn::hid::NpadBaseState *state, ulong port) {

    nn::hid::NpadStyleSet styleSet = nn::hid::GetNpadStyleSet(port);
    isReadInput = true;
    bool result = true;

    if (styleSet.Test((int) nn::hid::NpadStyleTag::NpadStyleFullKey)) {
        nn::hid::GetNpadState((nn::hid::NpadFullKeyState *) state, port);
    } else if (styleSet.Test((int) nn::hid::NpadStyleTag::NpadStyleJoyDual)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyDualState *) state, port);
    } else if (styleSet.Test((int) nn::hid::NpadStyleTag::NpadStyleJoyLeft)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyLeftState *) state, port);
    } else if (styleSet.Test((int) nn::hid::NpadStyleTag::NpadStyleJoyRight)) {
        nn::hid::GetNpadState((nn::hid::NpadJoyRightState *) state, port);
    } else {
        nn::hid::GetNpadState((nn::hid::NpadHandheldState *) state, 0x20);
    }

    isReadInput = false;

    return result;

}

bool InputUtil::isButtonHold(nn::hid::NpadButton button) {
    return curControllerState.mButtons.Test((int) button);
}

bool InputUtil::isButtonPress(nn::hid::NpadButton button) {
    return curControllerState.mButtons.Test((int) button) && !prevControllerState.mButtons.Test((int) button);
}

bool InputUtil::isButtonRelease(nn::hid::NpadButton button) {
    return !curControllerState.mButtons.Test((int) button) && prevControllerState.mButtons.Test((int) button);
}

bool InputUtil::isKeyHold(nn::hid::KeyboardKey key) {
    return curKeyboardState.mKeys.Test((int) key);
}

bool InputUtil::isKeyPress(nn::hid::KeyboardKey key) {
    return curKeyboardState.mKeys.Test((int) key) && !prevKeyboardState.mKeys.Test((int) key);
}

bool InputUtil::isKeyRelease(nn::hid::KeyboardKey key) {
    return !curKeyboardState.mKeys.Test((int) key) && prevKeyboardState.mKeys.Test((int) key);
}

bool InputUtil::isModifierActive(nn::hid::KeyboardModifier modifier) {
    return curKeyboardState.mModifiers.Test((int) modifier);
}

bool InputUtil::isMouseHold(nn::hid::MouseButton button) {
    return curMouseState.mButtons.Test((int) button);
}

bool InputUtil::isMousePress(nn::hid::MouseButton button) {
    return curMouseState.mButtons.Test((int) button) && !prevMouseState.mButtons.Test((int) button);
}

bool InputUtil::isMouseRelease(nn::hid::MouseButton button) {
    return !curMouseState.mButtons.Test((int) button) && prevMouseState.mButtons.Test((int) button);
}

bool InputUtil::isMouseConnected() {
    return curMouseState.mAttributes.Test((int) nn::hid::MouseAttribute::IsConnected);
}

void InputUtil::getMouseCoords(float *x, float *y) {
    *x = curMouseState.mX;
    *y = curMouseState.mY;
}

void InputUtil::getScrollDelta(float *x, float *y) {
    *x = curMouseState.mWheelDeltaX;
    *y = curMouseState.mWheelDeltaY;
}

bool InputUtil::getTouchCoords(s32 *x, s32 *y) {
    if (curTouchState.mCount > 0) {
        *x = curTouchState.mTouches[0].mX;
        *y = curTouchState.mTouches[0].mY;
        return true;
    }
    return false;
}

bool InputUtil::isHoldTouch() {
    return curTouchState.mCount > 0;
}

bool InputUtil::isPressTouch() {
    return curTouchState.mCount > 0 && prevTouchState.mCount == 0;
}

bool InputUtil::isReleaseTouch() {
    return curTouchState.mCount == 0 && prevTouchState.mCount > 0;
}
