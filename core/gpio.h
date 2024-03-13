#ifndef GPIO_H__
#define GPIO_H__
#include <vector>
#include "./GPIO/input.h"
#include "./GPIO/output.h"
#include "core.h"


namespace GPIOs {
  std::function<void()> beforeListenCallback;
  std::function<void()> afterListenCallback;
  struct Output {
    int pin;
    int type;
    Output(int pin, int type)
    : pin(pin), type(type) {}
  };
  struct InputOutputPair {
    int inputPin;
    Output* output;
    InputOutputPair(int inputPin, int outputPin, int outputType)
    : inputPin(inputPin) {
      this->output = new GPIOs::Output(outputPin, outputType);
    }
    ~InputOutputPair() {
      delete this->output;
    }
  };
  
  std::vector<InputGPIO*> inputs;

  void registerInput(InputGPIO* input) {
    GPIOs::inputs.push_back(input);
  }
  
  void unregisterInput(InputGPIO* input) {
    auto it = std::find(inputs.begin(), inputs.end(), input);
    if (it != inputs.end()) {
      inputs.erase(it);
    }
  }

  void beforeListen(std::function<void()> callback) {
    GPIOs::beforeListenCallback = callback;
  }

  void afterListen(std::function<void()> callback) {
    GPIOs::afterListenCallback = callback;
  }

  void begin() {
    setInterval([]() {
      invoke(GPIOs::beforeListenCallback);
      for (auto& input: GPIOs::inputs) {
        input->listen();
      }
      invoke(GPIOs::afterListenCallback);
    }, 50);
  }
};
#endif