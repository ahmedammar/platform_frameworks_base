#define bool int
#define true 1
#define false 0
#define LOGE GST_ERROR
#define TRACE() GST_ERROR("%s:%d",__FILE__,__LINE__)

//#define LOG_TAG "cut_worker"
//#include <utils/Log.h>

//#define USE_THREAD
//#define USE_SIMPLE_WATCH
//#define USE_NEW_WATCH
#define USE_ONE_STATE_CHANGE
//#define EARLY_THREAD
#define USE_TRIVIAL_PIPELINE
#define USE_CUSTOM_LOOP
#define USE_ASYNC
//#define USE_SYNC_HANDLER

static __thread bool seenerrors = false;

static const char *state_names[] = { "PENDING", "NULL", "READY", "PAUSED", "PLAYING "};

#ifdef USE_NEW_WATCH
static gboolean bus_message_cb_new (GstBus * bus, GstMessage * message, GMainLoop * mainloop)
#elif defined USE_SIMPLE_WATCH
static void bus_message_cb (GstBus * bus, GstMessage * message, GMainLoop * mainloop)
#else
static GstBusSyncReply bus_message (GstBus * bus, GstMessage * message, GMainLoop * mainloop)
#endif
{
  GstState old_state, new_state, pending;
  GError *gerror;
  gchar *debug;


  LOGE("Message callback received %s", GST_MESSAGE_TYPE_NAME(message));
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_ERROR:
      LOGE ("bus_message_cb: ERROR\n");
      seenerrors = true;
      g_main_loop_quit (mainloop);
      break;
    case GST_MESSAGE_WARNING:
      gst_message_parse_warning (message, &gerror, &debug);
      LOGE ("%s, %s", gerror?gerror->message:"<nil>", debug);
      g_error_free (gerror);
      g_free (debug);
      break;
    case GST_MESSAGE_EOS:
      LOGE ("bus_message_cb: Done\n");
      g_main_loop_quit (mainloop);
      break;
    case GST_MESSAGE_STATE_CHANGED:
      gst_message_parse_state_changed (message, &old_state, &new_state, &pending);
      LOGE ("bus_message_cb: STATE_CHANGED for %s: old %s new %s pending %s",
        GST_MESSAGE_SRC(message) ? GST_ELEMENT_NAME(GST_MESSAGE_SRC(message)) : "<unknown>",
        state_names[old_state], state_names[new_state], state_names[pending]);
      break;
    default:
      LOGE("bus_message_cb: wut ? %d", GST_MESSAGE_TYPE(message));
      break;
  }
#ifdef USE_NEW_WATCH
  return TRUE;
#elif !defined USE_SIMPLE_WATCH
  return GST_BUS_PASS;
#endif
}

static bool wait_for_set_state (GstBus *bus, int timeout_msec)
{
  GstMessage *msg;
  GstStateChangeReturn ret = GST_STATE_CHANGE_FAILURE;

  /* Wait for state change */
  msg = gst_bus_timed_pop_filtered (bus, timeout_msec * GST_MSECOND,     /* in nanosec */
      (GstMessageType) (GST_MESSAGE_ERROR | GST_MESSAGE_ASYNC_DONE));

  if (msg) {
    if ((GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ASYNC_DONE)) {
      ret = GST_STATE_CHANGE_SUCCESS;
      LOGE("Pipeline has changed state");
    }
    else {
      LOGE("Error happened");
    }

    gst_message_unref (msg);
  }
  else {
    LOGE("No message received before timeout");
  }

  return ret == GST_STATE_CHANGE_SUCCESS;
}

static bool set_state(GstPipeline *pipeline, GstState state, const char *state_name, GstBus *bus)
{
  GstStateChangeReturn state_change_return;
  bool ok = false;

  LOGE("Setting pipeline to %s", state_name);
  state_change_return = gst_element_set_state (GST_ELEMENT (pipeline), state);
  switch (state_change_return) {
    case GST_STATE_CHANGE_NO_PREROLL:
      LOGE (" -> Pipeline needs not preroll, started");
      ok = true;
      break;
    case GST_STATE_CHANGE_ASYNC:
      LOGE (" -> Pipeline starting asynchronously");
      ok = wait_for_set_state(bus, 10000);
      break;
    case GST_STATE_CHANGE_SUCCESS:
      LOGE (" -> Pipeline started");
      ok = true;
      break;
    case GST_STATE_CHANGE_FAILURE:
      LOGE (" -> Failed to change state");
      ok = false;
      break;
    default:
      LOGE (" -> Unknown state change return: %d", state_change_return);
      ok = false;
      break;
  }
  if (ok) {
    LOGE("Pipeline succesfully changed state to %s", state_name);
  }
  else {
    LOGE("Pipeline failed to change state to %s", state_name);
  }
  return ok;
}

#ifdef USE_THREAD
static gpointer threaded_g_main_loop_run(gpointer p)
{
  GMainLoop *mainloop = (GMainLoop*)p;
  LOGE("Running glib loop from thread");
  g_main_loop_run(mainloop);
  LOGE("glib loop from thread finished");
  return NULL;
}
#endif

static void add_bus_watch(GstBus *bus, GMainContext *maincontext, GMainLoop *mainloop)
{
#if !defined USE_SIMPLE_WATCH && !defined USE_NEW_WATCH
  GSource *mBusWatch = NULL;
#endif

#ifdef USE_SYNC_HANDLER
  gst_bus_set_sync_handler (bus, (GstBusSyncHandler) bus_message, mainloop);
#elif defined USE_NEW_WATCH
  gst_bus_add_watch (bus, (GstBusFunc)bus_message_cb_new, mainloop);
  gst_object_unref (bus);
#elif defined USE_SIMPLE_WATCH
  gst_bus_add_signal_watch (bus);
  TRACE();
  g_signal_connect (bus, "message", G_CALLBACK (bus_message_cb), mainloop);
  TRACE();
#else
  mBusWatch = gst_bus_create_watch (bus);
  gst_object_unref (bus);
  TRACE();
  g_source_set_callback (mBusWatch, (GSourceFunc) bus_message, mainloop, NULL);
  g_source_attach (mBusWatch, maincontext);
  TRACE();
#endif
  TRACE();
}

static bool step_pipeline(GstPipeline *pipeline)
{
  GstState old_state, new_state, pending;
  GError *gerror;
  gchar *debug;
  GstBus *bus = gst_element_get_bus (GST_ELEMENT(pipeline));
  bool running = true;
  GstMessage *message;

  LOGE("Polling bus...");
  while ((message = gst_bus_poll (bus, GST_MESSAGE_ANY, GST_SECOND))) {
    LOGE("Message callback received %s", GST_MESSAGE_TYPE_NAME(message));
    switch (GST_MESSAGE_TYPE (message)) {
      case GST_MESSAGE_ERROR:
        LOGE ("run_pipeline: ERROR\n");
        seenerrors = true;
        running = false;
        break;
      case GST_MESSAGE_WARNING:
        gst_message_parse_warning (message, &gerror, &debug);
        LOGE ("%s, %s", gerror?gerror->message:"<nil>", debug);
        g_error_free (gerror);
        g_free (debug);
        break;
      case GST_MESSAGE_EOS:
        LOGE ("run_pipeline: Done\n");
         running = false;
        break;
      case GST_MESSAGE_STATE_CHANGED:
        gst_message_parse_state_changed (message, &old_state, &new_state, &pending);
        LOGE ("run_pipeline: STATE_CHANGED for %s: old %s new %s pending %s",
          GST_MESSAGE_SRC(message) ? GST_ELEMENT_NAME(GST_MESSAGE_SRC(message)) : "<unknown>",
          state_names[old_state], state_names[new_state], state_names[pending]);
        break;
      default:
        LOGE("run_pipeline: wut ? %d", GST_MESSAGE_TYPE(message));
        break;
    }
  }
  LOGE("Bus polled");

  return running;
}

static void run_pipeline(GstPipeline *pipeline)
{
  LOGE("Running pipeline");
  while (step_pipeline(pipeline));
  LOGE("Pipeline ended");
}

#ifdef USE_ASYNC
static int gesops_cut_start(/*const*/ char *input_uri, guint64 start_time, guint64 duration, /*const*/ char *output_uri)
#else
static bool gesops_cut_worker(/*const*/ char *input_uri, guint64 start_time, guint64 duration, /*const*/ char *output_uri)
#endif
{
#ifdef USE_TRIVIAL_PIPELINE
  GstElement *fsrc = NULL, *fsink = NULL;
  GstPipeline *pipeline = NULL;
#else
  GstDiscoverer *discoverer = NULL;
  GstDiscovererInfo *info = NULL;
  GstEncodingProfile *prof = NULL;
  GESTimelinePipeline *pipeline = NULL;
  GESTimeline *timeline = NULL;
  GESTimelineLayer *layer = NULL;
  GESTrack *tracka = NULL, *trackv = NULL;
  GESTimelineObject *obj = NULL;
#endif
  GMainLoop *mainloop = NULL;
  GstBus *bus = NULL;
  GMainContext *maincontext = NULL;
#ifdef USE_THREAD
  GThread *thread = NULL;
#endif
  GError *error = NULL;
  bool ret = true;

  seenerrors = false;

#ifdef USE_TRIVIAL_PIPELINE
  TRACE();
  fsrc = gst_element_factory_make("filesrc", NULL);
  fsink = gst_element_factory_make("filesink", NULL);
  TRACE();
  if (!fsrc || ! fsink) {
    LOGE("Failed to create fake src/sink: %p %p",fsrc,fsink);
    goto bail;
  }
  pipeline = (GstPipeline*)gst_pipeline_new ("pipeline");
  gst_bin_add_many (GST_BIN (pipeline), fsrc, fsink, NULL);
  gst_element_link_many (fsrc, fsink, NULL);
  //g_object_set (fsrc, "num-buffers", (gint) 500, NULL);
  g_object_set (fsrc, "location", (const char*) "/sdcard/video.mov", NULL);
  g_object_set (fsink, "location", (const char*) "/sdcard/output.mov", NULL);
#else
  discoverer = gst_discoverer_new (20*GST_SECOND, &error);
  if (!discoverer) {
    LOGE("Failed to create discover");
    goto bail;
  }
  TRACE();
  info = gst_discoverer_discover_uri (discoverer, input_uri, &error);
  if (!info) {
    LOGE("Failed to discover uri %s", input_uri);
    goto bail;
  }
  TRACE();
  prof = gst_encoding_profile_from_discoverer (info);
  TRACE();
  if (!prof)
    goto bail;

  LOGE("Discovered, now creating pipeline");

  /* Create the pipeline */
  timeline = ges_timeline_new();
  if (!timeline) {
    LOGE("Failed to create timeline");
    goto bail;
  }
  TRACE();
  pipeline = ges_timeline_pipeline_new ();
  if (!pipeline) {
    LOGE("Failed to create pipeline");
    goto bail;
  }
  TRACE();

  tracka = ges_track_audio_raw_new ();
  trackv = ges_track_video_raw_new ();
  layer = (GESTimelineLayer *) ges_simple_timeline_layer_new ();
  if (!tracka || !trackv || !layer) {
    LOGE("Failed to create something in there: %p, %p, %p", tracka, trackv, layer);
    goto bail;
  }

  TRACE();
  /* Add the tracks and the layer to the timeline */
  if (!ges_timeline_add_layer (timeline, layer) ||
      !ges_timeline_add_track (timeline, tracka) ||
      !ges_timeline_add_track (timeline, trackv)) {
    LOGE("Failed to add components to the timeline");
    goto bail;
  }
  TRACE();

  obj = GES_TIMELINE_OBJECT (ges_timeline_filesource_new (input_uri));
  g_object_set (obj, "in-point", (guint64) start_time, "duration", (guint64) duration, NULL);
  ges_timeline_layer_add_object (layer, obj);
  TRACE();

  if (!ges_timeline_pipeline_add_timeline (pipeline, timeline)) {
    LOGE("Failed to add timeline to pipeline");
    goto bail;
  }
  TRACE();

  TRACE();
  if (!ges_timeline_pipeline_set_render_settings (pipeline, output_uri, prof))
    goto bail;
  TRACE();
  if (!ges_timeline_pipeline_set_mode (pipeline, TIMELINE_MODE_SMART_RENDER))
    goto bail;
  TRACE();

  {
    GstElement *urisink = gst_bin_get_by_name(GST_BIN(pipeline), "urisink");
    if (urisink) {
      g_object_set(urisink, "async", 0, NULL);
    }
    else {
      LOGE("Could not found element \"urisink\"");
    }
  }
#endif

#ifdef USE_THREAD
  maincontext = g_main_context_new();
#endif
  mainloop = g_main_loop_new (maincontext, FALSE);

#ifndef USE_CUSTOM_LOOP
  TRACE();
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  TRACE();

#ifdef USE_ONE_STATE_CHANGE
  add_bus_watch(bus, maincontext, mainloop);
#endif

#endif

#ifdef USE_THREAD
#ifdef EARLY_THREAD
  thread = g_thread_create((GThreadFunc)threaded_g_main_loop_run, mainloop, TRUE, &error);
  if (!thread) goto bail;
#endif
#endif

#if defined USE_ONE_STATE_CHANGE || defined USE_CUSTOM_LOOP || defined USE_ASYNC
  if (gst_element_set_state (GST_ELEMENT (pipeline), GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
    LOGE ("Failed to start the encoding");
    goto bail;
  }
#else
  set_state(pipeline, GST_STATE_READY, "READY", bus);
  TRACE();

  set_state(pipeline, GST_STATE_PAUSED, "PAUSED", bus);
  TRACE();

  set_state(pipeline, GST_STATE_PLAYING, "PLAYING", bus);
  TRACE();
#endif

#ifndef USE_ASYNC
#ifndef USE_CUSTOM_LOOP
#ifndef USE_ONE_STATE_CHANGE
  add_bus_watch(bus, maincontext, mainloop);
#endif
#endif
#endif

  TRACE();

#ifdef USE_ASYNC

  LOGE("Returning pipeline %p", pipeline);
  return (int)pipeline;

#else

#ifdef USE_CUSTOM_LOOP
  run_pipeline(pipeline);
#else
#ifdef USE_THREAD
#if !defined EARLY_THREAD
  thread = g_thread_create((GThreadFunc)threaded_g_main_loop_run, mainloop, TRUE, &error);
  if (!thread) goto bail;
#endif

  TRACE();
  LOGE("Waiting for thread to finish");
  g_thread_join(thread);
#else
  LOGE("Running glib loop");
  g_main_loop_run (mainloop);
  LOGE("glib loop done");
#endif
#endif

  TRACE();

  set_state(pipeline, GST_STATE_NULL, "NULL", bus);
  TRACE();

#endif

end:
#ifndef USE_TRIVIAL_PIPELINE
  if (prof) gst_encoding_profile_unref (prof);
#endif
  if (pipeline) gst_object_unref (pipeline);
  if (mainloop) g_main_loop_unref (mainloop);
  if (maincontext) g_main_context_unref (maincontext);

  return ret && !seenerrors;

bail:
  ret = false;
  goto end;
}

typedef struct {
  char *input_uri;
  guint64 start_time;
  guint64 duration;
  char *output_uri;

  gboolean ret;
} gesops_cut_args;

static gpointer gesops_cut_worker_thread(gpointer p)
{
//  gesops_cut_args *args = (gesops_cut_args*)p;
//  args->ret = gesops_cut_worker(args->input_uri, args->start_time, args->duration, args->output_uri);
  return NULL;
}

static gboolean gesops_cut(/*const*/ char *input_uri, guint64 start_time, guint64 duration, /*const*/ char *output_uri)
{
  GError *error = NULL;
  gesops_cut_args args = { strdup(input_uri), start_time, duration, strdup(output_uri), false };
  GThread *thread = g_thread_create(gesops_cut_worker_thread, &args, FALSE, &error);
  if (thread) {
    LOGE("Avoiding joining...");
    //g_thread_join(thread);
    return args.ret;
  }
  else {
    LOGE("Failed to create cut worker thread");
    return FALSE;
  }
}

static bool gesops_cut_update(int cookie)
{
  GstPipeline *pipeline = (GstPipeline*)cookie;
  LOGE("Stepping pipeline %p", pipeline);
  bool ret = step_pipeline(pipeline);
  LOGE("step_pipeline returned %d", ret);
  return ret;
}

