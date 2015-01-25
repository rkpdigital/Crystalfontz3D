/*
 * FILE NAME: hardware.cpp - system hardware configuration
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

#ifdef __AVR
#include <avr/wdt.h>			// used for software reset
#endif

#include "tinyg2.h"		// #1
#include "config.h"		// #2
#include "hardware.h"
#include "switch.h"
#include "controller.h"
#include "text_parser.h"

#ifdef __cplusplus
extern "C"{
#endif


/*** Global hardware structures ***/

Slow_Motor_t Slow_Motor;
Slow_ENB_LCD_t Slow_ENB_LCD;
Slow_INP_t Slow_INP;
Slow_DAC_OUT_t Slow_DAC_OUT;


/*
 * hardware_init() - lowest level hardware init
 */
void hardware_init()
{
	return;
}


/*
 * _get_id() - get a human readable signature
 *
 *	Produce a unique deviceID based on the factory calibration data.
 */
void _get_id(char *id)
{
	return;
}
 


/***********************************************************************************
 * CONFIGURATION AND INTERFACE FUNCTIONS
 * Functions to get and set variables from the cfgArray table
 ***********************************************************************************/

/*
 * hw_get_id() - get device ID (signature)
 */
stat_t hw_get_id(cmdObj_t *cmd) 
{
//	char tmp[SYS_ID_LEN];
//	_get_id(tmp);
//	cmd->objtype = TYPE_STRING;
//	ritorno(cmd_copy_string(cmd, tmp));
	return (STAT_OK);
}


/*
 * hw_set_hv() - set hardware version number
 */
stat_t hw_set_hv(cmdObj_t *cmd) 
{
	return (STAT_OK);
}


/***********************************************************************************
 * TEXT MODE SUPPORT
 * Functions to print variables from the cfgArray table
 ***********************************************************************************/

#ifdef __TEXT_MODE

static const char fmt_fb[] PROGMEM = "[fb]  firmware build%18.2f\n";
static const char fmt_fv[] PROGMEM = "[fv]  firmware version%16.2f\n";
static const char fmt_hp[] PROGMEM = "[hp]  hardware platform%15.2f\n";
static const char fmt_hv[] PROGMEM = "[hv]  hardware version%16.2f\n";
static const char fmt_id[] PROGMEM = "[id]  TinyG ID%30s\n";

void hw_print_fb(cmdObj_t *cmd) { text_print_flt(cmd, fmt_fb);}
void hw_print_fv(cmdObj_t *cmd) { text_print_flt(cmd, fmt_fv);}
void hw_print_hp(cmdObj_t *cmd) { text_print_flt(cmd, fmt_hp);}
void hw_print_hv(cmdObj_t *cmd) { text_print_flt(cmd, fmt_hv);}
void hw_print_id(cmdObj_t *cmd) { text_print_str(cmd, fmt_id);}

#endif //__TEXT_MODE 

#ifdef __cplusplus
}
#endif
