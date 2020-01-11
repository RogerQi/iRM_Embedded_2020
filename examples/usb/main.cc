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

#include "cmsis_os.h"
#include "main.h"

#include "bsp_usb.h"

#define RX_SIGNAL (1 << 0)

static uint8_t *rx_buf;
static uint32_t rx_len;

static osThreadId usb_task_id;
static osEvent usb_event;

static void example_usb_callback(void) {
  osSignalSet(usb_task_id, RX_SIGNAL);
}


static void usb_task(const void *argu) {
  UNUSED(argu);
  while (true) {
    usb_event = osSignalWait(RX_SIGNAL, osWaitForever);
    if (usb_event.value.signals & RX_SIGNAL) { // uncessary check
      rx_len = BSP::usb_read(&rx_buf);
      BSP::usb_write(rx_buf, rx_len);
    }
  }
}


void RM_RTOS_Init(void) {
  BSP::usb_setup_rx(64, example_usb_callback);
}


void RM_RTOS_Default_Task(const void *argu) {
  osThreadDef(usbTask, usb_task, osPriorityNormal, 0, 256);
  usb_task_id = osThreadCreate(osThread(usbTask), (void*)argu);
}

