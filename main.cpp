//#include <Wire.h>
//#include <SPI.h>

#define BT 19
#define STRIKE 20
#include "WProgram.h"
#include "Arduino.h"
//#include "LiquidCrystalFast.h"
#include "macro_oscillator.h"
#include "envelope.h"
#include "names.h"
#include "Bounce.h"
#include "Adafruit_SSD1306.h"
#include "ui.h"


using namespace braids;

Bounce butt(BT, 10);
Bounce strikeButt(STRIKE, 10);

MacroOscillator osc;
Envelope env;
IntervalTimer myTimer;
MB::UI ui;

const uint32_t kSampleRate = 96000;
const uint16_t kAudioBlockSize = 28;

uint8_t sync_buffer[kAudioBlockSize];

int16_t bufferA[kAudioBlockSize];
int16_t bufferB[kAudioBlockSize];
uint8_t buffer_sel;

uint8_t buffer_index;
volatile uint8_t wait;


uint8_t send_counter;

uint8_t selector, force;
uint8_t current_encoder;

int16_t shape = 0;
int16_t disp_shape;
int16_t color;
int16_t disp_color;
int16_t timbre;
int16_t disp_timbre;

int16_t attack;
int16_t disp_attack;
int16_t decay;
int16_t disp_decay;
int16_t sustain;
int16_t disp_sustain;
int16_t release;
int16_t disp_release;


volatile uint8_t done;


void old_ui() {

	
	if (butt.update() || strikeButt.update()) {
		if (strikeButt.fallingEdge()) {

			
			osc.Strike();
			env.Trigger(ENV_SEGMENT_ATTACK);
		//displayStuff();
		}

		if(strikeButt.risingEdge()){
			
			env.Trigger(ENV_SEGMENT_DEAD);
		}

		if (butt.fallingEdge()) {
			shape++;
			
		shape = shape >= MACRO_OSC_SHAPE_DIGITAL_MODULATION ? MACRO_OSC_SHAPE_DIGITAL_MODULATION : shape < 0 ? 0 : shape;

		// Sets the shape
		MacroOscillatorShape osc_shape = static_cast<MacroOscillatorShape>(shape);//
		osc.set_shape(osc_shape);
		
			//MacroOscillatorShape osc_shape = static_cast<MacroOscillatorShape>(shape);//
			//osc.set_shape(osc_shape);

		}
	}
}


// Timer interruption to put the following sample
void putSample(void) {
	
	if (!done) {
//		midi_event = ~midi_event;
		//digitalWriteFast(13, midi_event);
	}

	uint16_t val;
	if (buffer_sel)
		val = ((uint16_t)(bufferB[buffer_index] + 0x7000)) >> 4;
	else
		val = ((uint16_t)(bufferA[buffer_index] + 0x7000)) >> 4;

	buffer_index = buffer_index + 1;
	analogWrite(A14, val >> 2);
	if (buffer_index >= kAudioBlockSize) {
		wait = 0;
		buffer_index = 0;
		done = 1;
		buffer_sel = ~buffer_sel;
	}

}

int16_t pitch, pre_pitch;


extern "C" int main(void)
{
	
	pinMode(BT, INPUT_PULLUP);
	pinMode(STRIKE, INPUT_PULLUP);


	// Initalizes the buffers to zero
	memset(bufferA, 0, kAudioBlockSize);
	memset(bufferB, 0, kAudioBlockSize);

	// Global used to trigger the next buffer to render
	wait = 0;

	// Initializes the objects
	osc.Init();
	env.Init();
	osc.set_shape(MACRO_OSC_SHAPE_CSAW);
	osc.set_parameters(0, 0);
	myTimer.begin(putSample, 1e6 / 96000.0);

	pinMode(13, OUTPUT);
	pinMode(23, OUTPUT);
	analogWriteResolution(12);

	pitch = 44 << 7;


	
	// Loop
	while (1) {

		old_ui();
		// Set the pin to 1 to mark the begining of the render cycle
		digitalWriteFast(23, HIGH);
		// Clears the render buffer
		memset(sync_buffer, 0, sizeof(sync_buffer));
		// If the pitch changes update it
		if (pre_pitch != pitch) {
			osc.set_pitch(pitch);
			pre_pitch = pitch;
		}
		// Get the timbre and color parameters from the old_ui and set them
		timbre = analogRead(A0) * 16;
		color = analogRead(A1) * 16;
		osc.set_pitch(analogRead(A3) * 16);
		decay = analogRead(A2)  *16;

		
		env.Update(1, decay);
		uint32_t ad_value = env.Render();
		osc.set_parameters(timbre, color);
		


		if (buffer_sel) {
			osc.Render(sync_buffer, bufferA, kAudioBlockSize);
			//for(int i=0;i<kAudioBlockSize;i++){
			//    uint16_t ad_value = env.Render();
			//    int32_t product = static_cast<int32_t>(ad_value) * bufferA[i];
			//    bufferA[i] = product>>16;
			//}
		}
		else {
			osc.Render(sync_buffer, bufferB, kAudioBlockSize);
			//for(int i=0;i<kAudioBlockSize;i++){
			//    uint16_t ad_value = env.Render();
			//    int32_t product = static_cast<int32_t>(ad_value) * bufferB[i];
			//    bufferB[i] = product >> 16 ;
			//}
		}
		// Process the buttons and screen
		ui.update();
		ui.draw();
		// Reads the midi data
		//	usbMIDI.read();
		// Set the pin low to mark the end of rendering and processing
		digitalWriteFast(23, LOW);
		
		// Waits until the buffer is ready to render again
		wait = 1;
		while (wait){
		};

		done = 0;
	}

}


