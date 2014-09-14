const char *my_path[] = {
	"./",
	"/usr/bin/",
	"/bin/",
	NULL
};

while (...)
{
	/* Wait for input */
	printf ("prompt> ");
	fgets (...);

  	/* Parse input */
	while (( ... = strsep (...)) != NULL)
	{
		...
	}

	/* Check if executable exists and is executable */

	/* Launch executable */
	if (fork () == 0)
	{
		...
		execv (...);
		...
	}
	else
	{
		wait (...);
	}
}