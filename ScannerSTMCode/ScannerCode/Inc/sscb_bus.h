/**
 * @file sscb_bus.h
 * @author murat.demirtas@doktar.com
 *  Created on: Oct 29, 2019
 *      Author: fullstackcoder
 */
#include "generic_cam_sensor.h"
#include "stdbool.h"
#include <stdio.h>

#ifndef HL_DRIVERS_CAMERA_SENSORS_SSCB_BUS_H_
#define HL_DRIVERS_CAMERA_SENSORS_SSCB_BUS_H_

#define OV2640_CHIP_SSCB_BUS_ADDR_W 0x60
#define OV2640_CHIP_SSCB_BUS_ADDR_R 0x61
#define SSCB_DELAY_MS(delay) HAL_Delay(delay)
#define SSCB_TIMEOUT 10000
#define HAL_OK 0x00
/**
 * This function will help you for setting register values on camera sensor
 * @param address Target camera sscb bus device address
 * @param regID Target camera sensor register address
 * @param regDat Target camera sensor register value
 * @return is SSCB bus communication is ok?
 */
bool SSCB_wrSensorReg16_8(uint16_t address, uint16_t regID, uint16_t regDat);
/**
 * This function will send all sensor camera settings struct's to camera sensor over
 * I2C or SSCB bus
 * @param reglist target camera settings struct
 * @return I2C or SSCB bus error size
 */
int SSCB_wrSensorRegs16_8(const struct sensor_reg reglist[]);
/**
 * This function will help you for reading register values on camera sensor
 * @param address Target camera sscb bus device address
 * @param regID Target camera sensor register address
 * @param regDat Target camera sensor register value
 * @return is SSCB bus communication is ok?
 */
bool SSCB_rdSensorReg16_8(uint16_t address, uint16_t regID, uint8_t* regDat);
#endif /* HL_DRIVERS_CAMERA_SENSORS_SSCB_BUS_H_ */
