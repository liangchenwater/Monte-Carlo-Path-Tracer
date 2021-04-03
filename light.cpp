#include <OpenGL/gl.h>

#include "light.h"

// GLCanvas====================================================================
// Given below are the bodies of the glInit function for the
// DirectionalLight and PointLight subclasses.
// ====================================================================

// ====================================================================
// Create an OpenGL directional light source with the appropriate
// color and position 
// ====================================================================

void DirectionalLight::glInit(int id) {
  GLenum glLightID;
  switch(id) {
    case 0 : glLightID = GL_LIGHT0; break;
    case 1 : glLightID = GL_LIGHT1; break;
    case 2 : glLightID = GL_LIGHT2; break;
    case 3 : glLightID = GL_LIGHT3; break;
    case 4 : glLightID = GL_LIGHT4; break;
    case 5 : glLightID = GL_LIGHT5; break;
    case 6 : glLightID = GL_LIGHT6; break;
    case 7 : glLightID = GL_LIGHT7; break;
    default: {
      printf ("WARNING!  Too many lights for GL to imitate\n");
      return;
    }
  }

  // Set the last component of the position to 0 to indicate
  // a directional light source
  GLfloat glPosition[4];
  glPosition[0] = -direction.x();
  glPosition[1] = -direction.y();
  glPosition[2] = -direction.z();
  glPosition[3] = 0.0;

  GLfloat glColor[4];
  glColor[0] = color.r();
  glColor[1] = color.g();
  glColor[2] = color.b();
  glColor[3] = 1.0;

  GLfloat zero[4] = {0,0,0,1};

  glLightfv(glLightID, GL_POSITION, glPosition);
  glLightfv(glLightID, GL_DIFFUSE, glColor);
  glLightfv(glLightID, GL_SPECULAR, glColor);
  glLightfv(glLightID, GL_AMBIENT, zero);
  glEnable(glLightID);
}

// ====================================================================
// Create an OpenGL point light source with the appropriate
// color and position 
// ====================================================================

void PointLight::glInit(int id) {
  GLenum glLightID;
  switch(id) {
    case 0 : glLightID = GL_LIGHT0; break;
    case 1 : glLightID = GL_LIGHT1; break;
    case 2 : glLightID = GL_LIGHT2; break;
    case 3 : glLightID = GL_LIGHT3; break;
    case 4 : glLightID = GL_LIGHT4; break;
    case 5 : glLightID = GL_LIGHT5; break;
    case 6 : glLightID = GL_LIGHT6; break;
    case 7 : glLightID = GL_LIGHT7; break;
    default: {
      printf ("WARNING!  Too many lights for GL to imitate\n");
      return;
    }
  }

  // Set the last component of the position to 1 to indicate
  // a point light source
  GLfloat glPosition[4];
  glPosition[0] = position.x();
  glPosition[1] = position.y();
  glPosition[2] = position.z();
  glPosition[3] = 1.0;

  GLfloat glColor[4];
  glColor[0] = color.r();
  glColor[1] = color.g();
  glColor[2] = color.b();
  glColor[3] = 1.0;

  GLfloat zero[4] = {0,0,0,1};

  glLightfv(glLightID, GL_POSITION, glPosition);
  glLightfv(glLightID, GL_DIFFUSE, glColor);
  glLightfv(glLightID, GL_SPECULAR, glColor);
  glLightfv(glLightID, GL_AMBIENT, zero);
  glEnable(glLightID);
}

// ====================================================================
// ====================================================================

void AreaLight::glInit(int id) {
  GLenum glLightID;
  switch(id) {
    case 0 : glLightID = GL_LIGHT0; break;
    case 1 : glLightID = GL_LIGHT1; break;
    case 2 : glLightID = GL_LIGHT2; break;
    case 3 : glLightID = GL_LIGHT3; break;
    case 4 : glLightID = GL_LIGHT4; break;
    case 5 : glLightID = GL_LIGHT5; break;
    case 6 : glLightID = GL_LIGHT6; break;
    case 7 : glLightID = GL_LIGHT7; break;
    default: {
      printf ("WARNING!  Too many lights for GL to imitate\n");
      return;
    }
  }
  // assume the position is the barycenter of the triangle area light
    Vec3f a=shape->a,b=shape->b,c=shape->c;
    Vec3f position=1.0/3*(a+b+c);
  // Set the last component of the position to 1 to indicate
  // a point light source
  GLfloat glPosition[4];
  glPosition[0] = position.x();
  glPosition[1] = position.y();
  glPosition[2] = position.z();
  glPosition[3] = 1.0;

  GLfloat glColor[4];
  glColor[0] = color.r();
  glColor[1] = color.g();
  glColor[2] = color.b();
  glColor[3] = 1.0;

  GLfloat zero[4] = {0,0,0,1};

  glLightfv(glLightID, GL_POSITION, glPosition);
  glLightfv(glLightID, GL_DIFFUSE, glColor);
  glLightfv(glLightID, GL_SPECULAR, glColor);
  glLightfv(glLightID, GL_AMBIENT, zero);
  glEnable(glLightID);
}

void EnvLight::glInit(int id) {
  GLenum glLightID;
  switch(id) {
    case 0 : glLightID = GL_LIGHT0; break;
    case 1 : glLightID = GL_LIGHT1; break;
    case 2 : glLightID = GL_LIGHT2; break;
    case 3 : glLightID = GL_LIGHT3; break;
    case 4 : glLightID = GL_LIGHT4; break;
    case 5 : glLightID = GL_LIGHT5; break;
    case 6 : glLightID = GL_LIGHT6; break;
    case 7 : glLightID = GL_LIGHT7; break;
    default: {
      printf ("WARNING!  Too many lights for GL to imitate\n");
      return;
    }
  }
  // assume the position is the barycenter of the triangle area ligh
    Vec3f position(0,0,0);
  // Set the last component of the position to 1 to indicate
  // a point light source
  GLfloat glPosition[4];
  glPosition[0] = position.x();
  glPosition[1] = position.y();
  glPosition[2] = position.z();
  glPosition[3] = 1.0;

  GLfloat glColor[4];
    glColor[0] = 1.0;
    glColor[1] = 1.0;
    glColor[2] = 1.0;
  glColor[3] = 1.0;

  GLfloat zero[4] = {0,0,0,1};

  glLightfv(glLightID, GL_POSITION, glPosition);
  glLightfv(glLightID, GL_DIFFUSE, glColor);
  glLightfv(glLightID, GL_SPECULAR, glColor);
  glLightfv(glLightID, GL_AMBIENT, zero);
  glEnable(glLightID);
}
