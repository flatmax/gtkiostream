#ifndef ALSAEXTERNALPLUGIN_H
#define ALSAEXTERNALPLUGIN_H

#include "ALSA.H"
#include <alsa/pcm_external.h>
#include <alsa/pcm_plugin.h>

#include <typeinfo>
#include <sstream>

namespace ALSA {

class ALSAExternalPlugin : public Software {
	#define STATICFNNAME(name) name##_static
	#define STATICFNDEF(retType, name) static retType STATICFNNAME(name) (snd_pcm_extplug_t *extplug)
	#define STATICFNDEF2(retType, name, arg1_type, arg1) static retType STATICFNNAME(name) (snd_pcm_extplug_t *extplug, arg1_type arg1)
	#define STATICFNBODY(name) {std::cout<<__func__<<std::endl; return static_cast<ALSAExternalPlugin*>(extplug->private_data)->name();}
	#define STATICFNBODY2(name, arg1) {std::cout<<__func__<<std::endl; return static_cast<ALSAExternalPlugin*>(extplug->private_data)->name(arg1);}
	#define STATICFN(retType, name) STATICFNDEF(retType, name) STATICFNBODY(name)
	#define STATICFN2(retType, name, arg1_type, arg1) STATICFNDEF2(retType, name, arg1_type, arg1) STATICFNBODY2(name, arg1)

	snd_pcm_extplug_callback_t callbacks; /// The callback functions

    // Define the varextplugus snd_pcm_extplug_callback_t static functextplugns
	STATICFN(int, close)
	STATICFN2(int, hwParams, snd_pcm_hw_params_t *, params)
	STATICFN(int, hwFree)
	STATICFN2(void, dump, snd_output_t *, out)
	STATICFN(int, init)
//	STATICFN(snd_pcm_chmap_query_t **, queryChmaps)
//	STATICFN(snd_pcm_chmap_t *, getChmap)
//	STATICFN2(int, setChmap, const snd_pcm_chmap_t *, map)

	static snd_pcm_sframes_t transfer_static(snd_pcm_extplug_t *extplug, const snd_pcm_channel_area_t *dst_areas, snd_pcm_uframes_t dst_offset, const snd_pcm_channel_area_t *src_areas, snd_pcm_uframes_t src_offset, snd_pcm_uframes_t size){
//    	std::cout<<__func__<<std::endl;
		return static_cast<ALSAExternalPlugin*>(extplug->private_data)->doTransfer(dst_areas, dst_offset, src_areas, src_offset, size);
	}

protected:
  snd_pcm_extplug_t extplug; /// The ALSA plugin
	snd_config_t * slaveConf; /// The slave to use

	/** Set the name of the plugin
	\param name The name to set
	*/
	void setName(const char *name){
		extplug.name=name;
	}
public:

	ALSAExternalPlugin(){
    	std::cout<<__func__<<std::endl;
    	slaveConf=NULL;
		extplug.version = SND_PCM_EXTPLUG_VERSION;
		setName("ALSAExternalPlugin default name");
		extplug.callback = &callbacks;
		extplug.private_data=this;

		// setup the static callbacks
		callbacks.close=STATICFNNAME(close);
		callbacks.hw_params=STATICFNNAME(hwParams);
		callbacks.hw_free=STATICFNNAME(hwFree);
		callbacks.dump=STATICFNNAME(dump);
		callbacks.init=STATICFNNAME(init);
//		callbacks.query_chmaps=STATICFNNAME(queryChmaps);
//		callbacks.get_chmap=STATICFNNAME(getChmap);
//		callbacks.set_chmap=STATICFNNAME(setChmap)

		callbacks.transfer=transfer_static;
	}

	virtual ~ALSAExternalPlugin(){
    	std::cout<<__func__<<std::endl;
	}

    /** Close
    \return 0 on success, < 0 on failure
    */
    virtual int close(){
    	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
        return 0;
    }

    /** Set various hw params
    \params The params of the system
    \return 0 on success, < 0 on failure
    */
    virtual int hwParams(snd_pcm_hw_params_t *params){
    	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
		copyFrom(params);
		std::cout<<"channels : "<<getChannels()<<std::endl;
		std::cout<<"fs       : "<<getSampleRate(0)<<" Hz"<<std::endl;
        return 0;
    }

    /** We have to specify hwparams
    \return 0 on success, < 0 on failure
    */
    virtual int specifyHWParams()=0;

    /** free
    \return 0 on success, < 0 on failure
    */
    virtual int hwFree(){
    	////std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
        return 0;
    }

    /** dump
    \return 0 on success, < 0 on failure
    */
    virtual void dump(snd_output_t *out){
    	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
    }

    /** init
    \return 0 on success, < 0 on failure
    */
    virtual int init(){
    	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
        return 0;
    }

    snd_pcm_sframes_t doTransfer(const snd_pcm_channel_area_t *dst_areas, snd_pcm_uframes_t dst_offset, const snd_pcm_channel_area_t *src_areas, snd_pcm_uframes_t src_offset, snd_pcm_uframes_t size){
    	return transfer(dst_areas, dst_offset, src_areas, src_offset, size);
    }

	/** The transfer method
	*/
	virtual snd_pcm_sframes_t transfer(const snd_pcm_channel_area_t *dst_areas, snd_pcm_uframes_t dst_offset, const snd_pcm_channel_area_t *src_areas, snd_pcm_uframes_t src_offset, snd_pcm_uframes_t size){
    	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
    	return size;
	}

    /** This method creates the external plugin
    */
    int create(const char *name, snd_config_t * root, snd_pcm_stream_t stream, int mode){
		if (!slaveConf) {
			std::ostringstream oss;
			oss<<"No slave configuration for "<<name<<" : "<<extplug.name<<" external plugin";
			SNDERR(oss.str().c_str());
			return -EINVAL;
		}
		return snd_pcm_extplug_create(&extplug, name, root, slaveConf, stream, mode);
	}

    /** Parse the snd_config_t passed to the plugin
    \return 0 on success, < 0 on failure
    */
    virtual int parseConfig(const char *name, snd_config_t *conf, snd_pcm_stream_t stream, int mode){
    	//std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
		snd_config_iterator_t i, next;
		snd_config_for_each(i, next, conf) {
			snd_config_t *n = snd_config_iterator_entry(i);
			const char *id;
			if (snd_config_get_id(n, &id) < 0)
				continue;
			if (strcmp(id, "comment") == 0 || strcmp(id, "type") == 0 || strcmp(id, "hint") == 0)
				continue;
			if (strcmp(id, "slave") == 0) {
				slaveConf = n;
				continue;
			}
			SNDERR("Unknown field %s", id);
			return -EINVAL;
		}

		if (!slaveConf) {
			std::ostringstream oss;
			oss<<"No slave configuration for "<<name<<" : "<<extplug.name<<" external plugin";
			SNDERR(oss.str().c_str());
			return -EINVAL;
		}
        return 0;
    }

	/** This method returns the pcm struct
	\return The pcm struct
	*/
	virtual snd_pcm_t *getPCM(){
    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
			return extplug.pcm;
	}

};
};
#endif // ALSAEXTERNALPLUGIN_H
