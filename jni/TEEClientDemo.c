#include <jni.h>
#include <fcntl.h>
#include <string.h>
#include <android/log.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <android/log.h>
#include "include/tee_client_api.h"

#define __DEBUG__
#define LOG_TAG "uca"
#define ALOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,  __VA_ARGS__)


#define ERROR 1
#define INIT_CONTEXT_FAILED 0x0100
#define OPEN_SESSION_FAILED 0x0101

#define BIG_INT_ADD_COMMEND 0x0001

#define BIG_INT_LENGTH 256

char hostName[] = "mteetester";
TEEC_UUID uuid_ta = { 0x01020304, 0x0506, 0x0708, { 0x99, 0x0A, 0x0B,	0x0C, 0x0D, 0x0E, 0x0F, 0x10 } };

unsigned char* decodeJavaByteArray(JNIEnv* env, jbyteArray data, int* byteArrayLength)
{
	unsigned char *buffer = NULL;
	int dataLen = 0;
	dataLen = (*env)->GetArrayLength(env, data);
	buffer = (unsigned char*)malloc(dataLen);
	(*env)->GetByteArrayRegion(env, data, 0, dataLen, buffer);
	return buffer;
}

jbyteArray encodeJavaByteArray(JNIEnv* env, unsigned char *data, int dataLength)
{
	jbyteArray resultByteArray = {0};
	resultByteArray = (*env)->NewByteArray(env, dataLength);
	(*env)->SetByteArrayRegion(env, resultByteArray, 0, dataLength, data);
	return resultByteArray;
}

int init(TEEC_Context *context, TEEC_Session *session, int *returnOrigin)
{
	int returnValue = 0;
	TEEC_Result result;

	// initialize context
	result = TEEC_InitializeContext(hostName, context);

	if (result != TEEC_SUCCESS)
	{
		ALOGI("Failed to initialize context");
		returnValue = INIT_CONTEXT_FAILED;
	}

	// open session
	result = TEEC_OpenSession(context, session, &uuid_ta, TEEC_LOGIN_PUBLIC, NULL, NULL, returnOrigin);

	if (result != TEEC_SUCCESS)
	{
		returnValue = ERROR;
	}
}

jbyteArray Java_com_hello_demo_NativeAuthenticatorKernel_bigIntAdd(JNIEnv* env, jobject thiz) {
		jbyteArray return_value = {0};
        TEEC_Context context;
        TEEC_Session session;
        TEEC_Operation operation;
        TEEC_Result result;
        uint32_t returnOrigin;
        int ret;
        int returnValue;
        TEEC_SharedMemory bigInt1;
        TEEC_SharedMemory bigInt2;
        TEEC_SharedMemory bigOut;

    if(NULL == env)
        return NULL;

        ret = init(&context, &session, &returnOrigin);
        if (ret == INIT_CONTEXT_FAILED)
        {
                goto cleanup1;
        }
        else if (ret == OPEN_SESSION_FAILED)
        {
                goto cleanup2;
        }

        bigInt1.size = BIG_INT_LENGTH;
        bigInt1.flags = TEEC_MEM_INPUT;
        result = TEEC_AllocateSharedMemory(&context, &bigInt1);
        memset(bigInt1.buffer, 0xff, bigInt1.size);
        memset(bigInt1.buffer, 0x0, 1);

        bigInt2.size = BIG_INT_LENGTH;
        bigInt2.flags = TEEC_MEM_INPUT;
        result = TEEC_AllocateSharedMemory(&context, &bigInt2);
        memset(bigInt2.buffer, 0xff, bigInt2.size);
        memset(bigInt2.buffer, 0x0, 1);

        bigOut.size = BIG_INT_LENGTH;
        bigOut.flags = TEEC_MEM_OUTPUT;
        result = TEEC_AllocateSharedMemory(&context, &bigOut);
        memset(bigOut.buffer, 0, bigOut.size);

        // set operation parameters
        memset(&operation, 0x00, sizeof(operation));

        operation.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_MEMREF_PARTIAL_INPUT, TEEC_MEMREF_PARTIAL_INPUT, TEEC_MEMREF_PARTIAL_OUTPUT);
        operation.started = 1;

        operation.params[0].value.a = 3;
        operation.params[0].value.b = 0;

        operation.params[1].memref.parent = &bigInt1;
        operation.params[1].memref.offset = 0;
        operation.params[1].memref.size = BIG_INT_LENGTH;

        operation.params[2].memref.parent = &bigInt2;
        operation.params[2].memref.offset = 0;
        operation.params[2].memref.size = BIG_INT_LENGTH;

        operation.params[3].memref.parent = &bigOut;
        operation.params[3].memref.offset = 0;
        operation.params[3].memref.size = BIG_INT_LENGTH;

        // invoke commend
        result = TEEC_InvokeCommand(&session, BIG_INT_ADD_COMMEND, &operation, NULL);
        if (result != TEEC_SUCCESS)
        {
                goto cleanup3;
        }
		/*test only */
        operation.params[3].memref.parent = &bigOut;
        operation.params[3].memref.offset = 0;
        operation.params[3].memref.size = BIG_INT_LENGTH;

        return_value = encodeJavaByteArray(env, operation.params[3].memref.parent->buffer, operation.params[3].memref.parent->size);

cleanup3:
        TEEC_CloseSession(&session);
cleanup2:
        TEEC_FinalizeContext(&context);
cleanup1:
        return return_value;
}
