// =======================================
// CS488/688 base code
// (written by Toshiya Hachisuka)
// =======================================
#pragma once
#define _CRT_SECURE_NO_WARNINGS

// OpenGL
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../external/glm/glm.hpp"
#include "../external/glm/gtc/matrix_transform.hpp"
#include "../external/glm/gtc/type_ptr.hpp"

// image loader and writer
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

// linear algebra
#include "linalg.h"
using namespace linalg::aliases;

#include "shader.h"
#include "quad.h"
#include "perlinworley.h"

// misc
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <cfloat>

// main window
static GLFWwindow *globalGLFWindow;

// window size and resolution
// (do not make it too large - will be slow!)
constexpr int globalWidth = 1080;
constexpr int globalHeight = 720;

// degree and radian
constexpr float PI = 3.14159265358979f;
constexpr float DegToRad = PI / 180.0f;

// amount the camera moves with a mouse and a keyboard
constexpr float ANGFACT = 0.2f;
constexpr float SCLFACT = 1000.0f;

// dynamic camera parameters
float3 globalEye = float3(0.0f, 0.0f, 0.1f);
float3 globalLookat = float3(0.0f, 0.0f, 0.0f);
float3 globalUp = normalize(float3(0.0f, 1.0f, 0.0f));
float3 globalViewDir; // should always be normalize(globalLookat - globalEye)
float3 globalRight;

// mouse event
static bool mouseLeftPressed;
static double m_mouseX = 0.0;
static double m_mouseY = 0.0;

float fov = 45.0f;

float coverage = 0.30f;
float cloudType = 1.0f;
float anvilBias = 0.1f;

glm::vec3 cloudColor = glm::vec3(1.0f, 1.0f, 1.0f);

float eccentricity = 0.5f;
float silver_intensity = 0.15f;
float silver_spread = 0.5f;

float toneMapperEyeExposure = 0.8f;
// raymarch
float earthRadius = 500000.0f;
float cloudStartRadius = 520000.0f;
float cloudEndRadius = 530000.0f;

float sunIntensity = 0.6f;
float sunPitch = 45.0f;
glm::vec3 toneMapperColor = glm::vec3(1.0, 0.5, 1.0);

// OpenGL related data (do not modify it if it is working)
static GLuint GLFrameBufferTexture;
GLfloat prevTime = 0.0f;
GLuint prevFrameCount = 0;
GLuint curFrameCount = 0;

// image with a depth buffer
// (depth buffer is not always needed, but hey, we have a few GB of memory, so it won't be an issue...)
class Image
{
public:
    std::vector<float3> pixels;
    std::vector<float> depths;
    int width = 0, height = 0;

    static float toneMapping(const float r)
    {
        // you may want to implement better tone mapping
        return std::max(std::min(1.0f, r), 0.0f);
    }

    static float gammaCorrection(const float r, const float gamma = 1.0f)
    {
        // assumes r is within 0 to 1
        // gamma is typically 2.2, but the default is 1.0 to make it linear
        return pow(r, 1.0f / gamma);
    }

    void resize(const int newWdith, const int newHeight)
    {
        this->pixels.resize(newWdith * newHeight);
        this->depths.resize(newWdith * newHeight);
        this->width = newWdith;
        this->height = newHeight;
    }

    void clear()
    {
        for (int j = 0; j < height; j++)
        {
            for (int i = 0; i < width; i++)
            {
                this->pixel(i, j) = float3(0.0f);
                this->depth(i, j) = FLT_MAX;
            }
        }
    }

    Image(int _width = 0, int _height = 0)
    {
        this->resize(_width, _height);
        this->clear();
    }

    bool valid(const int i, const int j) const
    {
        return (i >= 0) && (i < this->width) && (j >= 0) && (j < this->height);
    }

    float &depth(const int i, const int j)
    {
        return this->depths[i + j * width];
    }

    float3 &pixel(const int i, const int j)
    {
        // optionally can check with "valid", but it will be slow
        return this->pixels[i + j * width];
    }

    void load(const char *fileName)
    {
        int comp, w, h;
        float *buf = stbi_loadf(fileName, &w, &h, &comp, 3);
        if (!buf)
        {
            std::cerr << "Unable to load: " << fileName << std::endl;
            return;
        }

        this->resize(w, h);
        int k = 0;
        for (int j = height - 1; j >= 0; j--)
        {
            for (int i = 0; i < width; i++)
            {
                this->pixels[i + j * width] = float3(buf[k], buf[k + 1], buf[k + 2]);
                k += 3;
            }
        }
        delete[] buf;
        std::cout << "Loaded " << fileName << std::endl;
    }
    void save(const char *fileName)
    {
        unsigned char *buf = new unsigned char[width * height * 3];
        int k = 0;
        for (int j = height - 1; j >= 0; j--)
        {
            for (int i = 0; i < width; i++)
            {
                buf[k++] = (unsigned char)(255.0f * gammaCorrection(toneMapping(pixel(i, j).x)));
                buf[k++] = (unsigned char)(255.0f * gammaCorrection(toneMapping(pixel(i, j).y)));
                buf[k++] = (unsigned char)(255.0f * gammaCorrection(toneMapping(pixel(i, j).z)));
            }
        }
        stbi_write_png(fileName, width, height, 3, buf, width * 3);
        delete[] buf;
        std::cout << "Saved " << fileName << std::endl;
    }
};

// main image buffer to be displayed
Image FrameBuffer(globalWidth, globalHeight);

// you may want to use the following later for progressive ray tracing
Image AccumulationBuffer(globalWidth, globalHeight);
GLuint sampleCount = 0;

// keyboard events (you do not need to modify it unless you want to)
void keyFunc(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        }

        case GLFW_KEY_W:
        {
            globalEye += SCLFACT * globalViewDir;
            globalLookat += SCLFACT * globalViewDir;
            break;
        }

        case GLFW_KEY_S:
        {
            globalEye -= SCLFACT * globalViewDir;
            globalLookat -= SCLFACT * globalViewDir;
            break;
        }

        case GLFW_KEY_Q:
        {
            globalEye += SCLFACT * globalUp;
            globalLookat += SCLFACT * globalUp;
            break;
        }

        case GLFW_KEY_Z:
        {
            globalEye -= SCLFACT * globalUp;
            globalLookat -= SCLFACT * globalUp;
            break;
        }

        case GLFW_KEY_A:
        {
            globalEye -= SCLFACT * globalRight;
            globalLookat -= SCLFACT * globalRight;
            break;
        }

        case GLFW_KEY_D:
        {
            globalEye += SCLFACT * globalRight;
            globalLookat += SCLFACT * globalRight;
            break;
        }

        case GLFW_KEY_UP:
        {
            sunPitch += 3;
            break;
        }

        case GLFW_KEY_DOWN:
        {
            sunPitch -= 3;
            break;
        }

        case GLFW_KEY_LEFT:
        {
            coverage -= 0.1;
            break;
        }

        case GLFW_KEY_RIGHT:
        {
            coverage += 0.1;
            break;
        }

        default:
            break;
        }
    }
}

// mouse button events (you do not need to modify it unless you want to)
void mouseButtonFunc(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            mouseLeftPressed = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mouseLeftPressed = false;
        }
    }
}

// mouse button events (you do not need to modify it unless you want to)
void cursorPosFunc(GLFWwindow *window, double mouse_x, double mouse_y)
{
    if (mouseLeftPressed)
    {
        const float xfact = -ANGFACT * float(mouse_y - m_mouseY);
        const float yfact = -ANGFACT * float(mouse_x - m_mouseX);
        float3 v = globalViewDir;

        // local function in C++...
        struct
        {
            float3 operator()(float theta, const float3 &v, const float3 &w)
            {
                const float c = cosf(theta);
                const float s = sinf(theta);

                const float3 v0 = dot(v, w) * w;
                const float3 v1 = v - v0;
                const float3 v2 = cross(w, v1);

                return v0 + c * v1 + s * v2;
            }
        } rotateVector;

        v = rotateVector(xfact * DegToRad, v, globalRight);
        v = rotateVector(yfact * DegToRad, v, globalUp);
        globalViewDir = v;
        globalLookat = globalEye + globalViewDir;
        globalRight = cross(globalViewDir, globalUp);

        m_mouseX = mouse_x;
        m_mouseY = mouse_y;
    }
    else
    {
        m_mouseX = mouse_x;
        m_mouseY = mouse_y;
    }
}

// main window
// you probably do not need to modify this in A0 to A3.
class CS488Window
{
public:
    // put this first to make sure that the glInit's constructor is called before the one for CS488Window

    CS488Window() {}
    virtual ~CS488Window() {}

    void (*process)() = NULL;

    void start() const
    {

        // initialize GLFW
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW." << std::endl;
            exit(-1);
        }

        // create a window
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

        globalGLFWindow = glfwCreateWindow(globalWidth, globalHeight, "Cloud", NULL, NULL);
        if (globalGLFWindow == NULL)
        {
            std::cerr << "Failed to open GLFW window." << std::endl;
            glfwTerminate();
            exit(-1);
        }

        // make OpenGL context for the window
        glfwMakeContextCurrent(globalGLFWindow);

        // initialize GLEW
        glewExperimental = true;
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW." << std::endl;
            glfwTerminate();
            exit(-1);
        }

        // set callback functions for events
        glfwSetKeyCallback(globalGLFWindow, keyFunc);
        glfwSetMouseButtonCallback(globalGLFWindow, mouseButtonFunc);
        glfwSetCursorPosCallback(globalGLFWindow, cursorPosFunc);

        GLuint skyShader = newShader("shaders/sky.vert", "shaders/sky.frag");
        GLuint quadShader = newShader("shaders/quad.vert", "shaders/quad.frag");

        // Generate PerlinWorley, if it exists then dont need to run it.
        std::ifstream noiseFile("assets/noiseShape.tga");
        if (!noiseFile.good())
        {
            Tileable3dNoise::GeneratePerlinWorleyTexture();
        }

        int x, y, n;
        GLuint worltex, perlworltex;

        unsigned char *perlWorlNoiseArray = stbi_load("assets/noiseShape.tga", &x, &y, &n, 4);

        glGenTextures(1, &perlworltex);
        glBindTexture(GL_TEXTURE_3D, perlworltex);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, 128, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, perlWorlNoiseArray);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_3D);
        glBindTexture(GL_TEXTURE_3D, 0);
        stbi_image_free(perlWorlNoiseArray);

        unsigned char *worlNoiseArray = stbi_load("assets/noiseErosion.tga", &x, &y, &n, 3);
        glGenTextures(1, &worltex);
        glBindTexture(GL_TEXTURE_3D, worltex);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, 32, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, worlNoiseArray);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_3D);
        glBindTexture(GL_TEXTURE_3D, 0);
        stbi_image_free(worlNoiseArray);

        GLuint mainFramebuffer, mainColorbuffer;

        glGenFramebuffers(1, &mainFramebuffer);
        glGenTextures(1, &mainColorbuffer);
        glBindTexture(GL_TEXTURE_2D, mainColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, globalWidth, globalHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainColorbuffer, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        FullScreenQuad quad;
        quad.GenBuffer();

        glUseProgram(skyShader);
        GLuint uInvView = glGetUniformLocation(skyShader, "invView");
        GLuint uInvProj = glGetUniformLocation(skyShader, "invProj");
        GLuint uCamera = glGetUniformLocation(skyShader, "camera");
        GLuint uTime = glGetUniformLocation(skyShader, "time");
        GLuint uResolution = glGetUniformLocation(skyShader, "resolution");
        GLuint uSunIntensity = glGetUniformLocation(skyShader, "sunIntensity");
        GLuint uSunPosition = glGetUniformLocation(skyShader, "sunPosition");
        GLuint uConverage = glGetUniformLocation(skyShader, "coverage");
        GLuint uCloudType = glGetUniformLocation(skyShader, "cloudType");
        GLuint uAnvilBias = glGetUniformLocation(skyShader, "anvilBias");
        GLuint uToneMapperExposure = glGetUniformLocation(skyShader, "toneMapperEyeExposure");
        GLuint uEccentricity = glGetUniformLocation(skyShader, "eccentricity");
        GLuint uSilverIntensity = glGetUniformLocation(skyShader, "silver_intensity");
        GLuint uSilverSpread = glGetUniformLocation(skyShader, "silver_spread");
        GLuint uEarthRadius = glGetUniformLocation(skyShader, "earthRadius");
        GLuint uCloudStartRadius = glGetUniformLocation(skyShader, "cloudStartRadius");
        GLuint uCloudEndRadius = glGetUniformLocation(skyShader, "cloudEndRadius");
        GLuint uToneMapperColor = glGetUniformLocation(skyShader, "toneMapperColor");
        GLuint uPerlinWorleySampler = glGetUniformLocation(skyShader, "perlworl");
        GLuint uWorleySampler = glGetUniformLocation(skyShader, "worl");

        glUniform1i(uPerlinWorleySampler, 0);
        glUniform1i(uWorleySampler, 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, perlworltex);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_3D, worltex);

        while (glfwWindowShouldClose(globalGLFWindow) == GL_FALSE)
        {
            glfwPollEvents();
            globalViewDir = normalize(globalLookat - globalEye);
            globalRight = normalize(cross(globalViewDir, globalUp));
            float curTime = (float)glfwGetTime();

            if (curTime - prevTime > 5.00)
            {
                std::cout << "Frames per second: " << (curFrameCount - prevFrameCount) / 5 << std::endl;
                prevFrameCount = curFrameCount;
                prevTime = curTime;
            }
            curFrameCount++;

            glBindFramebuffer(GL_FRAMEBUFFER, mainFramebuffer);
            glViewport(0, 0, globalWidth * 2, globalHeight * 2);

            glm::mat4 projection = glm::perspective(glm::radians(fov), (float)globalWidth / (float)globalHeight, 0.1f, 100.0f);
            glm::vec3 glmEye = glm::vec3(globalEye.x, globalEye.y, globalEye.z);
            glm::mat4 view = glm::lookAt(glm::vec3(globalEye.x, globalEye.y, globalEye.z),
                                         glm::vec3((globalEye + globalViewDir).x, (globalEye + globalViewDir).y, (globalEye + globalViewDir).z),
                                         glm::vec3(globalUp.x, globalUp.y, globalUp.z));

            glUseProgram(skyShader);

            glUniform3fv(uCamera, 1, &glmEye[0]);
            glUniformMatrix4fv(uInvView, 1, GL_FALSE, &glm::inverse(view)[0][0]);
            glUniformMatrix4fv(uInvProj, 1, GL_FALSE, &glm::inverse(projection)[0][0]);
            glUniform1f(uTime, curTime);

            glUniform2f(uResolution, globalWidth, globalHeight);

            glUniform1f(uConverage, coverage);
            glUniform1f(uCloudType, cloudType);
            glUniform1f(uAnvilBias, anvilBias);

            glUniform1f(uToneMapperExposure, toneMapperEyeExposure);

            glUniform1f(uEarthRadius, earthRadius);
            glUniform1f(uCloudStartRadius, cloudStartRadius);
            glUniform1f(uCloudEndRadius, cloudEndRadius);

            glUniform1f(uSunIntensity, sunIntensity);
            float sunPitchRadians = glm::radians(sunPitch);
            constexpr float yaw = (float)glm::radians(90.0);
            float sunposx = cos(yaw) * cos(sunPitchRadians);
            float sunposy = sin(sunPitchRadians);
            float sunposz = sin(yaw) * cos(sunPitchRadians);
            glUniform3f(uSunPosition, float(sunposx), float(sunposy), float(sunposz));
            glUniform3fv(uToneMapperColor, 1, &toneMapperColor[0]);

            quad.Render();

            glDisable(GL_DEPTH_TEST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClearColor(0.0, 0.0, 0.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glUseProgram(quadShader);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mainColorbuffer);

            quad.Render();

            glBindVertexArray(0);

            glfwSwapBuffers(globalGLFWindow);
        }
    }
};
