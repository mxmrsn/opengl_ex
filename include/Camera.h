#pragma once

#include <math.h>
#include <GL/glut.h>

class Camera {
public:
    Camera(double fov, double aspect_ratio, double near_clip, double far_clip,
           double position[3], double orientation[3], double up[3],
           double move_speed, double turn_speed)
        : fov_(fov)
        , aspect_ratio_(aspect_ratio)
        , near_clip_(near_clip)
        , far_clip_(far_clip)
        , move_speed_(move_speed)
        , turn_speed_(turn_speed) 
        {
            for (int i = 0; i < 3; ++i) {
                position_[i] = position[i];
                orientation_[i] = orientation[i];
                up_[i] = up[i];
            }
        }

    void setView() {
        double forward[3];
        forward[0] = orientation_[0];
        forward[1] = orientation_[1];
        forward[2] = orientation_[2];
        normalize(forward);

        double left[3];
        left[0] = up_[1] * forward[2] - up_[2] * forward[1];
        left[1] = up_[2] * forward[0] - up_[0] * forward[2];
        left[2] = up_[0] * forward[1] - up_[1] * forward[0];
        normalize(left);

        double up[3];
        up[0] = forward[1] * left[2] - forward[2] * left[1];
        up[1] = forward[2] * left[0] - forward[0] * left[2];
        up[2] = forward[0] * left[1] - forward[1] * left[0];

        double m[16];
        m[0] = left[0];
        m[1] = up[0];
        m[2] = -forward[0];
        m[3] = 0.0;
        m[4] = left[1];
        m[5] = up[1];
        m[6] = -forward[1];
        m[7] = 0.0;
        m[8] = left[2];
        m[9] = up[2];
        m[10] = -forward[2];
        m[11] = 0.0;
        m[12] = -dot(left, position_);
        m[13] = -dot(up, position_);
        m[14] = dot(forward, position_);
        m[15] = 1.0;

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glMultMatrixd(m);
    }

    void setPerspective() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(fov_, aspect_ratio_, near_clip_, far_clip_);
    }

    void setPosition(double x, double y, double z) {
        position_[0] = x;
        position_[1] = y;
        position_[2] = z;
    }

    void setOrientation(double x, double y, double z) {
        orientation_[0] = x;
        orientation_[1] = y;
        orientation_[2] = z;
    }

    void moveForwardBackward(double distance) {
        // Move position along forward vector
        double movement[3];
        for (int i = 0; i < 3; ++i) {
            movement[i] = orientation_[i] * distance;
        }
        for (int i = 0; i < 3; ++i) {
            position_[i] += movement[i];
        }
    }

    void moveLeftRight(double distance) {
        // Move position along left vector
        double left[3];
        left[0] = up_[1] * orientation_[2] - up_[2] * orientation_[1];
        left[1] = up_[2] * orientation_[0] - up_[0] * orientation_[2];
        left[2] = up_[0] * orientation_[1] - up_[1] * orientation_[0];
        normalize(left);
        double movement[3];
        for (int i = 0; i < 3; ++i) {
            movement[i] = left[i] * distance;
        }
        for (int i = 0; i < 3; ++i) {
            position_[i] += movement[i];
        }
    }

    void lookUpDown(double angle) {
        // Rotate orientation around left vector
        double left[3];
        left[0] = up_[1] * orientation_[2] - up_[2] * orientation_[1];
        left[1] = up_[2] * orientation_[0] - up_[0] * orientation_[2];
        left[2] = up_[0] * orientation_[1] - up_[1] * orientation_[0];
        normalize(left);

        rotateAroundAxis(orientation_, left, angle);
        normalize(orientation_);
    }

    void turnLeftRight(double angle) {
        // Rotate orientation around up vector
        rotateAroundAxis(orientation_, up_, angle);
        normalize(orientation_);
    }

    void normalize(double vec[3]) {
        double length = sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2]);
        vec[0] /= length;
        vec[1] /= length;
        vec[2] /= length;
    }

    double dot(double vec1[3], double vec2[3]) {
        return vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2];
    }

    void rotateAroundAxis(double vec[3], double axis[3], double theta) {
        double s = sin(theta);
        double c = cos(theta);
        double x = axis[0];
        double y = axis[1];
        double z = axis[2];
        double matrix[9] = {x * x * (1 - c) + c, x * y * (1 - c) - z * s, x * z * (1 - c) + y * s,
                            x * y * (1 - c) + z * s, y * y * (1 - c) + c, y * z * (1 - c) - x * s,
                            x * z * (1 - c) - y * s, y * z * (1 - c) + x * s, z * z * (1 - c) + c};
        double result[3] = {0.0, 0.0, 0.0};
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result[i] += matrix[3 * i + j] * vec[j];
            }
        }
        for (int i = 0; i < 3; ++i) {
            vec[i] = result[i];
        }
        std::cout<<"vec: "<<vec[0]<<vec[1]<<vec[2]<<std::endl;
    }

    void GetPosition(double (&returnArray)[3]) { 
        returnArray[0] = position_[0];
        returnArray[1] = position_[1];
        returnArray[2] = position_[2];
    }
    void GetOrientation(double (&returnArray)[3]) {
        returnArray[0] = orientation_[0];
        returnArray[1] = orientation_[1];
        returnArray[2] = orientation_[2];
    }
    void GetUp(double (&returnArray)[3]) {
        returnArray[0] = up_[0];
        returnArray[1] = up_[1];
        returnArray[2] = up_[2];
     }

private:
    double fov_;
    double aspect_ratio_;
    double near_clip_;
    double far_clip_;
    double position_[3];
    double orientation_[3];
    double up_[3];
    double move_speed_;
    double turn_speed_;

};