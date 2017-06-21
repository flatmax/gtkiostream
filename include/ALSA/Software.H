#ifndef SOFTWARE_H
#define SOFTWARE_H

#include <ALSA/ALSA.H>

namespace ALSA {
  class Software : public Hardware {
    snd_pcm_sw_params_t *sParams; ///< PCM software params

  public:
    Software(void){
      sParams=NULL;
      snd_pcm_sw_params_malloc(&sParams);
      if (!sParams)
        assert("Couldn't malloc SW params");
    }

    virtual ~Software(void){
      if (sParams)
        snd_pcm_sw_params_free(sParams);
      sParams=NULL;
    }

    /** Get the software params
    \return >= 0 on success
    */
    int getSWParams() {
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_sw_params_current(getPCM(), sParams);
    }

    /** Set the software params
    \return >= 0 on success
    */
    int setSWParams() {
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_sw_params(getPCM(), sParams);
    }

    /** Set the start threshold
    \param thresh Start threshold in frames
    \return >= 0 on success
    */
    int setSWThreshold(snd_pcm_uframes_t thresh) {
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_sw_params_set_start_threshold(getPCM(), sParams, thresh);
    }

    /** Set avail min inside a software configuration container
    \param cnt Minimum avail frames to consider PCM ready
    \return >= 0 on success
    */
    int setAvailMin(snd_pcm_uframes_t cnt) {
      PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
      return snd_pcm_sw_params_set_avail_min(getPCM(), sParams, cnt);
    }

    /** Return the address to start reading/writing to give an ALSA areas type
    \param areas The ALSA provided snd_pcm_channel_area_t type
    \return The pointer to start reading or writing to/from
    */
    void *getAddress(const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset) const {
      return (areas->addr+(areas->first+offset*areas->step)/8);
    }

    /** Return the number of channels in an ALSA provided areas type
    \param areas The ALSA provided snd_pcm_channel_area_t type
    \tparam SAMPLE_TYPE The type of the samples to derive the channel count for
    \return The number of channels
    */
    template<typename SAMPLE_TYPE>
    int getChannelCount(const snd_pcm_channel_area_t *areas) const {
      return areas->step/sizeof(SAMPLE_TYPE)/8;;
    }

    int dumpSWParams(){
      int ret=0;
      if (!logEnabled())
        return ret;
      return snd_pcm_sw_params_dump(sParams, log);
    }

  };
}
#endif //SOFTWARE_H
