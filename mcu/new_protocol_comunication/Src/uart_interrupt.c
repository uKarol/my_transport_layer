/*
 * uart_interrupt.c
 *
 *  Created on: 9 kwi 2021
 *      Author: Karol
 */

#include <stdint.h>
#include <string.h>
#include "usart.h"
#include "uart_interrupt.h"
#include "btld_com.h"
#include "cyclic_buffer.h"

#define UART USART2

#define RX_MAX_SIZE 128

volatile uint8_t uart_data[RX_MAX_SIZE];
volatile uint8_t msg_length;
volatile uint8_t uart_bit_ctr;

cyclic_buffer tx_buffer;

void uart_isr(void){
	uint8_t trash;

	if(UART->SR & USART_SR_RXNE){
		btld_com_callback(UART->DR);
	}
	else if(UART->SR & USART_SR_ORE){
		trash = UART->DR;
		trash++;
	}
	else if(UART->SR & USART_SR_IDLE){
		trash = UART->DR;
		trash++;
		clear_error();
	}
	else if( UART->SR & USART_SR_TXE){
		uint8_t byte;
		if(buffer_get(&tx_buffer, &byte, 1) == BUFFER_OK){
			UART->DR = byte;
		}
		else{
			UART->CR1 &= ~USART_CR1_TXEIE;
		}
	}
}
void uart_enable_rx_irq(void){
	UART->CR1 |= USART_CR1_RXNEIE;
	tx_buffer.max_length = 100;
	tx_buffer.head_ptr = 1;
	tx_buffer.tail_ptr = 0;
	tx_buffer.mode = BUFFER_ACTIVE;
}
void uart_enable_idle_irq(void){
	UART->CR1 |= USART_CR1_IDLEIE;
}
void uart_disable_idle_irq(void){
	UART->CR1 &= ~USART_CR1_IDLEIE;
}
void uart_disable_rx_irq(void){
	UART->CR1 &= ~USART_CR1_RXNEIE;
}
void uart_enable_tx_irq(void){
	UART->CR1 |= USART_CR1_TXEIE;
}
void uart_disable_tx_irq(void){
	UART->CR1 &= ~USART_CR1_TXEIE;
}
void uart_send(uint8_t *text, uint8_t length){
	UART->CR1 |= USART_CR1_TXEIE;
	msg_length = length;
	if(buffer_get_busy(&tx_buffer) == 0 ){
		buffer_put(&tx_buffer, &text[1], length-1);
		UART->DR = text[0];
	}
	else{
		buffer_put(&tx_buffer, text, length);
	}

}

