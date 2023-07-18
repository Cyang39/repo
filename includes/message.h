#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include "util.h"

void print_info(struct info *user);
void gen_login_msg(struct message *msg, char *username, char *password);
void gen_query_by_name_req_msg(struct message *msg, char *target_name);
void gen_query_by_name_res_msg(struct message *msg, struct info *user);

#endif // !__MESSAGE_H__
