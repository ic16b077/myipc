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
        int semid_sender, semid_empfaenger, character;
        int* shm;
	int i = 0;

        /* Get size of ringbuffer */
        ringbuffer_size = get_ringbuffer_size(argc, argv);
        printf("ringbuffer_size = %lld\n", ringbuffer_size);

        /* Semaphores */
        semid_empfaenger = get_semid(0);
        printf("semid_empfaenger = %d\n", semid_empfaenger);

        semid_sender = get_semid(ringbuffer_size);
        printf("semid_sender = %d\n", semid_sender);

        /* Shared memory */
        shm = get_shm(ringbuffer_size, SHM_RDONLY);

        do
        {
                P(semid_empfaenger);

                /* Get character */
                character = shm[i++ % ringbuffer_size++];
		fputc(character, stdout);

                V(semid_sender);
        } while(character != EOF);

        /* Semaphoren löschen */
        semrm(semid_empfaenger);
        semrm(semid_sender);

        /* Shared memory löschen */
        shm_del();

        return EXIT_SUCCESS;
}

/*
 * =================================================================== eof ==
 */
