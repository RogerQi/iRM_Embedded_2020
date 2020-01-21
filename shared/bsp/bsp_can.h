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

#include "can.h"

#define MAX_CAN_DATA_SIZE 8
#define MAX_CAN_DEVICES   12

namespace BSP {

typedef void (*can_rx_callback_t)(const uint8_t data[], void *args);

class CAN {
 public:
  CAN(CAN_HandleTypeDef *hcan, uint32_t start_id);

  bool Uses(CAN_HandleTypeDef *hcan) { return hcan_ == hcan; }
  int RegisterRxCallback(uint32_t std_id, can_rx_callback_t callback, void *args = NULL);
  int Transmit(uint16_t id, const uint8_t data[], uint32_t length);
  void RxCallback();

 private:
  void ConfigureFilter(CAN_HandleTypeDef *hcan);

  CAN_HandleTypeDef   *hcan_;
  uint32_t            start_id_;
  can_rx_callback_t   rx_callbacks_[MAX_CAN_DEVICES] = { 0 };
  void                *rx_args_[MAX_CAN_DEVICES] = { NULL };
};

} /* namespace BSP */
