#include "core/core.h"
#include "data-source.h"
#include "config.h"
void setup() {  
  Serial.begin(115200);
  DataSource::begin();
}

void loop() {
  Core::loop();
}
