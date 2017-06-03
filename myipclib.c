
#include "myipclib.h"

int get_ringbuffer_size(int argc, char* argv[]) {
	char* program_name;
        int option;
	int ringbuffer_size = 0;
	char* endptr;
	int base = 10;

        if (argv[0])
        {
                program_name = argv[0];
        }
	else
	{
		exit(EXIT_FAILURE); // Fehlerbehandlung?
	}

	if (argc < 2)
	{
		fprintf(stderr, "%s: ringbuffersize must be specified\n", program_name);
		fprintf(stderr, "usage: %s -m <ring buffer size>\n", program_name);
		exit(EXIT_FAILURE);
	}

	while ((option = getopt(argc, argv, "m:")) != -1)
	{
		switch (option)
		{
			case 'm':
				ringbuffer_size = strtol(optarg, &endptr, base);

				/* Fehlerbehandlung für strtol */
				if (errno == ERANGE && (ringbuffer_size == LONG_MAX || ringbuffer_size == LONG_MIN))
				{
					fprintf(stderr, "\n"); // Fehlerbehandlung ergänzen
					exit(EXIT_FAILURE);
					/*

					FEHLERMELDUNG VON bic-empfaenger -m 9999999999999999999999999999999999999

					bic-empfaenger: numeric overflow when converting ringbuffersize to long long value (value 99999999999999999999999999999999999999999 exceeds 9223372036854775807)
					usage: bic-empfaenger [-h] -m <ring buffer size>

					*/
				}

				if (errno != 0 && ringbuffer_size == 0)
				{
					fprintf(stderr, "\n"); // Fehlerbehandlung ergänzen
					exit(EXIT_FAILURE);
				}

				if (*endptr != '\0') {
					fprintf(stderr, "\n"); // Fehlerbehandlung ergänzen
					exit(EXIT_FAILURE);
				}
				break;
			/* default: */
				/* Fehlerbehandlung ergänzen */
		}
	}

        return ringbuffer_size;
}
