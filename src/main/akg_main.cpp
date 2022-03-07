#include <stdio.h>
#include <limits.h>
#include "gambatte.h"
#include "inputgetter.h"
#include "loadres.h"
extern "C" {
  #include "emuhost.h"
}

//TODO highly temporary

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

static int akg_cb_param(void *userdata,const char *k,int kc,const char *v,int vc) {
  return 0;
}

static int akg_cb_init(void *userdata) {
  akg_gb.setInputGetter(&akg_input);
  return 0;
}

static void akg_cb_quit(void *userdata) {
}

static void akg_swap_pixels() {
  uint8_t *v=(uint8_t*)akg_fb;
  int i=AKG_FB_W*AKG_FB_H;
  for (;i-->0;v+=4) {
    uint8_t b=v[0],g=v[1],r=v[2];
    v[0]=r;
    v[1]=g;
    v[2]=b;
    v[3]=0;
  }
}

static int akg_cb_update(void *userdata) {
  size_t framec=AKG_AUDIO_BUFFER_NOMINAL_SIZE_FRAMES;
  ptrdiff_t result=akg_gb.runFor((uint_least32_t*)akg_fb,AKG_FB_W,(uint_least32_t*)akg_audio_buffer,framec);
  void *fb=0;
  if (result>=0) {
    fb=akg_fb;
    framec=result;
  }
  int samplec=framec<<1;
  if (fb) akg_swap_pixels();
  eh_hi_frame(fb,akg_audio_buffer,samplec);
  return 0;
}

static int akg_gameboy_button_from_emuhost(int btnid) {
  switch (btnid) {
    case EH_BUTTON_LEFT: return gambatte::InputGetter::Button::LEFT;
    case EH_BUTTON_RIGHT: return gambatte::InputGetter::Button::RIGHT;
    case EH_BUTTON_UP: return gambatte::InputGetter::Button::UP;
    case EH_BUTTON_DOWN: return gambatte::InputGetter::Button::DOWN;
    case EH_BUTTON_A: return gambatte::InputGetter::Button::A;
    case EH_BUTTON_B: return gambatte::InputGetter::Button::B;
    case EH_BUTTON_AUX2: return gambatte::InputGetter::Button::SELECT;
    case EH_BUTTON_AUX1: return gambatte::InputGetter::Button::START;
  }
  return 0;
}

static int akg_cb_player_input(void *userdata,int plrid,int btnid,int value,int state) {
  int gbmask=akg_gameboy_button_from_emuhost(btnid);
  if (gbmask) {
    if (value) akg_input.state|=gbmask;
    else akg_input.state&=~gbmask;
  }
  return 0;
}

static int akg_cb_text_input(void *userdata,int codepoint) {
  return 0;
}

static int akg_cb_reset(void *userdata,const char *path) {
  
  gambatte::LoadRes const error = akg_gb.load(path,
    gambatte::GB::GBA_CGB|gambatte::GB::MULTICART_COMPAT
  );
  if (error) {
    fprintf(stderr,"gambatte.load() failed %s\n",to_string(error).c_str());
    return -1;
  }

  return 0;
}

int main(int argc,char **argv) {
  struct eh_hi_delegate delegate={
    .video_rate=60,
    .video_width=AKG_FB_W,
    .video_height=AKG_FB_H,
    .video_format=EH_VIDEO_FORMAT_XBGR8888,
    .audio_rate=2097152,
    .audio_chanc=2,
    .audio_format=EH_AUDIO_FORMAT_S16,
    .playerc=1,
    .appname="akg",
    .param=akg_cb_param,
    .init=akg_cb_init,
    .quit=akg_cb_quit,
    .update=akg_cb_update,
    .player_input=akg_cb_player_input,
    .text_input=akg_cb_text_input,
    .reset=akg_cb_reset,
  };
  return eh_hi_main(&delegate,argc,argv);
}
