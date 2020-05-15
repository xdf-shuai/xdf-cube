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
int record_acl_init(void * sub_proc, void * para)
{
	int ret;
	// add yorself's module init func here
	return 0;
}
int record_acl_start(void * sub_proc, void * para)
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
			ret=proc_access_write(sub_proc,recv_msg);
		}
		else if((type==TYPE(RECORD_DEFINE))&&(subtype==SUBTYPE(RECORD_DEFINE,RECORD)))
		{
			ret=proc_access_read(sub_proc,recv_msg);
		}
	}
	return 0;
}

int proc_access_write(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(RECORD_DEFINE,WRITE) * record_write;
	RECORD(LABEL_DEFINE,USER) * user_label;
	MSG_EXPAND * msg_expand;
	void * new_msg;
	
	ret=message_get_record(recv_msg,&record_write,0);
	if(ret<0)
		return ret;

	ret=message_remove_expand(recv_msg,TYPE_PAIR(LABEL_DEFINE,USER),&msg_expand);
	if(ret<0)
		return ret;
	if(msg_expand==NULL)
	{
		print_cubeerr("can't find user attached!\n");	
		return -EINVAL;
	}
	user_label=msg_expand->expand;
	
	if(user_label->role==Director)
	{
		if(Strncmp(record_write->segment,"Ex_design",DIGEST_SIZE)==0)
		{
			// permission denied
			new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);
			RECORD(USER_DEFINE,RETURN) * err_return = Talloc0(sizeof(*err_return));
			if(err_return==NULL)
				return -ENOMEM;
			err_return->return_info=dup_str("write permission denied!",0);
			err_return->return_code=NOACCESS;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);
			return ret;
		}
		if(Strncmp(record_write->segment,"Ex_paper",DIGEST_SIZE)==0)
		{
			// permission denied
			new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);
			RECORD(USER_DEFINE,RETURN) * err_return = Talloc0(sizeof(*err_return));
			if(err_return==NULL)
				return -ENOMEM;
			err_return->return_info=dup_str("write permission denied!",0);
			err_return->return_code=NOACCESS;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);
			return ret;
		}		
	}
	if(user_label->role==Teacher)
	{
		if(Strncmp(record_write->segment,"Ex_time",DIGEST_SIZE)==0)
		{
			// permission denied
			new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);
			RECORD(USER_DEFINE,RETURN) * err_return = Talloc0(sizeof(*err_return));
			if(err_return==NULL)
				return -ENOMEM;
			err_return->return_info=dup_str("write permission denied!",0);
			err_return->return_code=NOACCESS;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);
			return ret;
		}	
		if(Strncmp(record_write->segment,"Ex_site",DIGEST_SIZE)==0)
		{
			// permission denied
			new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);
			RECORD(USER_DEFINE,RETURN) * err_return = Talloc0(sizeof(*err_return));
			if(err_return==NULL)
				return -ENOMEM;
			err_return->return_info=dup_str("write permission denied!",0);
			err_return->return_code=NOACCESS;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);
			return ret;
		}
	
	}	
	if(user_label->role==Student)
	{
		if(Strncmp(record_write->segment,"Ex_time",DIGEST_SIZE)==0)
		{
			// permission denied
			new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);
			RECORD(USER_DEFINE,RETURN) * err_return = Talloc0(sizeof(*err_return));
			if(err_return==NULL)
				return -ENOMEM;
			err_return->return_info=dup_str("write permission denied!",0);
			err_return->return_code=NOACCESS;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);
			return ret;
		}	
		if(Strncmp(record_write->segment,"Ex_site",DIGEST_SIZE)==0)
		{
			// permission denied
			new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);
			RECORD(USER_DEFINE,RETURN) * err_return = Talloc0(sizeof(*err_return));
			if(err_return==NULL)
				return -ENOMEM;
			err_return->return_info=dup_str("write permission denied!",0);
			err_return->return_code=NOACCESS;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);
			return ret;
		}
		if(Strncmp(record_write->segment,"Ex_design",DIGEST_SIZE)==0)
		{
			// permission denied
			new_msg=message_create(TYPE_PAIR(USER_DEFINE,RETURN),recv_msg);
			RECORD(USER_DEFINE,RETURN) * err_return = Talloc0(sizeof(*err_return));
			if(err_return==NULL)
				return -ENOMEM;
			err_return->return_info=dup_str("write permission denied!",0);
			err_return->return_code=NOACCESS;
			message_add_record(new_msg,err_return);
			ret=ex_module_sendmsg(sub_proc,new_msg);
			return ret;
		}
	}	

	ret=ex_module_sendmsg(sub_proc,recv_msg);
	return ret;
}

int proc_access_read(void * sub_proc,void * recv_msg)
{
	int ret;
	RECORD(RECORD_DEFINE,RECORD) * record_data;
	RECORD(LABEL_DEFINE,USER) * user_label;
	MSG_EXPAND * msg_expand;
	void * new_msg;
	
	ret=message_get_record(recv_msg,&record_data,0);
	if(ret<0)
		return ret;

	ret=message_remove_expand(recv_msg,TYPE_PAIR(LABEL_DEFINE,USER),&msg_expand);
	if(ret<0)
		return ret;
	if(msg_expand==NULL)
	{
		print_cubeerr("can't find user attached!\n");	
		return -EINVAL;
	}
	user_label=msg_expand->expand;
	
	if(user_label->role==Student)
	{
		record_data->Ex_design=NULL;	
		new_msg=message_create(TYPE_PAIR(RECORD_DEFINE,RECORD),recv_msg);
		if(new_msg==NULL)
			return -EINVAL;
		message_add_record(new_msg,record_data);
		ret=ex_module_sendmsg(sub_proc,new_msg);
		return ret;
	}	
	ret=ex_module_sendmsg(sub_proc,recv_msg);
	return ret;
}
