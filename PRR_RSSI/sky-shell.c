#include "contiki.h"
#include "shell.h"
#include "serial-shell.h"
#include "collect-view.h"
#include "net/rime/rime.h"

/*---------------------------------------------------------------------------*/
PROCESS(sky_shell_process, "Sky Contiki shell");
AUTOSTART_PROCESSES(&sky_shell_process);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(sky_shell_process, ev, data)
{
    PROCESS_BEGIN();

    serial_shell_init();
    shell_nn_table();

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/