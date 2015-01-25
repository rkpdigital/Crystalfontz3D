/*
 * FILE NAME:  stepper.cpp - stepper motor controls
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
 * PURPOSE:	This code provides the low-level stepper drivers and some related
 * 	functions. It dequeues lines queued by the motor_queue routines.
 * 	This is some of the most heavily optimized code in the project.
 *
 * NOTES:  See stepper.h for a detailed explanation of this module
 *
 *
 * CHANGE HISTORY:
 *
 *    Revision: Initial 1.0
 *    User: R.K.Parker     Date: 05/29/14
 *    First prototype.
 *
 */

#include "tinyg2.h"  // 1
#include "util.h"    // 2
#include "config.h"
#include "stepper.h"
#include "planner.h"
#include "hardware.h"
#include "text_parser.h"
#include "cfa10049_fiq.h"


//#define ENABLE_DIAGNOSTICS
#ifdef ENABLE_DIAGNOSTICS
#define INCREMENT_DIAGNOSTIC_COUNTER(motor) st_run.m[motor].step_count_diagnostic++;
#else
#define INCREMENT_DIAGNOSTIC_COUNTER(motor)	// choose this one to disable counters
#endif

/**** Allocate structures ****/

stConfig_t st;
static stRunSingleton_t st_run;
static stPrepSingleton_t st_prep;
static bool Do_a_Step;
static fiq_line_t FIQ_Step_Clr;
static fiq_line_t FIQ_Step_Out;

/**** Setup local functions ****/

static void _load_move(void);
static void _request_load_move(void);
static void _clear_diagnostic_counters(void);

// handy macro
#define _f_to_period(f) (uint16_t)((float)F_CPU / (float)f)

//RKP TBD used
//volatile int MaxLoops = 0;
//int loopcount = 0;
//OutputPin<motor_common_enable_pin_num> common_enable;	 // shorter form of the above
//OutputPin<31> dda_debug_pin1;
//OutputPin<33> dda_debug_pin2;



/************************************************************************************
 **** CODE **************************************************************************
 ************************************************************************************/
/*
 * stepper_init() - initialize stepper motor subsystem
 *
 *	Notes:
 *	  - This init requires sys_init() to be run beforehand
 * 	  - microsteps are setup during config_init()
 *	  - motor polarity is setup during config_init()
 *	  - high level interrupts must be enabled in main() once all inits are complete
 */
void stepper_init()
{
	memset(&st_run, 0, sizeof(st_run));		// clear all values, pointers and status
	st_run.magic_start = MAGICNUM;
	st_prep.magic_start = MAGICNUM;
	_clear_diagnostic_counters();

    Do_a_Step = false;
    FIQ_Step_Out.cell.timer = ALL_ZEROES;  // Clear the initial buffer values
    FIQ_Step_Out.cell.clear = ALL_STEPS;
    FIQ_Step_Out.cell.set = ALL_ZEROES;

	// setup DDA timer (see FOOTNOTE)
//	dda_timer.setInterrupts(kInterruptOnOverflow | kInterruptOnMatchA | kInterruptPriorityHighest);
//	dda_timer.setDutyCycleA(0.25);

	// setup DWELL timer
//	dwell_timer.setInterrupts(kInterruptOnOverflow | kInterruptPriorityHighest);

	st_prep.exec_state = PREP_BUFFER_OWNED_BY_EXEC;		// initial condition
}


static void _clear_diagnostic_counters()
{
	st_run.m[MOTOR_1].step_count_diagnostic = 0;
	st_run.m[MOTOR_2].step_count_diagnostic = 0;
	st_run.m[MOTOR_3].step_count_diagnostic = 0;
	st_run.m[MOTOR_4].step_count_diagnostic = 0;
	st_run.m[MOTOR_5].step_count_diagnostic = 0;
	st_run.m[MOTOR_6].step_count_diagnostic = 0;
}


/*
 * st_assertions() - test assertions, return error code if violation exists
 */
stat_t st_assertions()
{
	if (st_run.magic_start  != MAGICNUM) return (STAT_MEMORY_FAULT);
	if (st_prep.magic_start != MAGICNUM) return (STAT_MEMORY_FAULT);
	return (STAT_OK);
}


/*
 * stepper_isbusy() - return TRUE if motors are running or a dwell is running
 */
uint8_t stepper_isbusy()
{
	if (st_run.dda_ticks_downcount == 0) {
		return (false);
	}
	return (true);
}


/*
 * Motor power management functions
 *
 * _motor_energize_ctrl()		- apply or remove power to a motor 0,1 -  1 is deenergize.
 * st_set_motor_power()			- set motor a specified power level
 * st_energize_motors()			- apply power to all motors
 * st_deenergize_motors()		- remove power from all motors
 * st_motor_power_callback()	- callback to manage motor power sequencing
 */
static void _motor_energize_ctrl(const uint8_t motor, unsigned int denergize )
{
    switch(motor)
    {
        case (1):
        {
            Slow_Motor.Bits.ENABLE_X_N = denergize;
            break;
		}
        case (2):
        {
            Slow_Motor.Bits.ENABLE_Y_N = denergize;
            break;
		}
        case (3):
        {
            Slow_Motor.Bits.ENABLE_Z_N = denergize;
            break;
		}
        case (4):
        {
            Slow_Motor.Bits.ENABLE_A_N = denergize;
            break;
		}
        case (5):
        {
            Slow_Motor.Bits.ENABLE_B_N = denergize;
            break;
		}
        default:
            break;
    }

    if( denergize == 0 )
	    st_run.m[motor].power_state = MOTOR_START_IDLE_TIMEOUT;
	else
	    st_run.m[motor].power_state = MOTOR_OFF;
}


void st_set_motor_power(const uint8_t motor, uint8_t power)
{	// code for PWM driven Vref goes here
}


void st_energize_motors()
{
    Slow_Motor.Bits.ENABLE_X_N = 0;  // clear enables the motor
    Slow_Motor.Bits.ENABLE_Y_N = 0;
    Slow_Motor.Bits.ENABLE_Z_N = 0;
    Slow_Motor.Bits.ENABLE_A_N = 0;
    Slow_Motor.Bits.ENABLE_B_N = 0;
//	common_enable.clear();			// enable gShield common enable

	st_run.m[MOTOR_1].power_state = MOTOR_START_IDLE_TIMEOUT;
	st_run.m[MOTOR_2].power_state = MOTOR_START_IDLE_TIMEOUT;
	st_run.m[MOTOR_3].power_state = MOTOR_START_IDLE_TIMEOUT;
	st_run.m[MOTOR_4].power_state = MOTOR_START_IDLE_TIMEOUT;
	st_run.m[MOTOR_5].power_state = MOTOR_START_IDLE_TIMEOUT;
}


void st_deenergize_motors()
{
    Slow_Motor.Bits.ENABLE_X_N = 1;  // set disables the motor
    Slow_Motor.Bits.ENABLE_Y_N = 1;
    Slow_Motor.Bits.ENABLE_Z_N = 1;
    Slow_Motor.Bits.ENABLE_A_N = 1;
    Slow_Motor.Bits.ENABLE_B_N = 1;
//common_enable.set();			// disable gShield common enable
}


stat_t st_motor_power_callback() 	// called by controller
{
	// manage power for each motor individually - facilitates advanced features
	for (uint8_t motor = MOTOR_1; motor < MOTORS; motor++) {

		if (st.m[motor].power_mode == MOTOR_ENERGIZED_DURING_CYCLE) {

			switch (st_run.m[motor].power_state) {
			    case (MOTOR_START_IDLE_TIMEOUT): {
//				st_run.m[motor].power_systick = SysTickTimer_getValue() + (uint32_t)(st.motor_idle_timeout * 1000);
				st_run.m[motor].power_state = MOTOR_TIME_IDLE_TIMEOUT;
				break;
				}

				case (MOTOR_TIME_IDLE_TIMEOUT): {
//				    if (SysTickTimer_getValue() > st_run.m[motor].power_systick ) {
					st_run.m[motor].power_state = MOTOR_IDLE;
					_motor_energize_ctrl(motor, 1);  //Set denergize to 1 to deenergize
//					}
				    break;
			    }
			}
			} else if(st.m[motor].power_mode == MOTOR_IDLE_WHEN_STOPPED) {
			switch (st_run.m[motor].power_state) {
			    case (MOTOR_START_IDLE_TIMEOUT): {
//				st_run.m[motor].power_systick = SysTickTimer_getValue() + (uint32_t)(IDLE_TIMEOUT_SECONDS * 1000);
				st_run.m[motor].power_state = MOTOR_TIME_IDLE_TIMEOUT;
				break;
				}

			    case (MOTOR_TIME_IDLE_TIMEOUT): {
//				if (SysTickTimer_getValue() > st_run.m[motor].power_systick ) {
					st_run.m[motor].power_state = MOTOR_IDLE;
					_motor_energize_ctrl(motor, 1);  //Set denergize to 1 to deenergize
//				}
				break;
			    }
			}

//		} else if(st_run.m[motor].power_mode == MOTOR_POWER_REDUCED_WHEN_IDLE) {	// future

//		} else if(st_run.m[motor].power_mode == DYNAMIC_MOTOR_POWER) {			// future

		}
	}
	return (STAT_OK);
}


/****************************************************************************************
 * FIQ output routine - Sequence and write output instructions to the FIQ file.
 *
 *	This interrupt is really 2 interrupts. It fires on timer overflow and also on match.
 *	Overflow interrupts are used to set step pins, match interrupts clear step pins.
 *	This way the duty cycle of the stepper pulse can be controlled by setting the match value.
 *
 *  Notes: Double tap the interrupt or the FIQ entry writes to do each step in two stages:
 *         (Could double the interrupt rate so that each write or interrupt is 1/2 usec so
 *         each double tap is at the maximum possible speed/resolution of 1mhz.)
 *
 *         The first stage sets/clears the direction bits and clears all the step pins.
 *         The second stage, one tick later, loads the new step pins value and the delay until
 *         the next interrupt pair.
 *
 *         This can be done with the existing interrupt code by making double the FIQ entries.
 *
 *         Byte 1 of the combined Dirs/Steps longword is currently unused and could be state
 *         monitoring commands.  (Stop on switch or error.)
 *         (Or the switches could be continuously monitored every interrupt.)
 *
 *         A new FIQ interrupt handler with built in double tap could use only one longword
 *         for the set/clear data.
 ****************************************************************************************/
void _output_to_FIQ() 	// Was an interrupt but now it's called _load_move
{
  unsigned int Next_Step;

//loopcount = 0;
    do  // Repeat this loop until The move time has been used up and push out motor substep sequences.
    {
        Next_Step =  ALL_ZEROES;

        if ( st_run.m[MOTOR_1].power_state == MOTOR_RUNNING )
            if ( (st_run.m[MOTOR_1].phase_accumulator += st_run.m[MOTOR_1].phase_increment) > 0 )
            {
 		st_run.m[MOTOR_1].phase_accumulator -= st_run.dda_ticks_X_substeps;

		Next_Step |= X_STEP_BIT;		// turn step bit on
		INCREMENT_DIAGNOSTIC_COUNTER(MOTOR_1);
		Do_a_Step = true;
            }


        if ( st_run.m[MOTOR_2].power_state == MOTOR_RUNNING )
            if ( (st_run.m[MOTOR_2].phase_accumulator += st_run.m[MOTOR_2].phase_increment) > 0 )
            {
 		st_run.m[MOTOR_2].phase_accumulator -= st_run.dda_ticks_X_substeps;

		Next_Step |= Y_STEP_BIT;		// turn step bit on
		INCREMENT_DIAGNOSTIC_COUNTER(MOTOR_2);
		Do_a_Step = true;
            }

        if ( st_run.m[MOTOR_3].power_state == MOTOR_RUNNING )
            if ( (st_run.m[MOTOR_3].phase_accumulator += st_run.m[MOTOR_3].phase_increment) > 0 )
            {
 		st_run.m[MOTOR_3].phase_accumulator -= st_run.dda_ticks_X_substeps;

		Next_Step |= Z_STEP_BIT;		// turn step bit on
		INCREMENT_DIAGNOSTIC_COUNTER(3);
		Do_a_Step = true;
            }

        if ( st_run.m[MOTOR_4].power_state == MOTOR_RUNNING )
            if ( (st_run.m[MOTOR_4].phase_accumulator += st_run.m[MOTOR_4].phase_increment) > 0 )
            {
 		st_run.m[MOTOR_4].phase_accumulator -= st_run.dda_ticks_X_substeps;

		Next_Step |= A_STEP_BIT;		// turn step bit on
		INCREMENT_DIAGNOSTIC_COUNTER(MOTOR_4);
		Do_a_Step = true;
            }

        if ( st_run.m[MOTOR_5].power_state == MOTOR_RUNNING )
            if ( (st_run.m[MOTOR_5].phase_accumulator += st_run.m[MOTOR_5].phase_increment) > 0 )
            {
 		st_run.m[MOTOR_5].phase_accumulator -= st_run.dda_ticks_X_substeps;

		Next_Step |= B_STEP_BIT;		// turn step bit on
		INCREMENT_DIAGNOSTIC_COUNTER(MOTOR_5);
		Do_a_Step = true;
            }
//loopcount++;
        if ( Do_a_Step == true )
        {
//loopcount++;
//loopcount--;
//#ifdef __LEGACY_FIQ
            fwrite(&FIQ_Step_Clr.value, 12, 1, Fout_fp);
//#endif
            fwrite(&FIQ_Step_Out.value, 12, 1, Fout_fp);

            FIQ_Step_Out.cell.set = Next_Step | FIQ_Step_Clr.cell.set;  // Preset the Next set for the Dirs.
            FIQ_Step_Out.cell.timer = 0x00000000;

	  	    Do_a_Step = false;
        }

        FIQ_Step_Out.cell.timer += 1;
    }
    while (--st_run.dda_ticks_downcount > 0);  //End of Repeat loop.
//if ( loopcount < MaxLoops )
//     MaxLoops = loopcount;

}


/****************************************************************************************
 * Exec sequencing code - computes and prepares next load segment
 * Used to be a software interrupt. Now it just executes this code.
 * st_request_exec_move()	- - Does what was the exec_timer interrupt to call exec function
 */
void st_request_exec_move()
{
	if (st_prep.exec_state == PREP_BUFFER_OWNED_BY_EXEC)	// bother interrupting
    {
		if (mp_exec_move() != STAT_NOOP)
		{
			st_prep.exec_state = PREP_BUFFER_OWNED_BY_LOADER; // flip it back
	        if (st_run.dda_ticks_downcount == 0)  //If the loader is ready.
	        {
	            _load_move();
	        }
		}
	}
}


/*
 * _load_move() - Dequeue move and load into stepper struct
 *
 *
 *	In aline() code:
 *	 - All axes must set steps and compensate for out-of-range pulse phasing.
 *	 - If axis has 0 steps the direction setting can be omitted
 *	 - If axis has 0 steps the motor must not be enabled to support power mode = 1
 */
void _load_move()
{
	// handle aline() loads first (most common case)  NB: there are no more lines, only alines()
	if (st_prep.move_type == MOVE_TYPE_ALINE)
	{
		st_run.dda_ticks_downcount = st_prep.dda_ticks;
		st_run.dda_ticks_X_substeps = st_prep.dda_ticks_X_substeps;

        FIQ_Step_Clr.cell.timer = 0x00000001;  // Clear the initial buffer values and set one Tick.
        FIQ_Step_Clr.cell.clear = ALL_STEPS;
        FIQ_Step_Clr.cell.set = ALL_ZEROES;

		st_run.m[MOTOR_1].phase_increment = st_prep.m[MOTOR_1].phase_increment;

		if (st_prep.reset_flag == true)           // compensate for pulse phasing
			st_run.m[MOTOR_1].phase_accumulator = -st_prep.m[MOTOR_1].phase_increment;

		if (st_run.m[MOTOR_1].phase_increment != 0) 	// motor is in this move
        {
			if (st_prep.m[MOTOR_1].dir == 0)
                FIQ_Step_Clr.cell.clear |= X_DIR_BIT;
            else
                FIQ_Step_Clr.cell.set |= X_DIR_BIT;

			st_run.m[MOTOR_1].power_state = MOTOR_RUNNING;
	    }
	    else
	    {
		if (st.m[MOTOR_1].power_mode == MOTOR_IDLE_WHEN_STOPPED)
		    st_run.m[MOTOR_1].power_state = MOTOR_START_IDLE_TIMEOUT;
                else
                    st_run.m[MOTOR_1].power_state = MOTOR_STOPPED;
	    }

        st_run.m[MOTOR_2].phase_increment = st_prep.m[MOTOR_2].phase_increment;

        if (st_prep.reset_flag == true)           // compensate for pulse phasing
            st_run.m[MOTOR_2].phase_accumulator = -st_prep.m[MOTOR_2].phase_increment;

        if (st_run.m[MOTOR_2].phase_increment != 0) 	// motor is in this move
        {
			if (st_prep.m[MOTOR_2].dir == 0)
                FIQ_Step_Clr.cell.clear |= Y_DIR_BIT;
            else
                FIQ_Step_Clr.cell.set |= Y_DIR_BIT;

            st_run.m[MOTOR_2].power_state = MOTOR_RUNNING;
        }
        else
        {
            if (st.m[MOTOR_2].power_mode == MOTOR_IDLE_WHEN_STOPPED)
                st_run.m[MOTOR_2].power_state = MOTOR_START_IDLE_TIMEOUT;
                else
                    st_run.m[MOTOR_2].power_state = MOTOR_STOPPED;
        }

        st_run.m[MOTOR_3].phase_increment = st_prep.m[MOTOR_3].phase_increment;

        if (st_prep.reset_flag == true)           // compensate for pulse phasing
            st_run.m[MOTOR_3].phase_accumulator = -st_prep.m[MOTOR_3].phase_increment;

        if (st_run.m[MOTOR_3].phase_increment != 0) 	// motor is in this move
        {
			if (st_prep.m[MOTOR_3].dir == 0)
                FIQ_Step_Clr.cell.clear |= Z_DIR_BIT;
            else
                FIQ_Step_Clr.cell.set |= Z_DIR_BIT;

            st_run.m[MOTOR_3].power_state = MOTOR_RUNNING;
        }
        else
        {
            if (st.m[MOTOR_3].power_mode == MOTOR_IDLE_WHEN_STOPPED)
                st_run.m[MOTOR_3].power_state = MOTOR_START_IDLE_TIMEOUT;
                else
                    st_run.m[MOTOR_3].power_state = MOTOR_STOPPED;
        }

        st_run.m[MOTOR_4].phase_increment = st_prep.m[MOTOR_4].phase_increment;

        if (st_prep.reset_flag == true)           // compensate for pulse phasing
            st_run.m[MOTOR_4].phase_accumulator = -st_prep.m[MOTOR_4].phase_increment;

        if (st_run.m[MOTOR_4].phase_increment != 0) 	// motor is in this move
        {
			if (st_prep.m[MOTOR_4].dir == 0)
                FIQ_Step_Clr.cell.clear |= A_DIR_BIT;
            else
                FIQ_Step_Clr.cell.set |= A_DIR_BIT;

            st_run.m[MOTOR_4].power_state = MOTOR_RUNNING;
        }
        else
        {
            if (st.m[MOTOR_4].power_mode == MOTOR_IDLE_WHEN_STOPPED)
                st_run.m[MOTOR_4].power_state = MOTOR_START_IDLE_TIMEOUT;
                else
                    st_run.m[MOTOR_4].power_state = MOTOR_STOPPED;
        }

        st_run.m[MOTOR_5].phase_increment = st_prep.m[MOTOR_5].phase_increment;

        if (st_prep.reset_flag == true)           // compensate for pulse phasing
            st_run.m[MOTOR_5].phase_accumulator = -st_prep.m[MOTOR_5].phase_increment;

        if (st_run.m[MOTOR_5].phase_increment != 0) 	// motor is in this move
        {
			if (st_prep.m[MOTOR_5].dir == 0)
                FIQ_Step_Clr.cell.clear |= B_DIR_BIT;
            else
                FIQ_Step_Clr.cell.set |= B_DIR_BIT;

            st_run.m[MOTOR_5].power_state = MOTOR_RUNNING;
        }
        else
        {
            if (st.m[MOTOR_5].power_mode == MOTOR_IDLE_WHEN_STOPPED)
                st_run.m[MOTOR_5].power_state = MOTOR_START_IDLE_TIMEOUT;
                else
                    st_run.m[MOTOR_5].power_state = MOTOR_STOPPED;
        }

        FIQ_Step_Out.cell.clear = ALL_DIRS & FIQ_Step_Clr.cell.clear;  // Preset the Clear for the Dirs.
        _output_to_FIQ();
	}
	else if (st_prep.move_type == MOVE_TYPE_DWELL)  // handle dwells
	{
        FIQ_Step_Out.cell.timer = st_prep.dda_ticks; //Directly add to the FIQ delay counter.
        FIQ_Step_Out.cell.clear = ALL_STEPS;         // Preset the Clear to clear all steps in the first phase.
        FIQ_Step_Out.cell.set = ALL_ZEROES;          // Do nothing.
//        fwrite(&FIQ_Step_Out.value, 12, 1, Fout_fp);

        st_run.dda_ticks_downcount = 0; //Ready to load the next move.
	}

	// all cases drop to here - such as Null moves queued by MCodes
	st_prep_null();						// needed to shut off timers if no moves left
	st_prep.exec_state = PREP_BUFFER_OWNED_BY_EXEC;		// flip it back
	st_request_exec_move();					// compute and prepare the next move
}


/*
 * st_prep_null() - Keeps the loader happy. Otherwise performs no action
 *
 *	Used by M codes, tool and spindle changes
 */
void st_prep_null()
{
	st_prep.move_type = MOVE_TYPE_NULL;
}


/*
 * st_prep_dwell() 	 - Add a dwell to the move buffer
 */
void st_prep_dwell(float microseconds)
{
	st_prep.move_type = MOVE_TYPE_DWELL;
	st_prep.dda_ticks = (uint32_t)((microseconds/1000000) * FREQUENCY_DWELL); // ARM code
}


/***********************************************************************************
 * st_prep_line() - Prepare the next move for the loader
 *
 *	This function does the math on the next pulse segment and gets it ready for
 *	the loader. It deals with all the DDA optimizations and timer setups so that
 *	loading can be performed as rapidly as possible. It works in joint space
 *	(motors) and it works in steps, not length units. All args are provided as
 *	floats and converted to their appropriate integer types for the loader.
 *
 * Args:
 *	steps[] are signed relative motion in steps per tick (can be non-integer values)
 *	Microseconds - how many microseconds the segment should run
 */
stat_t st_prep_line(float steps[], float microseconds)
{
	// *** defensive programming ***
	// trap conditions that would prevent queuing the line
	if (st_prep.exec_state != PREP_BUFFER_OWNED_BY_EXEC) { return (STAT_INTERNAL_ERROR);
	} else if (isfinite(microseconds) == false) { return (STAT_INPUT_EXCEEDS_MAX_LENGTH);
	} else if (microseconds < EPSILON) { return (STAT_MINIMUM_TIME_MOVE_ERROR);
	}
	st_prep.reset_flag = false;         // initialize accumulator reset flag for this move.

//    st_prep.dda_ticks = (uint32_t)((microseconds + .5) * (FREQUENCY_DDA/1000000));
	st_prep.dda_ticks = (uint32_t)((microseconds/1000000) * FREQUENCY_DDA);
	st_prep.dda_ticks_X_substeps = st_prep.dda_ticks * DDA_SUBSTEPS;

	// FOOTNOTE: The above expression was previously computed as below but floating
	// point rounding errors caused subtle and nasty accumulated position errors:
	// sp.dda_ticks_X_substeps = (uint32_t)((microseconds/1000000) * FREQUENCY_DDA * DDA_SUBSTEPS);

    // setup motor parameters
        for (uint8_t i=0; i<MOTORS; i++)
        {
            st_prep.m[i].dir = ((steps[i] < 0) ? 1 : 0) ^ st.m[i].polarity;
		    st_prep.m[i].phase_increment = (uint32_t)fabs(steps[i] * DDA_SUBSTEPS);
//printf("Motor %d has %lf steps the result is %d\n", i, steps[i], st_prep.m[i].phase_increment);
        }

    // anti-stall measure in case change in velocity between segments is too great
	if ((st_prep.dda_ticks * ACCUMULATOR_RESET_FACTOR) < st_prep.prev_ticks) {  // NB: uint32_t math
		st_prep.reset_flag = true;
	}
	st_prep.prev_ticks = st_prep.dda_ticks;
	st_prep.move_type = MOVE_TYPE_ALINE;
	return (STAT_OK);
}


/*
 * _set_hw_microsteps() - set microsteps in hardware
 *
 *	For now the microstep_mode is the same as the microsteps (1,2,4,8)
 *	Microsteps (16) is different and requires morphing.
 */
static void _set_hw_microsteps(const uint8_t motor, const uint8_t microstep_mode)
{
  unsigned int MS0 = 0, MS1 = 0, MS2 = 0;

	if (microstep_mode == 16) {
		MS0 = 1;
		MS1 = 1;
		MS2 = 1;
	} else if (microstep_mode == 8) {
		MS0 = 1;
		MS1 = 1;
	} else if (microstep_mode == 4) {
		MS1 = 1;
	} else if (microstep_mode == 2) {
		MS0 = 1;
	}

    switch(motor)
    {
        case (1):
        {
            Slow_Motor.Bits.MS0_X = MS0;
            Slow_Motor.Bits.MS1_X = MS1;
            Slow_Motor.Bits.MS2_X = MS2;
            break;
        }
        case (2):
        {
            Slow_Motor.Bits.MS0_Y = MS0;
            Slow_Motor.Bits.MS1_Y = MS1;
            Slow_Motor.Bits.MS2_Y = MS2;
            break;
		}
        case (3):
        {
            Slow_Motor.Bits.MS0_Z = MS0;
            Slow_Motor.Bits.MS1_Z = MS1;
            Slow_Motor.Bits.MS2_Z = MS2;
            break;
		}
        case (4):
        {
            Slow_Motor.Bits.MS0_A = MS0;
            Slow_Motor.Bits.MS1_A = MS1;
            Slow_Motor.Bits.MS2_A = MS2;
            break;
		}
        case (5):
        {
            Slow_Motor.Bits.MS0_B = MS0;
            Slow_Motor.Bits.MS1_B = MS1;
            Slow_Motor.Bits.MS2_B = MS2;
            break;
		}
        default:
            break;
    }
}


/***********************************************************************************
 * CONFIGURATION AND INTERFACE FUNCTIONS
 * Functions to get and set variables from the cfgArray table
 ***********************************************************************************/

/*
 * _get_motor() - helper to return motor number as an index or -1 if na
 */
static int8_t _get_motor(const index_t index)
{
	char *ptr;
    char motors[] = {"123456"};
	char tmp[CMD_TOKEN_LEN+1];

    strcpy(tmp, cfgArray[index].group);
    if ((ptr = strchr(motors, tmp[0])) == NULL)
    {
		return (-1);
	}
	return (ptr - motors);
}


/*
 * _set_motor_steps_per_unit() - what it says
 * This function will need to be rethought if microstep morphing is implemented
 */
static void _set_motor_steps_per_unit(cmdObj_t *cmd)
{
    uint8_t m = _get_motor(cmd->index);
    if (m != 255)
        st.m[m].steps_per_unit = (360 / (st.m[m].step_angle / st.m[m].microsteps) / st.m[m].travel_rev);
    else
        printf("Invalid motor number \n");
}


stat_t st_set_sa(cmdObj_t *cmd)			// motor step angle
{
	set_flt(cmd);
	_set_motor_steps_per_unit(cmd);
	return(STAT_OK);
}


stat_t st_set_tr(cmdObj_t *cmd)			// motor travel per revolution
{
	set_flu(cmd);
	_set_motor_steps_per_unit(cmd);
	return(STAT_OK);
}


stat_t st_set_mi(cmdObj_t *cmd)			// motor microsteps
{
	if (fp_NE(cmd->value,1) && fp_NE(cmd->value,2) && fp_NE(cmd->value,4) && fp_NE(cmd->value,8)) {
		cmd_add_conditional_message((const char *)"*** WARNING *** Setting non-standard microstep value");
	}
	set_ui8(cmd);							// set it anyway, even if it's unsupported
	_set_motor_steps_per_unit(cmd);
	_set_hw_microsteps(_get_motor(cmd->index), (uint8_t)cmd->value);
	return (STAT_OK);
}


stat_t st_set_pm(cmdObj_t *cmd)			// motor power mode
{
	ritorno (set_01(cmd));
	if (fp_ZERO(cmd->value)) { // people asked this setting take effect immediately, hence:
        _motor_energize_ctrl(_get_motor(cmd->index), 0);  //Set denergize to 0 to energize
	} else {
        _motor_energize_ctrl(_get_motor(cmd->index), 1);  //Set denergize to 1 to deenergize
	}
	return (STAT_OK);
}


stat_t st_set_mt(cmdObj_t *cmd)
{
	st.motor_idle_timeout = min(IDLE_TIMEOUT_SECONDS_MAX, max(cmd->value, IDLE_TIMEOUT_SECONDS_MIN));
	return (STAT_OK);
}


stat_t st_set_md(cmdObj_t *cmd)	// Make sure this function is not part of initialization --> f00
{
	st_deenergize_motors();
	return (STAT_OK);
}


stat_t st_set_me(cmdObj_t *cmd)	// Make sure this function is not part of initialization --> f00
{
	st_energize_motors();
	return (STAT_OK);
}


/***********************************************************************************
 * TEXT MODE SUPPORT
 * Functions to print variables from the cfgArray table
 ***********************************************************************************/

#ifdef __TEXT_MODE

static const char msg_units0[] PROGMEM = " in";	// used by generic print functions
static const char msg_units1[] PROGMEM = " mm";
static const char msg_units2[] PROGMEM = " deg";
static const char *const msg_units[] PROGMEM = { msg_units0, msg_units1, msg_units2 };
#define DEGREE_INDEX 2

static const char fmt_mt[] PROGMEM = "[mt]  motor idle timeout%14.2f Sec\n";
static const char fmt_me[] PROGMEM = "motors energized\n";
static const char fmt_md[] PROGMEM = "motors de-energized\n";
static const char fmt_0ma[] PROGMEM = "[%s%s] m%s map to axis%15d [0=X,1=Y,2=Z...]\n";
static const char fmt_0sa[] PROGMEM = "[%s%s] m%s step angle%20.3f%s\n";
static const char fmt_0tr[] PROGMEM = "[%s%s] m%s travel per revolution%9.3f%s\n";
static const char fmt_0mi[] PROGMEM = "[%s%s] m%s microsteps%16d [1,2,4,8]\n";
static const char fmt_0po[] PROGMEM = "[%s%s] m%s polarity%18d [0=normal,1=reverse]\n";
static const char fmt_0pm[] PROGMEM = "[%s%s] m%s power management%10d [0=remain powered,1=power down when idle]\n";

void st_print_mt(cmdObj_t *cmd) { text_print_flt(cmd, fmt_mt);}
void st_print_me(cmdObj_t *cmd) { text_print_nul(cmd, fmt_me);}
void st_print_md(cmdObj_t *cmd) { text_print_nul(cmd, fmt_md);}

static void _print_motor_ui8(cmdObj_t *cmd, const char *format)
{
    fprintf(stderr, format, cmd->group, cmd->token, cmd->group, (uint8_t)cmd->value);
}

static void _print_motor_flt_units(cmdObj_t *cmd, const char *format, uint8_t units)
{
    fprintf(stderr, format, cmd->group, cmd->token, cmd->group, cmd->value, GET_TEXT_ITEM(msg_units, units));
}

void st_print_ma(cmdObj_t *cmd) { _print_motor_ui8(cmd, fmt_0ma);}
void st_print_sa(cmdObj_t *cmd) { _print_motor_flt_units(cmd, fmt_0sa, DEGREE_INDEX);}
void st_print_tr(cmdObj_t *cmd) { _print_motor_flt_units(cmd, fmt_0tr, cm_get_units_mode(MODEL));}
void st_print_mi(cmdObj_t *cmd) { _print_motor_ui8(cmd, fmt_0mi);}
void st_print_po(cmdObj_t *cmd) { _print_motor_ui8(cmd, fmt_0po);}
void st_print_pm(cmdObj_t *cmd) { _print_motor_ui8(cmd, fmt_0pm);}

#endif // __TEXT_MODE
