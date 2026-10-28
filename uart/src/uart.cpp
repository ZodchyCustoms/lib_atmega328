#include "uart.h"


uint16_t Uart::_baudRateValue;
// for transmit:
char Uart::_txBuffer[TX_BUFFER_SIZE] = {'\0'};
char Uart::_numbers[NUMBERS_BUFFER_SIZE + 1] = {'\0'}; // size + 1 for '\0' - last symbol
uint8_t Uart::_tx_r = 0;
uint8_t Uart::_tx_w = 0;
uint8_t Uart::_bytesToRead = 0;
bool Uart::_busy = false;
// for recieve:
char Uart::_rxBuffer[RX_BUFFER_SIZE] = {'\0'};
char Uart::_message[RX_BUFFER_SIZE + 1] = {'\0'};

uint8_t Uart::_rx_r = 0;
uint8_t Uart::_rx_w = 0;
uint8_t Uart::_bytesRecieved = 0;


Uart::Uart() {}

Uart::~Uart()
{
  UCSR0B &= ~(1 << TXEN0) | ~(1 << RXEN0);
}

void Uart::_startTransmit()
{
  if (!_busy && _bytesToRead > 0)
  {
    _busy = true;
    UCSR0B |= (1 << UDRIE0);
  }
}

char* Uart::_convertNumbers(int number)
{
  bool _minus = false;
  uint8_t i = NUMBERS_BUFFER_SIZE - 1;
  if (number < 0)
  {
    number *= -1;
    _minus = true;
  }
  if (number == 0)
  {
    _numbers[i--] = '0';
  }
  else
  {
    while (number)
    {
      _numbers[i--] = number % 10 + '0';
      number /= 10;
    }
  }
  if (_minus)
    _numbers[i--] = '-';
  return &_numbers[i + 1];
}

void Uart::_txBufferWrite(char byte)
{
  _txBuffer[_tx_w++] = byte;
  _bytesToRead++;
  if (_tx_w >= TX_BUFFER_SIZE)
  {
    _tx_w = 0;
    // TODO: add actions if OVF
  }
}

void Uart::_putInTxBuffer(const char *byte)
{
  while (*byte)
  {
    _txBufferWrite(*byte++);
  }
}

char* Uart::_readFromRxBuffer()
{
  uint8_t i = 0;
  while (_bytesRecieved > 0 && i < RX_BUFFER_SIZE)
  {
    _message[i++] = _rxBuffer[_rx_r++];
    if (_rx_r == RX_BUFFER_SIZE)
      _rx_r = 0;
    _bytesRecieved--;
  }
  _message[i] = '\0';
  return _message;
}

void Uart::print(const char *message)
{
  _putInTxBuffer(message);
  _startTransmit();
}

void Uart::print(const char ch)
{
  //_putInTxBuffer(&ch);
  _txBufferWrite(ch);
  _startTransmit();
}

void Uart::print(int number)
{
  _putInTxBuffer(_convertNumbers(number));
  _startTransmit();
}

bool Uart::available()
{
  return _bytesRecieved;
}

void Uart::read()
{
  print(_readFromRxBuffer());
}

void Uart::rxEnable()
{
  UCSR0B |= (1 << RXCIE0);
}

void Uart::init(uint32_t baudRate)
{
  uint8_t p = 16;
  if (baudRate >= 115200)
  {
    p = 8;
    UCSR0A |= (1 << U2X0);
  }
  _baudRateValue = F_CPU / p / baudRate - 1;
  UBRR0H = _baudRateValue >> 8;
  UBRR0L = _baudRateValue;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << USBS0) | (3 << UCSZ00);
  sei();
}


void Uart::uartWriteInUDR0()
{
  if (_bytesToRead > 0)
  {
    UDR0 = _txBuffer[_tx_r++];
    if (_tx_r == TX_BUFFER_SIZE)
      _tx_r = 0;
    _bytesToRead--;
  } else
  {
    UCSR0B &= ~(1 << UDRIE0);
    _busy = false;
  }
}

void Uart::uartReadFromUDR0()
{
  _rxBuffer[_rx_w++] = UDR0;
  _bytesRecieved++;
  if (_rx_w == RX_BUFFER_SIZE)
  {
    _rx_w = 0;
    // TO DO: actions if got OVF
  }
}


ISR(USART_UDRE_vect)
{
  Uart::uartWriteInUDR0();
}

ISR(USART_RX_vect)
{
  Uart::uartReadFromUDR0();
}

