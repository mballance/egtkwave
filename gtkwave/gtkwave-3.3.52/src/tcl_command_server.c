#include "config.h"
#include "globals.h"
#include <glib.h>
#include <gio/gio.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <memory.h>
#include <stdlib.h>

#ifdef HAVE_LIBTCL
#include <tcl.h>
#endif

/**
 * Message format is:
 * [MAGIC:4] [LEN:4] [DATA]
 */

typedef struct tcl_command_server_connection_s {
	uint8_t			*buf;
	uint32_t		 buf_sz;
	uint32_t		 buf_max;

	uint8_t			*resp_buf;
	uint32_t		 resp_buf_max;

} tcl_command_server_connection_t;

static gboolean socket_read(
		GIOChannel *source,
		GIOCondition cond,
		gpointer data)
{
	GIOStatus status;
	gsize sz;
	tcl_command_server_connection_t *conn =
			(tcl_command_server_connection_t *)data;
	fprintf(stdout, "socket_read\n");

	do {
		if (conn->buf_sz >= conn->buf_max) {
			uint8_t *tmp = conn->buf;
			conn->buf = (uint8_t *)malloc(sizeof(uint8_t)*(conn->buf_max+4096));
			conn->buf_max += 4096;
			if (tmp) {
				memcpy(conn->buf, tmp, conn->buf_sz);
				free(tmp);
			}
		}
		status = g_io_channel_read_chars(source,
				&conn->buf[conn->buf_sz], (conn->buf_max-conn->buf_sz), &sz, NULL);

		if (sz >= 0) {
			conn->buf_sz += sz;
		}
	} while ((sz > 0) && (status == G_IO_STATUS_NORMAL || status == G_IO_STATUS_AGAIN));

	while (conn->buf_sz >= 8) {
		uint32_t magic =
				(conn->buf[0] << 0) |
				(conn->buf[1] << 8) |
				(conn->buf[2] << 16) |
				(conn->buf[3] << 24);
		uint32_t len =
				(conn->buf[4] << 0) |
				(conn->buf[5] << 8) |
				(conn->buf[6] << 16) |
				(conn->buf[7] << 24);

		fprintf(stdout, "magic=0x%08x len=0x%08x\n", magic, len);
		fflush(stdout);

		if (conn->buf_sz >= (len + 8)) {
#ifdef HAVE_LIBTCL
			{
				int tclrc;
				uint32_t msg_len;
				uint32_t write_idx = 0;
				gsize write_sz;
				const char *result;
				GIOStatus write_status;

				fprintf(stdout, "Eval %s\n", &conn->buf[8]);
				fflush(stdout);
				tclrc = Tcl_Eval(GLOBALS->interp, &conn->buf[8]);

				result = Tcl_GetStringResult(GLOBALS->interp);

				msg_len = 1 + strlen(result) + 1;

				if ((8+msg_len) > conn->resp_buf_max) {
					if (conn->resp_buf) {
						free(conn->resp_buf);
					}
					conn->resp_buf = (uint8_t *)malloc((msg_len+8)+4096);
					conn->resp_buf_max = ((msg_len+8)+4096);
				}

				conn->resp_buf[4] = (msg_len >> 0);
				conn->resp_buf[5] = (msg_len >> 8);
				conn->resp_buf[6] = (msg_len >> 16);
				conn->resp_buf[7] = (msg_len >> 24);

				conn->resp_buf[8] = tclrc;

				memcpy(&conn->resp_buf[9], result, (msg_len-1));

				while (write_idx < (msg_len+8)) {
					write_status = g_io_channel_write_chars(source,
							&conn->resp_buf[write_idx],
							((msg_len+8)-write_idx),
							&write_sz, NULL);

					write_idx += write_sz;

					if (write_status == G_IO_STATUS_ERROR) {
						break;
					}
				}
				g_io_channel_flush(source, NULL);
			}
#endif
			fprintf(stdout, "Have full message\n");
			fflush(stdout);



			if (conn->buf_sz == (len+8)) {
				// Just reset
				conn->buf_sz = 0;
			} else {
				// Move the next message back
				memcpy(conn->buf, &conn->buf[len+8],
						(conn->buf_sz-(len+8)));
				conn->buf_sz -= (len+8);
			}
		}
	}

	fprintf(stdout, "%d bytes in the buffer status=%d\n", conn->buf_sz, status);

	if (status == G_IO_STATUS_ERROR || status == G_IO_STATUS_EOF) {
		// TODO: close down connection
		fprintf(stdout, "Shutting down\n");
		if (conn->buf) {
			free(conn->buf);
		}
		if (conn->resp_buf) {
			free(conn->resp_buf);
		}
		free(conn);
	}

	return TRUE;
}


static gboolean connect_callback(
		GSocketService  *service,
		GSocketConnection *connection,
		GObject			*source_object,
		gpointer		user_data)
{
	GSocket *socket;
	GSource *source;
	GIOChannel *channel;
	gint fd;
	tcl_command_server_connection_t *conn;

	g_object_ref(connection);

	socket = g_socket_connection_get_socket(connection);

	fd = g_socket_get_fd(socket);

	channel = g_io_channel_unix_new(fd);
	g_io_channel_set_encoding(channel, NULL, NULL);

	conn = (tcl_command_server_connection_t *)malloc(sizeof(tcl_command_server_connection_t));
	memset(conn, 0, sizeof(tcl_command_server_connection_t));

	g_io_add_watch(channel, G_IO_IN+G_IO_HUP,
			(GIOFunc)socket_read, conn);

	return TRUE;
}

void tcl_command_server_init(int cmdserver_port)
{
	GError *error = NULL;
	GSocketService *service;
	GInetAddress *address;
	GSocketAddress *socket_address, *effective_address;
	g_type_init();

	service = g_socket_service_new();
	address = g_inet_address_new_loopback(G_SOCKET_FAMILY_IPV4);
	socket_address = g_inet_socket_address_new(address, cmdserver_port);

	g_socket_listener_add_address(G_SOCKET_LISTENER(service), socket_address,
			G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_TCP, NULL, &effective_address, NULL);

	cmdserver_port = g_inet_socket_address_get_port((GInetSocketAddress *)effective_address);

	fprintf(stdout, "GTKWAVE: Command server listening on port %d\n", cmdserver_port);

	g_object_unref(socket_address);
	g_object_unref(address);
	g_object_unref(effective_address);

	g_socket_service_start(service);

	g_signal_connect(service, "incoming",
		G_CALLBACK(connect_callback), NULL);


	g_print("Waiting for client\n");
}
