#include "contiki.h"
#include "shell.h"
#include "serial-shell.h"

/*---------------------------------------------------------------------------*/

PROCESS(sky_shell_process, "Sky Contiki shell");
AUTOSTART_PROCESSES(&sky_shell_process);

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(sky_shell_process, ev, data)
{
  PROCESS_BEGIN();

  shell_sky_init();
  serial_shell_init();
  shell_power_init();
  shell_ps_init();
  shell_blink_init();
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
