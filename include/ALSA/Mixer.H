/* Copyright 2000-2021 Matt Flax <flatmax@flatmax.org>
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

#include <ALSA/MixerElement.H>

namespace ALSA {
  #define MIXER_NOT_OPEN_CHECK_NO_PRINT(handle, type) {if (!handle) return (type)ALSADebug().evaluateError(ALSA_MIXER_NOT_OPEN_ERROR);}
	#define MIXER_NOT_OPEN_CHECK_TYPED(handle, type) {printf("\t\tfunc: %s\n",__func__); MIXER_NOT_OPEN_CHECK_NO_PRINT(handle, type)}
	#define MIXER_NOT_OPEN_CHECK(handle) MIXER_NOT_OPEN_CHECK_TYPED(handle, int)

  /** Class to alter the ALSA hardware mixer.
  This class holds all of the mixer controls. You can set mixer element values, like for example volume or volume in dB :
  \code
  mixer.setPlaybackVol("Master", 50)
  mixer.setPlaybackVolDB("Master", dB)
  \endcode
  */
  class Mixer {
  protected:
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
    virtual int open(){
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
    virtual int close(){
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
    virtual int attach(std::string card){
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
    virtual int registerMixer(){
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
    virtual int load(){
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
    virtual void scanMixer(){
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

    /** Get the ALSA mixer information in a JSON string
    \param name The name of the mixer element to get the state of
    \return Mixer state in a JSON string.
    */
    std::string getElementState(std::string name){
      int i=getElementIndex(name);
      std::ostringstream oss;
      if (i<0)
        oss<<"{\"name\":\""<<name<<"\",\"error\":"<< ALSA_MIXER_FIND_ELEMENT_ERROR <<"}";
      else
        oss<<mixerElements[i];
      return oss.str();
    }

    /** Given a mixer element name, return the index of the element in the mixerElements
    */
    int getElementIndex(std::string name){
      if (mixerElements.size()==0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, " element count is 0, have you called Mixer::scanMixer() ?\n");
      for (int i=0; i<mixerElements.size(); i++){
        if (name.compare(mixerElements[i].name)==0)
          return i;
      }
      return -1;
    }

    // /** Given a mixer element name, return the index of the element in the mixerElements
    // */
    // int getElementIndex(unsigned int id){
    //   for (int i=0; i<mixerElements.size(); i++)
    //   if (mixerElements[i].id==id)
    //     return i;
    //   return -1;
    // }
    //
    // /** Given an element ID, return a pointer to the element.
    // \param id The mixer element ID
    // \return NULL on error, or a pointer to the element on success.
    // */
    // MixerElement* getElement(unsigned int id){
    //   int i=getElementIndex(id);
    //   if (i>=0 && i<mixerElements.size())
    //     return &mixerElements[i];
    //   return NULL;
    // }

    /** Given an element ID, return a pointer to the element.
    \param name The mixer element name
    \return NULL on error, or a pointer to the element on success.
    */
    MixerElement* getElement(std::string name){
      int i=getElementIndex(name);
      if (i<0)
        return NULL;
      return &mixerElements[i];
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
    int setSwitch(std::string name, int v){
      int i=getElementIndex(name);
      if (i<0)
        return ALSADebug().evaluateError(ALSA_MIXER_FIND_ELEMENT_ERROR, name+'\n');
      return mixerElements[i].setSwitch(v);
    }

    /** Set the switch state of a mixer element given the name. The resulting state of the mixer element is returned
    \param name The name of the mixer element to change
    \param v The switch state to set - not updated on return
    \return JSON object, with val<0 on error otherwise, the current state of the mixer element's switch
    */
    std::string setSwitchJSON(std::string name, int v){
      int err=setSwitch(name, v);
      std::ostringstream ret;
      if (err<0)
        ret<<"{\"name\":\""<<name<<"\",\"error\":"<< err <<"}";
      else
        ret<<"{\"name\":\""<<name<<"\",\"switch\":"<< err <<"}";
      return ret.str();
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
