#include "core/core.h"
#include "core/database.h"
#include "data-source.h"
#include "core/gpio.h"
#include "config.h"
#include "display.h"
void setup() {  
  Serial.begin(115200);

  Display::begin();
  DataSource::begin();
  GPIOs::begin();
  setInterval([]() { 
    for (auto source: DataSource::sources) {
      source->show();
    }
  }, 500);
}

void loop() {
  Core::loop();
}
