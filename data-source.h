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
public:
  DataSource_T(int pin, String data): data(data) {
    this->gpio = new InputGPIO(pin, INPUT_PULLDOWN);
    GPIOs::registerInput(this->gpio); 
  }

  void onEmergency(std::function<void(String)> callback) {
    this->gpio->onStateLow([this, callback]() {
      callback(this->data);
    });
  }

  void onIssueResolve(std::function<void(String)> callback) {
    this->gpio->onStateHigh([this, callback]() {
      callback(this->data);
    });
  }

  ~DataSource_T() {
    GPIOs::unregisterInput(this->gpio);
  }
};

namespace DataSource {
  std::vector<DataSource_T*> sources;
  std::vector<String> emergencies;

  IntervalReference routineTracker = NULL_REFERENCE;
  TimeoutReference routineStarter = NULL_REFERENCE;

  void pauseRoutine();
  void startRoutine();

  void begin() {
    for (auto [gpio, message]: Configuration::gpioMessagePair) {
      DataSource_T* ds = new DataSource_T(gpio, message);
      ds->onEmergency([](String message) {
        if (std::find(emergencies.begin(), emergencies.end(), message) == emergencies.end()) {
          emergencies.push_back(message);
          DataSource::pauseRoutine();
          DataSource::startRoutine();
        }
      });
      ds->onIssueResolve([](String message) {
        auto it = std::find(emergencies.begin(), emergencies.end(), message);
        if (it != emergencies.end()) {
          emergencies.erase(it);
          DataSource::pauseRoutine();
          DataSource::startRoutine();
        }
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
      if (!DataSource::emergencies.empty()) {
        Display::setColor(Display::Colors::RED);
        auto frontMessage = std::move(DataSource::emergencies.front());
        DataSource::emergencies.erase(DataSource::emergencies.begin());
        DataSource::emergencies.push_back(std::move(frontMessage));
        Display::display(frontMessage);
        return;
      }
      if (!Configuration::fallbackMessages.empty()) {
        Display::setColor(Display::Colors::GREEN);
        auto frontMessage = std::move(Configuration::fallbackMessages.front());
        Configuration::fallbackMessages.erase(Configuration::fallbackMessages.begin());
        Configuration::fallbackMessages.push_back(std::move(frontMessage));
        Display::display(frontMessage);
        console.log(frontMessage);
      }
    }, SECONDS(3));
  }
};
#endif