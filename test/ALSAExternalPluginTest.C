/* Copyright 2000-2015 Matt Flax <flatmax@flatmax.org>
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

#include "ALSAExternalPlugin.H"
using namespace std;
using namespace ALSA;

class ALSAExternalPluginTest : public ALSAExternalPlugin {
		snd_pcm_format_t inFormat;
		snd_pcm_format_t outFormat;

public:
	ALSAExternalPluginTest(){
    	std::cout<<__func__<<std::endl;
		setName("ALSAExternalPluginTest");
	}

	virtual ~ALSAExternalPluginTest(){
    	std::cout<<__func__<<std::endl;
	}

 	virtual int parseConfig(const char *name, snd_config_t *conf, snd_pcm_stream_t stream, int mode){
    	std::cout<<typeid(*this).name()<<'\t'<<__func__<<std::endl;

		if (stream != SND_PCM_STREAM_PLAYBACK) {
			ostringstream oss;
			oss<<name<<" : is only for playback.";
			SNDERR(oss.str().c_str());
			return -EINVAL;
		}

 		return ALSAExternalPlugin::parseConfig(name, conf, stream, mode);
	}

	virtual int specifyHWParams(){
		std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
		int ret;
		snd_pcm_extplug_set_param_minmax(&extplug, SND_PCM_EXTPLUG_HW_CHANNELS, 2, 2);
		snd_pcm_extplug_set_slave_param(&extplug, SND_PCM_EXTPLUG_HW_CHANNELS, 2);
		cout<<Hardware::formatDescription(extplug.format)<<endl;
		cout<<Hardware::formatDescription(extplug.slave_format)<<endl;

		outFormat=SND_PCM_FORMAT_FLOAT_LE;
//		outFormat=SND_PCM_FORMAT_S32_LE;
		ret=snd_pcm_extplug_set_slave_param(&extplug, SND_PCM_EXTPLUG_HW_FORMAT, outFormat);
		cout<<ret<<endl;
//		inFormat=SND_PCM_FORMAT_S32_LE;
		inFormat=SND_PCM_FORMAT_FLOAT_LE;
		ret=snd_pcm_extplug_set_param(&extplug, SND_PCM_EXTPLUG_HW_FORMAT, inFormat);
		cout<<ret<<endl;

		return 0;
	}

	virtual snd_pcm_sframes_t transfer(const snd_pcm_channel_area_t *dst_areas, snd_pcm_uframes_t dst_offset, const snd_pcm_channel_area_t *src_areas, snd_pcm_uframes_t src_offset, snd_pcm_uframes_t size){
  //   	std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
		// std::cout<<"src : first="<<src_areas->first<<" step="<<src_areas->step<<'\t';
		// std::cout<<" : bytes : first="<<src_areas->first/8<<" step="<<src_areas->step/8<<std::endl;
		// std::cout<<"dest : first="<<dst_areas->first<<" step="<<dst_areas->step<<'\t';
		// std::cout<<" : bytes : first="<<dst_areas->first/8<<" step="<<dst_areas->step/8<<std::endl;
		int ret=snd_pcm_areas_copy(dst_areas, dst_offset, src_areas, src_offset, 2, size, SND_PCM_FORMAT_FLOAT_LE);
		float *fp=(float*)src_areas->addr;
		//cout<<size<<'\t'<<ret<<'\n';
		for (int i=0; i<size; i++)
			cout<<fp[i]<<'\n';
		//cout<<'\n';
    	return size;
	}
};

ALSAExternalPluginTest aEPlugin;
extern "C" SND_PCM_PLUGIN_DEFINE_FUNC(ALSAExternalPluginTest){
	std::cout<<__func__<<std::endl;
    aEPlugin.parseConfig(name, conf, stream, mode);

	int ret=aEPlugin.create(name, root, stream, mode);
	if (ret<0)
		return ret;

	aEPlugin.specifyHWParams();

	*pcmp=aEPlugin.getPCM();
	std::cout<<__func__<<" returning "<<std::endl;
    return 0;
}

SND_PCM_PLUGIN_SYMBOL(ALSAExternalPluginTest);
