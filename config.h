#ifndef CONFIG_H__
#define CONFIG_H__
#include <vector>
#include <utility>
#include "board.h"
namespace Configuration {
  typedef std::pair<int, String> GPIOMessagePair;
  std::vector<GPIOMessagePair> gpioMessagePair = {
    { M1_KEY, MESSAGE_1},
    { M2_KEY, MESSAGE_2},
    { M3_KEY, MESSAGE_3},
    { M4_KEY, MESSAGE_4},
    { M5_KEY, MESSAGE_5},
    { M6_KEY, MESSAGE_6},
    { M7_KEY, MESSAGE_7},
    { M8_KEY, MESSAGE_8}
  };
  std::vector<String> fallbackMessages = {
    ROUTINE_MESSAGE_1,
    ROUTINE_MESSAGE_2,
    ROUTINE_MESSAGE_3
  };
};
#endif