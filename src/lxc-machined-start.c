#include <errno.h>
#include <getopt.h>
#include <lxc/lxccontainer.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <systemd/sd-bus.h>
#include <systemd/sd-id128.h>

#include "util.h"

const char *bus_error_message(const sd_bus_error *e, int error) {
	if (e) {
		/* Sometimes, the D-Bus server is a little bit too verbose with
		 * its error messages, so let's override them here */
		if (sd_bus_error_has_name(e, SD_BUS_ERROR_ACCESS_DENIED))
			return "Access denied";

		if (e->message) {
			return e->message;
		}
	}

	if (error < 0) {
		error = -error;
	}

	return strerror(error);
}

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

void register_machine(
		char* machine_name,
		pid_t pid,
		sd_id128_t uuid,
		const char *directory,
		int local_ifindex) {
	int r;
	_cleanup_(sd_bus_flush_close_unrefp) sd_bus *bus = NULL;
	_cleanup_(sd_bus_error_free) sd_bus_error error = SD_BUS_ERROR_NULL;
	r = sd_bus_default_system(&bus);
	if (r < 0) {
		fprintf(stderr, "Failed to open system bus: %m");
	} else {
		r = sd_bus_call_method(
				bus,
				"org.freedesktop.machine1",
				"/org/freedesktop/machine1",
				"org.freedesktop.machine1.Manager",
				"RegisterMachineWithNetwork",
				&error,
				NULL,
				"sayssusai",
				machine_name,
				SD_BUS_MESSAGE_APPEND_ID128(uuid),
				"lxc",
				"container",
				(uint32_t) pid,
				directory ? directory : "",
				local_ifindex > 0 ? 1 : 0, local_ifindex);

		if (r < 0) {
			fprintf(stderr, "Failed to register machine: %s\n", bus_error_message(&error, r));
        }
	}
}

int start_container(struct start_arguments args) {
	int r;
	struct _cleanup_lxc_container_ lxc_container *c = NULL;
	sd_id128_t uuid;

	c = lxc_container_new(args.name, args.lxcpath);
	if (!c) {
		fprintf(stderr, "Failed to create lxc_container struct: %m\n");
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
		fprintf(stderr, "Failed to start the container\n");
		return EXIT_FAILURE;
	}

	r = sd_id128_from_string("231624bcd4b04700860f2fabc997e8be", &uuid);
	if (r < 0) {
		fprintf(stderr, "Invalid UUID: %s", "231624bcd4b04700860f2fabc997e8be");
	}

	register_machine(args.name,
			c->init_pid(c),
			uuid,
			c->get_running_config_item(c, "lxc.rootfs"),
			0);

	if (!c->wait(c, "STOPPING", -1)) {
		fprintf(stderr, "Failed to start the container\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char** argv) {
	int r;
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

	return start_container(args);
}
