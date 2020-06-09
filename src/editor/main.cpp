#include "mainwindow.h"
#include <QApplication>
#include <dvigl/core.h>

#include <webmplay/OpusVorbisDecoder.h>
#include <webmplay/VPXDecoder.h>
#include <webmplay/mkvparser.h>

class SDLWebmReader : public mkvparser::IMkvReader
{
public:
    SDLWebmReader(const char* filePath)
        : m_file(SDL_RWFromFile(filePath, "rb"))
    {
    }
    ~SDLWebmReader()
    {
        if (m_file)
            SDL_RWclose(m_file);
    }

    int Read(long long pos, long len, unsigned char* buf)
    {
        if (!m_file)
            return -1;
        SDL_RWseek(m_file, pos, RW_SEEK_SET);
        const size_t size = SDL_RWread(m_file, buf, 1, len);
        if (size < size_t(len))
            return -1;
        return 0;
    }
    int Length(long long* total, long long* available)
    {
        if (!m_file)
            return -1;
        Sint64 pos = SDL_RWtell(m_file);
        SDL_RWseek(m_file, 0, RW_SEEK_END);
        if (total)
            *total = SDL_RWtell(m_file);
        if (available)
            *available = SDL_RWtell(m_file);
        SDL_RWseek(m_file, pos, RW_SEEK_SET);
        return 0;
    }

private:
    SDL_RWops* m_file;
};

typedef struct AudioQueue
{
    short* pcm;
    int numOutSamples;
    struct AudioQueue* next;
} AudioQueue;

static volatile AudioQueue* audio_queue = NULL;
static volatile AudioQueue* audio_queue_tail = NULL;

static void SDLCALL audio_callback(void* userdata, Uint8* stream, int len)
{
    Sint16* dst = (Sint16*)stream;
    memset(dst, '\0', len);

    LOG("LEN %d\n", len);
    // LOG("audio_queue %p\n", audio_queue);
    if (audio_queue && (len > 0))
    {
        volatile AudioQueue* item = audio_queue;
        AudioQueue* next = item->next;
        // const int channels = 2;
        // for(int p=0; p < len/2; ++p){
        //     dst[p] = (Sint16)item->pcm[p];
        // //     dst[p+1] = item->pcm[p];
        // }
        // memcpy(dst, item->pcm, item->numOutSamples*2);
        SDL_MixAudio(stream, (Uint8*)item->pcm, len, SDL_MIX_MAXVOLUME);
    }
    if (!audio_queue)
        audio_queue_tail = NULL;
}

static void queue_audio(short* pcm, int numOutSamples)
{
    // LOG("queue_audio: pcm = %p\n", pcm);
    if (numOutSamples)
    {
        AudioQueue* item = (AudioQueue*)SDL_malloc(sizeof(AudioQueue));
        if (!item)
        {
            LOG("MEMORY ERROR\n");
            return;
        }

        item->pcm = pcm;
        item->numOutSamples = numOutSamples;
        item->next = NULL;

        SDL_LockAudio();
        if (audio_queue_tail)
            audio_queue_tail->next = item;
        else
            audio_queue = item;
        audio_queue_tail = item;
        SDL_UnlockAudio();
    }
}

int main(int argc, char* argv[])
{

    // SDL_Init(SDL_INIT_EVERYTHING);

    //   int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    // int win_w = 960;
    // int win_h = 720;
    // SDL_Window* win = SDL_CreateWindow("webm video", 100, 100, win_w, win_h, flags);
    // SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, 0);
    // SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, win_w,
    // win_h);
    // void *pixels = NULL;
    // int pitch = 0;

    //     WebMDemuxer demuxer(new SDLWebmReader("../res/videos/video.webm"));
    //     if (demuxer.isOpen())
    //     {
    //         VPXDecoder videoDec(demuxer, 8);
    //         OpusVorbisDecoder audioDec(demuxer);

    //         WebMFrame videoFrame, audioFrame;

    //         VPXDecoder::Image image;
    //         short *pcm = audioDec.isOpen() ? new short[audioDec.getBufferSamples() * demuxer.getChannels()] : NULL;

    //         fprintf(stderr, "Length: %f\n", demuxer.getLength());

    // SDL_AudioSpec spec;
    // memset(&spec, '\0', sizeof(SDL_AudioSpec));

    // spec.freq = (int) demuxer.getSampleRate();
    // spec.format = AUDIO_S16SYS;
    // spec.channels = demuxer.getChannels();
    // // spec.channels = 1;
    // spec.samples = audioDec.getBufferSamples();
    // // spec.samples = 1024;
    // spec.callback = audio_callback;

    // LOG("demuxer.getChannels() = %d\n", demuxer.getChannels());
    // LOG("audioDec.getBufferSamples() = %d\n", audioDec.getBufferSamples());
    // LOG("demuxer.getSampleRate() = %f\n", demuxer.getSampleRate());

    // SDL_OpenAudio(&spec, NULL);
    // SDL_PauseAudio(0);

    //         while (demuxer.readFrame(&videoFrame, &audioFrame))
    //         {
    //             if (videoDec.isOpen() && videoFrame.isValid())
    //             {
    //                 if (!videoDec.decode(videoFrame))
    //                 {
    //                     fprintf(stderr, "Video decode error\n");
    //                     break;
    //                 }
    //                 while (videoDec.getImage(image) == VPXDecoder::NO_ERROR)
    //                 {

    //                     SDL_LockTexture(texture, NULL, &pixels, &pitch);
    //                     Uint8 *dst = (Uint8*)pixels;
    //                     const Uint8 *src = (const Uint8 *)image.planes[0];

    //                     for (int row = 0; row < 720; ++row)
    //                     {
    //                         memcpy(dst, src, pitch);
    //                         dst += pitch;
    //                         src += image.linesize[0];
    //                     }

    //                     src = (const Uint8 *)image.planes[1];
    //                     for (int row = 0; row < 720/2; ++row)
    //                     {
    //                         memcpy(dst, src, pitch / 2);
    //                         dst += pitch / 2;
    //                         src += image.linesize[1];
    //                     }

    //                     src = (const Uint8 *)image.planes[2];
    //                     for (int row = 0; row < 720/2; ++row)
    //                     {
    //                         memcpy(dst, src, pitch / 2);
    //                         dst += pitch / 2;
    //                         src += image.linesize[2];
    //                     }

    //                     SDL_UnlockTexture(texture);
    //                 }
    //             }

    //             SDL_RenderClear(renderer);
    //             SDL_RenderCopy(renderer, texture, NULL, NULL);
    //             SDL_RenderPresent(renderer);
    //             SDL_Delay(10);
    //             if (audioDec.isOpen() && audioFrame.isValid())
    //             {
    //                 int numOutSamples;
    //                 if (!audioDec.getPCMS16(audioFrame, pcm, numOutSamples))
    //                 {
    //                     fprintf(stderr, "Audio decode error\n");
    //                     break;
    //                 } else {
    //                     if(numOutSamples >0){
    //                         queue_audio(pcm, numOutSamples);
    //                         LOG("numOutSamples = %d\n", numOutSamples);
    //                     }
    //                 }
    //            }
    //         }

    //         delete[] pcm;
    //     }

    //     return 0;
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
