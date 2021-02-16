## Non-blocking switch debounce

A non-blocking low-pass filter function used for contact switch debouncing.

The filter consusts is a bit buffer word and corresponding bit mask.

Upon each sample period, determined by a platform dependent counter, the physical input is sampled and shfited into the sample buffer.

If the sample buffer contains a mixture of 1s and 0s, then the output state is considered indeterminate, and is ignored.

When the sample buffer transitions to either all 1s or all 0s, then the output state transitions accordingly.

If the application callback is non-null, the callback is executed upon each transition of filtered input state.

### Usage Example

~~~~
#include "debounce.h"

#define FILTER_DEPTH 10

debounce_t debounce;

static bool get_switch(void);
static void switch_callback(debounce_t* debounce);

int main(int argc,char*argv[])
{

    debounce_setup( &debounce,
                    FILTER_DEPTH,
                    get_sw,
                    switch_callback
                );

    for(;;)
    {
        debounce_service();
    }
}

static bool get_switch(void)
{
    /* platform dependent read pin */
    return read_platform_pin();
}

static void switch_callback(debounce_t* debounce)
{
    if ( debounce_state(debounce) )
        printf("pressed");
    else
        printf("released");
}

~~~~