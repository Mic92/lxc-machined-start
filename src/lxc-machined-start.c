#include <errno.h>
#include <getopt.h>
#include <lxc/lxccontainer.h>
#include <stdio.h>
#include <stdnoreturn.h>

#include "util.h"

static _Noreturn void usage(FILE *out) {
	fprintf(out, "usage: %s [options] [key ...]\n\n", program_invocation_short_name);
	fputs("lxc-start start COMMAND in specified container NAME\n\n"
			"Options :\n"
			"-n, name=NAME NAME of the container\n", out);
	exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
	int r;
	static struct option opts[] = {
		{ "help", no_argument,       0, 'h' },
		{ "version", no_argument,   0, 'v' },
		{ "name", required_argument, 0, 'n' },
		{ 0, 0, 0, 0 }
	};

	while(1) {
		r = getopt_long (argc, argv, "n:", opts, NULL);

		if (r == -1) {
			break;
		}

		switch(r) {
			case 'n':

				break;
		}
	}
	struct lxc_container *c;
	int ret = 1;

	/* Setup container struct */
	c = lxc_container_new("apicontainer", NULL);
	if (!c) {
		fprintf(stderr, "Failed to setup lxc_container struct");
		goto out;
	}

	if (c->is_defined(c)) {
		fprintf(stderr, "Container already exists");
		goto out;
	}

	/* Create the container */
	if (!c->create(c, "download", NULL, NULL, LXC_CREATE_QUIET,
				"-d", "ubuntu", "-r", "trusty", "-a", "i386", NULL)) {
		fprintf(stderr, "Failed to create container rootfs");
		goto out;
	}

	/* Start the container */
	if (!c->start(c, 0, NULL)) {
		fprintf(stderr, "Failed to start the container");
		goto out;
	}

	/* Query some information */
	printf("Container state: %s", c->state(c));
	printf("Container PID: %d", c->init_pid(c));

	/* Stop the container */
	if (!c->shutdown(c, 30)) {
		printf("Failed to cleanly shutdown the container, forcing.");
		if (!c->stop(c)) {
			fprintf(stderr, "Failed to kill the container.");
			goto out;
		}
	}

	/* Destroy the container */
	if (!c->destroy(c)) {
		fprintf(stderr, "Failed to destroy the container.");
		goto out;
	}

	ret = 0;
out:
	lxc_container_put(c);
	return ret;
}
