/*	Library for time management on various microcontrollers
	Copyright (C) 2014 Jesus Ruben Santa Anna Zamudio.
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	Author website: http://www.geekfactory.mx
	Author e-mail: ruben at geekfactory dot mx
 */

#include <xc.h>
#include <stdint.h>
#include "tick.h"
#include "buttons.h"
#include "mcc_generated_files/mcc.h"

/* Internal counter for timer interrupts */
static volatile unsigned long tickcnt;
/* Total counter 6 bytes including timer register value */
static unsigned char tickbuffer[6];

static void tick_read_internal();

void tick_init()
{
    tickcnt = 0;
    TMR0_SetInterruptHandler(tick_update);
}

uint32_t tick_get()
{
	tick_read_internal();
	return *((uint32_t *) & tickbuffer[0]);
}

uint32_t tick_get_slow()
{
	tick_read_internal();
	return *((uint32_t *) & tickbuffer[2]);
}

void tick_update()
{
		tickcnt++;

}

static void tick_read_internal()
{
	do {
        PIE3bits.TMR0IE = 1; // Enable interrupt
		_delay(1);
		PIE3bits.TMR0IE = 0; // Disable interrupt

		tickbuffer[0] = TMR0L;
		tickbuffer[1] = TMR0H;

		*((uint32_t*) & tickbuffer[2]) = tickcnt;
	} while (TMR0IF);
	PIE3bits.TMR0IE = 1; // Enable interrupt
}
