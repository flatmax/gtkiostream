#ifndef CAPTURE_H
#define CAPTURE_H

#include <ALSA/ALSA.H>

namespace ALSA {
  /** The input channels of this sound card
  */
  class Capture : public Stream {
    void init(const char *devName){
      int err=Stream::init(devName, SND_PCM_STREAM_CAPTURE, block ? 0 : SND_PCM_NONBLOCK);
      if (err < 0) {
        std::cout<<err<<std::endl;
        std::cerr<<"Capture :: Capture : open error: "<< snd_strerror(err)<<std::endl;
        assert("open error");
      }
    }
  public:
    Capture(const char *devName) : Stream() {
      init(devName);
    };

    Capture() {
      init("default");
    };

    size_t readbuf(char *buffer, size_t len, size_t &frames, size_t &max){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      size_t r;

      if (!block) {
        do {
          r = snd_pcm_readi(getPCM(), buffer, len);
        } while (r == -EAGAIN);
        if (r > 0) {
          frames += r;
          if (max < r)
          max = r;
        }
        printf("read = %li\n", r);
      } else {
        int frame_bytes = (getFormatBits() / 8) * getChannels();
        do {
          r = snd_pcm_readi(getPCM(), buffer, len);
          if (r > 0) {
            buffer += r * frame_bytes;
            len -= r;
            frames += r;
            if (max < r)
            max = r;
          }
          printf("r = %li, len = %li\n", r, len);
        } while (r >= 1 && len > 0);
      }
      // showstat(getPCM(), 0);
      return r;
    }
  };
}
#endif //CAPTURE_H
