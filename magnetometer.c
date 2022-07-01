/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "magnetometer.h"
#include "twi_utilities.h"
#include "math.h"

void InitMagnetometer(void){
    
    I2CM_Start();
    CyDelay(4);
    uint8_t id = 0x45;
    uint8_t toSend=0x00;
    uint8_t test;
    
    //set to sleep mode
    ReadBuffer(0x4B, &toSend, 1);
    toSend |= 0x01;
    WriteByte(0x4B, toSend);
    
    //read ID
    ReadBuffer(0x40, &id, 1);
    if (id != MAG_ID){
        exit(0);   //exit if ID does not match
    }
    
    //read trim registers values
    read_trim_registers();
    
    //set power mode to normal
    ReadBuffer(0x4C, &toSend, 1);
    toSend &= 0xF9;
    WriteByte(0x4C, toSend);
    ReadBuffer(0x4C, &test, 1);
    CyDelay(1);
    
    
    //set ODR to 10 Hz
    ReadBuffer(0x4C, &toSend, 1);
    toSend &= 0xC7;
    WriteByte(0x4C, toSend);
    ReadBuffer(0x4C, &test, 1);
    CyDelay(1);
    
    //set number of repetitions
    WriteByte(0x51, 0x04); //nXY to 9
    WriteByte(0x52, 0x0E); //nZ to 15
    
}

void WriteByte(uint8_t RegAddress, uint8_t toSend){
    
    I2CM_Start();
    CyDelayUs(100);
    I2CM_MasterSendStart(MAG_ADDR, I2CM_WRITE_XFER_MODE);
    CyDelayUs(100);
    I2CM_MasterWriteByte(RegAddress);
    CyDelayUs(100);
    I2CM_MasterWriteByte(toSend);
    CyDelayUs(100); 
    I2CM_MasterSendStop();

}

uint8_t ReadByte(uint8_t RegAddress){
    
    uint8_t receivedByte;
    I2CM_Start();
    CyDelayUs(100);
    I2CM_MasterSendStart( MAG_ADDR, 0x00);
    CyDelayUs(100);
    I2CM_MasterWriteByte(RegAddress);
    CyDelayUs(100);
    I2CM_MasterSendStop();
    CyDelayUs(100);
    I2CM_MasterSendRestart( MAG_ADDR, 0x01);
    CyDelayUs(100);
    receivedByte = I2CM_MasterReadByte(I2CM_NAK_DATA);
    CyDelayUs(100);
    I2CM_MasterSendStop();
    
    return receivedByte;
}

void ReadBuffer(uint8_t RegAddress,void* Receivebuffer, uint8_t count){
    I2CM_Start();
    CyDelayUs(100);
    I2CM_MasterSendStart( MAG_ADDR, 0x00);
    CyDelayUs(100);
    I2CM_MasterWriteByte(RegAddress);
    CyDelayUs(100);
    I2CM_MasterSendStop();
    CyDelayUs(100);
    
    ReadI2cPacket_NS(Receivebuffer, count, MAG_ADDR);
    I2CM_MasterSendStop();
    
    
}

struct bmm150_trim_registers trim_data;
void read_trim_registers() {
    uint8_t trim_x1y1[2] = {0};
    uint8_t trim_xyz_data[4] = {0};
    uint8_t trim_xy1xy2[10] = {0};
    uint16_t temp_msb = 0;

    /* Trim register value is read */
    ReadBuffer(0x5D, trim_x1y1, 2);
    ReadBuffer(0x62, trim_xyz_data, 4);
    ReadBuffer(0x68, trim_xy1xy2, 10);
    /*  Trim data which is read is updated
        in the device structure */
    trim_data.dig_x1 = (int8_t)trim_x1y1[0];
    trim_data.dig_y1 = (int8_t)trim_x1y1[1];
    trim_data.dig_x2 = (int8_t)trim_xyz_data[2];
    trim_data.dig_y2 = (int8_t)trim_xyz_data[3];
    temp_msb = ((uint16_t)trim_xy1xy2[3]) << 8;
    trim_data.dig_z1 = (uint16_t)(temp_msb | trim_xy1xy2[2]);
    temp_msb = ((uint16_t)trim_xy1xy2[1]) << 8;
    trim_data.dig_z2 = (int16_t)(temp_msb | trim_xy1xy2[0]);
    temp_msb = ((uint16_t)trim_xy1xy2[7]) << 8;
    trim_data.dig_z3 = (int16_t)(temp_msb | trim_xy1xy2[6]);
    temp_msb = ((uint16_t)trim_xyz_data[1]) << 8;
    trim_data.dig_z4 = (int16_t)(temp_msb | trim_xyz_data[0]);
    trim_data.dig_xy1 = trim_xy1xy2[9];
    trim_data.dig_xy2 = (int8_t)trim_xy1xy2[8];
    temp_msb = ((uint16_t)(trim_xy1xy2[5] & 0x7F)) << 8;
    trim_data.dig_xyz1 = (uint16_t)(temp_msb | trim_xy1xy2[4]);

}

void getmagneticData(MAGVALUES *dev){
    int16_t msb_data;
    int8_t reg_data[8] = {0};
    struct bmm150_raw_mag_data raw_mag_data;
    
    //read 8 registers that contain data
    ReadBuffer(0x42, reg_data, 8);
    
    
    //concatenate raw data for x-axis
    raw_mag_data.raw_datax = reg_data[1];
    raw_mag_data.raw_datax = (raw_mag_data.raw_datax << 8)&0xff00;
    raw_mag_data.raw_datax += (reg_data[0]&0xff);
    raw_mag_data.raw_datax = (raw_mag_data.raw_datax>>3)&0x1fff;
    
    //concatenate raw data for y-axis
    raw_mag_data.raw_datay = reg_data[3];
    raw_mag_data.raw_datay = (raw_mag_data.raw_datay << 8)&0xff00;
    raw_mag_data.raw_datay += (reg_data[2]&0xff);
    raw_mag_data.raw_datay = (raw_mag_data.raw_datay>>3)&0x1fff;
    
    //concatenate raw data for z-axis
    raw_mag_data.raw_dataz = reg_data[5];
    raw_mag_data.raw_dataz = (raw_mag_data.raw_dataz << 8)&0xff00;
    raw_mag_data.raw_dataz += (reg_data[4]&0xff);
    raw_mag_data.raw_dataz = (raw_mag_data.raw_dataz>>1)&0x7fff;
    
    //concatenate raw data for hall resistance
    raw_mag_data.raw_data_r = reg_data[7];
    raw_mag_data.raw_data_r = (raw_mag_data.raw_data_r << 8)&0xff00;
    raw_mag_data.raw_data_r += (reg_data[6]&0xff);
    raw_mag_data.raw_data_r = (raw_mag_data.raw_data_r>>2)&0x3fff;
    
    /* Compensated Mag X data in int16_t format */
    dev->xaxis  = compensate_x(raw_mag_data.raw_datax, raw_mag_data.raw_data_r);
    /* Compensated Mag Y data in int16_t format */
    dev->yaxis = compensate_y(raw_mag_data.raw_datay, raw_mag_data.raw_data_r);
    /* Compensated Mag Z data in int16_t format */
    dev->zaxis = compensate_z(raw_mag_data.raw_dataz, raw_mag_data.raw_data_r);
    
    
    
    //get directions 
    float xyHeading = atan2(dev->xaxis, dev->yaxis);
    float zxHeading = atan2(dev->zaxis, dev->xaxis);
    float heading = xyHeading;

    if (heading < 0) {
        heading += 2 * PI;
    }
    if (heading > 2 * PI) {
        heading -= 2 * PI;
    }
    dev->headingDegrees = heading * 180 / M_PI;
    dev->xyHeadingDegrees = xyHeading * 180 / M_PI;
    dev->zxHeadingDegrees = zxHeading * 180 / M_PI;
   
    
    CyDelay(1000);
}

static float compensate_x(int16_t mag_data_x, uint16_t data_rhall)
{
    float retval = 0;
    float process_comp_x0;
    float process_comp_x1;
    float process_comp_x2;
    float process_comp_x3;
    float process_comp_x4;

    /* Overflow condition check */
    if ((mag_data_x != BMM150_OVERFLOW_ADCVAL_XYAXES_FLIP) && (data_rhall != 0) && (trim_data.dig_xyz1 != 0))
    {
        /* Processing compensation equations */
        process_comp_x0 = (((float)trim_data.dig_xyz1) * 16384.0f / data_rhall);
        retval = (process_comp_x0 - 16384.0f);
        process_comp_x1 = ((float)trim_data.dig_xy2) * (retval * retval / 268435456.0f);
        process_comp_x2 = process_comp_x1 + retval * ((float)trim_data.dig_xy1) / 16384.0f;
        process_comp_x3 = ((float)trim_data.dig_x2) + 160.0f;
        process_comp_x4 = mag_data_x * ((process_comp_x2 + 256.0f) * process_comp_x3);
        retval = ((process_comp_x4 / 8192.0f) + (((float)trim_data.dig_x1) * 8.0f)) / 16.0f;
    }
    else
    {
        /* Overflow, set output to 0.0f */
        retval = BMM150_OVERFLOW_OUTPUT_FLOAT;
    }

    return retval;
}

static float compensate_y(int16_t mag_data_y, uint16_t data_rhall)
{
    float retval = 0;
    float process_comp_y0;
    float process_comp_y1;
    float process_comp_y2;
    float process_comp_y3;
    float process_comp_y4;

    /* Overflow condition check */
    if ((mag_data_y != BMM150_OVERFLOW_ADCVAL_XYAXES_FLIP) && (data_rhall != 0) && (trim_data.dig_xyz1 != 0))
    {
        /* Processing compensation equations */
        process_comp_y0 = ((float)trim_data.dig_xyz1) * 16384.0f / data_rhall;
        retval = process_comp_y0 - 16384.0f;
        process_comp_y1 = ((float)trim_data.dig_xy2) * (retval * retval / 268435456.0f);
        process_comp_y2 = process_comp_y1 + retval * ((float)trim_data.dig_xy1) / 16384.0f;
        process_comp_y3 = ((float)trim_data.dig_y2) + 160.0f;
        process_comp_y4 = mag_data_y * (((process_comp_y2) + 256.0f) * process_comp_y3);
        retval = ((process_comp_y4 / 8192.0f) + (((float)trim_data.dig_y1) * 8.0f)) / 16.0f;
    }
    else
    {
        /* Overflow, set output to 0.0f */
        retval = BMM150_OVERFLOW_OUTPUT_FLOAT;
    }

    return retval;
}


static float compensate_z(int16_t mag_data_z, uint16_t data_rhall)
{
    float retval = 0;
    float process_comp_z0;
    float process_comp_z1;
    float process_comp_z2;
    float process_comp_z3;
    float process_comp_z4;
    float process_comp_z5;

    /* Overflow condition check */
    if ((mag_data_z != BMM150_OVERFLOW_ADCVAL_ZAXIS_HALL) && (trim_data.dig_z2 != 0) &&
        (trim_data.dig_z1 != 0) && (trim_data.dig_xyz1 != 0) && (data_rhall != 0))
    {
        /* Processing compensation equations */
        process_comp_z0 = ((float)mag_data_z) - ((float)trim_data.dig_z4);
        process_comp_z1 = ((float)data_rhall) - ((float)trim_data.dig_xyz1);
        process_comp_z2 = (((float)trim_data.dig_z3) * process_comp_z1);
        process_comp_z3 = ((float)trim_data.dig_z1) * ((float)data_rhall) / 32768.0f;
        process_comp_z4 = ((float)trim_data.dig_z2) + process_comp_z3;
        process_comp_z5 = (process_comp_z0 * 131072.0f) - process_comp_z2;
        retval = (process_comp_z5 / ((process_comp_z4) * 4.0f)) / 16.0f;
    }
    else
    {
        /* Overflow, set output to 0.0f */
        retval = BMM150_OVERFLOW_OUTPUT_FLOAT;
    }

    return retval;
}

/* [] END OF FILE */
