#include "core/core.h"
#include "core/database.h"
#include "data-source.h"
#include "config.h"
#include "display.h"
void setup() {  
  Serial.begin(115200);

  Display::begin();
  DataSource::begin();
}

void loop() {
  Core::loop();
}
