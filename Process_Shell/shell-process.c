#include "contiki.h"
#include "cc2420.h"

#include "sys/node-id.h"
#include "shell-ps.h"
#include "shell-process.h"

#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
PROCESS(shell_node_info, "nodeinfo");
PROCESS(shell_ps_process, "ps");
SHELL_COMMAND(node_info,
              "nodeinfo",
              "nodeinfo: Display current node ID and channel",
              &shell_node_info);
SHELL_COMMAND(ps_command,
              "ps",
              "ps: list all running processes",
              &shell_ps_process);
/*---------------------------------------------------------------------------*/
/* Process to display current node ID and channel */
PROCESS_THREAD(shell_node_info, ev, data)
{
  char buf[20];
  PROCESS_BEGIN();

  snprintf(buf, sizeof(buf), "%d", node_id); // Retrieve current Node ID
  shell_output_str(&node_info, "Node ID: ", buf);

  snprintf(buf, sizeof(buf), "%d", cc2420_get_channel()); // Retrieve current Channel
  shell_output_str(&node_info, "Channel: ", buf);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_ps_process, ev, data)
{
  struct process *p;
  PROCESS_BEGIN();

  shell_output_str(&ps_command, "Processes:", "");
  for (p = PROCESS_LIST(); p != NULL; p = p->next)
  {
    char namebuf[30];
    strncpy(namebuf, PROCESS_NAME_STRING(p), sizeof(namebuf));
    shell_output_str(&ps_command, namebuf, "");
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void shell_process_init(void)
{
  shell_register_command(&node_info);
  shell_register_command(&ps_command);
}
/*---------------------------------------------------------------------------*/
