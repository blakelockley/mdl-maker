#ifndef FPS_H
#define FPS_H

#include "glfw.h"

float calculate_fps() {
    static float fps = 0.0f;
    static double last_second = 0;
    static int frames = 0;

    double current_time = glfwGetTime();

    frames++;
    if (current_time - last_second > 1.0) {
        fps = frames / (current_time - last_second);

        frames = 0;
        last_second = current_time;
    }

    return fps;
}

#endif // FPS_H