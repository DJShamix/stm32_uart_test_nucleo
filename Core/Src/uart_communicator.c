#include "uart_communicator.h"

//UART_HandleTypeDef huart_handler;
extern UART_HandleTypeDef huart2;

/* Command format: */
/* |HEADER|COMMAND|VALUE| END  |# */
/* |011000|1000000|11111|110011|# */// Turn ON
/* |011000|1000000|00000|110011|# */// Turn OFF -> sleep
/* |011000|0010000|11111|110011|# */// Force Laser ON
/* |011000|0010000|00000|110011|# */// Force Laser OFF
/* |011000|0000100|xxxxx|110011|# */// Set Laser temperature (xxxxx value in HEX)
/* |011000|0001100|00001|110011|# */// Get Laser temperature (xxxxx value in HEX)
/* |011000|0001100|00010|110011|# */// Get ThermoRes 1 temperature (value in BIN)
/* |011000|0001100|00011|110011|# */// Get ThermoRes 2 temperature (value in BIN)
/* |011000|0001100|00100|110011|# */// Get Optical 1 temperature (value in BIN)
/* |011000|0001100|00101|110011|# */// Get Optical 2 temperature (value in BIN)


uint8_t rx_buffer[100] = {0};
int recieved_commands[5] = {0};
uint8_t is_new_message = 0;


//-----инициализация работы модуля UART---------------------------------------------------
//--------------------------------------------------------------------------------------------
void uart_helper_init(UART_HandleTypeDef uart_handle){

	//huart_handler =	uart_handle;
	HAL_UART_Receive_IT(&huart2, rx_buffer, 31);
}


//-----функция отправки ответных сообщений---------------------------------------------------
//--------------------------------------------------------------------------------------------
void send_message(uint8_t *message){

	uint8_t tx_buffer[100] = {0};

	sprintf((char*) tx_buffer, "Message: %s", message);

	HAL_UART_Transmit(&huart2, (uint8_t*) tx_buffer, sizeof(tx_buffer), 150);
}
//********************************************************************************************


//-----функция обработки принятых сообщений---------------------------------------------------
//--------------------------------------------------------------------------------------------
void parse_message(uint8_t *income_message, int message_size){

	//для отладки
	//#ifdef TEST
	HAL_UART_Transmit(&huart2,  (uint8_t*) income_message, 40, 100);
	//#endif

	uint8_t separator_count = 0;		 	//количество сепараторов
	uint16_t sep_indexes[5] = {0};			//адреса сепараторов в строке

	//проверка на наличие сепараторов
	for(int sep_index = 0; sep_index < message_size; sep_index++){

		// Если обнаружен '|' -> определяем его адрес
		char message_element = (char*) income_message[sep_index];
		if(message_element == '|')
			sep_indexes[separator_count++] = sep_index;

		if(separator_count >= 5)
			break;
	}


	//Разделение сообщения на части по сепараторам
	//********************************************
	uint8_t array_index = 0;
	int command_numbers[4] = {0};
	uint8_t part_buffer[4][20] = {0};
	for(int sep_index = 0; sep_index < 4; sep_index++, array_index++){
		uint16_t s_index = sep_indexes[sep_index];
		memccpy(part_buffer[array_index], &income_message[s_index+1], '|', 20);

		// Remove '|' symbol from sub-string
		for(int ind_symbol = 0; ind_symbol < sizeof(part_buffer[array_index]); ind_symbol++){
			char message_element = (char*) part_buffer[array_index][ind_symbol];
			if(message_element == '|'){
				part_buffer[array_index][ind_symbol] = 0;
				break;
			}
		}

		// Now we have substrings, let's convert them into numbers
		char string_buffer[20] = {0};
		strcpy(string_buffer, part_buffer[array_index]);

		command_numbers[sep_index] = strtol(string_buffer, NULL, 2);

		recieved_commands[sep_index] = command_numbers[sep_index];

		memset(string_buffer, 0, 20);
		sprintf((char*) string_buffer, "%d", command_numbers[sep_index]);
		HAL_UART_Transmit(&huart2,  (uint8_t*) "\n", 1, 100);
		HAL_UART_Transmit(&huart2,  (uint8_t*) part_buffer[array_index], 20, 100);
		HAL_UART_Transmit(&huart2,  (uint8_t*) "\n", 1, 100);
		HAL_UART_Transmit(&huart2,  (uint8_t*) string_buffer, 20, 100);
		HAL_UART_Transmit(&huart2,  (uint8_t*) "\n", 1, 100);
	}
	//--------------------------------------------

	//return income_message;
}
//********************************************************************************************


//-----прерывание от USART--------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//|011000|1000000|11111|110011|#
	uint8_t check_string[sizeof(rx_buffer)] = {0};

	for(int index = 0; index < sizeof(rx_buffer); index++){
		if(rx_buffer[index] == '#'){
			// Parse new message and split to params:
			parse_message(&check_string[0], sizeof(rx_buffer));

			// Check new command:
			check_commands();

			for(int buf_index = 0; buf_index < sizeof(rx_buffer); buf_index++){
				check_string[buf_index] = 0;
				rx_buffer[buf_index] = 0;
			}
		}
		else{
			check_string[index] = rx_buffer[index];
		}
	}

	HAL_UART_Receive_IT(&huart2, rx_buffer, 31);
}
//********************************************************************************************


//-----обработка входящей команды--------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void check_commands(){

	enum{
		HEADER, COMMAND, VALUE, END_POINT
	};

	switch(recieved_commands[COMMAND]){

		case COMMAND_SYS_ON_OFF:
			switch(recieved_commands[VALUE]){
				case VALUE_SYS_ON:
					// SYSTEM ON
					break;

				case VALUE_SYS_OFF:
					// SYSTEM SLEEP
					break;
			}break;

		case COMMAND_LASER_ON_OFF:
			switch(recieved_commands[VALUE]){
				case VALUE_LASER_ON:
					// LASER ON
					break;

				case VALUE_LASER_OFF:
					// LASER OFF
					break;
			}break;

		case COMMAND_SET_LAS_TEMP:
			if(recieved_commands[VALUE] > LASER_MIN_TEMP && recieved_commands[VALUE] < LASER_MAX_TEMP){
				int set_laser_temperature = recieved_commands[VALUE]; // Set laser temperature
			}break;

		case COMMAND_GET_TEMP:
			switch(recieved_commands[VALUE]){
				case VALUE_GET_LAS_TEMP:
					// GET LASER TEMPERATURE
					break;

				case VALUE_GET_THERM1_TEMP:
					// GET TEMPERATURE FROM SENSOR #1
					break;

				case VALUE_GET_THERM2_TEMP:
					// GET TEMPERATURE FROM SENSOR #2
					break;

				case VALUE_GET_OPT1_TEMP:
					// GET TEMPERATURE FROM OPT SENSOR #1
					break;

				case VALUE_GET_OPT2_TEMP:
					// GET TEMPERATURE FROM OPT SENSOR #2
					break;
			}break;
	}
}
//********************************************************************************************


//-----главный цикл программы--------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void uart_do_work(){

	while(1){

		if(is_new_message){

			//send_message(parse_message(""));
			//HAL_UART_Receive_DMA(&huart2, rx_buffer, 100);
			//HAL_UART_Transmit(&huart2,  (uint8_t*) rx_buffer, 12, 100);
			//HAL_UART_Receive_DMA(&huart2,  (uint8_t*) rx_buffer, 12);
			is_new_message = 0;
		}
	}
}
//********************************************************************************************

