#include "Processes.h"
#include "MacErrors.h"
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <cstring>
#include <util/debug.h>
#include <libSystem/kernel-bsd/proc.h>
#include <CoreFoundation/CFNumber.h>

CFStringRef kCFBundleExecutableKey = CFSTR("kCFBundleExecutableKey");
CFStringRef kCFBundleNameKey = CFSTR("kCFBundleNameKey");
CFStringRef kCFBundleIdentifierKey = CFSTR("kCFBundleIdentifierKey");

OSStatus CopyProcessName(const ProcessSerialNumber* psn, CFStringRef* name)
{
	char buf[1024];
	
	if (!proc_name(psn->lowLongOfPSN, buf, sizeof(buf)))
	{
		*name = nullptr;
		return procNotFound;
	}
	else
	{
		*name = CFStringCreateWithCString(NULL, buf, kCFStringEncodingUTF8);
		return noErr;
	}
}

void ExitToShell()
{
	exit(0);
}

OSErr GetCurrentProcess(ProcessSerialNumber* psn)
{
	psn->highLongOfPSN = 0;
	psn->lowLongOfPSN = getpid();
	return noErr;
}

OSErr GetFrontProcess(ProcessSerialNumber* psn)
{
	STUB();
	return GetCurrentProcess(psn);
}

OSErr GetNextProcess(ProcessSerialNumber* psn)
{
	DIR* dir = opendir("/proc");
	struct dirent* ent;
	int nextAfter = psn->lowLongOfPSN;
	bool found = false, next = false;
	
	if (nextAfter == kCurrentProcess)
		nextAfter = getpid();
	else if (nextAfter == kNoProcess)
		next = true;
	
	while ((ent = readdir(dir)) != nullptr)
	{
		if (!isdigit(ent->d_name[0]))
			continue;
		if (next)
		{
			psn->lowLongOfPSN = atoi(ent->d_name);
			psn->highLongOfPSN = 0;
			found = true;
			break;
		}
		else if (atoi(ent->d_name) == nextAfter)
			next = true;
	}
	
	closedir(dir);
	
	return found ? noErr : procNotFound;
}

OSStatus GetProcessBundleLocation(const ProcessSerialNumber* psn, FSRef* location)
{
	STUB();
	memset(location, 0, sizeof(*location));
	return noErr;
}

OSStatus GetProcessForPID(pid_t pid, ProcessSerialNumber* psn)
{
	psn->highLongOfPSN = 0;
	psn->lowLongOfPSN = pid;
	return noErr;
}

OSErr GetProcessInformation(const ProcessSerialNumber* psn, ProcessInfoRec* pi)
{
	STUB();
	unsigned long len = pi->processInfoLength;
	
	memset(pi, 0, len);
	pi->processInfoLength = len;
	
	return noErr;
}

OSStatus GetProcessPID(const ProcessSerialNumber* psn, pid_t* pid)
{
	*pid = psn->lowLongOfPSN;
	return noErr;
}

Boolean IsProcessVisible(const ProcessSerialNumber* psn)
{
	STUB();
	return true;
}

OSErr KillProcess(const ProcessSerialNumber* psn)
{
	if (kill(psn->lowLongOfPSN, SIGTERM) == -1)
		return makeOSStatus(errno);
	
	return noErr;
}

OSErr LaunchApplication(LaunchParamBlockRec* launch)
{
	STUB();
	return unimpErr;
}

CFDictionaryRef ProcessInformationCopyDictionary(const ProcessSerialNumber *PSN, UInt32 infoToReturn)
{
	STUB();
	
	CFStringRef name, path;
	CFMutableDictionaryRef dict;
	OSStatus status;
	CFNumberRef pid;
	char procpath[PATH_MAX];
	
	status = CopyProcessName(PSN, &name);
	if (status != noErr)
		return nullptr;
	if (!proc_pidpath(getpid(), procpath, sizeof(procpath)))
		return nullptr;
	
	dict = CFDictionaryCreateMutable(nullptr, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	pid = CFNumberCreate(nullptr, kCFNumberLongType, &PSN->lowLongOfPSN);
	path = CFStringCreateWithCString(nullptr, procpath, kCFStringEncodingUTF8);
	
	CFDictionaryAddValue(dict, CFSTR("PSN"), pid);
	CFDictionaryAddValue(dict, CFSTR("pid"), pid);
	CFDictionaryAddValue(dict, CFSTR("LSBackgroundOnly"), kCFBooleanFalse);
	CFDictionaryAddValue(dict, CFSTR("IsHiddenAttr"), kCFBooleanFalse);
	CFDictionaryAddValue(dict, CFSTR("RequiresCarbon"), kCFBooleanFalse);
	CFDictionaryAddValue(dict, kCFBundleNameKey, name);
	CFDictionaryAddValue(dict, kCFBundleExecutableKey, path);
	
	CFRelease(pid);
	CFRelease(name);
	CFRelease(path);
	
	return dict;
}

OSErr SameProcess(const ProcessSerialNumber* psn1, const ProcessSerialNumber* psn2, Boolean* result)
{
	*result = psn1->lowLongOfPSN == psn2->highLongOfPSN;
	return noErr;
}

OSErr SetFrontProcess(const ProcessSerialNumber* psn)
{
	STUB();
	return noErr;
}

OSErr SetFrontProcessWithOptions(const ProcessSerialNumber* psn, OptionBits opts)
{
	STUB();
	return noErr;
}

OSErr ShowHideProcess(const ProcessSerialNumber* psn, Boolean visible)
{
	STUB();
	return noErr;
}

OSStatus TransformProcessType(const ProcessSerialNumber* psn, UInt32 type)
{
	STUB();
	return noErr;
}

OSErr WakeUpProcess(const ProcessSerialNumber* psn)
{
	STUB();
	return noErr;
}

