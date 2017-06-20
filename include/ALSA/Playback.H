#ifndef PLAYBACK_H
#define PLAYBACK_H

#include <ALSA/ALSA.H>
#include <vector>

namespace ALSA {
  /** The output channels of this sound card
  */
  class Playback : public Stream {
    std::vector<void *> buffer; ///< Non interleaved buffer pointers
  public:
    Playback(const char *devName) : Stream() {
      open(devName);
    }

    Playback() {
      open();
    }

    virtual ~Playback(void){
    }

    void open(const char *devName="default"){
      int err=Stream::init(devName, SND_PCM_STREAM_PLAYBACK, block ? 0 : SND_PCM_NONBLOCK);
      if (err < 0) {
        std::cout<<err<<std::endl;
        std::cerr<<"Playback :: Playback : open error: "<< snd_strerror(err)<<std::endl;
        assert("open error");
      }
      if ((err=setParams())<0) {
        std::cerr<<"Playback :: Playback : set params error: "<< snd_strerror(err)<<std::endl;
        assert("set params error");
      }
    }

    /** Write audio data to a buffer, returning error or the number written
    @param buffers The buffer to write to the pcm device - non-interleaved data
    @param len The number of frames to write
    @param ch The number of channels (buffer must be of size ch)
    @return NO_ERROR on success an error otherwise
    */
    int writeBuf(void **buffers, size_t len, int ch){
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      std::cout<<"ch = "<<ch<<"\n";
      if (buffer.size()!=ch)
        buffer.resize(ch);
      for (int i=0;i<ch;i++)
        buffer[i]=buffers[i];
      int ret=0;
      while ((len-=ret) > 0) {
//        dumpStatus();
        ret = snd_pcm_writen(getPCM(), &buffer[0], len);
        std::cout<<"ret "<<ret<<std::endl;
        if (ret == -EAGAIN || (ret>=0 && ret<len))
          wait();
        if (ret<0 && ret!=-EAGAIN){
          printf("Write error: %s\n", snd_strerror(ret));
          return ret;
        }
        std::cout<<"ch = "<<ch<<"\n";
        for (int i=0;i<ch;i++){
          char *c=(char*)buffer[i];
          c+=ret*4;
          buffer[i]=(void*)c;
        }
      }
      return 0;
    }

    /** Write audio data to a buffer, returning error or the number written
    @param buffer The buffer to write to the pcm device - interleaved data
    @param len The number of frames to write
    @return NO_ERROR on success an error otherwise
    */
    int writeBuf(char *bufferIn, size_t len){
      PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), int) // check pcm is open
      int ret=snd_pcm_writei(getPCM(), (void *)bufferIn, len); // first time through - allow for starting if required
      if (prepared())
        if ((ret=start())<0)
          return ALSADebug().evaluateError(ret);

      while ((len-=ret) > 0) {
        if (ret == -EAGAIN || (ret>=0 && ret<len))
          wait();
        if (ret<0 && ret!=-EAGAIN)
          return ALSADebug().evaluateError(ret,std::string(" in writeBuf main loop\n"));
        bufferIn += ret*4;

        ret = snd_pcm_writei(getPCM(), (void *)bufferIn, len);
      }
      return 0;
    }

    template <typename Derived>
    int writeBuf(const Eigen::DenseBase<Derived> &audioData){
      if (audioData.cols()==audioData.rows()){ // can't tell which version of writeBuf to use - error
        printf("Error : audioData.cols()==audioData.rows()=%d\nPlease use writeBuf with pointers\n",audioData.cols());
        return -1;
      }
      if (audioData.rows()>audioData.cols()){ // non interleaved writes
        std::cout<<"non interleaved"<<std::endl;
        if (buffer.size()!=audioData.cols())
          buffer.resize(audioData.cols());
        for (int i=0;i<audioData.cols();i++)
          buffer[i]=(void*)audioData.col(i).data();
        return writeBuf(&buffer[0], audioData.rows(), audioData.cols());
      } else // interleaved writes
        return writeBuf((char*)&audioData(0,0), audioData.cols());
    }

    /** Using known parameters from the Eigen densebase, write the audio out.
    @param audioData One channel per row (transpose of the normal format)
    @return A reference to this instance for further use.
    */
    template <typename Derived>
    Playback& operator<<(const Eigen::DenseBase<Derived> &audioData){
//      int ret=writeBuf((char*)&audioData(0,0),audioData.cols());
      int ret=writeBuf(audioData);
      if (ret<0)
        throw;
      return *this;
    }

    // template <typename Derived>
    // void createInternalBuffer(Eigen::DenseBase<Derived> &audioMatrix){
    //
    // }
  };
}
#endif //PLAYBACK_H
