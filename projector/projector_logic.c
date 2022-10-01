#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gst/gst.h>
#include "common/gstcontext.h"
#include "common/effects.h"
#include "common/memory.h"

//outgoing flag
static volatile bool *video_done_flag = NULL;  //points to shared struct (if set)

GstPipeline *pipeline[GST_MOVIE_LAST + 1],*pipeline_active;


static void seek_to_time(gint64 time_nanoseconds,GstPipeline **pipeline)
{
	if (!gst_element_seek ( GST_ELEMENT(*pipeline), 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, time_nanoseconds, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
		g_print("Seek failed!\n");
	}
}

int projector_logic_update(int video_mode_requested) {
	if (GST_IS_ELEMENT(pipeline_active)){
		gstcontext_handle_bus(&pipeline_active); 
	}
	
	static int video_mode_current = -1;

	if (video_mode_requested > GST_MOVIE_LAST){
		printf("NON EXISTANT PIPELINE %d REQUESTED!\n", video_mode_requested);
		video_mode_requested = 2;
	}

	if (pipeline[video_mode_requested] == NULL){
		printf("NON EXISTANT PIPELINE %d REQUESTED!\n", video_mode_requested);
		video_mode_requested = 2;
	}
	
	//always immediately switch to new pipelines to be ready
	if (video_mode_requested != video_mode_current){
		uint32_t start_time = millis();

		//stop the old pipeline

		if (GST_IS_ELEMENT(pipeline_active)) {
			if (video_mode_current >= GST_MOVIE_FIRST && video_mode_current <= GST_MOVIE_LAST) {
				printf("Projector: Pause & Rewind Video Pipeline!\n");

				gst_element_set_state (GST_ELEMENT (pipeline_active), GST_STATE_PAUSED);
				
			} else {
				gst_element_set_state (GST_ELEMENT (pipeline_active), GST_STATE_NULL);
				printf("Projector: Null-ing Pipeline!\n");
			}
		}

		pipeline_active = pipeline[video_mode_requested];



		if (video_mode_requested >= GST_MOVIE_FIRST && video_mode_requested <= GST_MOVIE_LAST) {
			seek_to_time(0,&pipeline_active);
		}
		
		gst_element_set_state (GST_ELEMENT (pipeline_active), GST_STATE_PLAYING);
		
		video_mode_current = video_mode_requested;

		printf("Pipeline changed to %d in %d milliseconds!\n",video_mode_current,millis() - start_time);
	}
	return video_mode_current;
}

void projector_logic_init(volatile bool *video_done_flag_p) {

	video_done_flag = video_done_flag_p;
	
	//spawn audio udp loopback to prevent re-init issues (bringing it back from portal gun v1.0)
	system("gst-launch-1.0 --no-position alsasrc buffer-time=10000 device=hw:2 ! audio/x-raw,layout=interleaved,rate=44100,format=S16LE,channels=1 ! queue !  audioconvert ! rtpL24pay ! udpsink host=127.0.0.1 port=5000 sync=false &");

	
	//test patterns
	gstcontext_load_pipeline(GST_BLANK,             &pipeline[GST_BLANK],               GST_STATE_NULL,"videotestsrc pattern=24 ! video/x-raw,width=640,height=480,framerate=(fraction)30/1 ! queue ! glupload ! video/x-raw(memory:GLMemory),width=640,height=480,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true");
	gstcontext_load_pipeline(GST_VIDEOTESTSRC,      &pipeline[GST_VIDEOTESTSRC],        GST_STATE_NULL,"videotestsrc pattern=10 ! video/x-raw,width=640,height=480,framerate=(fraction)30/1 ! queue ! glupload ! video/x-raw(memory:GLMemory),width=640,height=480,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true");
	gstcontext_load_pipeline(GST_VIDEOTESTSRC_CUBED,&pipeline[GST_VIDEOTESTSRC_CUBED],  GST_STATE_NULL,"videotestsrc ! video/x-raw,width=640,height=480,framerate=(fraction)30/1 ! queue ! glupload ! glfiltercube ! video/x-raw(memory:GLMemory),width=640,height=480,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true");

	//wave must load and play a frame before synae loads

	gstcontext_load_pipeline(GST_LIBVISUAL_WAVE_RADIO,    &pipeline[GST_LIBVISUAL_WAVE_RADIO]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert !libvisual_jakdaw                                      ! video/x-raw,width=400,height=320,framerate=60/1 ! cacatv     ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");

	gstcontext_load_pipeline(GST_LIBVISUAL_INFINITE,&pipeline[GST_LIBVISUAL_INFINITE]   ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! volume volume=0.5 ! libvisual_infinite                ! video/x-raw,width=320,height=240,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_JAKDAW,  &pipeline[GST_LIBVISUAL_JAKDAW]     ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! libvisual_jakdaw                                      ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_OINKSIE, &pipeline[GST_LIBVISUAL_OINKSIE]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! libvisual_oinksie                                     ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_GOOM,              &pipeline[GST_GOOM]                 ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! goom                                                  ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_GOOM2K1,           &pipeline[GST_GOOM2K1]              ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! volume volume=1.5 ! goom2k1                           ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_WAVE,    &pipeline[GST_LIBVISUAL_WAVE]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! wavescope style=3                                     ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_JAKDAWV,  &pipeline[GST_LIBVISUAL_JAKDAWV]     ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! libvisual_jakdaw                                      ! video/x-raw,width=240,height=320,framerate=60/1 ! videoflip video-direction=1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_WAVEV,    &pipeline[GST_LIBVISUAL_WAVEV]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! wavescope style=3                                     ! video/x-raw,width=240,height=320,framerate=60/1  ! videoflip video-direction=1  ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=640,height=480 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");


	for (int slot = GST_MOVIE_FIRST; slot <= GST_MOVIE_LAST; slot++){
		char launch[1000];
		static int i = 1;
		sprintf(launch, "filesrc location=/home/pi/assets/movies/down/%d.mp4 ! queue ! qtdemux   ! queue ! avdec_h264 ! queue ! glupload ! glcolorconvert ! glcolorscale  ! video/x-raw(memory:GLMemory),width=640,height=480,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true async=true ",i++);
		gstcontext_load_pipeline(slot,&pipeline[slot],GST_STATE_PAUSED,launch);
	}
}