#include "TextInputSources.h"
#include <CoreFoundation/CFDictionary.h>
#include <CoreFoundation/CFData.h>
#include <os/lock.h>
#import <AppKit/NSDisplay.h>

static os_unfair_lock g_keyboardLock = OS_UNFAIR_LOCK_INIT;
static int g_lastKeyboardLayoutId = -1;
static TISInputSourceRef g_lastKeyboardLayout = NULL;

TISInputSourceRef TISCopyCurrentKeyboardLayoutInputSource(void)
{
	NSDisplay* display = [NSClassFromString(@"NSDisplay") currentDisplay];
	if (!display)
		return NULL;
	const int curLayoutId = [display keyboardLayoutId];

	if (g_lastKeyboardLayoutId != -1 && g_lastKeyboardLayout != NULL)
	{
		os_unfair_lock_lock(&g_keyboardLock);
		if (curLayoutId == g_lastKeyboardLayoutId)
		{
			TISInputSourceRef rv = (TISInputSourceRef) CFRetain((CFDictionaryRef) g_lastKeyboardLayout);
			os_unfair_lock_unlock(&g_keyboardLock);
			return rv;
		}
		os_unfair_lock_unlock(&g_keyboardLock);
	}

	uint32_t length;
	UCKeyboardLayout* layout = [display keyboardLayout: &length];
	if (!layout)
		return NULL;

	CFDataRef data = CFDataCreate(NULL, (UInt8*) layout, length);
	free(layout);

	const void* keys[1] = { kTISPropertyUnicodeKeyLayoutData };
	const void* values[1] = { data };
	CFDictionaryRef dict = CFDictionaryCreate(NULL, keys, values, 1, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

	CFRelease(data);

	os_unfair_lock_lock(&g_keyboardLock);

	if (g_lastKeyboardLayout)
		CFRelease(g_lastKeyboardLayout);

	g_lastKeyboardLayout = (TISInputSourceRef) CFRetain(dict);
	g_lastKeyboardLayoutId = curLayoutId;

	os_unfair_lock_unlock(&g_keyboardLock);

	return (TISInputSourceRef) dict;
}

void* TISGetInputSourceProperty(TISInputSourceRef inputSourceRef, CFStringRef key)
{
	return (void*) CFDictionaryGetValue((CFDictionaryRef)inputSourceRef, key);
}

