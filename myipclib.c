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
#define KEY getuid() * 1000

/*
 * ------------------------------------------------------------- functions --
 */
static void usage(void);
static void error_message(int error_number);
static void remove_sem(void);

/*
 * --------------------------------------------------------------- globals --
 */
static char* program_name;
static int shmid;
static int semid_sender;
static int semid_empfaenger;

/**
 *
 * \brief Function to get the size of the ringbuffer
 *
 * \param argc argument count
 * \param argv argument vector
 *
 * \return size of the ringbuffer
 * \retval ringbuffer_size
 *
 */
size_t get_ringbuffer_size(int argc, char* argv[]) {
	int option;
	long ringbuffer_size = 0;
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
	//printf(">> SIZE_MAX/sizeof(int)      = %u <<\n", SIZE_MAX/sizeof(int)); // 4294967295
	//printf(">> SIZE_MAX/4      = %u <<\n", SIZE_MAX/4); //1073741823
	//printf(">> SIZE_MAX+1    = %u <<\n", SIZE_MAX+1); //starts again at zero
	//printf(">> SIZE_MAX-4  = %u <<\n", SIZE_MAX-4); //max. value -4 last bytes = 4294967291
	while ((option = getopt(argc, argv, "m:")) != -1)
	{
		switch (option)
		{
		case 'm':
			ringbuffer_size = strtol(optarg, &endptr, base);

			/* Fehlerbehandlung für strtol */
			if (errno == ERANGE && (ringbuffer_size == LONG_MAX || ringbuffer_size == LONG_MIN))
			{
				fprintf(stderr, "%s: numeric overflow when converting ringbuffersize to long value (value %s exceeds %ld)\n", program_name, optarg, LONG_MAX);
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

			/* Fehlerbehandlung zu großer od. zu kleiner Wert */
                        /*4294967295 => max. size of size_t (SHMMAX)
                          Dieser Wert ist aufzufinden unter : /proc/sys/kernel/shmmax */
                        if (ringbuffer_size > (long) (SIZE_MAX / sizeof(int))) {

                                fprintf(stderr, "usage %s: ringbuffersize <= %u and not %ld\n", program_name, SIZE_MAX / sizeof(int), ringbuffer_size);
                                exit(EXIT_FAILURE);
                        }
			if (ringbuffer_size < 1) {
				fprintf(stderr, "%s: ringbuffersize must be > 0 and not %ld\n", program_name, ringbuffer_size);
				usage();
				exit(EXIT_FAILURE);
			}
			break;
		default:
			/* wenn error, dann wird er von getopt() ausgegeben -> hier keine Ausgabe erforderlich */
			usage();
			exit(EXIT_FAILURE);
		}
	}

	/*If there are no more option characters, getopt() returns -1.
	  Then optind is the index in argv of the first argv-element that is not an option. */
	if (optind < argc) {
		usage();
		exit(EXIT_FAILURE);
	}

	return (size_t) ringbuffer_size;
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
 * \brief prints errno-message to stderr
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
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			error_message(errno);
		}
	}

	if (initval > 0)
		semid_sender = semid;
	else
		semid_empfaenger = semid;

	count++;

	return semid;
}

/**
 *
 * \brief Function to create a shared memory and return a pointer to it
 *
 * \return shm
 * \retval pointer to the created shared memory
 *
 */
int* get_shm(size_t ringbuffer_size, int flags)
{
	int *shmptr;
	size_t size = ringbuffer_size * sizeof(int);

	if (size > SIZE_MAX - sizeof(int)) {
		remove_sem();
		exit(EXIT_FAILURE);
	}

	shmid = shmget(KEY, size, 0660 | IPC_CREAT);
	if (shmid == -1) {
		error_message(errno);
		remove_sem();
		exit(EXIT_FAILURE);
	}

	shmptr = shmat(shmid, NULL, flags);
	if (shmptr == (int *)-1) {
		remove_all();
		exit(EXIT_FAILURE);
		error_message(errno);
	}

	return shmptr;
}

void remove_sem(void) {
	if (semrm(semid_empfaenger) == -1) {
		fprintf(stderr, "%s: Cannot claim semaphore.\n", program_name);
		exit(EXIT_FAILURE);
	}
	if (semrm(semid_sender) == -1) {
		fprintf(stderr, "%s: Cannot claim semaphore.\n", program_name);
		exit(EXIT_FAILURE);
	}
}

void remove_all(void) {
	/* Shared memory löschen - BEACHTEN: Es sollte nur ein Prozeß das Shared Memory Segment entfernen */
	if (shmctl(shmid, IPC_RMID, NULL) == -1)
	{
		error_message(errno);
	}
	if (semrm(semid_empfaenger) == -1) {
		fprintf(stderr, "%s: Cannot claim semaphore.\n", program_name);
		exit(EXIT_FAILURE);
	}
	if (semrm(semid_sender) == -1) {
		fprintf(stderr, "%s: Cannot claim semaphore.\n", program_name);
		exit(EXIT_FAILURE);
	}
}
/*
 * =================================================================== eof ==
 */
