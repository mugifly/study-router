#include <stdio.c>
#include <string.h>
/* unistd = Unix standard */
#include <unistd.h>
/* ioctl = I/O control (for descriptor) */
#include <sys/ioctl.h>
/* inet.h = Convert IP-address with string */
#include <arpa/inet.h>

#include <sys/socket.h>

#include <linux/if.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <netinet/if_ether.h>

int InitRawSocket(char *device, int promiscFlag, int ipOnly){
	struct ifreq ifreq; /*  */
	struct sockaddr_ll sa; 
	int soc; /* Descriptor */

	if (ipOnly) {
		/* Get the data-link-layer as only the IP-packet, by using the socket method. */
		if ((soc = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP))) < 0){
			perror("socket");
			return(-1);
		}
	} else {
		/* Get the data-link-layer as all-packet, by using the socket method. */
		if ((soc = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0){
			perror("socket");
			return(-1);
		}
	}

	memset(&ifreq, 0, sizeof(struct ifreq));
	strncpy(ifreq.ifr_name, device, sizeof(ifreq.ifr_name) - 1);

	/* Get the index from network-interface name (soc), then into the ifreq struct.
	 SIOCGI* = Parameter for getter. */
	if (ioctl(soc, SIOCGIFINDEX, &ifreq) < 0) {
		perror("ioctl");
		close(soc);
		return(-1);
	}

	sa.sll_family = PF_PACKET;
	if (ipOnly) {
		sa.sll_protocol = htons(ETH_P_IP);
	} else {
		sa.sll_protocol = htons(ETH_P_ALL);
	}

	sa.sll_ifindex = ifreq.ifr_ifindex;

	/* Bind a soc with the interface  */
	if (bind( soc, (struct sockaddr *)&sa, sizeof(sa) ) < 0){
		perror("bind");
		close(soc);
		return(-1);
	}

	if (promiscFlag) {
		/* Get the flag on the device */
		if (ioctl(soc, SIOCGIFFLAGS, &ifreq) < 0) {
			perror("ioctl");
			close(soc);
			return(-1);
		}

		/* Turn on the IFF_PROMISC bit on ifr_flags */
		ifreq.ifr_flags = ifreq.ifr_flags | IFF_PROMISC;

		/* Write the flag. SIOCSI* = Parameter for setter.  */
		if (ioctl(soc, SIOCSIFFLAGS, &ifreq) < 0) {
			perror("ioctl");
			close(soc);
			return(-1);
		}
	}

	/* Return the descriptor */
	return(soc);
}

/* Convert the MAC-address with string. useful for the debug. */
char *myether_ntoa_r(u_char *hwaddr, char *buf, socklen_t size){
	snprintf(buf, size, "%02x:%02x:%02x:%02x:%02x:%02x",
		hwaddr[0], hwaddr[1], hwaddr[2], hwaddr[3], hwaddr[4], hwaddr[5]);

	return(buf);
}

<list>int PrintEtherHeader(struct ether_header *eh, FILE *fp){
	char buf[80];
	fprintf(fp, "ether_header----------\n");
	fprintf(fp, "ether_dhost=%s \n", myether_ntoa_r(eh->ether_dhost, buf, sizeof(buf)));
	
}