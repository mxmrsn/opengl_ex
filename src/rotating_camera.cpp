/*
// An Example of commanding a camera pose at fixed height, rotating around a coordinate frame;
// renders camera POV, and plots on the camera image frame (markup)
*/
#include <iostream>
#include <vector>
#include <math.h>

// #include <glad/glad.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Eigen/Dense>

using glm::lookAt;
using glm::mat4;
using glm::radians;
using glm::vec3;
using std::vector;

void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

class SimpleCamera
{
public:
    vec3 position;
    SimpleCamera()
    {
        position = vec3(0, 0, 0);
    }
};
SimpleCamera camera;

class SimpleLine
{
    int shaderProgram;
    unsigned int VBO, VAO;
    vector<float> vertices;
    vec3 startPoint;
    vec3 endPoint;
    mat4 MVP = mat4(1.0);
    vec3 lineColor;

public:
    SimpleLine(vec3 start, vec3 end)
    {

        startPoint = start;
        endPoint = end;
        lineColor = vec3(1, 1, 1);

        const char *vertexShaderSource = "#version 330 core\n"
                                         "layout (location = 0) in vec3 aPos;\n"
                                         "uniform mat4 MVP;\n"
                                         "void main()\n"
                                         "{\n"
                                         "   gl_Position = MVP * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                         "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
                                           "out vec4 FragColor;\n"
                                           "uniform vec3 color;\n"
                                           "void main()\n"
                                           "{\n"
                                           "   FragColor = vec4(color, 1.0f);\n"
                                           "}\n\0";

        // vertex shader
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors

        // fragment shader
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors

        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        vertices = {
            start.x,
            start.y,
            start.z,
            end.x,
            end.y,
            end.z,

        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    int setMVP(mat4 mvp)
    {
        MVP = mvp;
        return 1;
    }

    int setColor(vec3 color)
    {
        lineColor = color;
        return 1;
    }

    int draw()
    {
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &lineColor[0]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 2);
        return 1;
    }

    ~SimpleLine()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
    }
};

class CoordinateFrame {
    public:
    CoordinateFrame()
    : m_x(SimpleLine(vec3(0,0,0),vec3(1,0,0)))
    , m_y(SimpleLine(vec3(0,0,0),vec3(0,1,0)))
    , m_z(SimpleLine(vec3(0,0,0),vec3(0,0,1))) {
        // m_x = SimpleLine(vec3(0,0,0),vec3(1,0,0));
        m_x.setColor(vec3(1, 0, 0));

        // m_y = SimpleLine(vec3(0,0,0),vec3(0,1,0));
        m_y.setColor(vec3(0, 1, 0));

        // m_z = SimpleLine(vec3(0,0,0),vec3(0,0,1));
        m_z.setColor(vec3(0, 0, 1));
    }
    ~CoordinateFrame() {}
    void setMVP(mat4 mvp) {
        m_x.setMVP(mvp);
        m_y.setMVP(mvp);
        m_z.setMVP(mvp);
    }
    void draw() {
        m_x.draw();
        m_y.draw();
        m_z.draw();
    }
    // CoordinateFrame(Eigen::Matrix<double,3,3> r, Eigen::Matrix<double,3,1> p, const char* color_str){
    // };
    private:
    SimpleLine m_x;
    SimpleLine m_y;
    SimpleLine m_z;
};

GLFWwindow *window;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "drawing lines", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Scene Setup
    float rad = 10.0;
    camera.position = vec3(rad,rad,rad);

    // 3d lines example
    CoordinateFrame f1;

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    float angle = 0.0f;
    float rotationSpeed = 50.0f;  // deg/sec

    // // 2d line example
    vec3 start = vec3(0.8, 0.8, 0); // screen bnds = {[-1,1],[-1, 1]}
    vec3 end = vec3(0.9, 0.9, 0);
    SimpleLine line4(start, end); // drawn in 2D camera image frame because MVP set to I
    SimpleLine line5(vec3(0.8,0.9,0.),vec3(0.9,0.8,0.));

    // TODO: respond to resizing of window and set screen pts based on window size

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;

        processInput(window);
        glClearColor(0.0, 0.0, 0.0, 1.0); // black
        glClear(GL_COLOR_BUFFER_BIT);

        angle = deltaTime * rotationSpeed;

        // update camera position (rotating around origin at fixed height)
        camera.position = vec3(rad*cos(radians(angle)), rad, rad*sin(radians(angle)));
        mat4 view = lookAt(camera.position, vec3(0, 0, 0), vec3(0, 1, 0));

        f1.setMVP(projection * view);
        f1.draw();

        line4.draw(); // draw in 2D in camera frame
        line5.draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}