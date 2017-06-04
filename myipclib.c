/**
 * @file myipc.c
 * Betriebssysteme myipc File.
 * Beispiel 3
 *
 * @author Aleksandar Pavlovic, Johanna Hoffmann, Michael Käfer
 * @date 2017/06/03
 *
 * @version 1
 *
 * @todo Review
 *
 */

/*
 * -------------------------------------------------------------- includes --
 */
#include "myipclib.h"

/*
 * --------------------------------------------------------------- defines --
 */
#define RINGBUFFER_SIZE_MAX 200

/*
 * ------------------------------------------------------------- functions --
 */
static void usage(char* program_name);

/*
 * --------------------------------------------------------------- globals --
 */

/**
 *
 * \brief Helper function to get the size of the ringbuffer
 *
 * \param argc argument count
 * \param argv argument vector
 *
 * \return size of the ringbuffer
 * \retval between 1 and RINGBUFFER_SIZE_MAX
 *
 */
int get_ringbuffer_size(int argc, char* argv[]) {
	char* program_name;
        int option;
	long long ringbuffer_size = 0;
	char* endptr;
	int base = 10;

	/* Programmnamen holen */
        if (argv[0])
        {
                program_name = argv[0];
        }
	else
	{
		exit(EXIT_FAILURE);
	}

	/* zu wenig Argumente */
	if (argc < 2)
	{
		fprintf(stderr, "%s: ringbuffersize must be specified\n", program_name);
		usage(program_name);
		exit(EXIT_FAILURE);
	}

	/* zu viele Argumente */
	if (argc > 3) {
		usage(program_name);
		exit(EXIT_FAILURE);
	}

	/* Optionen verarbeiten */
	while ((option = getopt(argc, argv, "m:")) != -1)
	{
		switch (option)
		{
			case 'm':
				ringbuffer_size = strtoll(optarg, &endptr, base);

				/* Fehlerbehandlung für strtol */
				if (errno == ERANGE && (ringbuffer_size == LLONG_MAX || ringbuffer_size == LLONG_MIN))
				{
					fprintf(stderr, "%s: numeric overflow when converting ringbuffersize to long long value (value %s exceeds %lld)\n", program_name, optarg, LLONG_MAX);
			                usage(program_name);
					exit(EXIT_FAILURE);
				}
				if (errno != 0 && ringbuffer_size == 0)
				{
                                        fprintf(stderr, "%s: %s\n", program_name, strerror(errno));
					exit(EXIT_FAILURE);
				}
				if (*endptr != '\0') {
					fprintf(stderr, "%s: Cannot parse number \"%s\"after -m\n", program_name, optarg);
					usage(program_name);
					exit(EXIT_FAILURE);
				}

				/* Limits für ringbuffer_size */
				if (ringbuffer_size < 1 || ringbuffer_size > RINGBUFFER_SIZE_MAX) {
                                        fprintf(stderr, "%s: ringbuffersize must be between 1 and %d (value %lld is not possible)\n", program_name, RINGBUFFER_SIZE_MAX, ringbuffer_size);
					usage(program_name);
                                        exit(EXIT_FAILURE);
				}

				break;
			default:
				exit(EXIT_FAILURE);
		}
	}

        return ringbuffer_size;
}

/**
 *
 * \brief prints usage to stderr
 *
 * \param program_name name of the program
 *
 */
static void usage(char* program_name)
{
    fprintf(stderr, "usage: %s -m <ring buffer size>\n", program_name);
}

/*
 * =================================================================== eof ==
 */
