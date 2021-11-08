#include "syscall.h"

int main() {
	Write("\nExec Adder 1\n", 20, ConsoleOutput);
	Join(Exec("../test/adder"));
	Write("\nExec Adder 2\n", 20, ConsoleOutput);
	Join(Exec("../test/adder"));
	Write("\nExec Adder 3\n", 20, ConsoleOutput);
	Join(Exec("../test/adder"));
	Write("\nExec Adder 4\n", 20, ConsoleOutput);
	Join(Exec("../test/adder"));
	
	Write("\n\nAll Adder programs succesfully executed!\n", 50, ConsoleOutput);
	Write("\n   .\n", 10, ConsoleOutput);
	Write("\n   .\n", 10, ConsoleOutput);
	Write("\n   .\n", 10, ConsoleOutput);
	Write("\nCommencing Exit Process\n\n", 30, ConsoleOutput);
	Exit(0);
}
