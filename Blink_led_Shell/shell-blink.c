#include "contiki.h"
#include "shell.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
PROCESS(shell_blink_process, "blink");
PROCESS(shell_led_process, "led");
SHELL_COMMAND(blink_command,
              "blink",
              "blink [num]: blink LEDs ([num] times)",
              &shell_blink_process);
SHELL_COMMAND(led_command, "led", "led: each color lights up for 3 seconds", &shell_led_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_blink_process, ev, data)
{
  static struct etimer etimer;
  static int i, num, on = 0;
  const char *nextptr;

  PROCESS_EXITHANDLER(if(on) { leds_toggle(LEDS_ALL); });

  PROCESS_BEGIN();

  if(data != NULL) {
    num = shell_strtolong(data, &nextptr);
    if(nextptr != data) {
      etimer_set(&etimer, CLOCK_SECOND / 2);
      for(i = 0; i < num; ++i) {
        leds_toggle(LEDS_ALL);
        on = 1;
        PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
        etimer_reset(&etimer);
        
        leds_toggle(LEDS_ALL);
        on = 0;
        PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
        etimer_reset(&etimer);
      }
      PROCESS_EXIT();
    }
  }

  while(1) {
    struct shell_input *input;
    PROCESS_WAIT_EVENT();
    if(ev == PROCESS_EVENT_TIMER && data == &etimer) {
      if(on) {
        leds_toggle(LEDS_ALL);
        on = 0;
      }
    } else if(ev == shell_event_input) {
      input = data;
      if(input->len1 + input->len2 == 0) {
        if(on) {
          leds_toggle(LEDS_ALL);
          on = 0;
        }
        PROCESS_EXIT();
      }
      shell_output(&blink_command,
                   input->data1, input->len1,
                   input->data2, input->len2);

      if(on == 0) {
        leds_toggle(LEDS_ALL);
        on = 1;
      }
      etimer_set(&etimer, CLOCK_SECOND / 16);
    }
  }
  
  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_led_process, ev, data)
{
  static struct etimer etimer;

  PROCESS_BEGIN();

  etimer_set(&etimer, CLOCK_SECOND * 3);

  leds_on(LEDS_RED);
  PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
  leds_off(LEDS_RED);

  leds_on(LEDS_GREEN);
  etimer_reset(&etimer);
  PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
  leds_off(LEDS_GREEN);

  leds_on(LEDS_BLUE);
  etimer_reset(&etimer);
  PROCESS_WAIT_UNTIL(etimer_expired(&etimer));
  leds_off(LEDS_BLUE);

  PROCESS_END();
}


/*---------------------------------------------------------------------------*/
void shell_blink_init(void)
{
  shell_register_command(&blink_command);
  shell_register_command(&led_command);
}
/*---------------------------------------------------------------------------*/

