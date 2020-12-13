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
#ifndef ALSACONTROL_H_
#define ALSACONTROL_H_

#include <ALSA/Mixer.H>
#include "PollThreaded.H"

namespace ALSA {
  /** alsactl interface
  Control inherits from ALSAMixer. If you setup controls you can also do other tasks with the mixer.
  For example if you monitor controls, when the value changes, you can find that value with this class as well.

  This class opens one control.
  */
  class Control : public Mixer, public PollThreaded {
    snd_ctl_t *ctl; ///< The control pointer

    /** Poll found events, handle events.
    \return <0 to stop the thread processing and let it exit.
    */
    int processPollEvents(){
      unsigned short revents;
      snd_ctl_poll_descriptors_revents(ctl, &fds, 1, &revents);
      if (revents & POLLIN){
        snd_ctl_event_t *event;
        snd_ctl_event_alloca(&event);
        int err = snd_ctl_read(ctl, event);
        if (err < 0)
          return ALSADebug().evaluateError(err, "snd_ctl_read error when handling poll events\n");
        if (snd_ctl_event_get_type(event) != SND_CTL_EVENT_ELEM)
		      return 0;
        std::string elemName(snd_ctl_event_elem_get_name(event));
        MixerElement *me=getElement(elemName.substr(0, elemName.find(' ')));
        return processEvent(*me);
      }

      //   print_event(show_cards ? i : -1, ctls[i]);
      return 0;
    }

    /** Overloadable event which is called once a mixer element is changed.
    \param me The mixer element which is altered.
    */
    virtual int processEvent(MixerElement &me){
      std::cout<<me<<std::endl;
      return 0;
    }

  public:
    Control(){
      ctl=NULL;
    }

    virtual ~Control(){
      close();
    }

    /** Open a card for control
    \return <0 on error.
    */
    virtual int attach(std::string card){
      Mixer::attach(card);
      detach(); // close if any open
      int err = snd_ctl_open(&ctl, card.c_str(), SND_CTL_READONLY);
      if (err < 0)
        return ALSADebug().evaluateError(err, "Can't open the card for Control\n");
      if ((err=snd_ctl_subscribe_events(ctl, 1))<0){
        detach();
        return ALSADebug().evaluateError(err, "Can't subscribe to events for Control\n");
      }

      snd_ctl_poll_descriptors(ctl, &fds, 1); // get the polling file descriptors

      run(); // start the polling thread
      return err;
    }

    /** Free the control pointer.
    \return 0 on success, otherwise the error.
    */
    int detach(){
      if (running()) // if the thread is running
        stop(); // stop the polling thread
      if (ctl)
    		snd_ctl_close(ctl);
      ctl=NULL;
      return 0;
    }

    /** Close the control
    This method also closes the mixer.
    \return 0 on success, otherwise the error.
    */
    int close(){
      // Mixer::close();
      return detach();
    }

  };
}
#endif // ALSACONTROL_H_
