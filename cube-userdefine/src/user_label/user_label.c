#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>
 
#include "data_type.h"
#include "cube.h"
#include "cube_define.h"
#include "cube_record.h"
#include "user_define.h"
#include "record_define.h"
#include "label_define.h"
// add para lib_include
int user_label_init(void * sub_proc, void * para)
{
	int ret;
	// add yorself's module init func here
	return 0;
}
int user_label_start(void * sub_proc, void * para)
{
	int ret;
	void * recv_msg;
	int type;
	int subtype;
	// add yorself's module exec func here
	while(1)
	{
		usleep(time_val.tv_usec);
		ret=ex_module_recvmsg(sub_proc,&recv_msg);
		if(ret<0)
			continue;
		if(recv_msg==NULL)
			continue;
		type=message_get_type(recv_msg);
		subtype=message_get_subtype(recv_msg);
		if(!memdb_find_recordtype(type,subtype))
		{
			printf("message format (%d %d) is not registered!\n",
			message_get_type(recv_msg),message_get_subtype(recv_msg));
			continue;
		}
		if((type==TYPE(MESSAGE))&&(subtype==SUBTYPE(MESSAGE,CONN_ACKI)))
		{
			ret=proc_login_useraddr(sub_proc,recv_msg);
		}
		else if((type==TYPE(USER_DEFINE))&&(subtype==SUBTYPE(USER_DEFINE,LOGIN)))
		{
			ret=proc_login_addrattach(sub_proc,recv_msg);
		}
		else if((type==TYPE(USER_DEFINE))&&(subtype==SUBTYPE(USER_DEFINE,RETURN)))
		{
			ret=proc_login_addrverify(sub_proc,recv_msg);
		}
		else if(type==TYPE(RECORD_DEFINE))
		{
			ret=proc_login_userattach(sub_proc,recv_msg);
		}
	}
	return 0;
}

int proc_login_useraddr(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(LABEL_DEFINE,ADDR) * addr_label;
	RECORD(MESSAGE,CONN_ACKI) * conn_acki;
	BYTE conn_uuid[DIGEST_SIZE];
	
	ret=message_get_record(recv_msg,&conn_acki,0);
	if(ret<0)
		return ret;
	comp_proc_uuid(conn_acki->uuid,conn_acki->client_process,conn_uuid);


	DB_RECORD * db_record;

	db_record=memdb_find(conn_uuid,TYPE_PAIR(LABEL_DEFINE,ADDR));
	//db_record=memdb_find_first(TYPE_PAIR(LABEL_DEFINE,ADDR),"proc_uuid",conn_uuid);
	if(db_record==NULL)
	{
		addr_label=Talloc0(sizeof(*addr_label));
		if(addr_label==NULL)
			return -ENOMEM;
	//	Memcpy(addr_label->proc_uuid,conn_uuid,DIGEST_SIZE);
		Memcpy(addr_label->node_uuid,conn_acki->uuid,DIGEST_SIZE);
		Memset(addr_label->proc_name,0,DIGEST_SIZE);
		Strncpy(addr_label->proc_name,conn_acki->client_name,DIGEST_SIZE);
		print_cubeaudit("Add a new link, proc name %s\n",addr_label->proc_name);
		memdb_store(addr_label,TYPE_PAIR(LABEL_DEFINE,ADDR),NULL);
	}
	else
	{
		addr_label=(RECORD(LABEL_DEFINE,ADDR) *)db_record->record;
		print_cubeaudit("link exists!, proc name %s\n",addr_label->proc_name);
	}
	
	ret=ex_module_sendmsg(sub_proc,recv_msg);
	return ret;
}

int proc_login_addrattach(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(LABEL_DEFINE,ADDR) * addr_label;
	RECORD(USER_DEFINE,LOGIN) * user_login;
	MSG_EXPAND * msg_expand;
	BYTE *proc_uuid;

	ret=message_get_record(recv_msg,&user_login,0);
	if(ret<0)
		return ret;
	proc_uuid = message_get_sender(recv_msg);

	DB_RECORD * db_record;

	db_record=memdb_find(proc_uuid,TYPE_PAIR(LABEL_DEFINE,ADDR));
	//db_record=memdb_find_first(TYPE_PAIR(LABEL_DEFINE,ADDR),"proc_uuid",proc_uuid);
	if(db_record==NULL)
	{
		print_cubeerr("user_label: lost addr!");
		return -EINVAL;
	}
	addr_label=db_record->record;

	addr_label->user_name=dup_str(user_login->user_name,DIGEST_SIZE);
	if(addr_label->user_name==NULL)
		return -ENOMEM;

	ret=message_add_expand_data(recv_msg,TYPE_PAIR(LABEL_DEFINE,ADDR),addr_label);
	if(ret<0)
		return ret;
	
	ret=ex_module_sendmsg(sub_proc,recv_msg);
	return ret;
}
int proc_login_addrverify(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(LABEL_DEFINE,ADDR) * addr_label;
	RECORD(USER_DEFINE,RETURN) * user_return;
	RECORD(USER_DEFINE,SERVER_STATE) * server_state;
	MSG_EXPAND * msg_expand;
	BYTE *proc_uuid;

	ret=message_get_record(recv_msg,&user_return,0);
	if(ret<0)
		return ret;

	ret=message_remove_expand(recv_msg,TYPE_PAIR(LABEL_DEFINE,ADDR),&msg_expand);
	if(ret<0)
		return ret;
	if(msg_expand==NULL)
	{
		print_cubeerr("can't find addr attached!\n");	
		return -EINVAL;
	}
	addr_label=msg_expand->expand;

    if(user_return->return_code==1)
	{

		DB_RECORD * db_record;
		db_record=memdb_find_first(TYPE_PAIR(USER_DEFINE,SERVER_STATE),"user_name",addr_label->user_name);
		if(db_record==NULL)
		{
			print_cubeerr("user_label: lost user!");
			return -EINVAL;
		}	
		server_state=db_record->record;
		Memcpy(server_state->node_uuid,addr_label->node_uuid,DIGEST_SIZE);			
		Memcpy(server_state->proc_name,addr_label->proc_name,DIGEST_SIZE);			

		addr_label->role=server_state->role;
		memdb_store(addr_label,TYPE_PAIR(LABEL_DEFINE,ADDR),NULL);
	}		
	else
	{
		Free0(addr_label->user_name);
		addr_label->user_name=NULL;
		addr_label->role=0;
		memdb_store(addr_label,TYPE_PAIR(LABEL_DEFINE,ADDR),NULL);
	}

	ret=ex_module_sendmsg(sub_proc,recv_msg);
	return ret;
}

int proc_login_userattach(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(LABEL_DEFINE,ADDR) * addr_label;
	RECORD(LABEL_DEFINE,USER) * user_label;
	MSG_EXPAND * msg_expand;
	BYTE * proc_uuid;

	proc_uuid = message_get_sender(recv_msg);

	DB_RECORD * db_record;

	db_record=memdb_find(proc_uuid,TYPE_PAIR(LABEL_DEFINE,ADDR));
	if(db_record==NULL)
	{
		print_cubeerr("user_label: can't find addr!");
		return -EINVAL;
	}
	addr_label=db_record->record;

	user_label=Talloc0(sizeof(*user_label));
	if(user_label==NULL)
		return -ENOMEM;
	
	user_label->user_name=dup_str(addr_label->user_name,DIGEST_SIZE);
	if(user_label->user_name==NULL)
		return -ENOMEM;
	user_label->role=addr_label->role;

	ret=message_add_expand_data(recv_msg,TYPE_PAIR(LABEL_DEFINE,USER),user_label);
	if(ret<0)
		return ret;
	
	ret=ex_module_sendmsg(sub_proc,recv_msg);
	return ret;
}
