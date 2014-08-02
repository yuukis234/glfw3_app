#pragma once
//=====================================================================//
/*!	@file
	@brief	DDS 画像を扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img_io/i_img_io.hpp"
#include "img_io/img_idx8.hpp"
#include "img_io/img_rgba8.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	DDS 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class dds_io : public i_img_io {

		shared_img	img_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		dds_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~dds_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "dds"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	DDS ファイルか確認する
			@param[in]	fin	file_io クラス
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool probe(utils::file_io& fin) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	画像ファイルの情報を取得する
			@param[in]	fin	file_io クラス
			@param[in]	fo	情報を受け取る構造体
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool info(utils::file_io& fin, img::img_info& fo) override;


		//-----------------------------------------------------------------//
		/*!
			@brief	DDS ファイル、ロード(utils::file_io)
			@param[in]	fin	ファイル I/O クラス
			@param[in]	ext	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& ext = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	DDS ファイルをセーブする
			@param[in]	fout	ファイル I/O クラス
			@param[in]	ext	フォーマット固有の設定文字列
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& ext = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースを取得
			@return	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		shared_img get_image() override { return img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースの登録
			@param[in]	imf	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		void set_image(shared_img img) override { img_ = img; }
	};

}

