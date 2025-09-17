#include <GL/glut.h>
#include<stdio.h>
#include <cmath>
#include <vector>
#include <random>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cstring>
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif
using namespace std;

// M_PI is not defined in all C++ standards, so define it if it's missing.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global variable to control which scene is currently displayed
int currentScene = 1;


// =======================================================================
// |                        NAMESPACE FOR SCENE 1                        |
// |                   (Content from 'Tunnel 4.txt')                     |
// =======================================================================
namespace Scene1 {
    // All global variables and functions from your first file are placed here.
    float cloudX = -1.0f;
    float cloudSpeed = 0.001f;
    float carX = 1.5f;
    float carSpeed = 0.005f;
    float hatchbackX = 0.5f;
    float hatchbackSpeed = 0.003f;
    float whiteBusX = -2.5f;
    float whiteBusSpeed = 0.005f;
    float darkBusX = 2.5f;
    float darkBusSpeed = 0.003f;
    float frontalTrainY = -0.38f;
    float frontalTrainSize = 0.1f;
    float frontalTrainSpeed = 0.003f;
    float lightIntensity = 0.5f;
    float lightSpeed = 0.01f;
    float signalLightRed = 0.0f;
    float signalLightGreen = 0.0f;
    float signalLightBlue = 0.0f;
    float greyLightIntensity = 0.3f;
    float blackLightIntensity = 0.3f;

    default_random_engine generator;
    uniform_real_distribution<float> dist_offset(-0.02f, 0.02f);
    uniform_real_distribution<float> dist_crack_offset(-0.01f, 0.01f);
    uniform_real_distribution<float> dist_color_offset(-0.05f, 0.05f);
    bool isTrainMoving = true;
    bool isRaining = false;
    bool isNight = false;

    const int STAR_COUNT = 200;
    float starX[STAR_COUNT];
    float starY[STAR_COUNT];
    float starBrightness[STAR_COUNT];

    struct Raindrop {
        float x;
        float y;
        float speed;
        float length;
    };

    const int RAIN_COUNT = 500;
    std::vector<Raindrop> raindrops(RAIN_COUNT);

    void initRain() {
        default_random_engine gen;
        uniform_real_distribution<float> distX(-2.0f, 2.0f);
        uniform_real_distribution<float> distY(1.0f, 2.0f);
        uniform_real_distribution<float> distSpeed(0.01f, 0.03f);
        uniform_real_distribution<float> distLength(0.08f, 0.05f);
        for (int i = 0; i < RAIN_COUNT; ++i) {
            raindrops[i].x = distX(gen);
            raindrops[i].y = distY(gen);
            raindrops[i].speed = distSpeed(gen);
            raindrops[i].length = distLength(gen);
        }
    }

    void initStars() {
        default_random_engine gen;
        uniform_real_distribution<float> distX(-2.0f, 2.0f);
        uniform_real_distribution<float> distY(0.4f, 1.0f);
        uniform_real_distribution<float> distB(0.5f, 1.0f);
        for (int i = 0; i < STAR_COUNT; ++i) {
            starX[i] = distX(gen);
            starY[i] = distY(gen);
            starBrightness[i] = distB(gen);
        }
    }

    void drawFilledCircle_s1(float x, float y, float radius) {
        int triangleAmount = 40;
        float twicePi = 2.0f * 3.1416f;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                x + (radius * cos(i * twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
        glEnd();
    }

    void drawCartoonCloud(float x, float y, float scale) {
        glColor3f(0.99f, 0.98f, 0.95f);
        drawFilledCircle_s1(x, y, 0.1f * scale);
        drawFilledCircle_s1(x - 0.1f * scale, y + 0.03f * scale, 0.08f * scale);
        drawFilledCircle_s1(x + 0.1f * scale, y + 0.03f * scale, 0.08f * scale);
        drawFilledCircle_s1(x - 0.05f * scale, y + 0.09f * scale, 0.07f * scale);
        drawFilledCircle_s1(x + 0.05f * scale, y + 0.09f * scale, 0.07f * scale);
    }

    void drawSemiCircle_s1(float cx, float cy, float radiusX, float radiusY) {
        int triangleAmount = 50;
        float twicePi = 2.0f * 3.1416f;
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= triangleAmount; i++) {
            glVertex2f(
                cx + (radiusX * cos(i * twicePi / (triangleAmount * 2))),
                cy + (radiusY * sin(i * twicePi / (triangleAmount * 2)))
            );
        }
        glEnd();
    }

    void drawRailsPerspective() {
        float dim = isNight ? 0.5f : 1.0f;
        glBegin(GL_POLYGON);
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        glVertex2f(-0.4f, -1.0f);
        glVertex2f(0.4f, -1.0f);
        glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
        glVertex2f(0.08f, -0.39f);
        glVertex2f(-0.08f, -0.39f);
        glEnd();
        glColor3f(0.36f * dim, 0.25f * dim, 0.20f * dim);
        float y = -0.39f, spacing = 0.15f;
        float sleeperLengthBottom = 0.7f, sleeperLengthTop = 0.06f;
        for (int i = 0; i < 5; i++) {
            float t = (y + 1.0f) / 0.8f;
            float halfLength = sleeperLengthBottom * (1 - t) + sleeperLengthTop;
            glBegin(GL_QUADS);
            glVertex2f(-halfLength/2, y);
            glVertex2f( halfLength/2, y);
            glVertex2f( halfLength/2, y-0.04f);
            glVertex2f(-halfLength/2, y-0.04f);
            glEnd();
            y -= spacing;
            spacing *= 0.88f;
        }
        glPushMatrix();
        glScalef(1.85f, 1.21f, 0.1f);
        glColor3f(0.4f * dim, 0.4f * dim, 0.4f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.18f, -1.0f);
        glVertex2f(-0.28f, -1.0f);
        glVertex2f(-0.05f, -0.315f);
        glVertex2f(-0.03f, -0.315f);
        glVertex2f(0.18f, -1.0f);
        glVertex2f(0.28f, -1.0f);
        glVertex2f(0.05f, -0.315f);
        glVertex2f(0.03f, -0.315f);
        glEnd();
        glPopMatrix();
    }

    void drawTree_s1(float x, float y, float scale) {
        float dim = isNight ? 0.6f : 1.0f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 0.9f);
        glColor3f(0.55f * dim, 0.27f * dim, 0.07f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.05f, -0.6f);
        glVertex2f(0.05f, -0.6f);
        glColor3f(0.0f * dim, 0.6f * dim, 0.3f * dim);
        glVertex2f(0.05f, -0.3f);
        glVertex2f(-0.05f, -0.3f);
        glEnd();
        glColor3f(0.0f * dim, 0.5f * dim, 0.1f * dim);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.25f, -0.5f);
        glVertex2f(0.25f, -0.5f);
        glColor3f(0.0f * dim, 0.5f * dim, 0.3f * dim);
        glVertex2f(0.0f, 0.1f);
        glEnd();
        glColor3f(0.0f* dim, 0.5f* dim, 0.1f* dim);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.22f, -0.27f);
        glVertex2f(0.22f, -0.27f);
        glColor3f(0.0f* dim, 0.6f* dim, 0.3f* dim);
        glVertex2f(0.0f, 0.25f);
        glEnd();
        glColor3f(0.0f* dim, 0.5f* dim, 0.1f* dim);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.17f, -0.05f);
        glVertex2f(0.17f, -0.05f);
        glColor3f(0.0f* dim, 0.5f* dim, 0.3f* dim);
        glVertex2f(0.0f, 0.30f);
        glEnd();
        glColor3f(0.0f* dim, 0.5f* dim, 0.1f* dim);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.13f, 0.1f);
        glVertex2f(0.13f, 0.1);
        glColor3f(0.0f* dim, 0.5f* dim, 0.3f* dim);
        glVertex2f(0.0f, 0.35f);
        glEnd();
        glColor3f(0.9f* dim, 1.2f* dim, 0.9f* dim);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.03f, 0.3f);
        glVertex2f(0.03f, 0.3f);
        glColor3f(0.0f* dim, 0.9f* dim, 0.3f* dim);
        glVertex2f(0.0f, 0.45f);
        glEnd();
        glPopMatrix();
    }

    void drawFoliageCluster(float cx, float cy, float radius, int density) {
        float dim = isNight ? 0.6f : 1.0f;
        default_random_engine leaf_gen(static_cast<unsigned int>((cx * 100) + (cy * 100)));
        uniform_real_distribution<float> offset_dist(-radius, radius);
        uniform_real_distribution<float> color_dist(0.0f, 0.3f);
        uniform_real_distribution<float> size_dist(0.02f, 0.07f);
        for (int i = 0; i < density; ++i) {
            float offX = offset_dist(leaf_gen);
            float offY = offset_dist(leaf_gen);
            if (sqrt(offX * offX + offY * offY) < radius) {
                float r = 0.05f + color_dist(leaf_gen);
                float g = 0.3f + color_dist(leaf_gen);
                float b = 0.05f + color_dist(leaf_gen);
                glColor4f(r * dim, g * dim, b * dim, 0.7f);
                drawFilledCircle_s1(cx + offX, cy + offY, size_dist(leaf_gen));
            }
        }
    }

    void drawRealisticTree(float x, float y, float scale) {
        float dim = isNight ? 0.6f : 1.0f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(0.36f * dim, 0.25f * dim, 0.20f * dim);
        glLineWidth(30.0f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, -0.8f); glVertex2f(0.0f, -0.1f);
        glVertex2f(0.0f, -0.1f); glVertex2f(-0.2f, 0.2f);
        glVertex2f(0.0f, 0.0f); glVertex2f(0.25f, 0.25f);
        glVertex2f(0.0f, 0.1f); glVertex2f(0.05f, 0.4f);
        glEnd();
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(-0.2f, 0.2f); glVertex2f(-0.3f, 0.4f);
        glVertex2f(-0.2f, 0.2f); glVertex2f(-0.1f, 0.4f);
        glVertex2f(0.25f, 0.25f); glVertex2f(0.15f, 0.45f);
        glVertex2f(0.25f, 0.25f); glVertex2f(0.4f, 0.4f);
        glEnd();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        drawFoliageCluster(-0.2f, 0.45f, 0.3f, 200);
        drawFoliageCluster(0.2f, 0.5f, 0.35f, 250);
        drawFoliageCluster(0.4f, 0.3f, 0.25f, 150);
        drawFoliageCluster(0.0f, 0.2f, 0.3f, 200);
        glDisable(GL_BLEND);
        glPopMatrix();
    }

    void drawLeafCluster_s1(float x, float y, float radius, float lightFactor) {
        float dim = isNight ? 0.6f : 1.0f;
        glPushMatrix();
        glTranslatef(x, y, 0.8f);
        glColor3f(0.15f * dim, 0.5f * dim, 0.15f * dim);
        drawFilledCircle_s1(0.0f, 0.0f, radius);
        glColor3f(0.15f * dim, 0.5f * dim, 0.15f * dim);
        drawFilledCircle_s1(-0.3f, radius * 0.2f, radius * 0.9f);
        drawFilledCircle_s1( 0.3f, 0.0f, radius * 0.9f);
        drawFilledCircle_s1(-0.3f, 0.0f, radius * 0.9f);
        drawFilledCircle_s1(-0.3f, -radius * 0.2f, radius * 0.8f);
        glColor3f(0.0f* dim, 0.5f* dim, 0.0f* dim);
        glLineWidth(0.3f);
        int segments = 23;
        float angleStep = 0.5f * 3.14159f / segments;
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < segments; i++) {
            float angle = i * angleStep;
            glVertex2f(radius * cos(angle), radius * sin(angle));
        }
        glEnd();
        glPopMatrix();
    }

    void drawLeafyTree(float x, float y, float scale) {
        float dim = isNight ? 0.6f : 1.0f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 0.8f);
        glColor3f(0.55f * dim, 0.27f * dim, 0.07f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.1f, -0.8f);
        glVertex2f(0.1f, -0.8f);
        glVertex2f(0.06f, -0.1f);
        glVertex2f(-0.06f, -0.1f);
        glEnd();
        glLineWidth(5.0f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, -0.1f); glVertex2f(0.0f, 0.25f);
        glVertex2f(0.0f, -0.1f); glVertex2f(-0.2f, 0.1f);
        glVertex2f(0.0f, -0.1f); glVertex2f(0.2f, 0.1f);
        glVertex2f(-0.06f, -0.2f); glVertex2f(-0.3f, -0.05f);
        glVertex2f(0.06f, -0.2f); glVertex2f(0.3f, -0.05f);
        glEnd();
        drawLeafCluster_s1(0.0f, 0.45f, 0.35f, 1.0f);
        drawLeafCluster_s1(0.1f, 0.35f, 0.2f, 1.0f);
        drawLeafCluster_s1(0.10f, 0.3f, 0.25f, 1.0f);
        drawLeafCluster_s1(-0.25f, 0.25f, 0.3f, 1.0f);
        drawLeafCluster_s1(0.18f, 0.2f, 0.3f, 1.0f);
        drawLeafCluster_s1(0.05f, 0.2f, 0.1f, 1.0f);
        glBegin(GL_LINES);
        glLineWidth(0.5f);
        glColor3f(0.0f,0.0f,0.0f);
        glVertex2f(0.0f, -0.1f);
        glVertex2f(0.0f, 0.25f);
        glVertex2f(0.0f, -0.1f); glVertex2f(-0.2f, 0.1f);
        glVertex2f(0.0f, -0.1f); glVertex2f(0.2f, 0.1f);
        glVertex2f(-0.06f, -0.2f);
        glVertex2f(-0.3f, -0.05f);
        glVertex2f(0.06f, -0.2f); glVertex2f(0.3f, -0.05f);
        glEnd();
        glPopMatrix();
    }

    void drawBrick(float x, float y, float width, float height) {
        float dim = isNight ? 0.6f : 1.0f;
        glColor3f(0.7f * dim, 0.3f * dim, 0.2f * dim);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
    }

    void drawBricks(float x, float y, float width, float height, float brickWidth, float brickHeight, bool stagger) {
        float dim = isNight ? 0.6f : 1.0f;
        float brickGap = 0.004f;
        float currentY = y;
        glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + width, y);
        glVertex2f(x + width, y + height);
        glVertex2f(x, y + height);
        glEnd();
        for (int i = 0; currentY < y + height; ++i) {
            float currentRowWidth = 0.0f;
            float startX = x;
            if (stagger && i % 2 == 1) {
                startX += brickWidth / 2.0f;
            }
            while (currentRowWidth < width) {
                drawBrick(startX + currentRowWidth, currentY, brickWidth - brickGap, brickHeight - brickGap);
                currentRowWidth += brickWidth;
            }
            currentY += brickHeight;
        }
    }

    void tunnel() {
        float dim = isNight ? 0.6f : 1.0f;
        glPushMatrix();
        glTranslatef(0.0f, -0.2f, 0.0f);
        drawBricks(-0.75f, -0.2f, 1.5f, 0.8f, 0.1f, 0.05f, true);
        glColor3f(0.3f, 0.3f, 0.3f);
        drawSemiCircle_s1(0.0f, -0.2f, 0.266f, 0.38f);
        if(isNight) glColor3f(0.5f, 0.5f, 0.5f);
        else glColor3f(0.53f, 0.81f, 0.98f);
        drawSemiCircle_s1(0.0f, -0.1f, 0.126f, 0.18f);
        glBegin(GL_QUADS);
        if(isNight) glColor3f(0.9f, 0.9f, 0.9f); else glColor3f(0.53f, 0.81f, 0.98f);
        glVertex2f(-0.124f, -0.1f);
        glVertex2f(0.124f, -0.1f);
        if(isNight) glColor3f(0.9f, 0.9f, 0.7f); else glColor3f(0.7f, 0.98f, 1.0f);
        glVertex2f(0.124f, -0.2f);
        glVertex2f(-0.124f, -0.2f);
        glEnd();
        glPopMatrix();
        drawRailsPerspective();
    }

    void drawGravelBallast(float x,float y, float z) {
        float dim = isNight ? 0.5f : 1.0f;
        glPushMatrix();
        glScalef(x,y,z);
        glBegin(GL_POLYGON);
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        glVertex2f(-0.6f, -1.0f);
        glVertex2f(0.6f, -1.0f);
        glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
        glVertex2f(0.08f, -0.39f);
        glVertex2f(-0.08f, -0.39f);
        glEnd();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        default_random_engine gravel_gen(123);
        uniform_real_distribution<float> y_dist(-1.0f, -0.39f);
        uniform_real_distribution<float> color_dist(-0.1f, 0.1f);
        uniform_real_distribution<float> size_variation(0.5f, 1.5f);
        int num_stones = 3000;
        for (int i = 0; i < num_stones; ++i) {
            float y_pos = y_dist(gravel_gen);
            float t = (y_pos + 1.0f) / (-0.39f + 1.0f);
            if (t < 0.0f || t > 1.0f) continue;
            float left_x = -0.6f * (1.0f - t) + -0.08f * t;
            float right_x = 0.6f * (1.0f - t) + 0.08f * t;
            uniform_real_distribution<float> x_dist(left_x, right_x);
            float x_pos = x_dist(gravel_gen);
            float stone_size = (0.015f * (1.0f - t) + 0.003f * t) * size_variation(gravel_gen);
            float color_offset = color_dist(gravel_gen);
            glColor4f( (0.4f + color_offset) * dim, (0.38f + color_offset) * dim, (0.36f + color_offset) * dim, 0.7f);
            drawFilledCircle_s1(x_pos, y_pos, stone_size);
        }
        glDisable(GL_BLEND);
        glPopMatrix();
    }

    void drawStone(float x, float y, float scale) {
        float dim = isNight ? 0.5f : 1.0f;
        generator.seed(static_cast<unsigned int>((x * 100) + (y * 100)));
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        float baseR = 0.5f, baseG = 0.5f, baseB = 0.5f;
        glColor3f(baseR * dim, baseG * dim, baseB * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.3f + dist_offset(generator), 0.0f + dist_offset(generator));
        glVertex2f(-0.2f + dist_offset(generator), -0.1f + dist_offset(generator));
        glVertex2f(0.1f + dist_offset(generator), -0.15f + dist_offset(generator));
        glVertex2f(0.3f + dist_offset(generator), -0.05f + dist_offset(generator));
        glVertex2f(0.25f + dist_offset(generator), 0.15f + dist_offset(generator));
        glVertex2f(0.1f + dist_offset(generator), 0.25f + dist_offset(generator));
        glVertex2f(-0.1f + dist_offset(generator), 0.3f + dist_offset(generator));
        glVertex2f(-0.25f + dist_offset(generator), 0.18f + dist_offset(generator));
        glEnd();
        glColor3f(0.4f * dim, 0.4f * dim, 0.4f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.2f + dist_offset(generator), 0.1f + dist_offset(generator));
        glVertex2f(-0.05f + dist_offset(generator), 0.25f + dist_offset(generator));
        glVertex2f(0.05f + dist_offset(generator), 0.2f + dist_offset(generator));
        glVertex2f(-0.1f + dist_offset(generator), 0.15f + dist_offset(generator));
        glEnd();
        glColor3f(0.4f * dim, 0.4f * dim, 0.4f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(0.0f + dist_offset(generator), -0.1f + dist_offset(generator));
        glVertex2f(0.2f + dist_offset(generator), -0.08f + dist_offset(generator));
        glVertex2f(0.15f + dist_offset(generator), 0.05f + dist_offset(generator));
        glVertex2f(-0.05f + dist_offset(generator), 0.0f + dist_offset(generator));
        glEnd();
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        glLineWidth(1.3f);
        glBegin(GL_LINES);
        glVertex2f(-0.1f + dist_crack_offset(generator), 0.05f + dist_crack_offset(generator));
        glVertex2f(0.0f + dist_crack_offset(generator), -0.05f + dist_crack_offset(generator));
        glVertex2f(0.1f + dist_crack_offset(generator), 0.1f + dist_crack_offset(generator));
        glVertex2f(0.2f + dist_crack_offset(generator), 0.0f + dist_crack_offset(generator));
        glVertex2f(-0.18f + dist_crack_offset(generator), -0.02f + dist_crack_offset(generator));
        glVertex2f(-0.05f + dist_crack_offset(generator), 0.1f + dist_crack_offset(generator));
        glEnd();
        glColor3f(0.5f * dim, 0.5f * dim, 0.5f * dim);
        glLineWidth(1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.3f + dist_offset(generator), 0.0f + dist_offset(generator));
        glVertex2f(-0.2f + dist_offset(generator), -0.1f + dist_offset(generator));
        glVertex2f(0.1f + dist_offset(generator), -0.15f + dist_offset(generator));
        glVertex2f(0.3f + dist_offset(generator), -0.05f + dist_offset(generator));
        glVertex2f(0.25f + dist_offset(generator), 0.15f + dist_offset(generator));
        glVertex2f(0.1f + dist_offset(generator), 0.25f + dist_offset(generator));
        glVertex2f(-0.1f + dist_offset(generator), 0.3f + dist_offset(generator));
        glVertex2f(-0.25f + dist_offset(generator), 0.18f + dist_offset(generator));
        glEnd();
        glPopMatrix();
    }

    void drawGrassBlades(float x, float y, float width, float height, int density, float scale) {
        float dim = isNight ? 0.6f : 1.0f;
        default_random_engine generator;
        uniform_real_distribution<float> x_dist(x, x + width);
        uniform_real_distribution<float> y_dist(y, y + height);
        uniform_real_distribution<float> blade_width_dist(0.005f * scale, 0.015f * scale);
        uniform_real_distribution<float> blade_height_dist(0.05f * scale, 0.15f * scale);
        uniform_real_distribution<float> blade_lean_dist(-0.02f * scale, 0.02f * scale);
        uniform_real_distribution<float> color_dist(0.0f, 1.0f);
        for (int i = 0; i < density; ++i) {
            float blade_x = x_dist(generator);
            float blade_y = y_dist(generator);
            float b_width = blade_width_dist(generator);
            float b_height = blade_height_dist(generator);
            float b_lean = blade_lean_dist(generator);
            glColor3f((0.1f + color_dist(generator) * 0.1f) * dim, (0.5f + color_dist(generator) * 0.2f) * dim, (0.1f + color_dist(generator) * 0.1f) * dim);
            glBegin(GL_TRIANGLES);
            glVertex2f(blade_x - b_width / 2.0f, blade_y);
            glVertex2f(blade_x + b_width / 2.0f, blade_y);
            glVertex2f(blade_x + b_lean, blade_y + b_height);
            glEnd();
        }
    }

    void drawGrassWall() {
        float dim = isNight ? 0.6f : 1.0f;
        glColor3f(0.1f * dim, 0.4f * dim, 0.0f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-2.0f, -0.4f);
        glVertex2f(-0.75f, -0.4f);
        glVertex2f(-0.75f, 0.4f);
        glVertex2f(-2.0f, 0.4f);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(2.0f, -0.4f);
        glVertex2f(0.75f, -0.4f);
        glVertex2f(0.75f, 0.4f);
        glVertex2f(2.0f, 0.4f);
        glEnd();
        drawGrassBlades(-2.0f, -0.4f, 1.25f, 0.8f, 1000, 1.0f);
        drawGrassBlades(0.75f, -0.4f, 1.25f, 0.8f, 1000, 1.0f);
    }

    void drawRedCar(float x, float y) {
        float dim = isNight ? 0.7f : 1.0f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(0.5f, 0.5f, 1.0f);
        glColor3f(1.0f * dim, 0.0f * dim, 0.0f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.24f, 0.0f);
        glVertex2f(0.22f, 0.0f);
        glVertex2f(0.22f, 0.1f);
        glColor3f(0.6f * dim, 0.0f * dim, 0.2f * dim);
        glVertex2f(0.15f, 0.15f);
        glVertex2f(-0.11f, 0.15f);
        glVertex2f(-0.24f, 0.09f);
        glEnd();
        glColor3f(0.75f * dim, 0.75f * dim, 0.73f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, 0.07f);
        glVertex2f(-0.01f, 0.07f);
        glVertex2f(-0.01f, 0.13f);
        glVertex2f(-0.1f, 0.13f);
        glEnd();
        glColor3f(0.75f * dim, 0.75f * dim, 0.73f * dim);
        glBegin(GL_LINE_STRIP);
        glVertex2f(0.02f, 0.01f);
        glVertex2f(0.02f, 0.14f);
        glEnd();
        glColor3f(0.75f * dim, 0.75f * dim, 0.73f * dim);
        glBegin(GL_QUADS);
        glVertex2f(0.05f, 0.07f);
        glVertex2f(0.14f, 0.07f);
        glVertex2f(0.14f, 0.13f);
        glVertex2f(0.05f, 0.13f);
        glEnd();
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        drawFilledCircle_s1(-0.18f, 0.0f, 0.05f);
        drawFilledCircle_s1(0.18f, 0.0f, 0.05f);
        glColor3f(0.75f * dim, 0.75f * dim, 0.75f * dim);
        drawFilledCircle_s1(-0.18f, 0.0f, 0.02f);
        drawFilledCircle_s1(0.18f, 0.0f, 0.02f);
        if (isNight) {
            glColor3f(1.0f, 1.0f, 0.7f);
            drawFilledCircle_s1(-0.23f, 0.06f, 0.02f);
            glColor3f(1.0f, 0.0f, 0.0f);
            drawFilledCircle_s1(0.21f, 0.06f, 0.015f);
        }
        glPopMatrix();
    }

    void drawBlueCar(float x, float y) {
        float dim = isNight ? 0.7f : 1.0f;
        glPushMatrix();
        glTranslatef(-x, y, 0.0f);
        glScalef(-0.5f, 0.5f, -1.0f);
        glColor3f(0.0f * dim, 0.0f * dim, 1.0f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.24f, 0.0f);
        glVertex2f(0.22f, 0.0f);
        glVertex2f(0.22f, 0.1f);
        glColor3f(0.0f * dim, 0.3f * dim, 0.5f * dim);
        glVertex2f(0.15f, 0.15f);
        glVertex2f(-0.11f, 0.15f);
        glVertex2f(-0.24f, 0.09f);
        glEnd();
        glColor3f(0.75f * dim, 0.75f * dim, 0.73f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, 0.07f);
        glVertex2f(-0.01f, 0.07f);
        glVertex2f(-0.01f, 0.13f);
        glVertex2f(-0.1f, 0.13f);
        glEnd();
        glColor3f(0.75f * dim, 0.75f * dim, 0.75f * dim);
        glBegin(GL_LINE_STRIP);
        glVertex2f(0.02f, 0.01f);
        glVertex2f(0.02f, 0.14f);
        glEnd();
        glColor3f(0.75f * dim, 0.75f * dim, 0.73f * dim);
        glBegin(GL_QUADS);
        glVertex2f(0.05f, 0.07f);
        glVertex2f(0.14f, 0.07f);
        glVertex2f(0.14f, 0.13f);
        glVertex2f(0.05f, 0.13f);
        glEnd();
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        drawFilledCircle_s1(-0.18f, 0.0f, 0.05f);
        drawFilledCircle_s1(0.18f, 0.0f, 0.05f);
        glColor3f(0.75f * dim, 0.75f * dim, 0.75f * dim);
        drawFilledCircle_s1(-0.18f, 0.0f, 0.02f);
        drawFilledCircle_s1(0.18f, 0.0f, 0.02f);
        if (isNight) {
            glColor3f(1.0f, 1.0f, 0.7f);
            drawFilledCircle_s1(-0.23f, 0.06f, 0.02f);
            glColor3f(1.0f, 0.0f, 0.0f);
            drawFilledCircle_s1(0.21f, 0.06f, 0.015f);
        }
        glPopMatrix();
    }

    void drawRealisticBus(float x, float y) {
        float dim = isNight ? 0.7f : 1.0f;
        glPushMatrix();
        glTranslatef(-x, y, 0.0f);
        glScalef(0.5f, 0.5f, 1.0f);
        glColor3f(0.25f * dim, 0.55f * dim, 0.35f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.45f, 0.0f);
        glVertex2f(-0.4f, -0.01f);
        glVertex2f(0.4f, -0.01f);
        glVertex2f(0.42f, 0.0f);
        glVertex2f(0.42f, 0.18f);
        glVertex2f(0.38f, 0.22f);
        glVertex2f(-0.35f, 0.24f);
        glVertex2f(-0.45f, 0.18f);
        glEnd();
        glColor3f(0.1f * dim, 0.1f * dim, 0.1f * dim);
        drawFilledCircle_s1(-0.3f, 0.0f, 0.06f);
        drawFilledCircle_s1(0.28f, 0.0f, 0.06f);
        glColor3f(0.75f * dim, 0.75f * dim, 0.75f * dim);
        drawFilledCircle_s1(-0.3f, 0.0f, 0.03f);
        drawFilledCircle_s1(0.28f, 0.0f, 0.03f);
        glColor3f(isNight ? 0.9f : 0.85f * dim, isNight ? 0.9f : 0.85f * dim, isNight ? 0.0f : 0.85f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.42f, 0.11f);
        glVertex2f(-0.42f, 0.16f);
        glVertex2f(-0.38f, 0.21f);
        glVertex2f(-0.27f, 0.21f);
        glVertex2f(-0.27f, 0.11f);
        glEnd();
        for (int i = 0; i < 6; ++i) {
            float start_x = -0.25f + (i * 0.1f);
            glBegin(GL_QUADS);
            glVertex2f(start_x, 0.11f);
            glVertex2f(start_x + 0.08f, 0.11f);
            glVertex2f(start_x + 0.08f, 0.2f);
            glVertex2f(start_x, 0.2f);
            glEnd();
        }
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.45f, 0.16f);
        glVertex2f(-0.48f, 0.17f);
        glVertex2f(-0.48f, 0.14f);
        glVertex2f(-0.45f, 0.13f);
        glEnd();
        glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
        glBegin(GL_QUADS);
        glVertex2f(0.36f, 0.04f);
        glVertex2f(0.40f, 0.04f);
        glVertex2f(0.40f, 0.12f);
        glVertex2f(0.36f, 0.12f);
        glEnd();
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(0.41f, 0.15f);
        glVertex2f(0.42f, 0.15f);
        glVertex2f(0.42f, 0.17f);
        glVertex2f(0.41f, 0.17f);
        glEnd();
        glColor3f(0.85f * dim, 0.85f * dim, 0.85f * dim);
        for (int i = 0; i < 5; ++i) {
            float start_x = -0.2f + (i * 0.09f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(start_x, 0.02f);
            glVertex2f(start_x + 0.08f, 0.02f);
            glVertex2f(start_x + 0.08f, 0.08f);
            glVertex2f(start_x, 0.08f);
            glEnd();
        }
        if (isNight) {
            glColor3f(1.0f, 1.0f, 0.7f);
            drawFilledCircle_s1(-0.44f, 0.06f, 0.02f);
        }
        glPopMatrix();
    }

    void drawDarkBus(float x, float y) {
        float dim = isNight ? 0.7f : 1.0f;
        glPushMatrix();
        glTranslatef(-x, y, 0.0f);
        glScalef(-0.5f, 0.5f, 1.0f);
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.45f, 0.0f);
        glVertex2f(-0.4f, -0.01f);
        glVertex2f(0.4f, -0.01f);
        glVertex2f(0.42f, 0.0f);
        glVertex2f(0.42f, 0.18f);
        glVertex2f(0.38f, 0.22f);
        glVertex2f(-0.35f, 0.24f);
        glVertex2f(-0.45f, 0.18f);
        glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
        drawFilledCircle_s1(-0.3f, 0.0f, 0.06f);
        drawFilledCircle_s1(0.28f, 0.0f, 0.06f);
        glColor3f(0.6f * dim, 0.6f * dim, 0.6f * dim);
        drawFilledCircle_s1(-0.3f, 0.0f, 0.03f);
        drawFilledCircle_s1(0.28f, 0.0f, 0.03f);
        glColor3f(isNight ? 0.9f : 0.53f * dim, isNight ? 0.9f : 0.81f * dim, isNight ? 0.0f : 0.98f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.42f, 0.11f);
        glVertex2f(-0.42f, 0.16f);
        glVertex2f(-0.38f, 0.21f);
        glVertex2f(-0.27f, 0.21f);
        glVertex2f(-0.27f, 0.11f);
        glEnd();
        for (int i = 0; i < 6; ++i) {
            float start_x = -0.25f + (i * 0.1f);
            glBegin(GL_QUADS);
            glVertex2f(start_x, 0.11f);
            glVertex2f(start_x + 0.08f, 0.11f);
            glVertex2f(start_x + 0.08f, 0.2f);
            glVertex2f(start_x, 0.2f);
            glEnd();
        }
        glColor3f(0.2f * dim, 0.2f * dim, 0.2f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.45f, 0.16f);
        glVertex2f(-0.48f, 0.17f);
        glVertex2f(-0.48f, 0.14f);
        glVertex2f(-0.45f, 0.13f);
        glEnd();
        glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
        glBegin(GL_QUADS);
        glVertex2f(0.36f, 0.04f);
        glVertex2f(0.40f, 0.04f);
        glVertex2f(0.40f, 0.12f);
        glVertex2f(0.36f, 0.12f);
        glEnd();
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(0.41f, 0.15f);
        glVertex2f(0.42f, 0.15f);
        glVertex2f(0.42f, 0.17f);
        glVertex2f(0.41f, 0.17f);
        glEnd();
        glColor3f(0.5f * dim, 0.5f * dim, 0.5f * dim);
        for (int i = 0; i < 5; ++i) {
            float start_x = -0.2f + (i * 0.09f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(start_x, 0.02f);
            glVertex2f(start_x + 0.08f, 0.02f);
            glVertex2f(start_x + 0.08f, 0.08f);
            glVertex2f(start_x, 0.08f);
            glEnd();
        }
        if (isNight) {
            glColor3f(1.0f, 1.0f, 0.7f);
            drawFilledCircle_s1(-0.44f, 0.06f, 0.02f);
        }
        glPopMatrix();
    }

    void drawGrassPatch(float x, float y, float scale, int density) {
        float dim = isNight ? 0.6f : 1.0f;
        default_random_engine generator;
        uniform_real_distribution<float> angle_dist(-0.2f, 0.2f);
        uniform_real_distribution<float> height_dist(0.05f * scale, 0.15f * scale);
        uniform_real_distribution<float> color_dist(0.0f, 0.2f);
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        for (int i = 0; i < density; ++i) {
            float angle = angle_dist(generator);
            float height = height_dist(generator);
            glColor3f((0.1f + color_dist(generator)) * dim, (0.5f + color_dist(generator)) * dim, (0.1f + color_dist(generator)) * dim);
            glBegin(GL_TRIANGLES);
            glVertex2f(-0.005f, 0.0f);
            glVertex2f(0.005f, 0.0f);
            glVertex2f(angle, height);
            glEnd();
        }
        glPopMatrix();
    }

    void drawLeafPuff(float x, float y, float radius) {
        float dim = isNight ? 0.6f : 1.0f;
        glColor3f(0.1f * dim, 0.35f * dim, 0.05f * dim);
        drawFilledCircle_s1(x, y, radius);
        glColor3f(0.15f * dim, 0.5f * dim, 0.15f * dim);
        drawFilledCircle_s1(x + radius * 0.1f, y + radius * 0.1f, radius * 0.9f);
        drawFilledCircle_s1(x + radius * 0.2f, y + radius * 0.0f, radius * 0.9f);
        glColor3f(0.15f * dim, 0.2f * dim, 0.17f * dim);
        glColor3f(0.2f * dim, 0.6f * dim, 0.15f * dim);
        drawFilledCircle_s1(x - radius * 0.2f, y + radius * 0.3f, radius * 0.6f);
        drawFilledCircle_s1(x + radius * 0.3f, y + radius * 0.2f, radius * 0.5f);
    }

    void drawStylizedTree(float x, float y, float scale) {
        float dim = isNight ? 0.6f : 1.0f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glLineWidth(8.0f);
        glColor3f(0.35f * dim, 0.18f * dim, 0.05f * dim);
        glBegin(GL_LINE_STRIP);
        glVertex2f(0.0f, -0.2f); glVertex2f(-0.1f, 0.2f);
        glVertex2f(0.0f, 0.1f); glVertex2f(0.15f, 0.3f);
        glVertex2f(0.0f, 0.3f); glVertex2f(0.0f, 0.6f);
        glVertex2f(-0.1f, 0.2f); glVertex2f(-0.3f, 0.3f);
        glVertex2f(0.05f, -0.2f); glVertex2f(0.2f, -0.1f);
        glVertex2f(-0.05f, -0.5f); glVertex2f(-0.25f, -0.2f);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(0.45f * dim, 0.22f * dim, 0.07f * dim);
        glVertex2f(0.1f, -0.8f);
        glVertex2f(0.15f, 0.1f);
        glColor3f(0.25f * dim, 0.12f * dim, 0.02f * dim);
        glVertex2f(-0.15f, 0.1f);
        glVertex2f(-0.1f, -0.8f);
        glEnd();
        drawLeafPuff(0.0f, 0.65f, 0.3f);
        drawLeafPuff(-0.35f, 0.35f, 0.25f);
        drawLeafPuff(0.3f, 0.2f, 0.28f);
        drawLeafPuff(0.25f, -0.1f, 0.2f);
        drawLeafPuff(-0.25f, -0.15f, 0.18f);
        glPopMatrix();
    }

    void drawFrontalTrain(float x, float y,float z) {
        float dim = isNight ? 0.7f : 1.0f;
        glPushMatrix();
        glTranslatef(0.0f, frontalTrainY, 0.0f);
        glScalef(frontalTrainSize, frontalTrainSize, 1.0f);
        glColor3f(0.8f * dim, 0.0f * dim, 0.0f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.28f, -0.09f);
        glVertex2f(0.28f, -0.09f);
        glColor3f(0.4f * dim, 0.0f * dim, 0.2f * dim);
        glVertex2f(0.28f, 0.25f);
        glVertex2f(-0.28f, 0.25f);
        glEnd();
        glColor3f(0.0f * dim, 0.3f * dim, 0.0f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.29f, 0.25f);
        glVertex2f(0.29f, 0.25f);
        glColor3f(0.0f * dim, 0.3f * dim, 0.5f * dim);
        glVertex2f(0.29f, 0.4f);
        glVertex2f(-0.29f, 0.4f);
        glEnd();
        glColor3f(0.7f * dim, 0.6f * dim, 0.0f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.16f, 0.25f);
        glVertex2f(-0.16f, 0.35f);
        glColor3f(0.8f * dim, 0.5f * dim, 0.0f * dim);
        glVertex2f(0.16f, 0.25f);
        glVertex2f(0.16f, 0.35f);
        glEnd();
        glColor3f(0.6f * dim, 0.6f * dim, 0.6f * dim);
        glLineWidth(2.2f);
        glBegin(GL_LINES);
        glVertex2f(-0.28f, -0.05f); glVertex2f(-0.28f, 0.28f);
        glVertex2f(0.28f, -0.05f); glVertex2f(0.28f, 0.28f);
        glVertex2f(-0.17f, -0.05f); glVertex2f(-0.17f, 0.28f);
        glVertex2f(0.17f, -0.05f); glVertex2f(0.17f, 0.28f);
        glVertex2f(-0.27f, 0.0f); glVertex2f(0.27f, 0.0f);
        glVertex2f(-0.27f, 0.05f); glVertex2f(0.27f, 0.05f);
        glVertex2f(-0.3f, 0.28f); glVertex2f(0.3f, 0.28f);
        glEnd();
        glColor3f(0.2f * dim, 0.0f * dim, 0.8f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(-0.3f, 0.4f);
        glVertex2f(-0.3f, 0.5f);
        glVertex2f(-0.1f, 0.6f);
        glColor3f(0.2f * dim, 0.0f * dim, 0.5f * dim);
        glVertex2f(0.1f, 0.6f);
        glVertex2f(0.3f, 0.5f);
        glVertex2f(0.3f, 0.4f);
        glEnd();

         glColor3f(lightIntensity, lightIntensity, lightIntensity);
    drawFilledCircle_s1(0.03f, 0.52f, 0.02f);
    drawFilledCircle_s1(-0.03f, 0.52f, 0.02f);

    glColor3f(0.9f * dim,0.9f * dim,0.9f * dim);
    drawFilledCircle_s1(0.03f, 0.6f, 0.02f);
      glColor3f(0.5f * dim,0.5f * dim,0.5f * dim);
    drawFilledCircle_s1(0.03f, 0.6f, 0.01f);

    glColor3f(0.9f * dim,0.9f * dim,0.9f * dim);
    drawFilledCircle_s1(-0.03f, 0.6f, 0.02f);
         glColor3f(0.5f * dim,0.5f * dim,0.5f * dim);
    drawFilledCircle_s1(-0.03f, 0.6f, 0.01f);

    glColor3f(0.9f * dim,0.9f * dim,0.9f * dim);
    drawFilledCircle_s1(0.0f, 0.62f, 0.03f);
       glColor3f(0.5f * dim,0.5f * dim,0.5f * dim);
    drawFilledCircle_s1(0.0f, 0.62f, 0.02f);
    glColor3f(0.1f * dim,0.1f * dim,0.1f * dim);
    drawFilledCircle_s1(0.0f, 0.62f, 0.01f);


        glColor3f(0.85f * dim, 0.95f * dim, 1.0f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.18, 0.42f);
        glVertex2f(-0.04f, 0.42f);
        glColor3f(0.5f * dim, 0.5f * dim, 0.7f * dim);
        glVertex2f(-0.04f, 0.5f);
        glVertex2f(-0.18f, 0.5f);
        glEnd();
        glColor3f(0.9f * dim, 0.95f * dim, 1.0f * dim);
        glBegin(GL_QUADS);
        glVertex2f(0.04f, 0.42f);
        glVertex2f(0.18f, 0.42f);
        glColor3f(0.5f * dim, 0.5f * dim, 0.7f * dim);
        glVertex2f(0.18f, 0.5f);
        glVertex2f(0.04f, 0.5f);
        glEnd();
        glLineWidth(1.2f);
        glColor3f(0.0f * dim, 0.0f * dim, 0.5f * dim);
        glBegin(GL_LINES);
        glVertex2f(0.18f, 0.42f); glVertex2f(0.04f, 0.42f);
        glVertex2f(0.18f, 0.44f); glVertex2f(0.04f, 0.44f);
        glVertex2f(0.18f, 0.46f); glVertex2f(0.04f, 0.46f);
        glVertex2f(0.18f, 0.48f); glVertex2f(0.04f, 0.48f);
        glVertex2f(0.18f, 0.5f); glVertex2f(0.04f, 0.5f);
        glEnd();
        glLineWidth(1.2f);
        glColor3f(0.0f * dim, 0.0f * dim, 0.5f * dim);
        glBegin(GL_LINES);
        glVertex2f(-0.18f, 0.42f); glVertex2f(-0.04f, 0.42f);
        glVertex2f(-0.18f, 0.44f); glVertex2f(-0.04f, 0.44f);
        glVertex2f(-0.18f, 0.46f); glVertex2f(-0.04f, 0.46f);
        glVertex2f(-0.18f, 0.48f); glVertex2f(-0.04f, 0.48f);
        glVertex2f(-0.18f, 0.5f); glVertex2f(-0.04f, 0.5f);
        glEnd();
        glColor3f(0.8f * dim, 0.9f * dim, 1.0f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, 0.1f);
        glVertex2f(-0.02f, 0.1f);
        glVertex2f(-0.02f, 0.2f);
        glVertex2f(-0.1f, 0.2f);
        glVertex2f(0.02f, 0.1f);
        glVertex2f(0.1f, 0.1f);
        glVertex2f(0.1f, 0.2f);
        glVertex2f(0.02f, 0.2f);
        glEnd();
        glColor3f(0.1f * dim, 0.1f * dim, 0.1f * dim);
        drawFilledCircle_s1(-0.15f, -0.08f, 0.03f);
        drawFilledCircle_s1(0.15f, -0.08f, 0.03f);
        float headLightBrightness = isNight ? 1.0f : lightIntensity;
        glColor3f(headLightBrightness, headLightBrightness, 0.0f);
        drawFilledCircle_s1(-0.06f, 0.15f, 0.02f);
        drawFilledCircle_s1(0.06f, 0.15f, 0.02f);
        glColor3f(greyLightIntensity, greyLightIntensity, greyLightIntensity);
        drawFilledCircle_s1(-0.2f, 0.1f, 0.02f);
        drawFilledCircle_s1(-0.2f, 0.05f, 0.02f);
        glColor3f(blackLightIntensity, blackLightIntensity, blackLightIntensity);
        drawFilledCircle_s1(0.2f, 0.1f, 0.02f);
        drawFilledCircle_s1(0.2f, 0.05f, 0.02f);
        float textX = -0.07f;
        float textY = 0.36f;
        glColor3f(0.7f * dim, 0.7f * dim, 0.7f * dim);
        glLineWidth(1.5f);
        glPushMatrix();
        glTranslatef(textX+0.03, textY, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(-0.025f, -0.03f); glVertex2f(-0.025f, 0.03f);
        glVertex2f(-0.025f, 0.03f); glVertex2f(0.0f, 0.03f);
        glVertex2f(0.0f, 0.03f); glVertex2f(0.015f, 0.02f);
        glVertex2f(0.015f, 0.02f); glVertex2f(0.015f, 0.0f);
        glVertex2f(-0.025f, 0.0f); glVertex2f(0.015f, 0.0f);
        glVertex2f(-0.025f, -0.03f); glVertex2f(0.0f, -0.03f);
        glVertex2f(0.0f, -0.03f); glVertex2f(0.015f, -0.02f);
        glVertex2f(0.015f, -0.02f); glVertex2f(0.015f, 0.0f);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(textX + 0.08f, textY, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(-0.025f, -0.03f); glVertex2f(-0.025f, 0.03f);
        glVertex2f(-0.025f, 0.03f); glVertex2f(0.0f, 0.03f);
        glVertex2f(0.0f, 0.03f); glVertex2f(0.015f, 0.02f);
        glVertex2f(0.015f, 0.02f); glVertex2f(0.015f, 0.0f);
        glVertex2f(-0.025f, 0.0f); glVertex2f(0.015f, 0.0f);
        glVertex2f(0.0f, 0.0f); glVertex2f(0.015f, -0.03f);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(textX + 0.13f, textY, 0.0f);
        glBegin(GL_LINES);
        glVertex2f(-0.025f, -0.03f); glVertex2f(-0.025f, 0.03f);
        glVertex2f(-0.025f, 0.03f); glVertex2f(0.0f, 0.03f);
        glVertex2f(0.0f, 0.03f); glVertex2f(0.015f, 0.02f);
        glVertex2f(0.015f, 0.02f); glVertex2f(0.015f, 0.0f);
        glVertex2f(-0.025f, 0.0f); glVertex2f(0.015f, 0.0f);
        glVertex2f(-0.025f, -0.03f); glVertex2f(0.0f, -0.03f);
        glVertex2f(0.0f, -0.03f); glVertex2f(0.015f, -0.02f);
        glVertex2f(0.015f, -0.02f); glVertex2f(0.015f, 0.0f);
        glEnd();
        glPopMatrix();
        glPopMatrix();
    }

    void drawRailwaySignal(float x, float y) {
        float dim = isNight ? 0.6f : 1.0f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(1.5f, 1.8f, 1.0f);
        glColor3f(0.4f * dim, 0.4f * dim, 0.4f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.02f, -0.65f);
        glVertex2f(0.02f, -0.65f);
        glVertex2f(0.02f, -0.2f);
        glVertex2f(-0.02f, -0.2f);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, -0.2f);
        glVertex2f(0.1f, -0.2f);
        glVertex2f(0.1f, -0.18f);
        glVertex2f(-0.1f, -0.18f);
        glEnd();
        glColor3f(1.0f * dim, 1.0f * dim, 1.0f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, -0.3f);
        glVertex2f(0.1f, -0.2f);
        glVertex2f(0.12f, -0.22f);
        glVertex2f(-0.08f, -0.32f);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(-0.1f, -0.2f);
        glVertex2f(0.1f, -0.3f);
        glVertex2f(0.08f, -0.32f);
        glVertex2f(-0.12f, -0.22f);
        glEnd();
        glColor3f(0.9f, 0.2f, 0.2f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(-0.1f, -0.3f); glVertex2f(0.1f, -0.2f);
        glVertex2f(0.12f, -0.22f); glVertex2f(-0.08f, -0.32f);
        glEnd();
        glBegin(GL_LINES);
        glVertex2f(-0.1f, -0.2f); glVertex2f(0.1f, -0.3f);
        glVertex2f(0.08f, -0.32f); glVertex2f(-0.12f, -0.22f);
        glEnd();
        glColor3f(signalLightRed, signalLightGreen, signalLightBlue);
        drawFilledCircle_s1(-0.1f, -0.15f, 0.03f);
        drawFilledCircle_s1(0.1f, -0.15f, 0.03f);
        glPopMatrix();
    }

    void drawSunRays() {
        if (isNight) return;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        float centerX = 0.0f, centerY = 1.2f;
        int numRays = 10;
        for (int i = -numRays/2; i < numRays/2; ++i) {
            glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 1.0f, 0.9f, 0.15f);
            glVertex2f(centerX, centerY);
            glColor4f(1.0f, 1.0f, 0.9f, 0.0f);
            float xOffset = i * 0.4f;
            glVertex2f(centerX + xOffset - 0.2f, 0.4f);
            glVertex2f(centerX + xOffset + 0.2f, 0.4f);
            glEnd();
        }
        glDisable(GL_BLEND);
    }

    void drawMountain(float x, float y, float scale) {
        float dim = isNight ? 0.5f : 1.0f;
        glColor3f(0.5f, 0.6f, 0.7f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.35f * scale, y);
        glVertex2f(x + 0.35f * scale, y);
        glVertex2f(x, y + 0.45f * scale);
        glEnd();
        glColor3f(0.0f*dim, 0.4f*dim, 0.0f*dim);
        glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.2f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.2f * scale, y + 0.2f * scale);
        glVertex2f(x, y + 0.45f * scale);
        glEnd();
        glColor3f(0.35f*dim, 0.5f*dim, 0.2f*dim);
        glBegin(GL_TRIANGLES);
        glVertex2f(x + 0.35f * scale, y);
        glVertex2f(x + 0.15f * scale, y + 0.15f * scale);
        glVertex2f(x, y + 0.45f * scale);
        glEnd();
    }

    void drawRiver() {
        float dim = isNight ? 0.5f : 1.0f;
        glPushMatrix();
        glScalef(1.0f, 1.0f, 5.0f);
        glColor3f(0.3f * dim, 0.6f * dim, 1.0f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(0.7f, -0.6f);
        glVertex2f(1.0f, -0.9f);
        glVertex2f(1.6f, -1.0f);
        glVertex2f(2.0f, -0.95f);
        glVertex2f(2.0f, -0.65f);
        glVertex2f(1.1f, -0.55f);
        glEnd();
        glColor3f(0.3f * dim, 0.6f * dim, 0.9f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(0.7f, -0.6f);
        glVertex2f(0.85f, -0.7f);
        glVertex2f(1.2f, -0.7f);
        glVertex2f(1.7f, -0.85f);
        glVertex2f(2.0f, -0.8f);
        glVertex2f(2.0f, -0.65f);
        glVertex2f(1.1f, -0.55f);
        glEnd();
        glColor3f(0.1f * dim, 0.4f * dim, 0.8f * dim);
        glBegin(GL_POLYGON);
        glVertex2f(1.0f, -0.9f);
        glVertex2f(1.6f, -1.0f);
        glVertex2f(2.0f, -0.95f);
        glVertex2f(1.7f, -0.85f);
        glVertex2f(1.2f, -0.7f);
        glVertex2f(0.85f, -0.7f);
        glEnd();
        glPopMatrix();
    }

    void drawSun() {
        glColor3f(1.1f, 1.09f, 0.8f);
        glPushMatrix();
        glScalef(1.2f,1.09f,4.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.8f, 0.8f);
        for (int i = 0; i <= 360; ++i) {
            float angle = 2 * 3.14159f * i / 360;
            glVertex2f(0.8f + 0.05f * cos(angle), 0.8f + 0.05f * sin(angle));
        }
        glEnd();
        glPopMatrix();
    }

    void drawSky() {
        if (isNight) {
            glBegin(GL_QUADS);
            glColor3f(0.0f, 0.0f, 0.1f);
            glVertex2f(-2.0f, 1.0f);
            glVertex2f(2.0f, 1.0f);
            glColor3f(0.1f, 0.1f, 0.25f);
            glVertex2f(2.0f, 0.4f);
            glVertex2f(-2.0f, 0.4f);
            glEnd();
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.1f, 0.1f, 0.15f, 0.5f);
            glBegin(GL_QUADS);
            glVertex2f(-2.0f, 1.0f);
            glVertex2f(2.0f, 1.0f);
            glVertex2f(2.0f, 0.6f);
            glVertex2f(-2.0f, 0.6f);
            glEnd();
            glDisable(GL_BLEND);
            glColor3f(0.9f, 0.9f, 0.85f);
            drawFilledCircle_s1(cloudX + 0.8f, 0.8f, 0.1f);
            glBegin(GL_POINTS);
            for (int i = 0; i < STAR_COUNT; i++) {
                float twinkle = 0.5f + 0.5f * sin(glutGet(GLUT_ELAPSED_TIME) * 0.001f + i * 5);
                glColor3f(starBrightness[i] * twinkle, starBrightness[i] * twinkle, starBrightness[i] * twinkle);
                glVertex2f(starX[i], starY[i]);
            }
            glEnd();
        } else {
            glBegin(GL_QUADS);
            glColor3f(0.53f, 0.81f, 0.98f);
            glVertex2f(-2.0f, 1.0f);
            glVertex2f(2.0f, 1.0f);
            glColor3f(0.7f, 0.9f, 1.0f);
            glVertex2f(2.0f, 0.55f);
            glVertex2f(-2.0f, 0.55f);
            glEnd();
            glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
            drawSun();
            drawCartoonCloud(cloudX, 0.7f, 0.9f);
            drawCartoonCloud(cloudX + 0.6f, 0.8f, 0.8f);
            drawCartoonCloud(cloudX - 0.5f, 0.65f, 0.5f);
        }
    }

    void drawRain() {
        if (!isRaining) return;
        glLineWidth(1.0f);
        glColor4f(0.8f, 0.8f, 0.9f, 0.6f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_LINES);
        for (const auto& drop : raindrops) {
            glVertex2f(drop.x, drop.y);
            glVertex2f(drop.x, drop.y - drop.length);
        }
        glEnd();
        glDisable(GL_BLEND);
    }

    void drawHorizontalRoad() {
        float dim = isNight ? 0.6f : 1.0f;
        glColor3f(0.3f * dim, 0.3f * dim, 0.3f * dim);
        glBegin(GL_QUADS);
        glVertex2f(-2.0f, 0.55f);
        glVertex2f(2.0f, 0.55f);
        glVertex2f(2.0f, 0.42f);
        glVertex2f(-2.0f, 0.42f);
        glEnd();
    }

    void drawDashedLine() {
        float dim = isNight ? 0.8f : 1.0f;
        glColor3f(1.0f * dim, 1.0f * dim, 1.0f * dim);
        glLineWidth(2.0f);
        glEnable(GL_LINE_STIPPLE);
        glLineStipple(1, 0x00FF);
        glBegin(GL_LINES);
        glVertex2f(-2.0f, 0.5f);
        glVertex2f(2.0f, 0.5f);
        glEnd();
        glDisable(GL_LINE_STIPPLE);
    }

    void display() {
        if (isNight) {
            glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
        } else {
            glClearColor(0.5f, 0.5f, 0.3f, 0.0f);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        drawSky();
        drawSunRays();
        drawMountain(-1.6f, -0.4f, 3.0f);
        drawMountain(-1.3f, -0.4f, 3.0f);
        drawMountain(-1.0f, -0.3f, 2.5f);
        drawMountain(1.6f, -0.4f, 3.0f);
        drawMountain(1.3f, -0.4f, 3.0f);
        drawMountain(1.0f, -0.3f, 2.5f);
        drawHorizontalRoad();
        drawDashedLine();
        drawGravelBallast(20.0f,1.0f,0.6f);
        drawRiver();
        drawGrassPatch(1.5f, -0.7f, 1.0f, 30);
        drawRedCar(carX, 0.45f);
        drawBlueCar(hatchbackX, 0.45f);
        drawRealisticBus(whiteBusX, 0.45f);
        drawDarkBus(darkBusX, 0.45f);
        drawGrassWall();
        drawGrassPatch(-1.2f, -0.5f, 0.5f, 30);
        drawGrassPatch(-0.8f, -0.6f, 0.5f, 30);
        drawGrassPatch(-0.85f, -0.55f, 0.5f, 40);
        drawGrassPatch(-0.9f, -0.75f, 0.6f, 40);
        drawGrassPatch(-0.7f, -0.5f, 0.5f, 30);
        drawGrassPatch(1.2f, -0.85f, 1.2f, 40);
        tunnel();
        drawRealisticTree(-1.0f, -0.2f, 0.8f);
        drawStylizedTree(1.3f, -0.4f, 0.65f);
        drawRailsPerspective();
        drawFrontalTrain(0.35f,-0.5f,0.2f);
        drawTree_s1(-1.6f, -0.1f, 0.7f);
        drawTree_s1(-1.8f, -0.4f, 0.7f);
        drawTree_s1(-1.4f, -0.45f, 0.7f);
        drawRailwaySignal(0.8f, 0.3f);
        drawStone(-1.0f, -0.8f, 0.4f);
        drawStone(-1.0f, -0.8f, 0.3f);
        drawStone(-0.9f, -0.85f, 0.5f);
        drawGrassPatch(-1.1f, -0.85f, 0.8f, 40);
        drawStone(-0.7f, -0.85f, 0.2f);
        drawStone(-0.9f, -0.9f, 0.25f);
        drawGrassPatch(-0.9f, -0.75f, 0.8f, 40);
        drawStone(-0.8f, -0.9f, 0.25f);
        drawStone(-1.1f, -0.9f, 0.25f);
        drawStone(-1.2f, -0.85f, 0.5f);
        drawStone(-0.9f, -0.9f, 0.3f);
        drawStone(-0.8f, -0.75f, 0.4f);
        drawGrassPatch(-0.8f, -0.85f, 0.7f, 40);
        drawStone(-0.9f, -0.7f, 0.2f);
        drawStone(-0.6f, -0.8f, 0.25f);
        drawStone(1.5f, -0.8f, 0.4f);
        drawGrassPatch(1.5f, -0.85f, 1.15f, 40);
        drawStone(1.5f, -0.8f, 0.3f);
        drawStone(1.4f, -0.85f, 0.5f);
        drawStone(1.2f, -0.85f, 0.2f);
        drawStone(1.4f, -0.9f, 0.25f);
        drawStone(1.3f, -0.9f, 0.25f);
        drawStone(1.6f, -0.9f, 0.25f);
        drawStone(1.7f, -0.85f, 0.5f);
        drawRain();
    }

    void update(int value) {
        cloudX += cloudSpeed;
        if (cloudX > 2.5f) { cloudX = -2.5f; }
        carX -= carSpeed;
        if (carX < -2.5f) { carX = 2.5f; }
        hatchbackX -= hatchbackSpeed;
        if (hatchbackX < -2.5f) { hatchbackX = 2.5f; }
        whiteBusX += whiteBusSpeed;
        if (whiteBusX > 2.5f) { whiteBusX = -2.5f; }
        darkBusX -= darkBusSpeed;
        if (darkBusX < -2.5f) { darkBusX = 2.5f; }
        if (isTrainMoving) {
            frontalTrainSize += frontalTrainSpeed;
            if (frontalTrainSize > 1.0f) { frontalTrainSize = 0.1f; }
        }
        if (isRaining) {
            default_random_engine gen;
            uniform_real_distribution<float> distX(-2.0f, 2.0f);
            for (auto& drop : raindrops) {
                drop.y -= drop.speed;
                if (drop.y < -1.1f) { drop.y = 1.5f; }
            }
        }
        lightIntensity = 0.75f + 0.25f * sin(glutGet(GLUT_ELAPSED_TIME) * lightSpeed);
        float blink = sin(glutGet(GLUT_ELAPSED_TIME) * 0.01f) * 0.5f + 0.5f;
        if (frontalTrainSize < 0.5f) {
            signalLightRed = 0.8f * blink;
            signalLightGreen = 0.8f * blink;
            signalLightBlue = 0.0f;
        } else if (frontalTrainSize >= 0.5f && frontalTrainSize < 1.0f) {
            float transition = (frontalTrainSize - 0.4f) / 0.5f;
            signalLightRed = (0.8f * (1.0f - transition) + 1.0f * transition) * blink;
            signalLightGreen = (0.8f * (1.0f - transition) + 0.0f * transition) * blink;
            signalLightBlue = 0.0f;
        } else {
            signalLightRed = 1.0f * blink;
            signalLightGreen = 0.0f;
            signalLightBlue = 0.0f;
        }
        //glutPostRedisplay();
        //glutTimerFunc(20, update, 0);
    }


void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's': case 'S':
            // Toggle train movement on/off
            isTrainMoving = !isTrainMoving;
            #ifdef _WIN32
            if (isTrainMoving) {
                PlaySound(TEXT("whistle.wav"), NULL, SND_FILENAME | SND_ASYNC);
            } else {
                PlaySound(NULL, 0, 0);
            }
#endif
            break;

        case 'n': case 'N':
            isNight = true;
            break;

        case 'd': case 'D':
            isNight = false;
            break;

        case 'r': case 'R':
            isRaining = !isRaining;
            #ifdef _WIN32
            if (isRaining) {
                PlaySound(TEXT("rain.wav"), NULL, SND_FILENAME | SND_ASYNC);
            } else {
                PlaySound(NULL, 0, 0);
            }
#endif
            break;
    }
}

}

//.............................................
//..................Scene 2....................
//.............................................
namespace Scene2 {
    const int WINDOW_WIDTH = 1250;
    const int WINDOW_HEIGHT = 625;
    float celestial_angle = -0.2f;
    float sky_color_r = 0.1f, sky_color_g = 0.1f, sky_color_b = 0.2f;
    float cloud_x_offset = 0.0f;
    float bird_x_offset = 0.0f;
    bool isAnimating = true;
    bool isBird = true;

    bool isCar = true;
    bool isDayTime = false;
    const float night_celestial_angle = -0.2f;
    const float day_celestial_angle = M_PI / 2.0f;
    const float ANIMATION_SPEED = 0.0005f;
    const float CLOUD_SPEED = 0.0005f;
    const float BIRD_SPEED = 0.002f;
    const float MOUNTAIN_TOP_Y = 0.2f;
    const float CELESTIAL_RADIUS = 0.15f;
    const int NUM_STARS = 500;
    struct Star { float x, y, size, twinkle_phase; };
    vector<Star> stars;
    const int NUM_CARS = 4;
    const float CAR_SPEED = 0.001f;
    float zoom_level = 1.0f;
    vector<std::vector<float>> car_colors = {
        {0.8f, 0.1f, 0.1f}, {0.1f, 0.2f, 0.8f}, {0.1f, 0.8f, 0.1f},
        {0.8f, 0.8f, 0.1f}, {0.8f, 0.4f, 0.1f}, {0.6f, 0.1f, 0.8f}
    };
    struct Car {
        float y_position;
        bool is_going_down;
        int color_index;
        bool is_bus;
    };
    vector<Car> cars;
    struct Grass { float x, y, scale; };
    vector<Grass> grassPatches;
    mt19937 rng(std::random_device{}());

    void initStarS2() {
        uniform_real_distribution<float> x_dist(-2.0f, 2.0f);
        uniform_real_distribution<float> y_dist(0.2f, 1.0f);
        uniform_real_distribution<float> size_dist(1.0f, 3.0f);
        uniform_real_distribution<float> twinkle_dist(0.0f, 2 * M_PI);
        for (int i = 0; i < NUM_STARS; ++i) {
            stars.push_back({x_dist(rng), y_dist(rng), size_dist(rng), twinkle_dist(rng)});
        }
    }

    void drawFilledEllipse_helper(float xc, float yc, float xr, float yr) {
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * 3.14159 / 180.0;
            glVertex2f(xc + xr * cos(angle), yc + yr * sin(angle));
        }
        glEnd();
    }

    void drawFilledCircleS2_helper(float xc, float yc, float r) {
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * 3.14159 / 180.0;
            glVertex2f(xc + r * cos(angle), yc + r * sin(angle));
        }
        glEnd();
    }

    bool isNight(float sun_y) {
        return sun_y <= MOUNTAIN_TOP_Y;
    }

    void drawCarHeadlightBeam(float x_car, float y_car_front, float size) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 0.8f, 0.2f);
        glBegin(GL_POLYGON);
        glVertex2f(x_car - size * 0.4, y_car_front + size * 0.1);
        glVertex2f(x_car + size * 0.4, y_car_front + size * 0.1);
        float beam_length_factor = 0.5f;
        float beam_width_factor = 1.0f;
        float end_y = y_car_front - beam_length_factor;
        if (end_y < -1.0f) end_y = -1.0f;
        float current_road_width_at_end_y = 0.08f + (-end_y) * 1.8f;
        float current_car_x_offset_at_end_y = current_road_width_at_end_y * 0.25f;
        float projected_left_x = x_car - current_car_x_offset_at_end_y - (size * 0.4 * beam_width_factor);
        float projected_right_x = x_car + current_car_x_offset_at_end_y + (size * 0.4 * beam_width_factor);
        glVertex2f(projected_right_x, end_y);
        glVertex2f(projected_left_x, end_y);
        glEnd();
        glDisable(GL_BLEND);
    }

    void drawCarTopDown(float x_base, float y_base, float size, const std::vector<float>& color) {
        glColor3f(color[0], color[1], color[2]);
        glBegin(GL_POLYGON);
        glVertex2f(x_base - size * 0.2, y_base);
        glVertex2f(x_base + size * 0.2, y_base);
        glVertex2f(x_base + size * 0.2, y_base + size * 0.3);
        glVertex2f(x_base - size * 0.2, y_base + size * 0.3);
        glEnd();
        glColor3f(0.5f, 0.5f, 0.5f);
        glBegin(GL_POLYGON);
        glVertex2f(x_base - size * 0.23, y_base);
        glVertex2f(x_base + size * 0.23, y_base);
        glVertex2f(x_base + size * 0.23, y_base + size * 0.05);
        glVertex2f(x_base - size * 0.23, y_base + size * 0.05);
        glEnd();
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_POLYGON);
        glVertex2f(x_base - size * 0.15, y_base + size * 0.18);
        glVertex2f(x_base + size * 0.15, y_base + size * 0.18);
        glVertex2f(x_base + size * 0.15, y_base + size * 0.28);
        glVertex2f(x_base - size * 0.15, y_base + size * 0.28);
        glEnd();
        bool show_brake_lights = (static_cast<int>(glutGet(GLUT_ELAPSED_TIME) / 200) % 2 == 0);
        if (show_brake_lights) { glColor3f(1.0f, 0.0f, 0.0f); }
        else { glColor3f(0.7f, 0.0f, 0.0f); }
        glBegin(GL_POLYGON);
        glVertex2f(x_base - size * 0.21, y_base + size * 0.015);
        glVertex2f(x_base - size * 0.15, y_base + size * 0.015);
        glVertex2f(x_base - size * 0.15, y_base + size * 0.045);
        glVertex2f(x_base - size * 0.21, y_base + size * 0.045);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(x_base + size * 0.15, y_base + size * 0.015);
        glVertex2f(x_base + size * 0.21, y_base + size * 0.015);
        glVertex2f(x_base + size * 0.21, y_base + size * 0.045);
        glVertex2f(x_base + size * 0.15, y_base + size * 0.045);
        glEnd();
        glColor3f(1.0f, 0.6f, 0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(x_base - size * 0.14, y_base + size * 0.02);
        glVertex2f(x_base - size * 0.08, y_base + size * 0.02);
        glVertex2f(x_base - size * 0.08, y_base + size * 0.04);
        glVertex2f(x_base - size * 0.14, y_base + size * 0.04);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(x_base + size * 0.08, y_base + size * 0.02);
        glVertex2f(x_base + size * 0.14, y_base + size * 0.02);
        glVertex2f(x_base + size * 0.14, y_base + size * 0.04);
        glVertex2f(x_base + size * 0.08, y_base + size * 0.04);
        glEnd();
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_POLYGON);
        glVertex2f(x_base - size * 0.05, y_base + size * 0.015);
        glVertex2f(x_base + size * 0.05, y_base + size * 0.015);
        glVertex2f(x_base + size * 0.05, y_base + size * 0.035);
        glVertex2f(x_base - size * 0.05, y_base + size * 0.035);
        glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(x_base - size * 0.26, y_base + size * 0.07);
        glVertex2f(x_base - size * 0.20, y_base + size * 0.07);
        glVertex2f(x_base - size * 0.20, y_base + size * 0.13);
        glVertex2f(x_base - size * 0.26, y_base + size * 0.13);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(x_base + size * 0.20, y_base + size * 0.07);
        glVertex2f(x_base + size * 0.26, y_base + size * 0.07);
        glVertex2f(x_base + size * 0.26, y_base + size * 0.13);
        glVertex2f(x_base + size * 0.20, y_base + size * 0.13);
        glEnd();
        if (-y_base > 0.5) {
            glColor3f(0.1f, 0.1f, 0.1f);
            drawFilledEllipse_helper(x_base - size * 0.18, y_base - size * 0.01, size * 0.025, size * 0.01);
            drawFilledEllipse_helper(x_base + size * 0.18, y_base - size * 0.01, size * 0.025, size * 0.01);
        }
    }

    void drawVehicleFrontView(float x_base, float y_base, float size, const std::vector<float>& color, bool is_night, bool is_bus) {
        if (is_bus) {
            glColor3f(color[0], color[1], color[2]);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size * 0.65f, y_base);
            glVertex2f(x_base + size * 0.65f, y_base);
            glVertex2f(x_base + size * 0.60f, y_base + size * 0.9f);
            glVertex2f(x_base - size * 0.60f, y_base + size * 0.9f);
            glEnd();
            glColor3f(color[0] * 0.7f, color[1] * 0.7f, color[2] * 0.7f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size * 0.60f, y_base + size * 0.9f);
            glVertex2f(x_base + size * 0.60f, y_base + size * 0.9f);
            glVertex2f(x_base + size * 0.50f, y_base + size * 1.05f);
            glVertex2f(x_base - size * 0.50f, y_base + size * 1.05f);
            glEnd();
            glColor3f(color[0] * 0.6f, color[1] * 0.6f, color[2] * 0.6f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size * 0.50f, y_base + size * 1.05f);
            glVertex2f(x_base + size * 0.50f, y_base + size * 1.05f);
            glVertex2f(x_base + size * 0.50f, y_base + size * 0.7f);
            glVertex2f(x_base - size * 0.50f, y_base + size * 0.7f);
            glEnd();
            glColor3f(0.1f, 0.1f, 0.1f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size*0.2f, y_base + size*0.35f);
            glVertex2f(x_base + size*0.2f, y_base + size*0.35f);
            glVertex2f(x_base + size*0.18f, y_base + size*0.75f);
            glVertex2f(x_base - size*0.18f, y_base + size*0.75f);
            glEnd();
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size*0.45f, y_base + size*0.35f);
            glVertex2f(x_base - size*0.25f, y_base + size*0.35f);
            glVertex2f(x_base - size*0.22f, y_base + size*0.75f);
            glVertex2f(x_base - size*0.42f, y_base + size*0.75f);
            glEnd();
            glBegin(GL_POLYGON);
            glVertex2f(x_base + size*0.25f, y_base + size*0.35f);
            glVertex2f(x_base + size*0.45f, y_base + size*0.35f);
            glVertex2f(x_base + size*0.42f, y_base + size*0.75f);
            glVertex2f(x_base + size*0.22f, y_base + size*0.75f);
            glEnd();
            if (is_night) glColor3f(1.0f, 1.0f, 0.6f);
            else glColor3f(0.95f, 0.95f, 0.95f);
            drawFilledCircleS2_helper(x_base - size*0.45f, y_base + size*0.12f, size*0.1f);
            drawFilledCircleS2_helper(x_base + size*0.45f, y_base + size*0.12f, size*0.1f);
            glColor3f(0.0f, 0.0f, 0.0f);
            drawFilledEllipse_helper(x_base - size*0.5f, y_base, size*0.12f, size*0.06f);
            drawFilledEllipse_helper(x_base + size*0.5f, y_base, size*0.12f, size*0.06f);
            glColor3f(0.3f, 0.3f, 0.3f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size*0.65f, y_base);
            glVertex2f(x_base + size*0.65f, y_base);
            glVertex2f(x_base + size*0.55f, y_base + size*0.07f);
            glVertex2f(x_base - size*0.55f, y_base + size*0.07f);
            glEnd();
            glColor3f(0.95f, 0.9f, 0.1f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size*0.25f, y_base + size*0.80f);
            glVertex2f(x_base + size*0.25f, y_base + size*0.80f);
            glVertex2f(x_base + size*0.25f, y_base + size*0.88f);
            glVertex2f(x_base - size*0.25f, y_base + size*0.88f);
            glEnd();
        } else {
            glBegin(GL_POLYGON);
            glColor3f(color[0] * 1.1f, color[1] * 1.1f, color[2] * 1.1f);
            glVertex2f(x_base - size * 0.35f, y_base + size * 0.55f);
            glVertex2f(x_base + size * 0.35f, y_base + size * 0.55f);
            glColor3f(color[0] * 0.8f, color[1] * 0.8f, color[2] * 0.8f);
            glVertex2f(x_base + size * 0.55f, y_base);
            glVertex2f(x_base - size * 0.55f, y_base);
            glEnd();
            glColor3f(0.05f, 0.05f, 0.05f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size * 0.25f, y_base + size * 0.25f);
            glVertex2f(x_base + size * 0.25f, y_base + size * 0.25f);
            glVertex2f(x_base + size * 0.18f, y_base + size * 0.48f);
            glVertex2f(x_base - size * 0.18f, y_base + size * 0.48f);
            glEnd();
            glColor3f(0.0f, 0.0f, 0.0f);
            drawFilledEllipse_helper(x_base - size * 0.48f, y_base + size * 0.05f, size * 0.12f, size * 0.06f);
            drawFilledEllipse_helper(x_base + size * 0.48f, y_base + size * 0.05f, size * 0.12f, size * 0.06f);
            if (is_night) glColor3f(1.0f, 1.0f, 0.6f);
            else glColor3f(0.95f, 0.95f, 0.95f);
            drawFilledCircleS2_helper(x_base - size * 0.32f, y_base + size * 0.12f, size * 0.08f);
            drawFilledCircleS2_helper(x_base + size * 0.32f, y_base + size * 0.12f, size * 0.08f);
            glColor3f(0.2f, 0.2f, 0.2f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size * 0.22f, y_base + size * 0.05f);
            glVertex2f(x_base + size * 0.22f, y_base + size * 0.05f);
            glVertex2f(x_base + size * 0.22f, y_base + size * 0.18f);
            glVertex2f(x_base - size * 0.22f, y_base + size * 0.18f);
            glEnd();
            glColor3f(0.7f, 0.7f, 0.7f);
            glBegin(GL_LINES);
            for (float gx = -0.2f; gx <= 0.2f; gx += 0.05f) {
                glVertex2f(x_base + gx * size, y_base + size * 0.05f);
                glVertex2f(x_base + gx * size, y_base + size * 0.18f);
            }
            glEnd();
            glColor3f(0.3f, 0.3f, 0.3f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size * 0.55f, y_base);
            glVertex2f(x_base + size * 0.55f, y_base);
            glVertex2f(x_base + size * 0.45f, y_base + size * 0.05f);
            glVertex2f(x_base - size * 0.45f, y_base + size * 0.05f);
            glEnd();
            glColor3f(color[0] * 0.7f, color[1] * 0.7f, color[2] * 0.7f);
            drawFilledEllipse_helper(x_base - size * 0.58f, y_base + size * 0.35f, size * 0.06f, size * 0.025f);
            drawFilledEllipse_helper(x_base + size * 0.58f, y_base + size * 0.35f, size * 0.06f, size * 0.025f);
            glColor3f(0.9f, 0.9f, 0.9f);
            glBegin(GL_POLYGON);
            glVertex2f(x_base - size * 0.12f, y_base + size * 0.06f);
            glVertex2f(x_base + size * 0.12f, y_base + size * 0.06f);
            glVertex2f(x_base + size * 0.12f, y_base + size * 0.12f);
            glVertex2f(x_base - size * 0.12f, y_base + size * 0.12f);
            glEnd();
        }
    }

void drawTower(float x, float y, float scale) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(scale, scale, 1.0f);

    // Number of tower levels
    int levels = 8;
    float height = 1.8f;
    float baseWidth = 0.35f;
    float topWidth = 0.10f;

    // Draw lattice frame (vertical + diagonal supports)
    for (int i = 0; i < levels; i++) {
        float h1 = (i / (float)levels) * height;
        float h2 = ((i + 1) / (float)levels) * height;
        float w1 = baseWidth - (baseWidth - topWidth) * (i / (float)levels);
        float w2 = baseWidth - (baseWidth - topWidth) * ((i + 1) / (float)levels);

        // Alternate red & white paint for each section
        if (i % 2 == 0) glColor3f(0.85f, 0.0f, 0.0f); // red
        else glColor3f(0.9f, 0.9f, 0.9f);             // white

        glBegin(GL_LINES);
        // Four vertical legs (tapered)
        glVertex2f(-w1, h1); glVertex2f(-w2, h2);
        glVertex2f( w1, h1); glVertex2f( w2, h2);
        glVertex2f(0.0f, h1); glVertex2f(0.0f, h2); // center support
        glEnd();

        // Cross bracing
        glBegin(GL_LINES);
        glVertex2f(-w1, h1); glVertex2f(w2, h2);
        glVertex2f(w1, h1);  glVertex2f(-w2, h2);
        glEnd();

        // Horizontal rung
        glBegin(GL_LINES);
        glVertex2f(-w2, h2); glVertex2f(w2, h2);
        glEnd();
    }

    // Antenna panels (rectangular, near top)
    glColor3f(0.95f, 0.95f, 1.0f);
    for (int i = 0; i < 3; i++) {
        float angle = i * 120.0f * 3.14159f / 180.0f; // 3 sides
        float offsetX = 0.15f * cos(angle);
        float offsetY = 0.15f * sin(angle);
        glBegin(GL_POLYGON);
        glVertex2f(offsetX - 0.05f, height - 0.2f);
        glVertex2f(offsetX + 0.05f, height - 0.2f);
        glVertex2f(offsetX + 0.05f, height + 0.2f);
        glVertex2f(offsetX - 0.05f, height + 0.2f);
        glEnd();
    }

    // Dish antenna (microwave link)
    glColor3f(0.8f, 0.8f, 0.9f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.25f, height - 0.3f);
    for (int i = 0; i <= 360; i += 15) {
        float rad = i * 3.14159f / 180.0f;
        glVertex2f(0.25f + 0.08f * cos(rad), height - 0.3f + 0.08f * sin(rad));
    }
    glEnd();

    glPopMatrix();
}


    void drawCar(float x_base, float y_base, float size, const std::vector<float>& color, bool is_going_down, bool is_night, bool is_bus) {
        float sun_height_norm = (1.0f * sin(celestial_angle) - MOUNTAIN_TOP_Y) / (1.0f - MOUNTAIN_TOP_Y);
        if (sun_height_norm < 0.0f) sun_height_norm = 0.0f;
        if (sun_height_norm > 1.0f) sun_height_norm = 1.0f;
        float brightness_min = is_going_down ? 0.4f : 0.7f;
        float brightness = brightness_min + 0.6f * sun_height_norm;
        if (brightness > 1.0f) brightness = 1.0f;
        vector<float> adjusted_color = { color[0] * brightness, color[1] * brightness, color[2] * brightness };
        if (is_going_down) {
            drawVehicleFrontView(x_base, y_base, size, adjusted_color, is_night, is_bus);
        } else {
            drawCarTopDown(x_base, y_base, size, adjusted_color);
        }
    }

    void drawStarS2(float moon_y) {
        if (moon_y <= MOUNTAIN_TOP_Y) return;
        float star_alpha = (moon_y - MOUNTAIN_TOP_Y) / (1.0f - MOUNTAIN_TOP_Y);
        if (star_alpha < 0.0f) star_alpha = 0.0f;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBegin(GL_POINTS);
        for (const auto& star : stars) {
            glColor4f(1.0f, 1.0f, 1.0f, star_alpha);
            glPointSize(star.size);
            glVertex2f(star.x, star.y);
        }
        glEnd();
        glDisable(GL_BLEND);
    }

    void drawFilledCircleS2(float xc, float yc, float r) {
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 5) {
            float angle = i * 3.14159 / 180.0;
            glVertex2f(xc + r * cos(angle), yc + r * sin(angle));
        }
        glEnd();
    }

    void drawFilledSemiCircle(float xc, float yc, float r) {
        glBegin(GL_POLYGON);
        glVertex2f(xc - r, yc);
        glVertex2f(xc + r, yc);
        for (int i = 0; i <= 180; i += 5) {
            float angle = i * 3.14159 / 180.0;
            glVertex2f(xc + r * cos(angle), yc + r * sin(angle));
        }
        glEnd();
    }

    void drawCactus(float x, float y, float scale) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(0.0f, 0.6f, 0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(-0.05, 0.0);
        glVertex2f( 0.05, 0.0);
        glVertex2f( 0.05, -0.4);
        glVertex2f(-0.05, -0.4);
        glEnd();
        drawFilledSemiCircle(0.0, 0.0, 0.05);
        glBegin(GL_POLYGON);
        glVertex2f(-0.13, -0.13);
        glVertex2f(-0.05, -0.13);
        glVertex2f(-0.05, -0.23);
        glVertex2f(-0.13, -0.23);
        glEnd();
        drawFilledSemiCircle(-0.09, -0.13, 0.04);
        glBegin(GL_POLYGON);
        glVertex2f(0.05, -0.17);
        glVertex2f(0.13, -0.17);
        glVertex2f(0.13, -0.27);
        glVertex2f(0.05, -0.27);
        glEnd();
        drawFilledSemiCircle(0.09, -0.17, 0.04);
        glColor3f(0.9f, 0.9f, 0.9f);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (float i = 0.02f; i < 0.4f; i += 0.05f) {
            glVertex2f(-0.05f, -i);
            glVertex2f(-0.06f, -i - 0.01f);
            glVertex2f(0.05f, -i);
            glVertex2f(0.06f, -i - 0.01f);
        }
        glEnd();
        glBegin(GL_LINES);
        for (float i = -0.13f; i > -0.23f; i -= 0.04f) {
            glVertex2f(-0.09f, i);
            glVertex2f(-0.10f, i - 0.02f);
        }
        glEnd();
        glBegin(GL_LINES);
        for (float i = -0.17f; i > -0.27f; i -= 0.04f) {
            glVertex2f(0.09f, i);
            glVertex2f(0.10f, i - 0.02f);
        }
        glEnd();
        glColor3f(0.8f, 0.8f, 0.0f);
        glPointSize(1.5f);
        glBegin(GL_POINTS);
        for (float i = 0.03f; i < 0.38f; i += 0.05f) {
            for (float j = -0.04f; j < 0.05f; j += 0.04f) {
                glVertex2f(j, -i);
            }
        }
        glEnd();
        glPopMatrix();
    }


    void drawCloudS2(float x, float y, float sun_height_norm) {
        float r_day = 1.0f, g_day = 1.0f, b_day = 1.0f;
        float r_evening = 1.0f, g_evening = 0.7f, b_evening = 0.5f;
        float r_night = 0.65f, g_night = 0.65f, b_night = 0.75f;
        float r, g, b;
        if (sun_height_norm > 0.7f) {
            r = r_day; g = g_day; b = b_day;
        } else if (sun_height_norm > 0.0f) {
            float t = sun_height_norm / 0.7f;
            r = r_evening * (1 - t) + r_day * t;
            g = g_evening * (1 - t) + g_day * t;
            b = b_evening * (1 - t) + b_day * t;
        } else {
            r = r_night; g = r_night; b = b_night;
        }
        glColor3f(r, g, b);
        struct Circle { float dx, dy, radius; };
        vector<Circle> circles = {
            { -0.12f,  0.00f, 0.05f }, { -0.05f,  0.02f, 0.06f },
            {  0.00f,  0.00f, 0.07f }, {  0.07f,  0.01f, 0.06f },
            {  0.14f,  0.00f, 0.05f }, { -0.02f,  0.05f, 0.035f },
            {  0.06f,  0.04f, 0.035f }
        };
        for (auto& c : circles) {
            glBegin(GL_POLYGON);
            for (int i = 0; i < 360; i += 8) {
                float angle = i * 3.14159f / 180.0f;
                glVertex2f(x + c.dx + c.radius * cos(angle), y + c.dy + c.radius * sin(angle));
            }
            glEnd();
        }
    }

    void drawBird(float x, float y) {
        float scale = 0.04f + (y - MOUNTAIN_TOP_Y) * 0.02f;
        if (scale < 0.03f) scale = 0.03f;
        float t = glutGet(GLUT_ELAPSED_TIME) / 300.0f;
        float flap = sinf(t * 6.0f + x * 5.0f);
        float baseR = 0.0f, baseG = 0.0f, baseB = 0.0f;
        float wingR = 0.1f, wingG = 0.1f, wingB = 0.1f;
        float tailR = 0.05f, tailG = 0.05f, tailB = 0.05f;
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(baseR, baseG, baseB);
        glBegin(GL_POLYGON);
        for (int i = 0; i < 360; i += 8) {
            float angle = i * 3.14159f / 180.0f;
            glVertex2f(0.45f * cosf(angle), 0.25f * sinf(angle));
        }
        glEnd();
        glColor3f(tailR, tailG, tailB);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.45f, 0.0f); glVertex2f(-0.65f, -0.08f); glVertex2f(-0.65f,  0.08f);
        glVertex2f(-0.45f, 0.02f); glVertex2f(-0.6f,  0.12f); glVertex2f(-0.65f, 0.02f);
        glVertex2f(-0.45f, -0.02f); glVertex2f(-0.6f, -0.12f); glVertex2f(-0.65f, -0.02f);
        glEnd();
        glColor3f(wingR, wingG, wingB);
        glPushMatrix();
        glTranslatef(-0.05f, 0.05f, 0.0f);
        glRotatef(-flap * 35.0f - 12.0f, 0, 0, 1);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f); glVertex2f(-0.8f, 0.5f); glVertex2f(-0.2f, 0.25f);
        glEnd();
        glPopMatrix();
        glPushMatrix();
        glTranslatef(0.05f, 0.05f, 0.0f);
        glRotatef(flap * 35.0f + 12.0f, 0, 0, 1);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f); glVertex2f(0.8f, 0.5f); glVertex2f(0.2f, 0.25f);
        glEnd();
        glPopMatrix();
        glColor3f(0.9f, 0.6f, 0.1f);
        glBegin(GL_TRIANGLES);
        glVertex2f(0.45f, 0.0f);
        glVertex2f(0.70f, 0.08f);
        glVertex2f(0.70f, -0.08f);
        glEnd();
        glPopMatrix();
    }

    void drawSkyAndGround(float sun_height_norm) {
        glBegin(GL_QUADS);
        const float night_top_r = 0.02f, night_top_g = 0.02f, night_top_b = 0.08f;
        const float night_bottom_r = 0.1f, night_bottom_g = 0.1f, night_bottom_b = 0.2f;
        const float day_top_r = 0.2f, day_top_g = 0.5f, day_top_b = 0.9f;
        const float day_bottom_r = 0.9f, day_bottom_g = 0.8f, day_bottom_b = 0.6f;
        float top_r = night_top_r + (day_top_r - night_top_r) * sun_height_norm;
        float top_g = night_top_g + (day_top_g - night_top_g) * sun_height_norm;
        float top_b = night_top_b + (day_top_b - night_top_b) * sun_height_norm;
        float bottom_r = night_bottom_r + (day_bottom_r - night_bottom_r) * sun_height_norm;
        float bottom_g = night_bottom_g + (day_bottom_g - night_bottom_g) * sun_height_norm;
        float bottom_b = night_bottom_b + (day_bottom_b - night_bottom_b) * sun_height_norm;
        glColor3f(top_r, top_g, top_b);
        glVertex2f(-2.0f, 1.0f);
        glVertex2f( 2.0f, 1.0f);
        glColor3f(bottom_r, bottom_g, bottom_b);
        glVertex2f( 2.0f, 0.0f);
        glVertex2f(-2.0f, 0.0f);
        glEnd();
        const float day_ground_r = 0.95f, day_ground_g = 0.85f, day_ground_b = 0.55f;
        const float night_ground_r = 0.2f, night_ground_g = 0.18f, night_ground_b = 0.15f;
        float ground_top_r = night_ground_r + (day_ground_r - night_ground_r) * sun_height_norm;
        float ground_top_g = night_ground_g + (day_ground_g - night_ground_g) * sun_height_norm;
        float ground_top_b = night_ground_b + (day_ground_b - night_ground_b) * sun_height_norm;
        float ground_bottom_r = ground_top_r * 0.8f;
        float ground_bottom_g = ground_top_g * 0.8f;
        float ground_bottom_b = ground_top_b * 0.8f;
        int num_segments = 80;
        float start_y = 0.05f, end_y = -1.05f;
        for (int i = 0; i < num_segments; ++i) {
            float y1 = start_y + (end_y - start_y) * (float(i) / num_segments);
            float y2 = start_y + (end_y - start_y) * (float(i + 1) / num_segments);
            float t1 = (float)i / num_segments;
            float t2 = (float)(i + 1) / num_segments;
            float current_r1 = ground_top_r + (ground_bottom_r - ground_top_r) * t1;
            float current_g1 = ground_top_g + (ground_bottom_g - ground_top_g) * t1;
            float current_b1 = ground_top_b + (ground_bottom_b - ground_top_b) * t1;
            float current_r2 = ground_top_r + (ground_bottom_r - ground_top_r) * t2;
            float current_g2 = ground_top_g + (ground_bottom_g - ground_top_g) * t2;
            float current_b2 = ground_top_b + (ground_bottom_b - ground_top_b) * t2;
            glBegin(GL_QUAD_STRIP);
            for (float x = -2.0f; x <= 2.0f; x += 0.05f) {
                float wave1 = 0.03f * sinf(12.0f * x + i * 0.6f);
                float wave2 = 0.03f * sinf(12.0f * x + (i + 1) * 0.6f);
                float brightness1 = 0.7f + 0.2f * cosf(12.0f * x + i * 0.6f);
                float brightness2 = 0.7f + 0.2f * cosf(12.0f * x + (i + 1) * 0.6f);
                glColor3f(current_r1 * brightness1, current_g1 * brightness1, current_b1 * brightness1);
                glVertex2f(x, y1 + wave1);
                glColor3f(current_r2 * brightness2, current_g2 * brightness2, current_b2 * brightness2);
                glVertex2f(x, y2 + wave2);
            }
            glEnd();
        }
    }

    void drawCarBeams(float sun_y, float moon_y) {
        if (isNight(sun_y)) {
            for (const auto& car : cars) {
                float y_pos = car.y_position;
                if (car.is_going_down && y_pos < 0.0f && y_pos > -1.0f) {
                    float road_width_at_y = 0.08f + (-y_pos) * 1.8f;
                    float car_size = 0.1f + (-y_pos) * 0.3f;
                    float lane_offset = road_width_at_y * 0.25f;
                    float car_x = lane_offset;
                    drawCarHeadlightBeam(car_x, y_pos, car_size);
                }
            }
        }
    }

    void drawDeadGrass(float x, float y, float scale);

    void drawCelestialBodies(float sun_x, float sun_y, float moon_x, float moon_y) {
        if (sun_y > MOUNTAIN_TOP_Y) {
            float sun_height_norm = (sun_y - MOUNTAIN_TOP_Y) / (1.0f - MOUNTAIN_TOP_Y);
            if (sun_height_norm < 0.0f) sun_height_norm = 0.0f;
            if (sun_height_norm > 1.0f) sun_height_norm = 1.0f;
            float r_low = 1.0f, g_low = 0.4f, b_low = 0.0f;
            float r_high = 1.0f, g_high = 0.95f, b_high = 0.7f;
            float r = r_low + (r_high - r_low) * sun_height_norm;
            float g = g_low + (g_high - g_low) * sun_height_norm;
            float b = b_low + (b_high - b_low) * sun_height_norm;
            glColor3f(r, g, b);
            drawFilledCircleS2_helper(sun_x, sun_y, CELESTIAL_RADIUS);
        }
        if (moon_y > MOUNTAIN_TOP_Y) {
            glColor3f(0.95f, 0.95f, 0.9f);
            drawFilledCircleS2_helper(moon_x, moon_y, CELESTIAL_RADIUS);
            drawStarS2(moon_y);
        }
    }

    void drawMountainS2(float sun_height_norm) {
        const float mesa_day_r = 0.8f, mesa_day_g = 0.3f, mesa_day_b = 0.1f;
        const float mesa_night_r = 0.3f, mesa_night_g = 0.15f, mesa_night_b = 0.1f;
        float mesa_r = mesa_night_r + (mesa_day_r - mesa_night_r) * sun_height_norm;
        float mesa_g = mesa_night_g + (mesa_day_g - mesa_night_g) * sun_height_norm;
        float mesa_b = mesa_night_b + (mesa_day_b - mesa_night_b) * sun_height_norm;
        float shadow_r = mesa_r * 0.6f;
        float shadow_g = mesa_g * 0.6f;
        float shadow_b = mesa_b * 0.6f;
        glBegin(GL_POLYGON);
        glColor3f(mesa_r, mesa_g, mesa_b);
        glVertex2f(-2.25f, 0.0f);
        glVertex2f(-1.7f, 0.25f);
        glVertex2f(-1.2f, 0.1f);
        glVertex2f(-0.9f, 0.35f);
        glVertex2f(-0.5f, 0.3f);
        glVertex2f(0.0f, 0.45f);
        glVertex2f(0.5f, 0.35f);
        glVertex2f(0.9f, 0.45f);
        glVertex2f(1.3f, 0.25f);
        glVertex2f(1.7f, 0.3f);
        glVertex2f(2.1f, 0.0f);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(shadow_r, shadow_g, shadow_b);
        glVertex2f(-0.9f, 0.35f);
        glVertex2f(-0.7f, 0.0f);
        glVertex2f(-0.5f, 0.0f);
        glVertex2f(-0.5f, 0.3f);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(shadow_r, shadow_g, shadow_b);
        glVertex2f(0.5f, 0.35f);
        glVertex2f(0.7f, 0.0f);
        glVertex2f(0.9f, 0.0f);
        glVertex2f(0.9f, 0.45f);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(shadow_r, shadow_g, shadow_b);
        glVertex2f(0.0f, 0.45f);
        glVertex2f(0.4f, 0.35f);
        glVertex2f(0.4f, 0.0f);
        glVertex2f(0.1f, 0.0f);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(mesa_r * 0.8f, mesa_g * 0.8f, mesa_b * 0.8f);
        glVertex2f(-1.9f, 0.0f);
        glVertex2f(-1.7f, 0.15f);
        glVertex2f(-1.5f, 0.1f);
        glVertex2f(-1.3f, 0.0f);
        glEnd();
        glBegin(GL_POLYGON);
        glColor3f(mesa_r * 0.85f, mesa_g * 0.85f, mesa_b * 0.85f);
        glVertex2f(1.4f, 0.0f);
        glVertex2f(1.55f, 0.12f);
        glVertex2f(1.75f, 0.1f);
        glVertex2f(1.9f, 0.0f);
        glEnd();
    }

    void drawRoad() {
        glColor3f(0.2f, 0.2f, 0.2f);
        glBegin(GL_POLYGON);
        glVertex2f(-0.1, 0.0);
        glVertex2f(0.1, 0.0);
        glVertex2f(1.0, -1.0);
        glVertex2f(-1.0, -1.0);
        glEnd();
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_POLYGON);
        glVertex2f(-0.008, 0.0);
        glVertex2f(0.008, 0.0);
        glVertex2f(0.1, -1.0);
        glVertex2f(-0.1, -1.0);
        glEnd();
    }

    void drawAntenna(float x, float y, float scale) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(0.2f, 0.2f, 0.2f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, 0.5f);
        glEnd();
        glColor3f(0.9f, 0.9f, 0.9f);
        drawFilledCircleS2_helper(0.0f, 0.52f, 0.04f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.35f); glVertex2f(-0.15f, 0.45f);
        glVertex2f(0.0f, 0.35f); glVertex2f( 0.15f, 0.45f);
        glEnd();
        glPopMatrix();
    }

    void drawScenery() {
        drawCactus(1.334f, -0.167f, 0.6f);
        drawCactus(1.666f, -0.433f, 0.5f);
        drawCactus(-1.334f, -0.167f, 0.5f);
        drawCactus(-1.666f, -0.433f, 0.5f);
    }

    void drawAnimatedSkyObjects(float sun_y, float sun_height_norm) {
        drawCloudS2(0.666f + cloud_x_offset, 0.833f, sun_height_norm);
        drawCloudS2(1.334f + cloud_x_offset, 0.667f, sun_height_norm);
        drawCloudS2(-0.666f + cloud_x_offset, 0.6f, sun_height_norm);
        drawCloudS2(-0.066f + cloud_x_offset, 0.5f, sun_height_norm);
        if (sun_y > MOUNTAIN_TOP_Y) {
            drawBird(0.33f + bird_x_offset, 0.7f);
            drawBird(0.0f  + bird_x_offset, 0.6f);
            drawBird(-0.5f + bird_x_offset, 0.8f);
        }
    }

    void drawTraffic(bool is_night) {
        for (const auto& car : cars) {
            float y_pos = car.y_position;
            if (y_pos < 0.0f && y_pos > -1.0f) {
                float road_width_at_y = 0.08f + (-y_pos) * 1.8f;
                float car_size = 0.1f + (-y_pos) * 0.3f;
                float lane_offset = road_width_at_y * 0.25f;
                float car_x = car.is_going_down ? lane_offset : -lane_offset;
                drawCar(car_x, y_pos, car_size, car_colors[car.color_index], car.is_going_down, is_night, car.is_bus);
            }
        }
    }

    void drawHouse(float x, float y, float scale, float r, float g, float b, float r2, float g2, float b2, float r3, float g3, float b3) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(r, g, b);
        glBegin(GL_POLYGON);
        glVertex2f(-0.6f, 0.0f);
        glVertex2f(0.6f, 0.0f);
        glVertex2f(0.58f, 0.6f);
        glVertex2f(-0.58f, 0.6f);
        glEnd();
        glColor3f(r2, g2, b2);
        glBegin(GL_POLYGON);
        glVertex2f(0.6f, 0.0f);
        glVertex2f(0.9f, 0.1f);
        glVertex2f(0.88f, 0.55f);
        glVertex2f(0.58f, 0.6f);
        glEnd();
        glColor3f(r3, g3, b3);
        glBegin(GL_POLYGON);
        glVertex2f(-0.3f, 0.6f);
        glVertex2f(0.3f, 0.6f);
        glVertex2f(0.28f, 0.95f);
        glVertex2f(-0.28f, 0.95f);
        glEnd();
        glColor3f(0.75f, 0.55f, 0.35f);
        glBegin(GL_POLYGON);
        glVertex2f(-0.65f, 0.6f);
        glVertex2f(0.65f, 0.6f);
        glVertex2f(0.6f, 0.65f);
        glVertex2f(-0.6f, 0.65f);
        glEnd();
        glColor3f(0.3f, 0.2f, 0.1f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        for (float i = -0.55f; i <= 0.55f; i += 0.2f) {
            glVertex2f(i, 0.65f);
            glVertex2f(i, 0.7f);
        }
        glEnd();
        glColor3f(0.25f, 0.12f, 0.05f);
        glBegin(GL_POLYGON);
        glVertex2f(-0.15f, 0.0f);
        glVertex2f(0.1f, 0.0f);
        glVertex2f(0.1f, 0.35f);
        glVertex2f(-0.15f, 0.35f);
        glEnd();
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.15f, 0.0f);
        glVertex2f(0.1f, 0.0f);
        glVertex2f(0.1f, 0.35f);
        glVertex2f(-0.15f, 0.35f);
        glEnd();
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_POLYGON);
        glVertex2f(-0.45f, 0.25f);
        glVertex2f(-0.3f, 0.25f);
        glVertex2f(-0.3f, 0.4f);
        glVertex2f(-0.45f, 0.4f);
        glEnd();
        glBegin(GL_POLYGON);
        glVertex2f(0.25f, 0.28f);
        glVertex2f(0.4f, 0.28f);
        glVertex2f(0.4f, 0.42f);
        glVertex2f(0.25f, 0.42f);
        glEnd();
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_LINES);
        glVertex2f(-0.375f, 0.25f); glVertex2f(-0.375f, 0.4f);
        glVertex2f(-0.45f, 0.325f); glVertex2f(-0.3f, 0.325f);
        glVertex2f(0.325f, 0.28f); glVertex2f(0.325f, 0.42f);
        glVertex2f(0.25f, 0.35f);  glVertex2f(0.4f, 0.35f);
        glEnd();
        glPointSize(2.0f);
        glColor3f(0.55f, 0.35f, 0.25f);
        glBegin(GL_POINTS);
        for (float i = -0.55f; i < 0.55f; i += 0.08f) {
            for (float j = 0.05f; j < 0.55f; j += 0.1f) {
                glVertex2f(i, j);
            }
        }
        glEnd();
        glPopMatrix();
    }

    void drawDeadGrass(float x, float y, float scale) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(0.2f, 0.2f, 0.2f);
        glLineWidth(1.1f);
        glBegin(GL_LINES);
        for (float i = -0.05f; i <= 0.05f; i += 0.02f) {
            glVertex2f(0.0f, 0.0f);
            glVertex2f(i, 0.15f);
        }
        glEnd();
        glPopMatrix();
    }

    void drawBush(float x, float y, float scale) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(0.3f, 0.15f, 0.05f);
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f); glVertex2f(0.0f, 0.2f);
        glVertex2f(0.0f, 0.1f); glVertex2f(-0.1f, 0.25f);
        glVertex2f(0.0f, 0.15f); glVertex2f( 0.1f, 0.3f);
        glVertex2f(0.0f, 0.05f); glVertex2f(-0.08f, 0.15f);
        glVertex2f(0.0f, 0.05f); glVertex2f( 0.08f, 0.18f);
        glEnd();
        glPopMatrix();
    }

    void drawGrass() {
        for (const auto& grass : grassPatches) {
            drawDeadGrass(grass.x, grass.y, grass.scale);
        }
    }

    void drawDeadTree(float x, float y, float scale) {
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glScalef(scale, scale, 1.0f);
        glColor3f(0.4f, 0.25f, 0.1f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(0.0f, 0.4f);
        glEnd();
        glLineWidth(2.0f);
        glBegin(GL_LINES);
        glVertex2f(0.0f, 0.3f); glVertex2f(-0.15f, 0.45f);
        glVertex2f(0.0f, 0.35f); glVertex2f(0.15f, 0.5f);
        glVertex2f(0.0f, 0.25f); glVertex2f(-0.12f, 0.35f);
        glVertex2f(0.0f, 0.2f);  glVertex2f(0.12f, 0.32f);
        glEnd();
        glPopMatrix();
    }

    void initGrass() {
        uniform_real_distribution<float> y_dist(-0.9f, -0.05f);
        uniform_real_distribution<float> scale_dist(0.1f, 0.3f);
        for (int i = 0; i < 40; ++i) {
            float y_pos = y_dist(rng);
            float road_width_at_y = 0.08f + (-y_pos) * 1.8f;
            uniform_real_distribution<float> x_dist_left(-2.0f, -road_width_at_y / 2.0f - 0.1f);
            uniform_real_distribution<float> x_dist_right(road_width_at_y / 2.0f + 0.1f, 2.0f);
            grassPatches.push_back({x_dist_left(rng), y_pos, scale_dist(rng)});
            grassPatches.push_back({x_dist_right(rng), y_pos, scale_dist(rng)});
        }
    }

    void display() {
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(-2.0 * zoom_level, 2.0 * zoom_level, -1.0 * zoom_level, 1.0 * zoom_level);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        float sun_x = -1.5f + (celestial_angle / (2 * M_PI)) * 3.0f;
        float sun_y = 1.0f * sin(celestial_angle);
        float moon_x = 1.5f - (celestial_angle / (2 * M_PI)) * 3.0f;
        float moon_y = 1.0f * sin(celestial_angle + M_PI);
        bool night_time = isNight(sun_y);
        float sun_height_norm = (sun_y - MOUNTAIN_TOP_Y) / (1.0f - MOUNTAIN_TOP_Y);
        if (sun_height_norm > 1.0f) sun_height_norm = 1.0f;
        if (sun_height_norm < 0.0f) sun_height_norm = 0.0f;
        drawSkyAndGround(sun_height_norm);
        drawCarBeams(sun_y, moon_y);
        drawCelestialBodies(sun_x, sun_y, moon_x, moon_y);
        drawAnimatedSkyObjects(sun_y, sun_height_norm);
        drawRoad();
        drawTraffic(night_time);
        drawMountainS2(sun_height_norm);
        drawTower(-0.52f, 0.3f, 0.2f);
        drawTower(1.35f, 0.25f, 0.1f);
        drawGrass();
        drawScenery();
        drawHouse(1.6f, -0.2f, 0.4f, 0.55f, 0.27f, 0.07f, 0.45f, 0.22f, 0.05f, 0.60f, 0.30f, 0.10f);
        drawDeadTree(1.8f, -0.2f, 1.0f);
        drawDeadGrass(1.55f, -0.28f, 0.2f);
        drawDeadGrass(1.65f, -0.3f, 0.15f);
        drawHouse(-1.7f, -0.1f, 0.4f, 0.83f, 0.62f, 0.42f, 0.65f, 0.45f, 0.28f, 0.85f, 0.65f, 0.45f);
        drawDeadTree(-1.9f, -0.1f, 1.0f);
        drawDeadGrass(-1.75f, -0.18f, 0.15f);
        drawAntenna(-1.7f, -0.1f + 0.35f, 0.25f);
        drawAntenna(1.6f, -0.2f + 0.35f, 0.25f);
        drawBush(-1.0f, -0.35f, 0.3f);
        drawBush(0.5f, -0.4f, 0.25f);
        drawBush(1.2f, -0.3f, 0.35f);
    }

    void update(int value) {
        if (isAnimating) {
            celestial_angle += ANIMATION_SPEED;
            if (celestial_angle > 2 * M_PI) { celestial_angle = 0.0f; }
            cloud_x_offset += CLOUD_SPEED;
            if (cloud_x_offset > 2.0f) { cloud_x_offset = -2.0f; }
            bird_x_offset += BIRD_SPEED;
            if (bird_x_offset > 2.0f) { bird_x_offset = -2.0f; }
            for (auto& car : cars) {
                if (car.is_going_down) {
                    car.y_position -= CAR_SPEED;
                    if (car.y_position < -1.0f) { car.y_position = 0.0f; }
                } else {
                    car.y_position += CAR_SPEED;
                    if (car.y_position > 0.0f) { car.y_position = -1.0f; }
                }
            }

        }
                   if(isBird){};
                   if(isCar){};


        float sun_height_norm = (1.0f * sin(celestial_angle) - MOUNTAIN_TOP_Y) / (1.0f - MOUNTAIN_TOP_Y);
        if (sun_height_norm > 1.0f) sun_height_norm = 1.0f;
        if (sun_height_norm < 0.0f) sun_height_norm = 0.0f;
        const float day_r = 1.0f, day_g = 0.8f, day_b = 0.4f;
        const float night_r = 0.1f, night_g = 0.1f, night_b = 0.2f;
        sky_color_r = night_r + (day_r - night_r) * sun_height_norm;
        sky_color_g = night_g + (day_g - night_g) * sun_height_norm;
        sky_color_b = night_b + (day_b - night_b) * sun_height_norm;
        //glutPostRedisplay();
        //glutTimerFunc(16, update, 0); // ~60 FPS
    }

    void init() {
        initStarS2();
        initGrass();
        cars.resize(NUM_CARS);
        uniform_real_distribution<float> down_dist(0.0f, -1.0f);
        uniform_real_distribution<float> up_dist(-1.0f, 0.0f);
        uniform_int_distribution<int> color_dist(0, car_colors.size() - 1);
        uniform_int_distribution<int> bus_dist(0, 4);
        for (int i = 0; i < NUM_CARS; ++i) {
            bool going_down = (i % 2 == 0);
            float y_pos = going_down ? down_dist(rng) : up_dist(rng);
            bool is_bus = (bus_dist(rng) == 0);
            cars[i] = {y_pos, going_down, color_dist(rng), is_bus};
        }
    }

    void keyboard(unsigned char key, int x, int y) {
        switch (key) {
            case 'p': case 'P': isAnimating = !isAnimating; break;// Changed from 's' to 'p' to avoid conflict

            case 'b': case 'B': isBird = !isBird;

            #ifdef _WIN32
            if (isBird) {
                PlaySound(TEXT("birds.wav"), NULL, SND_FILENAME | SND_ASYNC);
            } else {
                PlaySound(NULL, 0, 0);
            }
#endif
             break;

                         case 'c': case 'C': isCar = !isCar;

            #ifdef _WIN32
            if (isBird) {
                PlaySound(TEXT("car.wav"), NULL, SND_FILENAME | SND_ASYNC);
            } else {
                PlaySound(NULL, 0, 0);
            }
#endif
             break;

            //case 'z': case 'Z': zoom_level = max(0.2f, zoom_level - 0.1f); break;
            //case 'x': case 'X': zoom_level = min(2.0f, zoom_level + 0.1f); break;
            case 'd': case 'D': isAnimating = false;celestial_angle = day_celestial_angle;break;
            case 'n': case 'N': isAnimating = false; celestial_angle = night_celestial_angle; break;
        }
    }
}


// =======================================================================
// |                           SCENE 3                        |                           |
// =======================================================================
namespace Scene3 {
    float smallCloud1X = -1.4f, smallCloud1Y = 0.75f;
    float smallCloud2X = -0.5f, smallCloud2Y = 0.85f;
    float smallCloud3X = 0.6f,   smallCloud3Y = 0.70f;
    float smallCloud4X = 1.4f,   smallCloud4Y = 0.80f;
    float bigCloudX    = -0.4f, bigCloudY    = 0.65f;
    bool isNight = false;
    bool isBell = true;
    float liftOneX = -1.8f;
    float liftOneSpeed = 0.005f;
    float liftOneY = 0.60f;
    float liftTwoX = 1.8f;
    float liftTwoSpeed = -0.005f;
    float liftTwoY = 0.55f;
    enum PenguinState { WALKING, JUMPING, VANISHED };
    struct Penguin {
        float x, y;
        PenguinState state;
        float jumpHeight;
        float jumpSpeed;
        bool jumpUp;
    };
    Penguin penguins[2];
    struct SmokeParticle { float x, y, size, speedY, speedX, alpha; };
    const int NUM_SMOKE = 5;
    SmokeParticle house01Smoke[NUM_SMOKE], house02Smoke[NUM_SMOKE], house03Smoke[NUM_SMOKE], house04Smoke[NUM_SMOKE];
    float fishAngle = 0.0f;
    float tailWiggle = 0.0f;
    bool tailDirection = true;
    float redCarX = -2.0f, yellowCarX = -1.5f, whiteCarX = 2.0f, blueCarX = 1.5f;
    float busX = -2.2f, busSpeed = 0.003f;
    float purpleBusX = 2.2f, purpleBusSpeed = -0.003f;
    enum TrafficLightState { RED, YELLOW, GREEN };
    TrafficLightState trafficLight = GREEN;
    bool yellowToGreenPending = false;
    int yellowToGreenTimer = 0;
    const int yellowToGreenDuration = 500;
    const int NUM_FLAKES = 500;
    struct Snowflake { float x, y, size, speed; };
    Snowflake snowflakes[NUM_FLAKES];
    float textPosX = -0.1f;
    float textSpeed = 0.003f;
    const char *billboardText = "S  N  O  W     L  A  N  D  S  C  A  P  E";

    void initSnowflakes() {
        for (int i = 0; i < NUM_FLAKES; ++i) {
            snowflakes[i].x = -2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(4.0f)));
            snowflakes[i].y = 1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/1.0f));
            snowflakes[i].size = 0.005f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/0.01f));
            snowflakes[i].speed = 0.002f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/0.004f));
        }
    }

    void initSmoke(SmokeParticle smoke[], float baseX, float baseY, float scale) {
        for (int i = 0; i < NUM_SMOKE; ++i) {
            smoke[i].x = baseX + ((rand() % 100) / 5000.0f) - 0.01f;
            smoke[i].y = baseY + ((rand() % 100) / 5000.0f);
            smoke[i].size = 0.03f * scale + ((rand() % 10) / 500.0f);
            smoke[i].speedY = 0.001f + ((rand() % 10) / 10000.0f);
            smoke[i].speedX = ((rand() % 20) - 10) / 10000.0f;
            smoke[i].alpha = 0.7f + ((rand() % 30) / 100.0f);
        }
    }

  void drawFilledCircleS3(float cx, float cy, float r_x, float r_y, int num_segments, float red, float green, float blue, float alpha=1.0f) {
    glColor4f(red, green, blue, alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for(int i=0;i<=num_segments;i++){
        float theta = 2.0f * 3.1415926f * i / num_segments;
        float x = r_x * cosf(theta);
        float y = r_y * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

    void drawRectangle(float x1, float y1, float x2, float y2, float r, float g, float b){
        glColor3f(r,g,b);
        glBegin(GL_POLYGON);
        glVertex2f(x1,y1);
        glVertex2f(x2,y1);
        glVertex2f(x2,y2);
        glVertex2f(x1,y2);
        glEnd();
    }

    void drawCloud(float x, float y, float scale = 1.0f){
        float widen = 1.5f;
        if (isNight) {
            drawFilledCircleS3(x, y, 0.04f*scale*widen,0.04f*scale,100,0.6f,0.6f,0.7f,0.7f);
            drawFilledCircleS3(x + 0.05f*scale*widen, y+0.02f*scale,0.05f*scale*widen,0.05f*scale,100,0.6f,0.6f,0.7f,0.7f);
            drawFilledCircleS3(x + 0.1f*scale*widen, y,0.04f*scale*widen,0.04f*scale,100,0.6f,0.6f,0.7f,0.7f);
            drawFilledCircleS3(x + 0.03f*scale*widen, y-0.024f*scale,0.036f*scale*widen,0.036f*scale,100,0.6f,0.6f,0.7f,0.7f);
            drawFilledCircleS3(x + 0.08f*scale*widen, y-0.026f*scale,0.036f*scale*widen,0.036f*scale,100,0.6f,0.6f,0.7f,0.7f);
            drawFilledCircleS3(x + 0.11f*scale*widen, y+0.006f*scale,0.02f*scale*widen,0.02f*scale,100,0.5f,0.5f,0.6f,0.5f);
            drawFilledCircleS3(x + 0.09f*scale*widen, y-0.02f*scale,0.018f*scale*widen,0.018f*scale,100,0.5f,0.5f,0.6f,0.5f);
        } else {
            drawFilledCircleS3(x, y, 0.04f*scale*widen,0.04f*scale,100,1,1,1);
            drawFilledCircleS3(x + 0.05f*scale*widen, y+0.02f*scale,0.05f*scale*widen,0.05f*scale,100,1,1,1);
            drawFilledCircleS3(x + 0.1f*scale*widen, y,0.04f*scale*widen,0.04f*scale,100,1,1,1);
            drawFilledCircleS3(x + 0.03f*scale*widen, y-0.024f*scale,0.036f*scale*widen,0.036f*scale,100,1,1,1);
            drawFilledCircleS3(x + 0.08f*scale*widen, y-0.026f*scale,0.036f*scale*widen,0.036f*scale,100,1,1,1);
            drawFilledCircleS3(x + 0.11f*scale*widen, y+0.006f*scale,0.02f*scale*widen,0.02f*scale,100,0.95f,0.95f,0.95f);
            drawFilledCircleS3(x + 0.09f*scale*widen, y-0.02f*scale,0.018f*scale*widen,0.018f*scale,100,0.95f,0.95f,0.95f);
        }
    }

    void drawFish(float tailAngle){
        drawFilledCircleS3(0,0,0.025f,0.012f,50,1.0f,0.5f,0.0f);
        float tailLength = 0.015f, tailHeight = 0.008f;
        float tailOffsetX = -0.025f;
        float tailH = tailHeight * sinf(tailAngle);
        glColor3f(1.0f,0.5f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(tailOffsetX,0);
        glVertex2f(tailOffsetX-tailLength, tailHeight + tailH);
        glVertex2f(tailOffsetX-tailLength, -tailHeight - tailH);
        glEnd();
        glColor3f(0.9f,0.4f,0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(-0.004f,0.012f);
        glVertex2f(0.004f,0.012f);
        glVertex2f(0.0f,0.024f);
        glEnd();
        drawFilledCircleS3(0.012f,0.005f,0.002f,0.002f,20,0,0,0);
    }

    void drawCrystalLake(float x, float y){
        drawFilledCircleS3(x, y + 0.07f, 0.55f, 0.28f, 100, 0.95f, 0.97f, 1.0f);
        drawFilledCircleS3(x, y + 0.07f, 0.47f, 0.23f, 100, 0.1f, 0.5f, 0.7f);
        drawFilledCircleS3(x, y + 0.07f, 0.37f, 0.18f, 100, 0.4f, 0.75f, 0.9f);
        float highlightX = x + 0.27f;
        float highlightY = y + 0.14f;
        drawFilledCircleS3(highlightX,highlightY,0.16f,0.08f,100,0.8f,0.95f,1.0f);
        drawFilledCircleS3(highlightX-0.05f,highlightY+0.02f,0.03f,0.015f,100,0.95f,0.97f,1.0f);
        drawFilledCircleS3(highlightX+0.04f,highlightY+0.01f,0.025f,0.012f,100,0.95f,0.97f,1.0f);
        drawFilledCircleS3(highlightX-0.01f,highlightY-0.015f,0.02f,0.01f,100,0.95f,0.97f,1.0f);
        drawFilledCircleS3(highlightX+0.06f,highlightY-0.02f,0.018f,0.009f,100,0.95f,0.97f,1.0f);
        float a = 0.09f, b = 0.05f;
        float wave = 0.01f * sinf(fishAngle * 3);
        float fishX = highlightX + a * cosf(fishAngle);
        float fishY = highlightY + b * sinf(fishAngle) + wave;
        float delta = 0.001f;
        float nextX = highlightX + a * cosf(fishAngle + delta);
        float nextY = highlightY + b * sinf(fishAngle + delta) + 0.01f * sinf((fishAngle + delta) * 3);
        float angleRad = atan2f(nextY - fishY, nextX - fishX);
        float angleDeg = angleRad * 180.0f / 3.1415926f;
        glPushMatrix();
        glTranslatef(fishX, fishY, 0.0f);
        glRotatef(angleDeg,0,0,1);
        drawFish(tailWiggle);
        glPopMatrix();
    }

    void drawWinterSun(float cx, float cy, float r){
        drawFilledCircleS3(cx,cy,r,r,100,1.0f,1.0f,0.8f);
         drawFilledCircleS3(cx-0.05f, cy+0.02f,0.06f,0.03f,100,0.95f,0.95f,0.95f,0.5f);
        drawFilledCircleS3(cx+0.03f, cy-0.01f,0.05f,0.025f,100,0.96f,0.96f,0.96f,0.5f);
        drawFilledCircleS3(cx-0.02f, cy+0.01f,0.04f,0.02f,100,0.97f,0.97f,0.97f,0.5f);
    }

    void drawSnowdrift(float x, float y, float scale) {
        drawFilledCircleS3(x, y, 0.45f * scale, 0.12f * scale, 100, 0.95f, 0.97f, 1.0f);
        drawFilledCircleS3(x + 0.1f * scale, y - 0.015f * scale, 0.35f * scale, 0.09f * scale, 100, 0.97f, 0.97f, 1.0f);
        drawFilledCircleS3(x - 0.12f * scale, y - 0.01f * scale, 0.28f * scale, 0.08f * scale, 100, 1.0f, 1.0f, 1.0f);
    }

    void drawSnowflake(float x, float y, float size) {
        if (isNight) {
            glColor4f(0.8f, 0.8f, 1.0f, 0.6f);
        } else {
            glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
        }
        glPointSize(size * 300.0f);
        glBegin(GL_POINTS);
        glVertex2f(x, y);
        glEnd();
    }

    void drawCarB(float x, float y, float r, float g, float b){
        float carWidth = 0.22f;
        float carHeight = 0.05f;
        float wheelRadius = 0.02f;
        drawRectangle(x - carWidth/2, y, x + carWidth/2, y + carHeight, r * 0.9, g * 0.9, b * 0.9);
        float cabinWidthBottom = carWidth * 0.7f;
        float cabinWidthTop = carWidth * 0.5f;
        float cabinHeight = carHeight * 0.7f;
        glColor3f(r, g, b);
        glBegin(GL_POLYGON);
        glVertex2f(x - cabinWidthBottom/2, y + carHeight);
        glVertex2f(x + cabinWidthBottom/2, y + carHeight);
        glVertex2f(x + cabinWidthTop/2, y + carHeight + cabinHeight);
        glVertex2f(x - cabinWidthTop/2, y + carHeight + cabinHeight);
        glEnd();
        float windowMargin = 0.01f;
        float windowHeight = cabinHeight * 0.7f;
        glColor4f(0.3f, 0.5f, 0.8f, 0.7f);
        glBegin(GL_POLYGON);
        glVertex2f(x - cabinWidthTop/2 + windowMargin, y + carHeight + cabinHeight - windowHeight);
        glVertex2f(x + cabinWidthTop/2 - windowMargin, y + carHeight + cabinHeight - windowHeight);
        glVertex2f(x + cabinWidthTop/2 - windowMargin, y + carHeight + cabinHeight - windowMargin);
        glVertex2f(x - cabinWidthTop/2 + windowMargin, y + carHeight + cabinHeight - windowMargin);
        glEnd();
        float sideWindowWidth = cabinWidthBottom * 0.3f;
        float sideWindowHeight = cabinHeight * 0.7f;
        drawRectangle(x - cabinWidthBottom/2 + windowMargin + 0.02f, y + carHeight + cabinHeight * 0.2f, x - cabinWidthBottom/2 + windowMargin + sideWindowWidth + 0.02f, y + carHeight + cabinHeight * 0.2f + sideWindowHeight, 0.5f, 0.8f, 1.0f);
        drawRectangle(x + cabinWidthBottom/2 - windowMargin - sideWindowWidth - 0.02f, y + carHeight + cabinHeight * 0.2f, x + cabinWidthBottom/2 - windowMargin - 0.02f, y + carHeight + cabinHeight * 0.2f + sideWindowHeight, 0.5f, 0.8f, 1.0f);
        drawFilledCircleS3(x - carWidth/2 + wheelRadius + 0.01f, y + wheelRadius/2, wheelRadius, wheelRadius, 50, 0.1f, 0.1f, 0.1f);
        drawFilledCircleS3(x + carWidth/2 - wheelRadius - 0.01f, y + wheelRadius/2, wheelRadius, wheelRadius, 50, 0.1f, 0.1f, 0.1f);
        drawFilledCircleS3(x - carWidth/2 + wheelRadius + 0.01f, y + wheelRadius/2, wheelRadius * 0.6f, wheelRadius * 0.6f, 30, 0.5f, 0.5f, 0.5f);
        drawFilledCircleS3(x + carWidth/2 - wheelRadius - 0.01f, y + wheelRadius/2, wheelRadius * 0.6f, wheelRadius * 0.6f, 30, 0.5f, 0.5f, 0.5f);
        drawFilledCircleS3(x + carWidth/2 - 0.005f, y + carHeight * 0.6f, 0.005f, 0.008f, 20, 1.0f, 1.0f, 0.0f);
        drawFilledCircleS3(x - carWidth/2 + 0.005f, y + carHeight * 0.6f, 0.005f, 0.008f, 20, 1.0f, 0.0f, 0.0f);
        float doorSeamX = x - carWidth/2 + carWidth * 0.5f;
        float doorSeamY_Top = y + carHeight;
        float doorSeamY_Bottom = y;
        drawRectangle(doorSeamX - 0.001f, doorSeamY_Bottom, doorSeamX + 0.001f, doorSeamY_Top, r * 0.3f, g * 0.3f, b * 0.3f);
        float handleWidth = 0.01f;
        float handleHeight = 0.003f;
        float handleY = y + carHeight * 0.5f;
        drawRectangle(doorSeamX - handleWidth - 0.01f, handleY, doorSeamX - 0.01f, handleY + handleHeight, 0.1f, 0.1f, 0.1f);
        drawRectangle(doorSeamX + 0.01f, handleY, doorSeamX + handleWidth + 0.01f, handleY + handleHeight, 0.1f, 0.1f, 0.1f);
    }

    void drawCarT(float x, float y, float r, float g, float b) {

    float carWidth = 0.22f;
    float carHeight = 0.05f;
    float wheelRadius = 0.02f;
    drawRectangle(x - carWidth/2, y, x + carWidth/2, y + carHeight, r * 0.9, g * 0.9, b * 0.9);
    float cabinWidthBottom = carWidth * 0.7f;
    float cabinWidthTop = carWidth * 0.5f;
    float cabinHeight = carHeight * 0.7f;

    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
        glVertex2f(x - cabinWidthBottom/2, y + carHeight);
        glVertex2f(x + cabinWidthBottom/2, y + carHeight);
        glVertex2f(x + cabinWidthTop/2, y + carHeight + cabinHeight);
        glVertex2f(x - cabinWidthTop/2, y + carHeight + cabinHeight);
    glEnd();
    float windowMargin = 0.01f;
    float windowHeight = cabinHeight * 0.7f;
    glColor4f(0.3f, 0.5f, 0.8f, 0.7f);
    glBegin(GL_POLYGON);
        glVertex2f(x - cabinWidthTop/2 + windowMargin, y + carHeight + cabinHeight - windowHeight);
        glVertex2f(x + cabinWidthTop/2 - windowMargin, y + carHeight + cabinHeight - windowHeight);
        glVertex2f(x + cabinWidthTop/2 - windowMargin, y + carHeight + cabinHeight - windowMargin);
        glVertex2f(x - cabinWidthTop/2 + windowMargin, y + carHeight + cabinHeight - windowMargin);
    glEnd();


    float sideWindowWidth = cabinWidthBottom * 0.3f;
    float sideWindowHeight = cabinHeight * 0.7f;
    drawRectangle(x - cabinWidthBottom/2 + windowMargin + 0.02f, y + carHeight + cabinHeight * 0.2f,
                  x - cabinWidthBottom/2 + windowMargin + sideWindowWidth + 0.02f, y + carHeight + cabinHeight * 0.2f + sideWindowHeight,
                 0.5f, 0.8f, 1.0f);
    drawRectangle(x + cabinWidthBottom/2 - windowMargin - sideWindowWidth - 0.02f, y + carHeight + cabinHeight * 0.2f,
                  x + cabinWidthBottom/2 - windowMargin - 0.02f, y + carHeight + cabinHeight * 0.2f + sideWindowHeight,
                 0.5f, 0.8f, 1.0f);
    drawFilledCircleS3(x - carWidth/2 + wheelRadius + 0.01f, y + wheelRadius/2, wheelRadius, wheelRadius, 50, 0.1f, 0.1f, 0.1f);
    drawFilledCircleS3(x + carWidth/2 - wheelRadius - 0.01f, y + wheelRadius/2, wheelRadius, wheelRadius, 50, 0.1f, 0.1f, 0.1f);
    drawFilledCircleS3(x - carWidth/2 + wheelRadius + 0.01f, y + wheelRadius/2, wheelRadius * 0.6f, wheelRadius * 0.6f, 30, 0.5f, 0.5f, 0.5f);
    drawFilledCircleS3(x + carWidth/2 - wheelRadius - 0.01f, y + wheelRadius/2, wheelRadius * 0.6f, wheelRadius * 0.6f, 30, 0.5f, 0.5f, 0.5f);
    drawFilledCircleS3(x + carWidth/2 - 0.005f, y + carHeight * 0.6f, 0.005f, 0.008f, 20, 1.0f, 0.0f, 0.0f);
    drawFilledCircleS3(x - carWidth/2 + 0.005f, y + carHeight * 0.6f, 0.005f, 0.008f, 20, 1.0f, 1.0f, 0.0f);

    float doorSeamX = x - carWidth/2 + carWidth * 0.5f;
    float doorSeamY_Top = y + carHeight;
    float doorSeamY_Bottom = y;

    drawRectangle(doorSeamX - 0.001f, doorSeamY_Bottom, doorSeamX + 0.001f, doorSeamY_Top, r * 0.3f, g * 0.3f, b * 0.3f);

    float handleWidth = 0.01f;
    float handleHeight = 0.003f;
    float handleY = y + carHeight * 0.5f;


    drawRectangle(doorSeamX - handleWidth - 0.01f, handleY, doorSeamX - 0.01f, handleY + handleHeight, 0.1f, 0.1f, 0.1f);

    drawRectangle(doorSeamX + 0.01f, handleY, doorSeamX + handleWidth + 0.01f, handleY + handleHeight, 0.1f, 0.1f, 0.1f);

    }
    void drawTree(float x, float y, float scale, float trunkR, float trunkG, float trunkB, float leavesR, float leavesG, float leavesB) {

    drawRectangle(x - 0.01f * scale, y,
                  x + 0.01f * scale, y + 0.06f * scale,
                  trunkR, trunkG, trunkB);


    float gR = 0.0f, gG = 0.5f, gB = 0.0f;


    drawFilledCircleS3(x, y + 0.08f * scale, 0.05f * scale, 0.03f * scale, 50, gR, gG, gB);
    drawFilledCircleS3(x - 0.03f * scale, y + 0.07f * scale, 0.04f * scale, 0.025f * scale, 50, gR, gG, gB);
    drawFilledCircleS3(x + 0.03f * scale, y + 0.07f * scale, 0.04f * scale, 0.025f * scale, 50, gR, gG, gB);


    drawFilledCircleS3(x, y + 0.12f * scale, 0.04f * scale, 0.025f * scale, 50, gR, gG, gB);
    drawFilledCircleS3(x - 0.02f * scale, y + 0.11f * scale, 0.03f * scale, 0.02f * scale, 50, gR, gG, gB);
    drawFilledCircleS3(x + 0.02f * scale, y + 0.11f * scale, 0.03f * scale, 0.02f * scale, 50, gR, gG, gB);


    drawFilledCircleS3(x, y + 0.15f * scale, 0.03f * scale, 0.02f * scale, 50, gR, gG, gB);


    drawFilledCircleS3(x, y + 0.15f * scale, 0.025f * scale, 0.015f * scale, 50, leavesR, leavesG, leavesB);
    drawFilledCircleS3(x - 0.02f * scale, y + 0.12f * scale, 0.02f * scale, 0.015f * scale, 50, leavesR, leavesG, leavesB);
    drawFilledCircleS3(x + 0.02f * scale, y + 0.12f * scale, 0.02f * scale, 0.015f * scale, 50, leavesR, leavesG, leavesB);
    drawFilledCircleS3(x, y + 0.08f * scale, 0.025f * scale, 0.015f * scale, 50, leavesR, leavesG, leavesB);
}

    void drawStreetLamp(float x, float y, float scale) {


    float poleWidth = 0.02f * scale;
    float poleHeight = 0.25f * scale;


    drawRectangle(x - 0.01f*scale, y, x + 0.01f*scale, y + poleHeight, 0.1f, 0.1f, 0.1f);


    float lampWidth = 0.04f * scale;
    float lampHeight = 0.025f * scale;
    drawRectangle(x - lampWidth/2, y + poleHeight, x + lampWidth/2, y + poleHeight + lampHeight, 0.1f, 0.1f, 0.1f);


    float innerMargin = 0.003f * scale;
    drawRectangle(
        x - lampWidth/2 + innerMargin,
        y + poleHeight + innerMargin,
        x + lampWidth/2 - innerMargin,
        y + poleHeight + lampHeight - innerMargin,
        1.0f, 1.0f, 0.6f
    );


    float triHeight = 0.01f * scale;
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - lampWidth/2 - 0.005f*scale, y + poleHeight + lampHeight);
        glVertex2f(x + lampWidth/2 + 0.005f*scale, y + poleHeight + lampHeight);
        glVertex2f(x, y + poleHeight + lampHeight + triHeight);
    glEnd();



    }
    void drawWoodenBench(float x, float y, float scale) {
    float seatWidth = 0.18f * scale;
    float seatDepth = 0.04f * scale;
    float plankHeight = 0.008f * scale;
    float plankGap = 0.004f * scale;

    float backrestHeight = 0.05f * scale;
    float backrestTilt = 0.03f * scale;

    float legWidth = 0.015f * scale;
    float legHeight = 0.045f * scale;


    for (int i = 0; i < 3; ++i) {
        drawRectangle(
            x - seatWidth / 2,
            y - i * (plankHeight + plankGap),
            x + seatWidth / 2,
            y - i * (plankHeight + plankGap) + plankHeight,
            0.55f, 0.27f, 0.07f
        );
    }

    for (int i = 0; i < 3; ++i) {
        drawRectangle(
            x - seatWidth / 2,
            y + plankHeight + i * (plankHeight + plankGap),
            x + seatWidth / 2,
            y + plankHeight + i * (plankHeight + plankGap) + plankHeight,
            0.55f, 0.27f, 0.07f
        );
    }


    drawRectangle(x - seatWidth / 2 + 0.01f, y, x - seatWidth / 2 + 0.01f + legWidth, y - legHeight, 0.1f, 0.1f, 0.1f);
    drawRectangle(x + seatWidth / 2 - 0.01f - legWidth, y, x + seatWidth / 2 - 0.01f, y - legHeight, 0.1f, 0.1f, 0.1f);


    drawRectangle(x - seatWidth / 2 + 0.01f, y - legHeight / 2, x + seatWidth / 2 - 0.01f, y - legHeight / 2 + 0.004f, 0.3f, 0.15f, 0.05f);




    }
    void drawWoodenFence(float startX, float startY, int numPosts, float scale) {
   float postWidth = 0.015f * scale;
    float postHeight = 0.06f * scale;
    float railHeight = 0.01f * scale;
    float gap = 0.05f * scale; // horizontal gap between posts

    // Draw vertical posts
    for(int i = 0; i < numPosts; i++){
        float x = startX + i * gap;
        drawRectangle(
            x - postWidth / 2,
            startY,
            x + postWidth / 2,
            startY + postHeight,
            0.55f, 0.27f, 0.07f  // wood brown color
        );
    }

    // Draw horizontal rails (2 rails connecting posts)
    float railY1 = startY + 0.02f * scale;
    float railY2 = startY + 0.04f * scale;

    drawRectangle(
        startX - postWidth/2,
        railY1,
        startX + (numPosts-1)*gap + postWidth/2,
        railY1 + railHeight,
        0.55f, 0.27f, 0.07f
    );

    drawRectangle(
        startX - postWidth/2,
        railY2,
        startX + (numPosts-1)*gap + postWidth/2,
        railY2 + railHeight,
        0.55f, 0.27f, 0.07f
    );

    }
    void updateBillboardText() {


         textPosX += textSpeed;

    // Get the length of the text in world coordinates
    float textWorldWidth = (float)glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)billboardText) / glutGet(GLUT_WINDOW_WIDTH) * 2.0f;

    // Reset position when the text scrolls completely past the right side of the board
    float boardWidth = 0.28f;
    if (textPosX > boardWidth / 2 + textWorldWidth) {
        textPosX = -boardWidth / 2 - textWorldWidth;
    }


    }
    void drawBillboard(float x, float y, float scale) {

        // 1. Posts (gray)
    float postWidth = 0.02f * scale;
    float postHeight = 0.15f * scale;
    drawRectangle(x - 0.08f, y, x - 0.08f + postWidth, y + postHeight, 0.0f, 0.5f, 0.0f);
    drawRectangle(x + 0.08f - postWidth, y, x + 0.08f, y + postHeight, 0.0f, 0.5f, 0.0f);

    // 2. Board (red) with black border, wider
    float boardWidth = 0.28f * scale;
    float boardHeight = 0.098f * scale;
    float boardX1 = x - boardWidth/2;
    float boardY1 = y + postHeight - 0.01f;
    float boardX2 = x + boardWidth/2;
    float boardY2 = y + postHeight + boardHeight - 0.01f;
    drawRectangle(boardX1, boardY1, boardX2, boardY2, 0.8f, 0.0f, 0.0f);

    // Black border
    float borderThickness = 0.005f * scale;
    drawRectangle(boardX1, boardY1, boardX2, boardY1 + borderThickness, 0,0,0); // Top
    drawRectangle(boardX1, boardY2 - borderThickness, boardX2, boardY2, 0,0,0); // Bottom
    drawRectangle(boardX1, boardY1, boardX1 + borderThickness, boardY2, 0,0,0); // Left
    drawRectangle(boardX2 - borderThickness, boardY1, boardX2, boardY2, 0,0,0); // Right

    // 3. Continuous scrolling text (white)

    // Get the board's horizontal boundaries in world coordinates
    float boardLeftEdge = boardX1 + borderThickness;
    float boardRightEdge = boardX2 - borderThickness;

    // Manually track the drawing position of the text
    float currentTextX = x - boardWidth/2 + textPosX;
    float textY = y + postHeight + boardHeight/2 - 0.01f;

    glColor3f(1.0f, 1.0f, 1.0f);

    // Loop through each character and draw it only if it's on the board
    for (const char* c = billboardText; *c != '\0'; c++) {
        float charWidth = glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, *c);
        float normalizedCharWidth = charWidth / glutGet(GLUT_WINDOW_WIDTH) * 2.0f;

        // Only draw the character if its position is within the board's visible area
        if (currentTextX >= boardLeftEdge && currentTextX <= boardRightEdge) {
            glRasterPos2f(currentTextX, textY);
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
        }

        // Advance the text position for the next character
        currentTextX += normalizedCharWidth;
    }
}
 void drawRealisticBus1(float x, float y, float scale = 1.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f) {

     float busLength = 0.4f * scale;
    float busHeight = 0.12f * scale;
    float busBottom = y;
    float busTop = y + busHeight;
    float roofHeight = 0.02f * scale;

// Bus body
     drawRectangle(x - busLength/2, busBottom, x + busLength/2, busTop, r, g, b);
     drawRectangle(x - busLength/2, busTop, x + busLength/2, busTop + roofHeight, r, g, b);

// Rounded roof
     drawFilledCircleS3(x - busLength/2 + 0.01f, busTop + roofHeight/2, 0.01f, roofHeight/2, 30, r, g, b);
     drawFilledCircleS3(x + busLength/2 - 0.01f, busTop + roofHeight/2, 0.01f, roofHeight/2, 30, r, g, b);

// Windows

     int numWindows = 5;
     float winWidth = 0.055f * scale;
     float winHeight = 0.045f * scale;
     float gap = 0.008f * scale;
     for(int i = 0; i < numWindows; i++){

            float winX1 = x - busLength/2 + 0.02f + i*(winWidth + gap);
            float winX2 = winX1 + winWidth;
            float winY1 = busBottom + 0.055f*scale;
            float winY2 = winY1 + winHeight;

     drawRectangle(winX1, winY1, winX2, winY2, 0.5f, 0.8f, 1.0f);

}

// Door
    float doorWidth = 0.03f * scale;
    float doorHeight = 0.085f * scale;
    drawRectangle(x + busLength/2 - doorWidth - 0.005f, busBottom, x + busLength/2 - 0.005f, busBottom + doorHeight, 0.2f, 0.2f, 0.2f);

// Front bumper
    drawRectangle(x + busLength/2, busBottom + 0.01f*scale, x + busLength/2 + 0.008f*scale, busBottom + 0.04f*scale, 0.3f, 0.3f, 0.3f);

// Headlights
   drawFilledCircleS3(x + busLength/2 + 0.004f*scale, busBottom + 0.025f*scale, 0.005f*scale, 0.005f*scale, 30, 1.0f, 1.0f, 0.0f);
    drawFilledCircleS3(x + busLength/2 + 0.004f*scale, busBottom + 0.055f*scale, 0.005f*scale, 0.005f*scale, 30, 1.0f, 1.0f, 0.0f);
    float wheelRadius = 0.02f * scale;
    drawFilledCircleS3(x - busLength/3, busBottom - wheelRadius/2, wheelRadius, wheelRadius, 50, 0, 0, 0);
    drawFilledCircleS3(x + busLength/3, busBottom - wheelRadius/2, wheelRadius, wheelRadius, 50, 0, 0, 0);
    drawFilledCircleS3(x - busLength/3, busBottom - wheelRadius/2, wheelRadius*0.5f, wheelRadius*0.5f, 50, 0.7f, 0.7f, 0.7f);
    drawFilledCircleS3(x + busLength/3, busBottom - wheelRadius/2, wheelRadius*0.5f, wheelRadius*0.5f, 50, 0.7f, 0.7f, 0.7f);

//backLight (red)
    drawRectangle(x - busLength/2 + 0.01f*scale, busTop - 0.02f*scale, x - busLength/2 - 0.006f*scale, busTop, 1.0f, 0.0f, 0.0f );

}

void drawRealisticBus2(float x, float y, float scale = 1.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f) {
    float busLength = 0.4f * scale;
    float busHeight = 0.12f * scale;
    float busBottom = y;
    float busTop = y + busHeight;
    float roofHeight = 0.02f * scale;

    // Bus body
    drawRectangle(x - busLength/2, busBottom, x + busLength/2, busTop, r, g, b);
    drawRectangle(x - busLength/2, busTop, x + busLength/2, busTop + roofHeight, r, g, b);

    // Rounded roof
    drawFilledCircleS3(x - busLength/2 + 0.01f, busTop + roofHeight/2, 0.01f, roofHeight/2, 30, r, g, b);
    drawFilledCircleS3(x + busLength/2 - 0.01f, busTop + roofHeight/2, 0.01f, roofHeight/2, 30, r, g, b);

    // Back-side door (toward the rear)
float doorWidth = 0.03f * scale;
float doorHeight = 0.085f * scale;
float doorX1 = x - busLength/2 + 0.008f*scale; // left edge of door
float doorX2 = doorX1 + doorWidth;            // right edge of door
drawRectangle(doorX1, busBottom, doorX2, busBottom + doorHeight, 0.2f, 0.2f, 0.2f);

// Back-side windows (start after door with extra gap)
int numWindows = 5;
float winWidth = 0.055f * scale;
float winHeight = 0.045f * scale;
float gap = 0.008f * scale;
float extraGap = 0.01f * scale; // additional space between door and first window

// Start windows after door with extra gap
float startX = doorX2 + gap + extraGap;
for(int i = 0; i < numWindows; i++){
    float winX1 = startX + i*(winWidth + gap);
    float winX2 = winX1 + winWidth;
    float winY1 = busBottom + 0.055f*scale;
    float winY2 = winY1 + winHeight;
    drawRectangle(winX1, winY1, winX2, winY2, 0.5f, 0.8f, 1.0f);
}

   // Back bumper
    drawRectangle(x - busLength/2 - 0.008f*scale, busBottom + 0.01f*scale, x - busLength/2, busBottom + 0.04f*scale, 0.3f, 0.3f, 0.3f);

// Tail lights
    drawFilledCircleS3(x - busLength/2 - 0.004f*scale, busBottom + 0.025f*scale, 0.005f*scale, 0.005f*scale, 30, 1.0f, 1.0f, 0.0f);
    drawFilledCircleS3(x - busLength/2 - 0.004f*scale, busBottom + 0.055f*scale, 0.005f*scale, 0.005f*scale, 30, 1.0f, 1.0f, 0.0f);

    float wheelRadius = 0.02f * scale;
    drawFilledCircleS3(x - busLength/3, busBottom - wheelRadius/2, wheelRadius, wheelRadius, 50, 0, 0, 0);
    drawFilledCircleS3(x + busLength/3, busBottom - wheelRadius/2, wheelRadius, wheelRadius, 50, 0, 0, 0);
    drawFilledCircleS3(x - busLength/3, busBottom - wheelRadius/2, wheelRadius*0.5f, wheelRadius*0.5f, 50, 0.7f, 0.7f, 0.7f);
    drawFilledCircleS3(x + busLength/3, busBottom - wheelRadius/2, wheelRadius*0.5f, wheelRadius*0.5f, 50, 0.7f, 0.7f, 0.7f);

    // backlight (red)
    drawRectangle(x + busLength/2 - 0.01f*scale, busTop - 0.02f*scale, x + busLength/2 + 0.006f*scale, busTop, 1.0f, 0.0f, 0.0f );
}
    void drawZebraCrossing(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_POLYGON);
        glVertex2f(x - width/2, y - height/2);
        glVertex2f(x + width/2, y - height/2);
        glVertex2f(x + width/2, y + height/2);
        glVertex2f(x - width/2, y + height/2);
    glEnd();
    }
    void drawLight(float cx, float cy, float r, float red, float green, float blue, bool on) {

       if (on) {
        // Outer glow circle (larger, semi-transparent)
        glColor4f(red, green, blue, 0.6f);
        drawFilledCircleS3(cx, cy, r * 1.4f, r * 1.4f, 50, red, green, blue, 0.6f);

        // Inner bright circle
        glColor3f(red, green, blue);
        drawFilledCircleS3(cx, cy, r, r, 50, red, green, blue);

        // Small highlight (white, semi-transparent)
        glColor4f(1, 1, 1, 0.4f);
        drawFilledCircleS3(cx - r * 0.3f, cy + r * 0.3f, r * 0.3f, r * 0.3f, 50, 1, 1, 1, 0.4f);
    } else {
        // Off light: dark circle with subtle shading
        glColor3f(0.15f, 0.15f, 0.15f);
        drawFilledCircleS3(cx, cy, r, r, 50, 0.15f, 0.15f, 0.15f);

        glColor3f(0.05f, 0.05f, 0.05f);
        drawFilledCircleS3(cx, cy, r * 0.7f, r * 0.7f, 50, 0.05f, 0.05f, 0.05f);
    }
    }
    void drawTrafficLight(float x, float y, float scale) {

   // Dimensions
    float poleWidth = 0.012f * scale;
    float poleHeight = 0.14f * scale;
    float baseHeight = 0.02f * scale;
    float boxWidth = 0.035f * scale;
    float boxHeight = 0.10f * scale;
    float cornerRadius = 0.005f * scale;

    // Draw pole (dark gray)
    drawRectangle(x - poleWidth / 2, y, x + poleWidth / 2, y + poleHeight, 0.15f, 0.15f, 0.15f);

    // Draw pole base (wider rectangle)
    drawRectangle(x - poleWidth, y - baseHeight, x + poleWidth, y, 0.1f, 0.1f, 0.1f);

    // Draw box background with rounded corners:
    // Main rectangle (box without corners)
    drawRectangle(x - boxWidth / 2, y + poleHeight, x + boxWidth / 2, y + poleHeight + boxHeight, 0.12f, 0.12f, 0.12f);

    // Corner circles (4 corners)
    drawFilledCircleS3(x - boxWidth / 2, y + poleHeight + cornerRadius, cornerRadius, cornerRadius, 30, 0.12f, 0.12f, 0.12f);
    drawFilledCircleS3(x + boxWidth / 2, y + poleHeight + cornerRadius, cornerRadius, cornerRadius, 30, 0.12f, 0.12f, 0.12f);
    drawFilledCircleS3(x - boxWidth / 2, y + poleHeight + boxHeight - cornerRadius, cornerRadius, cornerRadius, 30, 0.12f, 0.12f, 0.12f);
    drawFilledCircleS3(x + boxWidth / 2, y + poleHeight + boxHeight - cornerRadius, cornerRadius, cornerRadius, 30, 0.12f, 0.12f, 0.12f);

    // Draw visor (hood) - a small trapezoid on top
    float visorHeight = 0.01f * scale;
    float visorWidth = boxWidth * 1.1f;
    glColor3f(0.05f, 0.05f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(x - visorWidth / 2, y + poleHeight + boxHeight);
        glVertex2f(x + visorWidth / 2, y + poleHeight + boxHeight);
        glVertex2f(x + visorWidth / 2 - 0.005f * scale, y + poleHeight + boxHeight + visorHeight);
        glVertex2f(x - visorWidth / 2 + 0.005f * scale, y + poleHeight + boxHeight + visorHeight);
    glEnd();

    // Light positions and radius
    float lightRadius = 0.009f * scale;
    float spacing = 0.032f * scale;
    float redY = y + poleHeight + boxHeight - lightRadius - 0.015f * scale;
    float yellowY = redY - spacing;
    float greenY = yellowY - spacing;

    // Draw each light
    drawLight(x, redY, lightRadius, 1.0f, 0.0f, 0.0f, trafficLight == RED);
    drawLight(x, yellowY, lightRadius, 1.0f, 1.0f, 0.0f, trafficLight == YELLOW);
    drawLight(x, greenY, lightRadius, 0.0f, 1.0f, 0.0f, trafficLight == GREEN);
    }
    void drawMountain(float x, float y, float scale) {
    // Darker blue base for the mountain (closer to the pine tree blue)
    glColor3f(0.5f, 0.6f, 0.7f); // A good mid-blue for the mountain base
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.35f * scale, y);               // Bottom-left
        glVertex2f(x + 0.35f * scale, y);               // Bottom-right
        glVertex2f(x, y + 0.45f * scale);               // Peak
    glEnd();

    // White snow cap (on top half)
    glColor3f(1.0f, 1.0f, 1.0f); // Pure white for snow
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.2f * scale, y + 0.2f * scale); // Snow cap bottom-left
        glVertex2f(x + 0.2f * scale, y + 0.2f * scale); // Snow cap bottom-right
        glVertex2f(x, y + 0.45f * scale);               // Peak (shared with base)
    glEnd();

    // Subtle shading on the right side of the blue base (optional, for depth)
    // You can adjust this color or remove this part if you want a flatter look
    glColor3f(0.4f, 0.5f, 0.6f); // Slightly darker blue
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 0.35f * scale, y);
        glVertex2f(x + 0.15f * scale, y + 0.15f * scale);
        glVertex2f(x, y + 0.45f * scale);
    glEnd();

    }
    void drawSkiLift(float gondolaX, float gondolaY) {
        glColor3f(0.9f, 0.1f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2f(gondolaX - 0.06f, gondolaY - 0.1f);   // Bottom-left
        glVertex2f(gondolaX - 0.08f, gondolaY - 0.07f);  // Left lower-mid
        glVertex2f(gondolaX - 0.06f, gondolaY - 0.04f);  // Left upper-mid
        glVertex2f(gondolaX + 0.06f, gondolaY - 0.04f);  // Right upper-mid
        glVertex2f(gondolaX + 0.08f, gondolaY - 0.07f);  // Right lower-mid
        glVertex2f(gondolaX + 0.06f, gondolaY - 0.1f);   // Bottom-right
    glEnd();

    // Window (slightly inset polygon)
    glColor3f(0.85f, 0.95f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(gondolaX - 0.045f, gondolaY - 0.08f);
        glVertex2f(gondolaX - 0.06f, gondolaY - 0.06f);
        glVertex2f(gondolaX - 0.045f, gondolaY - 0.045f);
        glVertex2f(gondolaX + 0.045f, gondolaY - 0.045f);
        glVertex2f(gondolaX + 0.06f, gondolaY - 0.06f);
        glVertex2f(gondolaX + 0.045f, gondolaY - 0.08f);
    glEnd();

    // Gondola roof (Black trapezoid)
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(gondolaX - 0.06f, gondolaY - 0.04f);
        glVertex2f(gondolaX + 0.06f, gondolaY - 0.04f);
        glVertex2f(gondolaX + 0.04f, gondolaY - 0.025f);
        glVertex2f(gondolaX - 0.04f, gondolaY - 0.025f);
    glEnd();

    // Lift arm connecting to the wire (thin black line)
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(gondolaX, gondolaY - 0.025f);
        glVertex2f(gondolaX, gondolaY);
    glEnd();
    }
    void drawSkiLiftWires() {
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(2.0f);

    // Wire 1 (for left-to-right lift)
    glBegin(GL_LINES);
        glVertex2f(-2.0f, 0.60f);
        glVertex2f(2.0f, 0.60f);
    glEnd();

    // Wire 2 (for right-to-left lift)
    glBegin(GL_LINES);
        glVertex2f(-2.0f, 0.55f);
        glVertex2f(2.0f, 0.55f);
    glEnd();

    }
    void drawStar(float x, float y, float size) {
     glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
    glPointSize(size * 300.0f);
    glBegin(GL_POINTS);
        glVertex2f(x, y);
    glEnd();

    }
    void drawMoon(float cx, float cy, float r) {
        glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Glow behind moon (larger, semi-transparent white)
    glColor4f(1.0f, 1.0f, 1.0f, 0.15f);
    drawFilledCircleS3(cx, cy, r * 1.3f, r * 1.3f, 100, 1.0f, 1.0f, 1.0f, 0.15f);

    // Moon base: pure white circle
    drawFilledCircleS3(cx, cy, r, r, 100, 1.0f, 1.0f, 1.0f);

    // Overlay circle to create crescent shape (use dark night background color)
    float offset = r * 0.4f;
    glColor3f(0.05f, 0.05f, 0.1f); // Dark night background color
    drawFilledCircleS3(cx + offset, cy + offset * 0.2f, r, r, 100, 0.05f, 0.05f, 0.1f);
     glDisable(GL_BLEND);
    }
   // Helper: Draw filled circle with color and alpha
void drawFilledCirclesS3(float cx, float cy, float rx, float ry, int segments, float r, float g, float b, float alpha=1.0f) {
    glColor4f(r, g, b, alpha);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segments; i++) {
        float theta = 2.0f * 3.1415926f * i / segments;
        float x = rx * cosf(theta);
        float y = ry * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}

void drawCircledStar(float x, float y, float scale) {

     glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    float outerRadius = 0.07f * scale;
    float innerRadius = 0.03f * scale;

    // Glow behind star (soft yellow)
    drawFilledCircleS3(x, y, outerRadius * 1.5f, outerRadius * 1.5f, 50, 1.0f, 0.8f, 0.0f, 0.3f);

    // Star shape (10 points)
    glColor3f(1.0f, 0.8f, 0.0f); // Gold/yellow
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y); // Center
        for (int i = 0; i <= 10; ++i) {
            float angle = i * 3.1415926f / 5.0f - 3.1415926f / 2.0f;
            float radius = (i % 2 == 0) ? outerRadius : innerRadius;
            glVertex2f(x + cosf(angle) * radius, y + sinf(angle) * radius);
            glDisable(GL_BLEND);
        }
    glEnd();
}

   void drawOrnament(float x, float y, float r, float g, float b, float radius) {
    // Base circle
    drawFilledCirclesS3(x, y, radius, radius, 30, r, g, b);

    // White highlight (top-left)
    glColor4f(1.0f, 1.0f, 1.0f, 0.6f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        for (int i = 0; i <= 20; ++i) {
            float angle = i * 3.1415926f / 10.0f + 3.1415926f / 4.0f;
            float highlightRadius = radius * 0.4f;
            glVertex2f(x + cosf(angle) * highlightRadius, y + sinf(angle) * highlightRadius);
        }
    glEnd();
}
    void drawFoliageLayer(float x, float y, float width, float height, float scale, float r, float g, float b) {
        glBegin(GL_POLYGON);
        // Bottom vertices (wider base)
        glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
        glVertex2f(x - width * 0.5f * scale, y);      // bottom left
        glVertex2f(x + width * 0.5f * scale, y);      // bottom right

        // Top vertices (narrower top)
        glColor3f(r, g, b);
        glVertex2f(x + width * 0.2f * scale, y + height * scale); // top right
        glVertex2f(x - width * 0.2f * scale, y + height * scale); // top left
    glEnd();
    }
    void drawSnowBlob(float cx, float cy, float radius) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= 20; i++) {
            float angle = i * 2.0f * 3.14159f / 20;
            float dx = cos(angle) * radius;
            float dy = sin(angle) * radius;
            glVertex2f(cx + dx, cy + dy);
        }
    glEnd();
    }
    void drawChristmasTree(float x, float y, float scale) {
        // 1. Tree trunk
    glColor3f(0.5f, 0.35f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.05f * scale, y);
        glVertex2f(x + 0.05f * scale, y);
        glVertex2f(x + 0.05f * scale, y + 0.15f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.15f * scale);
    glEnd();

    // The foliage layers are now closer together
    // Height for each layer
    float layerHeight = 0.25f;

    // 2. Tree foliage (layered trapezoids)
    // The scale of the width decreases with each layer
    drawFoliageLayer(x, y + 0.15f * scale, 0.8f, layerHeight, scale, 0.0f, 0.5f, 0.0f);   // bottom layer
    drawFoliageLayer(x, y + (0.15f + layerHeight) * scale, 0.7f, layerHeight, scale, 0.0f, 0.55f, 0.0f); // layer 2
    drawFoliageLayer(x, y + (0.15f + 2 * layerHeight) * scale, 0.6f, layerHeight, scale, 0.0f, 0.6f, 0.0f); // layer 3
    drawFoliageLayer(x, y + (0.15f + 3 * layerHeight) * scale, 0.5f, layerHeight, scale, 0.0f, 0.65f, 0.0f); // layer 4

    // Cone-shaped top (now with a smaller base to fit the new layering)
    glBegin(GL_TRIANGLES);
        glColor3f(0.0f, 0.45f, 0.0f);
        glVertex2f(x - 0.2f * scale, y + (0.15f + 4 * layerHeight) * scale);
        glColor3f(0.0f, 0.55f, 0.0f);
        glVertex2f(x + 0.2f * scale, y + (0.15f + 4 * layerHeight) * scale);
        glColor3f(0.0f, 0.65f, 0.0f);
        glVertex2f(x, y + (0.15f + 5.5f * layerHeight) * scale);
    glEnd();

    // 3. Snow blobs
    drawSnowBlob(x - 0.15f * scale, y + 0.25f * scale, 0.03f * scale);
    drawSnowBlob(x + 0.18f * scale, y + 0.5f * scale, 0.025f * scale);
    drawSnowBlob(x - 0.1f * scale, y + 0.75f * scale, 0.02f * scale);
    drawSnowBlob(x + 0.05f * scale, y + 1.1f * scale, 0.03f * scale);
    drawSnowBlob(x - 0.2f * scale, y + 0.4f * scale, 0.025f * scale);

    // 4. Christmas star
    drawCircledStar(x, y + (0.15f + 5.75f * layerHeight) * scale, scale);

    // 5. Wire lights
    glColor3f(1.0f, 1.0f, 0.0f);
    glLineWidth(1.5f * scale);

    // Bottom layer lights
    glBegin(GL_LINE_STRIP);
        glVertex2f(x - 0.35f * scale, y + 0.25f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.3f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.38f * scale);
    glEnd();

    // Second layer lights
    glBegin(GL_LINE_STRIP);
        glVertex2f(x - 0.3f * scale, y + 0.5f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.55f * scale);
        glVertex2f(x - 0.1f * scale, y + 0.6f * scale);
    glEnd();

    // Third layer lights
    glBegin(GL_LINE_STRIP);
        glVertex2f(x - 0.25f * scale, y + 0.75f * scale);
        glVertex2f(x + 0.2f * scale, y + 0.7f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.8f * scale);
    glEnd();

    // Top layer lights
    glBegin(GL_LINE_STRIP);
        glVertex2f(x - 0.2f * scale, y + 0.95f * scale);
        glVertex2f(x + 0.15f * scale, y + 0.9f * scale);
        glVertex2f(x - 0.05f * scale, y + 1.05f * scale);
    glEnd();

    glBegin(GL_LINE_STRIP);
        glVertex2f(x - 0.15f * scale, y + 1.1f * scale);   // left side of cone
        glVertex2f(x + 0.1f * scale, y + 1.2f * scale);   // middle
        glVertex2f(x - 0.05f * scale, y + 1.3f * scale);  // near top
    glEnd();

    // 6. Ornaments
    float ornamentRadius = 0.03f * scale;
    drawOrnament(x - 0.2f * scale, y + 0.2f * scale, 1.0f, 0.0f, 0.0f, ornamentRadius);
    drawOrnament(x + 0.18f * scale, y + 0.35f * scale, 0.0f, 0.0f, 1.0f, ornamentRadius);
    drawOrnament(x - 0.12f * scale, y + 0.5f * scale, 1.0f, 1.0f, 0.0f, ornamentRadius);
    drawOrnament(x + 0.08f * scale, y + 0.65f * scale, 0.0f, 1.0f, 0.0f, ornamentRadius);
    drawOrnament(x - 0.25f * scale, y + 0.75f * scale, 1.0f, 0.0f, 1.0f, ornamentRadius);
    drawOrnament(x + 0.15f * scale, y + 0.9f * scale, 0.0f, 1.0f, 1.0f, ornamentRadius);

    // Ornaments on the cone part
    drawOrnament(x - 0.1f * scale, y + 1.15f * scale, 1.0f, 0.5f, 0.0f, ornamentRadius);
    drawOrnament(x + 0.1f * scale, y + 1.25f * scale, 0.5f, 0.5f, 1.0f, ornamentRadius);
    drawOrnament(x, y + 1.35f * scale, 1.0f, 0.0f, 0.5f, ornamentRadius);

    // 7. Gifts (restored from your original code)
        // Gift 1 (Red) - Bigger
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.25f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.20f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.20f * scale);
    glEnd();
    // Ribbon white vertical
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.16f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.14f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.14f * scale, y + 0.20f * scale);
        glVertex2f(x - 0.16f * scale, y + 0.20f * scale);
    glEnd();
    // Ribbon white horizontal
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.25f * scale, y + 0.12f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.12f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.14f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.14f * scale);
    glEnd();

    // Gift 2 (Blue) - Bigger
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.05f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.20f * scale);
        glVertex2f(x + 0.05f * scale, y + 0.20f * scale);
    glEnd();
    // Ribbon yellow vertical
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.14f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.16f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.16f * scale, y + 0.20f * scale);
        glVertex2f(x + 0.14f * scale, y + 0.20f * scale);
    glEnd();
    // Ribbon yellow horizontal
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.05f * scale, y + 0.12f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.12f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.14f * scale);
        glVertex2f(x + 0.05f * scale, y + 0.14f * scale);
    glEnd();

    // Gift 3 (Purple) - Bigger
    glColor3f(0.5f, 0.0f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.07f * scale, y - 0.02f * scale);
        glVertex2f(x + 0.07f * scale, y - 0.02f * scale);
        glVertex2f(x + 0.07f * scale, y + 0.12f * scale);
        glVertex2f(x - 0.07f * scale, y + 0.12f * scale);
    glEnd();
    // Ribbon pink vertical
    glColor3f(1.0f, 0.6f, 1.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.015f * scale, y - 0.02f * scale);
        glVertex2f(x + 0.015f * scale, y - 0.02f * scale);
        glVertex2f(x + 0.015f * scale, y + 0.12f * scale);
        glVertex2f(x - 0.015f * scale, y + 0.12f * scale);
    glEnd();
    // Ribbon pink horizontal
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.07f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.07f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.07f * scale, y + 0.07f * scale);
        glVertex2f(x - 0.07f * scale, y + 0.07f * scale);
    glEnd();
    }
    void drawH0Circle(float cx, float cy, float r, int num_segments, float red, float green, float blue) {

    glColor3f(red, green, blue);
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(num_segments);
        float x = r * cos(theta);
        float y = r * sin(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();

    }
    void drawHouse01(float x, float y, float scale) {

    // Main house body (red rectangle)
    glColor3f(0.8f, 0.2f, 0.2f); // Red
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.4f * scale, y);
        glVertex2f(x + 0.4f * scale, y);
        glVertex2f(x + 0.4f * scale, y + 0.5f * scale);
        glVertex2f(x - 0.4f * scale, y + 0.5f * scale);
    glEnd();

    // Main roof (darker red triangle)
    glColor3f(0.6f, 0.15f, 0.15f); // Darker Red for roof
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.5f * scale, y + 0.5f * scale);
        glVertex2f(x + 0.5f * scale, y + 0.5f * scale);
        glVertex2f(x, y + 0.8f * scale);
    glEnd();

    // Door (dark brown rectangle)
    glColor3f(0.3f, 0.15f, 0.0f); // Dark Brown
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.3f * scale, y);
        glVertex2f(x - 0.2f * scale, y);
        glVertex2f(x - 0.2f * scale, y + 0.25f * scale);
        glVertex2f(x - 0.3f * scale, y + 0.25f * scale);
    glEnd();



    // Main window (yellowish light)
    glColor3f(1.0f, 0.9f, 0.5f); // Yellowish light
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.05f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.35f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.35f * scale, y + 0.4f * scale);
        glVertex2f(x + 0.05f * scale, y + 0.4f * scale);
    glEnd();
    // Window panes (dark lines)
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray for panes
    glLineWidth(1.5f * scale);
    glBegin(GL_LINES);
        glVertex2f(x + 0.2f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.2f * scale, y + 0.4f * scale); // Vertical pane
        glVertex2f(x + 0.05f * scale, y + 0.3f * scale);
        glVertex2f(x + 0.35f * scale, y + 0.3f * scale); // Horizontal pane
    glEnd();

    // Attic window (circular, yellowish light)
    drawH0Circle(x, y + 0.6f * scale, 0.1f * scale, 30, 1.0f, 0.9f, 0.5f); // Yellowish light
    // Attic window frame/panes
    glColor3f(0.2f, 0.2f, 0.2f);
    glLineWidth(1.5f * scale);
    glBegin(GL_LINES);
        glVertex2f(x, y + 0.5f * scale);
        glVertex2f(x, y + 0.7f * scale); // Vertical pane
        glVertex2f(x - 0.1f * scale, y + 0.6f * scale);
        glVertex2f(x + 0.1f * scale, y + 0.6f * scale); // Horizontal pane
    glEnd();

    // Chimney (brown rectangle)
    glColor3f(0.6f, 0.3f, 0.1f); // Brown
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.45f * scale, y + 0.5f * scale);
        glVertex2f(x - 0.35f * scale, y + 0.5f * scale);
        glVertex2f(x - 0.35f * scale, y + 0.7f * scale);
        glVertex2f(x - 0.45f * scale, y + 0.7f * scale);
    glEnd();
    // Chimney top (darker brown)
    glColor3f(0.4f, 0.2f, 0.05f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.47f * scale, y + 0.7f * scale);
        glVertex2f(x - 0.33f * scale, y + 0.7f * scale);
        glVertex2f(x - 0.33f * scale, y + 0.73f * scale);
        glVertex2f(x - 0.47f * scale, y + 0.73f * scale);
    glEnd();

    // Snow on roof (white polygons) - thinner snow
    glColor3f(0.95f, 0.98f, 1.0f); // White
    // Main roof snow (thinner)
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.45f * scale, y + 0.65f * scale);
        glVertex2f(x + 0.45f * scale, y + 0.65f * scale);
        glVertex2f(x, y + 0.78f * scale);
    glEnd();
    // Snow on left side of main body (thinner)
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.4f * scale, y + 0.6f * scale);
        glVertex2f(x - 0.3f * scale, y + 0.6f * scale);
        glVertex2f(x - 0.29f * scale, y + 0.62f * scale);
        glVertex2f(x - 0.39f * scale, y + 0.62f * scale);
    glEnd();
    // Snow on right side of main body (thinner)
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.4f * scale, y + 0.6f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.6f * scale);
        glVertex2f(x + 0.31f * scale, y + 0.62f * scale);
        glVertex2f(x + 0.41f * scale, y + 0.62f * scale);
    glEnd();
    // Snow on chimney top (thinner)
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.46f * scale, y + 0.74f * scale);
        glVertex2f(x - 0.34f * scale, y + 0.74f * scale);
        glVertex2f(x - 0.34f * scale, y + 0.75f * scale);
        glVertex2f(x - 0.46f * scale, y + 0.75f * scale);
    glEnd();

    // Additional elements like the small entryway roof and snow on ground
    // Entryway structure (darker red rectangle)
    glColor3f(0.7f, 0.15f, 0.15f); // Slightly darker red
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.35f * scale, y - 0.05f * scale);
        glVertex2f(x - 0.15f * scale, y - 0.05f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.2f * scale);
        glVertex2f(x - 0.35f * scale, y + 0.2f * scale);
    glEnd();
    // Entryway roof (darker red triangle)
    glColor3f(0.6f, 0.15f, 0.15f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.38f * scale, y + 0.2f * scale);
        glVertex2f(x - 0.12f * scale, y + 0.2f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.35f * scale);
    glEnd();
    // Entryway door (dark brown) - slightly in front of main door
    glColor3f(0.3f, 0.15f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.3f * scale, y - 0.05f * scale);
        glVertex2f(x - 0.2f * scale, y - 0.05f * scale);
        glVertex2f(x - 0.2f * scale, y + 0.15f * scale);
        glVertex2f(x - 0.3f * scale, y + 0.15f * scale);
    glEnd();
    // Entryway roof snow (thinner)
    glColor3f(1.0f, 1.0f, 1.0f); // White
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.35f * scale, y + 0.21f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.21f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.34f * scale);
    glEnd();

    // Snow on the ground around the house
    glColor3f(0.9f, 0.9f, 0.95f); // Light blue-white for ground snow
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.5f * scale, y - 0.1f * scale);
        glVertex2f(x + 0.5f * scale, y - 0.1f * scale);
        glVertex2f(x + 0.6f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.4f * scale, y + 0.0f * scale);
        glVertex2f(x - 0.4f * scale, y - 0.05f * scale);
        glVertex2f(x - 0.6f * scale, y + 0.0f * scale);
    glEnd();

    }
    void drawHouse02(float x, float y, float scale) {

    // Base of the house (blue rectangle)
    glColor3f(0.3f, 0.5f, 0.8f); // Light blue
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.5f * scale, y);
        glVertex2f(x + 0.5f * scale, y);
        glVertex2f(x + 0.5f * scale, y + 0.4f * scale);
        glVertex2f(x - 0.5f * scale, y + 0.4f * scale);
    glEnd();

    // Main roof (darker blue/grey triangle)
    glColor3f(0.2f, 0.4f, 0.6f); // Darker blue for roof base
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.6f * scale, y + 0.4f * scale);
        glVertex2f(x + 0.6f * scale, y + 0.4f * scale);
        glVertex2f(x, y + 0.7f * scale);
    glEnd();

    // Heavy snow on main roof (white, rounded)
    glColor3f(0.95f, 0.98f, 1.0f); // Very light blue-white
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.65f * scale, y + 0.42f * scale);
        glVertex2f(x + 0.65f * scale, y + 0.42f * scale);
        glVertex2f(x + 0.5f * scale, y + 0.55f * scale);
        glVertex2f(x + 0.2f * scale, y + 0.75f * scale);
        glVertex2f(x, y + 0.8f * scale); // Peak of snow
        glVertex2f(x - 0.2f * scale, y + 0.75f * scale);
        glVertex2f(x - 0.5f * scale, y + 0.55f * scale);
    glEnd();

    // Central Entrance (arched dark brown door)
    glColor3f(0.4f, 0.2f, 0.0f); // Dark brown
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.15f * scale, y);
        glVertex2f(x + 0.15f * scale, y);
        glVertex2f(x + 0.15f * scale, y + 0.35f * scale);
        glVertex2f(x + 0.1f * scale, y + 0.4f * scale); // Arc start
        glVertex2f(x, y + 0.42f * scale);             // Arc peak
        glVertex2f(x - 0.1f * scale, y + 0.4f * scale); // Arc end
        glVertex2f(x - 0.15f * scale, y + 0.35f * scale);
    glEnd();
    // Door details (lighter brown panes)
    glColor3f(0.6f, 0.3f, 0.1f);
    glLineWidth(1.0f * scale);
    glBegin(GL_LINES);
        // Vertical split
        glVertex2f(x, y);
        glVertex2f(x, y + 0.38f * scale);
        // Horizontal splits
        glVertex2f(x - 0.13f * scale, y + 0.15f * scale);
        glVertex2f(x + 0.13f * scale, y + 0.15f * scale);
        glVertex2f(x - 0.13f * scale, y + 0.28f * scale);
        glVertex2f(x + 0.13f * scale, y + 0.28f * scale);
    glEnd();

    // Wreath on door (green circle with red dot)
    drawH0Circle(x, y + 0.4f * scale, 0.05f * scale, 20, 0.0f, 0.6f, 0.2f); // Green wreath
    drawH0Circle(x + 0.03f * scale, y + 0.37f * scale, 0.015f * scale, 10, 0.8f, 0.0f, 0.0f); // Red berry

    // Dormer Windows (two of them)
    // Left Dormer
    glColor3f(0.3f, 0.5f, 0.8f); // Blue dormer body
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.35f * scale, y + 0.45f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.45f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.55f * scale);
        glVertex2f(x - 0.35f * scale, y + 0.55f * scale);
    glEnd();
    glColor3f(0.2f, 0.4f, 0.6f); // Darker blue dormer roof
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.4f * scale, y + 0.55f * scale);
        glVertex2f(x - 0.1f * scale, y + 0.55f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.65f * scale);
    glEnd();
    // Left dormer window (yellow light)
    glColor3f(1.0f, 0.9f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.32f * scale, y + 0.47f * scale);
        glVertex2f(x - 0.18f * scale, y + 0.47f * scale);
        glVertex2f(x - 0.18f * scale, y + 0.53f * scale);
        glVertex2f(x - 0.32f * scale, y + 0.53f * scale);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f); // Window panes
    glLineWidth(1.0f * scale);
    glBegin(GL_LINES);
        glVertex2f(x - 0.25f * scale, y + 0.47f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.53f * scale);
        glVertex2f(x - 0.32f * scale, y + 0.50f * scale);
        glVertex2f(x - 0.18f * scale, y + 0.50f * scale);
    glEnd();
    // Left dormer snow
    glColor3f(0.95f, 0.98f, 1.0f); // Snow on dormer roof
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.4f * scale, y + 0.56f * scale);
        glVertex2f(x - 0.1f * scale, y + 0.56f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.66f * scale);
    glEnd();

    // Right Dormer (duplicate of left, shifted)
    glColor3f(0.3f, 0.5f, 0.8f); // Blue dormer body
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.15f * scale, y + 0.45f * scale);
        glVertex2f(x + 0.35f * scale, y + 0.45f * scale);
        glVertex2f(x + 0.35f * scale, y + 0.55f * scale);
        glVertex2f(x + 0.15f * scale, y + 0.55f * scale);
    glEnd();
    glColor3f(0.2f, 0.4f, 0.6f); // Darker blue dormer roof
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 0.1f * scale, y + 0.55f * scale);
        glVertex2f(x + 0.4f * scale, y + 0.55f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.65f * scale);
    glEnd();
    // Right dormer window (yellow light)
    glColor3f(1.0f, 0.9f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.18f * scale, y + 0.47f * scale);
        glVertex2f(x + 0.32f * scale, y + 0.47f * scale);
        glVertex2f(x + 0.32f * scale, y + 0.53f * scale);
        glVertex2f(x + 0.18f * scale, y + 0.53f * scale);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f); // Window panes
    glLineWidth(1.0f * scale);
    glBegin(GL_LINES);
        glVertex2f(x + 0.25f * scale, y + 0.47f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.53f * scale);
        glVertex2f(x + 0.18f * scale, y + 0.50f * scale);
        glVertex2f(x + 0.32f * scale, y + 0.50f * scale);
    glEnd();
    // Right dormer snow
    glColor3f(0.95f, 0.98f, 1.0f); // Snow on dormer roof
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.1f * scale, y + 0.56f * scale);
        glVertex2f(x + 0.4f * scale, y + 0.56f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.66f * scale);
    glEnd();

    // Side windows (simple rectangles)
    // Left side window
    glColor3f(1.0f, 0.9f, 0.5f); // Yellowish light
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.45f * scale, y + 0.1f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.1f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.35f * scale);
        glVertex2f(x - 0.45f * scale, y + 0.35f * scale);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f); // Window panes
    glLineWidth(1.0f * scale);
    glBegin(GL_LINES);
        glVertex2f(x - 0.35f * scale, y + 0.1f * scale);
        glVertex2f(x - 0.35f * scale, y + 0.35f * scale); // Vertical pane
        glVertex2f(x - 0.45f * scale, y + 0.225f * scale);
        glVertex2f(x - 0.25f * scale, y + 0.225f * scale); // Horizontal pane
    glEnd();

    // Right side window (duplicate of left, shifted)
    glColor3f(1.0f, 0.9f, 0.5f); // Yellowish light
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.25f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.45f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.45f * scale, y + 0.35f * scale);
        glVertex2f(x + 0.25f * scale, y + 0.35f * scale);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f); // Window panes
    glLineWidth(1.0f * scale);
    glBegin(GL_LINES);
        glVertex2f(x + 0.35f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.35f * scale, y + 0.35f * scale); // Vertical pane
        glVertex2f(x + 0.25f * scale, y + 0.225f * scale);
        glVertex2f(x + 0.45f * scale, y + 0.225f * scale); // Horizontal pane
    glEnd();


    // Chimney (darker blue rectangle)
    glColor3f(0.2f, 0.4f, 0.6f); // Darker blue
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.05f * scale, y + 0.7f * scale);
        glVertex2f(x + 0.15f * scale, y + 0.7f * scale);
        glVertex2f(x + 0.15f * scale, y + 0.8f * scale);
        glVertex2f(x + 0.05f * scale, y + 0.8f * scale);
    glEnd();
    // Chimney top (darkest blue)
    glColor3f(0.1f, 0.3f, 0.5f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.03f * scale, y + 0.8f * scale);
        glVertex2f(x + 0.17f * scale, y + 0.8f * scale);
        glVertex2f(x + 0.17f * scale, y + 0.83f * scale);
        glVertex2f(x + 0.03f * scale, y + 0.83f * scale);
    glEnd();

    // Snow on ground around the house (large, rounded mounds)
    glColor3f(0.95f, 0.98f, 1.0f); // Very light blue-white for ground snow
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.7f * scale, y - 0.05f * scale);
        glVertex2f(x + 0.7f * scale, y - 0.05f * scale);
        glVertex2f(x + 0.6f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.1f * scale, y + 0.15f * scale);
        glVertex2f(x - 0.1f * scale, y + 0.15f * scale);
        glVertex2f(x - 0.3f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.6f * scale, y + 0.1f * scale);
    glEnd();

    // Small pine trees (simplified cones) with snow
    // Left tree
    glColor3f(0.1f, 0.4f, 0.1f); // Dark green color
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.6f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.4f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.5f * scale, y + 0.35f * scale);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f); // White snow
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.52f * scale, y + 0.3f * scale);
        glVertex2f(x - 0.48f * scale, y + 0.3f * scale);
        glVertex2f(x - 0.5f * scale, y + 0.35f * scale);
    glEnd();

    // Right tree
    glColor3f(0.1f, 0.4f, 0.1f); // Dark green color
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.4f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.6f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.5f * scale, y + 0.35f * scale);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f); // White snow
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 0.48f * scale, y + 0.3f * scale);
        glVertex2f(x + 0.52f * scale, y + 0.3f * scale);
        glVertex2f(x + 0.5f * scale, y + 0.35f * scale);
    glEnd();

    }
    void drawHouse03(float x, float y, float scale) {

    // Main house body (light tan rectangle)
    glColor3f(0.85f, 0.75f, 0.6f); // Light tan/brown
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.4f * scale, y);
        glVertex2f(x + 0.4f * scale, y);
        glVertex2f(x + 0.4f * scale, y + 0.5f * scale);
        glVertex2f(x - 0.4f * scale, y + 0.5f * scale);
    glEnd();

    // Main roof (dark brown/grey triangle)
    glColor3f(0.4f, 0.3f, 0.2f); // Dark brown/grey for roof
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.5f * scale, y + 0.5f * scale);
        glVertex2f(x + 0.5f * scale, y + 0.5f * scale);
        glVertex2f(x, y + 0.8f * scale);
    glEnd();

    // Snow on the roof
    glColor3f(1.0f, 1.0f, 1.0f); // White snow
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.48f * scale, y + 0.51f * scale);
        glVertex2f(x + 0.48f * scale, y + 0.51f * scale);
        glVertex2f(x + 0.05f * scale, y + 0.77f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.76f * scale);
    glEnd();

    // Chimney (dark brick color)
    glColor3f(0.6f, 0.2f, 0.1f); // Dark brick red
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.3f * scale, y + 0.5f * scale);
        glVertex2f(x + 0.4f * scale, y + 0.5f * scale);
        glVertex2f(x + 0.4f * scale, y + 0.7f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.7f * scale);
    glEnd();
    // Chimney top (darker color)
    glColor3f(0.4f, 0.15f, 0.1f);
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.28f * scale, y + 0.7f * scale);
        glVertex2f(x + 0.42f * scale, y + 0.7f * scale);
        glVertex2f(x + 0.42f * scale, y + 0.73f * scale);
        glVertex2f(x + 0.28f * scale, y + 0.73f * scale);
    glEnd();

    // Door (dark brown rectangle with arched top)
    glColor3f(0.3f, 0.1f, 0.0f); // Dark brown
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.05f * scale, y);
        glVertex2f(x + 0.05f * scale, y);
        glVertex2f(x + 0.05f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.03f * scale, y + 0.25f * scale);
        glVertex2f(x, y + 0.27f * scale);
        glVertex2f(x - 0.03f * scale, y + 0.25f * scale);
        glVertex2f(x - 0.05f * scale, y + 0.2f * scale);
    glEnd();

    // Entryway roof (small triangle over the door)
    glColor3f(0.4f, 0.3f, 0.2f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.1f * scale, y + 0.27f * scale);
        glVertex2f(x + 0.1f * scale, y + 0.27f * scale);
        glVertex2f(x, y + 0.35f * scale);
    glEnd();

    // Windows (glowing yellow)
    // Left window
    glColor3f(1.0f, 0.9f, 0.6f); // Glowing yellow
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.3f * scale, y + 0.2f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.2f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.4f * scale);
        glVertex2f(x - 0.3f * scale, y + 0.4f * scale);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f); // Window panes
    glLineWidth(1.5f * scale);
    glBegin(GL_LINES);
        glVertex2f(x - 0.225f * scale, y + 0.2f * scale);
        glVertex2f(x - 0.225f * scale, y + 0.4f * scale); // Vertical pane
        glVertex2f(x - 0.3f * scale, y + 0.3f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.3f * scale); // Horizontal pane
    glEnd();

    // Right window
    glColor3f(1.0f, 0.9f, 0.6f); // Glowing yellow
    glBegin(GL_POLYGON);
        glVertex2f(x + 0.15f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.4f * scale);
        glVertex2f(x + 0.15f * scale, y + 0.4f * scale);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f); // Window panes
    glLineWidth(1.5f * scale);
    glBegin(GL_LINES);
        glVertex2f(x + 0.225f * scale, y + 0.2f * scale);
        glVertex2f(x + 0.225f * scale, y + 0.4f * scale); // Vertical pane
        glVertex2f(x + 0.15f * scale, y + 0.3f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.3f * scale); // Horizontal pane
    glEnd();

    // Snow on the ground around the house
    glColor3f(0.95f, 0.98f, 1.0f); // Light blue-white for ground snow
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.5f * scale, y - 0.05f * scale);
        glVertex2f(x + 0.5f * scale, y - 0.05f * scale);
        glVertex2f(x + 0.6f * scale, y + 0.05f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.0f * scale);
        glVertex2f(x - 0.3f * scale, y + 0.0f * scale);
        glVertex2f(x - 0.6f * scale, y + 0.05f * scale);
    glEnd();

    // Small pine tree with snow
    glColor3f(0.1f, 0.4f, 0.1f); // Dark green color
    glBegin(GL_POLYGON);
        glVertex2f(x - 0.65f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.45f * scale, y + 0.05f * scale);
        glVertex2f(x - 0.55f * scale, y + 0.35f * scale);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f); // White snow
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.57f * scale, y + 0.3f * scale);
        glVertex2f(x - 0.53f * scale, y + 0.3f * scale);
        glVertex2f(x - 0.55f * scale, y + 0.35f * scale);
    glEnd();

    }
    void drawHRectangle(float x1, float y1, float x2, float y2, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x1, y1);
        glVertex2f(x2, y1);
        glVertex2f(x2, y2);
        glVertex2f(x1, y2);
    glEnd();
}

    void drawFilledHCircle(float cx, float cy, float rx, float ry, int segments, float r, float g, float b) {

    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= segments; i++) {
            float theta = 2.0f * 3.1415926f * i / segments;
            float x = rx * cosf(theta);
            float y = ry * sinf(theta);
            glVertex2f(cx + x, cy + y);
        }
    glEnd();

    }
    void drawHouse04(float x, float y, float scale) {

     // Base Platform (Brown)
    drawHRectangle(x - 0.5f * scale, y, x + 0.5f * scale, y + 0.1f * scale, 0.6f, 0.4f, 0.2f);

    // Chimney (Blue) - behind house body
    drawHRectangle(x + 0.3f * scale, y + 0.3f * scale, x + 0.4f * scale, y + 0.8f * scale, 0.3f, 0.5f, 0.8f);
    // Chimney Top (slightly darker)
    drawHRectangle(x + 0.28f * scale, y + 0.8f * scale, x + 0.42f * scale, y + 0.85f * scale, 0.25f, 0.45f, 0.75f);

    // Main A-Frame Roof Trim (Dark Brown) - behind main body
    glColor3f(0.4f, 0.3f, 0.2f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.48f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.48f * scale, y + 0.1f * scale);
        glVertex2f(x, y + 1.05f * scale);
    glEnd();

    // Main A-Frame Body (Blue)
    glColor3f(0.3f, 0.5f, 0.8f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.45f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.45f * scale, y + 0.1f * scale);
        glVertex2f(x, y + 1.0f * scale);
    glEnd();

    // Snow on Roof (White) - slightly smaller peak for layering effect
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.45f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.45f * scale, y + 0.1f * scale);
        glVertex2f(x, y + 1.02f * scale);
    glEnd();

    // Re-draw blue part uncovered by snow for layered look
    glColor3f(0.3f, 0.5f, 0.8f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.40f * scale, y + 0.1f * scale);
        glVertex2f(x + 0.40f * scale, y + 0.1f * scale);
        glVertex2f(x, y + 0.92f * scale);
    glEnd();

    // Balcony Railing (Brown)
    float railR = 0.6f, railG = 0.4f, railB = 0.2f;
    // Top rail
    drawHRectangle(x - 0.35f * scale, y + 0.25f * scale, x + 0.35f * scale, y + 0.28f * scale, railR, railG, railB);
    // Vertical posts
    for (float postX = -0.35f; postX <= 0.35f; postX += 0.1f) {
        drawRectangle(x + postX * scale, y + 0.1f * scale, x + (postX + 0.02f) * scale, y + 0.28f * scale, railR, railG, railB);
    }

    // Door (Yellow) with subtle shading
    drawHRectangle(x - 0.08f * scale, y + 0.1f * scale, x + 0.08f * scale, y + 0.4f * scale, 1.0f, 0.8f, 0.0f);
    // Door panes (dark brown)
    drawHRectangle(x - 0.01f * scale, y + 0.1f * scale, x + 0.01f * scale, y + 0.4f * scale, 0.4f, 0.3f, 0.2f); // Vertical
    drawHRectangle(x - 0.08f * scale, y + 0.24f * scale, x + 0.08f * scale, y + 0.26f * scale, 0.4f, 0.3f, 0.2f); // Horizontal
    // Door knob (small dark circle)
    drawFilledCircleS3(x + 0.06f * scale, y + 0.25f * scale, 0.01f * scale, 0.01f * scale, 20, 0.2f, 0.1f, 0.0f);

    // Windows (Yellow) with frames
    drawHRectangle(x - 0.3f * scale, y + 0.3f * scale, x - 0.15f * scale, y + 0.45f * scale, 1.0f, 0.8f, 0.0f);
    drawHRectangle(x + 0.15f * scale, y + 0.3f * scale, x + 0.3f * scale, y + 0.45f * scale, 1.0f, 0.8f, 0.0f);
    // Window frames (dark brown lines)
    glColor3f(0.4f, 0.3f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        // Left window vertical center
        glVertex2f(x - 0.225f * scale, y + 0.3f * scale);
        glVertex2f(x - 0.225f * scale, y + 0.45f * scale);
        // Left window horizontal center
        glVertex2f(x - 0.3f * scale, y + 0.375f * scale);
        glVertex2f(x - 0.15f * scale, y + 0.375f * scale);
        // Right window vertical center
        glVertex2f(x + 0.225f * scale, y + 0.3f * scale);
        glVertex2f(x + 0.225f * scale, y + 0.45f * scale);
        // Right window horizontal center
        glVertex2f(x + 0.15f * scale, y + 0.375f * scale);
        glVertex2f(x + 0.3f * scale, y + 0.375f * scale);
    glEnd();

    // String Lights (small yellow solid circles) along roof edges
    int numLights = 10;
    for (int i = 1; i < numLights; ++i) {
        float t = (float)i / numLights;

        // Left side light position (linear interpolation)
        float lightX1 = (x - 0.45f * scale) * (1.0f - t) + (x) * t;
        float lightY1 = (y + 0.1f * scale) * (1.0f - t) + (y + 1.0f * scale) * t;
        drawFilledHCircle(lightX1, lightY1, 0.015f * scale, 0.015f * scale, 20, 1.0f, 1.0f, 0.0f); // Glow solid
        drawFilledHCircle(lightX1, lightY1, 0.008f * scale, 0.008f * scale, 20, 1.0f, 1.0f, 0.5f);  // Bulb solid

        // Right side light position (linear interpolation)
        float lightX2 = (x + 0.45f * scale) * (1.0f - t) + (x) * t;
        float lightY2 = (y + 0.1f * scale) * (1.0f - t) + (y + 1.0f * scale) * t;
        drawFilledHCircle(lightX2, lightY2, 0.015f * scale, 0.015f * scale, 20, 1.0f, 1.0f, 0.0f); // Glow solid
        drawFilledHCircle(lightX2, lightY2, 0.008f * scale, 0.008f * scale, 20, 1.0f, 1.0f, 0.5f);  // Bulb solid
    }

    // Stylized Tree on the Left (Light Blue)
    // Trunk
    drawHRectangle(x - 0.6f * scale, y + 0.1f * scale, x - 0.58f * scale, y + 0.2f * scale, 0.4f, 0.3f, 0.2f);
    // Foliage (ellipse)
    drawFilledHCircle(x - 0.59f * scale, y + 0.3f * scale, 0.08f * scale, 0.15f * scale, 50, 0.7f, 0.8f, 0.9f);

    // Ground Snow Piles (solid white ellipses)
    drawFilledHCircle(x - 0.2f * scale, y + 0.05f * scale, 0.4f * scale, 0.1f * scale, 50, 1.0f, 1.0f, 1.0f);
    drawFilledHCircle(x + 0.3f * scale, y + 0.05f * scale, 0.3f * scale, 0.08f * scale, 50, 1.0f, 1.0f, 1.0f);
    }
    void drawSmoke(SmokeParticle smoke[]) {
         for (int i = 0; i < NUM_SMOKE; ++i) {
        glColor4f(0.7f, 0.7f, 0.75f, smoke[i].alpha);
        drawFilledCircleS3(smoke[i].x, smoke[i].y, smoke[i].size, smoke[i].size * 0.6f, 20, 0.7f, 0.7f, 0.75f, smoke[i].alpha);
    }
    }
    void drawSnowman(float x, float y, float scale) {
    float whiteR = 1.0f, whiteG = 1.0f, whiteB = 1.0f;
    float blackR = 0.0f, blackG = 0.0f, blackB = 0.0f;
    float orangeR = 1.0f, orangeG = 0.5f, orangeB = 0.0f;
    float redR = 0.8f, redG = 0.0f, redB = 0.0f;
    float borderThickness = 0.005f * scale;
    drawFilledCircleS3(x, y, 0.15f * scale + borderThickness, 0.15f * scale + borderThickness, 100, blackR, blackG, blackB);
    drawFilledCircleS3(x, y + 0.22f * scale, 0.12f * scale + borderThickness, 0.12f * scale + borderThickness, 100, blackR, blackG, blackB);
    drawFilledCircleS3(x, y + 0.40f * scale, 0.09f * scale + borderThickness, 0.09f * scale + borderThickness, 100, blackR, blackG, blackB);
    drawFilledCircleS3(x, y, 0.15f * scale, 0.15f * scale, 100, whiteR, whiteG, whiteB);
    drawFilledCircleS3(x, y + 0.22f * scale, 0.12f * scale, 0.12f * scale, 100, whiteR, whiteG, whiteB);
    drawFilledCircleS3(x, y + 0.40f * scale, 0.09f * scale, 0.09f * scale, 100, whiteR, whiteG, whiteB);
    float eyeOffsetX = 0.03f * scale;
    float eyeOffsetY = 0.05f * scale;
    float eyeRadiusX = 0.01f * scale;
    float eyeRadiusY = 0.015f * scale;
    drawFilledCircleS3(x - eyeOffsetX, y + 0.45f * scale, eyeRadiusX, eyeRadiusY, 50, blackR, blackG, blackB);
    drawFilledCircleS3(x + eyeOffsetX, y + 0.45f * scale, eyeRadiusX, eyeRadiusY, 50, blackR, blackG, blackB);
    glColor3f(orangeR, orangeG, orangeB);
    glBegin(GL_TRIANGLES);
        glVertex2f(x, y + 0.40f * scale);
        glVertex2f(x + 0.06f * scale, y + 0.42f * scale);
        glVertex2f(x, y + 0.44f * scale);
    glEnd();
    float mouthRadius = 0.008f * scale;
    float mouthCenterX = x;
    float mouthCenterY = y + 0.38f * scale;
    int mouthDots = 5;
    float mouthSpacing = 0.015f * scale;
    for (int i = -2; i <= 2; ++i) {
        float dotX = mouthCenterX + i * mouthSpacing;
        float dotY = mouthCenterY - fabs(i) * 0.005f * scale;
        drawFilledCircleS3(dotX, dotY, mouthRadius, mouthRadius, 20, blackR, blackG, blackB);
    }
    float buttonRadius = 0.012f * scale;
    float buttonX = x;
    float buttonStartY = y + 0.15f * scale;
    float buttonSpacing = 0.06f * scale;
    for (int i = 0; i < 3; ++i) {
        drawFilledCircleS3(buttonX, buttonStartY + i * buttonSpacing, buttonRadius, buttonRadius, 30, blackR, blackG, blackB);
    }
    float scarfWidth = 0.14f * scale;
    float scarfHeight = 0.03f * scale;
    float scarfY = y + 0.33f * scale;
    drawRectangle(x - scarfWidth / 2, scarfY, x + scarfWidth / 2, scarfY + scarfHeight, redR, redG, redB);
    drawRectangle(x + scarfWidth / 2 - 0.02f * scale, scarfY - 0.07f * scale, x + scarfWidth / 2 + 0.02f * scale, scarfY, redR, redG, redB);
    drawRectangle(x - scarfWidth / 2 - 0.02f * scale, scarfY - 0.05f * scale, x - scarfWidth / 2 + 0.02f * scale, scarfY - 0.02f * scale, redR, redG, redB);
    glColor3f(redR, redG, redB);
    glBegin(GL_TRIANGLES);
        glVertex2f(x - 0.07f * scale, y + 0.50f * scale);
        glVertex2f(x + 0.07f * scale, y + 0.50f * scale);
        glVertex2f(x, y + 0.62f * scale);
    glEnd();

    // Hat brim (black thin rectangle)
    drawRectangle(x - 0.08f * scale, y + 0.49f * scale, x + 0.08f * scale, y + 0.51f * scale, blackR, blackG, blackB);
    }
    void drawPenguin(float x, float y, float scale) {
       // Body (black oval)
    drawFilledCircleS3(x, y, 0.03f * scale, 0.05f * scale, 50, 0.0f, 0.0f, 0.0f);

    // White belly (smaller oval)
    drawFilledCircleS3(x, y - 0.01f * scale, 0.015f * scale, 0.03f * scale, 50, 1.0f, 1.0f, 1.0f);

    // Head (black circle)
    drawFilledCircleS3(x, y + 0.04f * scale, 0.02f * scale, 0.02f * scale, 50, 0.0f, 0.0f, 0.0f);

    // Eyes (white circles with black pupils)
    drawFilledCircleS3(x - 0.007f * scale, y + 0.045f * scale, 0.005f * scale, 0.005f * scale, 20, 1.0f, 1.0f, 1.0f);
    drawFilledCircleS3(x + 0.007f * scale, y + 0.045f * scale, 0.005f * scale, 0.005f * scale, 20, 1.0f, 1.0f, 1.0f);
    drawFilledCircleS3(x - 0.007f * scale, y + 0.045f * scale, 0.002f * scale, 0.002f * scale, 20, 0.0f, 0.0f, 0.0f);
    drawFilledCircleS3(x + 0.007f * scale, y + 0.045f * scale, 0.002f * scale, 0.002f * scale, 20, 0.0f, 0.0f, 0.0f);

    // Beak (orange triangle)
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x, y + 0.035f * scale);
        glVertex2f(x + 0.01f * scale, y + 0.03f * scale);
        glVertex2f(x, y + 0.025f * scale);
    glEnd();

    // Feet (orange small ovals)
    drawFilledCircleS3(x - 0.015f * scale, y - 0.05f * scale, 0.008f * scale, 0.004f * scale, 20, 1.0f, 0.5f, 0.0f);
    drawFilledCircleS3(x + 0.015f * scale, y - 0.05f * scale, 0.008f * scale, 0.004f * scale, 20, 1.0f, 0.5f, 0.0f);

    }
    void drawBangladeshFlag(float x, float y, float scale) {
         // Flagpole - thicker and darker for visibility
    float poleHeight = 0.35f * scale;
    float poleWidth = 0.015f * scale;
    drawRectangle(x - poleWidth/2, y, x + poleWidth/2, y + poleHeight, 0.1f, 0.1f, 0.1f);

    // Flag position (top of pole)
    float flagX = x + poleWidth/2;
    float flagY = y + poleHeight - 0.18f * scale;  // Align flag bottom exactly at pole top

    // Flag dimensions
    float flagWidth = 0.25f * scale;
    float flagHeight = 0.15f * scale;

    // Flag background (green)
    drawRectangle(flagX, flagY, flagX + flagWidth, flagY + flagHeight, 0.0f, 0.6f, 0.2f);

    // Red circle (centered on flag)
    float circleCX = flagX + flagWidth * 0.4f;
    float circleCY = flagY + flagHeight * 0.5f;
    float circleRadiusX = 0.05f * scale;
    float circleRadiusY = 0.05f * scale;
    drawFilledCircleS3(circleCX, circleCY, circleRadiusX, circleRadiusY, 50, 0.8f, 0.0f, 0.0f);
    }
    void drawSemiEllipse(float cx, float cy, float rx, float ry, int segments, float r, float g, float b) {

    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy); // Center
    for (int i = 0; i <= segments; ++i) {
        float theta = M_PI * i / segments; // Angle from 0 to pi (semi-ellipse)
        float x = cx + rx * cosf(theta);
        float y = cy + ry * sinf(theta);
        glVertex2f(x, y);
    }
    glEnd();

    }
    void drawPhoneBooth(float x, float y, float scale) {
        // Dimensions
    float boothWidth = 0.2f * scale;
    float boothHeight = 0.5f * scale;
    float frameThickness = 0.015f * scale;
    float windowRows = 6;
    float windowCols = 3;
    float windowMargin = 0.02f * scale;

    // Booth main body (red)
    drawRectangle(x - boothWidth/2, y, x + boothWidth/2, y + boothHeight, 0.8f, 0.0f, 0.0f);

    // Booth frame (darker red border)
    drawRectangle(x - boothWidth/2, y, x - boothWidth/2 + frameThickness, y + boothHeight, 0.5f, 0.0f, 0.0f); // Left frame
    drawRectangle(x + boothWidth/2 - frameThickness, y, x + boothWidth/2, y + boothHeight, 0.5f, 0.0f, 0.0f); // Right frame
    drawRectangle(x - boothWidth/2, y + boothHeight - frameThickness, x + boothWidth/2, y + boothHeight, 0.5f, 0.0f, 0.0f); // Top frame
    drawRectangle(x - boothWidth/2, y, x + boothWidth/2, y + frameThickness, 0.5f, 0.0f, 0.0f); // Bottom frame

    // Windows (glass panes)
    float windowWidth = (boothWidth - 2 * windowMargin) / windowCols;
    float windowHeight = (boothHeight - 2 * windowMargin) / windowRows;

    glColor4f(0.7f, 0.9f, 1.0f, 0.5f); // Light blue transparent glass
    for (int row = 0; row < windowRows; ++row) {
        for (int col = 0; col < windowCols; ++col) {
            float wx1 = x - boothWidth/2 + windowMargin + col * windowWidth;
            float wy1 = y + windowMargin + row * windowHeight;
            float wx2 = wx1 + windowWidth - frameThickness * 0.5f;
            float wy2 = wy1 + windowHeight - frameThickness * 0.5f;
            drawRectangle(wx1, wy1, wx2, wy2, 0.7f, 0.9f, 1.0f);
        }
    }

    // Window frames (thin dark red lines)
    glColor3f(0.5f, 0.0f, 0.0f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    // Vertical lines
    for (int col = 1; col < windowCols; ++col) {
        float xLine = x - boothWidth/2 + windowMargin + col * windowWidth;
        glVertex2f(xLine, y + windowMargin);
        glVertex2f(xLine, y + boothHeight - windowMargin);
    }
    // Horizontal lines
    for (int row = 1; row < windowRows; ++row) {
        float yLine = y + windowMargin + row * windowHeight;
        glVertex2f(x - boothWidth/2 + windowMargin, yLine);
        glVertex2f(x + boothWidth/2 - windowMargin, yLine);
    }
    glEnd();

    // Door handle (small gray rectangle on right side)
    float handleWidth = 0.02f * scale;
    float handleHeight = 0.06f * scale;
    float handleX1 = x + boothWidth/2 - frameThickness * 2;
    float handleY1 = y + boothHeight/2 - handleHeight/2;
    drawRectangle(handleX1 - handleWidth, handleY1, handleX1, handleY1 + handleHeight, 0.6f, 0.6f, 0.6f);

    // Lights on booth body (small yellow circles along left and right edges)
    float lightRadius = 0.008f * scale;
    int numLights = 8;
    float lightSpacing = (boothHeight - 2 * windowMargin) / (numLights - 1);
    for (int i = 0; i < numLights; ++i) {
        float lightY = y + windowMargin + i * lightSpacing;
        // Left side light
        drawFilledCircleS3(x - boothWidth/2 + frameThickness/2, lightY, lightRadius, lightRadius, 20, 1.0f, 1.0f, 0.0f);
        // Right side light
        drawFilledCircleS3(x + boothWidth/2 - frameThickness/2, lightY, lightRadius, lightRadius, 20, 1.0f, 1.0f, 0.0f);
    }

    // ===========================
    // Curved roof (semi-circle)
    // ===========================
    int segments = 40;  // Smoothness of the curve
    float roofRadius = boothWidth / 2.0f; // Width matches booth
    float roofCenterX = x;
    float roofCenterY = y + boothHeight; // Top of booth

    // Filled curved roof
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.8f, 0.0f, 0.0f); // Red like booth
    glVertex2f(roofCenterX, roofCenterY); // Base center
    for (int i = 0; i <= segments; i++) {
        float angle = M_PI * i / segments; // 0 → PI
        float vx = roofCenterX + cos(angle) * roofRadius;
        float vy = roofCenterY + sin(angle) * roofRadius;
        glVertex2f(vx, vy);
    }
    glEnd();

    // Dark red outline for roof
    glColor3f(0.5f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = M_PI * i / segments;
        float vx = roofCenterX + cos(angle) * roofRadius;
        float vy = roofCenterY + sin(angle) * roofRadius;
        glVertex2f(vx, vy);
    }
    glEnd();

    }
    void drawTree2(float x, float y, float scale, float trunkR, float trunkG, float trunkB, float leavesR, float leavesG, float leavesB) {

      // --- Trunk ---
    float trunkWidth = 0.03f * scale;
    float trunkHeight = 0.08f * scale;
    drawRectangle(x - trunkWidth / 2, y, x + trunkWidth / 2, y + trunkHeight,
                  trunkR, trunkG, trunkB);

    // --- Leaves (pine layers) ---
    int numLayers = 4;
    float layerHeight = 0.05f * scale;
    float layerWidth = 0.12f * scale;

    for (int i = 0; i < numLayers; i++) {
        float topY = y + trunkHeight + i * (layerHeight * 0.7f); // overlap layers
        float width = layerWidth * (1.0f - i * 0.15f);           // smaller as we go up
        float height = layerHeight * (1.0f - i * 0.1f);

        // --- Draw layer (green) ---
        glColor3f(0.0f, 0.5f, 0.0f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x, topY + height);         // top point
            glVertex2f(x - width / 2, topY);      // bottom-left
            glVertex2f(x + width / 2, topY);      // bottom-right
        glEnd();

        // --- Light snow overlay (just small caps) ---
        glColor3f(1.0f, 1.0f, 1.0f);
        float snowW = width * 0.2f;   // small snow width
        float snowH = height * 0.15f; // small snow height

        // just one small snow blob on top of each triangle
        drawFilledCircleS3(x, topY + height - snowH / 2, snowW, snowH, 50, 1.0f, 1.0f, 1.0f);
    }
    }
    void updateSmoke(SmokeParticle smoke[], float baseX, float baseY, float scale) {

      for (int i = 0; i < NUM_SMOKE; ++i) {
        smoke[i].y += smoke[i].speedY;
        smoke[i].x += smoke[i].speedX;
        smoke[i].alpha -= 0.002f;
        if (smoke[i].alpha < 0.0f) smoke[i].alpha = 0.0f;

        if (smoke[i].y > baseY + 0.15f * scale || smoke[i].alpha <= 0.0f) {
            smoke[i].x = baseX + ((rand() % 100) / 5000.0f) - 0.01f;
            smoke[i].y = baseY + ((rand() % 100) / 5000.0f);
            smoke[i].size = 0.03f * scale + ((rand() % 10) / 500.0f);
            smoke[i].speedY = 0.001f + ((rand() % 10) / 10000.0f);
            smoke[i].speedX = ((rand() % 20) - 10) / 10000.0f;
            smoke[i].alpha = 0.7f + ((rand() % 30) / 100.0f);
        }
    }

}
    void initPenguins() {
    for (int i = 0; i < 2; ++i) {
        penguins[i].x = 2.2f + i * 0.3f; // Start off-screen right, spaced apart
        penguins[i].y = -0.06f;           // Base Y near lake highlight
        penguins[i].state = WALKING;
        penguins[i].jumpHeight = 0.0f;
        penguins[i].jumpSpeed = 0.015f;   // Jump speed
        penguins[i].jumpUp = true;
    }
    }


    void display(){
    glClear(GL_COLOR_BUFFER_BIT);  // Clear the screen
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);  // Set color to white (full opacity)
    glDisable(GL_BLEND);  // Turn off blending (you can enable it later when needed)
    glLineWidth(1.0f);  // Reset line thickness to default
    // Reset camera/view to default orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2.0, 2.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

        if (isNight) {
            drawRectangle(-2.0f, 0.3f, 2.0f, 1.0f, 0.05f, 0.05f, 0.1f);
            static float starPositions[100][2];
            static bool starsInitialized = false;
            if (!starsInitialized) {
                for (int i = 0; i < 100; ++i) {
                    starPositions[i][0] = -2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 4.0f));
                    starPositions[i][1] = 0.3f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 0.7f));
                }
                starsInitialized = true;
            }
            for (int i = 0; i < 100; ++i) {
                drawStar(starPositions[i][0], starPositions[i][1], 0.005f + (rand() % 5) * 0.001f);
            }
            drawMoon(0.8f, 0.75f, 0.12f);
        } else {
            drawRectangle(-2.0f, 0.3f, 2.0f, 1.0f, 0.6f, 0.9f, 1.0f);
            drawWinterSun(0.8f, 0.75f, 0.12f);
        }
        drawCloud(smallCloud1X, smallCloud1Y, 1.5f);
        drawCloud(smallCloud2X, smallCloud2Y, 1.5f);
        drawCloud(smallCloud3X, smallCloud3Y, 1.5f);
        drawCloud(smallCloud4X, smallCloud4Y, 1.5f);
        drawCloud(bigCloudX, bigCloudY, 1.5f);
        drawMountain(0.0f, -0.4f, 2.5f);
        float mountainX = 0.0f, mountainY = -0.4f, mountainScale = 2.5f;
        float peakY = mountainY + 0.45f * mountainScale;
        drawBangladeshFlag(mountainX, peakY, 0.25f);
        drawMountain(-1.8f, -0.4f, 2.5f); drawMountain(-1.4f, -0.4f, 2.5f); drawMountain(-1.0f, -0.4f, 2.2f);
        drawMountain(1.8f, -0.4f, 2.5f); drawMountain(1.4f, -0.4f, 2.5f); drawMountain(1.0f, -0.4f, 2.2f);
        drawRectangle(-2.0f,-1.0f,2.0f,0.3f,1.0f,1.0f,1.0f);
        drawSnowdrift(0.5f, 0.3f, 0.9f);
        //drawFilledCircleS3(0.6f,0.5f,0.2f,0.08f,100,1.0f,1.0f,0.9f,0.15f);
        drawCrystalLake(-1.68f,-0.2f);
        drawSnowdrift(1.2f, -0.7f, 2.5f); drawSnowdrift(-0.2f, -0.9f, 2.0f);
        drawSnowdrift(1.4f, 0.3f, 1.3f); drawSnowdrift(-1.2f, -0.6f, 1.4f);
        drawSnowdrift(1.6f, -0.3f, 1.6f); drawSnowdrift(0.0f, -0.2f, 0.8f);
        drawSnowdrift(-0.6f, 0.25f, 1.2f); drawSnowdrift(-1.5f, 0.25f, 1.0f);
        drawSnowdrift(-0.8f, -0.6f, 1.5f);
        drawRectangle(-2.0f,-0.8f,2.0f,-0.44f,0.2f,0.2f,0.2f);
        drawRectangle(-2.0f,-0.8f,2.0f,-0.78f,0.8f,0.8f,0.8f);
        drawRectangle(-2.0f,-0.46f,2.0f,-0.44f,0.8f,0.8f,0.8f);
        for(float i=-2.0f;i<=2.0f;i+=0.6f) drawRectangle(i,-0.64f,i+0.28f,-0.61f,1.0f,1.0f,0.0f);
        drawZebraCrossing(-0.36f, -0.48f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawZebraCrossing(-0.36f, -0.52f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawZebraCrossing(-0.36f, -0.56f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawZebraCrossing(-0.36f, -0.60f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawZebraCrossing(-0.36f, -0.64f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawZebraCrossing(-0.36f, -0.68f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawZebraCrossing(-0.36f, -0.72f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawZebraCrossing(-0.36f, -0.76f, 0.1f, 0.02f, 1.0f, 1.0f, 1.0f);
        drawTrafficLight(-0.28f, -0.42f, 1.0f);
        drawTree(-1.95f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree2(-1.80f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree(-1.65f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree2(-1.50f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree(-1.35f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenBench(-1.15f, -0.40f, 1.0f);
        drawStreetLamp(-1.01f, -0.44f, 1.0f);
        drawTree(-0.9f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenFence(1.6f, -0.44f, 8, 1.0f);
        drawBillboard(1.48f, -0.44f, 1.0f);
        drawWoodenFence(1.0f, -0.44f, 8, 1.0f);
        drawTree(0.88f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenFence(0.4f, -0.44f, 8, 1.0f);
        drawTree(0.28f, -0.44f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenFence(-0.2f, -0.44f, 8, 1.0f);
        drawWoodenFence(-0.8f, -0.44f, 8, 1.0f);
        drawCarB(redCarX, -0.73f, 1.0f,0.0f,0.0f);
        drawCarB(yellowCarX, -0.73f, 1.0f,1.0f,0.0f);
        drawCarT(whiteCarX, -0.55f, 1.0f,1.0f,1.0f);
        drawCarT(blueCarX, -0.55f, 0.0f,0.0f,1.0f);
       drawRealisticBus1(busX, -0.72f, 1.0f);
        drawRealisticBus2(purpleBusX, -0.54f, 1.0f, 0.8f, 0.6f, 0.9f);
        drawSnowdrift(-1.9f, -0.89f, 0.7f);
        drawTree(-1.92f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree2(-1.76f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenFence(-1.65f, -0.85f, 8, 1.0f);
        drawSnowdrift(-1.1f, -0.89f, 0.7f);
        drawTree(-1.18f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree2(-1.02f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f,1.0f, 1.0f);
        drawWoodenFence(-0.9f, -0.85f, 8, 1.0f);
        drawSnowdrift(-0.1f, -0.89f, 0.7f);
        drawTree(-0.48f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree2(-0.20f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenFence(-0.13f, -0.85f, 8, 1.0f);
        drawSnowdrift(0.9f, -0.89f, 0.7f);
        drawTree(0.35f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree2(0.52f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenFence(0.64f, -0.85f, 8, 1.0f);
        drawChristmasTree(0.8f, -0.95f, 0.3f);
        drawWoodenBench(1.17f, -0.81f, 1.0f);
        drawStreetLamp(1.33f, -0.85f, 1.0f);
        drawSnowdrift(1.7f, -0.89f, 0.7f);
        drawTree(1.45f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree2(1.62f, -0.85f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawWoodenFence(1.75f, -0.85f, 5, 1.0f);
        drawSkiLiftWires();
        drawSkiLift(liftOneX, liftOneY);
        drawSkiLift(liftTwoX, liftTwoY);
        drawHouse01(1.65f, 0.0f, 0.4f); drawSmoke(house01Smoke); drawTree(1.85f, 0.0f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawHouse02(1.05f, 0.0f, 0.4f); drawSmoke(house02Smoke); drawTree(1.35f, 0.0f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawHouse03(0.45f, 0.0f, 0.4f); drawSmoke(house03Smoke); drawTree(0.68f, 0.0f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawHouse04(-1.5f, 0.18f, 0.3f); drawSmoke(house04Smoke); drawTree(-1.8f, 0.18f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree(-1.25f, 0.18f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawPhoneBooth(0.07f, 0.0f, 0.6f);
        drawWoodenBench(-0.2f, 0.05f, 1.0f);
        drawTree(-0.038f, 0.0f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        drawTree(-0.37f, 0.0f, 0.8f, 0.5f, 0.3f, 0.1f, 1.0f, 1.0f, 1.0f);
        for (int i = 0; i < 2; ++i) {
            Penguin &p = penguins[i];
            if (p.state != VANISHED) {
                drawPenguin(p.x, p.y + p.jumpHeight, 0.8f);
            }
        }
        drawChristmasTree(-0.6f, -0.33f, 0.6f);
        drawSnowman(-1.6f, -0.8f, 0.4f);
        drawSnowman(-0.33f, 0.3f, 0.3f);
        drawSnowman(1.9f, -0.28f, 0.4f);
        drawChristmasTree(1.7f, -0.35f, 0.2f);
        for (int i = 0; i < NUM_FLAKES; ++i) {
            drawSnowflake(snowflakes[i].x, snowflakes[i].y, snowflakes[i].size);
        }
    }

    void update(int value){
        smallCloud1X += 0.0007f; if(smallCloud1X>2.2f) smallCloud1X=-2.2f;
        smallCloud2X += 0.0007f; if(smallCloud2X>2.2f) smallCloud2X=-2.2f;
        smallCloud3X += 0.0005f; if(smallCloud3X>2.2f) smallCloud3X=-2.2f;
        smallCloud4X += 0.0003f; if(smallCloud4X>2.2f) smallCloud4X=-2.2f;
        bigCloudX += 0.0003f; if(bigCloudX>2.4f) bigCloudX=-2.6f;
        liftOneX += liftOneSpeed; if (liftOneX > 2.0f) { liftOneX = -2.0f; }
        liftTwoX += liftTwoSpeed; if (liftTwoX < -2.0f) { liftTwoX = 2.0f; }
        updateBillboardText();
        updateSmoke(house01Smoke, 1.65f - 0.4f * 0.4f, 0.0f + 0.7f * 0.4f, 0.4f);
        updateSmoke(house02Smoke, 1.05f + 0.1f * 0.4f, 0.05f + 0.7f * 0.4f, 0.4f);
        updateSmoke(house03Smoke, 0.45f + 0.35f * 0.4f, 0.0f + 0.7f * 0.4f, 0.4f);
        updateSmoke(house04Smoke, -1.5f + 0.35f * 0.3f, 0.18f + 0.85f * 0.3f, 0.3f);
        for (int i = 0; i < 2; ++i) {
            Penguin &p = penguins[i];
            switch (p.state) {
                case WALKING: p.x -= 0.004f; if (p.x < -1.4f) { p.state = JUMPING; p.jumpUp = true; p.jumpHeight = 0.0f; } break;
                case JUMPING: if (p.jumpUp) { p.jumpHeight += p.jumpSpeed; if (p.jumpHeight >= 0.15f) { p.jumpUp = false; } } else { p.jumpHeight -= p.jumpSpeed; if (p.jumpHeight <= 0.0f) { p.state = VANISHED; } } break;
                case VANISHED: p.x -= 0.004f; if (p.x < -2.5f) { p.x = 2.2f + i * 0.3f; p.state = WALKING; p.jumpHeight = 0.0f; p.jumpUp = true; } break;
            }
        }
        fishAngle += 0.02f; if(fishAngle>2*M_PI) fishAngle -= 2*M_PI;
        if(tailDirection) tailWiggle+=0.2f; else tailWiggle-=0.2f;
        if(tailWiggle>0.6f) tailDirection=false;
        if(tailWiggle<-0.6f) tailDirection=true;
        const float stopZoneStart = -0.65f, stopZoneEnd = -0.08f;
        auto inStopZone = [&](float x) { return (x >= stopZoneStart && x <= stopZoneEnd); };
        if (!((trafficLight == RED || trafficLight == YELLOW) && inStopZone(redCarX))) { redCarX += 0.004f; }
        if (redCarX > 2.2f) redCarX = -2.2f;
        if (!((trafficLight == RED || trafficLight == YELLOW) && inStopZone(yellowCarX))) { yellowCarX += 0.008f; }
        if (yellowCarX > 2.2f) yellowCarX = -2.2f;
        if (!((trafficLight == RED || trafficLight == YELLOW) && inStopZone(whiteCarX))) { whiteCarX -= 0.004f; }
        if (whiteCarX < -2.2f) whiteCarX = 2.2f;
        if (!((trafficLight == RED || trafficLight == YELLOW) && inStopZone(blueCarX))) { blueCarX -= 0.008f; }
        if (blueCarX < -2.2f) blueCarX = 2.2f;
        if (!((trafficLight == RED || trafficLight == YELLOW) && inStopZone(busX))) { busX += busSpeed; }
        if (busX > 2.2f) busX = -2.2f;
        if (!((trafficLight == RED || trafficLight == YELLOW) && inStopZone(purpleBusX))) { purpleBusX += purpleBusSpeed; }
        if (purpleBusX < -2.2f) purpleBusX = 2.2f;
        if (yellowToGreenPending) {
            yellowToGreenTimer++;
            if (yellowToGreenTimer >= yellowToGreenDuration) {
                trafficLight = GREEN;
                yellowToGreenPending = false;
                yellowToGreenTimer = 0;
            }
        }
        for (int i = 0; i < NUM_FLAKES; ++i) {
            snowflakes[i].y -= snowflakes[i].speed;
            snowflakes[i].x += sinf(snowflakes[i].y * 15.0f + i) * 0.00015f;
            if (snowflakes[i].y < -1.0f) {
                snowflakes[i].y = 1.0f;
                snowflakes[i].x = -2.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(4.0f)));
                snowflakes[i].size = 0.005f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/0.01f));
                snowflakes[i].speed = 0.002f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/0.004f));
            }
        }
        //glutPostRedisplay();
        //glutTimerFunc(30, update, 0);
    }




    void init() {
        glClearColor(0.6f, 0.9f, 1.0f, 1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        srand(time(NULL));
        initSnowflakes();
        initSmoke(house01Smoke, 1.65f - 0.4f * 0.4f, 0.0f + 0.7f * 0.4f, 0.4f);
        initSmoke(house02Smoke, 1.05f + 0.1f * 0.4f, 0.0f + 0.7f * 0.4f, 0.4f);
        initSmoke(house03Smoke, 0.45f + 0.35f * 0.4f, 0.0f + 0.7f * 0.4f, 0.4f);
        initSmoke(house04Smoke, -1.5f + 0.35f * 0.3f, 0.18f + 0.85f * 0.3f, 0.3f);
    }




    void keyboard(unsigned char key, int x, int y) {
        if (key == 'r' || key == 'R') { trafficLight = RED; yellowToGreenPending = false; yellowToGreenTimer = 0; }
        else if (key == 'g' || key == 'G') { trafficLight = GREEN; yellowToGreenPending = false; yellowToGreenTimer = 0; }
        else if (key == 'y' || key == 'Y') { trafficLight = YELLOW; yellowToGreenPending = true; yellowToGreenTimer = 0; }
        else if (key == 'n' || key == 'N') { isNight = true; }
        else if (key == 'd' || key == 'D') { isNight = false; }
        else if (key == 'j' || key == 'J') { isBell = true;
        #ifdef _WIN32
            if (isBell) {
                PlaySound(TEXT("jbell.wav"), NULL, SND_FILENAME | SND_ASYNC);
            } else {
                PlaySound(NULL, 0, 0);
            }
#endif

              }
    }
}



// =======================================================================
// |                     MASTER CONTROL FUNCTIONS                        |
// =======================================================================

void display() {
    // This master display function calls the correct scene's display function.
    // A single glutSwapBuffers() is used here for smooth, double-buffered animation.
    switch (currentScene) {
        case 1:
            Scene1::display();
            break;
        case 2:
            Scene2::display();
            break;
        case 3:
            // Scene 3's original code used single buffering with glFlush().
            // To make it compatible, we just call its display function.
            Scene3::display();
            break;
    }
    glutSwapBuffers();
}
void update(int value) {
    // Call the update function for the currently active scene.
    // Each scene's update function is responsible for setting its own next timer.
    switch (currentScene) {
        case 1:
            Scene1::update(value);
            break;
        case 2:
            Scene2::update(value);
            break;
        case 3:
            Scene3::update(value);
            break;
    }
    glutPostRedisplay();
     glutTimerFunc(16, update, 0);
}
/*void initPenguins() {
    for (int i = 0; i < 2; ++i) {
        penguins[i].x = 2.2f + i * 0.3f; // Start off-screen right, spaced apart
        penguins[i].y = -0.06f;           // Base Y near lake highlight
        penguins[i].state = WALKING;
        penguins[i].jumpHeight = 0.0f;
        penguins[i].jumpSpeed = 0.015f;   // Jump speed
        penguins[i].jumpUp = true;
    }
}*/
void keyboard(unsigned char key, int x, int y) {
    // Global keyboard controls for scene switching
    switch (key) {
        case 27: // The 'esc' key
            exit(0);
            break;
        case '1':
             #ifdef _WIN32
        //Stop any currently playing sound before changing scene
        PlaySound(NULL, 0, 0);
#endif
            currentScene = 1;
            cout << "Switched to Scene 1" << endl;

            break;
        case '2':
            #ifdef _WIN32
        //Stop any currently playing sound before changing scene
        PlaySound(NULL, 0, 0);
#endif
            currentScene = 2;
            cout << "Switched to Scene 2" << endl;
            break;
        case '3':
             #ifdef _WIN32
        //Stop any currently playing sound before changing scene
        PlaySound(NULL, 0, 0);
#endif
            currentScene = 3;
            cout << "Switched to Scene 3" << endl;
            break;
    }

    // Scene-specific keyboard controls
    if (currentScene == 1) {
        Scene1::keyboard(key, x, y);
    } else if (currentScene == 2) {
        Scene2::keyboard(key, x, y);
    } else if (currentScene == 3) {
        Scene3::keyboard(key, x, y);
    }
}

// =======================================================================
// |                        MAIN FUNCTION                                |
// =======================================================================

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Use double buffering for smooth animation
    glutInitWindowSize(1250, 625);
    glutInitWindowPosition(12, 12);
    glutCreateWindow("Train tunnel, desert, snowscape");

    // Initialize all scenes at the start
    cout << "Initializing Scene 1 (Tunnel)..." << endl;
    Scene1::initStars();
    Scene1::initRain();

    cout << "Initializing Scene 2 (Desert)..." << endl;
    Scene2::init();

    cout << "Initializing Scene 3 (Snowscape)..." << endl;
    Scene3::init();
    Scene3::initPenguins();

    // Set up the master callback functions
    glutDisplayFunc(display);
    glutTimerFunc(40, update, 0); // Start the update loop
    glutKeyboardFunc(keyboard);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-2.0, 2.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    cout << "\nCONTROLS\n";
    //cout << "GLOBAL:\n";
    cout << "  'Press 1,2,3' -> Switch to the next scene\n";
    cout << "  'ESC'      -> Exit Program\n";

    cout << "\n";
    cout << "Scene 1 Tunnel:\n";
    cout << "  's'/'S': Train Movement\n";
    cout << "  'n'/'N': Switch to Night View\n";
    cout << "  'd'/'D': Switch to Day View\n";
    cout << "  'r'/'R': Toggle Rain\n";

    cout << "\n";
    cout << "Scene 2 Desert:\n";
    cout << "  'p'/'P': Pause/Play Animation\n";
    cout << "  'z'/'Z': Zoom In\n";
    cout << "  'x'/'X': Zoom Out\n";
    cout << "  'n'/'N': Set to Night\n";
    cout << "  'd'/'D': Set to Day\n";

    cout << "\n";
    cout << "Scene 3 Snowscape:\n";
    cout << "  'r'/'R': Traffic Light RED\n";
    cout << "  'g'/'G': Traffic Light GREEN\n";
    cout << "  'y'/'Y': Traffic Light YELLOW\n";
    cout << "  'n'/'N': Switch to Night View\n";
    cout << "  'd'/'D': Switch to Day View\n";

    glutMainLoop();
    return 0;
}
