#ifndef ALSAPLUGIN_H
#define ALSAPLUGIN_H

#include "ALSA.H"
#include <alsa/pcm_external.h>
#include <alsa/pcm_plugin.h>

#include <typeinfo>

namespace ALSA {

#define STATICFNNAME(name) name##_static
#define STATICFNDEF(retType, name) static retType STATICFNNAME(name) (snd_pcm_ioplug_t *io)
#define STATICFNBODY(name) {std::cout<<__func__<<std::endl; return static_cast<ALSAPlugin*>(io->private_data)->name();}
#define STATICFN(retType, name) STATICFNDEF(retType, name) STATICFNBODY(name)

class ALSAPlugin : public Hardware {
    snd_pcm_ioplug_t io; /// The ALSA plugin
	snd_pcm_ioplug_callback_t ioplugCallback; /// The callback functions

    // Define the various snd_pcm_ioplug_callback_t static functions
	STATICFN(int, start) ///< start the PCM; required
	STATICFN(int, stop) ///< stop the PCM; required
	STATICFN(snd_pcm_sframes_t, pointer) ///< get the current DMA position; required

	static snd_pcm_sframes_t transfer_static(snd_pcm_ioplug_t *io, const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset, snd_pcm_uframes_t size){
    	std::cout<<__func__<<std::endl;
		return static_cast<ALSAPlugin*>(io->private_data)->transfer(areas, offset, size);
	}

	static int HWParams_static(snd_pcm_ioplug_t *io, snd_pcm_hw_params_t *params){
    	std::cout<<__func__<<std::endl;
		return static_cast<ALSAPlugin*>(io->private_data)->HWParams(params);
    }

	// virtual int setupHWParams()=0;

	virtual int HWParams(snd_pcm_hw_params_t *params){
    	std::cout<<__func__<<std::endl;
		return 0;		
	}

protected:
    snd_pcm_t *slave; /// The slave

	void setName(const char *name){
		io.name=name;
	}
public:

	ALSAPlugin(){
		assert("not debugged");
    	std::cout<<__func__<<std::endl;
		io.version = SND_PCM_IOPLUG_VERSION;
		setName("ALSAPlugin default name");
		io.mmap_rw = 0;
		io.callback = &ioplugCallback;
		io.private_data=this;

		// setup the static callbacks
		ioplugCallback.start=STATICFNNAME(start);
		ioplugCallback.stop=STATICFNNAME(stop);
		ioplugCallback.pointer=STATICFNNAME(pointer);
		ioplugCallback.transfer=transfer_static;
		ioplugCallback.hw_params=HWParams_static;
	}

	virtual ~ALSAPlugin(){
    	std::cout<<__func__<<std::endl;
	}

    /** Parse the snd_config_t passed to the plugin
    \return 0 on success, < 0 on failure
    */
    virtual int parseConfig(const char *name, snd_config_t *conf, snd_pcm_stream_t stream, int mode){
    	std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
        return 0;
    }

    /// start method
    virtual int start(){
    	std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
		/* When trying to start a PCM that's already running, the result is
		   EBADFD. We might have implicitly started the buffer by filling it
		   up, so just ignore this request if we're already running. */
		if (snd_pcm_state(slave) == SND_PCM_STATE_RUNNING)
			return 0;

		return snd_pcm_start(slave);
    }

    /// stop method
    virtual int stop(){
    	std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
		return snd_pcm_drop(slave);
    }

    /// virtual pointer method
    virtual snd_pcm_sframes_t pointer()=0;

    /** This method creates the ioplug
    */
    int create(const char *name, snd_pcm_stream_t stream, int mode){
		return snd_pcm_ioplug_create(&io, name, stream, mode);
	}

	virtual snd_pcm_sframes_t transfer(const snd_pcm_channel_area_t *areas, snd_pcm_uframes_t offset, snd_pcm_uframes_t size){
    	std::cout<<typeid(this).name()<<'\t'<<__func__<<std::endl;
    	return size;
	}

	/** This method returns the pcm struct
	*/
	virtual snd_pcm_t *getPCM(){
    	std::cout<<'\t'<<__func__<<'\t'<<__LINE__<<std::endl;
			return io.pcm;
	}

	/** This method returns a pointer to the pcm struct
	*/
	snd_pcm_t **getPCMP(){
		return &(io.pcm);
	}

};
};
#endif // ALSAPLUGIN_H
