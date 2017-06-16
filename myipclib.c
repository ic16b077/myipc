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

	/* pull the first element of the argument vector in case of error handling */
	if (argv[0])
	{
		program_name = argv[0];
	}
	else
	{
		exit(EXIT_FAILURE);
	}

	/* verify that the appropriate argument amount is specified  */
	if (argc < 2)
	{
		fprintf(stderr, "%s: ringbuffersize must be specified\n", program_name);
		usage();
		exit(EXIT_FAILURE);
	}

	if (argc > 3) {
		usage();
		exit(EXIT_FAILURE);
	}

	/* parsing the command-line arguments with the use of the getopt() function */
	while ((option = getopt(argc, argv, "m:")) != -1)
	{
		switch (option)
		{
		case 'm':
			ringbuffer_size = strtol(optarg, &endptr, base);

			/* error handling for strtol()*/
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

			/* Check range of ringbuffer_size
			 * 4294967295 => max. size of size_t (SHMMAX)
			 * Value can be found in /proc/sys/kernel/shmmax or as the macro definition in the system library "<stdint.h>"
			 * Cast size_t to long for comparing 
			 */
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
			/* appropriate error handling is managed by the getopt()
			   usage message will be printed to stderr file stream
			*/
			usage();
			exit(EXIT_FAILURE);
		}
	}

	/* If there are no more option characters, getopt() returns -1.
	 * Then optind is the index in argv of the first argv-element that is not an option.
	 */
	if (optind < argc) {
		usage();
		exit(EXIT_FAILURE);
	}
	/* Explicit typecasting to an unsigned integer, since the shmget() system call processes inernally only with valid address ranges
	 * The address range of the specified shared memory is located from 0-SIZE_MAX (=> max. range for size_t)
	 * Cast to bigger data type was checked before
	 */
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
		/* Query if there is already an existing semaphore, which is linked to the system's unique identifier */
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
	/* The writer's semaphore will be invoked with the max. size of the ringbuffer and decremented on each writing process
	 * The reader's semaphore starts at NULL and after each character-oriented read-out of the ringbuffer the receiver will be blocked 
	 * => (P(semid_empfaenger) will decrement the value to zero)
	 */
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
 * \return shmptr
 * \retval pointer to the created shared memory
 *
 */
int* get_shm(size_t ringbuffer_size, int flags)
{
	int *shmptr;
	size_t size = ringbuffer_size * sizeof(int);

	/* Calculating the last possible memory block for an integer type
	   The set value in "/proc/sys/kernel/shmmax" is 4294967295, hence substracting the last four bytes
	   from the maximum bound of the unsigned integer "size_t" will deliver the last valid memory block for the given integer value
	   This calculation method returns "4294967291" as the last valid range
	   Referring to "Test 8" the given buffer size is -m1073741823, if you multiple this with sizeof(int) = 4 you will notice that the
	   returned value of buffer size is 4294967292, which exceeds the valid memory allocation range from above
	*/
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

/**
*
* \brief Function to delete the allocated unique semaphore identifier
* remove_sem() will be invoked in each failed system call, therefore only the semaphores get removed
*
*/
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

/**
*
* \brief Function to delete all allocated resources
* This function will be invoked in the unluckily case of a receiving error in either processes (asynchronous termination)
* In the case of a flawless program execution the receiver invokes remove_all() to release all resources (synchronous termination)
*
*/
void remove_all(void) {
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
