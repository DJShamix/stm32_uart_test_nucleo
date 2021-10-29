#ifndef INC_UART_COMMUNICATOR_H_
#define INC_UART_COMMUNICATOR_H_

//#include "main.h"
#include "stdio.h"
#include <string.h>
#include "stm32l4xx_hal.h"

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

#define COMMAND_HEADER			24 // 011000
#define COMMAND_END				51 // 110011

#define COMMAND_SYS_ON_OFF		64 // 1000000
#define COMMAND_LASER_ON_OFF  	16 // 0010000
#define COMMAND_SET_LAS_TEMP  	4  // 0000100
#define COMMAND_GET_TEMP  		12 // 0001100

#define VALUE_SYS_ON  			31 // 11111
#define VALUE_SYS_OFF  			0  // 00000
#define VALUE_LASER_ON  		31 // 11111
#define VALUE_LASER_OFF  		0  // 00000
#define VALUE_GET_LAS_TEMP 		1  // 00001
#define VALUE_GET_THERM1_TEMP 	2  // 00010
#define VALUE_GET_THERM2_TEMP 	3  // 00011
#define VALUE_GET_OPT1_TEMP 	4  // 00100
#define VALUE_GET_OPT2_TEMP 	5  // 00101


#define LASER_MIN_TEMP 			10
#define LASER_MAX_TEMP 			40


void uart_helper_init(UART_HandleTypeDef uart_handle);

void send_message(uint8_t *message);

void parse_message(uint8_t *income_message, int message_size);

void check_commands();

void uart_do_work();

#endif /* INC_UART_COMMUNICATOR_H_ */
