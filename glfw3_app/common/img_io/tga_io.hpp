#pragma once
//=====================================================================//
/*!	@file
	@brief	TGA 画像を扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "i_img_io.hpp"
#include "img_idx8.hpp"
#include "img_rgba8.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	TGA 画像クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class tga_io : public i_img_io {

		const i_img*	imf_;

		img_idx8	idx_;
		img_rgba8	img_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		tga_io() : imf_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~tga_io() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル拡張子を返す
			@return ファイル拡張子の文字列
		*/
		//-----------------------------------------------------------------//
		const char* get_file_ext() const override { return "tga"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	TGA ファイルか確認する @n
					※TGA形式はシグネチュアが無い為、不正なファイルでも「true」を返す場合がある
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
			@brief	TGA ファイル、ロード(utils::file_io)
			@param[in]	fin	ファイル I/O クラス
			@param[in]	opt	フォーマット固有の設定文字列
			@return エラーなら「false」を返す
		*/
		//-----------------------------------------------------------------//
		bool load(utils::file_io& fin, const std::string& opt = "") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	TGA ファイルをセーブする(utils::file_io)
			@param[in]	fout	ファイル I/O クラス
			@param[in]	opt		フォーマット固有の設定文字列 @n
								「rle」を指定すると圧縮で出力する
			@return エラーがあれば「false」
		*/
		//-----------------------------------------------------------------//
		bool save(utils::file_io& fout, const std::string& opt = "rle") override;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースを取得
			@return	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		const i_img* get_image() const override {
			if(!idx_.empty()) {
				return &idx_;
			} else if(!img_.empty()) {
				return &img_;
			} else {
				return 0;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージインターフェースの登録
			@param[in]	imf	イメージインターフェース
		*/
		//-----------------------------------------------------------------//
		void set_image(const i_img* img) override { imf_ = img; }

	};
}
