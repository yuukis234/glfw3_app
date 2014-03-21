#pragma once
//=====================================================================//
/*!	@file
	@brief	OGG ファイルを扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <neaacdec.h>
#include <ogg.h>
#include "snd_io/i_snd_io.hpp"

namespace al {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	OGG 音声ファイルクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ogg_io : public i_snd_io {

		i_audio*		audio_;
		i_audio*		stream_;

		tag				tag_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		ogg_io() : audio_(0), stream_(0), tag_() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~ogg_io() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize() override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "oog"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	AAC ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	AAC ファイルの情報を取得する
			@param[in]	fin		file_io クラス
			@param[in]	info	情報を受け取る構造体
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, audio_info& info) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルのタグを取得
			@return タグを返す
		*/
		//-----------------------------------------------------------------//
		const tag& get_tag() const override { return tag_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ロードする
			@param[in]	fin	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	音楽ファイルをセーブする
			@param[in]	fout	file_io クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・オープンする
			@param[in]	fi		file_io クラス
			@param[in]	size	バッファサイズ
			@param[in]	info	オーディオ情報を受け取る
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open_stream(utils::file_io& fi, int size, audio_info& info) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム読み込みバッファを取得する
			@return ストリーム用オーディオ・インターフェース
		*/
		//-----------------------------------------------------------------//
		const i_audio* get_stream() const override { return stream_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリーム・リード
			@param[in]	fin		ファイルI/O
			@param[in]	offset	開始位置
			@param[in]	samples	読み込むサンプル数
			@return 読み込んだサンプル数
		*/
		//-----------------------------------------------------------------//
		size_t read_stream(utils::file_io& fin, size_t offset, size_t samples) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	ストリームをクローズ
		*/
		//-----------------------------------------------------------------//
		void close_stream() override;


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオ・インターフェースを取得する
			@return オーディオ・インターフェースクラス
		*/
		//-----------------------------------------------------------------//
		const i_audio* get_audio_if() const override { return audio_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	オーディオの登録
			@param[in]	aif	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		void set_audio_if(const i_audio* aif) override { }


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy() override;

	};

}
