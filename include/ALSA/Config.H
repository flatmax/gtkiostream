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
#ifndef ALSACONFIG_H_
#define ALSACONFIG_H_

#include <ALSA/ALSA.H>
#include <sstream>

namespace ALSA {
  /** A class to get keys from ALSA configuration files such as alsa.conf and .asoundrc files.
  */
  class Config {
    snd_config_t *cfg; ///< The config pointer

    /** If cfg is valid, delete it and set it to null.
    */
    void destroy(){
      if (cfg)
        snd_config_delete(cfg);
      cfg=NULL;
    }

  public:
    /// Constructor
    Config(){
      cfg=NULL;
    }

    /// Destructor
    virtual ~Config(){
      destroy();
    }

    /** load and parse an ALSA configuration file
    \param file The filename (fully pathed) to load
    \return 0 on success.
    */
    int load(std::string file){
      destroy();
      int ret=snd_config_top(&cfg);
      if (ret)
        return ALSADebug().evaluateError(ret," When attempting to open the file.\n");

      FILE *fp = fopen(file.c_str(), "r");
      if (fp == NULL) {
        ALSADebug().evaluateError(ALSA_CONFIG_NOT_OPEN_ERROR," When attempting to fopen the file.\n");
        return -errno;
      }

      snd_input_t *in;
      ret = snd_input_stdio_attach(&in, fp, 1);
      if (ret < 0) {
        fclose(fp);
        SNDERR("could not attach stdio %s", file.c_str());
        return ret;
      }

      ret = snd_config_load(cfg, in);
      snd_input_close(in);
      if (ret){
        ALSADebug().evaluateError(ret);
        return ALSADebug().evaluateError(ALSA_CONFIG_NOT_OPEN_ERROR," When attempting to open the file.\n");
      }
      return 0;
    }

    /** Search through an opened config file for a key. From the alsa-lib doc, the keys are in the # comments :
    config {
        a 42               # "a"
        b {                # "b"
            c "cee"        # "b.c"
            d {            # "b.d"
                e 2.71828  # "b.d.e"
            }
        }
    }
    \param key The key to search for
    \param ret [out] The value of the option matching key, if found, then it is set to that value, otherwise left the same
    \return 0 on success error otherwise
    \tparam TYPE The type of the output argument
    */
    template<typename TYPE>
    int search(std::string key, TYPE &ret){
      CFG_NOT_OPEN_CHECK_NO_PRINT(cfg,int) // check config is open
      std::stringstream errMsg;
      snd_config_t *conf;
      int err=snd_config_search(cfg, key.c_str(), &conf);
      if (err){
        errMsg<<"Couldn't find a configuration matching the key : "<<key<<'\n';
        return ALSADebug().evaluateError(err, errMsg.str());
      }
      std::stringstream arg;
      switch (snd_config_get_type(conf)){
        case SND_CONFIG_TYPE_INTEGER:
          long vall;
          err=snd_config_get_integer(conf, &vall);
          arg<<vall;
          break;
        case SND_CONFIG_TYPE_INTEGER64:
          long long valll;
          err=snd_config_get_integer64(conf, &valll);
          arg<<valll;
          break;
        case SND_CONFIG_TYPE_REAL:
          double vald;
          err=snd_config_get_real(conf, &vald);
          arg<<vald;
          break;
        case SND_CONFIG_TYPE_STRING:
          const char *valc;
          err=snd_config_get_string(conf, &valc);
          arg<<valc;
          break;
        case SND_CONFIG_TYPE_POINTER:
        case SND_CONFIG_TYPE_COMPOUND:
          errMsg<<" When attempting to convert the key "<<key<<" which was in the config file as a pointer or compound config. Currently only handling int, int64, real and string types.\n";
          return ALSADebug().evaluateError(ALSA_UNHANDLED_TYPE, errMsg.str());
          break;
      }
      arg>>ret;
      return 0;
    }

    /** Check if a key exists
    */
    int keyExists(std::string key){
      CFG_NOT_OPEN_CHECK_NO_PRINT(cfg,int) // check config is open
      std::stringstream errMsg;
      snd_config_t *conf;
      int err=snd_config_search(cfg, key.c_str(), &conf);
      if (err){
        errMsg<<"Couldn't find a configuration matching the key : "<<key<<'\n';
        return ALSADebug().evaluateError(err, errMsg.str());
      }
      return 0;
    }
  };
}
#endif // ALSACONFIG_H_
