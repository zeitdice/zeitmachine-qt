#ifndef ZEITENGINE_H
#define ZEITENGINE_H

/** \file
 * ZeitEngine header
 * Declares the `ZeitEngine` class and `ZeitFilter` and `ZeitRate` enums
 */

#include <QApplication>
#include <QObject>
#include <QDebug>
#include <QElapsedTimer>
#include <QFileInfoList>
#include <QImage>
#include <QMutex>
#include <QScreen>
#include <QString>
#include <QThread>
#include <QWaitCondition>

#include <iostream>
#include <fstream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/avcodec.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libswscale/swscale.h>
}

#include "glvideowidget.h"

/*!
 * \brief Identifies a (to be) used filter, or no filter.
 */
enum ZeitFilter {
    ZEIT_FILTER_NONE = -1,
    ZEIT_FILTER_VIGNETTE,
    ZEIT_FILTER_BLACKWHITE,
    ZEIT_FILTER_SEPIA,
    ZEIT_FILTER_HIPSTAGRAM
};

/*!
 * \brief Identifies possible frame rates to use and configure
 */
enum ZeitRate {
    ZEIT_RATE_23_976,
    ZEIT_RATE_24p = 24,
    ZEIT_RATE_25p = 25,
    ZEIT_RATE_29_97,
    ZEIT_RATE_30p = 30,
    ZEIT_RATE_48p = 48,
    ZEIT_RATE_50p = 50,
    ZEIT_RATE_60p = 60
};

/*!
 * \brief The `ZeitEngine`: Central threadable encoding facility class
 *
 * This class is meant to run in a separate thread; It offers a high level
 * abstraction to caching, playing and exporting image and video data.
 * During continuous operations (especially playing) the `ZeitEngine` is
 * controlled via various flags (e.g. `stop_flag`, `filter_flag`, etc.), which
 * should only be accessed safely through the use of the `control_mutex'.
 */
class ZeitEngine : public QObject
{
    Q_OBJECT

    QElapsedTimer timer;

    const static AVPixelFormat DISPLAY_AV_PIXEL_FORMAT = AV_PIX_FMT_RGB24;
    const static QImage::Format DISPLAY_QT_PIXEL_FORMAT = QImage::Format_RGB888;

    const static AVPixelFormat EXPORT_PIXELFORMAT = AV_PIX_FMT_YUV420P;
    const static AVCodecID EXPORT_CODEC_ID = AV_CODEC_ID_H264;

    const static unsigned int ASSUMED_AVAILABLE_MEMORY = 512 * 1024 * 1024;

    // Source data

    QFileInfoList source_sequence;

    // Display data

    GLVideoWidget* display;

    unsigned int display_refresh_rate;
    unsigned int display_safe_max_width;
    unsigned int display_safe_max_height;
    unsigned int display_width;
    unsigned int display_height;
    bool display_initialized;    //!< Flag to check for display being initialized

    // Cache members
    //uint8_t **cache_data;

    // Decoder members

    AVInputFormat *decoder_format;
    AVCodec *decoder_codec;
    AVCodecContext *decoder_codec_context;
    AVFormatContext *decoder_format_context;
    AVFrame *decoder_frame;
    AVPacket *decoder_packet;

    // uint8_t *decoder_data[4];
    // int decoder_data_linesize[8];

    // Filter members

    AVFilterContext *buffersink_context;
    AVFilterContext *buffersource_context;
    AVFilterGraph *filter_graph;
    AVFrame *filter_frame;
    ZeitFilter configured_filter;   //!< Stores currently configured filter to
                                    //!< determine if a re-configuration needs
                                    //!< to happen on request of another filter

    bool filter_initialized;    //!< Flag to check for filters being initialized

    // Exporter members

    AVCodec *encoder_codec;
    AVCodecContext *encoder_codec_context;
    AVFrame *encoder_frame;
    AVPacket *encoder_packet;
    std::ofstream* encoder_output_file;

    bool exporter_initialized;

    // Scaler members

    SwsContext *scaler_context;
    AVFrame* scaler_frame;
    bool scaler_initialized;

    // Rescaler members

    SwsContext *rescaler_context;
    AVFrame* rescaler_frame;
    bool rescaler_initialized;

    // Play members

    QFileInfoList::const_iterator sequence_iterator;

    /*!
     * \brief Used for one-shot playing, aka first frame preview on footage loading
     *
     * First set the flag, then running Play() afterwards will stop after the
     * first frame and unset the preview_flag again.
     */
    bool preview_flag;

    /*!
     * \brief Probe the first image of the sequence for format/codec data
     *
     * Opens and decodes the first image of the sequence to determine and store
     * file format, pixel format, width, height and the likes for all following
     * decoding procedures.
     */
    void InitDecoder();

    /*!
     * \brief Free all decoder members
     *
     * Free all decoder members
     */
    void FreeDecoder();

    /*!
     * \brief Read a frame from disk into the `decoder_frame` buffer
     *
     * Read a frame from disk into the `decoder_frame` buffer. The image
     * currently pointed to by the `sequence_iterator` variable tells the
     * function which image to decode from the whole sequence.
     */
    void DecodeFrame();

    /*!
     * \brief Initialise all filter members
     * \param frame The source frame to filter from
     * \param filter The filter to initialise
     *
     * Don't call this yourself! `FilterFrame()` automatically initialises
     * itself the first time or when a new filter has been requested. Do call
     * `FreeFilter()` to clean up if you want though!
     */
    void InitFilter(AVFrame *frame, ZeitFilter filter);

    /*!
     * \brief Filter the frame
     * \param frame The source frame to filter from
     * \param filter The filter to apply
     */
    void FilterFrame(AVFrame *frame, ZeitFilter filter);

    /*!
     * \brief Free filter data for this frame
     */
    void FreeFilterData();

    /*!
     * \brief Free all filter members
     */
    void FreeFilter();

    /*!
     * \brief Initialise the scaler
     * \param Reference source frame to scale from
     * \param target_width Target frame width
     * \param target_height Target frame height
     * \param target_pixel_format Target frame pixel format
     * \sa ScaleFrame
     *
     * Don't call this yourself, `ScaleFrame()` automatically calls this the
     * first time you use it for scaling. (But do call `FreeScaler()` as soon
     * as the function you use scaling in finishes to free everything again!)
     */
    void InitScaler(AVFrame *frame,
                    const unsigned int target_width,
                    const unsigned int target_height,
                    const AVPixelFormat target_pixel_format);

    /*!
     * \brief Scale the currently loaded decoder frame
     * \param Reference source frame to scale from
     * \param target_width Target frame width
     * \param target_height Target frame height
     * \param target_pixel_format Target frame pixel format
     *
     * Provide your source frame and the target format, scaling results go into
     * scaler_frame! Use `FreeScaler()` to clean up after your function that
     * uses scaling.
     */
    void ScaleFrame(AVFrame *frame,
                   const unsigned int target_width,
                   const unsigned int target_height,
                   const AVPixelFormat target_pixel_format);

    /*!
     * \brief Free all allocated scaler members
     *
     * Free all allocated scaler members
     */
    void FreeScaler();

    /*!
     * \brief Initialise the rescaler
     * \param Reference source frame to rescale from
     * \param target_width Target frame width
     * \param target_height Target frame height
     * \param target_pixel_format Target frame pixel format
     * \sa RescaleFrame
     *
     * Don't call this yourself, `RescaleFrame()` automatically calls this the
     * first time you use it for scaling. (But do call `FreeRescaler()` as soon
     * as the function you use scaling in finishes to free everything again!)
     */
    void InitRescaler(AVFrame *frame,
                      const unsigned int target_width,
                      const unsigned int target_height,
                      const AVPixelFormat target_pixel_format);

    /*!
     * \brief Rescale the currently loaded decoder frame
     * \param Reference source frame to rescale from
     * \param target_width Target frame width
     * \param target_height Target frame height
     * \param target_pixel_format Target frame pixel format
     *
     * Provide your source frame and the target format, rescaling results go into
     * rescaler_frame! Use `FreeRescaler()` to clean up after your function that
     * uses rescaling.
     */
    void RescaleFrame(AVFrame *frame,
                      const unsigned int target_width,
                      const unsigned int target_height,
                      const AVPixelFormat target_pixel_format);

    /*!
     * \brief Free all allocated rescaler members
     *
     * Free all allocated rescaler members
     */
    void FreeRescaler();

    /*!
     * \brief Initalize the encoder
     * \param frame Pointer to the frame that shall be encoded
     * \param output_file The path of the movie file to be created
     * \sa EncodeFrame
     *
     * Initializes all members of the encoder to a configuration that fits the
     * passed frame for encoding; Opens the file for output to disk.
     */
    void InitExporter(AVFrame* frame, const QFileInfo output_file);

    /*!
     * \brief Encode current frame to video file on disk
     * \return Some return code
     */
    void ExportFrame(AVFrame *frame, const QFileInfo output_file);

    /*!
     * \brief Free all encoder members
     *
     * Free all encoder members
     */
    void CloseExport();

    /*!
     * \brief Sleep for x msec
     * \param msec Number of milliseconds to sleep
     */
    void Sleep(const unsigned int msec);

public:

    /*!
     * \brief Control mutex for safe signaling to the ZeitEngine
     *
     * This control mutex should be used when configuring ZeitEngine's public members
     * loop_flag, stop_flag, filter_flag and framerate_configuration.
     */
    QMutex control_mutex;

    /*!
     * \brief Used for signaling the ZeitEngine to loop playback
     */
    bool loop_flag;

    /*!
     * \brief Used for signaling the ZeitEngine to stop playback
     */
    bool stop_flag;

    /*!
     * \brief Used for signaling the ZeitEngine to flip source footage
     */
    bool vertical_flip_flag;

    /*!
     * \brief Used for signaling the ZeitEngine to stop playback
     */
    ZeitFilter filter_flag;

    /*!
     * \brief Used to configure playback and export framerate for the ZeitEngine
     */
    ZeitRate configured_framerate;

    /*!
     * \brief Initialize the ZeitEngine
     * \param video_widget The display widget context to output to
     *
     * Internally store references to the images to be processed, and decode
     * the first image of the sequence to initialize the codec, file format,
     * pixel format, width, height, linesize, etc.
     */
    explicit ZeitEngine(GLVideoWidget* video_widget, QObject *parent = 0);

    /*!
     * \brief Free the engine
     */
    ~ZeitEngine();

signals:
    void VideoConfigurationUpdated(const unsigned int width, const unsigned int height, const QImage::Format pixel_format);
    void VideoUpdated();
    void ControlsEnabled(const bool lock);
    void MessageUpdated(const QString text);
    void ProgressUpdated(const QString text, const int current, const int total);
public slots:

    /*!
     * \brief Load passed footage
     *
     *
     */
    void Load(const QFileInfoList& sequence);

    /*!
     * \brief Build the in-memory video cache
     *
     * Decodes and cheaply re-encodes all frames to the in-memory video cache
     */
    void Cache();

    /*!
     * \brief Request a refresh
     *
     * Refresh (single frame playback) after flipping/filtering during non-playback
     */
    void Refresh();

    /*!
     * \brief Start continuous playback, adhering to loop_flag and stop_flag
     *
     * Start continuous playback, adhering to loop_flag and stop_flag
     */
    void Play();

    /*!
     * \brief Export the sequence from source to a video file with high quality
     * \param The file to export to
     * \return true in case of success, false in case of failure
     */
    void Export(const QFileInfo file);

};

#endif // ZEITENGINE_H
