enum dtype_user_define {
	TYPE(USER_DEFINE)=0x3200,
};
enum subtype_user_define {
	SUBTYPE(USER_DEFINE,LOGIN)=0x1,
	SUBTYPE(USER_DEFINE,CLIENT_STATE),
	SUBTYPE(USER_DEFINE,SERVER_STATE),
	SUBTYPE(USER_DEFINE,RETURN)
};

enum enum_role_type
{
	Student=0x01,
	Teacher,
	Director
};

enum enum_user_state
{
	WAIT=0x01,
	LOGIN,
	ERROR
};

enum enum_login_state
{
	SUCCEED=0x01,
	INVALID,
	NOUSER,
	AUTHFAIL,
	NOACCESS
};

typedef struct user_define_login{
	char * user_name;
	char passwd[32];
	char proc_name[32];
	BYTE machine_uuid[32];
}__attribute__((packed)) RECORD(USER_DEFINE,LOGIN);

typedef struct user_define_client_state{
	char * user_name;
	UINT32 curr_state;
	char * user_info;
}__attribute__((packed)) RECORD(USER_DEFINE,CLIENT_STATE);

typedef struct user_define_server_state{
	char * user_name;
	BYTE node_uuid[32];
	char proc_name[32];
	char * passwd;
	enum enum_role_type role;
	enum enum_login_state curr_state;
}__attribute__((packed)) RECORD(USER_DEFINE,SERVER_STATE);

typedef struct user_define_return{
	UINT32 return_code;
	char * return_info;
}__attribute__((packed)) RECORD(USER_DEFINE,RETURN);
