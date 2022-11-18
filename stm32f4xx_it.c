/*------PRIVATE VARIABLES-----*/
uint8_t uart1_rx_data = 0;
uint8_t uart1_rx_flag = 0;

uint8_t uart6_rx_data = 0;
uint8_t uart6_rx_data = 0;

uint8_t m8n_rx_buf[36]; //36 bytes = size of a UBX message frame
uint8_t m8n_rx_cplt_flag = 0; // Shows receiving completed ("cplt" = complete)

/*------PERIPHERAL INTERRUPT HANDLERS------*/
// STM32 <-> PC
void USART1_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_RXNE(USART1))
	{
		LL_USART_ClearFlag_RXNE(USART1);
		uart1_rx_data = LL_USART_ReceiveData8(USART1);
		uart1_rx_flag = 1;

		LL_USART_TransmitData8(USART6, uart1_rx_data);
	}
}
// M8N
void USART6_IRQHandler(void)
{
	static unsigned char cnt=0; 	// var used to count order of the rx byte for a message frame (static prevents reinitialization and keeps var val when func is called)

	if (LL_USART_IsActiveFlag_RXNE(USART6))
	{
		LL_USART_ClearFlag_RXNE(USART6);
		uart6_rx_data = LL_USART_ReceiveData8(USART6);
		uart6_rx_flag = 1;

		// prints data from GPS(uart6) to PC(UART1)
		//LL_USART_TransmitData8(USART1, uart6_rx_data);

		switch(cnt)
		{
		case 0:
			if(uart6_rx_data == 0xb5)
			{
				m8n_rx_buf[cnt] = uart6_rx_data; //stored byte 0 of message frame as byte 0 in the temp buffer
				cnt++;
			}
			break;
		case 1:
			if(uart6_rx_data == 0x62)
			{
				m8n_rx_buf[cnt] = uart6_rx_data;
				cnt++;
			}
			else
				cnt = 0;
			break;
		case 35:
				m8n_rx_buf[cnt] = uart6_rx_data;
				cnt = 0;
				m8n_rx_cplt_flag = 1; // Message frame reception complete
				break;
		default:
				m8n_rx_buf[cnt] = uart6_rx_data;
				cnt++;
				break;
		}
	}
}