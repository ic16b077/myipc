#include "myipclib.h"

int main(int argc, char* argv[]) {
	int ringbuffer_size;

	ringbuffer_size = get_ringbuffer_size(argc, argv);

        printf("%d\n", ringbuffer_size);

	return 0;
}
