#include "../include/Camera.h"
#include "../include/Constants.h"

Camera::Camera() : x(0), y(0), width(SCREEN_WIDTH), height(SCREEN_HEIGHT) {}

void Camera::follow(float targetX, float targetY) {
    x = targetX - width / 2;
    if (x < 0) x = 0;
    int maxX = (MAP_WIDTH * TILE_SIZE) - width;
    if (x > maxX) x = maxX;
    y = 0;
}

int Camera::worldToScreenX(float worldX) const { return (int)(worldX - x); }
int Camera::worldToScreenY(float worldY) const { return (int)(worldY - y); }