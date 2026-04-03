#include "Password.h"
#include "1602_driver.h"

TaskHandle_t lcdTaskControl;
TaskHandle_t buttonTaskControl;

void TaskLcd(void *arg){
        gpio_set_level(brightnessPin, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        lcdSetText("Defuse Bomb", 3, 0);
        lcdSetText("_______", 5, 1);
        vTaskDelay(pdMS_TO_TICKS(1000));
        vTaskDelete(lcdTaskControl);
}


void ButtonMatrix(void *arg) {
  char columnNumber[3] = {columnOne, columnTwo, columnThree};
  char rowNumber[4] = {rowOne, rowTwo, rowThree, rowFour};

  char buttonStatus[3][4] = {
      {' ', 7, 4, 1},
      {0, 8, 5, 2},
      {'#', 9, 6, 3},
  };

  uint32_t tryCount = 0;
  bool rowStatus = false;

  double exp;
  int p;
  double password;

try:
  exp = 7;
  p = 0;
  password = 0;

  while (p < 7) {
    for (int i = 0; i < 3; i++) {
      gpio_set_level(columnNumber[i], 1);
      for (int j = 0; j < 4; j++) {
        rowStatus = gpio_get_level(rowNumber[j]);
        if (rowStatus) {
          lcdSetInt(buttonStatus[i][j], p+5, 1);

          password = password + (buttonStatus[i][j] * pow(10, (double)(exp-1)));
          exp--;

          p++;

          if (p>6) {
                vTaskDelay(pdMS_TO_TICKS(100));
                uint32_t correctPassword = 7355608;

                if (correctPassword == password) {
                        lcdClear();
                        vTaskDelay(pdMS_TO_TICKS(500));
                        lcdSetText("Bomb has been", 1, 0);
                        lcdSetText("Defused", 4, 1);

                        while (1) {
                                gpio_set_level(beaperPin, 0);
                                gpio_set_level(greenLed, 1);

                                vTaskSuspend(displayTaskControl);
                                vTaskDelay(pdMS_TO_TICKS(1000));
                        }
                } else {
                        vTaskDelay(pdMS_TO_TICKS(100));
                        lcdSetText("_______", 5, 1);
                        
                        xTaskNotifyGive(displayTaskControl);
                        vTaskDelay(pdMS_TO_TICKS(500));

                        goto try;
                }
          }
        }
      }
      gpio_set_level(columnNumber[i], 0);
      vTaskDelay(pdMS_TO_TICKS(50));
    }
  }

  vTaskDelete(NULL);
}


