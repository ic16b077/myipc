
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
		printf("%s: ringbuffersize must be specified\n", program_name);
		printf("usage: %s -m <ring buffer size>\n", program_name);
		exit(EXIT_FAILURE);
	}

	while ((option = getopt(argc, argv, "m:")) != -1)
	{
		switch (option)
		{
			case 'm':
				ringbuffer_size = strtol(optarg, &endptr, base);
				if (1 == 2) // Fehlerbehandlung ergänzen
				{
					exit(EXIT_FAILURE);
				}
				break;
			/* default: */
				/* Fehlerbehandlung ergänzen */
		}
	}

        return ringbuffer_size;
}
