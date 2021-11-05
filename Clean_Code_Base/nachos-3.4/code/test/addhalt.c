#include "syscall.h"

int main() {
	Write("\n\nExec 1\n", 12, ConsoleOutput);
	Write("\n\nBeginning multiprog.\n", 27, ConsoleOutput);
	Write("\n\nExec 2\n", 12, ConsoleOutput);
		Exec("../test/adder");
	//Write("\n\nExec 2\n", 12, ConsoleOutput);
		//Join(Exec("../test/halt"));
	Exit(0);
}
