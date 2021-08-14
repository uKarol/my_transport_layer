/*
 * uart_interrupt.h
 *
 *  Created on: 9 kwi 2021
 *      Author: Karol
 */

#ifndef INC_UART_INTERRUPT_H_
#define INC_UART_INTERRUPT_H_

typedef enum
{
	UART_OK,
	UART_TIMEOUT,
	UART_ERROR
} uart_status_t;

void uart_isr(void);
void uart_enable_rx_irq(void);
void uart_enable_idle_irq(void);
void uart_disable_idle_irq(void);
void uart_disable_rx_irq(void);
void uart_enable_tx_irq(void);
void uart_disable_tx_irq(void);
void uart_send(uint8_t *text, uint8_t length);

#endif /* INC_UART_INTERRUPT_H_ */
