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
#ifndef HARDWARE_H
#define HARDWARE_H

#include <ALSA/ALSA.H>

namespace ALSA {

	/** Hardware class for the ALSA lib, representing one sound card.
	*/
	class Hardware : public PCM {
		snd_pcm_hw_params_t *hParams; ///< PCM params
	protected:

		/** Set the hardware params
		\return >= 0 on success
		*/
		int setHWParams() {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			return snd_pcm_hw_params(getPCM(), hParams);
		}

	public:
		/** Set the hardware params
		\return >= 0 on success
		*/
		int getHWParams() {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			return snd_pcm_hw_params_current(getPCM(), hParams);
		}

		/** Fill params with a full configuration space for a PCM.
		Note you have to call setAccess, setFormat, setChannels, setSampleRate if you call this.
		\return >= 0 on success
		*/
		int fillParams() {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			int ret=snd_pcm_hw_params_any(getPCM(), hParams);
			if (ret<0)
				ALSADebug().evaluateError(ret);
			return ret;
		}

		/** Reset the params with a full configuration space for a PCM.
		Note you have to call setAccess, setFormat, setChannels, setSampleRate if you call this.
		\return >= 0 on success
		*/
		int resetParams() {
			return fillParams();
		}

		Hardware() {
			hParams=NULL;
			snd_pcm_hw_params_malloc(&hParams);
			if (!hParams)
				assert("Couldn't malloc HW params");
		}

		virtual ~Hardware() {
			if (hParams)
				snd_pcm_hw_params_free(hParams);
			hParams=NULL;
		}

		/** Copy the input hw_params to the class
		\param hParamsIn the hw_params to copy
		*/
		void copyFrom(snd_pcm_hw_params_t *hParamsIn){
			snd_pcm_hw_params_copy(hParams, hParamsIn);
		}

		/** Alter the sample rate resample.
		\param state True to resample and fals to not resample.
		\return >= 0 on success
		*/
		int rateResample(unsigned int state) {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			return snd_pcm_hw_params_set_rate_resample(getPCM(), hParams, state);
		}

		/** Set the type of channel layout in the buffer
		\param access is the type of channel layout, one of
		SND_PCM_ACCESS_MMAP_INTERLEAVED  mmap access with simple interleaved channels
		SND_PCM_ACCESS_MMAP_NONINTERLEAVED  mmap access with simple non interleaved channels
		SND_PCM_ACCESS_MMAP_COMPLEX mmap access with complex placement
		SND_PCM_ACCESS_RW_INTERLEAVED  snd_pcm_readi/snd_pcm_writei access
		SND_PCM_ACCESS_RW_NONINTERLEAVED snd_pcm_readn/snd_pcm_writen access
		\return >= 0 on success
		*/
		int setAccess(snd_pcm_access_t access) {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			int ret=snd_pcm_hw_params_set_access(getPCM(), hParams, access);
			if (ret<0)
				return ALSADebug().evaluateError(ret);
			return ret;
		}

		/** Get the access mode for the hardware
		\return <0 on error or the access mode
		*/
		int getAccess(void){
			snd_pcm_access_t access;
			int ret=snd_pcm_hw_params_get_access(hParams, &access);
			if (ret>=0)
				ret=access;
			return ret;
		}

		/** Set the sample type
		\param format is the sample type
		\return >= 0 on success
		SND_PCM_FORMAT_S8 Signed 8 bit
		SND_PCM_FORMAT_U8 Unsigned 8 bit
		SND_PCM_FORMAT_S16_LE Signed 16 bit Little Endian
		SND_PCM_FORMAT_S16_BE Signed 16 bit Big Endian
		SND_PCM_FORMAT_U16_LE Unsigned 16 bit Little Endian
		SND_PCM_FORMAT_U16_BE Unsigned 16 bit Big Endian
		SND_PCM_FORMAT_S24_LE Signed 24 bit Little Endian using low three bytes in 32-bit word
		SND_PCM_FORMAT_S24_BE Signed 24 bit Big Endian using low three bytes in 32-bit word
		SND_PCM_FORMAT_U24_LE Unsigned 24 bit Little Endian using low three bytes in 32-bit word
		SND_PCM_FORMAT_U24_BE Unsigned 24 bit Big Endian using low three bytes in 32-bit word
		SND_PCM_FORMAT_S32_LE Signed 32 bit Little Endian
		SND_PCM_FORMAT_S32_BE Signed 32 bit Big Endian
		SND_PCM_FORMAT_U32_LE Unsigned 32 bit Little Endian
		SND_PCM_FORMAT_U32_BE Unsigned 32 bit Big Endian
		SND_PCM_FORMAT_FLOAT_LE Float 32 bit Little Endian, Range -1.0 to 1.0
		SND_PCM_FORMAT_FLOAT_BE Float 32 bit Big Endian, Range -1.0 to 1.0
		SND_PCM_FORMAT_FLOAT64_LE Float 64 bit Little Endian, Range -1.0 to 1.0
		SND_PCM_FORMAT_FLOAT64_BE Float 64 bit Big Endian, Range -1.0 to 1.0
		SND_PCM_FORMAT_IEC958_SUBFRAME_LE IEC-958 Little Endian
		SND_PCM_FORMAT_IEC958_SUBFRAME_BE IEC-958 Big Endian
		SND_PCM_FORMAT_MU_LAW Mu-Law
		SND_PCM_FORMAT_A_LAW A-Law
		SND_PCM_FORMAT_IMA_ADPCM Ima-ADPCM
		SND_PCM_FORMAT_MPEG MPEG
		SND_PCM_FORMAT_GSM GSM
		SND_PCM_FORMAT_SPECIAL Special
		SND_PCM_FORMAT_S24_3LE Signed 24bit Little Endian in 3bytes format
		SND_PCM_FORMAT_S24_3BE Signed 24bit Big Endian in 3bytes format
		SND_PCM_FORMAT_U24_3LE Unsigned 24bit Little Endian in 3bytes format
		SND_PCM_FORMAT_U24_3BE Unsigned 24bit Big Endian in 3bytes format
		SND_PCM_FORMAT_S20_3LE Signed 20bit Little Endian in 3bytes format
		SND_PCM_FORMAT_S20_3BE Signed 20bit Big Endian in 3bytes format
		SND_PCM_FORMAT_U20_3LE Unsigned 20bit Little Endian in 3bytes format
		SND_PCM_FORMAT_U20_3BE Unsigned 20bit Big Endian in 3bytes format
		SND_PCM_FORMAT_S18_3LE Signed 18bit Little Endian in 3bytes format
		SND_PCM_FORMAT_S18_3BE Signed 18bit Big Endian in 3bytes format
		SND_PCM_FORMAT_U18_3LE Unsigned 18bit Little Endian in 3bytes format
		SND_PCM_FORMAT_U18_3BE Unsigned 18bit Big Endian in 3bytes format
		SND_PCM_FORMAT_G723_24 G.723 (ADPCM) 24 kbit/s, 8 samples in 3 bytes
		SND_PCM_FORMAT_G723_24_1B G.723 (ADPCM) 24 kbit/s, 1 sample in 1 byte
		SND_PCM_FORMAT_G723_40 G.723 (ADPCM) 40 kbit/s, 8 samples in 3 bytes
		SND_PCM_FORMAT_G723_40_1B G.723 (ADPCM) 40 kbit/s, 1 sample in 1 byte
		SND_PCM_FORMAT_DSD_U8 Direct Stream Digital (DSD) in 1-byte samples (x8)
		SND_PCM_FORMAT_DSD_U16_LE Direct Stream Digital (DSD) in 2-byte samples (x16)

		SND_PCM_FORMAT_S16 Signed 16 bit CPU endian
		SND_PCM_FORMAT_U16 Unsigned 16 bit CPU endian
		SND_PCM_FORMAT_S24 Signed 24 bit CPU endian
		SND_PCM_FORMAT_U24 Unsigned 24 bit CPU endian
		SND_PCM_FORMAT_S32 Signed 32 bit CPU endian
		SND_PCM_FORMAT_U32 Unsigned 32 bit CPU endian
		SND_PCM_FORMAT_FLOAT Float 32 bit CPU endian
		SND_PCM_FORMAT_FLOAT64 Float 64 bit CPU endian
		SND_PCM_FORMAT_IEC958_SUBFRAME IEC-958 CPU Endian
		*/
		int setFormat(snd_pcm_format_t format) {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			int ret=snd_pcm_hw_params_set_format(getPCM(), hParams, format);
			if (ret<0)
				return ALSADebug().evaluateError(ret);
			return ret;
		}

		void dumpState(){
			snd_output_t *output = NULL;
			int err = snd_output_stdio_attach(&output, stdout, 0);
	    if (err < 0)
				ALSADebug().evaluateError(err);
			else
				snd_pcm_hw_params_dump(hParams, output);
		}

		/** Print the available formats
		*/
		void printAvailableFormats()
		{
			fprintf(stdout, "Available formats:\n");
			for (snd_pcm_format_t format = SND_PCM_FORMAT_S8; format <= SND_PCM_FORMAT_LAST; format=(snd_pcm_format_t)((int)format+1)) {
				if (snd_pcm_hw_params_test_format(getPCM(), hParams, (snd_pcm_format_t)format) == 0)
					fprintf(stdout, "- %s\n", snd_pcm_format_name(format));
			}
		}

		/** Set the sample type
		\param format is the sample type string(PCM sample format name (case insensitive))
		\return >= 0 on success
		*/
		int setFormat(std::string format) {
			return setFormat(snd_pcm_format_value(format.c_str()));
		}

		/** Get the format from a configuration space
		\param params Configuration space
		\param format returned format
		\return format otherwise a negative error code if the configuration space does not contain a single value
		*/
		int getFormat(snd_pcm_format_t &format){
			return snd_pcm_hw_params_get_format(hParams, &format);
		}

		/** Get the bits per sample.
		\return <0 on error or the bits per sample.
		*/
		int getFormatPhysicalWidth(){
			int ret=0;
			snd_pcm_format_t format;
			if ((ret=getFormat(format))<0)
				return ret;
			return snd_pcm_format_physical_width(format);
		}

		/** Set the number of channels
		\param cnt The number of channels.
		\return >= 0 on success
		*/
		int setChannels(unsigned int cnt) {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			return snd_pcm_hw_params_set_channels(getPCM(), hParams, cnt);
		}

		/** Get the channel count
		\return >= 0 on success returns the channel count
		*/
		int getChannels() {
			unsigned int cnt;
			int err=snd_pcm_hw_params_get_channels(hParams, &cnt);
			if (err<0)
				return err;
			else
				return cnt;
		}

		/** Get the maximum number of channels for this device.
		\return <0 on error, the maximum number of channels this device can drive.
		*/
		int getMaxChannels(){
			PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), int) // check pcm is open
			unsigned int mCh;
			int ret=snd_pcm_hw_params_get_channels_max(hParams, &mCh);
			if (ret<0)
				return ALSADebug().evaluateError(ret);
			return mCh;
		}

		/** Get the minimum number of channels for this device.
		\return <0 on error, the maximum number of channels this device can drive.
		*/
		int getMinChannels(){
			PCM_NOT_OPEN_CHECK_NO_PRINT(getPCM(), int) // check pcm is open
			unsigned int mCh;
			int ret=snd_pcm_hw_params_get_channels_min(hParams, &mCh);
			if (ret<0)
				return ALSADebug().evaluateError(ret);
			return mCh;
		}

		/** Set the sample rate closest to the desired rate.
		\param rrate The desired sample rate.
		\param dir The direction to search in (-1,0,1)
		\return >= 0 on success
		*/
		int setSampleRate(unsigned int rrate, int dir=0) {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			int ret=snd_pcm_hw_params_set_rate_near(getPCM(), hParams, &rrate, &dir);
			if (ret<0)
				return ALSADebug().evaluateError(ret);
			return ret;
		}

		/** Get the sample rate
		\param dir The direction to search in (-1,0,1)
		\return >= 0 on success
		*/
		int getSampleRate(int dir=0) {
			unsigned int rate;
			int err=snd_pcm_hw_params_get_rate(hParams, &rate, &dir);
			if (err<0)
				return err;
			else
				return rate;
		}

		/** Get the maximum sample rate
		\return >= 0 on success
		*/
		int getMaxSampleRate() {
			unsigned int rate;
			int err=snd_pcm_hw_params_get_rate_max(hParams, &rate, 0);
			if (err<0)
				return ALSADebug().evaluateError(err);
			else
				return rate;
		}

		/** Get the minimum sample rate
		\return >= 0 on success
		*/
		int getMinSampleRate() {
			unsigned int rate;
			int err=snd_pcm_hw_params_get_rate_min(hParams, &rate, 0);
			if (err<0)
				return ALSADebug().evaluateError(err);
			else
				return rate;
		}

		/** Get the period size
		\param p Returned approximate period size in frames
		\param dir The direction to search in (-1,0,1)
		\return >= 0 on success
		*/
		int getPeriodSize(snd_pcm_uframes_t *p, int *dir=NULL) {
			int err=snd_pcm_hw_params_get_period_size(hParams, p, dir);
			if (err<0)
				return err;
			else
				return *p;
		}

		/** Get the number of periods
		\return >= 0 on success
		*/
		int getPeriods() {
			unsigned int m;
			int err=snd_pcm_hw_params_get_periods(hParams, &m, 0);
			if (err<0)
				return err;
			else
				return m;
		}

		/** Get the maximum period size
		\return >= 0 on success
		*/
		snd_pcm_uframes_t getMaxPeriodSize() {
			snd_pcm_uframes_t s;
			int err=snd_pcm_hw_params_get_period_size_max(hParams, &s, 0);
			if (err<0)
				return ALSADebug().evaluateError(err);
			else
				return s;
		}

		/** Get the minimum period size
		\return >= 0 on success
		*/
		snd_pcm_uframes_t getMinPeriodSize() {
			snd_pcm_uframes_t s;
			int err=snd_pcm_hw_params_get_period_size_min(hParams, &s, 0);
			if (err<0)
				return ALSADebug().evaluateError(err);
			else
				return s;
		}

		/** Get the period size
		\param p Returned approximate period size in frames
		\param dir The direction to search in (-1,0,1)
		\return >= 0 on success
		*/
		int getPeriodSize(int *dir=NULL) {
			snd_pcm_uframes_t p;
			return getPeriodSize(&p, dir);
		}

		/** Set the period size
		\param p approximate period size in frames
		\param dir The direction to search in (-1,0,1)
		\return >= 0 on success
		*/
		int setPeriodSize(snd_pcm_uframes_t *p, int *dir=0) {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			return snd_pcm_hw_params_set_period_size_near(getPCM(), hParams, p, dir);
		}

		/** Set the buffer size
		\param the desired buffer size
		\return <0 on error.
		*/
		int setBufSize(snd_pcm_uframes_t bufSize, int bufCnt=2) {
			PCM_NOT_OPEN_CHECK(getPCM()) // check pcm is open
			snd_pcm_uframes_t periodSize = bufSize * bufCnt;

			int err = snd_pcm_hw_params_set_buffer_size_near(getPCM(), hParams, &periodSize);
			if (err < 0) {
				printf("Unable to set buffer size %d %s \n",(int)(bufSize*2), snd_strerror(err));
				return err;
			}
			periodSize /= bufCnt;
			if ((err = setPeriodSize(&periodSize)) < 0)
				printf("Unable to set period size %d %s \n",(int)periodSize, snd_strerror(err));
			return err;
		}

		/** Get the minimum buffer size
		\return >= 0 on success
		*/
		snd_pcm_uframes_t getMaxBufSize() {
			snd_pcm_uframes_t s;
			int err=snd_pcm_hw_params_get_buffer_size_max(hParams, &s);
			if (err<0)
				return ALSADebug().evaluateError(err);
			else
				return s;
		}

		/** Get the minimum buffer size
		\return >= 0 on success
		*/
		snd_pcm_uframes_t getMinBufSize() {
			snd_pcm_uframes_t s;
			int err=snd_pcm_hw_params_get_buffer_size_min(hParams, &s);
			if (err<0)
				return ALSADebug().evaluateError(err);
			else
				return s;
		}

		const char *formatDescription(const snd_pcm_format_t format) const {
			return snd_pcm_format_description(format);
		}

		/** Get the name of the pcm device
		@return The device name
		*/
		const char *getDeviceName(){
			PCM_NOT_OPEN_CHECK_STRING(getPCM()) // check pcm is open
			return snd_pcm_name(getPCM());
		}

		int dumpHWParams(){
			int ret=0;
			if (!logEnabled())
				return ret;
			return snd_pcm_hw_params_dump(hParams, log);
		}

		/** Reset the params, then set the format, access, sample rate and channel count.
		You may still wish to set other parameters after this (\see setBufSize for example).
		Following the execution of setFARC, call setParams \see setParams to fill the params to the device before running the device.
		\param format ALSA PCM sample format, \see setFormat
		\param access ALSA PCM stream sample layout, \see setAccess
		\param fs The sample rate in Hz
		\param cnt The number of channels
		\retrn 0 on success
		*/
		int setFARC(snd_pcm_format_t format, snd_pcm_access_t access, unsigned int fs, unsigned int cnt){
			int res=resetParams();
			if (res<0) // we don't want defaults so reset and refil the params
				return ALSADebug().evaluateError(res);
			if ((res=setFormat(format))<0)
	    	return ALSADebug().evaluateError(res);
			if ((res=setAccess(access))<0)
		    return ALSADebug().evaluateError(res);
			if ((res=setSampleRate(fs))<0)
		    return ALSADebug().evaluateError(res);
			if ((res=setChannels(cnt))<0)
		    return ALSADebug().evaluateError(res);
			return 0;
		}
	};
}
#endif //HARDWARE_H
