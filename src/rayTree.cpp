#include "rayTree.h"
#include <OpenGL/gl.h>

// ====================================================================
// Initialize the static variables
int RayTree::activated = 0;  
Segment RayTree::main_segment;
SegmentVector RayTree::shadow_segments;
SegmentVector RayTree::reflected_segments;
SegmentVector RayTree::transmitted_segments;

// ====================================================================

void RayTree::Print() {
  main_segment.Print("main       ");
  int i;
  for (int i = 0; i < shadow_segments.getNumSegments(); i++) {
    shadow_segments.getSegment(i).Print("shadow     "); }
  for (int i = 0; i < reflected_segments.getNumSegments(); i++) {
    reflected_segments.getSegment(i).Print("reflected  "); }
  for (int i = 0; i < transmitted_segments.getNumSegments(); i++) {
    transmitted_segments.getSegment(i).Print("transmitted"); }
}

void RayTree::paintHelper(const Vec4f &m,const Vec4f &s,const Vec4f &r,const Vec4f &t) {
  glBegin(GL_LINES);
  glColor4f(m.r(),m.g(),m.b(),m.a());
  main_segment.paint();
  int i;
  glColor4f(s.r(),s.g(),s.b(),s.a());
  for (int i = 0; i < shadow_segments.getNumSegments(); i++) {
    shadow_segments.getSegment(i).paint(); }
  glColor4f(r.r(),r.g(),r.b(),r.a());
  for (int i = 0; i < reflected_segments.getNumSegments(); i++) {
    reflected_segments.getSegment(i).paint(); }
  glColor4f(t.r(),t.g(),t.b(),t.a());
  for (int i = 0; i < transmitted_segments.getNumSegments(); i++) {
    transmitted_segments.getSegment(i).paint(); }  
  glEnd();
}

// ====================================================================

void RayTree::paint() {

  glLineWidth(2);
  glDisable(GL_LIGHTING);

  // this allows you to see rays passing through objects
  // turn off the depth test and blend with the current pixel color
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_DST_COLOR);
  paintHelper(Vec4f(0.5,0.5,0.5,0.4),
	      Vec4f(0.1,0.9,0.1,0.4),
	      Vec4f(0.9,0.1,0.1,0.4),
	      Vec4f(0.1,0.1,0.9,0.4));
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  // with the depth test enabled, draw the lines solid
  paintHelper(Vec4f(0.5,0.5,0.5,1.0),
	      Vec4f(0.1,0.9,0.1,1.0),
	      Vec4f(0.9,0.1,0.1,1.0),
	      Vec4f(0.1,0.1,0.9,1.0));
	      
  glEnable(GL_LIGHTING);
}

// ====================================================================
