#ifndef INDICATOR_H__
#define INDICATOR_H__
#include <Shifty.h>
#include <Adafruit_NeoPixel.h>
#include "./../core.h"
using namespace Core;
enum GPIOType {
  WS_2811 = 0,
  HC_595,
  DIGITAL_PIN,
  ANALOG_PIN
};

enum Color {
  RED = 0xFF0000,
  GREEN = 0x00FF00,
  BLUE = 0x0000FF
};

#define NUMPIXELS           2
#define WS2812_DATA_PIN     26


#define HC_595_SH_DATA                    5
#define HC_595_ST_CP                      18
#define HC_595_SH_CP                      19
Adafruit_NeoPixel WS2811(NUMPIXELS, WS2812_DATA_PIN, NEO_GRB + NEO_KHZ800);

Shifty expander;
class OutputGPIO {
  int GPIO;
  byte type;
  uint32_t lastColor;
  uint8_t pwmChanel;
  uint8_t maxIntensity;
  static uint8_t PWMChanel;
  static bool initialized;
  static int count; 
  int id; 
  bool enabled;
  bool lastEnableStatus;
  int intensity;
  int blinkCount;
  IntervalReference blinkTracker;
  TimeoutReference blinkStopper;
  public:
    OutputGPIO(int GPIO, byte type) {
      this->enable();
      if (!OutputGPIO::initialized) {
        OutputGPIO::begin();
      }
      this->id = OutputGPIO::count++;
      this->maxIntensity = 100;
      this->attach(GPIO, type);
    }
    int getID() {
      return this->id;
    }

    void enable() {
      this->enabled = true;
    }

    void disable() {
      this->enabled = false;
      this->turnOff();
    }

    bool isEnabled() {
      return this->enabled;
    }

    void attach(int GPIO, byte type) {
      this->GPIO = GPIO;
      this->type = type;
      switch (this->type) {
        case GPIOType::ANALOG_PIN: {
          this->pwmChanel = OutputGPIO::PWMChanel;
          ledcAttachPin(this->GPIO, this->pwmChanel);
          OutputGPIO::PWMChanel ++;
          OutputGPIO::PWMChanel %= 16;
        } break;
        case GPIOType::DIGITAL_PIN: {
          pinMode(this->GPIO, OUTPUT);
        } break;
      }
      this->turnOff();
    }

    void show() {
      console.log(this->GPIO, this->type == GPIOType::HC_595 ? "595" : "other");
    }

    static void begin() {
      console.log("initializing gpios");
      expander.setBitCount(16);
      expander.setPins(HC_595_SH_DATA, HC_595_SH_CP, HC_595_ST_CP);
      WS2811.begin();
      WS2811.clear();
      WS2811.show();
      ledcSetup(0, 2000, 8);
      for (int i=0; i<16; i++) {
        expander.writeBit(i, LOW);
      }
      OutputGPIO::initialized = true;
    }

    void setIntensity(uint8_t intensity = 50) {
      if (!this->isEnabled()) {
        intensity = 0;
      }
      this->intensity = intensity;
      switch (this->type) {
        case GPIOType::WS_2811:
          WS2811.setPixelColor(this->GPIO, this->lastColor);
          WS2811.setBrightness(map(intensity, 0, 100, 0, 255));
          WS2811.show();
          break;
        case GPIOType::HC_595:
          expander.writeBit(this->GPIO, intensity >= 50 ? HIGH : LOW);
          break;
        case GPIOType::DIGITAL_PIN: digitalWrite(this->GPIO, map(intensity, 0, 100, LOW, HIGH));
          break;
        case GPIOType::ANALOG_PIN: ledcWrite(this->pwmChanel, map(intensity, 0, 100, 0, 255));          
          break;
        default: console.log("invlaid gpio type");
      }      
    }

    void setColor(uint32_t color) {
      WS2811.setPixelColor(this->GPIO, color);
      this->lastColor = color;
      this->setIntensity(this->maxIntensity);
    }

    bool isExpanderBit(int bit) {
      return bit >= 100 && bit < 200;
    }
    
    bool is2811Bit(int bit) {
      return bit >= 200;
    }

    void toggle(uint32_t color) {
      this->lastColor = color;
      this->intensity == 0 ? this->turnOn() : this->turnOff();
    }

    void turnOn() {
      this->setIntensity(this->maxIntensity);
    }

    void stopBlink() {
      this->turnOff();
      clearTimeout(this->blinkStopper);
      clearInterval(this->blinkTracker);
      if (!this->lastEnableStatus) {
        this->disable();
      }
    }

    void setMaxIntensity(int intensity) {
      this->maxIntensity = intensity;
    }

    void blink(uint32_t duration, int count = 1, bool forced = false) {
      this->lastEnableStatus = this->isEnabled();
      if (!this->lastEnableStatus) {
        if (forced) {
          this->enable();
        }
      }
      this->blinkCount = count;
      this->blinkTracker = setImmediate([this, duration]() {
        this->turnOn();
        this->blinkStopper = setTimeout([this]() {
          this->turnOff();
          this->blinkCount --;
          if (this->blinkCount <= 0) {
            this->stopBlink();
          }
        }, duration / 2);
      }, duration);
    }

    void turnOff() {
      this->setIntensity(0);
    }
    ~OutputGPIO() {
      // nothing to be done
    }
};
uint8_t OutputGPIO::PWMChanel = 0;
bool OutputGPIO::initialized = false;
int OutputGPIO::count = 0;
#endif