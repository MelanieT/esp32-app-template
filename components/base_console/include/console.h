struct console_handlers
{
    void (*send)(const char *data);
    void (*disconnect)(void);
    void (*set_process_line_cb)(void (*)(const char *));
    void (*set_interrupt_cb)(void (*)(void));
    void (*set_new_connection_cb)(void (*)(void));
    void (*process_command_cb)(int argc, char *argv[], void *user_data);
    void (*new_connection_cb)(void *user_data);
    void *user_data;
};
void console_init(void);
void console_set_handlers(struct console_handlers *handlers);
void console_send(const char *data);
void console_disconnect(void);

#define console_printf(fmt, ...) do { \
    char data[1024]; \
    os_sprintf(data, fmt, ##__VA_ARGS__); \
    console_send(data); \
    } while(0)
