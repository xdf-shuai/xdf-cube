#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>
 
#include "data_type.h"
#include "alloc.h"
#include "memfunc.h"
#include "basefunc.h"
#include "struct_deal.h"
#include "crypto_func.h"
#include "memdb.h"
#include "message.h"
#include "ex_module.h"
#include "sys_func.h"
#include "user_define.h"
#include "record_define.h"
#include "record_server.h"
// add para lib_include
int record_server_init(void * sub_proc, void * para)
{
	int ret;
	// add yorself's module init func here
	return 0;
}
int record_server_start(void * sub_proc, void * para)
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
		if((type==TYPE(RECORD_DEFINE))&&(subtype==SUBTYPE(RECORD_DEFINE,WRITE)))
		{
			ret=proc_record_write(sub_proc,recv_msg);
		}
		if((type==TYPE(RECORD_DEFINE))&&(subtype==SUBTYPE(RECORD_DEFINE,READ)))
		{
			ret=proc_record_read(sub_proc,recv_msg);
		}
	}
	return 0;
}

int proc_record_write(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(RECORD_DEFINE,RECORD) * record_data;
	RECORD(RECORD_DEFINE,WRITE) * write_data;
	RECORD(USER_DEFINE,RETURN) * return_info;
	RECORD(USER_DEFINE,SERVER_STATE) * user_state;

	void * new_msg;
	
	ret=message_get_record(recv_msg,&write_data,0);
	if(ret<0)
		return ret;

	return_info=Talloc0(sizeof(*return_info));
	if(return_info==NULL)
		return -ENOMEM;

// find the user state, 
	DB_RECORD * db_record;

//	find the record

	db_record=memdb_find_first(TYPE_PAIR(RECORD_DEFINE,RECORD),"record_no",write_data->record_no);
	if(db_record==NULL)
	{
		record_data=Talloc0(sizeof(*record_data));
		record_data->record_no = dup_str(write_data->record_no,0);
	}
	else
	{
		record_data=db_record->record;
	}


	void * record_template = memdb_get_template(TYPE_PAIR(RECORD_DEFINE,RECORD));
	if(record_template==NULL)
		return -EINVAL;
	
	ret = struct_write_elem_text(write_data->segment,record_data,write_data->text,record_template);
	if(ret<0)
		return -EINVAL;

	memdb_store(record_data,TYPE_PAIR(RECORD_DEFINE,RECORD),NULL);

	return_info->return_code=SUCCEED;
	return_info->return_info=dup_str("write data succeed!\n",0);

	// send a message store notice
    void * type_msg;
    RECORD(MESSAGE,TYPES) types_pair;

    types_pair.type=TYPE(RECORD_DEFINE);
    types_pair.subtype=SUBTYPE(RECORD_DEFINE,RECORD);

    type_msg=message_create(TYPE_PAIR(MESSAGE,TYPES),NULL);

    message_add_record(type_msg,&types_pair);
    ex_module_sendmsg(sub_proc,type_msg);
	

write_out:
	new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);	
	if(new_msg==NULL)
		return -EINVAL;
	ret=message_add_record(new_msg,return_info);
	if(ret<0)
		return ret;
	
	ret=ex_module_sendmsg(sub_proc,new_msg);
	return ret;
}

int proc_record_read(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(RECORD_DEFINE,RECORD) * record_data;
	RECORD(RECORD_DEFINE,READ) * read_data;
	RECORD(USER_DEFINE,SERVER_STATE) * user_state;
	RECORD(USER_DEFINE,RETURN) * return_info;
	void * new_msg;
	
	ret=message_get_record(recv_msg,&read_data,0);
	if(ret<0)
		return ret;
// find the user state, 
	DB_RECORD * db_record;

	// find the record
	db_record=memdb_find_first(TYPE_PAIR(RECORD_DEFINE,RECORD),"record_no",read_data->record_no);
	if(db_record==NULL)
	{
		record_data=Talloc0(sizeof(*record_data));
		record_data->record_no = dup_str(read_data->record_no,0);
	}
	else
	{
		record_data=db_record->record;
	}


	new_msg=message_create(TYPE_PAIR(RECORD_DEFINE,RECORD),recv_msg);	
	if(new_msg==NULL)
		return -EINVAL;
	ret=message_add_record(new_msg,record_data);
	if(ret<0)
		return ret;
	
	ret=ex_module_sendmsg(sub_proc,new_msg);
	return ret;
}
