//=====================================================================//
/*!	@file
	@brief	OpenAL オーディオ入出力
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <unistd.h>
#include <ctime>
#include "audio_io.hpp"
#include "pcm.hpp"
#ifdef WIN32
#include <windows.h>
#endif

namespace al {

	using namespace utils;

	static const int indentation = 4;
	static const int maxmimumWidth = 79;

	static void printChar(int c, int& width)
	{
		putchar(c);
		width = ((c == '\n') ? 0 : ((width) + 1));
	}

	static void indent(int& width)
	{
		for(int i = 0; i < indentation; i++) {
			printChar(' ', width);
		}
	}

	static void checkForErrors()
	{
		{
			ALCdevice* device = alcGetContextsDevice(alcGetCurrentContext());
			ALCenum error = alcGetError(device);
			if(error != ALC_NO_ERROR) {
				printf("ALC error: '%s'\n", (const char*)alcGetString(device, error));
			}
		}
		{
			ALenum error = alGetError();
			if(error != AL_NO_ERROR) {
				printf("AL error: '%s'\n", (const char*)alGetString(error));
			}
		}
	}


	static void printDevices(ALCenum which, const char* kind)
	{
		const char *s = alcGetString(NULL, which);
		checkForErrors();

		printf("Available %sdevices:\n", kind);
		while(*s != '\0') {
			printf("    %s\n", s);
			while(*s++ != '\0') ;
		}
	}


	static void printExtensions(const char* header, char separator, const char* extensions)
	{
		printf("%s:\n", header);
		if(extensions == NULL || extensions[0] == '\0') return;

		int width = 0, start = 0, end = 0;
		indent(width);
		while(1) {
			if(extensions[end] == separator || extensions[end] == '\0') {
				if(width + end - start + 2 > maxmimumWidth) {
					printChar('\n', width);
					indent(width);
				}
				while(start < end) {
					printChar(extensions[start], width);
					start++;
				}
				if(extensions[end] == '\0') break;
				start++;
				end++;
				if(extensions[end] == '\0') break;
				printChar(',', width);
				printChar(' ', width);
			}
			end++;
		}
		printChar('\n', width);
	}


	bool audio_io::set_buffer(ALuint bh, const i_audio* aif)
	{
		ALsizei num = aif->get_samples();
		ALenum format;
		switch(aif->get_type()) {
		case audio_format::PCM16_MONO:
			format = AL_FORMAT_MONO16;
			num *= 2;
			break;
		case audio_format::PCM16_STEREO:
			format = AL_FORMAT_STEREO16;
			num *= 4;
			break;
		case audio_format::PCM8_MONO:
			format = AL_FORMAT_MONO8;
			num *= 1;
			break;
		case audio_format::PCM8_STEREO:
			format = AL_FORMAT_STEREO8;
			num *= 2;
			break;
		default:
///			utils::string_printf(error_message_, "Can't decode file format.\n");
			return false;
			break;
		}
		alBufferData(bh, format, aif->get_wave(), num, static_cast<ALsizei>(aif->get_rate()));
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void audio_io::initialize()
	{
		if(init_) return;

		ALCdevice* device_ = alcOpenDevice(NULL);
		if(device_ == 0) {
			ALenum error = alGetError();
			if(error != AL_NO_ERROR) {
			// die("AL", (const char*)alGetString(error));
			}
			return;
		}

		ALCcontext* context_ = alcCreateContext(device_, NULL);
		if(context_ == 0) {
			ALCenum error = alcGetError(device_);
			if(error != ALC_NO_ERROR) {
			// die("ALC", (const char*)alcGetString(device, error));
			}
			return;
		}
		alcMakeContextCurrent(context_);
		init_ = true;
		destroy_ = false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	コンテキストの情報を表示（OpenAL）
	*/
	//-----------------------------------------------------------------//
	void audio_io::context_info() const
	{
		printf("OpenAL vendor string: %s\n", alGetString(AL_VENDOR));
		printf("OpenAL renderer string: %s\n", alGetString(AL_RENDERER));
		printf("OpenAL version string: %s\n", alGetString(AL_VERSION));
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ALC の情報を表示
	*/
	//-----------------------------------------------------------------//
	void audio_io::alc_info() const
	{
		if(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE) {
			if(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATE_ALL_EXT") == AL_TRUE) {
				printDevices(ALC_ALL_DEVICES_SPECIFIER, "playback ");
			} else {
				printDevices(ALC_DEVICE_SPECIFIER, "playback ");
				printDevices(ALC_CAPTURE_DEVICE_SPECIFIER, "capture ");
			}
		} else {
			printf("No device enumeration available\n");
		}

		ALCdevice* device = alcGetContextsDevice(alcGetCurrentContext());
		checkForErrors();

		printf("Default device: %s\n",
			alcGetString(device, ALC_DEFAULT_DEVICE_SPECIFIER));

		printf("Default capture device: %s\n",
			alcGetString(device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER));

		ALCint major, minor;
		alcGetIntegerv(device, ALC_MAJOR_VERSION, 1, &major);
		alcGetIntegerv(device, ALC_MINOR_VERSION, 1, &minor);
		checkForErrors();
		printf("ALC version: %d.%d\n", (int)major, (int)minor);

		printExtensions("ALC extensions", ' ',
			alcGetString(device, ALC_EXTENSIONS));
		checkForErrors();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	オーディオ・インターフェースから波形（buffer）を作成する
		@param[in]	aif	オーディオ・インターフェース・クラス
		@return	波形・ハンドルを返す
	*/
	//-----------------------------------------------------------------//
	audio_io::wave_handle audio_io::create_wave(const i_audio* aif)
	{
		wave_handle wh;

		if(aif == 0) return 0;

		alGenBuffers(1, &wh);
		set_buffer(wh, aif);

		if(wh == AL_NONE) {
			ALenum alerror = alGetError();
///			utils::string_printf(m_error_message, "Load file error: '%s'\n", alGetString(alerror));
			return 0;
		}

		return wh;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	波形を廃棄する。
		@param[in]	wh	波形ハンドル
	*/
	//-----------------------------------------------------------------//
	void audio_io::destroy_wave(wave_handle wh)
	{
		alDeleteBuffers(1, &wh);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	スロット（ソース）を作成する。
		@param[in]	wh	波形・ハンドル
		@return スロット・ハンドルを返す。
	*/
	//-----------------------------------------------------------------//
	audio_io::slot_handle audio_io::create_slot(wave_handle wh)
	{
		slot_handle sh;

		alGenSources(1, &sh);
		if(wh) {
			alSourcei(sh, AL_BUFFER, wh);
		}
		return sh;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	スロットの演奏状態を得る
		@param[in]	sh	スロット・ハンドル
		@return	「true」なら演奏中
	*/
	//-----------------------------------------------------------------//
	bool audio_io::get_slot_status(slot_handle sh) const
	{
		if(sh == 0) {
			return false;
		}
		ALint status;
		alGetSourcei(sh, AL_SOURCE_STATE, &status);
		if(status == AL_PLAYING) {
			return true;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	スロットを廃棄する。
		@param[in]	h	スロット・ハンドル
	*/
	//-----------------------------------------------------------------//
	void audio_io::destroy_slot(slot_handle h)
	{
		alDeleteSources(1, &h);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	スロットに、波形をセットする
		@param[in]	sh	再生するスロット・ハンドル
		@param[in]	wh	波形ハンドル
		@return	成功したら「true」
	*/
	//-----------------------------------------------------------------//
	bool audio_io::set_wave(slot_handle sh, wave_handle wh)
	{
		if(sh != 0 && wh != 0) {
			alSourcei(sh, AL_BUFFER, wh);
//			printf("set_wave: slot: %d, wave: %d\n", sh, wh);
			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリームを浄化する
		@param[in]	ssh	ストリーム・スロット・ハンドル
	*/
	//-----------------------------------------------------------------//
	void audio_io::purge_stream(slot_handle ssh)
	{
		if(ssh) {
			alSourceStop(ssh);

			// キューイングされたバッファを取り除く～
			ALint n;
			do {
				ALuint bh;
				alSourceUnqueueBuffers(ssh, 1, &bh);
				alDeleteBuffers(1, &bh);
				alGetSourcei(ssh, AL_BUFFERS_QUEUED, &n);
			} while(n != 0) ;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生の空きバッファを返す。
		@param[in]	ssh	ストリーム・スロット・ハンドル
		@return 有効な、バッファがあれば、そのハンドルを返す。（「０」なら無効）
	*/
	//-----------------------------------------------------------------//
	audio_io::wave_handle audio_io::status_stream(slot_handle ssh)
	{
		ALint state;
		alGetSourcei(ssh, AL_SOURCE_STATE, &state);
		if(state == AL_PAUSED) {
			return 0;
		}
		ALint n;
		wave_handle bh;
		alGetSourcei(ssh, AL_BUFFERS_QUEUED, &n);	// キューバッファを最大数まで作成
		if(n < queue_max_) {
			alGenBuffers(1, &bh);
		} else {
			alGetSourcei(ssh, AL_BUFFERS_PROCESSED, &n);	// キューバッファ利用完了数を取得
			if(n == 0) {
				return 0;	// キューバッファが空いていない場合。
			}
			alSourceUnqueueBuffers(ssh, 1, &bh);	// キューバッファ再利用のハンドルを取得
		}
		return bh;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生キューにバッファをセットする。
		@param[in]	ssh	ストリーム・スロット・ハンドル
		@param[in]	bh	有効なバッファ・ハンドル
		@param[in]	aif	バッファにセットするオーディオ・インターフェース@n
						※常に、同じ構成を与える必要がある。
	*/
	//-----------------------------------------------------------------//
	void audio_io::queue_stream(slot_handle ssh, wave_handle bh, const i_audio* aif)
	{
		if(ssh == 0 || bh == 0 || aif == 0) return;

		set_buffer(bh, aif);
		alSourceQueueBuffers(ssh, 1, &bh);

		// ストリーム・ソースを「PLAY」
		ALint state;
		alGetSourcei(ssh, AL_SOURCE_STATE, &state);
		if(state == AL_PAUSED) ;
		else if(state != AL_PLAYING) {
			alSourcePlay(ssh);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリームをポーズする。
		@param[in]	ssh	ストリーム・スロット・ハンドル
		@param[in]	ena	「false」ならポーズ解除
	*/
	//-----------------------------------------------------------------//
	void audio_io::pause_stream(slot_handle ssh, bool ena)
	{
		if(ssh == 0) return;

		if(ena) {
//			printf("Stream: Pause enable\n");
//			fflush(stdout);
			alSourcePause(ssh);
		} else {
//			printf("Stream: Pause disable\n");
//			fflush(stdout);
			alSourcePlay(ssh);
		}
	}


#if 0
	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム再生の実験コード
		@param[in]	filename	ファイル名
	*/
	//-----------------------------------------------------------------//
	void audio_io::test_stream(const char* filename)
	{
		static const int numbuff = 16;
		static const int bufsize = 4096;

		utils::fileio fin;
		if(!fin.open(filename, "rb")) {
			return;
		}
		wavio wav;
		audio_info info;
		wav.open_stream(fin, bufsize, info);
		const i_audio* aif = wav.get_stream();
		if(aif == 0) {
			wav.close_stream();
			fin.close();
			return;
		}

		int cnt = 0;

		ALuint source;
		alGenSources(1, &source);
		size_t pos = 0;
		while(pos < info.samples) {
			ALint n;
			alGetSourcei(source, AL_BUFFERS_QUEUED, &n);
			ALuint buffer;
			if(n < numbuff) {
				alGenBuffers(1, &buffer);
				++cnt;
			} else {
				ALint state;
				alGetSourcei(source, AL_SOURCE_STATE, &state);
				if(state != AL_PLAYING) {
					alSourcePlay(source);
				}
				while(alGetSourcei(source, AL_BUFFERS_PROCESSED, &n), n == 0) {
					useconds_t usec = 1000 * 10;	//[ms]
					usleep(usec);
				}
				alSourceUnqueueBuffers(source, 1, &buffer);
			}
			pos += wav.read_stream(fin, pos, bufsize);
			set_buffer(buffer, aif);
			alSourceQueueBuffers(source, 1, &buffer);
		}
		alDeleteSources(1, &source);

		wav.close_stream();
		fin.close();

		printf("buffer: %d\n", cnt);
	}
#endif

#if 0
		ALuint no;
		alGenBuffers(1, &no);
		ALshort data[8192];
		for(int i = 0; i < 8192; ++i) {
			// 580Hz
			data[i] = (ALshort)(cosf(2.0f * 3.141592f * (50.0f * (float)i) / 8192.0f) * 32767.0f);
		}
		alBufferData(no, AL_FORMAT_MONO16, data, 8192 * 2, 48000);

		audio_buffer2_ = no;

		alGenSources(1, &audio_source2_);
		alSourcei(audio_source2_, AL_BUFFER, audio_buffer2_);
		alSourcei(audio_source2_, AL_LOOPING, AL_TRUE);
		alSourcePlay(audio_source2_);
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void audio_io::destroy()
	{
		if(destroy_) return;

		purge_stream(0);

		alcMakeContextCurrent(NULL);
		alcDestroyContext(context_);
		alcCloseDevice(device_);
		destroy_ = true;
		init_ = true;
	}

	//-----------------------------------------------------------------//
	/*!
		@brief	スロットの状態を表示
		@param[in]	sh	スロット・ハンドル
	*/
	//-----------------------------------------------------------------//
	void audio_io::print_slots(slot_handle sh) const
	{
		std::string stt;
		ALint status;
		alGetSourcei(sh, AL_SOURCE_STATE, &status);
		switch(status) {
		case AL_INITIAL:
			stt = "INITIAL";
			break;
		case AL_STOPPED:
			stt = "STOPPED";
			break;
		case AL_PLAYING:
			stt = "PLAYING";
			break;
		case AL_PAUSED:
			stt = "PAUSED";
			break;
		}

		printf("%u: '%s', ", (unsigned int)sh, stt.c_str());
		printf("\n");
	}
}
