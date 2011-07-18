#include "testApp.h"

#define VISION_WIDTH  640
#define VISION_HEIGHT 480
//--------------------------------------------------------------
void testApp::setup(){
	settings.load();
	settings.add("mode", mode, AMBIENTLAPSE);
	settings.add("mirror", mirror, false);
	
	numPixels = VISION_WIDTH*VISION_HEIGHT*3;
	ofSetFrameRate(30);
	vidGrabber.setVerbose(true);
	vidGrabber.initGrabber(VISION_WIDTH,VISION_HEIGHT);

	img.allocate(VISION_WIDTH,VISION_HEIGHT, OF_IMAGE_COLOR);

	delayBuffer = new unsigned char *[DELAYSIZE];
	for(int i = 0; i < DELAYSIZE; i++) {
		delayBuffer[i] = new unsigned char[numPixels];
		/*for(int j = 0; j < ofGetWidth()*ofGetHeight(); j++) {
			delayBuffer[i][j] = 0;
		}*/
	}
	pix = new unsigned char[numPixels];
}

int r(int c) {
	return (c>>16)&0xFF;
}

int g(int c) {
	return (c>>8)&0xFF;
}

int b(int c) {
	return c&0xFF;
}

int ratio(int l, int r, float ratio) {
	float f = ((float)l)*ratio + ((float)r)*(1.f - ratio);
	return (int) f;
}

int bufferPos = 0;
void testApp::delayVideo(unsigned char *pixels) {
	memcpy(delayBuffer[bufferPos], pixels, numPixels);
	bufferPos++;
	if(bufferPos>=DELAYSIZE) {
		bufferPos = 0;
	}
}

int getDelayIndex(int pos) {
	return (DELAYSIZE + bufferPos - pos)%DELAYSIZE;
}

float feedback = 0.01;
	
void testApp::ambientLapse(unsigned char *pixels) {
	for (int i = 0; i < numPixels; i++) { // For each pixel in the video frame...
		pixels[i] = ratio(pixels[i], delayBuffer[0][i], feedback);
		delayBuffer[0][i] = pixels[i];
	}
}
void testApp::randomDelay(unsigned char *pixels) {
	delayVideo(pixels);
	int MAXDELAY = min(DELAYSIZE-1, 20);
	memcpy(pixels, delayBuffer[getDelayIndex(floor(ofRandom(0, MAXDELAY)))], numPixels);
}


int row = 0;

void testApp::slitScan(unsigned char *pixels) {
	for(int i = row*VISION_WIDTH*3; i < (row+5)*VISION_WIDTH*3 && i < numPixels; i++) {
		delayBuffer[0][i] = pixels[i];
	}
	memcpy(pixels, delayBuffer[0], numPixels);
	row+=5;
	if(row>=VISION_HEIGHT) row = 0;
}

void testApp::randomSlits(unsigned char *pixels) {
	delayVideo(pixels);
	int MAXDELAY = MIN(DELAYSIZE-1, 5);
	for(int y = 0; y < VISION_HEIGHT - 1; y++) {
		unsigned char *d = delayBuffer[getDelayIndex(floor(ofRandom(0, MAXDELAY)))];
		for(int x = y*VISION_WIDTH*3; x < (y+1)*VISION_WIDTH*3; x++) {
			pixels[x] = d[x];
		}
	}
}

void testApp::diagonalDelay(unsigned char *pixels) {
	delayVideo(pixels);
	for(int y = 0; y < VISION_HEIGHT - 1; y++) {
		unsigned char *d = delayBuffer[
				getDelayIndex((int)ofMap(y, 0, VISION_WIDTH-1, 0, DELAYSIZE-1))
		];
		
		for(int x = y*VISION_WIDTH*3; x < (y+1)*VISION_WIDTH*3; x++) {
			pixels[x] = d[x];
		}
	}
}


//--------------------------------------------------------------
void testApp::update(){
	//ofBackground(100,100,100);

    bool bNewFrame = false;

       vidGrabber.update();
	   bNewFrame = vidGrabber.isFrameNew();
  

	if (bNewFrame){

	    memcpy(pix, vidGrabber.getPixels(), numPixels);
		switch(mode) {
			case SLITSCAN:
				slitScan(pix);
				break;
			case AMBIENTLAPSE:
				ambientLapse(pix);
				break;
			case RANDOMDELAY:
				randomDelay(pix);
				break;
				
			case DIAGONALDELAY:
				diagonalDelay(pix);
				break;
			case RANDOMSLITS:
				randomSlits(pix);
				break;
		}
		img.setFromPixels(pix, VISION_WIDTH, VISION_HEIGHT,OF_IMAGE_COLOR);
		

		
		img.update();
	}
	
	
}

//--------------------------------------------------------------
void testApp::draw(){


	ofSetHexColor(0xffffff);
	ofFill();
	if(mirror) {
		img.draw(ofGetWidth(),0, -ofGetWidth(), ofGetHeight());
	} else {
		img.draw(0,0, ofGetWidth(), ofGetHeight());
	}
	// then draw the contours:
	
	//ofDrawBitmapString("Framerate: "+ofToString(ofGetFrameRate(), 4), 20, 20);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	switch(key) {
		case '1':
		case '2': 
		case '3': 
		case '4':
		case '5':
			mode = key;
			break;
		case OF_KEY_RIGHT:
			mode++;
			if(mode>'5') mode = '1';
			break;
		case OF_KEY_LEFT:
			mode--;
			if(mode<'1') mode = '5';

			break;
		case 'f':
		case 'F':
			ofToggleFullscreen();
			break;
		default:
			mirror = !mirror;
			break;
	}
	settings.save();
}
