#include "syscall.h"

int main() {
	Write("\n\nExec 1\n", 12, ConsoleOutput);
		Exec("../test/adder");
	Write("\n\nExec 2\n", 12, ConsoleOutput);
		Exec("../test/halt");
	Exit(0);
}
