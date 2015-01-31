#include "zeitengine.h"

ZeitEngine::ZeitEngine(GLVideoWidget* video_widget, QObject *parent) :
    QObject(parent)
{
    static bool avglobals_initialized = false;

    if(!avglobals_initialized) {
        avcodec_register_all();
        avfilter_register_all();
        av_register_all();

        avglobals_initialized = true;
    }

    display = video_widget;

    configured_framerate = ZEIT_RATE_24p;

    decoder_format = av_find_input_format("image2");
    decoder_format_context = NULL;
    decoder_codec = NULL;
    decoder_codec_context = NULL;
    decoder_frame = NULL;
    decoder_packet = NULL;

    scaler_context = NULL;
    scaler_frame = NULL;
    scaler_initialized = false;

    configured_filter = ZEIT_FILTER_NONE;
    filter_initialized = false;

    control_mutex.lock();
    stop_flag = false;
    loop_flag = false;
    filter_flag = ZEIT_FILTER_NONE;
    control_mutex.unlock();

    preview_flag = false;

    exporter_initialized = false;
}

ZeitEngine::~ZeitEngine()
{
    FreeDecoder();
}

void ZeitEngine::InitDecoder()
{
    int ret;

    try
    {
        QString first_image = (*source_sequence.constBegin()).absoluteFilePath();

        // Open input file
        if ((ret = avformat_open_input(&decoder_format_context, first_image.toUtf8().data(), decoder_format, NULL)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to open input file '%s'\n", first_image.toUtf8().data());
            throw(ret);
        }

        // Find decoder codec
        decoder_codec_context = decoder_format_context->streams[0]->codec;
        if( !(decoder_codec = avcodec_find_decoder(decoder_codec_context->codec_id)) ) {
            av_log(NULL, AV_LOG_ERROR, "Failed to find input codec\n");
            ret = AVERROR(EINVAL);
            throw(ret);
        }

        // Open decoder codec
        if( (ret = avcodec_open2(decoder_codec_context, decoder_codec, NULL)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to open input codec\n");
            throw(ret);
        }

        // Allocate decoder frame
        if( !(decoder_frame = av_frame_alloc()) ) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate decoder frame\n");
            ret = AVERROR(ENOMEM);
            throw(ret);
        }
    }
    catch(int code) {
        av_log(NULL, AV_LOG_ERROR, "Return code was " + code);
    }
}

void ZeitEngine::FreeDecoder()
{
    avcodec_close(decoder_codec_context);
    avformat_close_input(&decoder_format_context);
    av_frame_free(&decoder_frame);
}

void ZeitEngine::Load(const QFileInfoList& sequence)
{
    source_sequence = sequence;

    InitDecoder();

    preview_flag = true;

    Play();
}

void ZeitEngine::Cache()
{
    for(sequence_iterator = source_sequence.constBegin(); sequence_iterator != source_sequence.constEnd(); ++sequence_iterator)
    {
        emit ProgressUpdated("Caching sequence", sequence_iterator - source_sequence.constBegin(), source_sequence.size());

        DecodeFrame();

        ScaleFrame(decoder_frame,
                   DISPLAY_WIDTH,
                   DISPLAY_HEIGHT,
                   DISPLAY_AV_PIXEL_FORMAT);
    }

    emit ProgressUpdated("Cache ready", source_sequence.size(), source_sequence.size());
    emit MessageUpdated("Cache ready");

    FreeScaler();
}

void ZeitEngine::Play()
{
    sequence_iterator = source_sequence.constBegin();

    emit VideoConfigurationUpdated(DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_QT_PIXEL_FORMAT);
    emit MessageUpdated("Playback started");

    while(true) {
        float frame_timeframe;

        timer.start();

        control_mutex.lock();
        bool break_requested = stop_flag || (!loop_flag && sequence_iterator == source_sequence.constEnd());

        switch(configured_framerate) {

            case ZEIT_RATE_23_976:
                frame_timeframe = 1000.0f / (30000.0f / 1001.0f);
                break;

            case ZEIT_RATE_29_97:
                frame_timeframe = 1000.0f / (2997.0f / 125.0f);
                break;

            default:
                frame_timeframe = 1000.0f / configured_framerate;
        }


        ZeitFilter filter = filter_flag;
        control_mutex.unlock();

        if(break_requested) {
            emit MessageUpdated("Playback stopped");
            break;
        }

        if(sequence_iterator == source_sequence.constEnd()) {
            sequence_iterator = source_sequence.constBegin();
        }

        DecodeFrame();

        ScaleFrame(decoder_frame,
                   DISPLAY_WIDTH,
                   DISPLAY_HEIGHT,
                   DISPLAY_AV_PIXEL_FORMAT);

        if(filter != ZEIT_FILTER_NONE) {
            FilterFrame(scaler_frame, filter);
        }

        display->image_mutex.lock();
        if(filter != ZEIT_FILTER_NONE) {
            memcpy(display->image->bits(), filter_frame->data[0], filter_frame->linesize[0] * filter_frame->height);

        } else {
            memcpy(display->image->bits(), scaler_frame->data[0], scaler_frame->linesize[0] * scaler_frame->height);
        }
        display->image_mutex.unlock();

        emit VideoUpdated();

        FreeFilterData();

        ++sequence_iterator;

        if(timer.elapsed() > frame_timeframe) {
            qDebug() << "[Lag] " + QString::number(timer.elapsed() - frame_timeframe) + "ms";
        } else {
            qDebug() << "[Headroom] " + QString::number(frame_timeframe - timer.elapsed()) + "ms";
            QThread::currentThread()->msleep(frame_timeframe - timer.elapsed());
        }

        if(preview_flag) {
            preview_flag = false;
            qDebug() << "BREAKING BADLY";
            break;
        }
    }

    FreeFilter();
    FreeScaler();
}

void ZeitEngine::Export(const QFileInfo file)
{
    for(sequence_iterator = source_sequence.constBegin(); sequence_iterator != source_sequence.constEnd(); ++sequence_iterator)
    {
        emit ProgressUpdated("Exporting sequence", sequence_iterator - source_sequence.constBegin(), source_sequence.size());

        control_mutex.lock();
        ZeitFilter filter = filter_flag;
        control_mutex.unlock();

        DecodeFrame();

        ScaleFrame(decoder_frame,
                   decoder_frame->width,
                   decoder_frame->height,
                   EXPORT_PIXELFORMAT);

        if(filter != ZEIT_FILTER_NONE) {
            FilterFrame(scaler_frame, filter);
            ExportFrame(filter_frame, file);
            FreeFilterData();
        } else {
            ExportFrame(scaler_frame, file);
        }
    }

    emit ProgressUpdated("Export complete", source_sequence.size(), source_sequence.size());
    emit MessageUpdated("Export complete");

    CloseExport();
    FreeFilter();
    FreeScaler();

}

void ZeitEngine::DecodeFrame()
{
    int frame_decoded, ret;

    try
    {
        // Necessary procedure to get a stable reference to a char* path representation - well, check again.
        QByteArray image_bytearray = (*sequence_iterator).absoluteFilePath().toUtf8();
        const char* image_cstr = image_bytearray.data();

        // Open input file
        if ((ret = avformat_open_input(&decoder_format_context, image_cstr, decoder_format, NULL)) < 0) {
            av_log(NULL, AV_LOG_ERROR, "Failed to open input file '%s'\n", image_cstr);
            return;
        }

        // Allocate decoder packet
        decoder_packet = new AVPacket;
        av_init_packet(decoder_packet);

        // Read packet
        if( (ret = av_read_frame(decoder_format_context, decoder_packet)) < 0 ) {
            av_log(NULL, AV_LOG_ERROR, "Failed to read frame from file\n");
            throw(ret);
        }

        // Decode frame
        if( (ret = avcodec_decode_video2(decoder_codec_context, decoder_frame, &frame_decoded, decoder_packet)) < 0 || !frame_decoded ) {
            av_log(NULL, AV_LOG_ERROR, "Failed to decode image from file\n");
            throw(ret);
        }

        // Free decoder packet
        av_free_packet(decoder_packet);

        // Convert deprecated pixel formats to favored ones
        // See http://stackoverflow.com/questions/23067722/swscaler-warning-deprecated-pixel-format-used
        switch (decoder_frame->format) {
        case AV_PIX_FMT_YUVJ420P:
            decoder_frame->format = AV_PIX_FMT_YUV420P;
            break;
        case AV_PIX_FMT_YUVJ422P:
            decoder_frame->format = AV_PIX_FMT_YUV422P;
            break;
        case AV_PIX_FMT_YUVJ444P:
            decoder_frame->format = AV_PIX_FMT_YUV444P;
            break;
        case AV_PIX_FMT_YUVJ440P:
            decoder_frame->format = AV_PIX_FMT_YUV440P;
        default:
            decoder_frame->format = (AVPixelFormat)decoder_frame->format;
            break;
        }
    }
    catch(int code) {
        av_log(NULL, AV_LOG_ERROR, "Return code was " + code);
    }
}

void ZeitEngine::InitFilter(AVFrame* frame, ZeitFilter filter)
{
    AVFilter *buffersource = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *inputs = avfilter_inout_alloc();
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVPixelFormat pix_fmts[] = { (AVPixelFormat)frame->format, AV_PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params;
    char args[512];
    int ret;

    try
    {
        filter_graph = avfilter_graph_alloc();

        // Buffer Video Source (receives decoded frames)
        snprintf(args, sizeof(args),
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 frame->width,
                 frame->height,
                 frame->format,
                 1,
                 1,//source_framerate,//decoder_codec_context->time_base.den,
                 decoder_codec_context->sample_aspect_ratio.num,
                 decoder_codec_context->sample_aspect_ratio.den);

        if( (ret = avfilter_graph_create_filter(&buffersource_context,
                                                buffersource,
                                                "in",
                                                args,
                                                NULL,
                                                filter_graph)) < 0)
        {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
            throw(ret);
        }

        // Buffer Video Sink (terminates filter chain)
        buffersink_params = av_buffersink_params_alloc();
        buffersink_params->pixel_fmts = pix_fmts;
        ret = avfilter_graph_create_filter(&buffersink_context,
                                           buffersink,
                                           "out",
                                           NULL,
                                           buffersink_params,
                                           filter_graph);
        av_free(buffersink_params);
        if (ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
            throw(ret);
        }

        // Filter graph endpoints
        outputs->name = av_strdup("in");
        outputs->filter_ctx = buffersource_context;
        outputs->pad_idx = 0;
        outputs->next = NULL;
        inputs->name = av_strdup("out");
        inputs->filter_ctx = buffersink_context;
        inputs->pad_idx = 0;
        inputs->next = NULL;

        const char *filter_descriptor;

        switch(filter)
        {
        case ZEIT_FILTER_VIGNETTE:
            filter_descriptor = av_strdup("vignette=PI/4");
            break;
        case ZEIT_FILTER_BLACKWHITE:
            filter_descriptor = av_strdup("colorchannelmixer=.3:.4:.3:0:.3:.4:.3:0:.3:.4:.3");
            break;
        case ZEIT_FILTER_SEPIA:
            filter_descriptor = av_strdup("colorchannelmixer=.393:.769:.189:0:.349:.686:.168:0:.272:.534:.131");
            break;
        case ZEIT_FILTER_HIPSTAGRAM:
            filter_descriptor = av_strdup("colorbalance=rs=-.075:gs=.05:bs=.1:rm=.1:bm=-.05:rh=.1:gh=.1:bh=.1");
            break;
        case ZEIT_FILTER_NONE:
        default:
            filter_descriptor = av_strdup("null");
        }

        // Filter Danger Zone
        // const char *filter_descr = "boxblur=2:1:cr=0:ar=0";
        // const char *filter_descr = "scale=iw/1:-1";
        // const char *filter_descr = "zoompan=z='min(zoom+0.0015,1.5)':d=700:x='if(gte(zoom,1.5),x,x+1/a)':y='if(gte(zoom,1.5),y,y+1)':s=640x360";

        if( (ret = avfilter_graph_parse_ptr(filter_graph,
                                            filter_descriptor,
                                            &inputs,
                                            &outputs,
                                            NULL)) < 0)
        {
            throw(ret);
        }

        if( (ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        {
            return;
        }

        // Allocate filter frame
        if( !(filter_frame = av_frame_alloc()) ) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate filter frame\n");
            ret = AVERROR(ENOMEM);
            throw(ret);
        }

    }
    catch(int code)
    {
        av_log(NULL, AV_LOG_ERROR, "Return code was " + code);
        return;
    }

    configured_filter = filter;
    filter_initialized = true;
}

void ZeitEngine::FilterFrame(AVFrame* frame, ZeitFilter filter)
{
    if(!filter_initialized) {
        InitFilter(frame, filter);
    } else if(filter != configured_filter) {
        FreeFilter();
        InitFilter(frame, filter);
    }

    int ret;

    // Push the decoded frame into the filtergraph
    if( (ret = av_buffersrc_add_frame_flags(buffersource_context, frame, AV_BUFFERSRC_FLAG_KEEP_REF)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
        throw(ret);
    }

    // Pull filtered frames from the filtergraph
    while(true) {
        ret = av_buffersink_get_frame(buffersink_context, filter_frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            break;
        if (ret < 0) { throw(ret); }
    }
}

void ZeitEngine::FreeFilterData()
{
    if(filter_initialized) {
        av_frame_unref(filter_frame);
    }
}

void ZeitEngine::FreeFilter()
{
    if(filter_initialized) {
        avfilter_graph_free(&filter_graph);

        filter_initialized = false;
        configured_filter = ZEIT_FILTER_NONE;
    }
}

void ZeitEngine::InitScaler(AVFrame *frame,
                            const unsigned int target_width,
                            const unsigned int target_height,
                            const AVPixelFormat target_pixel_format)
{
    int ret;

    try
    {
        if( !(scaler_context = sws_getContext(frame->width,
                                              frame->height,
                                              (AVPixelFormat)frame->format,
                                              target_width,
                                              target_height,
                                              target_pixel_format,
                                              SWS_BILINEAR,
                                              NULL,
                                              NULL,
                                              NULL)) )
        {
            av_log(NULL, AV_LOG_ERROR,
            "Impossible to create scale context for the conversion "
            "fmt:%s s:%dx%d -> fmt:%s s:%dx%d\n",
            av_get_pix_fmt_name((AVPixelFormat)frame->format), frame->width, frame->height,
            av_get_pix_fmt_name(target_pixel_format), target_width, target_height);
            ret = AVERROR(EINVAL);
            throw(ret);
        }

        // Allocate scaler frame
        if( !(scaler_frame = av_frame_alloc()) ) {
            av_log(NULL, AV_LOG_ERROR, "Failed to allocate scaler frame\n");
            ret = AVERROR(ENOMEM);
            throw(ret);
        }

        av_frame_copy_props(scaler_frame, frame);
        scaler_frame->width = target_width;
        scaler_frame->height = target_height;
        scaler_frame->format = target_pixel_format;

        // Alignment has to be 32 because QImage needs it that way - But we don't always write to QImage - Keep an eye on this
        if( av_image_alloc(scaler_frame->data,
                           scaler_frame->linesize,
                           scaler_frame->width,
                           scaler_frame->height,
                           (AVPixelFormat)scaler_frame->format,
                           32) < 0 ) {
            fprintf(stderr, "Could not allocate scaler picture\n");
            exit(1);
        }

        // Buffer is going to be written to rawvideo file, no alignment <-- hm!
        //        if ((ret = av_image_alloc(scaler_data,
        //                                  scaler_linesize,
        //                                  target_width,
        //                                  target_height,
        //                                  target_pixel_format,
        //                                  1)) < 0)
        //        {
        //            fprintf(stderr, "Could not allocate destination image\n");
        //            throw(ret);
        //        }

        scaler_initialized = true;
    }
    catch(int code)
    {
        av_log(NULL, AV_LOG_ERROR, "Return code was " + code);
        return;
    }
}

void ZeitEngine::ScaleFrame(AVFrame *frame,
                            const unsigned int target_width,
                            const unsigned int target_height,
                            const AVPixelFormat target_pixel_format)
{
    if(!scaler_initialized) {
        InitScaler(frame,
                   target_width,
                   target_height,
                   target_pixel_format);
    }

    control_mutex.lock();
    if(vertical_flip_flag) {
        for(int i = 0; i < 4; i++) {
            frame->data[i] += frame->linesize[i] * (frame->height-1);
            frame->linesize[i] = -frame->linesize[i];
        }
    }
    control_mutex.unlock();

    sws_scale(scaler_context,
              (const uint8_t * const*)frame,
              frame->linesize,
              0,
              frame->height,
              scaler_frame->data,
              scaler_frame->linesize);
}

void ZeitEngine::FreeScaler()
{
    if(scaler_initialized) {
        sws_freeContext(scaler_context);
        av_freep(&scaler_frame->data[0]);
        av_frame_free(&scaler_frame);

        scaler_initialized = false;
    }
}

void ZeitEngine::InitExporter(AVFrame* frame, const QFileInfo output_file)
{
    int ret;
    encoder_output_file = new std::ofstream;

    if( !(encoder_codec = avcodec_find_encoder(EXPORT_CODEC_ID)) ) {
        av_log(NULL, AV_LOG_ERROR, "Codec not found\n");
        exit(1);
    }

    if( !(encoder_codec_context = avcodec_alloc_context3(encoder_codec)) ) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate video codec context\n");
        exit(1);
    }

    encoder_codec_context->bit_rate = 25000000;
    encoder_codec_context->width = frame->width;
    encoder_codec_context->height = frame->height;

    switch(configured_framerate) {

        case ZEIT_RATE_23_976:
            encoder_codec_context->time_base = (AVRational){1001, 30000};
            break;

        case ZEIT_RATE_29_97:
            encoder_codec_context->time_base = (AVRational){125, 2997};
            break;

        default:
            encoder_codec_context->time_base = (AVRational){1, configured_framerate};
            break;
    }

    encoder_codec_context->gop_size = 10; // Emit one intra frame every ten frames
    encoder_codec_context->max_b_frames = 1;
    encoder_codec_context->pix_fmt = (AVPixelFormat)frame->format;

    if(EXPORT_CODEC_ID == AV_CODEC_ID_H264) {
        av_opt_set(encoder_codec_context->priv_data, "preset", "slow", 0);
    }

    // Open encoder codec
    if( avcodec_open2(encoder_codec_context, encoder_codec, NULL) < 0 ) {
        av_log(NULL, AV_LOG_ERROR, "Could not open codec\n");
        exit(1);
    }

    encoder_output_file->open(output_file.absoluteFilePath().toUtf8().data(), std::ios::out | std::ios::binary);
    if ( !encoder_output_file->is_open() ) {
        av_log(NULL, AV_LOG_ERROR, "Could not open input file '%s'\n", output_file.absoluteFilePath().toUtf8().data());
        exit(1);
    }

    if ( !(encoder_frame = av_frame_alloc()) ) {
        av_log(NULL, AV_LOG_ERROR, "Could not allocate video frame\n");
        exit(1);
    }

    encoder_frame->format = encoder_codec_context->pix_fmt;
    encoder_frame->width = encoder_codec_context->width;
    encoder_frame->height = encoder_codec_context->height;

    // The image can be allocated by any means and av_image_alloc() is just the most convenient way if av_malloc() is to be used
    if( (ret = av_image_alloc(encoder_frame->data, encoder_frame->linesize, encoder_codec_context->width, encoder_codec_context->height, encoder_codec_context->pix_fmt, 1)) < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }

    encoder_packet = new AVPacket;
}

void ZeitEngine::ExportFrame(AVFrame* frame, const QFileInfo output_file)
{
    if(!exporter_initialized) {
        InitExporter(frame, output_file);
        exporter_initialized = true;
    }

    int got_output;

    av_init_packet(encoder_packet);
    encoder_packet->data = NULL; // Packet data will be allocated by the encoder
    encoder_packet->size = 0;

    // Y
    for(int y = 0; y < encoder_codec_context->height; y++) {
        for(int x = 0; x < encoder_codec_context->width; x++) {
            encoder_frame->data[0][y * encoder_frame->linesize[0] + x] = frame->data[0][y * frame->linesize[0] + x];
        }
    }

    // Cb and Cr
    for(int y = 0; y < encoder_codec_context->height / 2; y++) {
        for(int x = 0; x < encoder_codec_context->width / 2; x++) {
            encoder_frame->data[1][y * encoder_frame->linesize[1] + x] = frame->data[1][y * frame->linesize[1] + x] ;
            encoder_frame->data[2][y * encoder_frame->linesize[2] + x] = frame->data[2][y * frame->linesize[2] + x];
        }
    }

    encoder_frame->pts = sequence_iterator - source_sequence.constBegin();

    // Encode the image
    if( avcodec_encode_video2(encoder_codec_context, encoder_packet, encoder_frame, &got_output) < 0 ) {
        fprintf(stderr, "Error encoding frame\n");
        exit(1);
    }

    if(got_output) {
        encoder_output_file->write(reinterpret_cast<char *>(encoder_packet->data), encoder_packet->size);
        //av_freep(&encoder_packet->data);
        av_free_packet(encoder_packet);
    }
}

void ZeitEngine::CloseExport()
{
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    int got_output = 1;

    // Get the delayed frames
    while(got_output) {
        if( avcodec_encode_video2(encoder_codec_context, encoder_packet, NULL, &got_output) < 0 ) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (got_output) {
            printf("Write delayed frame (size=%5d)\n", encoder_packet->size);
            encoder_output_file->write(reinterpret_cast<char *>(encoder_packet->data), encoder_packet->size);
            av_free_packet(encoder_packet);
        }
    }

    // add sequence end code to have a real mpeg file
    //av_image_alloc(outputFrame->data, outputFrame->linesize, outputCodecContext->width, outputCodecContext->height, outputCodecContext->pix_fmt, 32)
    encoder_output_file->write(reinterpret_cast<char *>(endcode), sizeof(endcode));
    encoder_output_file->close();

    avcodec_close(encoder_codec_context);
    av_free(encoder_codec_context);         // Possibly double freeing
    av_freep(&encoder_frame->data[0]);
    av_frame_free(&encoder_frame);

    exporter_initialized = false;
}
