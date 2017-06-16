/**
 * @file sender.c
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

/*
 * ------------------------------------------------------------- functions --
 */

/*
 * --------------------------------------------------------------- globals --
 */

/**
 *
 * \brief Implementaion of a sender program
 *
 * This is the main entry point for any C program.
 *
 * \param argc the number of arguments
 * \param argv the arguments itselves (including the program name in argv[0])
 *
 * \return success or failure
 * \retval EXIT_SUCCESS success
 * \retval EXIT_FAILURE failure
 *
 */
int main(int argc, char* argv[]) {
	size_t ringbuffer_size;
	int semid_sender, semid_empfaenger, character;
	int* shmadr;
	int i = 0;

	ringbuffer_size = get_ringbuffer_size(argc, argv);

	semid_empfaenger = get_semid(0);
	semid_sender = get_semid(ringbuffer_size);

	/* Attaching the shared memory to the process's virtual memory
	 * The sender is authorized for read-write
	 */
	shmadr = get_shm(ringbuffer_size, 0);

	do
	{
		/* Wait for signal */
		if (P(semid_sender) == -1) {
			if (errno == EINTR) {
				continue;
			}
			else {
				fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
				remove_all();
				exit(EXIT_FAILURE);
			}
		}

		/* Get input */
        	character = fgetc(stdin);
		/* Error handling of fgetc() */
		if (ferror(stdin)) {
			fprintf(stderr, "%s: Error reading from stdin.\n", argv[0]);
			remove_all();
			exit(EXIT_FAILURE);
		}

		/* Write input to shared memory */
		shmadr[i++ % ringbuffer_size] = character;

		/* Send signal */
		if (V(semid_empfaenger) == -1) {
			remove_all();
			exit(EXIT_FAILURE);
		}
	} while(character != EOF);

	if (shmdt(shmadr) == -1) {
		fprintf(stderr; "%s: Shared memory could not be detached", argv[0]);
		remove_all();
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

/*
 * =================================================================== eof ==
 */
