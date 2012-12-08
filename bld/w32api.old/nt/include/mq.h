#ifndef _MQ_H
#define _MQ_H
#if __GNUC__ >= 3
#pragma GCC system_header
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PROPID_Q_INSTANCE 101
#define PROPID_Q_TYPE 102
#define PROPID_Q_PATHNAME 103
#define PROPID_Q_JOURNAL 104
#define PROPID_Q_QUOTA 105
#define PROPID_Q_BASEPRIORITY 106
#define PROPID_Q_JOURNAL_QUOTA 107
#define PROPID_Q_LABEL 108
#define PROPID_Q_CREATE_TIME 109
#define PROPID_Q_MODIFY_TIME 110
#define PROPID_Q_AUTHENTICATE 111
#define PROPID_Q_PRIV_LEVEL 112
#define PROPID_Q_TRANSACTION 113
#define PROPID_Q_PATHNAME_DNS 124
#define PROPID_Q_MULTICAST_ADDRESS 125
#define PROPID_Q_ADS_PATH 126
#define MQ_TRANSACTIONAL_NONE 0
#define MQ_TRANSACTIONAL 1
#define MQ_AUTHENTICATE_NONE 0
#define MQ_AUTHENTICATE 1
#define MQ_PRIV_LEVEL_NONE 0
#define MQ_PRIV_LEVEL_OPTIONAL 1
#define MQ_PRIV_LEVEL_BODY 2
/*#define PROPID_M_ABORT_COUNT*/
#define PROPID_M_CLASS 1
#define PROPID_M_MSGID 2
#define PROPID_M_CORRELATIONID 3
#define PROPID_M_PRIORITY 4
#define PROPID_M_DELIVERY 5
#define PROPID_M_ACKNOWLEDGE 6
#define PROPID_M_JOURNAL 7
#define PROPID_M_APPSPECIFIC 8
#define PROPID_M_BODY 9
#define PROPID_M_BODY_SIZE 10
#define PROPID_M_LABEL 11
#define PROPID_M_LABEL_LEN 12
#define PROPID_M_TIME_TO_REACH_QUEUE 13
#define PROPID_M_TIME_TO_BE_RECEIVED 14
#define PROPID_M_RESP_QUEUE 15
#define PROPID_M_RESP_QUEUE_LEN 16
#define PROPID_M_ADMIN_QUEUE 17
#define PROPID_M_ADMIN_QUEUE_LEN 18
#define PROPID_M_VERSION 19
#define PROPID_M_SENDERID 20
#define PROPID_M_SENDERID_LEN 21
#define PROPID_M_SENDERID_TYPE 22
#define PROPID_M_PRIV_LEVEL 23
#define PROPID_M_AUTH_LEVEL 24
#define PROPID_M_AUTHENTICATED 25
#define PROPID_M_HASH_ALG 26
#define PROPID_M_ENCRYPTION_ALG 27
#define PROPID_M_SENDER_CERT 28
#define PROPID_M_SENDER_CERT_LEN 29
#define PROPID_M_SRC_MACHINE_ID 30
#define PROPID_M_SENTTIME 31
#define PROPID_M_ARRIVEDTIME 32
#define PROPID_M_DEST_QUEUE 33
#define PROPID_M_DEST_QUEUE_LEN 34
#define PROPID_M_EXTENSION 35
#define PROPID_M_EXTENSION_LEN 36
#define PROPID_M_SECURITY_CONTEXT 37
#define PROPID_M_CONNECTOR_TYPE 38
#define PROPID_M_XACT_STATUS_QUEUE 39
#define PROPID_M_XACT_STATUS_QUEUE_LEN 40
#define PROPID_M_TRACE 41
#define PROPID_M_BODY_TYPE 42
#define PROPID_M_DEST_SYMM_KEY 43
#define PROPID_M_DEST_SYMM_KEY_LEN 44
#define PROPID_M_SIGNATURE 45
#define PROPID_M_SIGNATURE_LEN 46
#define PROPID_M_PROV_TYPE 47
#define PROPID_M_PROV_NAME 48
#define PROPID_M_PROV_NAME_LEN 49
#define PROPID_M_FIRST_IN_XACT 50
#define PROPID_M_LAST_IN_XACT 51
#define PROPID_M_XACTID 52
#define PROPID_M_AUTHENTICATED_EX 53
#if (_WIN32_WINNT >= 0x0501)
#define PROPID_M_RESP_FORMAT_NAME 54
#define PROPID_M_RESP_FORMAT_NAME_LEN 55
#define PROPID_M_DEST_FORMAT_NAME 58
#define PROPID_M_DEST_FORMAT_NAME_LEN 59
#define PROPID_M_LOOKUPID 60
#define PROPID_M_SOAP_ENVELOPE 61
#define PROPID_M_SOAP_ENVELOPE_LEN 62
#define PROPID_M_COMPOUND_MESSAGE 63
#define PROPID_M_COMPOUND_MESSAGE_SIZE 64
#define PROPID_M_SOAP_HEADER 65
#define PROPID_M_SOAP_BODY 66
#endif
/*#define PROPID_M_TOTAL_ABORT_COUNT*/
#define PROPID_M_CORRELATIONID_SIZE 20
#define PROPID_M_MSGID_SIZE 20
#define PROPID_M_XACTID_SIZE 20
#define MQMSG_DELIVERY_EXPRESS 0
#define MQMSG_DELIVERY_RECOVERABLE 1
#define MQMSG_JOURNAL_NONE 0
#define MQMSG_DEADLETTER 1
#define MQMSG_JOURNAL 2
#define MQMSG_TRACE_NONE 0
#define MQMSG_SEND_ROUTE_TO_REPORT_QUEUE 1
#define MQMSG_SENDERID_TYPE_NONE 0
#define MQMSG_SENDERID_TYPE_SID 1
#define MQMSG_NOT_FIRST_IN_XACT 0
#define MQMSG_FIRST_IN_XACT 1
#define MQMSG_NOT_LAST_IN_XACT 0
#define MQMSG_LAST_IN_XACT 1
#define MQMSG_PRIV_LEVEL_NONE 0
/*#define MQMSG_PRIV_LEVEL_BODY*/
#define MQMSG_PRIV_LEVEL_BODY_BASE 1
#define MQMSG_PRIV_LEVEL_BODY_ENHANCED 3
#define MQMSG_AUTHENTICATION_NOT_REQUESTED 0
#define MQMSG_AUTHENTICATION_REQUESTED 1
#define MQMSG_AUTHENTICATION_REQUESTED_EX 3
#define MQMSG_AUTH_LEVEL_NONE 0
#define MQMSG_AUTH_LEVEL_ALWAYS 1
#define MQMSG_AUTH_LEVEL_MSMQ10 2
#define MQMSG_AUTH_LEVEL_MSMQ20 4
#define MQMSG_AUTH_LEVEL_SIG10 2
#define MQMSG_AUTH_LEVEL_SIG20 4
#define MQMSG_AUTH_LEVEL_SIG30 8
#define MQMSG_AUTHENTICATED_SIG10 1
#define MQMSG_AUTHENTICATED_SIG20 3
#define MQMSG_AUTHENTICATED_SIG30 5
#define MQMSG_AUTHENTICATED_SIGXML 9
#define PROPID_MGMT_MSMQ_ACTIVEQUEUES 1
#define PROPID_MGMT_MSMQ_PRIVATEQ 2
#define PROPID_MGMT_MSMQ_DSSERVER 3
#define PROPID_MGMT_MSMQ_CONNECTED 4
#define PROPID_MGMT_MSMQ_TYPE 5
#define PROPID_MGMT_MSMQ_BYTES_IN_ALL_QUEUES 6
#define PROPID_MGMT_QUEUE_PATHNAME 1
#define PROPID_MGMT_QUEUE_FORMATNAME 2
#define PROPID_MGMT_QUEUE_TYPE 3
#define PROPID_MGMT_QUEUE_LOCATION 4
#define PROPID_MGMT_QUEUE_XACT 5
#define PROPID_MGMT_QUEUE_FOREIGN 6
#define PROPID_MGMT_QUEUE_MESSAGE_COUNT 7
#define PROPID_MGMT_QUEUE_BYTES_IN_QUEUE 8
#define PROPID_MGMT_QUEUE_JOURNAL_MESSAGE_COUNT 9
#define PROPID_MGMT_QUEUE_BYTES_IN_JOURNAL 10
#define PROPID_MGMT_QUEUE_STATE 11
#define PROPID_MGMT_QUEUE_NEXTHOPS 12
#define PROPID_MGMT_QUEUE_EOD_LAST_ACK 13
#define PROPID_MGMT_QUEUE_EOD_LAST_ACK_TIME 14
#define PROPID_MGMT_QUEUE_EOD_LAST_ACK_COUNT 15
#define PROPID_MGMT_QUEUE_EOD_FIRST_NON_ACK 16
#define PROPID_MGMT_QUEUE_EOD_LAST_NON_ACK 17
#define PROPID_MGMT_QUEUE_EOD_NEXT_SEQ 18
#define PROPID_MGMT_QUEUE_EOD_NO_READ_COUNT 19
#define PROPID_MGMT_QUEUE_EOD_NO_ACK_COUNT 20
#define PROPID_MGMT_QUEUE_EOD_RESEND_TIME 21
#define PROPID_MGMT_QUEUE_EOD_RESEND_INTERVAL 22
#define PROPID_MGMT_QUEUE_EOD_RESEND_COUNT 23
#define PROPID_MGMT_QUEUE_EOD_SOURCE_INFO 24
/*#define PROPID_MGMT_QUEUE_SUBQUEUE_COUNT*/
/*#define PROPID_MGMT_QUEUE_SUBQUEUE_NAMES*/
#define PROPID_PC_VERSION 5801
#define PROPID_PC_DS_ENABLED 5802
#define PROPID_QM_SITE_ID 201
#define PROPID_QM_MACHINE_ID 202
#define PROPID_QM_PATHNAME 203
#define PROPID_QM_CONNECTION 204
#define PROPID_QM_ENCRYPTION_PK 205
#define PROPID_QM_ENCRYPTION_PK_BASE 231
#define PROPID_QM_ENCRYPTION_PK_ENHANCED 232
#define PROPID_QM_PATHNAME_DNS 233
#define MQ_OK 0x00000000
#define MQ_INFORMATION_PROPERTY 0x400E0001
#define MQ_INFORMATION_ILLEGAL_PROPERTY 0x400E0002
#define MQ_INFORMATION_PROPERTY_IGNORED 0x400E0003
#define MQ_INFORMATION_UNSUPPORTED_PROPERTY 0x400E0004
#define MQ_INFORMATION_DUPLICATE_PROPERTY 0x400E0005
#define MQ_INFORMATION_OPERATION_PENDING 0x400E0006
#define MQ_INFORMATION_FORMATNAME_BUFFER_TOO_SMALL 0x400E0009
#define MQ_INFORMATION_INTERNAL_USER_CERT_EXIST 0x400E000A
#define MQ_INFORMATION_OWNER_IGNORED 0x400E000B
#define MQ_ERROR 0xC00E0001
#define MQ_ERROR_PROPERTY 0xC00E0002
#define MQ_ERROR_QUEUE_NOT_FOUND 0xC00E0003
#define MQ_ERROR_QUEUE_NOT_ACTIVE 0xC00E0004
#define MQ_ERROR_QUEUE_EXISTS 0xC00E0005
#define MQ_ERROR_INVALID_PARAMETER 0xC00E0006
#define MQ_ERROR_INVALID_HANDLE 0xC00E0007
#define MQ_ERROR_OPERATION_CANCELLED0xC00E0008
#define MQ_ERROR_SHARING_VIOLATION 0xC00E0009
#define MQ_ERROR_SERVICE_NOT_AVAILABLE 0xC00E000B
#define MQ_ERROR_MACHINE_NOT_FOUND 0xC00E000D
#define MQ_ERROR_ILLEGAL_SORT 0xC00E0010
#define MQ_ERROR_ILLEGAL_USER 0xC00E0011
#define MQ_ERROR_NO_DS 0xC00E0013
#define MQ_ERROR_ILLEGAL_QUEUE_PATHNAME 0xC00E0014
#define MQ_ERROR_ILLEGAL_PROPERTY_VALUE 0xC00E0018
#define MQ_ERROR_ILLEGAL_PROPERTY_VT 0xC00E0019
#define MQ_ERROR_BUFFER_OVERFLOW 0xC00E001A
#define MQ_ERROR_IO_TIMEOUT 0xC00E001B
#define MQ_ERROR_ILLEGAL_CURSOR_ACTION 0xC00E001C
#define MQ_ERROR_MESSAGE_ALREADY_RECEIVED 0xC00E001D
#define MQ_ERROR_ILLEGAL_FORMATNAME 0xC00E001E
#define MQ_ERROR_FORMATNAME_BUFFER_TOO_SMALL 0xC00E001F
#define MQ_ERROR_UNSUPPORTED_FORMATNAME_OPERATION 0xC00E0020
#define MQ_ERROR_ILLEGAL_SECURITY_DESCRIPTOR 0xC00E0021
#define MQ_ERROR_SENDERID_BUFFER_TOO_SMALL 0xC00E0022
#define MQ_ERROR_SECURITY_DESCRIPTOR_TOO_SMALL 0xC00E0023
#define MQ_ERROR_CANNOT_IMPERSONATE_CLIENT 0xC00E0024
#define MQ_ERROR_ACCESS_DENIED 0xC00E0025
#define MQ_ERROR_PRIVILEGE_NOT_HELD 0xC00E0026
#define MQ_ERROR_INSUFFICIENT_RESOURCES 0xC00E0027
#define MQ_ERROR_USER_BUFFER_TOO_SMALL 0xC00E0028
#define MQ_ERROR_MESSAGE_STORAGE_FAILED 0xC00E002A
#define MQ_ERROR_SENDER_CERT_BUFFER_TOO_SMALL 0xC00E002B
#define MQ_ERROR_INVALID_CERTIFICATE 0xC00E002C
#define MQ_ERROR_CORRUPTED_INTERNAL_CERTIFICATE 0xC00E002D
#define MQ_ERROR_INTERNAL_USER_CERT_EXIST 0xC00E002E
#define MQ_ERROR_NO_INTERNAL_USER_CERT 0xC00E002F
#define MQ_ERROR_CORRUPTED_SECURITY_DATA 0xC00E0030
#define MQ_ERROR_CORRUPTED_PERSONAL_CERT_STORE 0xC00E0031
#define MQ_ERROR_COMPUTER_DOES_NOT_SUPPORT_ENCRYPTION 0xC00E0033
#define MQ_ERROR_BAD_SECURITY_CONTEXT 0xC00E0035
#define MQ_ERROR_COULD_NOT_GET_USER_SID 0xC00E0036
#define MQ_ERROR_COULD_NOT_GET_ACCOUNT_INFO 0xC00E0037
#define MQ_ERROR_ILLEGAL_MQCOLUMNS 0xC00E0038
#define MQ_ERROR_ILLEGAL_PROPID 0xC00E0039
#define MQ_ERROR_ILLEGAL_RELATION 0xC00E003A
#define MQ_ERROR_ILLEGAL_PROPERTY_SIZE 0xC00E003B
#define MQ_ERROR_ILLEGAL_RESTRICTION_PROPID 0xC00E003C
#define MQ_ERROR_ILLEGAL_MQQUEUEPROPS 0xC00E003D
#define MQ_ERROR_PROPERTY_NOTALLOWED 0xC00E003E
#define MQ_ERROR_INSUFFICIENT_PROPERTIES 0xC00E003F
#define MQ_ERROR_MACHINE_EXISTS 0xC00E0040
#define MQ_ERROR_ILLEGAL_MQQMPROPS 0xC00E0041
#define MQ_ERROR_DS_IS_FULL 0xC00E0042
#define MQ_ERROR_DS_ERROR 0xC00E0043
#define MQ_ERROR_INVALID_OWNER 0xC00E0044
#define MQ_ERROR_UNSUPPORTED_ACCESS_MODE 0xC00E0045
#define MQ_ERROR_RESULT_BUFFER_TOO_SMALL 0xC00E0046
#define MQ_ERROR_DELETE_CN_IN_USE 0xC00E0048
#define MQ_ERROR_NO_RESPONSE_FROM_OBJECT_SERVER 0xC00E0049
#define MQ_ERROR_OBJECT_SERVER_NOT_AVAILABLE 0xC00E004A
#define MQ_ERROR_QUEUE_NOT_AVAILABLE 0xC00E004B
#define MQ_ERROR_DTC_CONNECT 0xC00E004C
#define MQ_ERROR_TRANSACTION_IMPORT 0xC00E004E
#define MQ_ERROR_TRANSACTION_USAGE 0xC00E0050
#define MQ_ERROR_TRANSACTION_SEQUENCE 0xC00E0051
#define MQ_ERROR_MISSING_CONNECTOR_TYPE 0xC00E0055
#define MQ_ERROR_STALE_HANDLE 0xC00E0056
#define MQ_ERROR_TRANSACTION_ENLIST 0xC00E0058
#define MQ_ERROR_QUEUE_DELETED 0xC00E005A
#define MQ_ERROR_ILLEGAL_CONTEXT 0xC00E005B
#define MQ_ERROR_ILLEGAL_SORT_PROPID 0xC00E005C
#define MQ_ERROR_LABEL_TOO_LONG 0xC00E005D
#define MQ_ERROR_LABEL_BUFFER_TOO_SMALL 0xC00E005E
#define MQ_ERROR_MQIS_SERVER_EMPTY 0xC00E005F
#define MQ_ERROR_MQIS_READONLY_MODE 0xC00E0060
#define MQ_ERROR_SYMM_KEY_BUFFER_TOO_SMALL 0xC00E0061
#define MQ_ERROR_SIGNATURE_BUFFER_TOO_SMALL 0xC00E0062
#define MQ_ERROR_PROV_NAME_BUFFER_TOO_SMALL 0xC00E0063
#define MQ_ERROR_ILLEGAL_OPERATION 0xC00E0064
#define MQ_ERROR_WRITE_NOT_ALLOWED 0xC00E0065
#define MQ_ERROR_WKS_CANT_SERVE_CLIENT 0xC00E0066
#define MQ_ERROR_DEPEND_WKS_LICENSE_OVERFLOW 0xC00E0067
#define MQ_CORRUPTED_QUEUE_WAS_DELETED 0xC00E0068
#define MQ_ERROR_REMOTE_MACHINE_NOT_AVAILABLE 0xC00E0069
#define MQ_ERROR_UNSUPPORTED_OPERATION 0xC00E006A
#define MQ_ERROR_ENCRYPTION_PROVIDER_NOT_SUPPORTED 0xC00E006B
#define MQ_ERROR_CANNOT_SET_CRYPTO_SEC_DESCR 0xC00E006C
#define MQ_ERROR_CERTIFICATE_NOT_PROVIDED 0xC00E006D
#define MQ_ERROR_Q_DNS_PROPERTY_NOT_SUPPORTED 0xC00E006E
#define MQ_ERROR_CANNOT_CREATE_CERT_STORE 0xC00E006F
#define MQ_ERROR_CANNOT_OPEN_CERT_STORE 0xC00E0070
#define MQ_ERROR_ILLEGAL_ENTERPRISE_OPERATION 0xC00E0071
#define MQ_ERROR_CANNOT_GRANT_ADD_GUID 0xC00E0072
#define MQ_ERROR_CANNOT_LOAD_MSMQOCM 0xC00E0073
#define MQ_ERROR_NO_ENTRY_POINT_MSMQOCM 0xC00E0074
#define MQ_ERROR_NO_MSMQ_SERVERS_ON_DC 0xC00E0075
#define MQ_ERROR_CANNOT_JOIN_DOMAIN 0xC00E0076
#define MQ_ERROR_CANNOT_CREATE_ON_GC 0xC00E0077
#define MQ_ERROR_GUID_NOT_MATCHING 0xC00E0078
#define MQ_ERROR_PUBLIC_KEY_NOT_FOUND 0xC00E0079
#define MQ_ERROR_PUBLIC_KEY_DOES_NOT_EXIST 0xC00E007A
#define MQ_ERROR_ILLEGAL_MQPRIVATEPROPS 0xC00E007B
#define MQ_ERROR_NO_GC_IN_DOMAIN 0xC00E007C
#define MQ_ERROR_NO_MSMQ_SERVERS_ON_GC 0xC00E007D
#define MQ_ERROR_CANNOT_GET_DN 0xC00E007E
#define MQ_ERROR_CANNOT_HASH_DATA_EX 0xC00E007F
#define MQ_ERROR_CANNOT_SIGN_DATA_EX 0xC00E0080
#define MQ_ERROR_CANNOT_CREATE_HASH_EX 0xC00E0081
#define MQ_ERROR_FAIL_VERIFY_SIGNATURE_EX 0xC00E0082
#define MQ_ERROR_CANNOT_DELETE_PSC_OBJECTS 0xC00E0083
#define MQ_ERROR_NO_MQUSER_OU 0xC00E0084
#define MQ_ERROR_CANNOT_LOAD_MQAD 0xC00E0085
#define MQ_ERROR_CANNOT_LOAD_MQDSSRV 0xC00E0086
#define MQ_ERROR_PROPERTIES_CONFLICT 0xC00E0087
#define MQ_ERROR_MESSAGE_NOT_FOUND 0xC00E0088
#define MQ_ERROR_CANT_RESOLVE_SITES 0xC00E0089
#define MQ_ERROR_NOT_SUPPORTED_BY_DEPENDENT_CLIENTS 0xC00E008A
#define MQ_ERROR_OPERATION_NOT_SUPPORTED_BY_REMOTE_COMPUTER 0xC00E008B
#define MQ_ERROR_NOT_A_CORRECT_OBJECT_CLASS 0xC00E008C
#define MQ_ERROR_MULTI_SORT_KEYS 0xC00E008D
#define MQ_ERROR_GC_NEEDED 0xC00E008E
#define MQ_ERROR_DS_BIND_ROOT_FOREST 0xC00E008F
#define MQ_ERROR_DS_LOCAL_USER 0xC00E0090
#define MQ_ERROR_Q_ADS_PROPERTY_NOT_SUPPORTED 0xC00E0091
#define MQ_ERROR_BAD_XML_FORMAT 0xC00E0092
#define MQ_ERROR_UNSUPPORTED_CLASS xC00E0093
#define MQ_ERROR_UNINITIALIZED_OBJECT 0xC00E0094
#define MQ_ERROR_CANNOT_CREATE_PSC_OBJECTS 0xC00E0095
#define MQ_ERROR_CANNOT_UPDATE_PSC_OBJECTS 0xC00E0096
#define MQSEC_DELETE_MESSAGE 0x00000001
#define MQSEC_PEEK_MESSAGE 0x00000002
#define MQSEC_WRITE_MESSAGE 0x00000004
#define MQSEC_DELETE_JOURNAL_MESSAGE 0x00000008
#define MQSEC_SET_QUEUE_PROPERTIES 0x00000010
#define MQSEC_GET_QUEUE_PROPERTIES 0x00000020
#define MQSEC_DELETE_QUEUE 0x00010000
#define MQSEC_GET_QUEUE_PERMISSIONS 0x00020000
#define MQSEC_CHANGE_QUEUE_PERMISSIONS 0x00040000
#define MQSEC_TAKE_QUEUE_OWNERSHIP 0x00080000
#define MQSEC_RECEIVE_MESSAGE (MQSEC_DELETE_MESSAGE|MQSEC_PEEK_MESSAGE)
#define MQSEC_RECEIVE_JOURNAL_MESSAGE (MQSEC_DELETE_JOURNAL_MESSAGE|MQSEC_PEEK_MESSAGE)
#define MQSEC_QUEUE_GENERIC_READ (MQSEC_GET_QUEUE_PROPERTIES|MQSEC_GET_QUEUE_PERMISSIONS|MQSEC_RECEIVE_MESSAGE|MQSEC_RECEIVE_JOURNAL_MESSAGE)
#define MQSEC_QUEUE_GENERIC_WRITE (MQSEC_GET_QUEUE_PROPERTIES|MQSEC_GET_QUEUE_PERMISSIONS|MQSEC_WRITE_MESSAGE)
#define MQSEC_QUEUE_GENERIC_ALL (MQSEC_RECEIVE_MESSAGE|MQSEC_RECEIVE_JOURNAL_MESSAGE|MQSEC_WRITE_MESSAGE|MQSEC_SET_QUEUE_PROPERTIES|MQSEC_GET_QUEUE_PROPERTIES|MQSEC_DELETE_QUEUE|MQSEC_GET_QUEUE_PERMISSIONS|MQSEC_CHANGE_QUEUE_PERMISSIONS|MQSEC_TAKE_QUEUE_OWNERSHIP)
#define MSMQ_CONNECTED L"CONNECTED"
#define MSMQ_DISCONNECTED L"DISCONNECTED"
#define MQ_RECEIVE_ACCESS 0x00000001
#define MQ_SEND_ACCESS 0x00000002
#define MQ_PEEK_ACCESS 0x00000020
#define MQ_ADMIN_ACCESS 0x00000080
#define MQ_DENY_NONE 0x00000000
#define MQ_DENY_RECEIVE_SHARE 0x00000001
#define MQ_ACTION_RECEIVE 0x00000000
#define MQ_ACTION_PEEK_CURRENT 0x80000000
#define MQ_ACTION_PEEK_NEXT 0x80000001
/*#define MQ_NO_TRANSACTION*/
/*#define MQ_MTS_TRANSACTION*/
/*#define MQ_SINGLE_MESSAGE*/
/*#define MQ_XA_TRANSACTION*/
#if (_WIN32_WINNT >= 0x0501)
#define MQ_LOOKUP_PEEK_CURRENT 0x40000010
#define MQ_LOOKUP_PEEK_NEXT 0x40000011
#define MQ_LOOKUP_PEEK_PREV 0x40000012
#define MQ_LOOKUP_PEEK_FIRST 0x40000014
#define MQ_LOOKUP_PEEK_LAST 0x40000018
#define MQ_LOOKUP_RECEIVE_CURRENT 0x40000020
#define MQ_LOOKUP_RECEIVE_NEXT 0x40000021
#define MQ_LOOKUP_RECEIVE_PREV 0x40000022
#define MQ_LOOKUP_RECEIVE_FIRST 0x40000024
#define MQ_LOOKUP_RECEIVE_LAST 0x40000028
#endif
#if (_WIN32_WINNT >= 0x0500)
#define MQCERT_REGISTER_ALWAYS 0x00000001
#define MQCERT_REGISTER_IF_NOT_EXIST 0x00000002
#endif
#define PRLT 0
#define PRLE 1
#define PRGT 2
#define PRGE 3
#define PREQ 4
#define PRNE 5
#define MQ_MIN_PRIORITY 0
#define MQ_MAX_PRIORITY 7
#define MQ_DEFAULT_PRIORITY 3
#define QUERY_SORTASCEND 0
#define QUERY_SORTDESCEND 1

typedef struct tagMQCOLUMNSET {
	ULONG cCol;
	PROPID *aCol;
} MQCOLUMNSET;
typedef PROPID MGMTPROPID;
typedef struct tagMQMGMTPROPS {
	DWORD cProp;
	MGMTPROPID *aPropID;
	MQPROPVARIANT *aPropVar;
	HRESULT *aStatus;
} MQMGMTPROPS;
typedef PROPID MSGPROPID;
typedef struct tagMQMSGPROPS {
	DWORD cProp;
	MSGPROPID *aPropID;
	MQPROPVARIANT *aPropVar;
	HRESULT *aStatus;
} MQMSGPROPS;
typedef PROPID QMPROPID;
typedef struct tagMQPRIVATEPROPS {
	DWORD cProp;
	QMPROPID *aPropID;
	MQPROPVARIANT *aPropVar;
	HRESULT *aStatus;
} MQPRIVATEPROPS;
typedef struct tagMQPROPERTYRESTRICTION {
	ULONG rel;
	PROPID prop;
	MQPROPVARIANT prval;
} MQPROPERTYRESTRICTION;
typedef struct tagMQQMPROPS {
	DWORD cProp;
	QMPROPID *aPropID;
	MQPROPVARIANT *aPropVar;
	HRESULT *aStatus;
} MQQMPROPS;
typedef PROPID QUEUEPROPID;
typedef struct tagMQQUEUEPROPS {
	DWORD cProp;
	QUEUEPROPID *aPropID;
	MQPROPVARIANT *aPropVar;
	HRESULT *aStatus;
} MQQUEUEPROPS;
typedef struct tagMQRESTRICTION {
	ULONG cRes;
	MQPROPERTYRESTRICTION *paPropRes;
} MQRESTRICTION;
typedef struct tagMQSORTKEY {
	PROPID propColumn;
	ULONG dwOrder;
} MQSORTKEY;
typedef struct tagMQSORTSET {
	ULONG cCol;
	MQSORTKEY *aCol;
} MQSORTSET;
typedef struct tagMQPROPVARIANT {
	VARTYPE vt;
	WORD wReserved1;
	WORD wReserved2;
	WORD wReserved3;
	union {
		UCHAR bVal;
		SHORT iVal;
		USHORT uiVal;
		VARIANT_BOOL boolVal;
		LONG lVal;
		ULONG ulVal;
		ULARGE_INTEGER uhVal;
		SCODE scode;
		DATE date;
		CLSID _RPC_FAR *puuid;
		BLOB blob;
		LPOLESTR bstrVal;
		LPSTR pszVal;
		LPWSTR pwszVal;
		CAUB caub;
		CAI cai;
		CAUI caui;
		CABOOL cabool;
		CAL cal;
		CAUL caul;
		CACLSID cauuid;
		CABSTR cabstr;
		CALPWSTR calpwstr;
		CAPROPVARIANT capropvar;
	};                      
} MQPROPVARIANT;
typedef struct tagSEQUENCE_INFO {
	LONGLONG SeqID;
	ULONG SeqNo; 
	ULONG PrevNo;
} SEQUENCE_INFO;
typedef HANDLE QUEUEHANDLE;
typedef VOID (*PMQRECEIVECALLBACK)(VOID);

#if (_WIN32_WINNT >= 0x0501)
HRESULT WINAPI MQADsPathToFormatName(LPCWSTR,LPWSTR,LPDWORD);
#endif
HRESULT WINAPI MQBeginTransaction(ITransaction **);
HRESULT WINAPI MQCloseCursor(HANDLE);
HRESULT WINAPI MQCloseQueue(QUEUEHANDLE);
HRESULT WINAPI MQCreateCursor(QUEUEHANDLE,PHANDLE);
HRESULT WINAPI MQCreateQueue(PSECURITY_DESCRIPTOR,MQQUEUEPROPS *,LPWSTR,LPDWORD);
HRESULT WINAPI MQDeleteQueue(LPCWSTR);
VOID WINAPI MQFreeMemory(PVOID);
VOID WINAPI MQFreeSecurityContext(HANDLE);
HRESULT WINAPI MQGetMachineProperties(LPCWSTR,const GUID *,MQQMPROPS *);
#if (_WIN32_WINNT >= 0x0500)
HRESULT WINAPI MQGetOverlappedResult(LPOVERLAPPED);
HRESULT WINAPI MQGetPrivateComputerInformation(LPCWSTR,MQPRIVATEPROPS *);
#endif
HRESULT WINAPI MQGetQueueProperties(LPCWSTR,MQQUEUEPROPS *);
HRESULT WINAPI MQGetQueueSecurity(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
HRESULT WINAPI MQGetSecurityContext(LPVOID,DWORD,HANDLE *);
HRESULT WINAPI MQGetMachineProperties(LPCWSTR,const GUID *,MQQMPROPS *);
HRESULT WINAPI MQGetOverlappedResult(LPOVERLAPPED);
HRESULT WINAPI MQGetPrivateComputerInformation(LPCWSTR,MQPRIVATEPROPS *);
HRESULT WINAPI MQGetQueueProperties(LPCWSTR,MQQUEUEPROPS *);
HRESULT WINAPI MQGetQueueSecurity(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR,DWORD,LPDWORD);
HRESULT WINAPI MQGetSecurityContext(LPVOID,DWORD,HANDLE *);
#if (_WIN32_WINNT >= 0x0500)
HRESULT WINAPI MQGetSecurityContextEx(LPVOID,DWORD,HANDLE *);
#endif
HRESULT WINAPI MQHandleToFormatName(QUEUEHANDLE,LPWSTR,LPDWORD);
HRESULT WINAPI MQInstanceToFormatName(GUID *,LPWSTR,LPDWORD);
HRESULT WINAPI MQLocateBegin(LPCWSTR,MQRESTRICTION *,MQCOLUMNSET *,MQSORTSET *,PHANDLE);
HRESULT WINAPI MQLocateEnd(HANDLE);
HRESULT WINAPI MQLocateNext(HANDLE,DWORD *,MQPROPVARIANT[]);
#if (_WIN32_WINNT >= 0x0600)
HRESULT WINAPI MQMarkMessageRejected(QUEUEHANDLE,ULONGLONG);
#endif
#if (_WIN32_WINNT >= 0x0501)
HRESULT WINAPI MQMgmtAction(LPCWSTR,LPCWSTR,LPCWSTR);
HRESULT WINAPI MQMgmtGetInfo(LPCWSTR,LPCWSTR,MQMGMTPROPS *);
#endif
#if (_WIN32_WINNT >= 0x0600)
HRESULT WINAPI MQMoveMessage(QUEUEHANDLE,QUEUEHANDLE,ULONGLONG,ITransaction *);
#endif
HRESULT WINAPI MQOpenQueue(LPCWSTR,DWORD,DWORD,QUEUEHANDLE *);
HRESULT WINAPI MQPathNameToFormatName(LPCWSTR,LPWSTR,LPDWORD);
#if (_WIN32_WINNT >= 0x0501)
HRESULT WINAPI MQPurgeQueue(QUEUEHANDLE);
#endif
HRESULT WINAPI MQReceiveMessage(QUEUEHANDLE,DWORD,DWORD,MQMSGPROPS *,LPOVERLAPPED,PMQRECEIVECALLBACK,HANDLE,ITransaction *);
#if (_WIN32_WINNT >= 0x0501)
HRESULT WINAPI MQReceiveMessageByLookupId(QUEUEHANDLE,ULONGLONG,DWORD,MQMSGPROPS *,LPOVERLAPPED,PMQRECEIVECALLBACK,ITransaction *);
#endif
#if (_WIN32_WINNT >= 0x0500)
HRESULT WINAPI MQRegisterCertificate(DWORD,PVOID,DWORD);
#endif
HRESULT WINAPI MQSendMessage(QUEUEHANDLE,MQMSGPROPS *,ITransaction *);
HRESULT WINAPI MQSetQueueProperties(LPCWSTR,MQQUEUEPROPS *);
HRESULT WINAPI MQSetQueueSecurity(LPCWSTR,SECURITY_INFORMATION,PSECURITY_DESCRIPTOR);

#ifdef __cplusplus
}
#endif
#endif
#if !defined(_ENABLE_AUTODEPEND)
  #pragma read_only_file;
#endif
