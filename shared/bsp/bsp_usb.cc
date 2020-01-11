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
#include "bsp_usb.h"

namespace BSP {

static struct {
  uint8_t *write = NULL;
  uint8_t *read = NULL;
  uint32_t size = 0;
  uint32_t count = 0;
  usb_callback_t callback = NULL;
} usb_rx;

static void usb_data_ready(uint8_t *data, uint32_t length) {
  if (usb_rx.write) {
    if (length + usb_rx.count > usb_rx.size)
      length = usb_rx.size - usb_rx.count;
    memcpy(usb_rx.write + usb_rx.count, data, length);
    usb_rx.count += length;
  }
  // optional rx notification callback
  if (usb_rx.callback)
    usb_rx.callback();
}

void usb_setup_rx(uint32_t buffer_size, usb_callback_t callback) {
  if (usb_rx.write || usb_rx.read) {
    delete[] usb_rx.write;
    delete[] usb_rx.read;
  }

  usb_rx.write = new uint8_t[buffer_size];
  usb_rx.read = new uint8_t[buffer_size];
  usb_rx.size = buffer_size;
  usb_rx.count = 0;
  usb_rx.callback = callback;
}

int32_t usb_write(uint8_t *data, uint32_t length) {
  uint8_t status;

  while ((status = CDC_Transmit_FS(data, (uint16_t)length)) == USBD_BUSY);
  if (status == USBD_OK)
    return length;
  else
    return -1; 
}

int32_t usb_read(uint8_t **data) {
  if (!data || !usb_rx.size)
    return -1;

  int32_t ret;
  uint8_t *tmp;
  taskENTER_CRITICAL();
  ret = usb_rx.count;
  usb_rx.count = 0;
  tmp = usb_rx.write;
  usb_rx.write = usb_rx.read;
  usb_rx.read = tmp;
  taskEXIT_CRITICAL();

  *data = usb_rx.read;
  return ret;
}

} /* namespace BSP */

void RM_USB_RxCplt_Callback(uint8_t *Buf, uint32_t Len) {
  BSP::usb_data_ready(Buf, Len);
}

