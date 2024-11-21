
#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include <stdio.h>

#include "shell.h"
#include "serial-shell.h"
#include "collect-view.h"
#include "net/rime/rime.h"

/************************************************
 *                  Constants                   *
 ************************************************/
#define NN_STRATEGY 2 // #define 	1.RSSI	2.PRR

/************************************************
 *                  Structs                     *
 ************************************************/
typedef struct
{
    int8_t rssi;
    float prr;
    uint8_t rx;
    uint8_t id;
} nn_struct;

/************************************************
 *              Global variables                *
 ************************************************/

static nn_struct nnTable[3] = {{-128, 0, 0, 1}, {-128, 0, 0, 2}, {-128, 0, 0, 3}};
uint8_t size;
uint8_t count;
uint8_t i, j;
static uint8_t TARGET_NODE;
static uint8_t send_num = 1;
static struct etimer et;
char snum[4];
uint8_t idx;
static struct broadcast_conn broadcast;

/************************************************
 *                  Functions                   *
 ************************************************/

// Swap two elements of the array
void swap(nn_struct *a, nn_struct *b)
{
    nn_struct temp = *a;
    *a = *b;
    *b = temp;
}
// Choose a pivot element and partition the array around it
int partition(nn_struct arr[], int low, int high)
{
    nn_struct pivot = arr[high];
    int i = low - 1;
    for (j = low; j < high; j++)
    {
#if NN_STRATEGY == 1 // 1.RSSI
        if (arr[j].rssi >= pivot.rssi)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
#elif NN_STRATEGY == 2 // 2.PRR
        if (arr[j].prr >= pivot.prr)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
#endif
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}
// Recursively sort the subarrays
void quicksort(nn_struct arr[], int low, int high)
{
    if (low < high)
    {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
    /*Find index in nnTable*/
    for (i = 0; i < count; i++)
    {
        if (nnTable[i].id == from->u8[0])
        {
            idx = i;
            break;
        }
    }
    /*Calculate PRR and RSSI*/
    nnTable[idx].rx++;
    nnTable[idx].prr = (float)nnTable[idx].rx / (float)atoi((char *)packetbuf_dataptr()) * 100;
    nnTable[idx].rssi = (int8_t)packetbuf_attr(PACKETBUF_ATTR_RSSI);
    /*Sort nnTable by NN_STRATEGY*/
    quicksort(nnTable, 0, count - 1);
    printf("rcv ccdmcs\n");
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

/************************************************
 *                  Processes                   *
 ************************************************/
PROCESS(broadcast_process, "Broadcast proccess");
PROCESS_THREAD(broadcast_process, ev, data)
{
    PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
    PROCESS_BEGIN();
    broadcast_open(&broadcast, 129, &broadcast_call);
    /*Initialize some variables*/
    TARGET_NODE = linkaddr_node_addr.u8[0];
    size = sizeof(nnTable[0]);
    count = sizeof(nnTable) / size;
    while (1)
    {
        /* Delay 2-4 seconds */
        etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 2));
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        /* Turn integer to string */
        itoa(send_num++, snum, 10);
        /* BROADCAST */
        packetbuf_copyfrom(snum, 4);
        broadcast_send(&broadcast);
        printf("ccdmcs\n");
    }
    PROCESS_END();
}

PROCESS(shell_rssi_prr_process, "Using shell to print out RSSI and PRR");
SHELL_COMMAND(rssi_prr_command,
              "nntable",
              "nntable: Print out RSSI and PRR table sorted by strategy",
              &shell_rssi_prr_process);
PROCESS_THREAD(shell_rssi_prr_process, ev, data)
{
    PROCESS_BEGIN();

    char buf[100];

    shell_output_str(&rssi_prr_command, "----------------------------------------------------", "");
    shell_output_str(&rssi_prr_command, "ID     RSSI         PRR", "");
    shell_output_str(&rssi_prr_command, "----------------------------------------------------", "");
    for (i = 0; i < count; i++)
    {
        if (nnTable[i].id == TARGET_NODE)
        {
            continue;
        }
        else
        {

            snprintf(buf, sizeof(buf), "%d       %d          %d", (uint8_t)(nnTable[i].id), (int8_t)(nnTable[i].rssi), (uint8_t)(nnTable[i].prr));
            shell_output_str(&rssi_prr_command, buf, "");
        }
    }
    PROCESS_END();
}

/*---------------------------------------------------------------------------*/
// AUTOSTART_PROCESSES(&broadcast_process, &shell_rssi_prr_process);
/*---------------------------------------------------------------------------*/

void shell_nn_table(void)
{
    process_start(&broadcast_process, NULL);
    shell_register_command(&rssi_prr_command);
}