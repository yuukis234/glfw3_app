#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenAL オーディオ入出力（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include <cmath>

#ifdef WIN32
#include <AL/alc.h>
#include <AL/al.h>
#include <AL/alext.h>
#endif

#ifdef __APPLE__
#include <OpenAL/alc.h>
#include <OpenAL/al.h>
#endif

#if defined(IPHONE) || defined(IPAD) || defined(IPHONE_IPAD)
#include <OpenAL/alc.h>
#include <OpenAL/al.h>
#ifndef IPHONE_IPAD
#define IPHONE_IPAD
#endif
#endif

#include "utils/file_io.hpp"
#include "i_audio.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	audioio クラス（OpenAL のラッパークラス）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class audio_io {

	public:
		typedef ALuint	wave_handle;
		typedef ALuint	slot_handle;

	private:
		ALCdevice*	device_;
		ALCcontext*	context_;

		std::string	error_message_;

		int		queue_max_;
		bool	init_;
		bool	destroy_;

		bool set_buffer(ALuint bh, const i_audio* aif);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		audio_io() : device_(0), context_(0),
					queue_max_(16), init_(false), destroy_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~audio_io() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	コンテキストの情報を表示（OpenAL）
		*/
		//-----------------------------------------------------------------//
		void context_info() const;


		//-----------------------------------------------------------------//
		/*!
			@brief	ALC の情報を表示
		*/
		//-----------------------------------------------------------------//
		void alc_info() const;


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・インターフェースから波形（buffer）を作成する
			@param[in]	aif	オーディオ・インターフェース・クラス
			@return	波形・ハンドルを返す
		*/
		//-----------------------------------------------------------------//
		wave_handle create_wave(const i_audio* aif);


		//-----------------------------------------------------------------//
		/*!
			@brief	波形を廃棄する。
			@param[in]	wh	波形ハンドル
		*/
		//-----------------------------------------------------------------//
		void destroy_wave(wave_handle wh);


		//-----------------------------------------------------------------//
		/*!
			@brief	スロット（ソース）を作成する。
			@param[in]	h	波形・ハンドル
			@return スロット・ハンドルを返す。
		*/
		//-----------------------------------------------------------------//
		slot_handle create_slot(wave_handle h);


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットにループを設定する。
			@param[in]	sh	スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void set_loop(slot_handle sh, bool flag = true) {
			if(flag) {
				alSourcei(sh, AL_LOOPING, AL_TRUE);
			} else {
				alSourcei(sh, AL_LOOPING, AL_FALSE);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットにゲインを設定する
			@param[in]	sh	スロット・ハンドル
			@param[in]	gain	ゲイン値（0.0 to 1.0)
		*/
		//-----------------------------------------------------------------//
		void set_gain(slot_handle sh, float gain) { alSourcef(sh, AL_GAIN, gain); }


		//-----------------------------------------------------------------//
		/*!
			@brief	再生
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool play(slot_handle sh) {
			bool ret = true;
			if(sh) {
				alSourcePlay(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	一時停止
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool pause(slot_handle sh) {
			bool ret = true;
			if(sh) {
				alSourcePause(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	停止
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool stop(slot_handle sh) {
			bool ret = true;
			if(sh) {
				alSourceStop(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	巻き戻し
			@param[in]	sh	スロット・ハンドル
							※「０」の場合は、全スロット
		*/
		//-----------------------------------------------------------------//
		bool rewind(slot_handle sh) {
			bool ret = true;
			if(sh) {
				alSourceRewind(sh);
			} else {
				ret = false;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットの演奏状態を得る
			@param[in]	sh	スロット・ハンドル
			@return	「true」なら演奏中
		*/
		//-----------------------------------------------------------------//
		bool get_slot_status(slot_handle sh) const;


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットを廃棄する。
			@param[in]	h	スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void destroy_slot(slot_handle h);


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットに、波形をセットする
			@param[in]	sh	再生するスロット・ハンドル
			@param[in]	wh	波形ハンドル
			@return	成功したら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_wave(slot_handle sh, wave_handle wh);


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームを浄化する
			@param[in]	ssh	ストリーム・スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void purge_stream(slot_handle ssh);


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生の空きバッファを返す。
			@param[in]	ssh	ストリーム・スロット・ハンドル
			@return 有効な、バッファがあれば、そのハンドルを返す。（「０」なら無効）
		*/
		//-----------------------------------------------------------------//
		wave_handle status_stream(slot_handle ssh);


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム再生キューにバッファをセットする。
			@param[in]	ssh	ストリーム・スロット・ハンドル
			@param[in]	bh	有効なバッファ・ハンドル
			@param[in]	aif	バッファにセットするオーディオ・インターフェース@n
							※常に、同じ構成を与える必要がある。
		*/
		//-----------------------------------------------------------------//
		void queue_stream(slot_handle ssh, wave_handle bh, const i_audio* aif);


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをポーズする。
			@param[in]	ssh	ストリーム・スロット・ハンドル
			@param[in]	ena	「false」ならポーズ解除
		*/
		//-----------------------------------------------------------------//
		void pause_stream(slot_handle ssh, bool ena = true);


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();


		//-----------------------------------------------------------------//
		/*!
			@brief	最後のエラーメッセージを取得
			@return	エラーメッセージ
		*/
		//-----------------------------------------------------------------//
		const std::string& get_error_message() const { return error_message_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スロットの状態を表示
			@param[in]	sh	スロット・ハンドル
		*/
		//-----------------------------------------------------------------//
		void print_slots(slot_handle sh) const;

	};

}	// al
