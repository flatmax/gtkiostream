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
  /** alsactl interface
  MixerEvents inherits from ALSAMixer. If you setup controls you can also do other tasks with the mixer.
  For example if you monitor controls, when the value changes, you can find that value with this class as well.

  This class opens one control.
  */
  class MixerEvents : public Mixer, public ThreadedMethod {

    void *threadMain(void){
      int ret=0;
      while (ret>=0) {
        int res = snd_mixer_wait(handle, -1);
        if (res >= 0) {
          printf("events triggered\n");
          ret = snd_mixer_handle_events(handle);
        }
      }
      ALSADebug().evaluateError(ret); // print the error before returning
      return NULL;
    }

    static void sevents_value(snd_mixer_selem_id_t *sid)
    {
    	printf("event value: '%s',%i\n", snd_mixer_selem_id_get_name(sid), snd_mixer_selem_id_get_index(sid));
    }

    static int melem_event(snd_mixer_elem_t *elem, unsigned int mask)
    {
    	snd_mixer_selem_id_t *sid;
    	snd_mixer_selem_id_alloca(&sid);
    	snd_mixer_selem_get_id(elem, sid);
    	// if (mask == SND_CTL_EVENT_MASK_REMOVE) {
    	// 	sevents_remove(sid);
    	// 	return 0;
    	// }
    	// if (mask & SND_CTL_EVENT_MASK_INFO)
    	// 	sevents_info(sid);
    	if (mask & SND_CTL_EVENT_MASK_VALUE)
    		sevents_value(sid);
      printf("melem_event\n");
    	return 0;
    }

    /** Taken from amixer.c to add an element callback
    */
    static void sevents_add(snd_mixer_elem_t *elem)
    {
    	snd_mixer_selem_id_t *sid;
    	snd_mixer_selem_id_alloca(&sid);
    	snd_mixer_selem_get_id(elem, sid);
    	printf("event add: '%s',%i\n", snd_mixer_selem_id_get_name(sid), snd_mixer_selem_id_get_index(sid));
    	snd_mixer_elem_set_callback(elem, melem_event);
    }

    /** Taken from amixer.c to add an element
    */
    static int mixer_event(snd_mixer_t *mixer, unsigned int mask,
    		snd_mixer_elem_t *elem)
    {
    	if (mask & SND_CTL_EVENT_MASK_ADD)
    		sevents_add(elem);
    	return 0;
    }

    /** register the simple mixer element class
    \return 0 on success, otherwise the error.
    */
    virtual int registerMixer(){
      int ret=Mixer::registerMixer();
      if (ret==0)
        snd_mixer_set_callback(handle, mixer_event);
      return ret;
    }

  public:

    /** Open a card for control
    \return <0 on error.
    */
    virtual int attach(std::string card){
      int err = Mixer::attach(card);
      if (err==0) {
        detach(); // close if any open
        err=run(); // start the thread
      }
      return err;
    }

    /** Free the control pointer.
    \return 0 on success, otherwise the error.
    */
    virtual int detach(){
      if (running()) // if the thread is running
        stop(); // stop the polling thread
      return 0;
    }

    /** Close the control
    This method also closes the mixer.
    \return 0 on success, otherwise the error.
    */
    int close(){
      return detach();
    }

  };
}
#endif // ALSAMIXEREVENTS_H_
