#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"


#define AMBIENTLAPSE	'1'
#define SLITSCAN		'2'
#define RANDOMDELAY		'3'
#define DIAGONALDELAY	'4'
#define RANDOMSLITS		'5'


#define DELAYSIZE		40
#include "Settings.h"


class testApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	
private:
	
	void delayVideo(unsigned char *pixels);
	void ambientLapse(unsigned char *pixels);
	void randomDelay(unsigned char *pixels);
	void slitScan(unsigned char *pixels);
	void randomSlits(unsigned char *pixels);
	void diagonalDelay(unsigned char *pixels);
	
	ofVideoGrabber 		vidGrabber;

	Settings settings;
	ofImage img;
	
	bool mirror;
	
	int mode;
	unsigned char *pix;
	unsigned char **delayBuffer;
	int numPixels;
	
};

#endif
