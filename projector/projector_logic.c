#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gst/gst.h>
#include "common/gstcontext.h"
#include "common/effects.h"
#include "common/memory.h"


//in frames
const long long int GST_MOVIE_1  = ((long long int)0 * 1000000000 / 30);
const long long int GST_MOVIE_2  = ((long long int)1821 * 1000000000 / 30);
const long long int GST_MOVIE_3  = ((long long int)2720 * 1000000000 / 30);
const long long int GST_MOVIE_4  = ((long long int)4700 * 1000000000 / 30);
const long long int GST_MOVIE_5  = ((long long int)(5556 + 100)* 1000000000 / 30);
const long long int GST_MOVIE_6  = ((long long int)(10781 + 30) * 1000000000 / 30);
const long long int GST_MOVIE_7  = ((long long int)17613 * 1000000000 / 30);
const long long int GST_MOVIE_8  = ((long long int)18364 * 1000000000 / 30);
const long long int GST_MOVIE_9  = ((long long int)19575 * 1000000000 / 30);
const long long int GST_MOVIE_10  = ((long long int)21378 * 1000000000 / 30);
const long long int GST_MOVIE_11  = ((long long int)23178 * 1000000000 / 30);
const long long int GST_MOVIE_12  = ((long long int)25099 * 1000000000 / 30);
const long long int GST_MOVIE_13  = ((long long int)26899 * 1000000000 / 30);
const long long int GST_MOVIE_14  = ((long long int)29411 * 1000000000 / 30);
const long long int GST_MOVIE_15  = ((long long int)31211 * 1000000000 / 30);
const long long int GST_MOVIE_16  = ((long long int)31813 * 1000000000 / 30);
const long long int GST_MOVIE_17  = ((long long int)33734 * 1000000000 / 30);
const long long int GST_MOVIE_18  = ((long long int)35534 * 1000000000 / 30);
const long long int GST_MOVIE_19  = ((long long int)37501 * 1000000000 / 30);
const long long int GST_MOVIE_20  = ((long long int)39307 * 1000000000 / 30);
const long long int GST_MOVIE_21  = ((long long int)41228 * 1000000000 / 30);
const long long int GST_MOVIE_22  = ((long long int)43149 * 1000000000 / 30);
const long long int GST_MOVIE_23  = ((long long int)45080 * 1000000000 / 30);
const long long int GST_MOVIE_24  = ((long long int)47011 * 1000000000 / 30);
const long long int GST_MOVIE_25  = ((long long int)48932 * 1000000000 / 30);
const long long int GST_MOVIE_26  = ((long long int)50974 * 1000000000 / 30);
const long long int GST_MOVIE_27  = ((long long int)52655 * 1000000000 / 30);
const long long int GST_MOVIE_28  = ((long long int)54455 * 1000000000 / 30);
const long long int GST_MOVIE_29  = ((long long int)56255 * 1000000000 / 30);
const long long int GST_MOVIE_30  = ((long long int)58055 * 1000000000 / 30);
const long long int GST_MOVIE_31  = ((long long int)59861 * 1000000000 / 30);
const long long int GST_MOVIE_32  = ((long long int)61663 * 1000000000 / 30);
const long long int GST_MOVIE_33  = ((long long int)63405 * 1000000000 / 30);
const long long int GST_MOVIE_34  = ((long long int)65405 * 1000000000 / 30);
const long long int GST_MOVIE_35  = ((long long int)(67752 +28) * 1000000000 / 30);
const long long int GST_MOVIE_36  = ((long long int)70814 * 1000000000 / 30);
const long long int GST_MOVIE_37  = ((long long int)74025 * 1000000000 / 30);
const long long int GST_MOVIE_38  = ((long long int)80387 * 1000000000 / 30);
const long long int GST_MOVIE_39  = ((long long int)81624 * 1000000000 / 30);
const long long int GST_MOVIE_40  = ((long long int)86184 * 1000000000 / 30);  // the end, not a movie


const long long int GST_MOVIE_OFFSET_START  = ((long long int) 0 * 1000000000 / 30);
const long long int GST_MOVIE_OFFSET_END  = ((long long int) 2 * 1000000000 / 30);

const long long int movie_start_times[GST_MOVIE_LAST - GST_MOVIE_FIRST +1]  = { 
GST_MOVIE_1 + GST_MOVIE_OFFSET_START,
GST_MOVIE_2 + GST_MOVIE_OFFSET_START,
GST_MOVIE_3 + GST_MOVIE_OFFSET_START,
GST_MOVIE_4 + GST_MOVIE_OFFSET_START,
GST_MOVIE_5 + GST_MOVIE_OFFSET_START ,
GST_MOVIE_6 + GST_MOVIE_OFFSET_START,
GST_MOVIE_7 + GST_MOVIE_OFFSET_START,
GST_MOVIE_8 + GST_MOVIE_OFFSET_START,
GST_MOVIE_9 + GST_MOVIE_OFFSET_START,
GST_MOVIE_10 + GST_MOVIE_OFFSET_START,
GST_MOVIE_11 + GST_MOVIE_OFFSET_START,
GST_MOVIE_12 + GST_MOVIE_OFFSET_START,
GST_MOVIE_13 + GST_MOVIE_OFFSET_START,
GST_MOVIE_14 + GST_MOVIE_OFFSET_START,
GST_MOVIE_15 + GST_MOVIE_OFFSET_START,
GST_MOVIE_16 + GST_MOVIE_OFFSET_START,
GST_MOVIE_17 + GST_MOVIE_OFFSET_START,
GST_MOVIE_18 + GST_MOVIE_OFFSET_START,
GST_MOVIE_19 + GST_MOVIE_OFFSET_START,
GST_MOVIE_20 + GST_MOVIE_OFFSET_START,
GST_MOVIE_21 + GST_MOVIE_OFFSET_START,
GST_MOVIE_22 + GST_MOVIE_OFFSET_START,
GST_MOVIE_23 + GST_MOVIE_OFFSET_START,
GST_MOVIE_24 + GST_MOVIE_OFFSET_START,
GST_MOVIE_25 + GST_MOVIE_OFFSET_START,
GST_MOVIE_26 + GST_MOVIE_OFFSET_START,
GST_MOVIE_27 + GST_MOVIE_OFFSET_START,
GST_MOVIE_28 + GST_MOVIE_OFFSET_START,
GST_MOVIE_29 + GST_MOVIE_OFFSET_START,
GST_MOVIE_30 + GST_MOVIE_OFFSET_START,
GST_MOVIE_31 + GST_MOVIE_OFFSET_START,
GST_MOVIE_32 + GST_MOVIE_OFFSET_START,
GST_MOVIE_33 + GST_MOVIE_OFFSET_START,
GST_MOVIE_34 + GST_MOVIE_OFFSET_START,
GST_MOVIE_35 + GST_MOVIE_OFFSET_START,
GST_MOVIE_36 + GST_MOVIE_OFFSET_START,
GST_MOVIE_37 + GST_MOVIE_OFFSET_START,
GST_MOVIE_38 + GST_MOVIE_OFFSET_START,
GST_MOVIE_39 + GST_MOVIE_OFFSET_START };

const long long int movie_end_times[GST_MOVIE_LAST - GST_MOVIE_FIRST +1 ]   = { 
GST_MOVIE_2 - GST_MOVIE_OFFSET_END,
GST_MOVIE_3 - GST_MOVIE_OFFSET_END,
GST_MOVIE_4 - GST_MOVIE_OFFSET_END,
GST_MOVIE_5 - GST_MOVIE_OFFSET_END- ((long long int)(100)* 1000000000 / 30),
GST_MOVIE_6 - GST_MOVIE_OFFSET_END- ((long long int)(30)* 1000000000 / 30),
GST_MOVIE_7 - GST_MOVIE_OFFSET_END,
GST_MOVIE_8 - GST_MOVIE_OFFSET_END,
GST_MOVIE_9 - GST_MOVIE_OFFSET_END,
GST_MOVIE_10 - GST_MOVIE_OFFSET_END,
GST_MOVIE_11 - GST_MOVIE_OFFSET_END,
GST_MOVIE_12 - GST_MOVIE_OFFSET_END,
GST_MOVIE_13 - GST_MOVIE_OFFSET_END,
GST_MOVIE_14 - GST_MOVIE_OFFSET_END,
GST_MOVIE_15 - GST_MOVIE_OFFSET_END,
GST_MOVIE_16 - GST_MOVIE_OFFSET_END,
GST_MOVIE_17 - GST_MOVIE_OFFSET_END,
GST_MOVIE_18 - GST_MOVIE_OFFSET_END,
GST_MOVIE_19 - GST_MOVIE_OFFSET_END,
GST_MOVIE_20 - GST_MOVIE_OFFSET_END,
GST_MOVIE_21 - GST_MOVIE_OFFSET_END,
GST_MOVIE_22 - GST_MOVIE_OFFSET_END,
GST_MOVIE_23 - GST_MOVIE_OFFSET_END,
GST_MOVIE_24 - GST_MOVIE_OFFSET_END,
GST_MOVIE_25 - GST_MOVIE_OFFSET_END,
GST_MOVIE_26 - GST_MOVIE_OFFSET_END,
GST_MOVIE_27 - GST_MOVIE_OFFSET_END,
GST_MOVIE_28 - GST_MOVIE_OFFSET_END,
GST_MOVIE_29 - GST_MOVIE_OFFSET_END,
GST_MOVIE_30 - GST_MOVIE_OFFSET_END,
GST_MOVIE_31 - GST_MOVIE_OFFSET_END,
GST_MOVIE_32 - GST_MOVIE_OFFSET_END,
GST_MOVIE_33 - GST_MOVIE_OFFSET_END,
GST_MOVIE_34 - GST_MOVIE_OFFSET_END,
GST_MOVIE_35 - GST_MOVIE_OFFSET_END- ((long long int)(28)* 1000000000 / 30),
GST_MOVIE_36 - GST_MOVIE_OFFSET_END,
GST_MOVIE_37 - GST_MOVIE_OFFSET_END,
GST_MOVIE_38 - GST_MOVIE_OFFSET_END,
GST_MOVIE_39 - GST_MOVIE_OFFSET_END,
GST_MOVIE_40 - GST_MOVIE_OFFSET_END  };


//outgoing flag
static volatile bool *video_done_flag = NULL;  //points to shared struct (if set)

GstPipeline *pipeline[GST_MOVIE_LAST + 1],*pipeline_active;


static void seek_to_time(gint64 time_nanoseconds,GstPipeline **pipeline)
{
	if (!gst_element_seek ( GST_ELEMENT(*pipeline), 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH, GST_SEEK_TYPE_SET, time_nanoseconds, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) {
		g_print("Seek failed!\n");
	}
}
gint64 get_position(){
	gint64 pos;
	bool result = gst_element_query_position ( GST_ELEMENT(pipeline[GST_MOVIE_FIRST]), GST_FORMAT_TIME, &pos);
	if (!result) pos = -1;
	return pos;
}



int projector_logic_update(int video_mode_requested) {
	if (GST_IS_ELEMENT(pipeline_active)){
		gstcontext_handle_bus(&pipeline_active); 
	}
	
	static int video_mode_current = -1;
	static bool movie_not_eof = false;
	
	if (video_mode_current >= GST_MOVIE_FIRST && video_mode_current <= GST_MOVIE_LAST && movie_not_eof){
		if ( get_position() >= movie_end_times[video_mode_current -GST_MOVIE_FIRST]   )  {
			*video_done_flag = true;
			printf("EOF by Time\n");
			movie_not_eof = false;
		}
		
	}
	
	if (video_mode_requested > GST_MOVIE_LAST){
		printf("NON EXISTANT PIPELINE %d REQUESTED!\n", video_mode_requested);
		video_mode_requested = 2;
	}

	if (pipeline[video_mode_requested] == NULL && video_mode_requested <GST_MOVIE_FIRST){
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
		
		if (video_mode_requested >= GST_MOVIE_FIRST && video_mode_requested <= GST_MOVIE_LAST) {
			pipeline_active = pipeline[GST_MOVIE_FIRST];
	
			seek_to_time( movie_start_times[video_mode_requested -GST_MOVIE_FIRST],&pipeline_active);
			movie_not_eof = true;
		}else{
			
			pipeline_active = pipeline[video_mode_requested];
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
	gstcontext_load_pipeline(GST_BLANK,             &pipeline[GST_BLANK],               GST_STATE_NULL,"videotestsrc pattern=2 ! video/x-raw,width=640,height=480,framerate=(fraction)30/1 ! queue ! glupload ! glcolorconvert ! glcolorscale  ! video/x-raw(memory:GLMemory),width=1280,height=720,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true");
	gstcontext_load_pipeline(GST_VIDEOTESTSRC,      &pipeline[GST_VIDEOTESTSRC],        GST_STATE_NULL,"videotestsrc  ! video/x-raw,width=640,height=480,framerate=(fraction)30/1 ! queue ! glupload ! glcolorconvert ! glcolorscale  !  video/x-raw(memory:GLMemory),width=1280,height=720,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true");
	gstcontext_load_pipeline(GST_VIDEOTESTSRC_CUBED,&pipeline[GST_VIDEOTESTSRC_CUBED],  GST_STATE_NULL,"videotestsrc ! video/x-raw,width=640,height=480,framerate=(fraction)30/1 ! queue ! glupload ! glfiltercube ! glcolorconvert ! glcolorscale  ! video/x-raw(memory:GLMemory),width=1280,height=720,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true");

	//wave must load and play a frame before synae loads

	gstcontext_load_pipeline(GST_LIBVISUAL_WAVE_RADIO,    &pipeline[GST_LIBVISUAL_WAVE_RADIO]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert !libvisual_jakdaw                                      ! video/x-raw,width=400,height=320,framerate=60/1 ! cacatv     ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");

	gstcontext_load_pipeline(GST_LIBVISUAL_INFINITE,&pipeline[GST_LIBVISUAL_INFINITE]   ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! volume volume=0.5 ! libvisual_infinite                ! video/x-raw,width=320,height=240,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_JAKDAW,  &pipeline[GST_LIBVISUAL_JAKDAW]     ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! libvisual_jakdaw                                      ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_OINKSIE, &pipeline[GST_LIBVISUAL_OINKSIE]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! libvisual_oinksie                                     ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_GOOM,              &pipeline[GST_GOOM]                 ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! goom                                                  ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_GOOM2K1,           &pipeline[GST_GOOM2K1]              ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! volume volume=1.5 ! goom2k1                           ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_WAVE,    &pipeline[GST_LIBVISUAL_WAVE]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! wavescope style=3                                     ! video/x-raw,width=400,height=320,framerate=60/1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_JAKDAWV,  &pipeline[GST_LIBVISUAL_JAKDAWV]     ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! libvisual_jakdaw                                      ! video/x-raw,width=240,height=320,framerate=60/1 ! videoflip video-direction=1 ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");
	gstcontext_load_pipeline(GST_LIBVISUAL_WAVEV,    &pipeline[GST_LIBVISUAL_WAVEV]    ,GST_STATE_NULL,"udpsrc port=5000 caps=application/x-rtp,channels=1,clock-rate=44100 retrieve-sender-address=false ! rtpL24depay ! queue max-size-time=10000000 leaky=downstream ! audioconvert ! wavescope style=3                                     ! video/x-raw,width=240,height=320,framerate=60/1  ! videoflip video-direction=1  ! glupload ! glcolorconvert ! glcolorscale ! video/x-raw(memory:GLMemory),width=1280,height=720 ! glfilterapp name=grabtexture ! fakesink sync=true async=false");


	char launch[1000];	
	sprintf(launch, "filesrc location=/home/pi/assets/all.mp4 ! queue ! qtdemux   ! h264parse ! v4l2h264dec output-io-mode=4 ! glupload ! glcolorconvert ! glcolorscale   ! video/x-raw(memory:GLMemory),width=1280,height=720,format=RGBA ! glfilterapp name=grabtexture ! fakesink sync=true async=false ");
	gstcontext_load_pipeline(GST_MOVIE_FIRST,&pipeline[GST_MOVIE_FIRST],GST_STATE_PAUSED,launch);

}