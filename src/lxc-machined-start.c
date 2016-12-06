#include <errno.h>
#include <getopt.h>
#include <lxc/lxccontainer.h>
#include <stdio.h>
#include <stdnoreturn.h>

#include "util.h"

static _Noreturn void usage(FILE *out) {
	fprintf(out, "usage: %s [options]\n\n", program_invocation_short_name);
	fputs("Options :\n"
			"-n, --name=NAME     NAME of the container\n"
			"-P, --lxcpath=PATH  Use specified container path\n", out);
	exit(out == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

#define LXC_MACHINED_START_VERSION "1"

static inline void lxc_container_putp(struct lxc_container **p) {
	if (*p) {
		lxc_container_put(*p);
	}
}
#define _cleanup_lxc_container_  _cleanup_(lxc_container_putp)

struct start_arguments {
	char *name, *lxcpath;
};

int main(int argc, char** argv) {
	int r;
	struct _cleanup_lxc_container_ lxc_container *c = NULL;
	struct start_arguments args = {};
	static struct option opts[] = {
		{ "help", no_argument,       0, 'h' },
		{ "version", no_argument,   0, 'v' },
		{ "name", required_argument, 0, 'n' },
		{ "lxcpath", required_argument, 0, 'P' },
		{ 0, 0, 0, 0 }
	};

	while(1) {
		r = getopt_long (argc, argv, "hvn:p:", opts, NULL);

		if (r == -1) {
			break;
		}

		switch(r) {
			case 'h':
				usage(stdout);
				break;
			case 'v':
				printf("%s %s\n", program_invocation_short_name, LXC_MACHINED_START_VERSION);
				return 0;
			case 'n':
				args.name = optarg;
				break;
			case 'P':
				remove_trailing_slashes(optarg);
				args.lxcpath = optarg;
				break;
			default:
				usage(stderr);
		}
	}

	if (!args.name) {
		fprintf(stderr, "missing container name, use --name option");
		return EXIT_FAILURE;
	}

	c = lxc_container_new(args.name, args.lxcpath);
	if (!c) {
		fprintf(stderr, "Failed to create lxc_container struct: %m");
		return EXIT_FAILURE;
	}

	if (!c->is_defined(c)) {
		fprintf(stderr, "Error: container '%s' is not defined\n", c->name);
		return EXIT_FAILURE;
	}

	if (!c->may_control(c)) {
		fprintf(stderr, "Insufficent privileges to control '%s'\n", c->name);
		return EXIT_FAILURE;
	}

	if (c->is_running(c)) {
		fprintf(stderr, "Container '%s' is already running\n", c->name);
		return EXIT_FAILURE;
	}

	if (!c->start(c, 0, NULL)) {
		fprintf(stderr, "Failed to start the container");
		return EXIT_FAILURE;
	}

	//if (!c->wait(c, "RUNNING", -1)) {
	//	fprintf(stderr, "Failed to start the container");
	//	exit(EXIT_FAILURE);
	//}

	return 0;
}
