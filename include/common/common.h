#ifndef COMMON_H
#define COMMON_H

#define DFU_UNKOWN_TYPE (0)
#define DFU_LEGACY      (1 << 1)
#define DFU_IPHONE7     (1 << 2)
#define DFU_IPHONEX     (1 << 3)

#define MEOW_CONTROL_TRANSFER_TIME(c, brt, br, wv, wi, d, wl, t) usb_ctrl_transfer_with_time(c, brt, br, wv, wi, d, wl, t)
#define MEOW_ASYNC_CONTROL_TRANSFER_CANCEL(c, brt, br, wv, wi, d, wl, t) async_usb_ctrl_transfer_with_cancel(c, brt, br, wv, wi, d, wl, t)
#define MEOW_SEND_CAT(c, b, s) send_data(c, b, s)
#define MEOW_SEND_CAT_TIME(c, b, s, t) send_data_with_time(c, b, s, t)
#define MEOW_RECONNECT(c, r, stg, f, srnm, sec) io_reconnect(c, r, stg, f, srnm, sec)
#define MEOWMEOW(haystack, haystacklen, needle, needlelen) memmem(haystack, haystacklen, needle, needlelen)
#define MEOWSET(dest, c, count) memset(dest, c, count)
#define MEOWCPY(dest, src, count) memcpy(dest, src, count)
#define SPRINTMEOW(buffer, fmtstr, arglist) sprintf(buffer, fmtstr, arglist)

int enter_dfu_via_recovery(io_client_t client);
int payload_stage2(io_client_t client, checkra1n_payload_t payload);
int sendPongo(io_client_t client, checkra1n_payload_t payload);
int connect_to_stage2(io_client_t client, checkra1n_payload_t payload);

transfer_t send_data(io_client_t client, unsigned char* buf, size_t size);
transfer_t send_data_with_time(io_client_t client, unsigned char* buf, size_t size, int timeout);
transfer_t get_status(io_client_t client, unsigned char* buf, size_t size);
transfer_t send_abort(io_client_t client);

const char *IOReturnName(IOReturn res);

#endif
