
#include "model_platform_immediate.h"
#include "gl/glew.h"
#include "glm/glm.hpp"

void modelViewer::render::model_platform_immediate::draw() {
    
    const int linesCount = 11;
    const float lineLength = 20;
    const float lineSpace = 2;
    
    glm::vec3 startPos{-10,-1,10};
    glm::vec3 currentPos = startPos;
    glBegin(GL_LINES);
    glColor4f(0.5f,0.5f,0.5f,0.1f);
    
    for (int i = 0; i < linesCount; ++i) {
        
        glVertex3f(currentPos.x,currentPos.y, currentPos.z);
        glVertex3f(currentPos.x,currentPos.y, currentPos.z - lineLength);
        currentPos.x += lineSpace;
    }

    currentPos = startPos;
    for (int i = 0; i < linesCount; ++i) {

        glVertex3f(currentPos.x,currentPos.y, currentPos.z);
        glVertex3f(currentPos.x + lineLength,currentPos.y, currentPos.z);
        currentPos.z -= lineSpace;
    }


    glEnd();
}
