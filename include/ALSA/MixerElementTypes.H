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
#ifndef MIXERELEMENTTYPES_H_
#define MIXERELEMENTTYPES_H_

namespace ALSA {

  /** Common ALSA channel element
  */
  class ChannelElement {
  public:
    snd_mixer_selem_channel_id_t channel; ///< The ALSA channel ID

    /** Constructor taking the channel ID
    Sets levels and dB to 0.
    */
    ChannelElement(snd_mixer_selem_channel_id_t ch){
      channel=ch;
    }

    virtual ~ChannelElement(){} ///< Destructor
  };

  /** Class to manage a channel volumes
  */
  class ChannelVolumeElement : public ChannelElement {
  protected:
    long volDB; ///< If the channel has a volume, this holds its state in 100*dB
  public:
    long vol; ///< If the channel has a volume, this holds its state
    double dB; ///< If the channel has a volume, this holds its state in dB
    int sw; ///< If the channel has a switch, this holds its state

    /** Constructor taking the channel ID
    Sets levels and dB to 0.
    */
    ChannelVolumeElement(snd_mixer_selem_channel_id_t ch) : ChannelElement(ch) {
      vol=volDB=0;
      dB=0.;
      channel=ch;
    }

    virtual ~ChannelVolumeElement(){} ///< Destructor

    /** Convert from ALSA dB representation to dB
    \param v The ALSA representation
    \return v/100 The dB equivalent.
    */
    double toDB(long v) const {
      return (double)v/100.;
    }

    /** Convert from dB to ALSA representation (*100)
    \param d The dB to convert
    \return d*100
    */
    long fromDB(double d) const {
      return (long)(d*100.);
    }

    /** Form a JSON structure of the channel, level and dB states
    \param os The output stream to load.
    */
    void stringifyLevel(std::ostream &os){
      os<<"{\"channel\": "<<channel<<", "<<"\"level\":"<<vol<<", \"dB\":"<<dB<<"}";
    }

    /** Form a JSON structure of the channel and switch state
    \param os The output stream to load.
    */
    void stringifySwitch(std::ostream &os){
      os<<"{\"channel\": "<<channel<<", "<<"\"switch\":"<<sw<<"}";
    }

    /** Form a JSON structure of the channel level, dB and switch state
    \param os The output stream to load.
    */
    void stringifyLevelAndSwitch(std::ostream &os){
      os<<"{\"channel\": "<<channel<<", "<<"\"level\":"<<vol<<", \"dB\":"<<dB
            <<", "<<"\"switch\":"<<sw<<"}";
    }
  };

  /** Class to manage a playback channel volumes
  */
  class PlaybackChannelVolumeElement : public ChannelVolumeElement {
  public:
    PlaybackChannelVolumeElement(snd_mixer_selem_channel_id_t ch) : ChannelVolumeElement(ch) {}

    virtual ~PlaybackChannelVolumeElement(){}

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
  class CaptureChannelVolumeElement : public ChannelVolumeElement {
  public:
    CaptureChannelVolumeElement(snd_mixer_selem_channel_id_t ch) : ChannelVolumeElement(ch) {}
    virtual ~CaptureChannelVolumeElement(){}

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

  /** Class to manage a channel enum elements
  */
  class ChannelEnumElement : public ChannelElement {
    char name[128]; ///< The name of this enum element
    unsigned int idx; ///< The current index state of this enum element
  public:
    /** Constructor taking the channel ID
    */
    ChannelEnumElement(snd_mixer_selem_channel_id_t ch) : ChannelElement(ch) {
    }

    virtual ~ChannelEnumElement(){} ///< Destructor

    /** Get the name of this enum element (knowing its channel or index number)
    \param elem the mixer element to get the name from
    \return <0 on error
    */
    int getName(snd_mixer_elem_t *elem){
      int ret=snd_mixer_selem_get_enum_item_name(elem, channel, sizeof(name), name);
      if (ret)
        ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "Get channel enum's name\n");
      return ret;
    }

    /** Get the name of this enum element (knowing its channel or index number)
    \param elem the mixer element to get the name from
    \return <0 on error
    */
    int getIndex(snd_mixer_elem_t *elem){
      int ret=snd_mixer_selem_get_enum_item(elem, channel, &idx);
      if (ret)
        ALSADebug().evaluateError(ALSA_MIXER_QUERY_ERROR, "Get channel enum's state\n");
      return ret;
    }

    /** Form a JSON structure of the channel enum
    \param os The output stream to load.
    */
    void stringifyEnumName(std::ostream &os){
      os<<"{\"channel\": "<<channel<<", "<<"\"name\":\""<<name<<", "<<"\"index\":\""<<idx<<"\"}";
    }
  };

  /** Class to manage playback channel enum elements
  */
  class PlaybackChannelEnumElement : public ChannelEnumElement {
  public:
    PlaybackChannelEnumElement(snd_mixer_selem_channel_id_t ch) : ChannelEnumElement(ch) {}
    virtual ~PlaybackChannelEnumElement(){}
  };

  /** Class to manage capture channel enum elements
  */
  class CaptureChannelEnumElement : public ChannelEnumElement {
  public:
    CaptureChannelEnumElement(snd_mixer_selem_channel_id_t ch) : ChannelEnumElement(ch) {}
    virtual ~CaptureChannelEnumElement(){}
  };
}
#endif // MIXERELEMENTTYPES_H_
