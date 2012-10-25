#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class StarbucksSearchRTreeApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void StarbucksSearchRTreeApp::setup()
{
}

void StarbucksSearchRTreeApp::mouseDown( MouseEvent event )
{
}

void StarbucksSearchRTreeApp::update()
{
}

void StarbucksSearchRTreeApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_BASIC( StarbucksSearchRTreeApp, RendererGl )
