#include "Adafruit_SSD1306.h"

enum uiParams{
	TIMBRE,
	COLOR,
	ATTACK,
	DECAY
}

class ui
{
public:
	ui();
	~ui();
	void init();
	void display();
	void update(){
		//readADC

	}

	uint16_t getParam(const uint8_t param){
		return mParams[param];
	}
private:

	uint16_t mParams[4];
	uint8_t adcIndex;
	uint8_t displayIndex;

};