#include "stdbool.h"
#include "stdint.h"
#include "HalInterrupt.h"

/* Entry.S 에서 Irq_Handler로 연결 */
__attribute__ ((interrupt ("IRQ"))) void Irq_Handler(void)
{
    Hal_interrupt_run_handler();
}

__attribute__ ((interrupt ("FIQ"))) void Fiq_Handler(void)
{
    while(true);
}