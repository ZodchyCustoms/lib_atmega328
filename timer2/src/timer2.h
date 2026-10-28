#include <avr/io.h>
#include <avr/interrupt.h>
#include <my_pins.h>

// Making
// CTC and Normal modes are working.
// PWM and double PWM working ok



class Timer2
{
public:
  Timer2();
  ~Timer2();

  enum class Mode : uint8_t
  {
    Normal = 0,
    PwmPhaseCorrect = 1,
    Ctc = 2,
    FastPwm = 3
  };

  enum class Interrupt : uint8_t
  {
    Overflow = 0,
    CompareA = 1,
    CompareB = 2
  };

  enum class Oc2x : uint8_t
  {
    A = 0,
    B = 1
  };

  void init(Mode mode, uint8_t prescaller);
  void initPwm(Mode mode, Oc2x oc2X, uint8_t prescaller, uint8_t pwmDuty);
  void start();
  void stop();
  void off();
  void setInrerrupt(Interrupt, uint8_t ocr);
  void setPwmDuty(Oc2x, uint8_t);

private:
  bool _initCorrect = false;
  uint8_t _prescaller = 0, _mode = 0;

  void _resetRegs();
  void _setWgmBits();
  void _setPrescaller();
  void _init(Mode, uint8_t);
  void _initPwm(uint8_t, Oc2x);
};

extern Timer2 timer2;

