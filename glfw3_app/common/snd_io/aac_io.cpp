//=====================================================================//
/*!	@file
	@brief	AAC ファイルを扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "aac_io.hpp"
#include "pcm.hpp"
#include <cstring>
#include <cstdio>
#include "img_io/img_files.hpp"
#include "img_io/img_utils.hpp"
#include <boost/format.hpp>

namespace al {

	// make this higher to support files with more channels
	static const int MAX_CHANNELS = 6;

	// FAAD file buffering routines
	struct aac_buffer {
		utils::file_io&	fin;
		long	bytes_into_buffer;
		long	bytes_consumed;
		long	file_offset;
		unsigned char*	buffer;
		int	at_eof;
		aac_buffer(utils::file_io& fin_) : fin(fin_), bytes_into_buffer(0),
		  bytes_consumed(0), file_offset(0), buffer(0),
		  at_eof(0) { }
	};

	static int fill_buffer_(aac_buffer& b)
	{
		int bread;

		if (b.bytes_consumed > 0) {
			if (b.bytes_into_buffer) {
				memmove((void*)b.buffer, (void*)(b.buffer + b.bytes_consumed),
					b.bytes_into_buffer * sizeof(unsigned char));
			}

			if (!b.at_eof) {
				bread = b.fin.read(b.buffer + b.bytes_into_buffer, 1, b.bytes_consumed);

				if (bread != b.bytes_consumed) b.at_eof = 1;

				b.bytes_into_buffer += bread;
			}

			b.bytes_consumed = 0;

			if (b.bytes_into_buffer > 3) {
				if (memcmp(b.buffer, "TAG", 3) == 0) b.bytes_into_buffer = 0;
			}
			if (b.bytes_into_buffer > 11) {
				if (memcmp(b.buffer, "LYRICSBEGIN", 11) == 0) b.bytes_into_buffer = 0;
			}
			if (b.bytes_into_buffer > 8) {
				if (memcmp(b.buffer, "APETAGEX", 8) == 0) b.bytes_into_buffer = 0;
			}
		}
		return 1;
	}


	static void advance_buffer_(aac_buffer& b, int bytes)
	{
		b.file_offset += bytes;
		b.bytes_consumed = bytes;
		b.bytes_into_buffer -= bytes;
		if (b.bytes_into_buffer < 0) b.bytes_into_buffer = 0;
	}


	static const int adts_sample_rates_[] = {
		96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000,7350,0,0,0
	};


	static int adts_parse_(aac_buffer& b, int *bitrate, float *length)
	{
		int frames, frame_length;
		int t_framelength = 0;
		int samplerate;
		float frames_per_sec, bytes_per_frame;

		/* Read all frames to ensure correct time and bitrate */
		for (frames = 0; /* */; frames++) {
			fill_buffer_(b);

			if (b.bytes_into_buffer > 7) {
            	/* check syncword */
				if (!((b.buffer[0] == 0xFF)&&((b.buffer[1] & 0xF6) == 0xF0))) break;

				if (frames == 0) {
					samplerate = adts_sample_rates_[(b.buffer[2] & 0x3c) >> 2];
				}

				frame_length = ((((unsigned int)b.buffer[3] & 0x3)) << 11)
					| (((unsigned int)b.buffer[4]) << 3) | (b.buffer[5] >> 5);

				t_framelength += frame_length;

				if (frame_length > b.bytes_into_buffer) break;

				advance_buffer_(b, frame_length);
			} else {
				break;
			}
		}

		frames_per_sec = (float)samplerate / 1024.0f;
		if (frames != 0) {
			bytes_per_frame = (float)t_framelength / (float)(frames*1000);
		} else {
			bytes_per_frame = 0;
		}
		*bitrate = (int)(8. * bytes_per_frame * frames_per_sec + 0.5);
		if (frames_per_sec != 0) {
			*length = (float)frames / frames_per_sec;
		} else {
			*length = 1;
		}

		return 1;
	}


#if 0
	static int FindAdtsSRIndex_(int sr)
	{
		for (int i = 0; i < 16; i++) {
			if (sr == adts_sample_rates_[i]) return i;
		}
		return 16 - 1;
	}


	static unsigned char *MakeAdtsHeader_(int *dataSize,
		NeAACDecFrameInfo *hInfo, int old_format)
	{
		unsigned char *data;
		int profile = (hInfo->object_type - 1) & 0x3;
		int sr_index = ((hInfo->sbr == SBR_UPSAMPLED) || (hInfo->sbr == NO_SBR_UPSAMPLED)) ?
        	FindAdtsSRIndex_(hInfo->samplerate / 2) : FindAdtsSRIndex_(hInfo->samplerate);
		int skip = (old_format) ? 8 : 7;
		int framesize = skip + hInfo->bytesconsumed;

		if (hInfo->header_type == ADTS) framesize -= skip;

		*dataSize = 7;

		data = (unsigned char*)malloc(*dataSize * sizeof(unsigned char));
		memset(data, 0, *dataSize * sizeof(unsigned char));

		data[0] += 0xFF; /* 8b: syncword */

		data[1] += 0xF0; /* 4b: syncword */
		/* 1b: mpeg id = 0 */
		/* 2b: layer = 0 */
		data[1] += 1; /* 1b: protection absent */

		data[2] += ((profile << 6) & 0xC0); /* 2b: profile */
		data[2] += ((sr_index << 2) & 0x3C); /* 4b: sampling_frequency_index */
		/* 1b: private = 0 */
		data[2] += ((hInfo->channels >> 2) & 0x1); /* 1b: channel_configuration */

		data[3] += ((hInfo->channels << 6) & 0xC0); /* 2b: channel_configuration */
		/* 1b: original */
		/* 1b: home */
		/* 1b: copyright_id */
		/* 1b: copyright_id_start */
		data[3] += ((framesize >> 11) & 0x3); /* 2b: aac_frame_length */

		data[4] += ((framesize >> 3) & 0xFF); /* 8b: aac_frame_length */

		data[5] += ((framesize << 5) & 0xE0); /* 3b: aac_frame_length */
		data[5] += ((0x7FF >> 6) & 0x1F); /* 5b: adts_buffer_fullness */

		data[6] += ((0x7FF << 2) & 0x3F); /* 6b: adts_buffer_fullness */
		/* 2b: num_raw_data_blocks */

		return data;
	}
#endif

	static audio decode_aac_file_(utils::file_io& fin)
	{
		int old_format = 0;
		int outputFormat = FAAD_FMT_16BIT;
		int object_type = LC;
		int downMatrix = 0;
		int def_srate = 0;

		int tagsize;
		unsigned long samplerate;
		unsigned char channels;
		void *sample_buffer;

		NeAACDecHandle hDecoder;
		NeAACDecFrameInfo frameInfo;
		NeAACDecConfigurationPtr config;

		char percents[200];
		int percent, old_percent = -1;
		int bread, fileread;
		int header_type = 0;
		int bitrate = 0;
		float length = 0;

///		int first_time = 1;

		aac_buffer b(fin);

		fileread = b.fin.get_file_size();

		if (!(b.buffer = (unsigned char*)malloc(FAAD_MIN_STREAMSIZE * MAX_CHANNELS))) {
			/// std::cerr << "Memory allocation error\n";
			return 0;
		}
		memset(b.buffer, 0, FAAD_MIN_STREAMSIZE * MAX_CHANNELS);

		bread = b.fin.read(b.buffer, 1, FAAD_MIN_STREAMSIZE * MAX_CHANNELS);
		b.bytes_into_buffer = bread;
		b.bytes_consumed = 0;
		b.file_offset = 0;

		if (bread != FAAD_MIN_STREAMSIZE*MAX_CHANNELS) b.at_eof = 1;

		tagsize = 0;
		if (!memcmp(b.buffer, "ID3", 3)) {
	        /* high bit is not used */
			tagsize = (b.buffer[6] << 21) | (b.buffer[7] << 14) |
				(b.buffer[8] <<  7) | (b.buffer[9] <<  0);

			tagsize += 10;
			advance_buffer_(b, tagsize);
			fill_buffer_(b);
		}

		hDecoder = NeAACDecOpen();

		/* Set the default object type and samplerate */
		/* This is useful for RAW AAC files */
		config = NeAACDecGetCurrentConfiguration(hDecoder);
		if (def_srate) {
			config->defSampleRate = def_srate;
		}
		config->defObjectType = object_type;
		config->outputFormat = outputFormat;
		config->downMatrix = downMatrix;
		config->useOldADTSFormat = old_format;
		//config->dontUpSampleImplicitSBR = 1;
		NeAACDecSetConfiguration(hDecoder, config);

		/* get AAC infos for printing */
		header_type = 0;
		if ((b.buffer[0] == 0xFF) && ((b.buffer[1] & 0xF6) == 0xF0)) {
			adts_parse_(b, &bitrate, &length);
			b.fin.seek(tagsize, utils::file_io::seek::set);

			bread = b.fin.read(b.buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS);
			if (bread != FAAD_MIN_STREAMSIZE * MAX_CHANNELS) b.at_eof = 1;
			else b.at_eof = 0;
			b.bytes_into_buffer = bread;
			b.bytes_consumed = 0;
			b.file_offset = tagsize;

			header_type = 1;
		} else if (memcmp(b.buffer, "ADIF", 4) == 0) {
			int skip_size = (b.buffer[4] & 0x80) ? 9 : 0;
			bitrate = ((unsigned int)(b.buffer[4 + skip_size] & 0x0F)<<19) |
				((unsigned int)b.buffer[5 + skip_size]<<11) |
				((unsigned int)b.buffer[6 + skip_size]<<3) |
				((unsigned int)b.buffer[7 + skip_size] & 0xE0);

			length = (float)fileread;
			if (length != 0) {
				length = ((float)length*8.f) / ((float)bitrate) + 0.5f;
			}

			bitrate = (int)((float)bitrate / 1000.0f + 0.5f);

			header_type = 2;
		}

///    *song_length = length;

		fill_buffer_(b);
		if ((bread = NeAACDecInit(hDecoder, b.buffer,
			b.bytes_into_buffer, &samplerate, &channels)) < 0) {
			/* If some error initializing occured, skip the file */
			std::cerr << "Error initializing decoder library.\n";
            free(b.buffer);
			NeAACDecClose(hDecoder);
			return 0;
		}
		advance_buffer_(b, bread);
		fill_buffer_(b);

#if 0
		/* print AAC file info */
		std::cerr << aacfile << " file info:\n"; 
		switch (header_type) {
		case 0:
			std::cerr << "RAW\n\n";
		break;
		case 1:
			std::cerr << boost::format("ADTS, %.3f sec, %d kbps, %d Hz\n\n")
				% length % bitrate % samplerate;
		break;
		case 2:
			std::cerr << boost::format("ADIF, %.3f sec, %d kbps, %d Hz\n\n")
				% length % bitrate % samplerate;
		break;
		}
#endif

		do {
			sample_buffer = NeAACDecDecode(hDecoder, &frameInfo,
				b.buffer, b.bytes_into_buffer);

			/* update buffer indices */
			advance_buffer_(b, frameInfo.bytesconsumed);

			if (frameInfo.error > 0) {
				std::cerr << "Error: " << NeAACDecGetErrorMessage(frameInfo.error) << std::endl;
			}

			percent = std::min(static_cast<int>((b.file_offset * 100) / fileread), 100);
			if (percent > old_percent) {
				old_percent = percent;
				/// std::cerr << boost::format("%d%% decoding %s.") % percent % aacfile;
				std::cerr << boost::format("%s\r") % percents;
			}

			if ((frameInfo.error == 0) && (frameInfo.samples > 0)) {
				/// if (write_audio_file(aufile, sample_buffer, frameInfo.samples, 0) == 0)
                /// break;
			}

			/* fill buffer */
			fill_buffer_(b);

			/* to make sure it stops now */
			if (b.bytes_into_buffer == 0) sample_buffer = NULL;

		} while (sample_buffer != NULL);

		NeAACDecClose(hDecoder);

		free(b.buffer);

//		return frameInfo.error;
		return 0;
	}


	static uint32_t read_callback_(void* user_data, void* buffer, uint32_t length)
	{
		utils::file_io* fin = static_cast<utils::file_io*>(user_data);
		if(fin) return fin->read(buffer, 1, length);
		return 0;
	}

	static uint32_t seek_callback_(void* user_data, uint64_t position)
	{
		utils::file_io* fin = static_cast<utils::file_io*>(user_data);
		if(fin) return fin->seek(position, utils::file_io::seek::set);
		else return 0;
	}

	static int get_aac_track_(mp4ff_t *infile)
	{
		/* find AAC track */
		int numTracks = mp4ff_total_tracks(infile);

		for(int i = 0; i < numTracks; i++) {
			unsigned char *buff = NULL;
			unsigned int buff_size = 0;
			mp4AudioSpecificConfig mp4ASC;

			mp4ff_get_decoder_config(infile, i, &buff, &buff_size);

			if (buff) {
				int rc = NeAACDecAudioSpecificConfig(buff, buff_size, &mp4ASC);
				free(buff);

				if (rc < 0) continue;
				return i;
			}
		}

		/* can't decode this */
		return -1;
	}


#if 0
	static void list_mp4(mp4ff_t* infile, mp4AudioSpecificConfig& mp4ASC, int track)
	{
       	static const char* ot[] = {
			"NULL", "MAIN AAC", "LC AAC", "SSR AAC", "LTP AAC", "HE AAC"
		};
		long samples = mp4ff_num_samples(infile, track);
		float f = 1024.0f;
		float seconds;
		if (mp4ASC.sbr_present_flag == 1) {
			f = f * 2.0;
		}
		seconds = (float)samples * (float)(f - 1.0f) / (float)mp4ASC.samplingFrequency;

		std::cout << boost::format("%s\t%.3f secs, %d ch, %d Hz\n\n")
			% (ot[(mp4ASC.objectTypeIndex > 5) ? 0 : mp4ASC.objectTypeIndex])
			% seconds % mp4ASC.channelsConfiguration % mp4ASC.samplingFrequency;

#define PRINT_MP4_METADATA
#ifdef PRINT_MP4_METADATA
///		char *tag = 0;
///		char *item = 0;
		int j = mp4ff_meta_get_num_items(infile);
		for(int k = 0; k < j; k++) {
			char* item;
			char* value;
			if(mp4ff_meta_get_by_index(infile, k, &item, &value)) {
   				if(std::string(item) == "cover") {
	   				size_t size = mp4ff_meta_get_item_size(infile, k);
	   				std::cout << boost::format("%s: (%d)\n") % item % size;
	   			} else {
	   				std::cout << boost::format("%s: %s\n") % item % value;
	   			}
				mp4ff_meta_free(infile, item, value);
   			}
		}
		if (j > 0) std::cout << std::endl;
#endif
	}
#endif

	void aac_io::destroy_mp4_(decode_mp4_t& dt)
	{
		mp4ff_close(dt.infile);
		NeAACDecClose(dt.h_decoder);
	}


	bool aac_io::decode_audio_mp4_(decode_mp4_t& dt, uint32_t idx)
	{
		long dur = mp4ff_get_sample_duration(dt.infile, dt.track, idx);
		unsigned char* tmp = 0;
		uint32_t size = 0;
		int rc = mp4ff_read_sample(dt.infile, dt.track, idx, &tmp, &size);
		if(rc == 0) {
			dt.tmp = 0;
			return false;
		}

		dt.sample = NeAACDecDecode(dt.h_decoder, &dt.frame_info, tmp, size);
		dt.tmp = tmp;

		if(!dt.no_gapless) {
			if(idx == 0) dur = 0;

			if(dt.useAacLength || (dt.timescale != dt.samplerate)) {
				dt.count = dt.frame_info.samples;
			} else {
				dt.count = (unsigned int)(dur * dt.frame_info.channels);
				if(dt.count > dt.frame_info.samples) {
					dt.count = dt.frame_info.samples;
				}

				if(!dt.useAacLength && !dt.initial && (idx < dt.samples / 2)
				  && (dt.count != dt.frame_info.samples)) {
					std::cout << "MP4 seems to have incorrect frame duration, using values from AAC data.\n";
					dt.useAacLength = 1;
					dt.count = dt.frame_info.samples;
				}
			}

			if(dt.initial && (dt.count < dt.framesize * dt.frame_info.channels)
			  && (dt.frame_info.samples > dt.count)) {
				dt.delay = dt.frame_info.samples - dt.count;
			}
		} else {
			dt.count = dt.frame_info.samples;
		}

		if(dt.count > 0) dt.initial = true;

		return true;
	}


	bool aac_io::decode_track_mp4_(utils::file_io& fin, decode_mp4_t& dt)
	{
		dt.h_decoder = NeAACDecOpen();

		// Set configuration
		dt.config = NeAACDecGetCurrentConfiguration(dt.h_decoder);
		dt.config->outputFormat = FAAD_FMT_16BIT;
		dt.config->downMatrix   = 0;
		//config->dontUpSampleImplicitSBR = 1;
		NeAACDecSetConfiguration(dt.h_decoder, dt.config);

		dt.mp4cb.read = read_callback_;
		dt.mp4cb.seek = seek_callback_;
		dt.mp4cb.user_data = &fin;
		dt.infile = mp4ff_open_read(&dt.mp4cb);
		if(dt.infile == 0) {
			NeAACDecClose(dt.h_decoder);
			return false;
		}

		dt.track = get_aac_track_(dt.infile);
		if(dt.track < 0) {
///			std::cout << "Unable to find correct AAC sound track in the MP4 file.\n";
			mp4ff_close(dt.infile);
			NeAACDecClose(dt.h_decoder);
			return false;
		}

/// std::cout << boost::format("Decode track: %d\n") % dt.track;

		return true;
	}


	bool aac_io::decode_mp4_param_(decode_mp4_t& dt)
	{
		unsigned char* buffer = 0;
		unsigned int buffer_size = 0;
		mp4ff_get_decoder_config(dt.infile, dt.track, &buffer, &buffer_size);

		unsigned long samplerate;
		unsigned char channels;
		if(NeAACDecInit2(dt.h_decoder, buffer, buffer_size, &samplerate, &channels) < 0) {
///	std::cout << "Error initializing decoder library.\n";
			NeAACDecClose(dt.h_decoder);
			mp4ff_close(dt.infile);
			return false;
		}

		dt.channels = channels;
		dt.samplerate = samplerate;
		dt.useAacLength = 0;
		dt.no_gapless = false;
		dt.timescale = mp4ff_time_scale(dt.infile, dt.track);
		dt.framesize = 1024;
		if(NeAACDecAudioSpecificConfig(buffer, buffer_size, &dt.mp4ASC) >= 0) {
			if(dt.mp4ASC.frameLengthFlag == 1) dt.framesize = 960;
			if(dt.mp4ASC.sbr_present_flag == 1) dt.framesize *= 2;
		}
		free(buffer);

		dt.samples = mp4ff_num_samples(dt.infile, dt.track);

		return true;
	}


	audio aac_io::decode_mp4_file_(utils::file_io& fin)
	{
		decode_mp4_t dt;
		if(!decode_track_mp4_(fin, dt)) {
			return 0;
		}

		if(!decode_mp4_param_(dt)) {
			return 0;
		}

/// list_mp4(dt.infile, dt.mp4ASC, dt.track);

		if(dt.samples <= 1) {
			return 0;
		}

		uint32_t out_size = (dt.samples - 1) * dt.framesize;
		audio aif;
		if(dt.channels == 1) {
			aif = audio(new audio_mno16);
		} else if(dt.channels == 2) {
			aif = audio(new audio_sto16);
		} else {
			std::cout << boost::format("Can't decode chanel: %d\n") % dt.channels;
			fflush(stdout);
			NeAACDecClose(dt.h_decoder);
			mp4ff_close(dt.infile);
			return aif;
		}
		aif->create(dt.samplerate, out_size);

/// std::cout << boost::format("Chanels: %d\n") % channels);
/// std::cout << boost::format("Sample rate: %d [Hz]\n") % samplerate);
/// std::cout << boost::format("Samples: %d\n") % out_size);

		dt.count = 0;
		uint32_t pos = 0;
		for(uint32_t idx = 0; idx < dt.samples; idx++) {
			dt.delay = 0;
			bool f = decode_audio_mp4_(dt, idx);
			if(!f) {
				std::cout << boost::format("Reading from MP4 file failed. (%d)\n") % dt.count;
				destroy_mp4_(dt);
				return 0;
			}

			if(dt.delay) {
/// std::cout << boost::format("Delay: %d\n") % dt.delay;
			}

			for(uint32_t i = 0; i < dt.count / dt.channels; ++i) {
				if(dt.channels == 1) {
					short* p = static_cast<short*>(dt.sample);
					pcm16_m pcm;
					pcm.w = p[i];
					aif->put(pos, pcm);
				} else if(dt.channels == 2) {
					short* p = static_cast<short*>(dt.sample);
					pcm16_s pcm;
					pcm.l = p[i * 2 + 0];
					pcm.r = p[i * 2 + 1];
					aif->put(pos, pcm);
				}
				++pos;
			}
			free(dt.tmp);
		}

		destroy_mp4_(dt);

		return aif;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	AAC ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool aac_io::probe(utils::file_io& fin)
	{
		uint32_t pos = fin.tell();
		bool ret = false;
		uint8_t header[8];
		uint32_t len = fin.read(header, 1, 8);
		fin.seek(pos, utils::file_io::seek::set);
		if(len != 8) {
			return ret;
		}

		if(header[4] == 'f' && header[5] == 't' && header[6] == 'y' && header[7] == 'p') {
			decode_mp4_t dt;
			if(decode_track_mp4_(fin, dt)) {
				ret = true;
				destroy_mp4_(dt);
			}
		} else {

// decode AAC...

		}
		return ret;
	}


	bool aac_io::create_mp4_info_(utils::file_io& fin, decode_mp4_t& dt, audio_info& info)
	{
		dt.no_gapless = false;
		dt.initial = false;
		if(!decode_track_mp4_(fin, dt)) {
			return false;
		}
		if(!decode_mp4_param_(dt)) {
			return false;
		}
		if(dt.samples <= 1) {
			return false;
		}

		bool ret = true;
		if(dt.channels == 1) {
			info.type = audio_format::PCM16_MONO;
		} else if(dt.channels == 2) {
			info.type = audio_format::PCM16_STEREO;
		} else {
			ret = false;
		}
		if(ret) {
			info.samples = (dt.samples - 1) * dt.framesize;
			info.chanels = dt.channels;
			info.bits = 16;
			info.frequency = dt.samplerate;
			info.block_align = dt.channels * 2;
		}

		tag_.clear();

		int j = mp4ff_meta_get_num_items(dt.infile);
		for(int k = 0; k < j; ++k) {
			char* item;
			char* value;
			if(mp4ff_meta_get_by_index(dt.infile, k, &item, &value)) {
   				std::string s = item;
				{
///					size_t size = mp4ff_meta_get_item_size(dt.infile, k);
///					std::cout << s << ", " << static_cast<int>(size) << ", " << value << std::endl;
				}
	   			if(s == "title") tag_.title_ = value;
		   		else if(s == "artist") tag_.artist_ = value;
			   	else if(s == "writer") tag_.writer_ = value;
				else if(s == "album") tag_.album_ = value;
		   		else if(s == "track") tag_.track_ = value;
		   		else if(s == "totaltracks") tag_.total_tracks_ = value;
		   		else if(s == "disc") tag_.disc_ = value;
		   		else if(s == "totaldiscs") tag_.total_discs_ = value;
		   		else if(s == "date") tag_.date_ = value;
		   		else if(s == "cover") {
					size_t size = mp4ff_meta_get_item_size(dt.infile, k);
		   			utils::file_io fio;
		   			fio.open(value, size);
		   			img::img_files imgf;
		   			imgf.initialize();
		   			imgf.load(fio);
		   			if(imgf.get_image()) {
		   				tag_.image_ = tag::image(img::copy_image(imgf.get_image()));
		   			} else {
		   				tag_.image_ = 0;
		   			}
		   			fio.close();
		   		}
				mp4ff_meta_free(dt.infile, item, value);
			}
		}
		tag_.update();

		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	AAC ファイルの情報を取得する
		@param[in]	fin		file_io クラス
		@param[in]	info	情報を受け取る構造体
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool aac_io::info(utils::file_io& fin, audio_info& info)
	{
		long pos = fin.tell();
		uint8_t header[8];
		uint32_t len = fin.read(header, 1, 8);
		fin.seek(pos, utils::file_io::seek::set);
		if(len != 8) return false;

		if(header[4] == 'f' && header[5] == 't' && header[6] == 'y' && header[7] == 'p') {
			decode_mp4_t dt;
			bool ret = create_mp4_info_(fin, dt, info);
			destroy_mp4_(dt);
			return ret;
		} else {

// info AAC

		}

		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロードする
		@param[in]	fin	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool aac_io::load(utils::file_io& fin, const std::string& opt)
	{
		long pos = fin.tell();
		uint8_t header[8];
		uint32_t len = fin.read(header, 1, 8);
		fin.seek(pos, utils::file_io::seek::set);
		if(len != 8) return false;

		if(header[4] == 'f' && header[5] == 't' && header[6] == 'y' && header[7] == 'p') {
			audio_ = decode_mp4_file_(fin);
		} else {
			audio_ = decode_aac_file_(fin);
		}

		return (audio_ != 0);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	音楽ファイルをセーブする
		@param[in]	fout	file_io クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool aac_io::save(utils::file_io& fout, const std::string& opt)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム・オープンする
		@param[in]	fi		file_io クラス
		@param[in]	size	バッファサイズ
		@param[in]	info	オーディオ情報を受け取る
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool aac_io::open_stream(utils::file_io& fi, int size, audio_info& info)
	{
		long pos = fi.tell();
		uint8_t header[8];
		uint32_t len = fi.read(header, 1, 8);
		fi.seek(pos, utils::file_io::seek::set);
		if(len != 8) return false;

		if(header[4] == 'f' && header[5] == 't' && header[6] == 'y' && header[7] == 'p') {
			create_mp4_info_(fi, mp4_t_, info);

			if(mp4_t_.channels == 1) {
				info.type = audio_format::PCM16_MONO;
				stream_ = audio(new audio_mno16);
				buffer_ = audio(new audio_mno16);
			} else if(mp4_t_.channels == 2) {
				info.type = audio_format::PCM16_STEREO;
				stream_ = audio(new audio_sto16);
				buffer_ = audio(new audio_sto16);
			} else {
				return false;
			}
			stream_->zero();

			stream_->create(info.frequency, size);
			mp4_t_.count = 0;
			mp4_t_.delay = 0;
			index_ = 0;
			while(mp4_t_.count == 0) {
				bool f = decode_audio_mp4_(mp4_t_, index_);
				if(!f) {
					return 0;
				}
				free(mp4_t_.tmp);
				++index_;
			}
			--index_;
			buffer_->create(mp4_t_.samplerate, mp4_t_.count);
			buffer_cnt_ = 0;
			stream_cnt_ = 0;
			return true;
		} else {

// info AAC


		}

		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリーム・リード
		@param[in]	fin		ファイルI/O
		@param[in]	offset	開始位置
		@param[in]	samples	読み込むサンプル数
		@return 読み込んだサンプル数
	*/
	//-----------------------------------------------------------------//
	size_t aac_io::read_stream(utils::file_io& fin, size_t offset, size_t samples)
	{
		if(stream_cnt_) {
			while(stream_cnt_ < stream_->get_samples()) {
				if(mp4_t_.channels == 1) {
					pcm16_m pcm;
					buffer_->get(buffer_cnt_, pcm);
					stream_->put(stream_cnt_, pcm);
				} else {
					pcm16_s pcm;
					buffer_->get(buffer_cnt_, pcm);
					stream_->put(stream_cnt_, pcm);
				}
				++buffer_cnt_;
				++stream_cnt_;
			}
		}

		uint32_t bnum = mp4_t_.count / mp4_t_.channels;
		uint32_t idx = (offset / bnum) + index_;

		uint32_t smp = samples;

		stream_cnt_ = 0;
		for(int n = 0; n < 2; ++n) {
			mp4_t_.delay = 0;
			bool f = decode_audio_mp4_(mp4_t_, idx);
			if(!f) {
				return 0;
			}
			++idx;

			uint32_t pos = 0;
			for(uint32_t i = 0; i < bnum; ++i) {
				if(mp4_t_.channels == 1) {
					short* p = static_cast<short*>(mp4_t_.sample);
					pcm16_m pcm;
					pcm.w = p[i];
					buffer_->put(pos, pcm);
				} else if(mp4_t_.channels == 2) {
					short* p = static_cast<short*>(mp4_t_.sample);
					pcm16_s pcm;
					pcm.l = p[i * 2 + 0];
					pcm.r = p[i * 2 + 1];
					buffer_->put(pos, pcm);
				}
				++pos;
			}
			free(mp4_t_.tmp);

			for(uint32_t i = 0; i < bnum; ++i) {
				if(mp4_t_.channels == 1) {
					pcm16_m pcm;
					buffer_->get(i, pcm);
					stream_->put(stream_cnt_, pcm);
				} else {
					pcm16_s pcm;
					buffer_->get(i, pcm);
					stream_->put(stream_cnt_, pcm);
				}
				++stream_cnt_;
				--smp;
				if(smp == 0) {
					stream_cnt_ = stream_cnt_ % bnum;
					return samples;
				}
			}
		}

		if(stream_cnt_ >= stream_->get_samples()) {
			stream_cnt_ = 0;
		}

		return samples;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ストリームをクローズ
	*/
	//-----------------------------------------------------------------//
	void aac_io::close_stream()
	{
		destroy_mp4_(mp4_t_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	廃棄
	*/
	//-----------------------------------------------------------------//
	void aac_io::destroy()
	{
	}

}
