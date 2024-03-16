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
  for (auto source: DataSource::sources) {
    console.log(*source);
  }
}

void loop() {
  Core::loop();
}
