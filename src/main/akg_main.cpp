#include <stdio.h>
#include <limits.h>
#include "gambatte.h"
#include "inputgetter.h"
#include "loadres.h"
extern "C" {
  #include "emuhost.h"
}

#define AKG_FB_W 160
#define AKG_FB_H 144
#define AKG_AUDIO_BUFFER_NOMINAL_SIZE_FRAMES 35112
#define AKG_AUDIO_BUFFER_SIZE_FRAMES (AKG_AUDIO_BUFFER_NOMINAL_SIZE_FRAMES+2064)
#define AKG_AUDIO_BUFFER_SIZE_SAMPLES (AKG_AUDIO_BUFFER_SIZE_FRAMES*2)

class MyInputGetter : public gambatte::InputGetter {
public:
  int state;
  MyInputGetter() {
    state=0;
  }
  unsigned operator()() { return state; }
};

static gambatte::GB akg_gb;
static uint32_t akg_fb[AKG_FB_W*AKG_FB_H];
static int16_t akg_audio_buffer[AKG_AUDIO_BUFFER_SIZE_SAMPLES];
static MyInputGetter akg_input;

static int akg_cb_configure(const char *k,int kc,const char *v,int vc,int vn) {
  return 0;
}

static int akg_cb_load(const char *path) {
  akg_gb.setInputGetter(&akg_input);
  
  char *savedir=0;
  int savedirc=eh_get_scratch_directory(&savedir);
  if (savedirc>0) {
    fprintf(stderr,"savedir: %s\n",savedir);
    akg_gb.setSaveDir(savedir);
    free(savedir);
  }
  
  gambatte::LoadRes const error = akg_gb.load(path,
    gambatte::GB::GBA_CGB|gambatte::GB::MULTICART_COMPAT
  );
  if (error) {
    fprintf(stderr,"gambatte.load() failed %s\n",to_string(error).c_str());
    return -1;
  }
  return 0;
}

static void akg_condition_audio(int16_t *v,int c) {
  for (;c-->0;v++) {
    *v=*v>>2;
  }
}

static int akg_cb_update(int partial) {

  uint16_t input=eh_input_get(0);
  akg_input.state=
    ((input&EH_BTN_LEFT)?(gambatte::InputGetter::Button::LEFT):0)|
    ((input&EH_BTN_RIGHT)?(gambatte::InputGetter::Button::RIGHT):0)|
    ((input&EH_BTN_UP)?(gambatte::InputGetter::Button::UP):0)|
    ((input&EH_BTN_DOWN)?(gambatte::InputGetter::Button::DOWN):0)|
    ((input&EH_BTN_SOUTH)?(gambatte::InputGetter::Button::A):0)|
    ((input&EH_BTN_WEST)?(gambatte::InputGetter::Button::B):0)|
    ((input&EH_BTN_AUX1)?(gambatte::InputGetter::Button::START):0)|
    ((input&EH_BTN_AUX2)?(gambatte::InputGetter::Button::SELECT):0)|
  0;

  size_t framec=AKG_AUDIO_BUFFER_NOMINAL_SIZE_FRAMES;
  ptrdiff_t result=akg_gb.runFor((uint_least32_t*)akg_fb,AKG_FB_W,(uint_least32_t*)akg_audio_buffer,framec);
  void *fb=0;
  if (result>=0) {
    fb=akg_fb;
    framec=result;
  }
  int samplec=framec<<1;
  akg_condition_audio(akg_audio_buffer,samplec);
  eh_video_write(fb);
  eh_audio_write(akg_audio_buffer,samplec>>1);
  return 0;
}

int main(int argc,char **argv) {
  struct eh_delegate delegate={
    .name="akgambatte",
    .iconrgba=0,//TODO
    .iconw=0,
    .iconh=0,
    .video_width=AKG_FB_W,
    .video_height=AKG_FB_H,
    .video_format=EH_VIDEO_FORMAT_RGB32,
    .rmask=0x000000ff,
    .gmask=0x0000ff00,
    .bmask=0x00ff0000,
    .video_rate=60,
    .audio_rate=2097152/47,
    .audio_chanc=2,
    .audio_format=EH_AUDIO_FORMAT_S16N,
    .playerc=1,
    .configure=akg_cb_configure,
    .load_file=akg_cb_load,
    .update=akg_cb_update,
  };
  return eh_main(argc,argv,&delegate);
}
