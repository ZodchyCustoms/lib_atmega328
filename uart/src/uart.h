#pragma once
#include <stdint.h>
#include <avr/interrupt.h>

#define TX_BUFFER_SIZE 200
#define RX_BUFFER_SIZE 50
#define NUMBERS_BUFFER_SIZE 13

class Uart
{
public:
  Uart();
  ~Uart();

  static void init(uint32_t);
  static void print(const char*);
  static void print(const char);
  static void print(int);
  static bool available();
  static void read();
  static void rxEnable();

  // need to make a complite interrupt mode
  static void uartWriteInUDR0();
  static void uartReadFromUDR0();

private:
  static void _startTransmit();
  static void _putInTxBuffer(const char*);
  static char* _readFromRxBuffer();
  static void _txBufferWrite(char);
  static char* _convertNumbers(int);


  static char _txBuffer[TX_BUFFER_SIZE];
  static char _rxBuffer[RX_BUFFER_SIZE];
  static char _numbers[NUMBERS_BUFFER_SIZE + 1];
  static char _message[RX_BUFFER_SIZE + 1];
  static bool _busy;
  static uint8_t _tx_r, _tx_w, _rx_r, _rx_w, _bytesToRead, _bytesRecieved;
  static uint16_t _baudRateValue;

};


void _uartWriteInUDR0();
void _uartReadFromUDR0();

