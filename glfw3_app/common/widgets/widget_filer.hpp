#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ファイラー（ヘッダー）@n
			ファイル選択を行う GUI モジュール
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <vector>
#include <boost/unordered_map.hpp>
#include "utils/files.hpp"
#include "utils/drive_info.hpp"
#include "utils/preference.hpp"
#include "widget_director.hpp"
#include "widget.hpp"
#include "widget_null.hpp"
#include "widget_frame.hpp"
#include "widget_label.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI filer クラス（ファイル選択）
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class widget_filer {

		widget_director&	wd_;

		utils::files		fsc_;
		utils::drive_info	drv_;

		std::string	ext_filter_;

		std::string	path_text_;

		widget_frame*	base_;	///< ベース・フレーム
		widget_label*	path_;	///< パス・フレーム
		widget*			main_;	///< メイン・フレーム
		widget*			files_;	///< ファイル・フレーム

		struct widget_file {
			widget*			base;
			widget_label*	name;
			widget_label*	info;
			bool			dir;
			widget_file() : base(0), name(0), info(0), dir(false) { }
		};
		typedef std::vector<widget_file> widget_files;
		typedef std::vector<widget_file>::iterator widget_files_it;
		typedef std::vector<widget_file>::const_iterator widget_files_cit;
		widget_files	left_;
		widget_files	center_;
		widget_files	right_;

		vtx::fpos	speed_;
		vtx::fpos	position_;

		std::string	file_;

		struct file_t {
			vtx::spos	position_;
			file_t() : position_(0) { }
		};

		typedef boost::unordered_map<std::string, file_t>	file_map;
		typedef boost::unordered_map<std::string, file_t>::iterator	file_map_it;
		typedef std::pair<std::string, file_t> file_map_pair;
		file_map	file_map_;

		void create_files_(widget_files& wfs, short ofs);
		widget_files_cit scan_select_file_(widget_files& wfs);
		void resize_files_(widget_files& wfs, short width);
		void destroy_files_(widget_files& wfs);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_filer(widget_director& wd) : wd_(wd), fsc_(),
			base_(0), path_(0), main_(0), files_(0)
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~widget_filer() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	許可状態の取得
			@return 許可状態
		*/
		//-----------------------------------------------------------------//
		bool get_enable() const {
			if(base_) {
				return base_->get_state(widget::state::ENABLE);
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	許可
			@param[in]	f	「false」を指定すると不許可
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) {
			wd_.enable(base_, f, true);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	拡張子フィルターの設定
			@param[in]	ext	拡張子
		*/
		//-----------------------------------------------------------------//
		void set_filter(const std::string& ext) {
			ext_filter_ = ext;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リソースを作成
			@param[in]	rect	位置と大きさ
			@param[in]	path	ファイル・パス
		*/
		//-----------------------------------------------------------------//
		widget* create(const vtx::srect& rect, const std::string& path);


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
			@return ファイルが選択されたら「true」を返す
		*/
		//-----------------------------------------------------------------//
		bool update();


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイルを返す
			@return ファイル
		*/
		//-----------------------------------------------------------------//
		const std::string& get_file() const { return file_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル・リストを取得
			@param[out]	ss	ファイル・リスト
			@param[in]	dir	ディレクトリーを含める場合「true」
			@return ファイル・パス・インデックス（負の値ならマッチ無し）
		*/
		//-----------------------------------------------------------------//
		int get_file_list(utils::strings& ss, bool dir = false);


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル郡を取得
			@return ファイル郡
		*/
		//-----------------------------------------------------------------//
		const utils::files& get_files() const { return fsc_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre);


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(sys::preference& pre);


		//-----------------------------------------------------------------//
		/*!
			@brief	リソースの廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};

}
