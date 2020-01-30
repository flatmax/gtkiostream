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
#ifndef MIXERELEMENT_H_
#define MIXERELEMENT_H_

#include <ALSA/MixerElementTypes.H>

#include <string>
#include <sstream>

namespace ALSA {

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
    bool isPlaybackEnum; ///<0 if no control is present, 1 if it's present
    bool isCaptureEnum; ///<0 if no control is present, 1 if it's present

    std::vector<PlaybackChannelVolumeElement> playbackVolumes; ///< Volume setting for each playback channel in this mixer element
    std::vector<CaptureChannelVolumeElement> captureVolumes; ///< Volume setting for each capture channel in this mixer element
    std::vector<PlaybackChannelEnumElement> playbackEnums; ///< Names (inherit indexes) for each enumerated playback channel in this mixer element
    std::vector<CaptureChannelEnumElement> captureEnums; ///< Names (inherit indexes) for each enumerated capture channel in this mixer element
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
      isPlaybackEnum=snd_mixer_selem_is_enum_playback(elem);
      isCaptureEnum=snd_mixer_selem_is_enum_capture(elem);

      unsigned long chMask=1;
      for (int i=0; i<SND_MIXER_SCHN_LAST; i++)
        if (snd_mixer_selem_has_playback_channel(elem, (snd_mixer_selem_channel_id_t)i)>0){
          playbackVolumes.push_back(PlaybackChannelVolumeElement((snd_mixer_selem_channel_id_t)i));
          if (hasPlaybackVolume || hasPlaybackVolumeJoined)
            playbackVolumes[playbackVolumes.size()-1].getLevels(elem);
          if (hasCommonSwitch || hasPlaybackSwitch || hasPlaybackSwitchJoined)
            playbackVolumes[playbackVolumes.size()-1].getSwitch(elem);
        }

      for (int i=0; i<SND_MIXER_SCHN_LAST; i++)
        if (snd_mixer_selem_has_capture_channel(elem, (snd_mixer_selem_channel_id_t)i)>0){
          captureVolumes.push_back(CaptureChannelVolumeElement((snd_mixer_selem_channel_id_t)i));
          if (hasCaptureVolume || hasCaptureVolumeJoined)
            captureVolumes[captureVolumes.size()-1].getLevels(elem);
          if (hasCommonSwitch || hasCaptureSwitch || hasCaptureSwitchJoined)
            captureVolumes[captureVolumes.size()-1].getSwitch(elem);
        }

      if (isPlaybackEnum){
        for (int i=0; i<snd_mixer_selem_get_enum_items(elem); i++){
          playbackEnums.push_back(PlaybackChannelEnumElement((snd_mixer_selem_channel_id_t)i));
          playbackEnums[playbackEnums.size()-1].getName(elem);
          playbackEnums[i].getIndex(elem); // The index of the enum element is common assumed at index zero
        }
      }

      if (isCaptureEnum){
        for (int i=0; i<snd_mixer_selem_get_enum_items(elem); i++){
          captureEnums.push_back(CaptureChannelEnumElement((snd_mixer_selem_channel_id_t)i));
          captureEnums[captureEnums.size()-1].getName(elem);
          captureEnums[i].getIndex(elem); // The index of the enum element is common assumed at index zero
        }
      }

      if (hasPlaybackVolume || hasPlaybackVolumeJoined){
        if (snd_mixer_selem_get_playback_volume_range(elem, &playbackVolMin, &playbackVolMax))
          ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "playback volume range");
        long min, max;
        if (snd_mixer_selem_get_playback_dB_range(elem, &min, &max))
          ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "playback dB range");
        playbackVolMinDB=ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).toDB(min); // dummy id
        playbackVolMaxDB=ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).toDB(max); // dummy id
      }

      if (hasCaptureVolume || hasCaptureVolumeJoined){
        if (snd_mixer_selem_get_capture_volume_range(elem, &captureVolMin, &captureVolMax))
          ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "capture volume range");
        long min, max;
        if (snd_mixer_selem_get_capture_dB_range(elem, &min, &max))
          ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "capture dB range");
        captureVolMinDB=ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).toDB(min); // dummy id
        captureVolMaxDB=ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).toDB(max); // dummy id
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
        if ((err=snd_mixer_selem_set_playback_dB(elem, playbackVolumes[i].channel, ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).fromDB(dB), 1))<0)
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
          if ((err=snd_mixer_selem_set_playback_dB(elem, playbackVolumes[i].channel, ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).fromDB(dB),1))<0) // dummy id
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
        if ((err=snd_mixer_selem_set_capture_dB(elem, captureVolumes[i].channel, ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).fromDB(dB), 1))<0)
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
          if ((err=snd_mixer_selem_set_capture_dB(elem, captureVolumes[i].channel, ChannelVolumeElement((snd_mixer_selem_channel_id_t)0).fromDB(dB),1))<0) // dummy id
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
      os<<"\"hasCaptureSwitchExclusive\":"<<me.hasCaptureSwitchExclusive<<",\n";
      os<<"\"isPlaybackEnum\":"<<me.isPlaybackEnum<<",\n";
      os<<"\"isCaptureEnum\":"<<me.isCaptureEnum<<"\n";

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
          me.playbackVolumes[i].stringifyLevelAndSwitch(os);
          os<<((i<me.playbackVolumes.size()-1) ? ",\n" : "\n");
        }
        os<<"]\n";
      } else
          if (me.hasPlaybackVolume || me.hasPlaybackVolumeJoined){
            os<<",\"playbackVolumes\":[\n";
            for (int i=0; i<me.playbackVolumes.size(); i++){
              me.playbackVolumes[i].stringifyLevel(os);
              os<<((i<me.playbackVolumes.size()-1) ? ",\n" : "\n");
            }
            os<<"]\n";
          } else
            if (me.hasCommonSwitch || me.hasPlaybackSwitch || me.hasPlaybackSwitchJoined){
              os<<",\"playbackVolumes\":[\n";
              for (int i=0; i<me.playbackVolumes.size(); i++){
                me.playbackVolumes[i].stringifySwitch(os);
                os<<((i<me.playbackVolumes.size()-1) ? ",\n" : "\n");
              }
              os<<"]\n";
            }

        if ((me.hasCaptureVolume || me.hasCaptureVolumeJoined) && (me.hasCommonSwitch || me.hasCaptureSwitch || me.hasCaptureSwitchJoined)){
          os<<",\"captureVolumes\":[\n";
          for (int i=0; i<me.captureVolumes.size(); i++){
            me.captureVolumes[i].stringifyLevelAndSwitch(os);
            os<<((i<me.captureVolumes.size()-1) ? ",\n" : "\n");
          }
          os<<"]\n";
        } else
            if (me.hasCaptureVolume || me.hasCaptureVolumeJoined){
              os<<",\"captureVolumes\":[\n";
              for (int i=0; i<me.captureVolumes.size(); i++){
                me.captureVolumes[i].stringifyLevel(os);
                os<<((i<me.captureVolumes.size()-1) ? ",\n" : "\n");
              }
              os<<"]\n";
            } else
              if (me.hasCommonSwitch || me.hasCaptureSwitch || me.hasCaptureSwitchJoined){
                os<<",\"captureVolumes\":[\n";
                for (int i=0; i<me.captureVolumes.size(); i++){
                  me.captureVolumes[i].stringifySwitch(os);
                  os<<((i<me.captureVolumes.size()-1) ? ",\n" : "\n");
                }
                os<<"]\n";
              }

      if (me.isCaptureEnum){
        os<<",\"captureEnums\":[\n";
        for (int i=0; i<me.captureEnums.size(); i++){
          me.captureEnums[i].stringifyEnumName(os);
          os<<((i<me.captureEnums.size()-1) ? ",\n" : "\n");
        }
        os<<"]\n";
      }

      os<<"}";
      return os;
    }
  };
}
#endif // MIXERELEMENT_H_
