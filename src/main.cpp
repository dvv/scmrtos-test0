#include <Arduino.h>
#include <scmRTOS.h>

#define LED_PIN PB1

void ticker() {
  OS::system_timer_isr();
}

extern "C" void __init_system_timer() {
  systick_attach_callback(ticker);
}

void LOCK_SYSTEM_TIMER() {
  systick_disable();
}

void UNLOCK_SYSTEM_TIMER() {
  systick_enable();
}

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.print("Starting os...\r\n");

  // start OS
  UNLOCK_SYSTEM_TIMER();
  OS::run();
}

void loop()
{
}

typedef OS::process<OS::pr0, 600> TProc0;
typedef OS::process<OS::pr1, 600> TProc1;

TProc0 proc0;
TProc1 proc1;

OS::TMutex serialMutex;

void serialBlink(char const* message)
{
  OS::TMutexLocker lock(serialMutex);
  Serial.print(message);
}

OS::TEventFlag eventFlag;

namespace OS
{
  template<> OS_PROCESS void TProc0::exec()
  {
    for (;;)
    {
      serialBlink("0");
      digitalWrite(LED_PIN, 0);
      OS::sleep(50);
      eventFlag.signal();
      OS::sleep(950);
    }
  }

  template<> OS_PROCESS void TProc1::exec() {
    for (;;) {
      eventFlag.wait();
      digitalWrite(LED_PIN, 1);
      serialBlink("1");
    }
  }
}
