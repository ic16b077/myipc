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
#define KEY getuid() * 1000

/*
 * ------------------------------------------------------------- functions --
 */
static void usage(void);
static void error_message(int error_number);

/*
 * --------------------------------------------------------------- globals --
 */
static char* program_name;
static int shmid;

/**
 *
 * \brief Function to get the size of the ringbuffer
 *
 * \param argc argument count
 * \param argv argument vector
 *
 * \return size of the ringbuffer
 * \retval between 1 and RINGBUFFER_SIZE_MAX
 *
 */
long long get_ringbuffer_size(int argc, char* argv[]) {
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
		usage();
		exit(EXIT_FAILURE);
	}

	/* zu viele Argumente */
	if (argc > 3) {
		usage();
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
			                usage();
					exit(EXIT_FAILURE);
				}
				if (errno != 0 && ringbuffer_size == 0)
				{
                                        error_message(errno);
					exit(EXIT_FAILURE);
				}
				if (*endptr != '\0') {
					fprintf(stderr, "%s: Cannot parse number \"%s\"after -m\n", program_name, optarg);
					usage();
					exit(EXIT_FAILURE);
				}

				/* Limits für ringbuffer_size */
				if (ringbuffer_size < 1 || ringbuffer_size > RINGBUFFER_SIZE_MAX) {
                                        fprintf(stderr, "%s: ringbuffersize must be between 1 and %d (value %lld is not possible)\n", program_name, RINGBUFFER_SIZE_MAX, ringbuffer_size);
					usage();
                                        exit(EXIT_FAILURE);
				}

				break;
			default:
				/* wenn error, dann wird er von getopt() ausgegeben -> hier keine Ausgabe erforderlich */
				exit(EXIT_FAILURE);
		}
	}

        return ringbuffer_size;
}

/**
 *
 * \brief prints usage to stderr
 *
 */
static void usage(void)
{
    fprintf(stderr, "usage: %s -m <ring buffer size>\n", program_name);
}

/**
 *
 * \brief prints errno-meassage to stderr
 *
 */
static void error_message(int error_number)
{
    fprintf(stderr, "%s: %s\n", program_name, strerror(error_number));
}

/**
 *
 * \brief Function to create a semaphore and return its id
 *
 * \return semid
 * \retval id of the created semaphore
 *
 */
int get_semid(int initval)
{
	static int count = 0;
	int semid = 0;

	if ((semid = seminit(KEY + count, 0660, initval)) == -1)
	{
		if (errno == EEXIST)
		{
			semid = semgrab(KEY + count);
			if(semid == -1)
			{
				error_message(errno);
				/* SEMAPHOREN LOESCHEN */
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			error_message(errno);
		}
	}

	count++;

	return semid;
}

/**
 *
 * \brief Function to create a shared memory and return its id
 *
 * \return shmid
 * \retval id of the created shared memory
 *
 */
int* get_shm(size_t size, int flags)
{
	int* shm;

	shmid = shmget(KEY, size * sizeof(int), 0660 | IPC_CREAT);

	shm = (int *) shmat(shmid, NULL, flags);

	return shm;
}






void shm_del(void) {
	/* Shared memory löschen - BEACHTEN: Es sollte nur ein Prozeß das Shared Memory Segment entfernen */
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
	{
		error_message(errno);
	}
}
/*
 * =================================================================== eof ==
 */
