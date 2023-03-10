#pragma once
#include <GL/glut.h>
#include <Eigen/Dense>

class CoordinateFrame {
public:
    CoordinateFrame() = delete;
    CoordinateFrame(const Eigen::Matrix<float,3,1>& o, float scale)
        : m_origin(o)
        , m_scale(scale) {};
    void draw() {
        glBegin(GL_LINES);
        // Draw x-axis in red
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(m_origin(0), m_origin(1), m_origin(2));
        glVertex3f(m_origin(0)+m_scale, m_origin(1), m_origin(2));
        // Draw y-axis in green
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(m_origin(0), m_origin(1), m_origin(2));
        glVertex3f(m_origin(0), m_origin(1)+m_scale, m_origin(2));
        // Draw z-axis in blue
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(m_origin(0), m_origin(1), m_origin(2));
        glVertex3f(m_origin(0), m_origin(1), m_origin(2)+m_scale);
        glEnd();
        // std::cout<<"drawing!"<<std::endl;
    }
private:
    Eigen::Matrix<float,3,1> m_origin;
    float m_scale;
};