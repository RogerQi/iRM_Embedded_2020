#include "bsp_can.h"
#include "bsp_error_handler.h"

namespace BSP {

static CAN *can1 = 0;
static CAN *can2 = 0;

static inline CAN* find_can_instance(CAN_HandleTypeDef *hcan) {
  if (can1 && hcan == &hcan1)
    return can1;
  else if (can2 && hcan == &hcan2)
    return can2;
  return NULL;
}

static inline bool can_handle_exists(CAN_HandleTypeDef *hcan) {
  return (can1 && hcan == &hcan1) || (can2 && hcan == &hcan2);
}

static void can_rx_fifo0_message_pending_callback(CAN_HandleTypeDef *hcan) {
  CAN* can = find_can_instance(hcan);
  if (!can)
    return;
  can->RxCallback();
}

CAN::CAN(CAN_HandleTypeDef *hcan, uint32_t start_id) 
      : hcan_(hcan), start_id_(start_id) {
  RM_ASSERT_FALSE(can_handle_exists(hcan), "Repeated CAN initialization");
  ConfigureFilter(hcan);
  // activate rx interrupt
  RM_ASSERT_HAL_OK(HAL_CAN_RegisterCallback(hcan, HAL_CAN_RX_FIFO0_MSG_PENDING_CB_ID,
        can_rx_fifo0_message_pending_callback), "Cannot register CAN rx callback");
  RM_ASSERT_HAL_OK(HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING),
      "Cannot activate CAN rx message pending notification");
  RM_ASSERT_HAL_OK(HAL_CAN_Start(hcan), "Cannot start CAN");
  // save can instance as global pointer
  if (hcan == &hcan1)
    can1 = this;
  else if (hcan == &hcan2)
    can2 = this;
}

int CAN::RegisterRxCallback(uint32_t std_id, can_rx_callback_t callback) {
  int callback_id = std_id - start_id_;

  if (callback_id < 0 || callback_id >= MAX_CAN_DEVICES)
    return -1;

  rx_callbacks_[callback_id] = callback;
  return 0;
}

int CAN::Transmit(uint16_t id, const uint8_t data[], uint32_t length) {
  RM_EXPECT_TRUE(IS_CAN_DLC(length), "CAN tx data length exceeds limit");
  if (!IS_CAN_DLC(length))
    return -1;
  
  CAN_TxHeaderTypeDef header = {
    .StdId              = id,
    .ExtId              = 0x0, // don't care since we use standard id mode
    .IDE                = CAN_ID_STD,
    .RTR                = CAN_RTR_DATA,
    .DLC                = length,
    .TransmitGlobalTime = DISABLE,
  };
  uint32_t mailbox;

  if (HAL_CAN_AddTxMessage(hcan_, &header, (uint8_t*)data, &mailbox) != HAL_OK)
    return -1;

  // poll for can transmission to complete
  while(HAL_CAN_IsTxMessagePending(hcan_, mailbox));

  return length;
}

void CAN::RxCallback() {
  CAN_RxHeaderTypeDef header;
  uint8_t data[MAX_CAN_DATA_SIZE];
  HAL_CAN_GetRxMessage(hcan_, CAN_RX_FIFO0, &header, data);
  int callback_id = header.StdId - start_id_;
  // find corresponding callback
  if (callback_id >= 0 && callback_id < MAX_CAN_DEVICES && rx_callbacks_[callback_id])
    rx_callbacks_[callback_id](data);
}

void CAN::ConfigureFilter(CAN_HandleTypeDef *hcan) {
  CAN_FilterTypeDef       CAN_FilterConfigStructure;
  /* Configure Filter Property */
  CAN_FilterConfigStructure.FilterIdHigh = 0x0000;
  CAN_FilterConfigStructure.FilterIdLow = 0x0000;
  CAN_FilterConfigStructure.FilterMaskIdHigh = 0x0000;
  CAN_FilterConfigStructure.FilterMaskIdLow = 0x0000;
  CAN_FilterConfigStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  CAN_FilterConfigStructure.FilterMode = CAN_FILTERMODE_IDMASK;
  CAN_FilterConfigStructure.FilterScale = CAN_FILTERSCALE_32BIT;
  CAN_FilterConfigStructure.FilterActivation = ENABLE;
  CAN_FilterConfigStructure.SlaveStartFilterBank = 14;  //CAN1 and CAN2 split all 28 filters
  /* Configure each CAN bus */
  if (hcan == &hcan1)
      CAN_FilterConfigStructure.FilterBank = 0; //Master CAN1 get filter 0-13
  else if (hcan == &hcan2)
      CAN_FilterConfigStructure.FilterBank = 14; //Slave CAN2 get filter 14-27

  RM_EXPECT_HAL_OK(HAL_CAN_ConfigFilter(hcan_, &CAN_FilterConfigStructure), 
      "CAN filter configuration failed.");
}

} /* namespace BSP */