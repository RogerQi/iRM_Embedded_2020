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

#pragma once

#include "main.h"

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

namespace BSP {

typedef void (*usb_callback_t)(void);

/**
 * @brief register an arbitrary function to handle usb rx callback
 *
 * @param callback  a funtion pointer of type usb_callback_t
 */
void usb_setup_rx(uint32_t buffer_size, usb_callback_t callback = NULL);

/**
 * @brief transmit data via usb
 *
 * @param data    data buffer
 * @param length  length [in bytes] to transmit
 *
 * @return number of bytes succesfully transmitted, -1 if failed
 *
 * @note will wait (poll) for ongoing transmission to complete
 * @note CANNOT be called in IRQ handler (interrupt context)
 */
int32_t usb_write(uint8_t *data, uint32_t length);

/**
 * @brief read pending data from usb rx buffer
 *
 * @param data  output data buffer
 *
 * @return  number of bytes read 
 */
int32_t usb_read(uint8_t **data);

} /* namespace BSP */
