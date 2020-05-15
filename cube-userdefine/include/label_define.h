enum dtype_label_define {
	TYPE(LABEL_DEFINE)=0x3220
};
enum subtype_label_define {
	SUBTYPE(LABEL_DEFINE,ADDR)=0x1,
	SUBTYPE(LABEL_DEFINE,USER)
};

typedef struct label_define_addr{
	BYTE node_uuid[DIGEST_SIZE];
	char proc_name[DIGEST_SIZE];
	char * user_name;
	enum enum_role_type role;
}__attribute__((packed)) RECORD(LABEL_DEFINE,ADDR);

typedef struct label_define_user{
	char * user_name;
	enum enum_role_type role;
}__attribute__((packed)) RECORD(LABEL_DEFINE,USER);
