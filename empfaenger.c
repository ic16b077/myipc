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
	long long ringbuffer_size;
	int semid_sender;
	int semid_empfaenger;

	ringbuffer_size = get_ringbuffer_size(argc, argv);
        printf("ringbuffer_size = %lld\n", ringbuffer_size);

	semid_empfaenger = get_semid(0);
	printf("semid_empfaenger = %d\n", semid_empfaenger);

        semid_sender = get_semid(ringbuffer_size);
        printf("semid_sender = %d\n", semid_sender);

	semrm(semid_empfaenger);
	semrm(semid_sender);

	return EXIT_SUCCESS;
}

/*
 * =================================================================== eof ==
 */
