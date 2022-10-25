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

bool button_indicated = false;

static GLuint basic_program,basic_u_mvpMatrix,basic_in_Position,basic_in_TexCoord,basic_u_Texture;
static GLuint helmet_mask,helmet_mask2;

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
extern bool loop_it;
uint32_t auto_advance_time = 0;

bool auto_advance = true;
bool auto_change_category = false;

int mode = 0;

int eighties_index = 50;  // 50 to 57   8 items    id 2
int eighties_index_qty = GST_MOVIE_80s_LAST - GST_MOVIE_80s_FIRST + 1;

int fast_index = 58;   // 58 to 67   10 items    id 3
int fast_index_qty = GST_MOVIE_FAST_LAST - GST_MOVIE_FAST_FIRST + 1;

int slow_index = 68;   //68 to 80   13 items   id4 
int slow_index_qty = GST_MOVIE_SLOW_LAST - GST_MOVIE_SLOW_FIRST + 1;

int other_index = 81;  //81 to 88   8 items    id 1
int other_index_qty = GST_MOVIE_OTHER_LAST - GST_MOVIE_OTHER_FIRST + 1;

int audio_index = 6;    // id 5
int audio_index_qty = GST_LIBVISUAL_LAST - GST_LIBVISUAL_FIRST + 1;


int gst_state = GST_VIDEOTESTSRC_CUBED;

void set_mode(int direction, int count){
	if (count > 0){
		count--;
		if (direction == 1){
			gst_state = GST_MOVIE_OTHER_FIRST + (count % other_index_qty);
			other_index = gst_state;
		}
		if (direction == 2){
			gst_state = GST_MOVIE_80s_FIRST + ( count % eighties_index_qty);
			eighties_index = gst_state;
		}
		if (direction ==3){
			gst_state = GST_MOVIE_FAST_FIRST + (count % fast_index_qty);
			fast_index = gst_state;
		}
		if (direction == 4){
			gst_state = GST_MOVIE_SLOW_FIRST+(count % slow_index_qty);
			slow_index = gst_state;
		}
		if (direction == 5){
			gst_state = GST_LIBVISUAL_FIRST + (count % audio_index_qty);
			audio_index = gst_state;
		}	
	}else{
		if (direction == 1){
			gst_state = other_index;
			other_index = GST_MOVIE_OTHER_FIRST + ((other_index - GST_MOVIE_OTHER_FIRST + 1) % other_index_qty);
		}
		if (direction == 2){
			gst_state = eighties_index;
			eighties_index = GST_MOVIE_80s_FIRST + ((eighties_index - GST_MOVIE_80s_FIRST + 1) % eighties_index_qty);
		}
		if (direction ==3){
			gst_state = fast_index;
			fast_index = GST_MOVIE_FAST_FIRST + ((fast_index - GST_MOVIE_FAST_FIRST + 1) % fast_index_qty);
		}
		if (direction == 4){
			gst_state = slow_index;
			slow_index = GST_MOVIE_SLOW_FIRST + ((slow_index - GST_MOVIE_SLOW_FIRST + 1) % slow_index_qty);
		}
		if (direction == 5){
			gst_state = audio_index;
			audio_index = GST_LIBVISUAL_FIRST + ((audio_index - GST_LIBVISUAL_FIRST + 1) % audio_index_qty);
		}	
	}
	

}
void plus_minus(int count){
	if (gst_state >= GST_MOVIE_80s_FIRST && gst_state <= GST_MOVIE_80s_LAST){
		int current = (gst_state - GST_MOVIE_80s_FIRST);
		gst_state = GST_MOVIE_80s_FIRST + ((current + eighties_index_qty + count) % eighties_index_qty);
		eighties_index = gst_state;
	}
	if (gst_state >= GST_MOVIE_FAST_FIRST && gst_state <= GST_MOVIE_FAST_LAST){
		int current = (gst_state - GST_MOVIE_FAST_FIRST);
		gst_state = GST_MOVIE_FAST_FIRST + ((current + fast_index_qty + count) % fast_index_qty);
		fast_index = gst_state;
	}
	if (gst_state >= GST_MOVIE_SLOW_FIRST && gst_state <= GST_MOVIE_SLOW_LAST){
		int current = (gst_state - GST_MOVIE_SLOW_FIRST);
		gst_state = GST_MOVIE_SLOW_FIRST + ((current + slow_index_qty + count) % slow_index_qty);
		slow_index = gst_state;
	}
	if (gst_state >= GST_LIBVISUAL_FIRST && gst_state <= GST_LIBVISUAL_LAST){
		int current = (gst_state - GST_LIBVISUAL_FIRST);
		gst_state = GST_LIBVISUAL_FIRST + ((current + audio_index_qty + count) % audio_index_qty);
		audio_index = gst_state;
	}
	if (gst_state >= GST_MOVIE_OTHER_FIRST && gst_state <= GST_MOVIE_OTHER_LAST){
		int current = (gst_state - GST_MOVIE_OTHER_FIRST);
		gst_state = GST_MOVIE_OTHER_FIRST + ((current + other_index_qty + count) % other_index_qty);
		other_index = gst_state;
	}
	
	
}

void auto_next(void){
	if (gst_state >= GST_MOVIE_80s_FIRST && gst_state <= GST_MOVIE_80s_LAST){ //2
		set_mode(3,0);
	}
	else if (gst_state >= GST_MOVIE_FAST_FIRST && gst_state <= GST_MOVIE_FAST_LAST){ //3
		set_mode(4,0);
	}
	else if (gst_state >= GST_MOVIE_SLOW_FIRST && gst_state <= GST_MOVIE_SLOW_LAST){ //4
		set_mode(2,0);
	}
	else if (gst_state >= GST_LIBVISUAL_FIRST && gst_state <= GST_LIBVISUAL_LAST){ //5
		set_mode(5,0);
	}
	else if (gst_state >= GST_MOVIE_OTHER_FIRST && gst_state <= GST_MOVIE_OTHER_LAST){ //1
		set_mode(1,0);
	}
}

static void projector_scene_draw(unsigned i,char *debug_msg)
{
	if (video_done){
		video_done = false;
		auto_next();
	}
	
	if (auto_advance_time + 10000 < millis()){  //add a button to extend a mode time
		if (loop_it == false && auto_advance == true){
			auto_next();
		}
	}
	
	if (debug_msg[0] != '\0') {
		int temp[3];
		int result = sscanf(debug_msg,"%d %d", &temp[0], &temp[1]);
		if (result == 2) {
			if (temp[0] >= 1 && temp[0] <= 5){
				set_mode ( temp[0], temp[1]);
				if (temp[1] > 0){
					auto_advance = false;
				}
			}
		    if (temp[0] == 6){
				plus_minus((temp[1] + 1));
			}
			 if (temp[0] == 7){
				plus_minus(-1 * (temp[1] + 1));
			}
		}
		
		
		if (result == 1) {
			printf("\nDebug Message Parsed: Setting gst_state: %d \n",temp[0]);
			if (temp[0] == 200){
				button_indicated = true;
			}else if (temp[0] == 201){
				button_indicated = false;
			}else if (temp[0] == 202){
				gst_state = 0;
			}else if (temp[0] == 205){
				auto_next();
				auto_advance = true;
				
			}else if (temp[0] == 206){
				loop_it = true;
				auto_advance = false;
			}else if (temp[0] == 207){
				loop_it = false;
				auto_advance = false;
			}else if (temp[0] == 208){
				auto_advance_time = millis() + 600000;
			}else{
				gst_state = temp[0];			
			}
		}
	}
	
	static int previous_gst_state = -1;
	
	if (previous_gst_state != gst_state){
		loop_it = false;
		auto_advance_time = millis();
		previous_gst_state = gst_state;
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
	if(button_indicated == false){
		glBindTexture(GL_TEXTURE_2D,helmet_mask);
	}else{
		glBindTexture(GL_TEXTURE_2D,helmet_mask2);
	}

	
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
	helmet_mask2 = png_load("/home/pi/projector/mask2.png", NULL, NULL);
	//fire up gstreamer
	gstcontext_init(egl.display, egl.context, &gstcontext_texture_id, &gstcontext_texture_fresh, &video_done);
	projector_logic_init(&video_done);

	egl.draw = projector_scene_draw;

	return &egl;
}
