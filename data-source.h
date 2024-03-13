#ifndef DATA_SOURCE_H__
#define DATA_SOURCE_H__
#include "core/gpio.h"
#include "core/GPIO/input.h"
#include "config.h"
#include <map>
#include "display.h"

class DataSource_T {
  InputGPIO* gpio;
  String data;
  std::function<void()> dataChangeCallback;
public:
  DataSource_T(int pin, String data): data(data) {
    this->gpio = new InputGPIO(pin);
    this->gpio->onStateLow([this]() {
      Display::display(this->data);
    });   
    GPIOs::registerInput(this->gpio); 
  }

  void onDataChange(std::function<void()> callback) {
    this->dataChangeCallback = callback;
  }
  ~DataSource_T() {
    GPIOs::unregisterInput(this->gpio);
  }
};

namespace DataSource {
  std::vector<DataSource_T*> sources;

  IntervalReference routineTracker = NULL_REFERENCE;
  TimeoutReference routineStarter = NULL_REFERENCE;

  void pauseRoutine();
  void startRoutine();

  void begin() {
    for (auto [gpio, message]: Configuration::gpioMessagePair) {
      DataSource_T* ds = new DataSource_T(gpio, message);
      ds->onDataChange([]() {
        DataSource::pauseRoutine();
        DataSource::routineStarter = setTimeout([]() {
          DataSource::startRoutine();
        }, SECONDS(10));
      });
      DataSource::sources.push_back(ds);
    }
    DataSource::startRoutine();
  }

  void pauseRoutine() {
    clearInterval(DataSource::routineTracker);
    clearTimeout(DataSource::routineStarter);
  }

  void startRoutine() {
    DataSource::routineTracker = setImmediate([]() {
      if (!Configuration::fallbackMessages.empty()) {
        auto frontMessage = std::move(Configuration::fallbackMessages.front());
        Configuration::fallbackMessages.erase(Configuration::fallbackMessages.begin());
        Configuration::fallbackMessages.push_back(std::move(frontMessage));
        Display::display(frontMessage);
      }
    }, SECONDS(10));
  }
};
#endif