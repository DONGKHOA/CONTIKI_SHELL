#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
#include "serial-shell.h"

#define NN_STRATEGY 2

typedef struct {
    int8_t rssi;
    float prr;
    uint8_t rx;
    uint8_t id;
} nn_struct;

static nn_struct nnTable[2] = {{-128, 0, 0, 1}, {-128, 0, 0, 2}};
static const struct broadcast_callbacks broadcast_call; // Forward declaration
static struct broadcast_conn broadcast_conn;

void swap(nn_struct *a, nn_struct *b) {
    nn_struct temp = *a;
    *a = *b;
    *b = temp;
}

int partition(nn_struct arr[], int low, int high) {
    nn_struct pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
#if NN_STRATEGY == 1
        if (arr[j].rssi >= pivot.rssi) {
            i++;
            swap(&arr[i], &arr[j]);
        }
#elif NN_STRATEGY == 2
        if (arr[j].prr >= pivot.prr) {
            i++;
            swap(&arr[i], &arr[j]);
        }
#endif
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort(nn_struct arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}

static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {
    char *msg = (char *)packetbuf_dataptr();
    printf("Received broadcast from %d.%d: %s\n", from->u8[0], from->u8[1], msg);

    for (int i = 0; i < 2; i++) {
        if (nnTable[i].id == from->u8[0]) {
            nnTable[i].rx++;
            nnTable[i].rssi = atoi(msg);
            nnTable[i].prr = (float)nnTable[i].rx / (nnTable[i].rx + 1) * 100;
            break;
        }
    }

    quicksort(nnTable, 0, 1);
}

static const struct broadcast_callbacks broadcast_call = {broadcast_recv};

PROCESS(shell_rssi_prr_process, "RSSI and PRR Shell");
SHELL_COMMAND(rssi_prr_command,
              "nntable",
              "nntable: Print RSSI and PRR table",
              &shell_rssi_prr_process);

PROCESS_THREAD(shell_rssi_prr_process, ev, data) {
    PROCESS_BEGIN();

    char buf[100];
    shell_output_str(&rssi_prr_command, "ID   RSSI   PRR", "");

    for (int i = 0; i < 2; i++) {
        snprintf(buf, sizeof(buf), "%d    %d     %d",
                 nnTable[i].id, nnTable[i].rssi, (int)(nnTable[i].prr));
        shell_output_str(&rssi_prr_command, buf, "");
    }

    PROCESS_END();
}

PROCESS(broadcast_process, "Broadcast Process");
PROCESS_THREAD(broadcast_process, ev, data) {
    static struct etimer et;
    static char message[20];

    PROCESS_EXITHANDLER(broadcast_close(&broadcast_conn);)
    PROCESS_BEGIN();

    broadcast_open(&broadcast_conn, 129, &broadcast_call);

    while (1) {
        etimer_set(&et, CLOCK_SECOND * 5);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        snprintf(message, sizeof(message), "%d", -50 + random_rand() % 20);
        packetbuf_copyfrom(message, sizeof(message));
        broadcast_send(&broadcast_conn);
        printf("Broadcast sent: %s\n", message);
    }

    PROCESS_END();
}

void prr_rssi_shell_init(void) {
    serial_shell_init();
    shell_register_command(&rssi_prr_command);
}

