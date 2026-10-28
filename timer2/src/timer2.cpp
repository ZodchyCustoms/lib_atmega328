#include "timer2.h"

Timer2 timer2;

Timer2::Timer2() {}
Timer2::~Timer2() {}

void Timer2::_resetRegs()
{
  TCCR2A = 0;
  TCCR2B = 0;
  TIMSK2 = 0;
  OCR2A = 0;
  OCR2B = 0;
  TCNT2 = 0;
  TIFR2 = 0;
}

void Timer2::_setWgmBits()
{
  TCCR2A |= _mode & 0b11;           // take WGM20 and WGM21 bits
  TCCR2B |= (_mode & 0b100) << 1;   // take WGM22 bit
}

void Timer2::_setPrescaller()
{
  TCCR2B &= ~( (1 << CS20) | (1 << CS21) | (1 << CS22) );
  TCCR2B |= 0b111 & _prescaller;
}

void Timer2::_init(Mode mode, uint8_t prescaller)
{
  if (prescaller > 7)
    prescaller = 7;
  _prescaller = prescaller;
  _mode = (uint8_t) mode;
  _resetRegs();
  _setWgmBits();
}

// default behaivor is non-inverting mode
void Timer2::_initPwm(uint8_t duty, Oc2x OC2x)
{
  uint8_t pin;
  switch (OC2x)
  {
    case Oc2x::A:
    TCCR2A &= ~( (1 << COM2A0) | (1 << COM2A1) );
    TCCR2A |= (1 << COM2A1);
    OCR2A = duty;
    pin = 11;
    break;
    case Oc2x::B:
    TCCR2A &= ~( (1 << COM2B0) | (1 << COM2B1) );
    TCCR2A |= (1 << COM2B1);
    OCR2B = duty;
    pin = 3;
    break;
    default:
    _initCorrect = false;
    _resetRegs();
    return;
  };
  setPinOutput(pin);
}

// #################### PUBLIC METHODS ####################

// prescaller - number of description
void Timer2::init(Mode mode, uint8_t prescaller)
{
  if (mode == Mode::Normal || mode == Mode::Ctc)
  {
    _init(mode, prescaller);
    _initCorrect = true;
  }
}

// oc2X - is 0-> OC2A or 1-> OC2B
// default mode is FAST PWM
void Timer2::initPwm(Mode mode, Oc2x oc2X, uint8_t prescaller, uint8_t pwmDuty)
{
  if (mode == Mode::FastPwm || mode == Mode::PwmPhaseCorrect)
  {
    uint8_t oldOc2x = TCCR2A & 0b11110000;
    _init(mode, prescaller);
    TCCR2A |= oldOc2x;
    _initPwm(pwmDuty, oc2X);
    _initCorrect = true;
  }
}

void Timer2::start()
{
  if (_initCorrect)
    _setPrescaller();
}

void Timer2::stop()
{
  TCCR2B &= ~( (1 << CS20) | (1 << CS21) | (1 << CS22) );
}

void Timer2::off()
{
  _resetRegs();
}

void Timer2::setInrerrupt(Interrupt interruptVect, uint8_t ocr)
{
  switch (interruptVect)
  {
    case Interrupt::CompareA:
    OCR2A = ocr;
    break;
    case Interrupt::CompareB:
    OCR2B = ocr;
    break;
    default:
    break;
  };
  uint8_t vect = (uint8_t) interruptVect;
  TIMSK2 |= (1 << vect);
  _setPrescaller();
}

void Timer2::setPwmDuty(Oc2x oc2X, uint8_t duty)
{
  if (_initCorrect)
  {
    switch (oc2X)
    {
      case Oc2x::A:
      OCR2A = duty;
      break;
      case Oc2x::B:
      OCR2B = duty;
      break;
      default:
      break;
    };
  }
}

