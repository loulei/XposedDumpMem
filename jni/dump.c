#include <android/log.h>
#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>
#include <fcntl.h>


#define TAG    "xposeddumpmem"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)

#define u1 unsigned char
#define u4 unsigned int
#define MAX_BUFF_LEN     1024
#define MAX_SEGMENT_SIZE 5120

typedef unsigned long ulong;



JNIEXPORT void JNICALL Java_com_example_xposeddumpmem_Dumpper_dump(JNIEnv *env, jclass clazz, jstring modulename, jstring packagename){
	LOGD("start dump");
	const char *module_name = NULL;
	const char *package_name = NULL;
	jboolean isCopy;

	module_name = (*env)->GetStringUTFChars(env, modulename, &isCopy);
	package_name = (*env)->GetStringUTFChars(env, packagename, &isCopy);

	LOGD("dump module: %s", module_name);
	char mapsfile[100] = {0};
	char dumpfilepath[256] = {0};
	int pid = getpid();
	LOGD("pid=%d", pid);
	sprintf(dumpfilepath, "/data/data/%s/dump_%s", package_name, module_name);
	sprintf(mapsfile, "/proc/%d/maps", pid);

	FILE *mapsfp = fopen(mapsfile, "r");

	ulong start = 0xFFFFFFFFL;
	ulong end = 0x00000000L;
	ulong tmpstart;
	ulong tmpend;

	if(mapsfp){
		char line[2048];
		while(fgets(line, sizeof(line), mapsfp) != NULL){
			if(strstr(line, module_name)){
				LOGD("find module:%s", line);
				int rv = sscanf(line, "%lx-%lx %*s %*s %*s %*s %*s", &tmpstart, &tmpend);
				LOGD("start=0x%08lx, end=0x%08lx, rv=%d", tmpstart, tmpend, rv);
				if(start > tmpstart){
					start = tmpstart;
				}
				if(end < tmpend){
					end = tmpend;
				}
			}
		}
		LOGD("start=0x%08lx, end=0x%08lx", start, end);

		int modulesize = end - start;
		void *buffer = malloc(modulesize);
		int PAGE_START = start/PAGE_SIZE * PAGE_SIZE;
		int PAGE_END = (end/PAGE_SIZE +1) * PAGE_SIZE;
		if(mprotect(PAGE_START, PAGE_END - PAGE_START, PROT_READ | PROT_EXEC | PROT_WRITE) != 0){
			LOGD("mprotect 0x%08lx to 0x%08lx failed", PAGE_START, PAGE_END);
			return;
		}
		LOGD("mprotect 0x%08lx to 0x%08lx success", PAGE_START, PAGE_END);
		memcpy(buffer, (void*)start, modulesize);
		FILE *dumpfd = fopen(dumpfilepath, "wb+");
		if(dumpfd){
			fwrite(buffer, modulesize, 1, dumpfd);
			free(buffer);
			fclose(dumpfd);
		}else{
			LOGD("fopen dumpfile %s failed", dumpfilepath);
			LOGD("%s", strerror(errno));
			return;
		}

		fclose(mapsfp);
	}else{
		LOGD("fopen maps failed");
		return;
	}




	(*env)->ReleaseStringUTFChars(env, modulename, module_name);
	(*env)->ReleaseStringUTFChars(env, packagename, package_name);
}




