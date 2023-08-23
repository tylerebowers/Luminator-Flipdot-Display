#ifndef FLIPDOT_24X16_H
#define FLIPDOT_24X16_H

#include <SPI.h>
#include <Adafruit_GFX.h>

#define FLIPDOT_BLACK          0
#define FLIPDOT_YELLOW         1

#define FLIPDOT_PULSE_DURATION 500

#define FLIPDOT_MODULE_WIDTH   24
#define FLIPDOT_MODULE_HEIGHT  16
#define SPI_SIZE	7

class FlipDot_24x16 : public Adafruit_GFX {
	private:
		const int enablePin = 6;
		const int chipSelectPin = 10;
		unsigned int spiData[SPI_SIZE];
		unsigned int spiResponse[SPI_SIZE];
		boolean *imageBuffer = NULL;
		boolean *oldImageBuffer = NULL;
		uint16_t _pixelDelay = 0;
		int col2chipNo[24] =      { 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2};
		int col2chipGroup[24] =   { 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1};
		int col2chipCFbit[24] =   { 1, 2, 3, 4, 5, 6, 1, 2, 1, 2, 3, 4, 5, 6, 1, 2, 1, 2, 3, 4, 5, 6, 1, 2};
		int col2chipENbit[24] =   { 7, 8, 9,10,11,12, 7, 8, 7, 8, 9,10,11,12, 7, 8, 7, 8, 9,10,11,12, 7, 8};
		int row2chipNo[16] =      { 3, 3, 3, 3, 4, 4, 4, 4,  5, 5, 5, 5, 6, 6, 6, 6};
		int row2chipGroup[16] =   { 0, 0, 0, 1, 0, 0, 0, 1,  0, 0, 0, 1, 0, 0, 0, 1};
		int row2chipCFbitHI[16] = { 1, 3, 5, 1, 1, 3, 5, 1,  1, 3, 5, 1, 1, 3, 5, 1};
		int row2chipCFbitLO[16] = { 2, 4, 6, 2, 2, 4, 6, 2,  2, 4, 6, 2, 2, 4, 6, 2};
		int row2chipENbitHI[16] = { 7, 9,11, 7, 7, 9,11, 7,  7, 9,11, 7, 7, 9,11, 7};
		int row2chipENbitLO[16] = { 8,10,12, 8, 8,10,12, 8,  8,10,12, 8, 8,10,12, 8};
//		int hBridgeInit[2] = {0xA001, 0xE001}; // SRR=1, HBSEL=0/1, ULDSC=1
		int hBridgeInit[2] = {0x8001, 0xC001}; // SRR=1, HBSEL=0/1, ULDSC=0
		void displayPixel(int16_t, int16_t, boolean);
		void writeFrame(unsigned int*, unsigned int*, byte);

	public:
		void begin(void);
		void drawPixel(int16_t, int16_t, uint16_t);
		void display(void);
		void setPixelDelay(uint16_t);
		FlipDot_24x16();
};

void printData(unsigned int*, unsigned int*, byte);

#endif //FLIPDOT_24X16_H