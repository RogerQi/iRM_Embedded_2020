/****************************************************************************
 *                                                                          *
 *  Copyright (C) 2020 RoboMaster.                                          *
 *  Illini RoboMaster @ University of Illinois at Urbana-Champaign          *
 *                                                                          *
 *  This program is free software: you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                          *
 ****************************************************************************/

#include "bsp_print.h"
#include "bsp_uart.h"
#include "bsp_usb.h"

#include "main.h"
#include "printf.h" // third party tiny-printf implemnetations

#define MAX_PRINT_LEN 80

static BSP::UART *print_uart = NULL;

void print_use_uart(UART_HandleTypeDef *huart) {
  if (print_uart)
    delete print_uart;

  print_uart = new BSP::UART(huart);
  print_uart->SetupTx(MAX_PRINT_LEN * 2); // burst transfer size up to 2x max buffer size
}

void print_use_usb() {
  if (print_uart)
    delete print_uart;

  print_uart = NULL;
}

int32_t print(const char *format, ...) {
#ifdef NDEBUG
  UNUSED(format);
  return 0;
#else // == #ifdef DEBUG
  char    buffer[MAX_PRINT_LEN];
  va_list args; 
  int     length;

  va_start(args, format);
  length = vsnprintf(buffer, MAX_PRINT_LEN, format, args);
  va_end(args);

  if (print_uart) 
    return print_uart->Write((uint8_t*)buffer, length);  
  else
    return usb_transmit((uint8_t*)buffer, length);  
#endif // #ifdef NDEBUG
}
