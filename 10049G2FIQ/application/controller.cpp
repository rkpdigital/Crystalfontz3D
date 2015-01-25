/*
 * FILE NAME: controller.cpp - exec controller and main dispatch loop
 *
 * Copyright (c) 2014 Robert K. Parker
 *
 * This file was part of the TinyG project
 *
 * Copyright (c) 2010 - 2013 Alden S. Hart, Jr.
 * Copyright (c) 2013 Robert Giseburt
 *
 * Now it is in crystalfontz3D
 *
 * This file ("the software") is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2 as published by the
 * Free Software Foundation. You should have received a copy of the GNU General Public
 * License, version 2 along with the software.  If not, see <http://www.gnu.org/licenses/>.
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
 * PURPOSE:	This code is the top level exec for command input and opening files.
 *
 *
 * CHANGE HISTORY:
 *
 *    Revision: Initial 1.0
 *    User: R.K.Parker     Date: 05/29/14
 *    First prototype.
 *
 */

#include "tinyg2.h"				// #1
#include "config.h"				// #2
#include "controller.h"
//#include "json_parser.h"
#include "text_parser.h"
#include "gcode_parser.h"
#include "canonical_machine.h"
#include "plan_arc.h"
#include "planner.h"
#include "stepper.h"
#include "hardware.h"
#include "switch.h"
//#include "gpio.h"
#include "report.h"
#include "help.h"
#include "util.h"
#include "xio.h"


/***********************************************************************************
 **** STRUCTURE ALLOCATIONS *********************************************************
 ***********************************************************************************/

controller_t cs;		// controller state structure

/***********************************************************************************
 **** STATICS AND LOCALS ***********************************************************
 ***********************************************************************************/

static void _controller_HSM(void);
static stat_t _alarm_idler(void);
static stat_t _normal_idler(void);
static stat_t _limit_switch_handler(void);
static stat_t _system_assertions(void);
static stat_t _sync_to_planner(void);
static stat_t _sync_to_tx_buffer(void);
static stat_t _command_dispatch(void);

// prep for export to other modules:
stat_t hardware_hard_reset_handler(void);
//stat_t hardware_bootloader_handler(void);

/***********************************************************************************
 **** CODE *************************************************************************
 ***********************************************************************************/
/*
 * controller_init() - controller init
 */

void controller_init(uint8_t std_in, uint8_t std_out, uint8_t std_err)
{
	cs.magic_start = MAGICNUM;
	cs.magic_end = MAGICNUM;
	cs.fw_build = TINYG_FIRMWARE_BUILD;
	cs.fw_version = TINYG_FIRMWARE_VERSION;
	cs.hw_platform = TINYG_HARDWARE_PLATFORM;		// NB: HW version is set from EEPROM

	cs.linelen = 0;									// initialize index for read_line()
	cs.state = CONTROLLER_NOT_CONNECTED;

// find USB next
//	cs.reset_requested = false;
//	cs.bootloader_requested = false;

//	xio_set_stdin(std_in);
//	xio_set_stdout(std_out);
//	xio_set_stderr(std_err);
//	cs.default_src = std_in;
}

/*
 * controller_run() - MAIN LOOP - top-level controller
 *
 * The order of the dispatched tasks is very important.
 * Tasks are ordered by increasing dependency (blocking hierarchy).
 * Tasks that are dependent on completion of lower-level tasks must be
 * later in the list than the task(s) they are dependent upon.
 *
 * Tasks must be written as continuations as they will be called repeatedly,
 * and are called even if they are not currently active.
 *
 * The DISPATCH macro calls the function and returns to the controller parent
 * if not finished (STAT_EAGAIN), preventing later routines from running
 * (they remain blocked). Any other condition - OK or ERR - drops through
 * and runs the next routine in the list.
 *
 * A routine that had no action (i.e. is OFF or idle) should return STAT_NOOP
 */

void controller_run()
{
	do
	{
		_controller_HSM();
	}
	while (cs.state != CONTROLLER_EXIT);
}

#define	DISPATCH(func) if (func == STAT_EAGAIN) return;
static void _controller_HSM()
{
//----- Interrupt Service Routines have been removed from this implementation of
//      Crystalfontz3D. In TinyG2 they were the highest priority controller functions
//      See hardware.h in TinyG2 for a list of ISRs and their priorities. ----//
//
//----- kernel level ISR handlers ----(flags are set in ISRs)------------------------//
												// Order is important:
//	DISPATCH(hw_hard_reset_handler());			// 1. handle hard reset requests
//	DISPATCH(hw_bootloader_handler());			// 2. handle requests to enter bootloader
	DISPATCH(_alarm_idler());					// 3. idle in alarm state (shutdown)
//	DISPATCH( poll_switches());					// 4. run a switch polling cycle
	DISPATCH(_limit_switch_handler());			// 5. limit switch has been thrown

	DISPATCH(cm_feedhold_sequencing_callback());// 6a. feedhold state machine runner
	DISPATCH(mp_plan_hold_callback());			// 6b. plan a feedhold from line runtime
	DISPATCH(_system_assertions());				// 7. system integrity assertions

//----- planner hierarchy for gcode and cycles ---------------------------------------//

	DISPATCH(st_motor_power_callback());		// stepper motor power sequencing
//	DISPATCH(switch_debounce_callback());		// debounce switches
	DISPATCH(sr_status_report_callback());		// conditionally send status report
	DISPATCH(qr_queue_report_callback());		// conditionally send queue report
	DISPATCH(cm_arc_callback());				// arc generation runs behind lines
	DISPATCH(cm_homing_callback());				// G28.2 continuation
//	DISPATCH(cm_probe_callback());				// G38.2 continuation

//----- command readers and parsers --------------------------------------------------//

	DISPATCH(_sync_to_planner());				// ensure there is at least one free buffer in planning queue
//	DISPATCH(_sync_to_tx_buffer());				// sync with TX buffer (pseudo-blocking)
//	DISPATCH(set_baud_callback());				// perform baud rate update (must be after TX sync)
	DISPATCH(_command_dispatch());				// read and execute next command
	DISPATCH(_normal_idler());					// blink LEDs slowly to show everything is OK
}

/*****************************************************************************
 * _command_dispatch() - dispatch line received from active input device
 *
 *	Reads next command line and dispatches to relevant parser or action
 *	Accepts commands if the move queue has room - EAGAINS if it doesn't
 *	Manages cutback to serial input from file devices (EOF)
 *	Also responsible for prompts and for flow control
 */

static stat_t _command_dispatch()
{

        if (cs.state == CONTROLLER_WORKING)
	{
	    // read a line from the Gcode file and check for end of file.
            if (NULL != fgets(cs.in_buf, sizeof(cs.in_buf), Gin_fp))
		{
			cs.linelen = strlen(cs.in_buf);
			cs.bufp = cs.in_buf;
                        cs.lineNumber++;
			if( (cs.lineNumber & 0xFF) == 0 )
                        {
                            printf("Line 0x%x\n", cs.lineNumber);
//                            printf("Line 0x%x Loop %d\n", cs.lineNumber, MaxLoops);
//                            MaxLoops = 6000;
                        }

		}
		else
		{
                    fclose(Gin_fp);
                    printf("Completed processing the G code file.\n");
		    if (!isCompressing)
                    {
                        fclose(Fout_fp);
                    }
                    else
                    {
                        Temp_fp = fopen(FcodePathFile, "wb");
                        if (!Temp_fp)
                        {
                            printf("Can't Open  the compressed file %s\n", FcodePathFile);
                            return -1;
                        }
                        printf("Saved %ld bytes in compression\n", compressFile(Fout_fp, Temp_fp));
                        fclose(Fout_fp);
                        fclose(Temp_fp);
                    }

                    cs.state = CONTROLLER_PROMPT;
                    return (STAT_OK);	// Exit if no string to process. returns OK for anything NOT OK, so the idler always runs
		}
	}
        else if (cs.state == CONTROLLER_NOT_CONNECTED)
	{
        if (strlen(GcodePathFile) == 0)
		{
		    cs.state = CONTROLLER_PROMPT;
		    cm_request_queue_flush();
                    return (STAT_OK);	// Exit if no string to process. returns OK for anything NOT OK, so the idler always runs
		}
		else
		{
                    Gin_fp = fopen(GcodePathFile, "r");
                    if (!Gin_fp)
                    {
                        printf("Can't Open the input file %s\n", GcodePathFile);
                        return -1;  // Failed.
                }

    // If you are compressing then we will write to a temp file for now. Other wise we will write to the specified file.
                if (!isCompressing)
                {
                    Fout_fp = fopen(FcodePathFile, "wb");
                    if (!Fout_fp)
                    {
                        printf("Can't Open the output file %s\n", FcodePathFile);
                        return -1;  // Failed.
                    }
                }
                else
                {
                    Fout_fp = fopen(TempPathFile, "w+b");
                    if (!Fout_fp)
                    {
                        printf("Can't Open a temporary file: %s\n", TempPathFile);
                        return -1;  // Failed.
                    }
                }

	        cm_request_queue_flush();
		cs.lineNumber = 0;
                cs.totalLineNumber = fLineCount(Gin_fp);
		cs.state = CONTROLLER_WORKING;
		return (STAT_OK);  // Exit if file process just started. returns OK for anything NOT OK, so the idler always runs
	    }
	}
	else if (cs.state == CONTROLLER_PROMPT)
	{
	    // Blocking read input line from stdin.
	    if (fgets(cs.in_buf, sizeof(cs.in_buf), stdin) == STAT_OK)
		{
		    cs.linelen = strlen(cs.in_buf);
		    cs.bufp = cs.in_buf;
		    return (STAT_OK);	// returns OK for anything NOT OK, so the idler always runs
		}

    }
	else
	{
	    return (STAT_OK);
	}

    // execute the text line from PROMPT or WORKING file.
    strncpy(cs.saved_buf, cs.bufp, SAVED_BUFFER_LEN-1);	// save input buffer for reporting
    cs.linelen = 0;

    // dispatch the new text line
    switch (toupper(*cs.bufp))
    {				// first char

        case NUL:
        { 							// blank line (just a CR)
            if (cfg.comm_mode != JSON_MODE) {
                text_response(STAT_OK, cs.saved_buf);
            }
            break;
        }
        case 'H':
        { 							// intercept help screens
            cfg.comm_mode = TEXT_MODE;
            help_general((cmdObj_t *)NULL);
            text_response(STAT_OK, cs.bufp);
            break;
        }
        case '$': case '?':
        { 					// text-mode configs
            cfg.comm_mode = TEXT_MODE;
            text_response(text_parser(cs.bufp), cs.saved_buf);
            break;
        }
        case '{':
        { 							// JSON input
            //cfg.comm_mode = JSON_MODE;
            //json_parser(cs.bufp);
            break;
        }
        case '&':
        { 							// Exit program
            cs.state = CONTROLLER_EXIT;
            break;
        }
        default:
        {								// anything else must be Gcode
            text_response(gc_gcode_parser(cs.bufp), cs.saved_buf);
        }
    }
	return (STAT_OK);
}

/**** Local Utilities ********************************************************/
/*
 * _alarm_idler() - blink rapidly and prevent further activity from occurring
 * _normal_idler() - blink Indicator LED slowly to show everything is OK
 *
 *	Alarm idler flashes indicator LED rapidly to show everything is not OK.
 *	Alarm function returns EAGAIN causing the control loop to never advance beyond
 *	this point. It's important that the reset handler is still called so a SW reset
 *	(ctrl-x) or bootloader request can be processed.
 */

static stat_t _alarm_idler()
{
	if (cm_get_machine_state() != MACHINE_ALARM) { return (STAT_OK);}

	return (STAT_EAGAIN);	// EAGAIN prevents any lower-priority actions from running
}

static stat_t _normal_idler()
{
	return (STAT_OK);
}

/*
 * _sync_to_tx_buffer() - return eagain if TX queue is backed up
 * _sync_to_planner() - return eagain if planner is not ready for a new command
 */

static stat_t _sync_to_tx_buffer()
{
//	if ((xio_get_tx_bufcount_usart(ds[XIO_DEV_USB].x) >= XOFF_TX_LO_WATER_MARK)) {
//		return (STAT_EAGAIN);
//	}
	return (STAT_OK);
}

static stat_t _sync_to_planner()
{
	if (mp_get_planner_buffers_available() < PLANNER_BUFFER_HEADROOM) { // allow up to N planner buffers for this line
		return (STAT_EAGAIN);
	}
	return (STAT_OK);
}

/*
 * _limit_switch_handler() - shut down system if limit switch fired
 */
static stat_t _limit_switch_handler(void)
{
/*
	if (cm_get_machine_state() == MACHINE_ALARM) { return (STAT_NOOP);}
	if (cm.limit_tripped_flag == false) { return (STAT_NOOP);}
	cm.limit_tripped_flag = false;
//	cm_alarm(0);
*/
	return (STAT_OK);
}

/*
 * _controller_assertions() - check memory integrity of controller
 */
stat_t _ct_assertions()
{
	if ((cs.magic_start != MAGICNUM) || (cs.magic_end != MAGICNUM)) return (STAT_MEMORY_FAULT);
	return (STAT_OK);
}

/*
 * _system_assertions() - check memory integrity and other assertions
 */
stat_t _system_assertions()
{
	stat_t status;

	for (;;) {	// run this loop only once, but enable breaks

		if ((status = _ct_assertions()) != STAT_OK) break;
		if ((status = cm_assertions()) != STAT_OK) break;
		if ((status = mp_assertions()) != STAT_OK) break;
		if ((status = st_assertions()) != STAT_OK) break;
// 		if ((status = xio_assertions()) != STAT_OK) break;
//		if (rtc.magic_end 		!= MAGICNUM) { value = 19; }
//		xio_assertions(&value);									// run xio assertions

		break;
	}
	if (status == STAT_OK) return (STAT_OK);
	cm_alarm(status);		// else report exception and shut down
	return (STAT_EAGAIN);	// do not allow main loop to advance beyond this point
}

