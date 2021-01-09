#include"threading.h"

int main() {

	unsigned int max_base = 36;
	for (unsigned int base = 3; base <= max_base; base++) {
		threading(base);
	}
}
