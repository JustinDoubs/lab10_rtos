#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h> // For PRIxx and SCNxx macros

#include "stm32f4xx_hal.h"
#include "cmd_line_buffer.h"
#include "cmd_parser.h"

#include "potentiometer.h"
#include "led_rgb.h"
#include "light.h"
#include "dimmer_task.h"
#include "heartbeat_task.h"

// Type for each command table entry
typedef struct
{
    void (*func)(int argc, char *argv[]);   // Command function pointer
    const char * cmd;                       // Command name
    const char * args;                      // Command arguments syntax
    const char * help;                      // Command description
} CMD_T;

// Command function declarations
static void _cmd_help(int, char *[]);
static void _cmd_pot(int, char *[]);
static void _cmd_heartbeat(int, char *[]);
static void _cmd_dimmer(int, char *[]);
static void _cmd_light(int, char *[]);

// Command table
static CMD_T cmd_table[] =
{
    {_cmd_help,         "help",         "",                 "Displays this help message"},
    {_cmd_pot,          "pot",          "",                 "Get potentiometer ADC value"},
    {_cmd_heartbeat,    "heartbeat",    "[start|stop]",     "Get status or start/stop heartbeat task"},
    {_cmd_dimmer,       "dimmer",       "[start|stop]",     "Get status or start/stop dimmer task"},
    {_cmd_light,        "light",        "[<H> <S> <V>]",    "Get or set light HSV values"},
};
enum {CMD_TABLE_SIZE = sizeof(cmd_table)/sizeof(CMD_T)};
enum {CMD_MAX_TOKENS = 5};      // Maximum number of tokens to process (command + arguments)


// Command function definitions

void _cmd_dimmer(int argc, char *argv[])
{
    if (argc <= 1)
    {
        if (dimmer_task_is_running())
            printf("Dimmer is currently running. Better catch it.\n");
        else
            printf("Dimmer is not currently running.\n");
    }
    else
    {
        if (strcmp(argv[1], "start") == 0)
        {
            dimmer_task_resume();
            printf("Dimmer has started\n");
        }
        else if (strcmp(argv[1], "stop") == 0)
        {
            dimmer_task_stop();
            printf("Dimmer has stopped\n");
        }
        else
        {
            printf("%s: invalid argument \"%s\", syntax is: %s [start|stop]\n", argv[0], argv[1], argv[0]);
        }
    }
}

void _cmd_heartbeat(int argc, char *argv[])
{
    if (argc <= 1)
    {
        if (heartbeat_task_is_running())
            printf("Heartbeat is currently running\n");
        else
            printf("Heartbeat is not currently running\n");
    }
    else
    {
        if (strcmp(argv[1], "start") == 0)
        {
            heartbeat_task_start();
            printf("Heartbeat has started\n");
        }
        else if (strcmp(argv[1], "stop") == 0)
        {
            heartbeat_task_stop();
            printf("Heartbeat has stopped\n");
        }
        else
        {
            printf("%s: invalid argument \"%s\", syntax is: %s [start|stop]\n", argv[0], argv[1], argv[0]);
        }
    }
}

void _cmd_pot(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("Potentiometer ADC value is %" PRIu16 "\n", pot_get_value());
    }
    else
    {
        printf("%s: invalid argument \"%s\", syntax is: %s\n", argv[0], argv[1], argv[0]);
    }
}

void _cmd_light(int argc, char *argv[])
{
    if (argc <= 1)
    {
        printf("Current LED hue = %" PRIu16 ", sat = %" PRIu16 ", val = %" PRIu16 "\n",
            (uint16_t)(light_get_hue()/LIGHT_HUE_DEGREE), light_get_saturation(), light_get_brightness());
    }
    else if (argc == 4)
    {
        dimmer_task_stop();
        
        int hue = atoi(argv[1]);
        int sat = atoi(argv[2]);
        int val = atoi(argv[3]);

        // Input sanitisation
        hue = (hue % 360 + 360) % 360;
        sat = sat <= LIGHT_MIN_SATURATION ? LIGHT_MIN_SATURATION : sat >= LIGHT_MAX_SATURATION ? LIGHT_MAX_SATURATION : sat;
        val = val <= LIGHT_MIN_BRIGHTNESS ? LIGHT_MIN_BRIGHTNESS : val >= LIGHT_MAX_BRIGHTNESS ? LIGHT_MAX_BRIGHTNESS : val;

        // Set HSV values
        light_set_hue(hue*LIGHT_HUE_DEGREE);
        light_set_saturation(sat);
        light_set_brightness(val);

        // Update LED RGB channels
        led_rgb_set_red(light_get_red());
        led_rgb_set_green(light_get_green());
        led_rgb_set_blue(light_get_blue());

        printf("Set LED hue = %" PRIu16 ", sat = %" PRIu16 ", val = %" PRIu16 "\n",
            (uint16_t)(light_get_hue()/LIGHT_HUE_DEGREE), light_get_saturation(), light_get_brightness());
    }
    else
    {
        printf("%s: invalid argument \"%s\", syntax is: %s [<hue> <saturation> <brightness>]\n", argv[0], argv[1], argv[0]);
    }
}

static void _print_chip_pinout(void);

void _cmd_help(int argc, char *argv[])
{
    UNUSED(argv); // Avoid compiler warning about unused variable
    printf(
        "\n"
        "\n"
    );

    _print_chip_pinout();
    
    printf("\n");

    // Describe argument syntax using POSIX.1-2008 convention
    // see http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html
    // printf(
    //     "Available commands:\n"
    //     "    help                       Print this help message\n"
    //     "    pot                        Get potentiometer ADC value\n"
    //     "    enc [reset]                Get or reset encoder count\n"
    //     "    light [<hue> <sat> <val>]  Get or set light HSV values\n"
    //     "    dimmer [start|stop]        Get status or start/stop dimmer task\n"
    //     "    heartbeat [start|stop]     Get status or start/stop heartbeat task\n"
    //     "\n"
    // );
    switch (argc)
    {
    case 1:
        printf(
            "   Command Arguments            Description\n"
            "-------------------------------------------\n"
        );
        for (int i = 0; i < CMD_TABLE_SIZE; i++)
        {
            printf("%10s %-20s %s\n", cmd_table[i].cmd, cmd_table[i].args, cmd_table[i].help);
        }
        // printf("\nFor more information, enter help followed by the command name\n\n");
        break;
    // case 2:
    //     printf("To be implemented...\n\n");
        // TODO: Scan command table, and lookup extended help string.
        break;
    // default:
        // printf("help is expecting zero or one argument.\n\n");
    }
}

void _print_chip_pinout(void)
{
    printf(
        "Pin configuration:\n"
        "\n"
        "       .---------------------------------------.\n"
        " PC10--|  1  2 --PC11              PC9--  1  2 |--PC8\n"
        " PC12--|  3  4 --PD2               PB8--  3  4 |--PC6\n"
        "  VDD--|  5  6 --E5V               PB9--  5  6 |--PC5\n"
        "BOOT0--|  7  8 --GND              AVDD--  7  8 |--U5V\n"
        "   NC--|  9 10 --NC                GND--  9 10 |--NC\n"
        "   NC--| 11 12 --IOREF             PA5-- 11 12 |--PA12\n"
        " PA13--| 13 14 --RESET             PA6-- 13 14 |--PA11\n"
        " PA14--| 15 16 --+3v3              PA7-- 15 16 |--PB12\n"
        " PA15--| 17 18 --+5v               PB6-- 17 18 |--NC\n"
        "  GND--| 19 20 --GND               PC7-- 19 20 |--GND\n"
        "  PB7--| 21 22 --GND               PA9-- 21 22 |--PB2\n"
        " PC13--| 23 24 --VIN               PA8-- 23 24 |--PB1\n"
        " PC14--| 25 26 --NC               PB10-- 25 26 |--PB15\n"
        " PC15--| 27 28 --PA0               PB4-- 27 28 |--PB14\n"
        "  PH0--| 29 30 --PA1               PB5-- 29 30 |--PB13\n"
        "  PH1--| 31 32 --PA4               PB3-- 31 32 |--AGND\n"
        " VBAT--| 33 34 --PB0              PA10-- 33 34 |--PC4\n"
        "  PC2--| 35 36 --PC1               PA2-- 35 36 |--NC\n"
        "  PC3--| 37 38 --PC0               PA3-- 37 38 |--NC\n"
        "       |________                   ____________|\n"
        "                \\_________________/\n"
    );
}

// Command parser

static int _makeargv(char *s, char *argv[], int argvsize);

#ifdef NO_LD_WRAP
void cmd_parse(char *) __asm__("___real_cmd_parse");
#endif

void cmd_parse(char * cmd)
{
    if (cmd == NULL)
    {
        printf("ERROR: Tried to parse NULL command pointer\n");
        return;
    }
    else if (*cmd == '\0') // Empty command string
    {
        return;
    }

    // Tokenise command string
    char *argv[CMD_MAX_TOKENS];
    int argc = _makeargv(cmd, argv, CMD_MAX_TOKENS);

    // Execute corresponding command function
    for (int i = 0; i < CMD_TABLE_SIZE; i++)
    {
        if (strcmp(argv[0], cmd_table[i].cmd) == 0)
        {
            cmd_table[i].func(argc, argv);
            return;
        }
    }

    // Command not found
    printf("Unknown command: \"%s\"\n", argv[0]);
    // if (argc > 1)
    // {
    //     printf("    with arguments:\n");
    //     for (int i = 1; i < argc; i++)
    //     {
    //         printf("        %d: %s\n", i, argv[i]);
    //     }
    // }
    // printf_P(PSTR("\nEnter \"help\" for a list of commands.\n\n"));
}

int _makeargv(char *s, char *argv[], int argvsize)
{
    char *p = s;
    int argc = 0;

    for(int i = 0; i < argvsize; ++i)
    {
        // skip leading whitespace
        while (isspace(*p))
            p++;

        if(*p != '\0')
            argv[argc++] = p;
        else
        {
            argv[argc] = NULL;
            break;
        }

        // scan over arg
        while(*p != '\0' && !isspace(*p))
            p++;

        // terminate arg
        if(*p != '\0' && i < argvsize - 1)
            *p++ = '\0';
    }

    return argc;
}


