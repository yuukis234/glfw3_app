#pragma once
//=====================================================================//
/*!	@file
	@brief	漢字フォントイメージを freetype2 で扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <map>
#include <math.h>
#include <boost/unordered_map.hpp>
#include "gl_fw/Ikfimg.hpp"
#include "img_io/i_img.hpp"
#include "img_io/img_gray8.hpp"
#ifndef DEPEND_ESCAPE
#include <ft2build.h>
#include FT_FREETYPE_H
#endif

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	漢字フォントイメージ(Bitmap)クラス@n
				※FreeType2 ライブラリーを使う。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class kfimg_ft2 : public Ikfimg {

		FT_Library	library_;

		typedef std::pair<std::string, FT_Face>	face_pair;
		typedef boost::unordered_map<std::string, FT_Face>	face_map;
		typedef face_map::iterator			face_map_it;
		typedef face_map::const_iterator	face_map_cit;
		face_map	face_map_;
		std::string	alias_;
		FT_Face		face_;

		FT_Matrix	matrix_;

		font_metrics::metrics	metrics_;
		img_gray8	img_;

		std::map<int, int>	offset_y_;

		bool		antialias_;

		void create_font_bitmap_(img_gray8& img, const vtx::spos& size, wchar_t code, bool antialias);

		face_map_it find_face_(const std::string& name) { return face_map_.find(name); }
		bool install_face_(const std::string& name, const FT_Face face) {
			std::pair<face_map_it, bool> ret;
			ret = face_map_.insert(face_pair(name, face));
			return ret.second;
		}
		void erase_face_() { face_map_.clear(); }

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		kfimg_ft2() : face_(0), antialias_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		 */
		//-----------------------------------------------------------------//
		virtual ~kfimg_ft2() {
			for(face_map_it it = face_map_.begin(); it != face_map_.end(); ++it) {
				FT_Done_Face(it->second);
			}
			FT_Done_FreeType(library_);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		 */
		//-----------------------------------------------------------------//
		void initialize() {
			FT_Error error = FT_Init_FreeType(&library_);
			if(error) {
				// throw error handling...
			}

			double	angle = (0.0 / 360) * 2.0 * 3.14159265398979;
			matrix_.xx = static_cast<FT_Fixed>( cos( angle ) * 0x10000L );
			matrix_.xy = static_cast<FT_Fixed>(-sin( angle ) * 0x10000L );
			matrix_.yx = static_cast<FT_Fixed>( sin( angle ) * 0x10000L );
			matrix_.yy = static_cast<FT_Fixed>( cos( angle ) * 0x10000L );
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントをインストール
			@param[in]	fontfile	フォント・ファイル名
			@param[in]	alias		フォントの別名@n
						省略した場合、フォント・ファイル名と同一となる。
			@return 成功した場合は「true」
		 */
		//-----------------------------------------------------------------//
		bool install_font_type(const std::string& fontfile, const std::string& alias = "");


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを指定
			@param[in]	alias	フォント名
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool set_font(const std::string& alias) {
			face_map_it it = find_face_(alias);
			if(it != face_map_.end()) {
				alias_ = it->first;
				face_  = it->second;
				FT_Vector pen;
				pen.x = pen.y = 0;
				FT_Set_Transform(face_, &matrix_, &pen);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを取得
			@return フォントの別名を返す
		 */
		//-----------------------------------------------------------------//
		const std::string& get_font() const {
			static std::string tmp;
			if(face_ == 0) return tmp;
			return alias_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの有無を検査
			@param[in]	alias	フォント名
			@return フォントがインストール済みの場合は「true」
		 */
		//-----------------------------------------------------------------//
		bool find_font(const std::string& alias) const {
			face_map_cit cit = face_map_.find(alias);
			if(cit != face_map_.end()) {
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント（face）を破棄する
			@param[in]	alias	フォント名
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		bool delete_font(const std::string& alias) {
			face_map_it it = find_face_(alias);
			if(it != face_map_.end()) {
				face_map_.erase(it);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	生成するビットマップのアンチエリアス設定をする
			@param[in]	value	「true」（無し） の場合は、アンチエリアス有効、@n
								「false」 を指定すると、アンチエリアスし無効
		 */
		//-----------------------------------------------------------------//
		void set_antialias(bool value = true) { antialias_ = value; }


		//-----------------------------------------------------------------//
		/*!
			@brief	unicode に対応するビットマップを生成する。
			@param[in]	size	生成するビットマップのサイズ
			@param[in]	unicode	生成するビットマップの UNICODE
		 */
		//-----------------------------------------------------------------//
		void create_bitmap(const vtx::spos& size, wchar_t unicode);


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントのビットマップイメージを得る。
			@return	ビットマップイメージの参照
		 */
		//-----------------------------------------------------------------//
		const i_img* get_img() const { return dynamic_cast<const i_img*>(&img_); }


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの測定基準（メトリックス）を得る。
			@return	metrics 構造体
		 */
		//-----------------------------------------------------------------//
		const font_metrics::metrics& get_metrics() const { return metrics_; }

	};

}	// namespace img
