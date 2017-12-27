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

#include "ALSA/ALSAPlugin.H"
using namespace std;
using namespace ALSA;

#include <sstream>

// class ALSAPluginTest : public ALSAPlugin {
// 	virtual int parseConfig(const char *name, snd_config_t *conf, snd_pcm_stream_t stream, int mode){

//         snd_config_iterator_t i, next;
//         snd_config_for_each(i, next, conf) {
//             snd_config_t *n = snd_config_iterator_entry(i);
//             const char *id;
//             if (snd_config_get_id(n, &id) < 0)
//                 continue;

//             if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0 || strcmp(id, "hint") == 0)
//                 continue;

//             if (strcmp(id, "card") == 0) {
//                 if (snd_config_get_string(n, &card) < 0) {
//                     long val;
//                     err = snd_config_get_integer(n, &val);
//                     if (err < 0) {
//                         SNDERR("Invalid type for %s", id);
//                         return -EINVAL;
//                     }
//                     char tmpcard[8];
//                     snprintf(tmpcard, sizeof(tmpcard), "%ld", val);
//                     card = tmpcard;
//                 }
//                 continue;
//             }
// 			if (strcmp(id, "slave") == 0) {
// 				if (snd_config_get_string(n, &slave_string) < 0) {
// 					SNDERR("slave must be a string");
// 					return -EINVAL;
// 				}
// 				continue;
// 			}

//         }
// 		return parseConfig(name, conf, stream, mode);
//     }
// };

// SND_PCM_PLUGIN_DEFINE_FUNC(ALSAPluginTester){
//     ALSAPlugin aPlugin;
//     aPlugin.parseConfig(config);
// }

// SND_PCM_PLUGIN_SYMBOL(ALSAPluginTester);

class ALSAPluginTest : public ALSAPlugin {
    /** Start method
    */
    // virtual int start(){
    // 	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
    // 	return 0;
    // }

    /** Start method
    */
    // virtual int stop(){
    // 	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
    // 	return 0;
    // }

    /** Pointer method
    */
    virtual snd_pcm_sframes_t pointer(){
    	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
    	return 0;
    }
public:
	ALSAPluginTest(){
		assert("not debugged");
    	std::cout<<__func__<<std::endl;
		setName("ALSAPluginTest");
	}

	virtual ~ALSAPluginTest(){
    	std::cout<<__func__<<std::endl;
	}

 	virtual int parseConfig(const char *name, snd_config_t *conf, snd_pcm_stream_t stream, int mode){
    	//std::cout<<typeid(*this).name()<<'\t'<<__func__<<std::endl;
		const char *card = NULL;
		const char *pcm_string = NULL;

		if (stream != SND_PCM_STREAM_PLAYBACK) {
			ostringstream oss;
			oss<<name<<" : is only for playback.";
			SNDERR(oss.str().c_str());
			return -EINVAL;
		}

 		int ret=ALSAPlugin::parseConfig(name, conf, stream, mode);
 		if (ret<0)
 			return ret;

		snd_config_iterator_t i, next;
		snd_config_for_each(i, next, conf) {
			snd_config_t *n = snd_config_iterator_entry(i);
			const char *id;
			if (snd_config_get_id(n, &id) < 0)
				continue;
			if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0 || strcmp(id, "hint") == 0)
				continue;

			if (strcmp(id, "card") == 0) {
				if (snd_config_get_string(n, &card) < 0) {
					long val;
					ret = snd_config_get_integer(n, &val);
					if (ret < 0) {
						SNDERR("Invalid type for %s", id);
						return -EINVAL;
					}
					char tmpcard[8];
					snprintf(tmpcard, sizeof(tmpcard), "%ld", val);
					card = tmpcard;
				}
				continue;
			}
			if (strcmp(id, "slave") == 0) {
				if (snd_config_get_string(n, &pcm_string) < 0) {
					SNDERR("slave must be a string");
					return -EINVAL;
				}
				continue;
			}

		}

		if (! pcm_string) {
	    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
			ostringstream oss;
			if (card)
				oss<<"CARD "<<card;
			ret = snd_pcm_open(&slave, oss.str().c_str(), stream, mode);
			if (ret < 0)
				return ret;
//			ret = snd_pcm_lfloat_open(&slave, NULL, SND_PCM_FORMAT_FLOAT_LE, slave, 1);
//			if (ret < 0)
				return ret;
		} else {
	    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
			ret = snd_pcm_open(&slave, pcm_string, stream, mode);
			if (ret < 0)
				return ret;
		}

		return ret;
	}

	virtual int HWParams(snd_pcm_hw_params_t *params){
    	std::cout<<__func__<<std::endl;
		return 0;
	}

	// virtual int setupHWParams(){
 //    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
	// 	int ret;
 //        if ((ret=fillParams())<0){
 //            std::cerr<<"Couldn't fillParams"<<std::endl;
 //        	return ret;
 //        }

 //    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
 //        if ((ret=rateResample(0))<0){
 //            std::cerr<<"Couldn't remove rateResample"<<std::endl;
 //        	return ret;
 //        }
 //    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
 //        if ((ret=setAccess(SND_PCM_ACCESS_RW_INTERLEAVED))<0){
 //            std::cerr<<"Couldn't setAccess"<<std::endl;
 //        	return ret;
 //        }
 //    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
 //        return ret;
	// }
};

ALSAPluginTest aPlugin;
extern "C" SND_PCM_PLUGIN_DEFINE_FUNC(ALSAPluginTest){
    	std::cout<<__func__<<std::endl;
    aPlugin.parseConfig(name, conf, stream, mode);

	int ret=aPlugin.create(name, stream, mode);
	if (ret<0)
		return ret;

	// ret=aPlugin.setupHWParams();
	// if (ret<0)
	// 	return ret;

	*pcmp=aPlugin.getPCM();
	std::cout<<__func__<<" returning "<<std::endl;
    return 0;
}

SND_PCM_PLUGIN_SYMBOL(ALSAPluginTest);
