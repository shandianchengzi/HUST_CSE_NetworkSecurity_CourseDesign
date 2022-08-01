#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <unistd.h>

#define NETLINK_TEST	17
#define MSG_LEN	256

char *default_data = "Netlink Test Default Data";

struct msg_to_kernel {
	struct nlmsghdr hdr;
	char data[MSG_LEN];
};

struct u_packet_info {
	struct nlmsghdr hdr;
	char msg[MSG_LEN];
};

int main(int argc, char *argv[])
{
	char *data;
	int dlen;
	struct sockaddr_nl local;
	struct sockaddr_nl kpeer;
	int skfd, ret, kpeerlen = sizeof(struct sockaddr_nl);
	struct nlmsghdr *message;
	struct u_packet_info info;
	char *retval;

	if (argc == 2) {
		data = argv[1];
	} else {
		data = default_data;
	}
	dlen = strlen(data) + 1;

	skfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
	if (skfd < 0) {
		printf("can not create a netlink socket\n");
		return -1;
	}

	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0;
	if (bind(skfd, (struct sockaddr *) &local, sizeof(local)) != 0) {
		printf("bind() error\n");
		return -1;
	}
	memset(&kpeer, 0, sizeof(kpeer));
	kpeer.nl_family = AF_NETLINK;
	kpeer.nl_pid = 0;
	kpeer.nl_groups = 0;

	message = (struct nlmsghdr *) malloc(sizeof(struct msg_to_kernel));
	if (message == NULL) {
		printf("malloc() error\n");
		return -1;
	}

	memset(message, '\0', sizeof(struct nlmsghdr));
	message->nlmsg_len = NLMSG_SPACE(dlen);
	message->nlmsg_flags = 0;
	message->nlmsg_type = 0;
	message->nlmsg_seq = 0;
	message->nlmsg_pid = local.nl_pid;

	retval = memcpy(NLMSG_DATA(message), data, strlen(data));

	printf("message sendto kernel, content: '%s', len: %d\n", (char *) NLMSG_DATA(message), message->nlmsg_len);
	ret = sendto(skfd, message, message->nlmsg_len, 0, (struct sockaddr *) &kpeer, sizeof(kpeer));
	if (!ret) {
		perror("sendto:");
		exit(-1);
	}

	ret = recvfrom(skfd, &info, sizeof(struct u_packet_info), 0, (struct sockaddr *) &kpeer, &kpeerlen);
	if (!ret) {
		perror("recvfrom:");
		exit(-1);
	}

	printf("message recvfrom kernel, content: '%s'\n", (char *) info.msg);

	close(skfd);
	return 0;
}

