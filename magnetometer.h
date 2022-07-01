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

#include <stdio.h> 

#ifndef magnetometer_h
    #define magnetometer_h
    #define MAG_ADDR 0x10
    #define MAG_ID 0x32
    
//    #define BMM150_XYAXES_FLIP_OVERFLOW_ADCVAL	(-4096)
//    #define BMM150_ZAXIS_HALL_OVERFLOW_ADCVAL	  (-16384)
//    #define BMM150_OVERFLOW_OUTPUT			        (-32768)
//    #define BMM150_NEGATIVE_SATURATION_Z        (-32767)
//    #define BMM150_POSITIVE_SATURATION_Z        (32767)
    
    #define BMM150_OVERFLOW_ADCVAL_XYAXES_FLIP        INT16_C(-4096)
    #define BMM150_OVERFLOW_ADCVAL_ZAXIS_HALL         INT16_C(-16384)
    #define BMM150_OVERFLOW_OUTPUT                    INT16_C(-32768)
    #define BMM150_NEGATIVE_SATURATION_Z              INT16_C(-32767)
    #define BMM150_POSITIVE_SATURATION_Z              INT16_C(32767)
    #define BMM150_OVERFLOW_OUTPUT_FLOAT              0.0f
    
    
    #define PI 3.141343
    
    typedef struct magvalues_t {
        float xaxis,yaxis,zaxis;
        float headingDegrees,xyHeadingDegrees,zxHeadingDegrees;
    }MAGVALUES;
    
    void InitMagnetometer(void);
    void WriteByte(uint8_t Address, uint8_t toSend);
    void ReadBuffer(uint8_t RegAddress, void * Receivebuffer, uint8_t count);
    void getmagneticData (MAGVALUES *dev);
    void read_trim_registers();
    static float compensate_x(int16_t mag_data_x, uint16_t data_rhall);
    static float compensate_y(int16_t mag_data_y, uint16_t data_rhall);
    static float compensate_z(int16_t mag_data_z, uint16_t data_rhall);

    struct bmm150_raw_mag_data {
        /*! Raw mag X data */
        int16_t raw_datax;
        /*! Raw mag Y data */
        int16_t raw_datay;
        /*! Raw mag Z data */
        int16_t raw_dataz;
        /*! Raw mag resistance value */
        uint16_t raw_data_r;
    };
    
    struct bmm150_trim_registers {
        /*! trim x1 data */
        int8_t dig_x1;
        /*! trim y1 data */
        int8_t dig_y1;
        /*! trim x2 data */
        int8_t dig_x2;
        /*! trim y2 data */
        int8_t dig_y2;
        /*! trim z1 data */
        uint16_t dig_z1;
        /*! trim z2 data */
        int16_t dig_z2;
        /*! trim z3 data */
        int16_t dig_z3;
        /*! trim z4 data */
        int16_t dig_z4;
        /*! trim xy1 data */
        uint8_t dig_xy1;
        /*! trim xy2 data */
        int8_t dig_xy2;
        /*! trim xyz1 data */
        uint16_t dig_xyz1;
    };
    
    extern struct bmm150_trim_registers trim_data;
   
    
#endif    
/* [] END OF FILE */
