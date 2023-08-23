#include "FlipDot_24x16.h"

FlipDot_24x16::FlipDot_24x16() :
	Adafruit_GFX(FLIPDOT_MODULE_WIDTH, FLIPDOT_MODULE_HEIGHT) {
	imageBuffer = (boolean*) calloc(width()*height(), sizeof(boolean));
	oldImageBuffer = (boolean*) calloc(FLIPDOT_MODULE_WIDTH*height(), sizeof(boolean));
}

void FlipDot_24x16::begin() {
	SPI.begin();
	pinMode(enablePin, INPUT);
  	pinMode(chipSelectPin, OUTPUT);
 	digitalWrite(chipSelectPin, HIGH);
 	// initalize the  data ready and chip select pins:
    // Initialize all chips
	for(int j = 0; j<=1; j++) {	
		// initialize H-Bridges of current group
		for(int i = 0; i<SPI_SIZE; i++) {
			spiData[i] = hBridgeInit[j]; // SRR=1, HBSEL=0/1, ULDSC=1
		}
  		writeFrame(spiData, spiResponse, SPI_SIZE);
  	}
  	delay(100);
	fillScreen(FLIPDOT_YELLOW);
	display();
	fillScreen(FLIPDOT_BLACK);
	display();
	setTextColor(FLIPDOT_YELLOW, FLIPDOT_BLACK);
}

void FlipDot_24x16::displayPixel(int16_t x, int16_t y, boolean color) {
	// Display a single pixel
	//Serial.write(0x0c);
	if (x < FLIPDOT_MODULE_WIDTH && y < FLIPDOT_MODULE_HEIGHT) {
		// iterate through low and high group (0 and 1) of H-bridge chips
	  	for(int j = 0; j<=1; j++) {	  		
			// initialize H-Bridge data for current group
			for(int i = 0; i<SPI_SIZE; i++) {
				spiData[i] = hBridgeInit[j]; // SRR=1, HBSEL=0/1, ULDSC=1
	  		}
	  		// setting CF and EN bits of current group
	  		if(row2chipGroup[y] == j) {
  				if(color == FLIPDOT_YELLOW) {
  					spiData[row2chipNo[y]] += (1<<row2chipCFbitHI[y]);
  					spiData[row2chipNo[y]] += (1<<row2chipENbitHI[y]);
  				} else {
  					spiData[row2chipNo[y]] += (0<<row2chipCFbitLO[y]);
  					spiData[row2chipNo[y]] += (1<<row2chipENbitLO[y]);
  				}
  			}
  			if(col2chipGroup[x] == j) {
  				if(color == FLIPDOT_YELLOW) {
  					spiData[col2chipNo[x]] += (0<<col2chipCFbit[x]);
  					spiData[col2chipNo[x]] += (1<<col2chipENbit[x]);
  				} else {
  					spiData[col2chipNo[x]] += (1<<col2chipCFbit[x]);
  					spiData[col2chipNo[x]] += (1<<col2chipENbit[x]);  					
  				}  					
  			}
  			// write data for current group
			writeFrame(spiData, spiResponse, SPI_SIZE);
			//printData(spiData, spiResponse, SPI_SIZE);
  		}	  		
		// WAIT
		delayMicroseconds(FLIPDOT_PULSE_DURATION);
		// disable H-Bridges
  		for(int j = 0; j<=1; j++) {	  		
			// initialize H-Bridges of current group
			for(int i = 0; i<SPI_SIZE; i++) {
				spiData[i] = hBridgeInit[j]; // SRR=1, HBSEL=0/1, ULDSC=1
  			}
			writeFrame(spiData, spiResponse, SPI_SIZE);
			//printData(spiData, spiResponse, SPI_SIZE);
		}
		// WAIT
		delayMicroseconds(_pixelDelay);
	}
}

void FlipDot_24x16::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (x >= width() || y >= height()) return;
	imageBuffer[y*width()+x] = (color>0);
}

void FlipDot_24x16::setPixelDelay(uint16_t pixelDelay) {
	_pixelDelay = pixelDelay;
}

void FlipDot_24x16::display(void) {
	// Update entire display
	for (uint8_t x = 0; x < FLIPDOT_MODULE_WIDTH; x++) {
		for (uint8_t y = 0; y < FLIPDOT_MODULE_HEIGHT; y++) {
			if (oldImageBuffer[y*width()+x] == imageBuffer[y*width()+x]) continue; // don't update pixel if not different
			displayPixel(x,y,imageBuffer[y*width()+x]);
			oldImageBuffer[y*width()+x] = imageBuffer[y*width()+x];
		}
	}
}


//Sends a frame to the array of 7 NCV7719
void FlipDot_24x16::writeFrame(unsigned int dataToSend[], unsigned int response[], byte dataSize) {
// gain control of the SPI port
  // and configure settings
  SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE1));
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  // Write SPI data
  for(int i = 0; i<dataSize; i++) {
    response[i] = SPI.transfer16(dataToSend[i]);
  } 
  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
  // release control of the SPI port
  SPI.endTransaction();
}


void printData(unsigned int array1[], unsigned int array2[], byte arraySize) {
  for(int i = 0; i<arraySize; i++) {
    Serial.print("Sent: ");
    Serial.print(array1[i], HEX);
    Serial.print("\tResponse: ");
    Serial.println(array2[i], HEX);
  }
}