#ifndef CAMERA_H
#define CAMERA_H

class Camera {
public:
    float x, y;
    int width, height;

    Camera();
    void follow(float targetX, float targetY);
    int worldToScreenX(float worldX) const;
    int worldToScreenY(float worldY) const;
};

#endif