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
#ifndef ALSAMIXEREVENTS_H_
#define ALSAMIXEREVENTS_H_

#include <ALSA/Mixer.H>
#include "Thread.H"

namespace ALSA {
  /** Handle mixer events with this class.
  \example ALSAMixerEvents.C
  */
  class MixerEvents : public Mixer, public ThreadedMethod {

    void *threadMain(void){
      int ret=0;
      while (ret>=0) {
        int res = snd_mixer_wait(handle, -1);
        if (res >= 0)
          ret = snd_mixer_handle_events(handle);
      }
      ALSADebug().evaluateError(ret); // print the error before returning
      return NULL;
    }

    static int handleValueEventsStatic(snd_mixer_elem_t *elem, unsigned int mask) {
    	if (mask & SND_CTL_EVENT_MASK_VALUE){
        MixerElement me(elem);
        return static_cast<MixerEvents *>(snd_mixer_elem_get_callback_private(elem))->handleValueEvents(me);
      }
    	return 0;
    }

    /** Enables the ALSA element's callback
    \param elem the alsa element
    */
    virtual void addElem(snd_mixer_elem_t *elem){
      Mixer::addElem(elem);
      snd_mixer_elem_set_callback_private(elem, this);
      snd_mixer_elem_set_callback(elem, handleValueEventsStatic);
    }

protected:

  /** This method should be overloaded. A new mixer element event has occurred. Handle it here.
  \param me The current state of the mixer element
  \return 0 on success
  */
  virtual int handleValueEvents(MixerElement &me) {
    std::cout<<me<<std::endl;
    std::cout<<"MixerEvents::"<<__func__<<" not overriden."<<std::endl;
    return 0;
  }
public:

    /** load the mixer elements then run the event catching thread
    \return 0 on success, otherwise the error.
    */
    virtual int load(){
        int err = Mixer::load();
        if (err==0) {
          unload(); // close if any open
          err=run(); // start the thread
        }
        return err;
    }

    /** stop the thread
    \return 0 on success, otherwise the error.
    */
    virtual int unload(){
      if (running()) // if the thread is running
        stop(); // stop the polling thread
      return 0;
    }

    /** Close the control
    This method also closes the mixer.
    \return 0 on success, otherwise the error.
    */
    int close(){
      return unload();
    }

  };
}
#endif // ALSAMIXEREVENTS_H_
