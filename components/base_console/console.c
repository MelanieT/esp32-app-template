#include "inttypes.h"
#include "string.h"
#include "console.h"

#include <esp_ota_ops.h>

#define ICACHE_FLASH_ATTR

#define os_sprintf sprintf

static struct console_handlers *handlers;

ICACHE_FLASH_ATTR
void console_init(void)
{
}

ICACHE_FLASH_ATTR
void console_send(const char *data)
{
    handlers->send(data);
}

ICACHE_FLASH_ATTR
char * os_strdup(const char *s)
{
    char *ret = (char *)malloc(strlen(s) + 1);
    if (!ret)
        return 0;

    strcpy(ret, s);

    return ret;
}

ICACHE_FLASH_ATTR
static void process_line(const char *line)
{
    char *l = os_strdup(line);
    char *sections[32];
    int sections_count = 0;
    char *args[64];
    int args_count = 0;

    if (!*line)
    {
        console_send("# ");
        return;
    }

    char *str = strtok(l, "\"");
    do
    {
        sections[sections_count++] = os_strdup(str);
        str = strtok(0, "\"");
    } while (str);
    free(l);

    for (int i = 0 ; i < sections_count ; i++)
    {
        if (i % 2)
        {
            args[args_count++] = sections[i];
            continue;
        }

        str = strtok(sections[i], " ");
        do
        {
            if (str[0] != 0)
                args[args_count++] = os_strdup(str);
            str = strtok(0, " ");
        } while (str);
        free(sections[i]);
    }

    handlers->process_command_cb(args_count, args, handlers->user_data);
    for (int i = 0 ; i < args_count ; i++)
        free(args[i]);

    console_send("# ");
}

ICACHE_FLASH_ATTR
static void interrupt(void)
{
    console_send("# ");
}

ICACHE_FLASH_ATTR
static void new_connection(void)
{
    if (handlers->new_connection_cb)
        handlers->new_connection_cb(handlers->user_data);

    const esp_partition_t *part = esp_ota_get_boot_partition();
    console_printf("Booted from %08lx\r\n", part->address);

    console_send("# ");
}

ICACHE_FLASH_ATTR
void console_set_handlers(struct console_handlers *h)
{
    handlers = h;
    h->set_process_line_cb(process_line);
    h->set_interrupt_cb(interrupt);
    if (h->set_new_connection_cb)
        h->set_new_connection_cb(new_connection);
}

ICACHE_FLASH_ATTR
void console_disconnect()
{
    if (handlers->disconnect)
        handlers->disconnect();
}
