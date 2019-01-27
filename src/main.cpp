//------------------------------------------------------------------------------

#include <Arduino.h>

//------------------------------------------------------------------------------

#include <scmRTOS.h>

#if scmRTOS_IDLE_HOOK_ENABLE
void OS::idle_process_user_hook() { loop(); }
#endif

#if SCMRTOS_USE_CUSTOM_TIMER

/**
 * Custom system timer configuration.
 */
extern "C" void __init_system_timer()
{
	RCC_BASE->APB1ENR  |= RCC_APB1ENR_TIM4EN;
  nvic_irq_set_priority(NVIC_TIMER4, 0xFF);
  NVIC_BASE->ISER[(((uint32_t)30) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)30) & 0x1FUL));
	TIMER4->regs.gen->PSC = 1;
	TIMER4->regs.gen->ARR = 36000;          // 1KHz
	TIMER4->regs.gen->EGR = TIMER_EGR_UG;     // generate an update event to reload the prescaler value immediately
	TIMER4->regs.gen->DIER = TIMER_DIER_UIE;  // enable update interrupt
	TIMER4->regs.gen->CR1 = TIMER_CR1_CEN;    // run timer
}

void LOCK_SYSTEM_TIMER()
{
	TIMER4->regs.gen->CR1 &= ~TIMER_CR1_CEN;
}

void UNLOCK_SYSTEM_TIMER()
{
	TIMER4->regs.gen->CR1 |= TIMER_CR1_CEN;
}

/**
 * Interrupt handler.
 * Clears interrupt flags and calls the OS::system_timer_isr();
 */
OS_INTERRUPT void TIM4_IRQHandler()
{
	if (TIMER4->regs.gen->SR & TIMER_SR_UIF)
	{
		TIMER4->regs.gen->SR = ~TIMER_SR_UIF;
OS::TISRW ISR; // DVV: need ???
		OS::system_timer_isr();
	}
}

#endif

//------------------------------------------------------------------------------

void setup() {

  Serial.begin(115200);

  // check USBSerial works
  delay(2000);
  Serial.println("INIT");

  // start OS
  UNLOCK_SYSTEM_TIMER();
  OS::run();

}

//------------------------------------------------------------------------------

void loop() {
}

//------------------------------------------------------------------------------

typedef OS::process<OS::pr0, 256> TProc0;
static TProc0 Proc0;

namespace OS
{
  template<> OS_PROCESS void TProc0::exec() {
    for (;;) {
      OS::sleep(1000);
      Serial.print("~");
    }
  }
}

//------------------------------------------------------------------------------
