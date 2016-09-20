#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <flite/flite.h>

cst_voice *register_cmu_us_kal();

ndspWaveBuf waveBuf;
cst_wave *fliteWave;
cst_voice *voice;
u16 *samples = NULL;

const char *textList[] = {
	"What the fuck did you just fucking say about me,",
	"I'll have you know I graduated top of my class in the Navy Seals,",
	"and I've been involved in numerous secret raids on Al-Quaeda,",
	"and I have over 300 confirmed kills.",
	"I am trained in gorilla warfare",
	"and I'm the top sniper in the entire US armed forces.",
	"You are nothing to me but just another target.",
	"I will wipe you the fuck out with precision",
	"the likes of which has never been seen before on this Earth,",
	"mark my fucking words.",
	"You think you can get away",
	"with saying that shit to me over the Internet?",
	"Think again, fucker.",
	"As we speak I am contacting my secret network of spies across the USA",
	"and your IP is being traced right now",
	"so you better prepare for the storm, maggot.",
	"The storm that wipes out the pathetic little thing you call your life.",
	"You're fucking dead, kid.",
	"I can be anywhere, anytime,",
	"and I can kill you in over seven hundred ways,",
	"and that's just with my bare hands.",
	"Not only am I extensively trained in unarmed combat,",
	"but I have access to the entire arsenal of the United States Marine Corps",
	"and I will use it to its full extent",
	"to wipe your miserable ass off the face of the continent,",
	"you little shit.",
	"If only you could have known what unholy retribution",
	"your little \"clever\" comment was about to bring down upon you,",
	"maybe you would have held your fucking tongue.",
	"But you couldn't,",
	"you didn't,",
	"and now you're paying the price,",
	"you goddamn idiot.",
	"I will shit fury all over you",
	"and you will drown in it.",
	"You're fucking dead, kiddo.",
};

void processText(const char *text)
{
	static int channel = 0;
	int dataSize;
	
	printf("%s ", text);
	
	fliteWave = flite_text_to_wave(text, voice);
	dataSize = fliteWave->num_samples * fliteWave->num_channels * 2;
	
	linearFree(samples);
	samples = linearAlloc(dataSize);
	memcpy(samples, fliteWave->samples, dataSize);
	
	memset(&waveBuf, 0, sizeof(ndspWaveBuf));
	waveBuf.data_vaddr = samples;
	waveBuf.nsamples = fliteWave->num_samples / fliteWave->num_channels;
	waveBuf.looping = false;
	waveBuf.status = NDSP_WBUF_FREE;
	
	ndspChnReset(channel);
	ndspChnSetInterp(channel, NDSP_INTERP_POLYPHASE);
	ndspChnSetRate(channel, fliteWave->sample_rate);
	ndspChnSetFormat(channel, NDSP_FORMAT_MONO_PCM16);
	
	DSP_FlushDataCache((u8*)samples, dataSize);
	ndspChnWaveBufAdd(channel, &waveBuf);
}

int main()
{
	gfxInitDefault();
	ndspInit();
	consoleInit(GFX_TOP, NULL);
	
	flite_init();
	voice = register_cmu_us_kal(NULL);
	
	int textCount = sizeof textList / sizeof *textList;
	int textIndex = 0;
	
	waveBuf.status = NDSP_WBUF_DONE;
	svcSleepThread(2000000000);

	while (aptMainLoop())
	{
		hidScanInput();
		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break;

		if (waveBuf.status == NDSP_WBUF_DONE)
		{
			if (textIndex >= textCount)
				break;
			if (textIndex == textCount - 1)
				printf("\x1b[31;1m");
			processText(textList[textIndex++]);
		}

		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();
	}
	
	linearFree(samples);

	ndspExit();
	gfxExit();
	return 0;
}
