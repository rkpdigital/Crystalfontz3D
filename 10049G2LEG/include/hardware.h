/*
 * FILE NAME: hardware.h - system hardware configuration
 *
 * Copyright (c) 2014 Robert K. Parker
 *
 * This file was part of the TinyG project
 *
 * Copyright (c) 2013 Alden S. Hart, Jr.
 * Copyright (c) 2013 Robert Giseburt
 *
 * Now it is in crystalfontz3D
 *
 * This file ("the software") is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2 as published by the
 * Free Software Foundation. You should have received a copy of the GNU General Public
 * License, version 2 along with the software.  If not, see <http://www.gnu.org/licenses/> .
 *
 * As a special exception, you may use this file as part of a software library without
 * restriction. Specifically, if other files instantiate templates or use macros or
 * inline functions from this file, or you compile this file and link it with  other
 * files to produce an executable, this file does not by itself cause the resulting
 * executable to be covered by the GNU General Public License. This exception does not
 * however invalidate any other reasons why the executable file might be covered by the
 * GNU General Public License.
 *
 * THE SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT WITHOUT ANY
 * WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * PURPOSE: 10049 hardware interface specification.
 *
 *
 * VERSION: This file is Crystalfontz 10049 platform specific using the 10036 SOM
 *
 * NOTES:
 *
 *
 * CHANGE HISTORY:
 *
 *    Revision: Initial 1.0
 *    User: R.K.Parker     Date: 05/29/14
 *    First prototype.
 *
 */

#ifndef HARDWARE_H_ONCE
#define HARDWARE_H_ONCE

////////////////////////////
/////// ARM VERSION ////////
////////////////////////////

#ifdef __cplusplus
extern "C"{
#endif

/**** Global System Defines ****/

//#undef F_CPU							// CPU clock - set for delays
#define F_CPU 247000000UL


/**** Stepper DDA and dwell timer settings ****/

#define FREQUENCY_DDA		1000000UL // Was 100k now running at 1 microsecond resolution.
#define FREQUENCY_DWELL		1000UL


/**** FIQ pin values from ArmStrongPrinting ****/
#define STEPPER_NEGATIVE 1

#define B_DIR_BIT   0b00000010000000000000000000000000
#define B_STEP_BIT  0b00000000001000000000000000000000
#define A_DIR_BIT   0b00000000000000010000000000000000
#define A_STEP_BIT  0b00000000000000000000000001000000
#define Z_DIR_BIT   0b00000000000000000000000000100000
#define Z_STEP_BIT  0b00000000000000000000000000010000
#define Y_DIR_BIT   0b00000000000000000000000000001000
#define Y_STEP_BIT  0b00000000000000000000000000000100
#define X_DIR_BIT   0b00000000000000000000000000000010
#define X_STEP_BIT  0b00000000000000000000000000000001
#define ALL_STEPS   0b00000000001000000000000001010101
#define ALL_DIRS    0b00000010000000010000000000101010
#define ALL_CMDS    0x0000FF00  // Tenative place for commands to stop on event or increment count??
#define ALL_ZEROES  0x00000000


//Direct GPIO port hardware pins.
#define BIT_SET 0x4
#define BIT_CLR	0x8
#define BIT_TOG	0xc

#define HW_PINCTRL_DOE0			0xb00
#define HW_PINCTRL_DOE2			0xb20
#define HW_PINCTRL_DOE3			0xb30
#define HW_PINCTRL_DIN0			0x900
#define HW_PINCTRL_DIN2			0x920
#define HW_PINCTRL_DIN3			0x930
#define HW_PINCTRL_DOUT0		0x700
#define HW_PINCTRL_DOUT2		0x720
#define HW_PINCTRL_DOUT3		0x730

// Limit Switches
#define XU_LIM_P3_BIT  0b00000000001000000000000000000000
#define YU_LIM_P3_BIT  0b00000000010000000000000000000000
#define ZU_LIM_P3_BIT  0b00000000100000000000000000000000
#define XD_LIM_P2_BIT  0b00000000000000000000000000010000
#define YD_LIM_P2_BIT  0b00000000000000000000000000100000
#define ZD_LIM_P2_BIT  0b00000000000000000000000001000000
#define STOP_P3_BIT    0b00000000000001000000000000000000


//SPI Motor Output Pins P0.26 in order LSB first/top (Little Endian)

#ifndef __cplusplus
typedef
#endif
union Slow_Motor_t
  {
  struct
    {
    unsigned int T6_EMITTER : 1;    // 0..1   (1 bits)
    unsigned int T5_EMITTER : 1;    // 0..1   (1 bits)
    unsigned int MS2_A      : 1;    // 0..1   (1 bits)
    unsigned int MS1_A      : 1;    // 0..1   (1 bits)
    unsigned int MS0_A      : 1;    // 0..1   (1 bits)
    unsigned int SLEEP_A_N  : 1;    // 0..1   (1 bits)
    unsigned int ENABLE_A_N : 1;    // 0..1   (1 bits)
    unsigned int RESET_A_N  : 1;    // 0..1   (1 bits)

    unsigned int MS2_B      : 1;    // 0..1   (1 bits)
    unsigned int MS1_B      : 1;    // 0..1   (1 bits)
    unsigned int MS0_B      : 1;    // 0..1   (1 bits)
    unsigned int SLEEP_B_N  : 1;    // 0..1   (1 bits)
    unsigned int ENABLE_B_N : 1;    // 0..1   (1 bits)
    unsigned int RESET_B_N  : 1;    // 0..1   (1 bits)
    unsigned int MS2_Z      : 1;    // 0..1   (1 bits)
    unsigned int MS1_Z      : 1;    // 0..1   (1 bits)

    unsigned int MS0_Z      : 1;    // 0..1   (1 bits)
    unsigned int SLEEP_Z_N  : 1;    // 0..1   (1 bits)
    unsigned int ENABLE_Z_N : 1;    // 0..1   (1 bits)
    unsigned int RESET_Z_N  : 1;    // 0..1   (1 bits)
    unsigned int MS2_Y      : 1;    // 0..1   (1 bits)
    unsigned int MS1_Y      : 1;    // 0..1   (1 bits)
    unsigned int MS0_Y      : 1;    // 0..1   (1 bits)
    unsigned int SLEEP_Y_N  : 1;    // 0..1   (1 bits)

    unsigned int ENABLE_Y_N : 1;    // 0..1   (1 bits)
    unsigned int RESET_Y_N  : 1;    // 0..1   (1 bits)
    unsigned int MS2_X      : 1;    // 0..1   (1 bits)
    unsigned int MS1_X      : 1;    // 0..1   (1 bits)
    unsigned int MS0_X      : 1;    // 0..1   (1 bits)
    unsigned int SLEEP_X_N  : 1;    // 0..1   (1 bits)
    unsigned int ENABLE_X_N : 1;    // 0..1   (1 bits)
    unsigned int RESET_X_N  : 1;    // 0..1   (1 bits)
    } Bits;
    unsigned char Bytes[4];
    unsigned int Frame;
  };

//SPI Enb/LCD Output Pins P0.17 in order LSB first/top (Little Endian)

#ifndef __cplusplus
typedef
#endif
union Slow_ENB_LCD_t
  {
  struct
    {
    unsigned int EN_H1      : 1;    // 0..1   (1 bits)
    unsigned int EN_H2      : 1;    // 0..1   (1 bits)
    unsigned int EN_H3      : 1;    // 0..1   (1 bits)
    unsigned int EN_12V     : 1;    // 0..1   (1 bits)
    unsigned int LCD_IM0    : 1;    // 0..1   (1 bits)
    unsigned int LCD_IM1    : 1;    // 0..1   (1 bits)
    unsigned int LCD_IM2    : 1;    // 0..1   (1 bits)
    unsigned int QH         : 1;    // 0..1   (1 bits)

    unsigned int F_PWR1     : 1;    // 0..1   (1 bits)
    unsigned int F_PWR2     : 1;    // 0..1   (1 bits)
    unsigned int F_PWR3     : 1;    // 0..1   (1 bits)
    unsigned int F_PWR4     : 1;    // 0..1   (1 bits)
    unsigned int F_PWR5     : 1;    // 0..1   (1 bits)
    unsigned int F_PWR6     : 1;    // 0..1   (1 bits)
    unsigned int SLEEP_M    : 1;    // 0..1   (1 bits)
    unsigned int MODE_M     : 1;    // 0..1   (1 bits)
    } Bits;
    unsigned char Bytes[2];
    unsigned short Frame;
  };


//I2C Input Pins in order LSB first/top (Little Endian)

#ifndef __cplusplus
typedef
#endif
union Slow_INP_t
  {
  struct
    {
    unsigned int F_TACH1    : 1;    // 0..1   (1 bits)
    unsigned int F_TACH2    : 1;    // 0..1   (1 bits)
    unsigned int F_TACH3    : 1;    // 0..1   (1 bits)
    unsigned int F_TACH4    : 1;    // 0..1   (1 bits)
    unsigned int F_TACH5    : 1;    // 0..1   (1 bits)
    unsigned int F_TACH6    : 1;    // 0..1   (1 bits)
    unsigned int TP_06      : 1;    // 0..1   (1 bits)
    unsigned int TP_07      : 1;    // 0..1   (1 bits)

    unsigned int DRDY_T12   : 1;    // 0..1   (1 bits)
    unsigned int DRDY_T34   : 1;    // 0..1   (1 bits)
    unsigned int DRDY_T56   : 1;    // 0..1   (1 bits)
    unsigned int TP_13      : 1;    // 0..1   (1 bits)
    unsigned int TP_14      : 1;    // 0..1   (1 bits)
    unsigned int TP_15      : 1;    // 0..1   (1 bits)
    unsigned int TP_16      : 1;    // 0..1   (1 bits)
    unsigned int FAULT_N    : 1;    // 0..1   (1 bits)
    } Bits;
    unsigned char Bytes[2];
    unsigned short Frame;
  };


//SPI DAC output control word in order LSB first/top (Little Endian)

#ifndef __cplusplus
typedef
#endif
union Slow_DAC_OUT_t
  {
  struct
    {
    unsigned int OUTSEL_7   : 1;    // 0..1   (1 bits)  // Valid values are 1-6
    unsigned int OUTSEL_6   : 1;    // 0..1   (1 bits)
    unsigned int OUTSEL_5   : 1;    // 0..1   (1 bits)
    unsigned int OUTSEL_4   : 1;    // 0..1   (1 bits)
    unsigned int OUTSEL_3   : 1;    // 0..1   (1 bits)
    unsigned int OUTSEL_2   : 1;    // 0..1   (1 bits) //Used
    unsigned int OUTSEL_1   : 1;    // 0..1   (1 bits) //Used
    unsigned int OUTSEL_0   : 1;    // 0..1   (1 bits) //Used

    unsigned int OUTVAL_7   : 1;    // 0..1   (1 bits)
    unsigned int OUTVAL_6   : 1;    // 0..1   (1 bits)
    unsigned int OUTVAL_5   : 1;    // 0..1   (1 bits)
    unsigned int OUTVAL_4   : 1;    // 0..1   (1 bits)
    unsigned int OUTVAL_3   : 1;    // 0..1   (1 bits)
    unsigned int OUTVAL_2   : 1;    // 0..1   (1 bits)
    unsigned int OUTVAL_1   : 1;    // 0..1   (1 bits)
    unsigned int OUTVAL_0   : 1;    // 0..1   (1 bits)
    } Bits;
    unsigned char Bytes[2];
    unsigned short Frame;
  };


/*** hardware structure externs ***/

extern Slow_Motor_t Slow_Motor;
extern Slow_ENB_LCD_t Slow_ENB_LCD;
extern Slow_INP_t Slow_INP;
extern Slow_DAC_OUT_t Slow_DAC_OUT;


/*** function prototypes ***/

void hardware_init(void);			// master hardware init
stat_t hw_set_hv(cmdObj_t *cmd);
stat_t hw_get_id(cmdObj_t *cmd);


#ifdef __TEXT_MODE

	void hw_print_fb(cmdObj_t *cmd);
	void hw_print_fv(cmdObj_t *cmd);
	void hw_print_hp(cmdObj_t *cmd);
	void hw_print_hv(cmdObj_t *cmd);
	void hw_print_id(cmdObj_t *cmd);

#else

	#define hw_print_fb tx_print_stub
	#define hw_print_fv tx_print_stub
	#define hw_print_hp tx_print_stub
	#define hw_print_hv tx_print_stub
	#define hw_print_id tx_print_stub

#endif // __TEXT_MODE

#ifdef __cplusplus
}
#endif

#endif	// end of include guard: HARDWARE_H_ONCE
