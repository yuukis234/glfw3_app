#pragma once
//=====================================================================//
/*! @file
	@brief  BMC コア関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <bitset>
#include "utils/string_utils.hpp"
#include "utils/bit_array.hpp"
#include "img_io/img_rgba8.hpp"
#include "img_io/bdf_io.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  BMC コア・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class bmc_core {
	public:

		struct option {
			enum type {
				preview,	///< プレビューを有効
				verbose,	///< 詳細なメッセージ出力
				no_header,	///< サイズヘッダーを出力しない
				text,		///< テキストベース出力
				c_style,	///< C スタイルのテキスト出力
				offset,		///< オフセット
				size,		///< サイズ
				bdf,		///< BDF ファイル入力
				append,		///< 追加出力
				inverse,	///< 画像反転

///				dither,

				limit_
			};
		};

	private:
		std::bitset<option::limit_>	option_;
		std::string	inp_fname_;
		std::string out_fname_;
		std::string	symbol_;
		vtx::srect	clip_;

		int		argc_;
		char**	argv_;

		float	version_;

		img::img_rgba8	src_img_;
		img::img_rgba8	dst_img_;

		utils::bit_array	bits_;

		void bitmap_convert_();
		void bitmap_convert_(img::bdf_io& bdf);
		uint32_t save_file_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		bmc_core(int argc, char** argv) : argc_(argc), argv_(argv), clip_(0),
			version_(0.25f) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  help 表示
		*/
		//-----------------------------------------------------------------//
		void help() const;


		//-----------------------------------------------------------------//
		/*!
			@brief  コマンドライン解析
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool analize();


		//-----------------------------------------------------------------//
		/*!
			@brief  実行
			@return エラーが無ければ「true」
		*/
		//-----------------------------------------------------------------//
		bool execute();


		//-----------------------------------------------------------------//
		/*!
			@brief  オプションの取得
			@param[in]	t	オプション・タイプ
			@return 状態
		*/
		//-----------------------------------------------------------------//
		bool get_option(option::type t) const { return option_[t]; }


		//-----------------------------------------------------------------//
		/*!
			@brief  入力ファイルの取得
			@return 入力ファイル名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_inp_file() const { return inp_fname_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  出力ファイルの取得
			@return 出力ファイル名
		*/
		//-----------------------------------------------------------------//
		const std::string& get_out_file() const { return out_fname_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ソース画像の参照を取得
			@return ソース画像の参照
		*/
		//-----------------------------------------------------------------//
		const img::img_rgba8& get_src_image() const { return src_img_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  ソース画像の参照を取得
			@return ソース画像の参照
		*/
		//-----------------------------------------------------------------//
		const img::img_rgba8& get_dst_image() const { return dst_img_; }
	};
}
