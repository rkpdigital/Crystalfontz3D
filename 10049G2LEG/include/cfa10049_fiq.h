/*
 * FILE NAME: cfa10049_fiq.h
 *
 * Copyright (c) 2014 Robert K. Parker
 * Copyright (C) 2012 Free Electrons
 *
 * This file is part of crystalfontz3D
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
 * PURPOSE: Crystalfontz CFA-10049 FIQ handler API
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

#include <linux/ioctl.h>

/*FIQ data defines.*/
#define FIQ_BUFFER_SIZE		(12 * 1024 * 1024)

#define FIQ_IOC_MAGIC            'p'
#define FIQ_START		_IO(FIQ_IOC_MAGIC, 0xb0)
#define FIQ_STOP		_IO(FIQ_IOC_MAGIC, 0xb1)
#define FIQ_RESET		_IO(FIQ_IOC_MAGIC, 0xb2)

#define FIQ_STATUS_STOPPED	(0)
#define FIQ_STATUS_RUNNING	(1 << 0)
#define FIQ_STATUS_ERR_URUN	(1 << 1)


#ifndef __cplusplus
typedef
#endif
struct fiq_cell_t
{
    unsigned int	timer;
    unsigned int	clear;
    unsigned int	set;
};

#ifndef __cplusplus
typedef
#endif
struct fiq_buffer_t
{
    unsigned int	rd_idx;
    unsigned int	wr_idx;
    unsigned int	size;
    unsigned int	status;
        fiq_cell_t      data[];
};

#ifndef __cplusplus
typedef
#endif
union fiq_line_t
{
    unsigned int value[3];
    fiq_cell_t    cell;
};

