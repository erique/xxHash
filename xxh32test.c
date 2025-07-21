// m68k-amigaos-gcc -noixemul -m68020 -mregparm=4 -O2 -fomit-frame-pointer -ffunction-sections -fdata-sections -Wl,--gc-sections xxh32test.c -o xxh32test -lm
#include <stdio.h>

#include <proto/exec.h>
#include <proto/timer.h>
#include <devices/timer.h>

#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION
#include "xxhash.h"

int getdata(const char* filename, void** mem, size_t* len)
{
	FILE* f = fopen(filename, "rb");
	if (!f)
	{
		printf("unable to open '%s'\n", filename);
		return -1;
	}
	fseek(f, 0, SEEK_END);
	*len = ftell(f);
	fseek(f, 0, SEEK_SET);
	*mem = malloc(*len);
	if (!*mem)
	{
		printf("failed to alloc %d bytes\n", *len);
		return -1;
	}
	if (fread(*mem, 1, *len, f) != *len)
	{
		printf("failed to read %d bytes\n", *len);
		return -1;
	}
	fclose(f);
	return 0;
}

__stdargs int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("%s <file>\n", argv[0]);
		return -1;
	}

	struct TimeRequest timerReq = {0};

	if (SysBase->LibNode.lib_Version >= 36)
	{
		if (OpenDevice(TIMERNAME, UNIT_ECLOCK, (struct IORequest*)&timerReq, 0))
		{
			printf("unable to open '%s'\n", TIMERNAME);
			return -1;
		}
	}

	struct Device* TimerBase = timerReq.tr_node.io_Device;

	const char* filename = argv[1];

	void* mem;
	size_t len;

	if (getdata(filename, &mem, &len) || mem == 0)
		return -1;

	printf("'%s' (buffer = %p ; %d bytes) -> ", filename, mem, len);

	struct EClockVal start, end;

	uint32_t efreq = ReadEClock(&start);

	uint32_t hash = XXH32(mem, len, 0);

	ReadEClock(&end);

	if (!hash) printf("");

	uint64_t diff = *(uint64_t*)(&end) - *(uint64_t*)(&start);

	float s = (float)diff / (float) efreq;
	float mb_s = ((float) len / (float)(1<<20) ) / s;

	printf("xxh32 = %08x ; %3.3f MB/s\n", hash, mb_s);

	CloseDevice((struct IORequest*)&timerReq);
	return 0;
}
