/* Copyright 2000-2018 Matt Flax <flatmax@flatmax.org>
   This file is part of GTK+ IOStream class set

   GTK+ IOStream is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GTK+ IOStream is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You have received a copy of the GNU General Public License
   along with GTK+ IOStream
*/
#ifndef MIXER_H_
#define MIXER_H_

#include <ALSA/ALSA.H>
#include <string>

namespace ALSA {

  /** Class to manage a channel volumes
  */
  class ChannelElement {
    long volDB;
  public:
    long vol;
    double dB;
    snd_mixer_selem_channel_id_t channel;

    ChannelElement(snd_mixer_selem_channel_id_t ch){
      vol=volDB=0;
      dB=0.;
      channel=ch;
    }

    double toDB(long v) const {
      return (double)v/100.;
    }
    long fromDB(double d) const {
      return (long)(d*100.);
    }

    /** Get the playback levels for this channel
    \param elem the mixer element to get playback levels for
    \return <0 on error
    */
    int getPlaybackLevels(snd_mixer_elem_t *elem){
      int err=0;
      if ((err=snd_mixer_selem_get_playback_volume(elem, channel, &vol))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting playback volume\n");
      if ((err=snd_mixer_selem_get_playback_dB(elem, channel, &volDB))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting playback dB\n");
      dB=toDB(volDB); // convert it to a dB value
      return 0;
    }

    /** Get the capture levels for this channel
    \param elem the mixer element to get capture levels for
    \return <0 on error
    */
    int getCaptureLevels(snd_mixer_elem_t *elem){
      int err=0;
      if ((err=snd_mixer_selem_get_capture_volume(elem, channel, &vol))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting playback volume\n");
      if ((err=snd_mixer_selem_get_capture_dB(elem, channel, &volDB))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting playback dB\n");
      dB=toDB(volDB); // convert it to a dB value
      return 0;
    }

    friend std::ostream &operator<<(std::ostream &os, ChannelElement &ce){
      os<<"Channel "<<ce.channel<<'\t'<<"level = "<<ce.vol<<" dB = "<<ce.dB;
      return os;
    }
  };

  /** Class to alter the ALSA hardware mixer
  */
  class MixerElement {
  public:
    std::string name;
    unsigned int index;
    bool hasCommonVolume; ///< element has only one volume control for both playback and capture
    bool hasPlaybackVolume; ///< 0 if no control is present, 1 if it's present
    bool hasPlaybackVolumeJoined; ///< 0 if control is separated per channel, 1 if control acts on all channels together
    bool hasCaptureVolume; ///<0 if no control is present, 1 if it's present
    bool hasCaptureVolumeJoined; ///<0 if control is separated per channel, 1 if control acts on all channels together
    bool hasCommonSwitch; ///<0 separated control, 1 common control
    bool hasPlaybackSwitch; ///<0 if no control is present, 1 if it's present
    bool hasPlaybackSwitchJoined; ///<0 if control is separated per channel, 1 if control acts on all channels together
    bool hasCaptureSwitch; ///<0 if no control is present, 1 if it's present
    bool hasCaptureSwitchJoined; ///<0 if control is separated per channel, 1 if control acts on all channels together
    bool hasCaptureSwitchExclusive; ///<0 if control is separated per element, 1 if control acts on other elements too (i.e. only one active at a time inside a group)
    long playbackVolMax;
    long playbackVolMin;
    double playbackVolMaxDB;
    double playbackVolMinDB;
    long captureVolMax;
    long captureVolMin;
    double captureVolMaxDB;
    double captureVolMinDB;

    std::vector<ChannelElement> playbackVolumes; ///< Volume setting for each playback channel in this mixer element
    std::vector<ChannelElement> captureVolumes; ///< Volume setting for each capture channel in this mixer element
  public:
    MixerElement(){
      name="";
      index=0;

      playbackVolMax=playbackVolMin=0;
      playbackVolMaxDB=playbackVolMinDB=0.;
      captureVolMax=captureVolMin=0;
      captureVolMaxDB=captureVolMinDB=0.;
      hasCommonVolume=false;
      hasPlaybackVolume=false;
      hasPlaybackVolumeJoined=false;
      hasCaptureVolume=false;
      hasCaptureVolumeJoined=false;
      hasCommonSwitch=false;
      hasPlaybackSwitch=false;
      hasPlaybackSwitchJoined=false;
      hasCaptureSwitch=false;
      hasCaptureSwitchJoined=false;
      hasCaptureSwitchExclusive=false;
    }

    MixerElement(snd_mixer_elem_t *elem){
      name=snd_mixer_selem_get_name(elem);
      index=snd_mixer_selem_get_index(elem);

      playbackVolMax=playbackVolMin=0;
      playbackVolMaxDB=playbackVolMinDB=0.;
      captureVolMax=captureVolMin=0;
      captureVolMaxDB=captureVolMinDB=0.;

      hasCommonVolume=snd_mixer_selem_has_common_volume(elem);
      hasPlaybackVolume=snd_mixer_selem_has_playback_volume(elem);
      hasPlaybackVolumeJoined=snd_mixer_selem_has_playback_volume_joined(elem);
      hasCaptureVolume=snd_mixer_selem_has_capture_volume(elem);
      hasCaptureVolumeJoined=snd_mixer_selem_has_capture_volume_joined(elem);
      hasCommonSwitch=snd_mixer_selem_has_common_switch(elem);
      hasPlaybackSwitch=snd_mixer_selem_has_playback_switch(elem);
      hasPlaybackSwitchJoined=snd_mixer_selem_has_playback_switch_joined(elem);
      hasCaptureSwitch=snd_mixer_selem_has_capture_switch(elem);
      hasCaptureSwitchJoined=snd_mixer_selem_has_capture_switch_joined(elem);
      hasCaptureSwitchExclusive=snd_mixer_selem_has_capture_switch_exclusive(elem);

      unsigned long chMask=1;
      for (int i=0; i<SND_MIXER_SCHN_LAST; i++)
        if (snd_mixer_selem_has_playback_channel (elem, (snd_mixer_selem_channel_id_t)i)>0){
          ChannelElement ce((snd_mixer_selem_channel_id_t)i);
          ce.getPlaybackLevels(elem);
          playbackVolumes.push_back(ce);
        }

      chMask=1;
      for (int i=0; i<SND_MIXER_SCHN_LAST; i++, chMask<<=1)
        if (snd_mixer_selem_has_capture_channel (elem, (snd_mixer_selem_channel_id_t)i)>0){
          ChannelElement ce((snd_mixer_selem_channel_id_t)i);
          ce.getCaptureLevels(elem);
          captureVolumes.push_back(ce);
        }

        if (hasPlaybackVolume || hasPlaybackVolumeJoined){
          if (snd_mixer_selem_get_playback_volume_range(elem, &playbackVolMin, &playbackVolMax))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "playback volume range");
          long min, max;
          if (snd_mixer_selem_get_playback_dB_range(elem, &min, &max))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "playback dB range");
          playbackVolMinDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(min);
          playbackVolMaxDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(max);
        }

        if (hasCaptureVolume || hasCaptureVolumeJoined){
          if (snd_mixer_selem_get_capture_volume_range(elem, &captureVolMin, &captureVolMax))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "capture volume range");
          long min, max;
          if (snd_mixer_selem_get_capture_dB_range(elem, &min, &max))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "capture dB range");
          captureVolMinDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(min);
          captureVolMaxDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(max);
        }
    }

    friend std::ostream &operator<<(std::ostream &os, MixerElement &me){
      os<<me.name<<'\n';
      os<<me.index<<'\n';

      if (me.hasCommonVolume)
        os<<"Common Volume"<<'\n';
      if (me.hasPlaybackVolume)
        os<<"Playback Volume"<<'\n';
      if (me.hasPlaybackVolumeJoined)
        os<<"Playback Volume Joined"<<'\n';
      if (me.hasCaptureVolume)
        os<<"Capture Volume"<<'\n';
      if (me.hasCaptureVolumeJoined)
        os<<"Capture Volume Joined"<<'\n';
      if (me.hasCommonSwitch)
        os<<"Common Switch"<<'\n';
      if (me.hasPlaybackSwitch)
        os<<"Playback Switch"<<'\n';
      if (me.hasPlaybackSwitchJoined)
        os<<"Playback Switch Joined"<<'\n';
      if (me.hasCaptureSwitch)
        os<<"Capture Switch"<<'\n';
      if (me.hasCaptureSwitchJoined)
        os<<"Capture Switch Joined"<<'\n';
      if (me.hasCaptureSwitchExclusive)
        os<<"Capture Switch Exclusive"<<'\n';
      if (me.hasPlaybackVolume || me.hasPlaybackVolumeJoined){
        os<<"Playback volume levels : "<<'\n';
        for (int i=0; i<me.playbackVolumes.size(); i++)
          os<<me.playbackVolumes[i]<<'\n';
      }
      if (me.hasCaptureVolume || me.hasCaptureVolumeJoined){
        os<<"Capture volume levels : "<<'\n';
        for (int i=0; i<me.captureVolumes.size(); i++)
        os<<me.captureVolumes[i]<<'\n';
      }
      return os;
    }
  };

  /** Class to alter the ALSA hardware mixer
  */
  class Mixer {
    snd_mixer_t *handle;
    std::vector<MixerElement> mixerElements; ///< The vector of all mixer elements

  public:
    Mixer(){
      handle=NULL;
    }

    virtual ~Mixer(){
      if (handle)
        close();
    }

    /** Acquire the mixer handle.
    \return 0 on success, otherwise the error.
    */
    int open(){
      int err;
      if ((err = snd_mixer_open(&handle, 0)) < 0) {
        handle=NULL;
  			return ALSADebug().evaluateError(err);
      }
      return 0;
    }

    /** Free the mixer handle.
    \return 0 on success, otherwise the error.
    */
    int close(){
      MIXER_NOT_OPEN_CHECK(handle)
      int err;
      if ((err = snd_mixer_close(handle)) < 0) {
        handle=NULL;
  			return ALSADebug().evaluateError(err);
      }
      handle=NULL;
      return 0;
    }

    /** Attach to the card for setting the mixer
    \param card The card to open, e.g. default, hw:0, etc.
    \return 0 on success, otherwise the error.
    */
    int attach(std::string card){
      MIXER_NOT_OPEN_CHECK(handle)
      int err;
      if ((err = snd_mixer_attach(handle, card.c_str())) < 0) {
        snd_mixer_close(handle);
        std::string msg="Mixer attach "; msg+=card;
        return ALSADebug().evaluateError(err, msg);
      }
      return 0;
    }

    /** register the simple mixer element class
    \return 0 on success, otherwise the error.
    */
    int registerMixer(){
      MIXER_NOT_OPEN_CHECK(handle)
      int err;
      if ((err = snd_mixer_selem_register(handle, NULL, NULL)) < 0) {
        snd_mixer_close(handle);
        return ALSADebug().evaluateError(err, "Mixer register ");
      }
      return 0;
    }

    /** load the mixer elements
    \return 0 on success, otherwise the error.
    */
    int load(){
      MIXER_NOT_OPEN_CHECK(handle)
      int err;
      if ((err = snd_mixer_load(handle)) < 0) {
        snd_mixer_close(handle);
        return ALSADebug().evaluateError(err, "Mixer load ");
      }
      return 0;
    }

    int openCard(std::string card){
      int err=0;
      if (handle)
        if ((err=close())<0)
          return err;
      if ((err=open())<0)
        return err;
      if ((err=attach(card))<0)
        return err;
      if ((err=registerMixer())<0)
        return err;
      if ((err=load())<0)
        return err;
      return err;
    }

    /** Given a mixer element, find its capabilities and state
    \param elem the ALSA mixer element
    \return A populated MixerElement class
    */
    MixerElement findCapabilities(snd_mixer_elem_t *elem){
      MixerElement me(elem);
      return me;
    }

    /** Scan the mixer to find capabilites and available controls.
    Upon completion, all playback and capture mixer elements are populated
    */
    void scanMixer(){
      mixerElements.resize(0);
      for (snd_mixer_elem_t *elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem))
        mixerElements.push_back(findCapabilities(elem));

      for (int i=0; i<mixerElements.size(); i++)
        std::cout<<mixerElements[i]<<std::endl;
    }
  };
}
#endif // MIXER_H_
