#include "stdlib.h"
#include "signal.h"

void handleSig(int sig)
{
  (void)sig;
  exit(1);
}

int main(void)
{
	signal(SIGQUIT, handleSig);
	char *me = malloc(sizeof(char) * 5);
	while (1) {}
	return 0;
}