/** ***************************************************************************
 @file debounce.c
 @author Mike Sharkey (mike@8bitgeek.net)
 @brief Switch debouncer function for GPIO inputs
 @version 0.1
 @date 2022-09-22
 
 @copyright Copyright (c) 2021 Mike Sharkey <mike@8bitgeek.net>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#include <debounce.h>

static int8_t   debounce_shift_input_bit(debounce_t* debounce, bool bit);
static int8_t   debounce_get_input_state(debounce_t* debounce);

static void     debounce_set_bit    (debounce_bitmask_t* bitmask, bool bit);
static void     debounce_reset_bit  (debounce_bitmask_t* bitmask, bool bit);
static void     debounce_set_bits   (debounce_bitmask_t* bitmask, uint8_t bits);
static void     debounce_reset_bits (debounce_bitmask_t* bitmask, uint8_t bits);
static void     debounce_inval_bits (debounce_bitmask_t* bitmask, uint8_t bits);

/******************************************************************************
 * SETUP                                                                      *
 ******************************************************************************/

extern void debounce_setup(
                            debounce_t* debounce, 
                            uint8_t filter_depth,
                            debounce_callback_timer_t   callback_timer,
                            debounce_callback_read_t    callback_read,
                            debounce_callback_event_t   callback_event
                            )
{
    memset(debounce,0,sizeof(debounce_t));

    debounce->callback_timer = callback_timer;
    debounce->callback_read = callback_read;
    debounce->callback_event = callback_event;

    debounce->filter_depth = (filter_depth > debounce_bitmask_size()) ? debounce_bitmask_size() : filter_depth;
    
    debounce_set_bits( &debounce->filter_mask, debounce->filter_depth );
    debounce_inval_bits( &debounce->input_bits, debounce->filter_depth );

    debounce->ticks = debounce->callback_timer();
}

extern bool debounce_state(debounce_t* debounce)
{
    return debounce->state;
}

/******************************************************************************
 * SERVICE                                                                    *
 ******************************************************************************/

extern void debounce_service(debounce_t* debounce)
{
    if ( debounce->ticks != debounce->callback_timer() )
    {
        int8_t sample_state = debounce_shift_input_bit(debounce,debounce->callback_read());

        debounce->ticks = debounce->callback_timer();
        
        if ( sample_state >= 0 )
        {
            if ( (bool)sample_state != debounce->state )
            {
                debounce->state = (bool)sample_state;
                if ( debounce->callback_event )
                {
                    debounce->callback_event(debounce);
                }
            }
        }
    }
}

/******************************************************************************
 * INPUT STATE                                                                *
 ******************************************************************************/

static int8_t debounce_shift_input_bit(debounce_t* debounce, bool bit)
{
    debounce->input_bits <<= 1;
    debounce->input_bits |= bit;
    debounce->input_bits &= debounce->filter_mask;
    return debounce_get_input_state(debounce);
}

static int8_t debounce_get_input_state(debounce_t* debounce)
{
    if ( debounce->input_bits == (debounce_bitmask_t)0 )
    {
        return 0;
    }
    else
    if ( (debounce->input_bits & debounce->filter_mask) == debounce->filter_mask )
    {
        return 1;
    }

    return -1; /* indeterminate input state */
}



/******************************************************************************
 * BITMASK                                                                    *
 ******************************************************************************/

static void debounce_set_bit(debounce_bitmask_t* bitmask, bool bit)
{
    *bitmask |= (debounce_bitmask_t)(1<<bit);
}

static void debounce_reset_bit(debounce_bitmask_t* bitmask, bool bit)
{
    *bitmask &= ~(debounce_bitmask_t)(1<<bit);
}

static void debounce_set_bits(debounce_bitmask_t* bitmask, uint8_t bits)
{
    for(uint8_t n=0; n < bits; n++)
    {
        debounce_set_bit(bitmask,n);
    }
}

static void debounce_reset_bits(debounce_bitmask_t* bitmask, uint8_t bits)
{
    for(uint8_t n=0; n < bits; n++)
    {
        debounce_reset_bit(bitmask,n);
    }
}

static void debounce_inval_bits (debounce_bitmask_t* bitmask, uint8_t bits)
{
    for(uint8_t n=0; n < bits; n++)
    {
        (n&1) ? debounce_set_bit(bitmask,n) : debounce_reset_bit(bitmask,n);
    }
}

