// Firmware for the LED Driver Board
// Current version 1.04 - Changes history:
// December 19, 2023 - ver. 1.01: Turned LEDs OFF before shuting off power
// December 20, 2023 - ver. 1.02: Added support for modulating the LEDs voltage
// Decemebr 22, 2023 - ver. 1.03: Corrected voltage scale and fixed power on/off sequence
// January 1, 2024 - ver. 1.04: Nick further fixed on/off sequence, including in 'post' routine, also enable 3v3 trigger supply

#include <stdint.h>
#include <string>
#include "ERC_common.hpp"

uint32_t active_driver = 0;
uint32_t all_drivers = 0;

const uint32_t pin_SPI_CS1_SWDIO     =  PA13          ;
const uint32_t pin_SPI_CS2_SWCLK     =  PA14          ;
const uint32_t pin_LED_BIAS_CTRL4    =  PA15          ;
const uint32_t pin_LED_IMON2         =  PA0           ;
const uint32_t pin_LED_IMON0         =  PA1           ;
const uint32_t pin_LED_IMON7         =  PA2           ;
const uint32_t pin_LED_IMON5         =  PA3           ;
const uint32_t pin_LED_IMON6         =  PA4           ;
const uint32_t pin_LED_IMON3         =  PA5           ;
const uint32_t pin_LED_IMON1         =  PA6           ;
const uint32_t pin_LED_IMON4         =  PA7           ;
const uint32_t pin_LOCAL_TRIGGER     =  PA8           ;
//const uint32_t pin_UART_RX           =  PA9           ;
const uint32_t pin_V_IN_MON          =  PB0           ;
const uint32_t pin_I_IN_MON          =  PB1           ;
const uint32_t pin_LED_PSU_CTRL      =  PB10          ;
const uint32_t pin_SPI_CS0           =  PB12          ;
const uint32_t pin_SPI_SCK           =  PB13          ;
const uint32_t pin_SPI_MISO          =  PB14          ;
const uint32_t pin_SPI_MOSI          =  PB15          ;
const uint32_t pin_FAULT_N           =  PB2           ;
const uint32_t pin_LED_BIAS_CTRL1    =  PB3           ;
const uint32_t pin_LED_BIAS_CTRL3    =  PB4           ;
const uint32_t pin_LED_BIAS_CTRL6    =  PB5           ;
const uint32_t pin_LED_BIAS_CTRL5    =  PB6           ;
const uint32_t pin_LED_BIAS_CTRL7    =  PB7           ;
const uint32_t pin_LED_BIAS_CTRL0    =  PB8           ;
const uint32_t pin_LED_BIAS_CTRL2    =  PB9           ;
const uint32_t pin_EN_3V3_ANALOG     =  PC13          ;
const uint32_t pin_EN_3V3_TRIGGER    =  PC14          ;
const uint32_t pin_EN_BIAS           =  PC15          ;


//uint8_t rx_buffer[1000];
//uint8_t tx_buffer[1000];

void print_log(std::string file, unsigned int line, unsigned int log_tag, std::string message)
{
    // Filter based on tag here if needed or with grep on log_tag
    std::string pmessage = file + ":" + std::to_string(line) + " (log_tag" + std::to_string(log_tag) + ") " + message + "\n";
    SerialUSB.write(pmessage.c_str(), pmessage.length());
    SerialUSB.flush();
}

class LED_driver_t {
  private:

    uint32_t bias_pin;
    uint32_t imon_pin;
    uint32_t bias;

  public:

    LED_driver_t() {
      bias_pin = 0;
      imon_pin = 0;
      bias     = 0;
    }

    void set_bias_pin(uint32_t bias_pin_ID) {
      bias_pin = bias_pin_ID;
      pinMode(bias_pin, OUTPUT);

      return;
    }

    void set_imon_pin(uint32_t imon_pin_ID) {
      imon_pin = imon_pin_ID;

      return;
    }

    uint32_t imon(void) {

      return (analogRead(imon_pin))/4;
    }

    void bias_abs(uint32_t val) {
      bias = (val % 256);
      analogWrite(bias_pin, bias);

      return;
    }

    void bias_rel(int32_t val) {
      bias += val;
      bias = bias % 256;
      analogWrite(bias_pin, bias);

      return;
    }

};

LED_driver_t LED_drivers[8];

// HardwareSerial Serial1(PA10, PA9); // (RX, TX pins)

void setup() {
  SerialUSB.begin(115200);
  delay(2000);
  SerialUSB.println("Hello from the LED driver board!");
  SerialUSB.println("Ver. 1.04 with MIDAS LPC support");
  
  pinMode(pin_EN_3V3_ANALOG, OUTPUT);
  pinMode(pin_EN_3V3_TRIGGER, OUTPUT);
  pinMode(pin_EN_BIAS, OUTPUT);
  pinMode(pin_LED_PSU_CTRL, OUTPUT);
  
  LED_drivers[0].set_bias_pin(pin_LED_BIAS_CTRL0);
  LED_drivers[0].set_imon_pin(pin_LED_IMON0);
  LED_drivers[0].bias_abs(0);
  
  LED_drivers[1].set_bias_pin(pin_LED_BIAS_CTRL1);
  LED_drivers[1].set_imon_pin(pin_LED_IMON1);
  LED_drivers[1].bias_abs(0);
  
  LED_drivers[2].set_bias_pin(pin_LED_BIAS_CTRL2);
  LED_drivers[2].set_imon_pin(pin_LED_IMON2);
  LED_drivers[2].bias_abs(0);
  
  LED_drivers[3].set_bias_pin(pin_LED_BIAS_CTRL3);
  LED_drivers[3].set_imon_pin(pin_LED_IMON3);
  LED_drivers[3].bias_abs(0);
  
  LED_drivers[4].set_bias_pin(pin_LED_BIAS_CTRL4);
  LED_drivers[4].set_imon_pin(pin_LED_IMON4);
  LED_drivers[4].bias_abs(0);
  
  LED_drivers[5].set_bias_pin(pin_LED_BIAS_CTRL5);
  LED_drivers[5].set_imon_pin(pin_LED_IMON5);
  LED_drivers[5].bias_abs(0);
  
  LED_drivers[6].set_bias_pin(pin_LED_BIAS_CTRL6);
  LED_drivers[6].set_imon_pin(pin_LED_IMON6);
  LED_drivers[6].bias_abs(0);
  
  LED_drivers[7].set_bias_pin(pin_LED_BIAS_CTRL7);
  LED_drivers[7].set_imon_pin(pin_LED_IMON7);
  LED_drivers[7].bias_abs(0);
  
  return;
}

void loop() {
  std::string input = "";
  std::string temp = "";
  float temp_f = 0.0;
  char c = 0;
  bool debug_flag = false;
  
  while(1) {
    if(SerialUSB.available() > 0) {
      c = SerialUSB.read(); // remove the terminator from the buffer
      if((c == '\n') || (c == '\r') || (c == '\0')) {
        if (debug_flag) {
          SerialUSB.print("Received command: ");
          SerialUSB.println(input.c_str());
        }
        if ((input == "select 0") || (input == "L1")) {
          active_driver = 0;
          all_drivers = 0;
          if (input == "L1") {
            SerialUSB.print("\r\nL1 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 0");
          }
        }
        else if ((input == "select 1") || (input == "L2")) {
          active_driver = 1;
          all_drivers = 0;
          if (input == "L2") {
            SerialUSB.print("\r\nL2 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 1");
          }
        }
        else if ((input == "select 2") || (input == "L3")) {
          active_driver = 2;
          all_drivers = 0;
          if (input == "L3") {
            SerialUSB.print("\r\nL3 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 2");
          }
        }
        else if ((input == "select 3") || (input == "L4")) {
          active_driver = 3;
          all_drivers = 0;
          if (input == "L4") {
            SerialUSB.print("\r\nL4 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 3");
          }
        }
        else if ((input == "select 4") || (input == "L5")) {
          active_driver = 4;
          all_drivers = 0;
          if (input == "L5") {
            SerialUSB.print("\r\nL5 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 4");
          }
        }
        else if ((input == "select 5") || (input == "L6")) {
          active_driver = 5;
          all_drivers = 0;
          if (input == "L6") {
            SerialUSB.print("\r\nL6 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 5");
          }
        }
        else if ((input == "select 6") || (input == "L7")) {
          active_driver = 6;
          all_drivers = 0;
          if (input == "L7") {
            SerialUSB.print("\r\nL7 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 6");
          }
        }
        else if ((input == "select 7") || (input == "L8")) {
          active_driver = 7;
          all_drivers = 0;
          if (input == "L8") {
            SerialUSB.print("\r\nL8 selected\r\n");
          }
          else {
            SerialUSB.println("Have selected active driver 7");
          }
        }
        else if ((input == "select all") || (input == "L9")) {
          active_driver = 0;
          all_drivers = 1;
          if (input == "L9") {
            SerialUSB.print("\r\nAll LEDs selected\r\n");
          }
          else {
            SerialUSB.println("All drivers have been activated");
          }
        }
        else if ((input == "power on") || (input == "E")) {
          digitalWrite(pin_EN_3V3_ANALOG, HIGH);
          digitalWrite(pin_EN_3V3_TRIGGER, HIGH);
          delay(100);
          digitalWrite(pin_EN_BIAS, HIGH);
          analogWrite(pin_LED_PSU_CTRL, (uint32_t)(80)); // set LED voltage to 12 V
          delay(100);
          if (input == "E") {
            SerialUSB.print("\r\nBoard Active\r\n");
          }
          else {
            SerialUSB.println("Board power is ON");
          }
        }
        else if ((input == "power off") || (input == "D")) {
          for (uint32_t i = 0; i < 8; i++) {
            LED_drivers[i].bias_abs(0);
          }
          digitalWrite(pin_LED_PSU_CTRL, LOW);
          digitalWrite(pin_EN_BIAS, LOW);
          delay(100);
          digitalWrite(pin_EN_3V3_ANALOG, LOW);
          digitalWrite(pin_EN_3V3_TRIGGER, LOW);
          delay(100);
          if (input == "D") {
            SerialUSB.print("\r\nBoard Standby\r\n");
          }
          else {
            SerialUSB.println("Board power is OFF");
          }
        }
        else if (input == "debug on") {
          debug_flag = true;
          SerialUSB.println("Debug reply set ON");
        }
        else if (input == "debug off") {
          debug_flag = false;
          SerialUSB.println("Debug reply set OFF");
        }
        else if (input == "post") {
          digitalWrite(pin_EN_3V3_ANALOG, HIGH);
          delay(100);
          digitalWrite(pin_EN_BIAS, HIGH);
          analogWrite(pin_LED_PSU_CTRL, (uint32_t)(80)); // set LED voltage to 12 V
          delay(100);
          for (uint32_t i = 0; i < 8; i++) {
            for (uint32_t j = 0; j < 255; j += 15) {
              LED_drivers[i].bias_abs(j);
              delay(1000);
              temp = "driver "+std::to_string(i)+" to "+std::to_string(j)+" IMON = " + std::to_string(LED_drivers[i].imon());
              SerialUSB.println(temp.c_str());
            }
            for (uint32_t j = 255; j > 0; j -= 15) {
              LED_drivers[i].bias_abs(j);
              delay(1000);
              temp = "driver "+std::to_string(i)+" to "+std::to_string(j)+" IMON = " + std::to_string(LED_drivers[i].imon());
              SerialUSB.println(temp.c_str());
            }
          }
        }
        else if (input == "L0") {
          for (uint32_t i = 0; i < 8; i++) {
            active_driver = i;
            LED_drivers[active_driver].bias_abs(0);
          }
          SerialUSB.print("\r\nLEDs off\r\n");
        }
        else if (input == "zero") {
          if (all_drivers) {
            for (uint32_t i = 0; i < 8; i++) {
              active_driver = i;
              LED_drivers[active_driver].bias_abs(0);
              SerialUSB.print("Brightness set to zero for LED ");
              SerialUSB.println(active_driver);
            }
          }
          else {
            LED_drivers[active_driver].bias_abs(0);
            SerialUSB.println("LED brightness set to zero");
          }
        }
        else if (input == "full") {
          if (all_drivers) {
            for (uint32_t i = 0; i < 8; i++) {
              active_driver = i;
              LED_drivers[active_driver].bias_abs(254);
              SerialUSB.print("Brightness set to maximum for LED ");
              SerialUSB.println(active_driver);
            }
          }
          else {
            LED_drivers[active_driver].bias_abs(254);
            SerialUSB.println("LED brightness set to maximum");
          }
        }
        else if (input == "up") {
          if (all_drivers) {
            for (uint32_t i = 0; i < 8; i++) {
              active_driver = i;
              LED_drivers[active_driver].bias_rel(16);
              SerialUSB.print("Increasing brightness by about 6.25% for LED ");
              SerialUSB.println(active_driver);
            }
          }
          else {
            LED_drivers[active_driver].bias_rel(16);
            SerialUSB.println("Increasing LED brightness by about 6.25%");
          }
        }
        else if (input == "down") {
          if (all_drivers) {
            for (uint32_t i = 0; i < 8; i++) {
              active_driver = i;
              LED_drivers[active_driver].bias_rel(-16);
              SerialUSB.print("Decreasing brightness by about 6.25% for LED ");
              SerialUSB.println(active_driver);
            }
          }
          else {
            LED_drivers[active_driver].bias_rel(-16);
            SerialUSB.println("Decreasing LED brightness by about 6.25%");
          }
        }
        else if (input == "Q") {
          SerialUSB.print("\n\rBias ADC = ");
          SerialUSB.print(LED_drivers[active_driver].imon() * 16);
          SerialUSB.print("\r\n");
        }
        else if (input == "imon") {
          if (all_drivers) {
            for (uint32_t i = 0; i < 8; i++) {
              active_driver = i;
              SerialUSB.print("Current value for LED  ");
              SerialUSB.print(active_driver);
              SerialUSB.print(" is ");
              SerialUSB.println(LED_drivers[active_driver].imon());
            }
          }
          else {
            SerialUSB.print("Current value is ");
            SerialUSB.println(LED_drivers[active_driver].imon());
          }
        }
        else if (input == "TE") {
          SerialUSB.print("\r\nTrigger Source: Set External\r\n");
        }
        else if (input == "TI") {
          SerialUSB.print("\r\nTrigger Source: Set Internal\r\n");
        }
        else if (input == "RS") {
          SerialUSB.print("\r\nTrigger Rate: Set 1.5kHz\r\n");
        }
        else if (input == "RF") {
          SerialUSB.print("\r\nTrigger Rate: Set 8MHz\r\n");
        }
        else if ((input.length() == 5) && (input.substr(0, 1) == "S")) {
          if (all_drivers) {
            for (uint32_t i = 0; i < 8; i++) {
              active_driver = i;
              LED_drivers[active_driver].bias_abs(stoul(input.substr(1, 4)) / 4);
            }
          }
          else {
            LED_drivers[active_driver].bias_abs(stoul(input.substr(1, 4)) / 4);
          }
          SerialUSB.print("\r\nBias DAC Set\r\n");
        }
        else if ((input.length() >= 18) && (input.substr(0, 15) == "set voltage at ")) {
          temp_f = std::stof(input.substr(15, (input.length() - 15))); // get the floating point number of volts at LED supply
          temp_f *= -0.2128;
          temp_f += 3.598; // calculate the floating point value of the control voltage
          if (temp_f < 0.0) {
            digitalWrite(pin_LED_PSU_CTRL, LOW);
            SerialUSB.println("LEDs voltage control set to maximum");
          }
          else if (temp_f > 3.3) {
            digitalWrite(pin_LED_PSU_CTRL, HIGH);
            SerialUSB.println("LEDs voltage control set to minimum");
          }
          else {
            analogWrite(pin_LED_PSU_CTRL, ((uint32_t)(temp_f * 77.5757)) % 256);
            SerialUSB.print("LEDs voltage control set to ");
            SerialUSB.print(temp_f);
            SerialUSB.println(" V");
          }
        }
        else if ((input == "help") || (input == "?")) {
          std::string temp = "\
          help OR ?\r\n\
          select [0 .. 7] OR all\r\n\
          zero\r\n\
          full\r\n\
          up\r\n\
          down\r\n\
          imon\r\n\
          power on\r\n\
          power off\r\n\
          set voltage at <float>\r\n\
          debug on\r\n\
          debug off\r\n\
          post\r\n";
          SerialUSB.write(temp.c_str());
        }
        else {
          SerialUSB.write("Unrecognized command: ");
          SerialUSB.println(input.c_str());
        }
        input = "";
      }
      else {
        if (c >= 0) {
          input += c;
        }
      }
    }
    else {
      delay(20);
    }
  } // code never exits this loop

  return;
}
