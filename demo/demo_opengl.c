//
// Created by v on 16-10-24.
//

#include <GL/glut.h>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POLYGON);
    glVertex2f(-0.5, -0.5);
    glVertex2f(-0.5, -0.5);
    glVertex2f(0.5, 0.5);
    glVertex2f(0.5, -0.5);
    glEnd();

    glFlush();
}

int glut_demo(int argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("draw Rect");
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

int main(int argc, char **argv)
{
    glut_demo(argc, argv);
    return 0;
}

