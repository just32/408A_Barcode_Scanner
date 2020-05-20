/**
 * @brief
 * @author murat.demirtas@doktar.com
 * @file ov2640.h
 *
 */

#include <stdio.h>
#include "stdbool.h"
#include "sscb_bus.h"
#include "ov2640_regs.h"

#ifndef HL_DRIVERS_CAMERA_SENSORS_OV2640_OV2640_H_
#define HL_DRIVERS_CAMERA_SENSORS_OV2640_OV2640_H_

/**
 * This function will test camera communication over sscb or I2C bus. Some
 * cameras supporting I2C protocol.
 * @param chip_id_high_addr Camera sensor chip id high(vid) addr
 * @param chip_id_low_addr Camera sensor chip id low(pid) addr
 * @return is test passed?
 */
bool Camera_Test_SSCB_Bus(uint16_t chip_id_high_addr, uint16_t chip_id_low_addr);

#endif /* HL_DRIVERS_CAMERA_SENSORS_OV2640_OV2640_H_ */
