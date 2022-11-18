/* M8N GPS CODE*/

#include "M8N.h"

/*------PRIVATE TYPEDEF------*/
int _write(int file, char* p, int len)
{
	for(int i=0;i<len;i++)
	{
		while(!LL_USART_IsActiveFlag_TXE(USART1)); //Checks to see if tx buffer is empty before transmitting data to UART
		LL_USART_TransmitData8(USART1, *(p+i));
	}
	return len;
}

/*------PRIVATE VARIABLES------*/
extern uint8_t uart1_rx_data;
extern uint8_t uart1_rx_flag;

extern uint8_t uart6_rx_data;
extern uint8_t uart6_rx_flag;

extern uint8_t m8n_rx_buf[36]; //36 bytes = size of a UBX message frame
extern uint8_t m8n_rx_cplt_flag; // Shows receiving completed ("cplt" = complete)

int main(void)
{

	LL_USART_EnableIT_RXNE(USART1);		// UART Adapter <-> PC
	LL_USART_EnableIT_RXNE(USART6);		// M8N GPS <-> STM32 Nucleo

	while(1)
	{	
		if(m8n_rx_cplt_flag == 1) // If data is received
		{
			m8n_rx_cplt_flag = 0;
			if(M8N_UBX_CHKSUM_Check(&m8n_rx_buf[0], 36) == 1) 	//If the return of CHKSUM_Check is 1
			{
				//decode and parse data + store to struct
				LL_GPIO_TogglePin();
				M8N_UBX_NAV_POSLLH_Parsing(&m8n_rx_buf[0], &posllh);

				printf("LAT: %ld\tLON: %ld\tHeight:%ld\r\n", posllh.lat, posllh.lon, posllh.height);

			}
		}
	}
}
