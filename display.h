#ifndef DISPLAY_H__
#define DISPLAY_H__
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include "ESP32-VirtualMatrixPanel-I2S-DMA.h"
#define PANEL_RES_X 32
#define PANEL_RES_Y 32
#define NUM_ROWS 1
#define NUM_COLS 3
#define SERPENT true
#define TOPDOWN false
namespace Display {
  MatrixPanel_I2S_DMA *dma_display = nullptr;
  VirtualMatrixPanel *FourScanPanel = nullptr;
  namespace Colors {
    uint16_t BLACK;
    uint16_t WHITE;
    uint16_t GREEN;
    uint16_t RED;
    uint16_t BLUE;

    void begin() {
      BLACK = dma_display->color565(0, 0, 0);
      WHITE = dma_display->color565(255, 255, 255);
      GREEN = dma_display->color565(255, 0, 0);
      RED = dma_display->color565(0, 255, 0);
      BLUE = dma_display->color565(0, 0, 255);
    }
  };

  bool begin() {
    HUB75_I2S_CFG mxconfig(
      PANEL_RES_X * 2,
      PANEL_RES_Y / 2,
      NUM_ROWS * NUM_COLS);
    mxconfig.clkphase = false;
    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->setBrightness8(40);
    if (not dma_display->begin()) {
      return false;
    }
    dma_display->clearScreen();
    Colors::begin();
    FourScanPanel = new VirtualMatrixPanel((*dma_display), NUM_ROWS, NUM_COLS, PANEL_RES_X, PANEL_RES_Y);
    FourScanPanel->setPhysicalPanelScanRate(FOUR_SCAN_32PX_HIGH);
    FourScanPanel->setTextSize(1);
    FourScanPanel->setTextWrap(true);
    return true;
  }
  void setColor(uint16_t color) {
    FourScanPanel->setTextColor(color);
  }
  void display(String message) {
    dma_display->clearScreen();
    FourScanPanel->setCursor(0, 0);
    FourScanPanel->print(message.c_str());
  }
};
#endif