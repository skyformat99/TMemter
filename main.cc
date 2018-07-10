#include "TMemter.h"

int main(int argc, char *argv[]) {
	if (argc != 5) {
		fprintf(stderr, "Usage: %s <connections> <timeout> <ip> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	TMemter t(atoi(argv[1]), atoi(argv[2]), argv[3], atoi(argv[4]));
	t.start();
	return 0;
}
