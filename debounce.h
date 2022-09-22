/******************************************************************************
 Copyright (c) 2021 Mike SHarkey <mike@8bitgeek.net>

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
#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

#include <stdbool.h>
#include <stdint.h>




/******************************************************************************
 * @brief timer.h is the source header for a platform dependent tick counter  *
 *        nominally, provides a millisecond counter.                          *
 *        debounce_tick_t abstracts away the platform dependent counter type. *
 *****************************************************************************/
#include <timer.h>  /**< Replace with your timer tick source */
/*****************************************************************************
 * @brief replace timer_t with your platform tick counter type               *
 *****************************************************************************/
typedef timer_tick_t  debounce_tick_t; /**< Replace w/your timer tick type */
/*****************************************************************************
 * @brief The debounce_callback_timer_t type defines a function callback     *
 *        which is intended to read the timer ticks (typically millisecond). *
 * @return A bool data type representing the state of the physical input.    *
 *****************************************************************************/
typedef debounce_tick_t (*debounce_callback_timer_t)(void);




/*****************************************************************************
 * Diasble C++ name mangling for "C" language linkage.                       *
 *****************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * @brief The debounce_bitmask_t type defines the maximum bit-depth of the   *
 *        sample buffer temporal units according to debounce_tick_t          *
 *****************************************************************************/
typedef uint32_t debounce_bitmask_t;
#define debounce_bitmask_size()    (sizeof(debounce_bitmask_t)*8)  

/*****************************************************************************
 * @brief The debounce_callback_read_t type defines a function callback      *
 *        which is intended to read the current state of the switch.         *
 * @return A bool data type representing the state of the physical input.    *
 *****************************************************************************/
typedef bool (*debounce_callback_read_t)(void);

/*****************************************************************************
 * @brief The debounce_t type maintains the state of a single input          *
 *        One instance required per input                                    *
 *****************************************************************************/
typedef struct _debounce_t
{
    debounce_callback_timer_t   callback_timer;
    debounce_callback_read_t    callback_read;
    void                        (*callback_event)(struct _debounce_t* debounce);
    debounce_tick_t                ticks;
    uint8_t                     filter_depth;
    bool                        state;
    debounce_bitmask_t          input_bits;
    debounce_bitmask_t          filter_mask;
} debounce_t;

/*****************************************************************************
 * @brief The debounce_callback_event_t type defines a function callback     *
 *        which is intended notify the application upon a change in state    *
 * @param debounce pointer. Call @ref debounce_state() within callback.      *
 *****************************************************************************/
typedef void (*debounce_callback_event_t)(debounce_t* debounce);

/*****************************************************************************
 * @brief To be called once per input to initialize state.                   *
 * @param debounce pointer to an uninitialized instance of debounce_t type.  *
 * @param filter_depth the depth of the sample buffer in debounce_tick_t     *
 *        units up to a maximum of (sizeof(debounce_bitmask_t)*8) bits.      *
 * @param callback_read callback function to read the GPIO input pin         *
 * @param callback_event callbback notify application of input state change  *
 *****************************************************************************/
extern void debounce_setup  (
                                debounce_t* debounce, 
                                uint8_t filter_depth,
                                debounce_callback_timer_t   callback_timer,
                                debounce_callback_read_t    callback_read,
                                debounce_callback_event_t   callback_event
                            );

/*****************************************************************************
 * @brief To be called regularly in a non-blocking loop.                     *
 * @param debounce pointer to a debounce_t type that has been initialized.   *
 *****************************************************************************/
extern void debounce_service(debounce_t* debounce);

/*****************************************************************************
 * @brief Will always maintain the last filtered input state.                *
 * @param debounce pointer to a debounce_t type that has been initialized.   *
 * @return The filtered input state                                          *
 *****************************************************************************/
extern bool debounce_state  (debounce_t* debounce);

/*****************************************************************************
 * End- Diasble C++ name mangling for "C" language linkage.                  *
 *****************************************************************************/
#ifdef __cplusplus
}
#endif

#endif 
