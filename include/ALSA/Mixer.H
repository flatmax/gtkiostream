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
#include <sstream>

namespace ALSA {

  /** Class to manage a channel volumes
  */
  class ChannelElement {
  protected:
    long volDB; ///< If the channel has a volume, this holds its state in 100*dB
  public:
    long vol; ///< If the channel has a volume, this holds its state
    double dB; ///< If the channel has a volume, this holds its state in dB
    int sw; ///< If the channel has a switch, this holds its state
    snd_mixer_selem_channel_id_t channel;

    ChannelElement(snd_mixer_selem_channel_id_t ch){
      vol=volDB=0;
      dB=0.;
      channel=ch;
    }

    virtual ~ChannelElement(){}

    double toDB(long v) const {
      return (double)v/100.;
    }

    long fromDB(double d) const {
      return (long)(d*100.);
    }

    void outputLevel(std::ostream &os){
      os<<"{\"channel\": "<<channel<<", "<<"\"level\":"<<vol<<", \"dB\":"<<dB<<"}";
    }

    void outputSwitch(std::ostream &os){
      os<<"{\"channel\": "<<channel<<", "<<"\"switch\":"<<sw<<"}";
    }

    void outputLevelAndSwitch(std::ostream &os){
      os<<"{\"channel\": "<<channel<<", "<<"\"level\":"<<vol<<", \"dB\":"<<dB
            <<", "<<"\"switch\":"<<sw<<"}";
    }
  };

  /** Class to manage a playback channel volumes
  */
  class ChannelPlaybackElement : public ChannelElement {
  public:
    ChannelPlaybackElement(snd_mixer_selem_channel_id_t ch) : ChannelElement(ch) {}

    virtual ~ChannelPlaybackElement(){}

    /** Get the playback levels for this channel
    \param elem the mixer element to get playback levels for
    \return <0 on error
    */
    int getLevels(snd_mixer_elem_t *elem){
      int err=0;
      if ((err=snd_mixer_selem_get_playback_volume(elem, channel, &vol))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting playback volume\n");
      if ((err=snd_mixer_selem_get_playback_dB(elem, channel, &volDB))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting playback dB\n");
      dB=toDB(volDB); // convert it to a dB value
      return 0;
    }

    /** Get the switch state for this channel
    \param elem the mixer element to get switch state for
    \return <0 on error
    */
    int getSwitch(snd_mixer_elem_t *elem){
      int err=0;
      if ((err=snd_mixer_selem_get_playback_switch(elem, channel, &sw))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting playback switch\n");
      return 0;
    }
  };

  /** Class to manage a capture channel volumes
  */
  class ChannelCaptureElement : public ChannelElement {
  public:
    ChannelCaptureElement(snd_mixer_selem_channel_id_t ch) : ChannelElement(ch) {}
    virtual ~ChannelCaptureElement(){}

    /** Get the capture levels for this channel
    \param elem the mixer element to get capture levels for
    \return <0 on error
    */
    int getLevels(snd_mixer_elem_t *elem){
      int err=0;
      if ((err=snd_mixer_selem_get_capture_volume(elem, channel, &vol))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting capture volume\n");
      if ((err=snd_mixer_selem_get_capture_dB(elem, channel, &volDB))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting capture dB\n");
      dB=toDB(volDB); // convert it to a dB value
      return 0;
    }

    /** Get the switch state for this channel
    \param elem the mixer element to get switch state for
    \return <0 on error
    */
    int getSwitch(snd_mixer_elem_t *elem){
      int err=0;
      if ((err=snd_mixer_selem_get_capture_switch(elem, channel, &sw))<0)
        return ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "getting catpure switch\n");
      return 0;
    }
  };

  /** Class to alter the ALSA hardware mixer
  */
  class MixerElement {
  public:
    std::string name; ///< The mixer element name
    snd_mixer_elem_t *elem; ///< The pointer to the element
    unsigned int index; ///< The mixer element index
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

    std::vector<ChannelPlaybackElement> playbackVolumes; ///< Volume setting for each playback channel in this mixer element
    std::vector<ChannelCaptureElement> captureVolumes; ///< Volume setting for each capture channel in this mixer element
  public:
    MixerElement(){
      name="";
      index=0;
      elem=NULL;

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

    MixerElement(snd_mixer_elem_t *e){
      elem=e;
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
      for (int i=0; i<SND_MIXER_SCHN_LAST; i++){
        if (snd_mixer_selem_has_playback_channel(elem, (snd_mixer_selem_channel_id_t)i)>0){
          playbackVolumes.push_back(ChannelPlaybackElement((snd_mixer_selem_channel_id_t)i));
          if (hasPlaybackVolume || hasPlaybackVolumeJoined)
            playbackVolumes[playbackVolumes.size()-1].getLevels(elem);
          if (hasCommonSwitch || hasPlaybackSwitch || hasPlaybackSwitchJoined)
            playbackVolumes[playbackVolumes.size()-1].getSwitch(elem);
        }
      }

      for (int i=0; i<SND_MIXER_SCHN_LAST; i++)
        if (snd_mixer_selem_has_capture_channel(elem, (snd_mixer_selem_channel_id_t)i)>0){
          captureVolumes.push_back(ChannelCaptureElement((snd_mixer_selem_channel_id_t)i));
          if (hasCaptureVolume || hasCaptureVolumeJoined)
            captureVolumes[captureVolumes.size()-1].getLevels(elem);
          if (hasCommonSwitch || hasCaptureSwitch || hasCaptureSwitchJoined)
            captureVolumes[captureVolumes.size()-1].getSwitch(elem);
        }

        if (hasPlaybackVolume || hasPlaybackVolumeJoined){
          if (snd_mixer_selem_get_playback_volume_range(elem, &playbackVolMin, &playbackVolMax))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "playback volume range");
          long min, max;
          if (snd_mixer_selem_get_playback_dB_range(elem, &min, &max))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "playback dB range");
          playbackVolMinDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(min); // dummy id
          playbackVolMaxDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(max); // dummy id
        }

        if (hasCaptureVolume || hasCaptureVolumeJoined){
          if (snd_mixer_selem_get_capture_volume_range(elem, &captureVolMin, &captureVolMax))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "capture volume range");
          long min, max;
          if (snd_mixer_selem_get_capture_dB_range(elem, &min, &max))
            ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "capture dB range");
          captureVolMinDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(min); // dummy id
          captureVolMaxDB=ChannelElement((snd_mixer_selem_channel_id_t)0).toDB(max); // dummy id
        }
    }

    virtual ~MixerElement(){
    }

    /** Set the playback switch state for all channels, returning the current playback switch state
    \param v The value to set
    \return <0 on error or the current state of the switch
    */
    int setPlaybackSwitch(int v){
      int err;
      if (!hasCommonSwitch && !hasPlaybackSwitch && !hasPlaybackSwitchJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a playback switch and I tried to set it\n");
      for (int i=0; i<playbackVolumes.size(); i++){
        if ((err=snd_mixer_selem_set_playback_switch(elem, playbackVolumes[i].channel, v))<0)
          return ALSADebug().evaluateError(err, name+" setting playback switches"+'\n');
        if ((err=playbackVolumes[i].getSwitch(elem))<0) // refresh the playback level
          return ALSADebug().evaluateError(err, name+" getting playback switches"+'\n');
      }
      return playbackVolumes[0].sw; // all channels have the same volume in this method
    }

    /** Set the playback volume for all channels, returning the current playback volume
    \param v The value to set
    \return <0 on error or the current state of the volume
    */
    long setPlaybackVol(long v){
      int err;
      if (!hasPlaybackVolume && !hasPlaybackVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a playback volume and I tried to set it\n");
      for (int i=0; i<playbackVolumes.size(); i++){
        if ((err=snd_mixer_selem_set_playback_volume(elem, playbackVolumes[i].channel, v))<0)
          return ALSADebug().evaluateError(err, name+" setting playback levels"+'\n');
        if ((err=playbackVolumes[i].getLevels(elem))<0) // refresh the playback level
          return ALSADebug().evaluateError(err, name+" getting playback levels"+'\n');
      }
      return playbackVolumes[0].vol; // all channels have the same volume in this method
    }

    /** Set the playback volume for one channel, returning the current playback volume for that channel
    \param channel The channel to set the volume on
    \param v The value to set
    \return <0 on error or the current state of the volume
    */
    long setPlaybackVol(snd_mixer_selem_channel_id_t channel, long v){
      int err;
      if (!hasPlaybackVolume && !hasPlaybackVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a playback volume and I tried to set it\n");
      for (int i=0; i<playbackVolumes.size(); i++)
        if (channel==playbackVolumes[i].channel){
          if ((err=snd_mixer_selem_set_playback_volume (elem, playbackVolumes[i].channel, v))<0)
            return ALSADebug().evaluateError(err, name+" setting playback level for this channel"+'\n');
          if ((err=playbackVolumes[i].getLevels(elem))<0) // refresh the playback level
            return ALSADebug().evaluateError(err, name+" getting playback level for this channel"+'\n');
          return playbackVolumes[i].vol; // all channels have the same volume in this method
        }
      return ALSADebug().evaluateError(ALSA_MIXER_NO_CHANNEL_ERROR, name+" channel not found");
    }

    /** Set the playback volume (in dB) for all channels, returning the current playback volume in dB
    \param[in,out] dB The value to set and on return, then actual value
    \return <0 on error - the current dB of the volume is returned in the parameter dB
    */
    int setPlaybackVolDB(double &dB){
      int err;
      if (!hasPlaybackVolume && !hasPlaybackVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a playback volume and I tried to set it\n");
      for (int i=0; i<playbackVolumes.size(); i++){
        if ((err=snd_mixer_selem_set_playback_dB(elem, playbackVolumes[i].channel, ChannelElement((snd_mixer_selem_channel_id_t)0).fromDB(dB), 1))<0)
          return ALSADebug().evaluateError(err, name+" setting playback levels"+'\n');
        if ((err=playbackVolumes[i].getLevels(elem))<0) // refresh the playback level
          return ALSADebug().evaluateError(err, name+" getting playback levels"+'\n');
        dB=playbackVolumes[i].dB; // all channels have the same volume in this method
      }
      return 0;
    }

    /** Set the playback volume for one channel, returning the current playback volume for that channel
    \param channel The channel to set the volume on
    \param[in,out] dB The value to set and on return, then actual value
    \return <0 on error - the current dB of the volume is returned in the parameter dB
    */
    long setPlaybackVolDB(snd_mixer_selem_channel_id_t channel, double &dB){
      int err;
      if (!hasPlaybackVolume && !hasPlaybackVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a playback volume and I tried to set it\n");
      for (int i=0; i<playbackVolumes.size(); i++)
        if (channel==playbackVolumes[i].channel){
          if ((err=snd_mixer_selem_set_playback_dB(elem, playbackVolumes[i].channel, ChannelElement((snd_mixer_selem_channel_id_t)0).fromDB(dB),1))<0) // dummy id
            return ALSADebug().evaluateError(err, name+" setting playback level for this channel"+'\n');
          if ((err=playbackVolumes[i].getLevels(elem))<0) // refresh the playback level
            return ALSADebug().evaluateError(err, name+" getting playback level for this channel"+'\n');
          dB=playbackVolumes[i].dB; // all channels have the same volume in this method
          return 0;
        }
      return ALSADebug().evaluateError(ALSA_MIXER_NO_CHANNEL_ERROR, name+" channel not found");
    }

    /** Set the capture switch state for all channels, returning the current capture switch state
    \param v The value to set
    \return <0 on error or the current state of the switch
    */
    int setCaptureSwitch(int v){
      int err;
      if (!hasCommonSwitch && !hasCaptureSwitch && !hasCaptureSwitchJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a capture switch and I tried to set it\n");
      for (int i=0; i<captureVolumes.size(); i++){
        if ((err=snd_mixer_selem_set_capture_switch(elem, captureVolumes[i].channel, v))<0)
          return ALSADebug().evaluateError(err, name+" setting capture switches"+'\n');
        if ((err=captureVolumes[i].getSwitch(elem))<0) // refresh the capture level
          return ALSADebug().evaluateError(err, name+" getting capture switches"+'\n');
      }
      return captureVolumes[0].sw; // all channels have the same volume in this method
    }

    /** Set the capture volume for all channels, returning the current capture volume
    \param v The value to set
    \return <0 on error or the current state of the volume
    */
    long setCaptureVol(long v){
      int err;
      if (!hasCaptureVolume && !hasCaptureVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a capture volume and I tried to set it\n");
      for (int i=0; i<captureVolumes.size(); i++){
        if ((err=snd_mixer_selem_set_capture_volume(elem, captureVolumes[i].channel, v))<0)
          return ALSADebug().evaluateError(err, name+" setting capture levels"+'\n');
        if ((err=captureVolumes[i].getLevels(elem))<0) // refresh the capture level
          return ALSADebug().evaluateError(err, name+" getting capture levels"+'\n');
      }
      return captureVolumes[0].vol; // all channels have the same volume in this method
    }

    /** Set the capture volume for one channel, returning the current capture volume for that channel
    \param channel The channel to set the volume on
    \param v The value to set
    \return <0 on error or the current state of the volume
    */
    long setCaptureVol(snd_mixer_selem_channel_id_t channel, long v){
      int err;
      if (!hasCaptureVolume && !hasCaptureVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a capture volume and I tried to set it\n");
      for (int i=0; i<captureVolumes.size(); i++)
        if (channel==captureVolumes[i].channel){
          if ((err=snd_mixer_selem_set_capture_volume (elem, captureVolumes[i].channel, v))<0)
            return ALSADebug().evaluateError(err, name+" setting capture level for this channel"+'\n');
          if ((err=captureVolumes[i].getLevels(elem))<0) // refresh the capture level
            return ALSADebug().evaluateError(err, name+" getting capture level for this channel"+'\n');
          return captureVolumes[i].vol; // all channels have the same volume in this method
        }
      return ALSADebug().evaluateError(ALSA_MIXER_NO_CHANNEL_ERROR, name+" channel not found");
    }

    /** Set the capture volume (in dB) for all channels, returning the current capture volume in dB
    \param[in,out] dB The value to set and on return, then actual value
    \return <0 on error - the current dB of the volume is returned in the parameter dB
    */
    int setCaptureVolDB(double &dB){
      int err;
      if (!hasCaptureVolume && !hasCaptureVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a capture volume and I tried to set it\n");
      for (int i=0; i<captureVolumes.size(); i++){
        if ((err=snd_mixer_selem_set_capture_dB(elem, captureVolumes[i].channel, ChannelElement((snd_mixer_selem_channel_id_t)0).fromDB(dB), 1))<0)
          return ALSADebug().evaluateError(err, name+" setting capture levels"+'\n');
        if ((err=captureVolumes[i].getLevels(elem))<0) // refresh the capture level
          return ALSADebug().evaluateError(err, name+" getting capture levels"+'\n');
        dB=captureVolumes[i].dB; // all channels have the same volume in this method
      }
      return 0;
    }

    /** Set the capture volume for one channel, returning the current capture volume for that channel
    \param channel The channel to set the volume on
    \param[in,out] dB The value to set and on return, then actual value
    \return <0 on error - the current dB of the volume is returned in the parameter dB
    */
    long setCaptureVolDB(snd_mixer_selem_channel_id_t channel, double &dB){
      int err;
      if (!hasCaptureVolume && !hasCaptureVolumeJoined)
        return ALSADebug().evaluateError(ALSA_MIXER_NO_PLAYBACK_VOL_ERROR, name+" doesn't have a capture volume and I tried to set it\n");
      for (int i=0; i<captureVolumes.size(); i++)
        if (channel==captureVolumes[i].channel){
          if ((err=snd_mixer_selem_set_capture_dB(elem, captureVolumes[i].channel, ChannelElement((snd_mixer_selem_channel_id_t)0).fromDB(dB),1))<0) // dummy id
            return ALSADebug().evaluateError(err, name+" setting capture level for this channel"+'\n');
          if ((err=captureVolumes[i].getLevels(elem))<0) // refresh the capture level
            return ALSADebug().evaluateError(err, name+" getting capture level for this channel"+'\n');
          dB=captureVolumes[i].dB; // all channels have the same volume in this method
          return 0;
        }
      return ALSADebug().evaluateError(ALSA_MIXER_NO_CHANNEL_ERROR, name+" channel not found");
    }

    friend std::ostream &operator<<(std::ostream &os, MixerElement &me){
      os<<"{";
      os<<"\"name\":\""<<me.name<<"\",\n";
      os<<"\"index\":"<<me.index<<",\n";
      os<<"\"hasCommonVolume\":"<<me.hasCommonVolume<<",\n";
      os<<"\"hasPlaybackVolume\":"<<me.hasPlaybackVolume<<",\n";
      os<<"\"hasPlaybackVolumeJoined\":"<<me.hasPlaybackVolumeJoined<<",\n";
      os<<"\"hasCaptureVolume\":"<<me.hasCaptureVolume<<",\n";
      os<<"\"hasCaptureVolumeJoined\":"<<me.hasCaptureVolumeJoined<<",\n";
      os<<"\"hasCommonSwitch\":"<<me.hasCommonSwitch<<",\n";
      os<<"\"hasPlaybackSwitch\":"<<me.hasPlaybackSwitch<<",\n";
      os<<"\"hasPlaybackSwitchJoined\":"<<me.hasPlaybackSwitchJoined<<",\n";
      os<<"\"hasCaptureSwitch\":"<<me.hasCaptureSwitch<<",\n";
      os<<"\"hasCaptureSwitchJoined\":"<<me.hasCaptureSwitchJoined<<",\n";
      os<<"\"hasCaptureSwitchExclusive\":"<<me.hasCaptureSwitchExclusive<<"\n";

      if (me.hasPlaybackVolume || me.hasPlaybackVolumeJoined){
        os<<",\"playbackVolMax\":"<<me.playbackVolMax<<"\n";
        os<<",\"playbackVolMin\":"<<me.playbackVolMin<<"\n";
        os<<",\"playbackVolMaxDB\":"<<me.playbackVolMaxDB<<"\n";
        os<<",\"playbackVolMinDB\":"<<me.playbackVolMinDB<<"\n";
      }

      if (me.hasCaptureVolume || me.hasCaptureVolumeJoined){
        os<<",\"captureVolMax\":"<<me.captureVolMax<<"\n";
        os<<",\"captureVolMin\":"<<me.captureVolMin<<"\n";
        os<<",\"captureVolMaxDB\":"<<me.captureVolMaxDB<<"\n";
        os<<",\"captureVolMinDB\":"<<me.captureVolMinDB<<"\n";
      }

      if ((me.hasPlaybackVolume || me.hasPlaybackVolumeJoined) && (me.hasCommonSwitch || me.hasPlaybackSwitch || me.hasPlaybackSwitchJoined)){
        os<<",\"playbackVolumes\":[\n";
        for (int i=0; i<me.playbackVolumes.size(); i++){
          me.playbackVolumes[i].outputLevelAndSwitch(os);
          os<<((i<me.playbackVolumes.size()-1) ? ",\n" : "\n");
        }
        os<<"]\n";
      } else
          if (me.hasPlaybackVolume || me.hasPlaybackVolumeJoined){
            os<<",\"playbackVolumes\":[\n";
            for (int i=0; i<me.playbackVolumes.size(); i++){
              me.playbackVolumes[i].outputLevel(os);
              os<<((i<me.playbackVolumes.size()-1) ? ",\n" : "\n");
            }
            os<<"]\n";
          } else
            if (me.hasCommonSwitch || me.hasPlaybackSwitch || me.hasPlaybackSwitchJoined){
              os<<",\"playbackVolumes\":[\n";
              for (int i=0; i<me.playbackVolumes.size(); i++){
                me.playbackVolumes[i].outputSwitch(os);
                os<<((i<me.playbackVolumes.size()-1) ? ",\n" : "\n");
              }
              os<<"]\n";
            }

        if ((me.hasCaptureVolume || me.hasCaptureVolumeJoined) && (me.hasCommonSwitch || me.hasCaptureSwitch || me.hasCaptureSwitchJoined)){
          os<<",\"captureVolumes\":[\n";
          for (int i=0; i<me.captureVolumes.size(); i++){
            me.captureVolumes[i].outputLevelAndSwitch(os);
            os<<((i<me.captureVolumes.size()-1) ? ",\n" : "\n");
          }
          os<<"]\n";
        } else
            if (me.hasCaptureVolume || me.hasCaptureVolumeJoined){
              os<<",\"captureVolumes\":[\n";
              for (int i=0; i<me.captureVolumes.size(); i++){
                me.captureVolumes[i].outputLevel(os);
                os<<((i<me.captureVolumes.size()-1) ? ",\n" : "\n");
              }
              os<<"]\n";
            } else
              if (me.hasCommonSwitch || me.hasCaptureSwitch || me.hasCaptureSwitchJoined){
                os<<",\"captureVolumes\":[\n";
                for (int i=0; i<me.captureVolumes.size(); i++){
                  me.captureVolumes[i].outputSwitch(os);
                  os<<((i<me.captureVolumes.size()-1) ? ",\n" : "\n");
                }
                os<<"]\n";
              }
      os<<"}";
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

    /** Open a card given the device name
    \param card The device name
    \return <0 on failure
    */
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

    /** Scan the mixer to find capabilites and available controls.
    Upon completion, all playback and capture mixer elements are populated
    */
    void scanMixer(){
      if (!handle)
        ALSADebug().evaluateError(ALSA_MIXER_NOT_OPEN_ERROR);
      else {
        mixerElements.resize(0);
        for (snd_mixer_elem_t *elem = snd_mixer_first_elem(handle); elem; elem = snd_mixer_elem_next(elem)){
          mixerElements.push_back(MixerElement(elem));
        }
      }
    }

    /** output json data to an output stream
    */
    friend std::ostream &operator<<(std::ostream &os, Mixer &m){
      os<<"[\n";
      for (int i=0; i<m.mixerElements.size(); i++)
        os<<m.mixerElements[i]<< ((i<m.mixerElements.size()-1) ? ",\n" : "\n");
      os<<"]\n";
      return os;
    }

    /** Get the ALSA mixer information in a JSON string
    \return Mixer state in a JSON string.
    */
    std::string getState(){
      std::ostringstream oss;
      oss<<*this;
      return oss.str();
    }

    /** Given a mixer element name, return the index of the element in the mixerElements
    */
    int getElementIndex(std::string name){
      for (int i=0; i<mixerElements.size(); i++)
        if (name.compare(mixerElements[i].name)==0)
          return i;
      return -1;
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The volume to set.
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    long setPlaybackVol(std::string name, long v){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setPlaybackVol(v);
    }

    /** Set the volume of a mixer element's channel, given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set.
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    long setPlaybackChVol(std::string name, snd_mixer_selem_channel_id_t channel, long v){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setPlaybackVol(channel, v);
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The volume to set.
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setPlaybackVolJSON(std::string name, long v){
      long val=setPlaybackVol(name, v);
      std::ostringstream ret;
      if (val<0)
        ret<<"{\"name\":\""<<name<<"\",\"error\":"<< val <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"val\":"<<val<<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set.
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setPlaybackChVolJSON(std::string name, snd_mixer_selem_channel_id_t channel, long v){
      long val=setPlaybackChVol(name, channel, v);
      std::ostringstream ret;
      if (val<0)
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"error\":"<< val <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"val\":"<<val<<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set.
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setPlaybackChVolJSON(std::string name, int channel, long v){
      return setPlaybackChVolJSON(name, (snd_mixer_selem_channel_id_t) channel, v);
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The volume to set - updated on return to the actual value set.
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    int setPlaybackVolDB(std::string name, double &dB){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setPlaybackVolDB(dB);
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set - updated on return to the actual value set.
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    int setPlaybackChVolDB(std::string name, snd_mixer_selem_channel_id_t channel, double &dB){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setPlaybackVolDB(channel, dB);
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned in a JSON format
    \param name The name of the mixer element to change
    \param v The volume to set - not updated on return
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setPlaybackVolDBJSON(std::string name, double dB){
      int err=setPlaybackVolDB(name, dB);
      std::ostringstream ret;
      if (err<0)
        ret<<"{\"name\":\""<<name<<"\",\"error\":"<< err <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"val\":"<< dB <<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned in a JSON format
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set - not updated on return
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setPlaybackChVolDBJSON(std::string name, snd_mixer_selem_channel_id_t channel, double dB){
      int err=setPlaybackChVolDB(name, channel, dB);
      std::ostringstream ret;
      if (err<0)
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"error\":"<< err <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"val\":"<< dB <<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned in a JSON format
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set - not updated on return
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setPlaybackChVolDBJSON(std::string name, int channel, double dB){
      return setPlaybackChVolDBJSON(name, (snd_mixer_selem_channel_id_t) channel, dB);
    }

    /** Set the switch state of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The state to set.
    \return <0 on error otherwise, the current state of the mixer element's switch.
    */
    int setPlaybackSwitch(std::string name, int v){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setPlaybackSwitch(v);
    }

    /** Set the switch state of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The switch state to set - not updated on return
    \return JSON object, with val<0 on error otherwise, the current state of the mixer element's switch
    */
    std::string setPlaybackSwitchJSON(std::string name, int v){
      int err=setPlaybackSwitch(name, v);
      std::ostringstream ret;
      if (err<0)
        ret<<"{\"name\":\""<<name<<"\",\"error\":"<< err <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"switch\":"<< err <<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The volume to set.
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    long setCaptureVol(std::string name, long v){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setCaptureVol(v);
    }

    /** Set the volume of a mixer element's channel, given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set.
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    long setCaptureVol(std::string name, snd_mixer_selem_channel_id_t channel, long v){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setCaptureVol(channel, v);
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The volume to set.
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setCaptureVolJSON(std::string name, long v){
      long val=setCaptureVol(name, v);
      std::ostringstream ret;
      if (val<0)
        ret<<"{\"name\":\""<<name<<"\",\"error\":"<< val <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"val\":"<<val<<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set.
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setCaptureChVolJSON(std::string name, snd_mixer_selem_channel_id_t channel, long v){
      long val=setCaptureVol(name, channel, v);
      std::ostringstream ret;
      if (val<0)
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"error\":"<< val <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"val\":"<<val<<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set.
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setCaptureChVolJSON(std::string name, int channel, long v){
      return setCaptureChVolJSON(name, (snd_mixer_selem_channel_id_t) channel, v);
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The volume to set - the value is updated on return
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    int setCaptureVolDB(std::string name, double &dB){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setCaptureVolDB(dB);
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set - the value is updated on return
    \return <0 on error otherwise, the current volume of the mixer element.
    */
    int setCaptureChVolDB(std::string name, snd_mixer_selem_channel_id_t channel, double &dB){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setCaptureVolDB(channel, dB);
    }

    /** Set the volume of a mixer element given the name. The resulting state of the mixer element is returned in the JSON string
    \param name The name of the mixer element to change
    \param v The volume to set - the value is not updated on return
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setCaptureVolDBJSON(std::string name, double dB){
      int err=setCaptureVolDB(name, dB);
      std::ostringstream ret;
      if (err<0)
        ret<<"{\"name\":\""<<name<<"\",\"error\":"<< err <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"val\":"<< dB <<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned in the JSON string
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set - the value is not updated on return
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setCaptureChVolDBJSON(std::string name, snd_mixer_selem_channel_id_t channel, double dB){
      int err=setCaptureChVolDB(name, channel, dB);
      std::ostringstream ret;
      if (err<0)
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"error\":"<< err <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"channel\":"<< channel<<",\"val\":"<< dB <<"}";
      return ret.str();
    }

    /** Set the volume of a mixer element's channel given the name. The resulting state of the mixer element is returned in the JSON string
    \param name The name of the mixer element to change
    \param channel The channel to set the volume on
    \param v The volume to set - the value is not updated on return
    \return JSON object, with val<0 on error otherwise, the current volume of the mixer element.
    */
    std::string setCaptureChVolDBJSON(std::string name, int channel, double dB){
      return setCaptureChVolDBJSON(name, (snd_mixer_selem_channel_id_t) channel, dB);
    }
    /** Set the switch state of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The switch state to set.
    \return <0 on error otherwise, the current state of the mixer element's switch
    */
    int setCaptureSwitch(std::string name, int v){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setCaptureSwitch(v);
    }

    /** Set the switch state of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The switch state to set.
    \return JSON object, with val<0 on error otherwise, the current state of the mixer element's swtich
    */
    std::string setCaptureSwitchJSON(std::string name, int v){
      int err=setCaptureSwitch(name, v);
      std::ostringstream ret;
      if (err<0)
        ret<<"{\"name\":\""<<name<<"\",\"error\":"<< err <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"switch\":"<< err <<"}";
      return ret.str();
    }
  };
}
#endif // MIXER_H_
