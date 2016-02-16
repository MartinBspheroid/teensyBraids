#include "Adafruit_SSD1306.h"

enum uiParams {
	TIMBRE,
	COLOR,
	ATTACK,
	DECAY
};
namespace MB {
class UI
{
public:
	UI(){};
	~UI(){};
	void init() {
		display.setResetPin(5);
		display.begin(SSD1306_SWITCHCAPVCC, 0x3C);     // initialize with the I2C addr 0x3C (for the 128x64)
		display.clearDisplay();   // clears the screen and buffer

		display.display();
	}
	void draw(){};
	void update() {
		//readADC

	}

	uint16_t getParam(const uint8_t param) {
		return mParams[param];
	}
private:
	Adafruit_SSD1306 display;
	uint16_t mParams[4];
	uint8_t adcIndex;
	uint8_t displayIndex;

};
}