#include <string>
#include <iostream>
#include "RenderSystem.h"
#include "../OpenGL.h"
#include "../Components/Shape.h"
#include "../Components/Line.h"
#include "../Components/Health.h"
#include "../Components/HealthBar.h"
#include "../Components/Particle.h"
#include "../GameModel.h"
#include "../Helpers.h"
#include "../Quaternion.h"

void RenderSystem::update(EntityManager &entities, double dt) {
    drawDifficulty();
    switch (gameModel.state) {
        case GameState::START:
            renderString(0, 0, "Press SPACE BAR to start...", TextAlignment::CENTER);
            renderString(0, -10, "(Press 1 or 2 to switch difficulty modes)", TextAlignment::CENTER);
            break;
        case GameState::GAME_OVER:
        case GameState::PLAY_AGAIN:
            renderString(0, 0, "Game Over. Press SPACE BAR to play again...", TextAlignment::CENTER);
            renderString(0, -10, "(Press 1 or 2 to switch difficulty modes)", TextAlignment::CENTER);
            break;
        case GameState::WAVE_OVER:
        case GameState::PLAYING:
            drawScore();
            drawEntities(entities);
            break;
    }
}

void RenderSystem::drawEntities(EntityManager &entities) {
    for (Entity* entity: entities.getEntitiesWith<Transform, Texture>()) {
        Transform *transform = entity->get<Transform>();
        Texture *texture = entity->get<Texture>();

        if (entity->has<Health, HealthBar>()) {
            drawHealthBars(entity);
        }

        glPushMatrix();
            glTranslatef(transform->position.x, transform->position.y, 0);
//            glRotatef(2 * transform->rotation, 0, 0, 1);
            glScalef(transform->scale.x, transform->scale.y, transform->scale.z);

            Quaternion rotation = Quaternion::angleAxis(transform->rotation /2, Vec3(0, 0, 1));
            std::cout << transform->rotation << " degrees : " << rotation.toString() << std::endl;

            glMultMatrixd(rotation.toRotationMatrix());

            glColor3f(texture->red, texture->green, texture->blue);

//            if (entity->has<Shape>()) {
//                drawShape(entity);
//            } else if (entity->has<Line>()) {
//                drawLine(entity);
//            } else if (entity->has<Particle>()) {
//                drawParticle(entity);
//            }
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_POLYGON);                // Begin drawing the color cube with 6 quads
        // Top face (y = 1.0f)
        // Define vertices in counter-clockwise (CCW) order with normal pointing out
        glColor3f(0.0f, 1.0f, 0.0f);     // Green
        glVertex3f(1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);

// Bottom face (y = -1.0f)
        glColor3f(1.0f, 0.5f, 0.0f);     // Orange
        glVertex3f(1.0f, -1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);

// Front face  (z = 1.0f)
        glColor3f(1.0f, 0.0f, 0.0f);     // Red
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);

// Back face (z = -1.0f)
        glColor3f(1.0f, 1.0f, 0.0f);     // Yellow
        glVertex3f(1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(1.0f, 1.0f, -1.0f);

// Left face (x = -1.0f)
        glColor3f(0.0f, 0.0f, 1.0f);     // Blue
        glVertex3f(-1.0f, 1.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, -1.0f);
        glVertex3f(-1.0f, -1.0f, 1.0f);

// Right face (x = 1.0f)
        glColor3f(1.0f, 0.0f, 1.0f);     // Magenta
        glVertex3f(1.0f, 1.0f, -1.0f);
        glVertex3f(1.0f, 1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, -1.0f);
        glEnd();  // End of drawing color-cube

        glFlush();
        glPopMatrix();
    }
}

void RenderSystem::drawScore() {
    glColor3f(1, 1, 1);
    int arenaSize = gameModel.arenaSize;
    renderString(-arenaSize, arenaSize + 2, "Score: " + std::to_string(gameModel.score),
                 TextAlignment::LEFT);

    renderString(0, arenaSize + 2, "Wave: " + std::to_string(gameModel.waveCount),
                 TextAlignment::CENTER);

    renderString(arenaSize, arenaSize + 2, "Time: " + formatTime(gameModel.elapsedTime - gameModel.resetTime),
                 TextAlignment::RIGHT);
}

void
RenderSystem::renderString(GLdouble x, GLdouble y, const std::string &string,
                           TextAlignment alignment)
{
    double width = 0;
    for (int n = 0; n < string.size(); ++n) {
        width += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, string[n]);
    }

    width = width / gameModel.getWorldToPixelRatioWidth();

    double offset = 0;
    if (alignment == TextAlignment::CENTER) {
        offset = width / 2;
    } else if (alignment == TextAlignment::RIGHT) {
        offset = width;
    }

    glRasterPos2d(x - offset, y);
    for (int n = 0; n < string.size(); ++n) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[n]);
    }
}



void RenderSystem::drawParticle(Entity *entity) const {
    int size = entity->get<Particle>()->size;
    glPointSize(size);
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);
    glEnd();
}

void RenderSystem::drawLine(Entity *entity) const {
    Line *line = entity->get<Line>();
    glLineWidth(2.0);
    glBegin(GL_LINES);
    {
        glVertex3f(line->start.x, line->start.y, 0);
        glVertex3f(line->end.x, line->end.y, 0);
    }
    glEnd();
}

void RenderSystem::drawShape(Entity *entity) const {
    Shape *shape = entity->get<Shape>();

    glLineWidth(2.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
    {
        for(Vec3 vertex: shape->vertices) {
            glVertex3f(vertex.x, vertex.y, 0);
        }
    }
    glEnd();
}

void RenderSystem::drawHealthBars(Entity *entity) const {
    Transform *transform = entity->get<Transform>();

    glPushMatrix();
    glTranslatef(transform->position.x, transform->position.y, 0);
    glScalef(transform->scale.x, transform->scale.y, 1);

    int width = 10;
    Health *health = entity->get<Health>();
    double healthWidth = width * ((double) health->health / (double) health->maxHealth);

    glBegin(GL_POLYGON);
    {
        glColor3f(1, 1, 1);
        glVertex3f(-5, -0.2, 0);
        glVertex3f(5, -0.2, 0);
        glVertex3f(5, 0.2, 0);
        glVertex3f(-5, 0.2, 0);

        glColor3f(1, 0, 0);
        glVertex3f(-5, -0.2, 0);
        glVertex3f(healthWidth - 5, -0.2, 0);
        glVertex3f(healthWidth - 5, 0.2, 0);
        glVertex3f(-5, 0.2, 0);
    }
    glEnd();
    glPopMatrix();
}

void RenderSystem::drawDifficulty() {
    int arenaSize = gameModel.arenaSize;
    switch (gameModel.difficulty) {
        case Difficulty::HARD:
            glColor3f(1, 0, 0);
            return renderString(0, -(arenaSize + 6), "Difficulty: HARD", TextAlignment::CENTER);
        case Difficulty::EASY:
            glColor3f(1, 1, 1);
            return renderString(0, -(arenaSize + 6), "Difficulty: EASY", TextAlignment::CENTER);
    }
}

