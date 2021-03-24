#include <iostream>
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <libavformat/avformat.h>
}

static void encode(AVCodecContext *enc_ctx, AVFrame *frame, AVPacket *pkt,
                   FILE *outfile)
{
    int ret;
    /* send the frame to the encoder */
    //if (frame)
    //    printf("Send frame %3"PRId64"\n", frame->pts);
    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "Error sending a frame for encoding\n");
        exit(1);
    }
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc_ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "Error during encoding\n");
            exit(1);
        }
        //printf("Write packet %3"PRId64" (size=%5d)\n", pkt->pts, pkt->size);
        fwrite(pkt->data, 1, pkt->size, outfile);
        av_packet_unref(pkt);
    }
}


int main(int argc, char *argv[]) {


    int ret;
    const char *filename;
    const char *codec_name;
    const char *codec_name1;
    const AVCodec *codec;
    const AVCodec *codec1;
    AVCodecContext *c= NULL;
    AVCodecContext *c1= NULL;

    AVDictionary *opts = NULL;
    ret = av_dict_set(&opts, "thisiskey", "123456789", 0);
    int i, x, y;
    FILE *f;
    AVFrame *frame;
    AVPacket *pkt;
    pkt = av_packet_alloc();
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    std::cout << "Hello, World!" << std::endl;
    av_log_set_level(48); // show everything

    filename = argv[1];
    codec_name = argv[2];
    //codec_name1 = argv[3];

   // EB_H265_ENC_CONFIGURATION *enc_config;
   // enc_config->encMode = 11;

    codec = avcodec_find_encoder_by_name(codec_name);
    //codec1 = avcodec_find_encoder_by_name(codec_name1);
    if (!codec)
        std::cout << "could not find codec" << std::endl;

    c = avcodec_alloc_context3(codec);
    AVCodecParameters *params = NULL;
    params = avcodec_parameters_alloc();
    c->codec_type = AVMEDIA_TYPE_VIDEO;
    ret = avcodec_parameters_from_context(params, c);
    //c1 = avcodec_alloc_context3(codec1);

    c->width = 320;
    c->height = 240;
    c->gop_size = 15;
    c->pix_fmt = AV_PIX_FMT_YUV420P;
    c->time_base= (AVRational){1,15};


    av_opt_set(c->priv_data, "enc_mode", "11",0);

    if (!av_codec_is_encoder(codec))
        std::cout << "not encoder" << std::endl;

    if (avcodec_open2(c, codec, &opts) < 0)
        std::cout << "could not open codec" << std::endl;

    f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", filename);
        exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width  = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate the video frame data\n");
        exit(1);
    }

    std::ifstream in("/home/ubu-dev/Developer/sample.yuv", std::ios::binary);
    in.seekg(0, std::ios::beg);
    int width = 320, height = 240;
    int inFrameSize = width * height * 1.5;
    int numOfFrames = 300;
    int size = inFrameSize*numOfFrames;

    char* inBuffer = (char*)malloc(size);
    in.read(inBuffer, size);
    in.close();

    /* encode 1 second of video */
    for (i = 0; i < numOfFrames; i++) {
        fflush(stdout);

        /* make sure the frame data is writable */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);

        for (y = 0; y < c->height; y++) {
            for (x = 0; x < c->width; x++) {
                frame->data[0][y * frame->linesize[0] + x] = inBuffer[y * frame->linesize[0] + x + i*(width*height + (width*height/4) + (width*height/4))];
            }
        }

        for (y = 0; y < c->height/2; y++) {
            for (x = 0; x < c->width/2; x++) {
                frame->data[1][y * frame->linesize[1] + x] = inBuffer[width*height + y * frame->linesize[1] + x + i*(width*height + (width*height/4) + (width*height/4))];
                frame->data[2][y * frame->linesize[2] + x] = inBuffer[width*height + (width*height/4) + y * frame->linesize[2] + x + i*(width*height + (width*height/4) + (width*height/4))];
            }
        }

        frame->pts = i;
        encode(c,frame,pkt,f);
        /* prepare a dummy image */
        /* Y */
//        for (y = 0; y < c->height; y++) {
//            for (x = 0; x < c->width; x++) {
//                frame->data[0][y * frame->linesize[0] + x] = x + y + i * 3;
//            }
//        }
//
//        /* Cb and Cr */
//        for (y = 0; y < c->height/2; y++) {
//            for (x = 0; x < c->width/2; x++) {
//                frame->data[1][y * frame->linesize[1] + x] = 128 + y + i * 2;
//                frame->data[2][y * frame->linesize[2] + x] = 64 + x + i * 5;
//            }
//        }

//        frame->pts = i;

        /* encode the image */
        //(c, frame, pkt, f);
        //avcodec_encode_video2(c,pkt, frame, f);

//        encode(c,frame,pkt,f);


    }
    //flush encoder
    encode(c, NULL, pkt, f);

    fclose(f);

    avcodec_close(c);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    return 0;
}
