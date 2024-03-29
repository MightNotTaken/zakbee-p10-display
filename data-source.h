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
  DataSource_T(int pin, String _data) {
    this->data = _data;
    this->gpio = new InputGPIO(pin, INPUT_PULLDOWN);
  }

  void registerDS() {
    GPIOs::registerInput(this->gpio); 
  }

  void onEmergency(std::function<void(String)> callback) {
    this->gpio->onStateLow([this, callback]() {
      console.log("State low");
      callback(this->data);
    });
  }

  void onIssueResolve(std::function<void(String)> callback) {
    this->gpio->onStateHigh([this, callback]() {
      console.log("State high");
      callback(this->data);
    });
  }

  String toString() {
    String data = this->data + ":" + gpio->getGPIO();
    return data;
  }

  void show() {
    console.log("pin state of ", gpio->getGPIO(), "is", gpio->getCurrentState() ? "HIGH" : "LOW");
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
      console.log(gpio, message);
      DataSource_T* ds = new DataSource_T(gpio, message);
      ds->onEmergency([](String message) {
        console.log("Emergency message received from", message);
        if (std::find(emergencies.begin(), emergencies.end(), message) == emergencies.end()) {
          emergencies.push_back(message);
          DataSource::pauseRoutine();
          DataSource::startRoutine();
        }
      });
      ds->onIssueResolve([](String message) {
        console.log(message, "Emergency resolved");
        auto it = std::find(emergencies.begin(), emergencies.end(), message);
        if (it != emergencies.end()) {
          emergencies.erase(it);
          DataSource::pauseRoutine();
          DataSource::startRoutine();
        }
      });
      ds->registerDS();
      DataSource::sources.push_back(ds);
    }
    DataSource::startRoutine();
    console.log(Configuration::fallbackMessages);
  }

  void pauseRoutine() {
    clearInterval(DataSource::routineTracker);
    clearTimeout(DataSource::routineStarter);
  }

  void startRoutine() {
    DataSource::routineTracker = setImmediate([]() {
      if (!DataSource::emergencies.empty()) {
        Display::setColor(Display::Colors::RED);
        String frontMessage = DataSource::emergencies.front();
        DataSource::emergencies.erase(DataSource::emergencies.begin());
        DataSource::emergencies.push_back(frontMessage);
        Display::display(frontMessage.c_str());
        return;
      }
      if (!Configuration::fallbackMessages.empty()) {
        Display::setColor(Display::Colors::GREEN);
        String frontMessage = Configuration::fallbackMessages.front();
        Configuration::fallbackMessages.erase(Configuration::fallbackMessages.begin());
        Configuration::fallbackMessages.push_back(frontMessage);
        Display::display(frontMessage.c_str());
      }
    }, SECONDS(3));
  }
};
#endif