#include "stdint.h"
#include "stdbool.h"

#include "Uart.h"
#include "HalUart.h"
#include "HalInterrupt.h"

#include "Kernel.h"

extern volatile PL011_t* Uart;

static void interrupt_handler(void);

void Hal_uart_init()
{
	Uart->uartcr.bits.UARTEN = 0;
	Uart->uartcr.bits.TXE = 1;
	Uart->uartcr.bits.RXE = 1;
	Uart->uartcr.bits.UARTEN = 1;

	/* Enable input interrupt */
	Uart->uartimsc.bits.RXIM = 1;

	/* Register UART interrupt handler */
	Hal_interrupt_enable(UART_INTERRUPT0);
	Hal_interrupt_register_handler(interrupt_handler, UART_INTERRUPT0);
}

void Hal_uart_put_char(uint8_t ch)
{
	while(Uart->uartfr.bits.TXFF);
	Uart->uartdr.all = (ch & 0xFF);
}

uint8_t Hal_uart_get_char()
{
	uint8_t data;

	while(Uart->uartfr.bits.RXFE);

	data = Uart->uartdr.bits.DATA;

	if (data & 0xFFFFFF00)
	{
		// clear
		Uart->uartrsr.bits.BE = 1;
		Uart->uartrsr.bits.FE = 1;
		Uart->uartrsr.bits.OE = 1;
		Uart->uartrsr.bits.PE = 1;
		return 0;
	}
	return (uint8_t)(data & 0xFF);
}

static void interrupt_handler(void)
{
	uint8_t ch = Hal_uart_get_char();

	if(ch == 'U')
	{
		Kernel_send_events(KernelEventFlag_Unlock);
		return;
	}
	if(ch == 'X')
	{
		Kernel_send_events(KernelEventFlag_CmdOut);
		return;
	}

	Hal_uart_put_char(ch);
	Kernel_send_msg(KernelMsgQ_Task0, &ch, 1);
	Kernel_send_events(KernelEventFlag_UartIn);
	/*
	if(ch != 'X') {
		Hal_uart_put_char(ch);
		Kernel_send_msg(KernelMsgQ_Task0, &ch, 1);
		Kernel_send_events(KernelEventFlag_UartIn);
	}
	else
	{
		Kernel_send_events(KernelEventFlag_CmdOut);
	}
	*/
}