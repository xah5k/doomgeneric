#include <stdio.h>
#include <ctype.h>
// #include <ah5kos.h>
#include <time.h>
#include "../../_shared/dwmapi.h"
#include <string.h>

#include "doomkeys.h"
#include "i_system.h"
#include "i_video.h"
#include "m_argv.h"
#include "doomgeneric.h"

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;
static Framebuffer* gFb = NULL;
static WNDHDL gWnd = NULL;
extern KSTATUS OsSleepMs(uint64_t ms);

void DG_DrawFrame() {
	memcpy((void*)gFb->ptr, (const void*)DG_ScreenBuffer, (DOOMGENERIC_RESX * DOOMGENERIC_RESY) * sizeof(pixel_t));
	DwmApiDrawFinish(gWnd);
}

void DG_SleepMs(uint32_t ms) {
    OsSleepMs((uint64_t)ms);
}

uint32_t DG_GetTicksMs() {
	return time(NULL)*1000;
}

int DG_GetKey(int* pressed, unsigned char* key) {
	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
		return 0;
	} else {
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*key = keyData & 0xFF;
		return 1;
	}
	return 0;
}

void DG_SetWindowTitle(const char * title) {
	DwmApiChangeName(gWnd, title);
}

static unsigned char convertToDoomKey(unsigned char key) {
	switch (key)
	{
    case '\n':
	case '\r':
		key = KEY_ENTER;
		break;
	case 0xE0: // doom keys conflict with ah5kos.h keys
		key = KEY_ESCAPE;
		break;
	case 'A':
		key = KEY_LEFTARROW;
		break;
	case 'D':
		key = KEY_RIGHTARROW;
		break;
	case 'W':
		key = KEY_UPARROW;
		break;
	case 'S':
		key = KEY_DOWNARROW;
		break;
	case 'F':
	case 'f':
		key = KEY_FIRE;
		break;
	case ' ':
		key = KEY_USE;
		break;
	case 0xF0: // F1 = rshift
		key = KEY_RSHIFT;
		break;
	default:
		key = tolower(key);
		break;
	}
	return key;
}

static void addKeyToQueue(int pressed, unsigned char keyCode) {
	unsigned char key = convertToDoomKey(keyCode);
	unsigned short keyData = (pressed << 8) | key;
	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

KSTATUS DWMCALLBACK WndEventCallback(DwmReEventResponse* info) {
    if (info->Type == DWMPCK_EVENT_KBD)  { 
		printf("process char %c\r\n", info->KbdChar);
		addKeyToQueue(1, info->KbdChar); 
		OsSleepMs(5);
		addKeyToQueue(0, info->KbdChar);
	}
    return KSUCCESS;
}

void DG_Init() {
    DwmApiInitalize();
    WNDHDL Window = NULL;
    Framebuffer* Fb = malloc(sizeof(Framebuffer));
    gFb = Fb;
    DwmApiCreateWin(DOOMGENERIC_RESX, DOOMGENERIC_RESY, 10, 10, DWMPCK_CRWIN_WINDOWED, &Window, Fb);
    if (!Window) {
        perror("window creation fail");
        return;
    }
	gWnd = Window;
    DwmApiChangeName(Window, "doomgeneric_ah5kos");
    memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));

}

int main(int argc, char **argv) {
	doomgeneric_Create(argc, argv);

	while (1)
	{
		doomgeneric_Tick();
	}


	return 0;
}
