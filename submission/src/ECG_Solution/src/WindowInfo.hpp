#pragma once

#include "Camera.hpp"
#include "Cursor.hpp"

// To be set as WindowUserPointer or whatever it's called
// so camera and cursor can be passed to the callbacks
struct WindowInfo {
    Camera camera;
    Cursor cursor;
};
