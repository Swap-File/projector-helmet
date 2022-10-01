#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include "common/memory.h"
#include "common/opengl.h"
#include "common/esUtil.h"
#include "common/gstcontext.h"
#include "common/effects.h"

#include "projector_logic.h"
#include "projector_scene.h"
#include "png.h"

//struct gun_struct *this_gun;
static volatile GLint gstcontext_texture_id; //passed to gstcontext
static volatile bool gstcontext_texture_fresh; //passed to gstcontext

static struct egl egl;

static GLuint basic_program,basic_u_mvpMatrix,basic_in_Position,basic_in_TexCoord,basic_u_Texture;
static GLuint helmet_mask;

#define VIDEO_WIDTH 1280
#define VIDEO_HEIGHT 720

extern bool video_done_early;

static GLuint vbo;  // for vVertices & vTexCoords

static const GLfloat vVertices[] = {
	// video
	-VIDEO_WIDTH/2, -VIDEO_HEIGHT/2, 0.0f,
	+VIDEO_WIDTH/2, -VIDEO_HEIGHT/2, 0.0f,
	-VIDEO_WIDTH/2, +VIDEO_HEIGHT/2, 0.0f,
	+VIDEO_WIDTH/2, +VIDEO_HEIGHT/2, 0.0f,
	// puddle
	-VIDEO_WIDTH/2, -VIDEO_HEIGHT/2, 0.0f,
	+VIDEO_WIDTH/2, -VIDEO_HEIGHT/2, 0.0f,
	-VIDEO_WIDTH/2, +VIDEO_HEIGHT/2, 0.0f,
	+VIDEO_WIDTH/2, +VIDEO_HEIGHT/2, 0.0f,
	// portal
	-VIDEO_WIDTH/2, -VIDEO_HEIGHT/2, 0.0f,
	+VIDEO_WIDTH/2, -VIDEO_HEIGHT/2, 0.0f,
	-VIDEO_WIDTH/2, +VIDEO_HEIGHT/2, 0.0f,
	+VIDEO_WIDTH/2, +VIDEO_HEIGHT/2, 0.0f,
};

static GLfloat vTexCoords[] = {
	// video
	0.0f, 1.0f,
	1.0f, 1.0f,   
	0.0f, 0.0f,
	1.0f, 0.0f,

	// puddle
		1.0f, 0.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	// portal
	1.0f, 1.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 0.0f,
};




volatile bool video_done = false;

bool auto_advance = false;
bool auto_swap = false;


int gst_state = GST_VIDEOTESTSRC_CUBED;
int movie_index = GST_MOVIE_FIRST;
int audio_index = GST_LIBVISUAL_FIRST;

	static uint32_t adv_time = 0;

void next_or_previous(int type){
	
	int maximum = GST_VIDEOTESTSRC_CUBED;
	int minimum = GST_VIDEOTESTSRC_CUBED;
	
	if (gst_state >= GST_LIBVISUAL_FIRST && gst_state <= GST_LIBVISUAL_LAST){
		maximum = GST_LIBVISUAL_LAST;
		minimum = GST_LIBVISUAL_FIRST;
	}
	if (gst_state >= GST_MOVIE_FIRST && gst_state <= GST_MOVIE_LAST){
		maximum = GST_MOVIE_LAST;
		minimum = GST_MOVIE_FIRST;	
	}
	
	if ( type){
		gst_state++;
		if (gst_state > maximum)
		gst_state = minimum;
	}else{
		gst_state--;
		if (gst_state < minimum)
		gst_state = maximum;
		
	}
	adv_time = millis();
}

static void projector_scene_draw(unsigned i,char *debug_msg)
{
	
	
	
	if (video_done){
		video_done = false;
		gst_state = 0; /// TO DO auto next mode
	}
	

	if (auto_advance){
		if (millis() - adv_time > 10000){
			next_or_previous(true);
			
		}		
	}
	
	
	if (debug_msg[0] != '\0') {
		int temp[3];
		int result = sscanf(debug_msg,"%d", &temp[0]);
		if (result == 1) {
			printf("\nDebug Message Parsed: Setting gst_state: %d \n",temp[0]);
			
			if (temp[0] == 100){  //Trigger + back button
				
				if (gst_state >= GST_LIBVISUAL_FIRST && gst_state <= GST_LIBVISUAL_LAST){
					//TODO special mode when already in mode
				}else{
					gst_state = audio_index;
				}
			}else if (temp[0] == 101){  //plus button
				next_or_previous(true);
				
			}else if (temp[0] == 102){  //minus button
				next_or_previous(false);	
			}else if (temp[0] == 120){  //big button
						next_or_previous(true);
			adv_time = millis();
			}else if (temp[0] == 106){  //Trigger + minus button
				gst_state = 0;
			}else if (temp[0] == 108){  //Trigger + home button
			
						if (gst_state != 0){
				auto_swap = true;
				} else {
					gst_state = movie_index;
				}
				
			}else if (temp[0] == 107){  //Trigger + back button
				if (gst_state != 0){
				auto_swap = false;
				} else {
					gst_state = audio_index;
				}
				
			}else{
				
				gst_state = temp[0];
				
			}
			
		}
	}
	
	if (gst_state == 0){
		movie_index = GST_MOVIE_FIRST;
		audio_index = GST_LIBVISUAL_FIRST;
	}
	
	//save indexes
	if (gst_state >= GST_LIBVISUAL_FIRST && gst_state <= GST_LIBVISUAL_LAST){
		audio_index = gst_state;
	}
	if (gst_state >= GST_MOVIE_FIRST && gst_state <= GST_MOVIE_LAST){
		movie_index = gst_state;
	}
	
	bool frameskip = false;
	uint32_t start_time = millis();
	while (gstcontext_texture_fresh == false) {
		usleep(10);
		if (millis() - start_time > 2) {
			frameskip = true;
			break;
		}
	}
	gstcontext_texture_fresh = false;
	
	
	gst_state = projector_logic_update(gst_state);
	
	uint32_t render_start_time = micros();

	glClearColor(0.0, 0.0, 0.0, 0.3);

	glClear(GL_COLOR_BUFFER_BIT );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ESMatrix modelviewprojection;
	esMatrixLoadIdentity(&modelviewprojection);
	esOrtho(&modelviewprojection, -VIDEO_WIDTH/2, VIDEO_WIDTH/2,-VIDEO_HEIGHT/2, VIDEO_HEIGHT/2, -1,1);

	//this is the polygon that displays the gstreamer texture
	glUseProgram(basic_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,gstcontext_texture_id);
	glUniform1i(basic_u_Texture, 0); /* '0' refers to texture unit 0. */
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(basic_in_Position);
	glVertexAttribPointer(basic_in_Position, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)0);
	glEnableVertexAttribArray(basic_in_TexCoord);
	glVertexAttribPointer(basic_in_TexCoord, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)sizeof(vVertices));
	glUniformMatrix4fv(basic_u_mvpMatrix, 1, GL_FALSE, &modelviewprojection.m[0][0]);

	/* Draw */
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//this is the polygon that displays the gstreamer texture
	glBindTexture(GL_TEXTURE_2D,helmet_mask);
	glUniform1i(basic_u_Texture, 0); /* '0' refers to texture unit 0. */
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(basic_in_Position);
	glVertexAttribPointer(basic_in_Position, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)0);
	glEnableVertexAttribArray(basic_in_TexCoord);
	glVertexAttribPointer(basic_in_TexCoord, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid *)(intptr_t)sizeof(vVertices));
	glUniformMatrix4fv(basic_u_mvpMatrix, 1, GL_FALSE, &modelviewprojection.m[0][0]);

	/* Draw Masking */
	  glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
	
	/* FPS Counter */
	glFinish();
	fps_counter("Projector: ",render_start_time,frameskip);
}


const struct egl * scene_init(const struct gbm *gbm, int samples)
{

	int ret = init_egl(&egl, gbm, samples);
	if (ret)
	return NULL;

	if (egl_check(&egl, eglCreateImageKHR) ||
			egl_check(&egl, glEGLImageTargetTexture2DOES) ||
			egl_check(&egl, eglDestroyImageKHR))
	return NULL;

	basic_program = create_program_from_disk("/home/pi/projector/common/basic.vert", "/home/pi/projector/common/basic.frag");
	link_program(basic_program);
	basic_u_mvpMatrix = glGetUniformLocation(basic_program, "u_mvpMatrix");
	basic_in_Position = glGetAttribLocation(basic_program, "in_Position");
	basic_in_TexCoord = glGetAttribLocation(basic_program, "in_TexCoord");
	basic_u_Texture = glGetAttribLocation(basic_program, "u_Texture");

	//upload data for video, puddle, and portal
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices) + sizeof(vTexCoords), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vVertices), &vVertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(vVertices), sizeof(vTexCoords), &vTexCoords[0]);

	helmet_mask = png_load("/home/pi/projector/mask.png", NULL, NULL);

	//fire up gstreamer
	gstcontext_init(egl.display, egl.context, &gstcontext_texture_id, &gstcontext_texture_fresh, &video_done);
	projector_logic_init(&video_done);

	egl.draw = projector_scene_draw;

	return &egl;
}
