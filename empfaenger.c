/**
 * @file empfaenger.c
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
 * \brief Implementaion of a receiver program
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

	/* The value linked to the stored address in "shmadr" may not be modified from the receiver's view */
        const int* shmadr;
	int i = 0;

	/* Get ringbuffer size */
	ringbuffer_size = get_ringbuffer_size(argc, argv);

	/* Initialising the semaphore identifier */
	semid_empfaenger = get_semid(0);
	semid_sender = get_semid(ringbuffer_size);

	/* Attaching the shared memory to the process's virtual memory
	 * The receiver has only authorized read-only permissions
	*/
	shmadr = get_shm(ringbuffer_size, SHM_RDONLY);

        do
        {
		/* Wait for signal */
		if (P(semid_empfaenger) == -1) {
			/* External signals need to be handled correctly */
			if (errno == EINTR) {
				continue;
			}
			else {
				fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
				remove_all();
				exit(EXIT_FAILURE);
			}
		}

		/* Read character by character from shared memory */
		character = shmadr[i++ % ringbuffer_size];

		/* Output character */
		if (character != EOF) {
			fputc(character, stdout);
			/* Error handling for fputc() */
			if (ferror(stdout)) {
				fprintf(stderr, "%s: Error writing to stdout.\n", argv[0]);
				remove_all();
				exit(EXIT_FAILURE);
			}
		}

		/* Send signal */
		if(V(semid_sender) == -1) {
			remove_all();
			exit(EXIT_FAILURE);
		}
        } while(character != EOF);

        if (shmdt(shmadr) == -1) {
		fprintf(stderr, "%s: Shared memory could not be detached\n", argv[0]);
		remove_all();
		exit(EXIT_FAILURE);
        }

	/* After receiving EOF the receiver shall terminate successfully and release all resources*/
	remove_all();

        return EXIT_SUCCESS;
}

/*
 * =================================================================== eof ==
 */
