#pragma once

// Cursor class to keep track of when the mouse is pressed, and how much it has moved since last checked
// Will also be put into own file
class Cursor {
private:
    double xpos, ypos;
public:
    Cursor();
    bool isPressed;
    double deltaX, deltaY;
    void MoveTo(double x, double y);
};
