#ifndef DISPLAY_H__
#define DISPLAY_H__
#include <RGBmatrixPanel.h>

#define OE                         13
#define LAT                        23
#define CLK                        21
#define A                          27
#define B                          19
#define C                          14
#define PLUS_Button                16
#define MINUS_Button               17
#define RESET_Button               22

#define MATRIX_WIDTH               96
#define MATRIX_HEIGHT              32

namespace Display {
  String currentMessage = "";
  IntervalReference scroller;
  int margin = 2;
  int border = 2;
  int textPosition = 0;
  uint32_t scrollDelay = SECONDS(.5);
  RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, true, 3);
  void begin() {
    matrix.begin();
    matrix.fillScreen(0);
  }

  template<typename T>
  void displayBorder(int thickness, T color, int cutvalue = 0) {
    if (thickness <= cutvalue) {
      return;
    }
    int start = thickness - 1;
    matrix.drawRect(start, start, MATRIX_WIDTH - 2 * start, MATRIX_HEIGHT - 2 * start, color);
    return displayBorder(thickness - 2,  color);
  }

  void displayText(String text) {
    matrix.setTextSize(3);
    matrix.setCursor(Display::textPosition, 6);
    matrix.print(text);
  }

  void displayMargin() {
    Display::displayBorder(Display::border + Display::margin, matrix.Color333(0, 0, 0), Display::border);
  }

  void displayTextMarginBorder() {
    Display::displayText(Display::currentMessage);
    Display::displayMargin();
    Display::displayBorder(Display::border, matrix.Color333(7, 0, 0));
  }

  void resetPosition() {
    Display::textPosition = Display::margin + Display::border;
  }

  void scrollIfNeeded() {
    if (Display::currentMessage.length() > MATRIX_WIDTH - 2 * Display::border - 2 * Display::margin) {
      scroller = setInterval([]() {
        Display::textPosition --;
        if (Display::textPosition < Display::border + Display::margin - 15 * Display::currentMessage.length()) {
          Display::resetPosition();
        }
        Display::displayTextMarginBorder();
      }, Display::scrollDelay);
    }
  }

  void display(String message) {
    clearInterval(scroller);
    Display::currentMessage = message;
    Display::resetPosition();
    Display::displayTextMarginBorder();
    Display::scrollIfNeeded();
  }
};
#endif