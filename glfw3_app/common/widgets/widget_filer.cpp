//=====================================================================//
/*!	@file
	@brief	GUI Widget ファイラー
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_utils.hpp"
#include "widgets/widget_filer.hpp"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

namespace gui {

	static const char* key_path_   = { "widget/filer/current_path" };
	static const char* key_locate_ = { "widget/filer/locate" };
	static const char* key_size_   = { "widget/filer/size" };

	void widget_filer::create_files_(widget_files& wfs, short ofs)
	{
		// ラベルを新規に作成
		vtx::srect rect;
		rect.org.set(ofs, 0);
		rect.size.x = path_->get_rect().size.x;
		rect.size.y = 32;

		const utils::file_infos& fos = fsc_.get();

		BOOST_FOREACH(const utils::file_info& fi, fos) {
			std::string fn = fi.get_name();
			if(fn == ".") continue;

			widget_file wf;
			if(fn == "..") ;
			else if(fi.is_directory()) {
				fn += '/';
				wf.dir = true;
			}
			{
				widget::param wp(rect, files_);
				wp.state_.reset(widget::state::RENDER_ENABLE);
				widget_null::param wp_;
				wf.base = wd_.add_widget<widget_null>(wp, wp_);
				wf.base->set_state(widget::state::POSITION_LOCK);
				wf.base->set_state(widget::state::SIZE_LOCK);
				wf.base->set_state(widget::state::CLIP_PARENTS);
				wf.base->set_state(widget::state::DRAG_UNSELECT);
				wf.base->set_state(widget::state::SELECT_PARENTS);
			}
			short fns = rect.size.x * 2 / 3;
			short ats = rect.size.x - fns - 2;
			{
				vtx::srect r;
				r.org.set(0);
				r.size.set(fns, label_height_);
				widget::param wp(r, wf.base);
				wp.action_.set(widget::action::SELECT_HIGHLIGHT);
				widget_label::param wp_(fn);
				wp_.text_param_.placement_.hpt = vtx::placement::holizontal::LEFT;
				wp_.plate_param_.frame_width_ = 0;
				wp_.plate_param_.round_radius_ = 0;
				wp_.plate_param_.resizeble_ = true;
				wf.name = wd_.add_widget<widget_label>(wp, wp_);
				wf.name->set_state(widget::state::POSITION_LOCK);
				wf.name->set_state(widget::state::CLIP_PARENTS);
				wf.name->set_state(widget::state::DRAG_UNSELECT);
			}
			{
				vtx::srect r;
				r.org.set(fns + 2, 0);
				r.size.set(ats, label_height_);
				widget::param wp(r, wf.base);
				wp.action_.set(widget::action::SELECT_HIGHLIGHT);
				widget_label::param wp_;
				wp_.text_param_.placement_.hpt = vtx::placement::holizontal::LEFT;
				wp_.plate_param_.frame_width_ = 0;
				wp_.plate_param_.round_radius_ = 0;
				wp_.plate_param_.resizeble_ = true;
				wf.info = wd_.add_widget<widget_label>(wp, wp_);
				wf.info->set_state(widget::state::POSITION_LOCK);
				wf.info->set_state(widget::state::CLIP_PARENTS);
				wf.info->set_state(widget::state::DRAG_UNSELECT);
			}
			wf.size = fi.get_size();
			wf.time = fi.get_time();
			wf.mode = fi.get_mode();
			wfs.push_back(wf);
			rect.org.y += label_height_;
		}
		files_->at_rect().size.y = rect.org.y;

		file_map_it it = file_map_.find(param_.path_);
		if(it != file_map_.end()) {
			position_ = it->second.position_;
			select_pos_ = it->second.select_pos_;
			files_->at_rect().org = it->second.position_;
		}

		// ウィジェットの強制的な優先順位設定
		wd_.top_widget(this);
		wd_.top_widget(path_);
		wd_.top_widget(info_);
		wd_.top_widget(main_);
		wd_.top_widget(files_);
	}


	widget_filer::widget_files_cit widget_filer::scan_select_file_(widget_files& wfs)
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(wf.base->get_select()) {
				return cit;
			}
		}
		return wfs.end();
	}


	widget_filer::widget_files_cit widget_filer::scan_selected_file_(widget_files& wfs)
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(wf.base->get_selected()) {
				return cit;
			}
		}
		return wfs.end();
	}


	void widget_filer::resize_files_(widget_files& wfs, short width)
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			wf.base->at_rect().size.x = width;
			short name_size = width * 2 / 3;
			short space = 2;
			short info_size = width - name_size - space;
			short info_limit = 130;
			if(info_state_ == info_state::TIME) {
				info_limit = 200;
			}
			if(info_size >= info_limit) {
				info_size = info_limit;
				name_size = width - space - info_limit;
			}
			if(info_state_ == info_state::NONE) {
				name_size = width;
				info_size = 0;
			}
			wf.name->at_rect().size.x = name_size;
			wf.info->at_rect().org.x  = name_size + space;
			wf.info->at_rect().size.x = info_size;
		}
	}


	void widget_filer::update_files_info_(widget_files& wfs)
	{
		for(widget_files_cit cit = wfs.begin(); cit != wfs.end(); ++cit) {
			const widget_file& wf = *cit;
			if(info_state_ == info_state::SIZE) {
				wf.info->at_local_param().text_param_.text_
					= ' ' + boost::lexical_cast<std::string>(wf.size);
			} else if(info_state_ == info_state::TIME) {
				std::string s;
				s += ' ';
				struct tm* t = localtime(&wf.time);
				s += (boost::format("%02d:%02d ") % t->tm_hour % t->tm_min).str();
				s += (boost::format("%d/%d ")
					% (t->tm_mon + 1) % t->tm_mday).str();
				s += (boost::format("%4d") % (t->tm_year + 1900)).str();
				wf.info->at_local_param().text_param_.text_ = s;
			} else if(info_state_ == info_state::MODE) {
				std::string s;
				s += ' ';
				uint32_t bit = 1 << 8;
				static const char chmod[9]
					= { 'r', 'w', 'x', 'r', 'w', 'x', 'r', 'w', 'x' };
				for(int i = 0; i < 9; ++i) {
					if(wf.mode & bit) {
						s += chmod[i];
					} else {
						s += '-';
					}
					bit >>= 1;
				}
				wf.info->at_local_param().text_param_.text_ = s;
			}
		}
	}


	void widget_filer::destroy_files_(widget_files& wfs)
	{
		// ラベル郡を破棄
		BOOST_FOREACH(const widget_file& wf, wfs) {
			wd_.del_widget(wf.info);
			wd_.del_widget(wf.name);
			wd_.del_widget(wf.base);
		}
		wfs.clear();
		speed_.set(0);
		position_.set(0);
	}


	void widget_filer::destroy_()
	{
		destroy_files_(center_);
		wd_.del_widget(files_);
		wd_.del_widget(main_);
		wd_.del_widget(path_);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル・リストを取得
		@param[out]	ss	ファイル・リスト
		@param[in]	dir	ディレクトリーを含める場合「true」
		@return ファイル・パス・インデックス（負の値ならマッチ無し）
	*/
	//-----------------------------------------------------------------//
	int widget_filer::get_file_list(utils::strings& ss, bool dir)
	{
		uint32_t n = 0;
		int ret = -1;
		BOOST_FOREACH(const utils::file_info& fi, fsc_.get()) {
			std::string fn;
			if(fi.get_name() == ".") continue;
			else if(fi.get_name() == "..") continue;
			utils::append_path(param_.path_, fi.get_name(), fn);
			if(fn == file_) ret = n;
			if(dir && fi.is_directory()) {
				fn += "/";
				ss.push_back(fn);
				++n;
			} else {
				ss.push_back(fn);
				++n;
			}
		}
		return ret;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	*/
	//-----------------------------------------------------------------//
	void widget_filer::initialize()
	{
		// 自由な大きさの変更
		at_param().state_.set(widget::state::SIZE_LOCK, false);
		at_param().state_.set(widget::state::RESIZE_H_ENABLE);
		at_param().state_.set(widget::state::RESIZE_V_ENABLE);

		param_.plate_param_.resizeble_ = true;
		// フレームの生成
		objh_ = frame_init(wd_, at_param(), param_.plate_param_, param_.color_param_);

		fsc_.set_path(param_.path_, param_.filter_);

		short frame_width = param_.plate_param_.frame_width_;
		// path（ハンドル）
		{
			vtx::srect r;
			r.org.set(frame_width);
			r.size.x = get_rect().size.x - frame_width * 2;
			r.size.y = path_height_;
			widget::param wp(r, this);
			wp.action_.set(widget::action::SELECT_HIGHLIGHT);
			widget_label::param wp_(param_.path_);
			wp_.text_param_.placement_.hpt = vtx::placement::holizontal::LEFT;
			wp_.color_param_.fore_color_.set(236, 181, 63);
			wp_.color_param_.back_color_.set(131, 104, 45);
			wp_.color_param_select_.fore_color_ = wp_.color_param_.fore_color_;
			wp_.color_param_select_.back_color_ = wp_.color_param_.back_color_;
			wp_.color_param_select_.fore_color_.gainY(1.3f);
			wp_.color_param_select_.back_color_.gainY(1.3f);
			wp_.plate_param_.resizeble_ = true;
			wp_.plate_param_.frame_width_  = 2;
			wp_.plate_param_.round_radius_ = 4;
			path_ = wd_.add_widget<widget_label>(wp, wp_);
			path_->set_state(widget::state::POSITION_LOCK);
		}

		// info ボタン
		{
			gl::glmobj::handle hnd = wd_.get_share_image().right_box_;
			vtx::srect r;
			r.size = wd_.at_mobj().get_size(hnd);
			short space = 4;
			r.org.x = get_rect().size.x - r.size.x - frame_width - space;
			r.org.y = frame_width + (path_height_ - r.size.y) / 2;
			widget::param wp(r, this);
			widget_button::param wp_;
			wp_.handle_ = hnd;
			info_ = wd_.add_widget<widget_button>(wp, wp_);
		}

		// main null frame
		{
			vtx::srect r(vtx::spos(frame_width, frame_width + path_height_),
				vtx::spos(get_rect().size.x - 8, get_rect().size.y - 32 - 2 * 4 - 4));
			widget::param wp(r, this);
			wp.state_.reset(widget::state::RENDER_ENABLE);
			widget_null::param wp_;
			main_ = wd_.add_widget<widget_null>(wp, wp_);
			main_->set_state(widget::state::POSITION_LOCK);
			main_->set_state(widget::state::SIZE_LOCK);
		}

		// files null frame
		{
			vtx::srect r(vtx::spos(0), get_rect().size);
			widget::param wp(r, main_);
			wp.state_.reset(widget::state::RENDER_ENABLE);
			widget_null::param wp_;
			files_ = wd_.add_widget<widget_null>(wp, wp_);
			files_->set_state(widget::state::CLIP_PARENTS);
			files_->set_state(widget::state::SIZE_LOCK);
		}

		position_ = files_->get_rect().org;
		speed_.set(0.0f, 0.0f);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	アップデート
	*/
	//-----------------------------------------------------------------//
	void widget_filer::update()
	{
		// 情報ボタンが押された場合の処理
		if(info_->get_selected()) {
			info_state_ = static_cast<info_state::type>(info_state_ + 1);
			if(info_state_ == info_state::limit_) {
				info_state_ = info_state::NONE;
			}
			update_files_info_(center_);
		}

		// フレームのサイズを、仮想ウィジェットに反映
		{
			short fw = param_.plate_param_.frame_width_;
			const vtx::spos& size = get_rect().size;
			path_->at_rect().size.x = size.x - fw * 2 - path_height_;
			main_->at_rect().size.x = size.x - fw * 2;
			main_->at_rect().size.y = size.y - path_->get_rect().size.y - fw * 2;
			files_->at_rect().size.x = size.x - fw * 2;
			short space = 4;
			info_->at_rect().org.x = size.x - info_->get_rect().size.x - fw - space;
			resize_files_(center_, size.x - fw * 2);
		}

		short base_size = main_->get_rect().size.y;
		short d = base_size - files_->get_rect().size.y;
		short scroll_gain = 12;
		float gain = 0.85f;
		float slip_gain = 0.5f;
		float speed_gain = 0.95f;

		// スプリング効果
//		if(files_->get_select()) {
		if(get_select()) {
			position_ = files_->get_rect().org;
			if(d < 0) {
				if(position_.y < d) {
					position_.y -= d;
					position_.y *= slip_gain;
					position_.y += d;
				} else if(position_.y > 0) {
					position_.y *= slip_gain;
				}
			} else {
				position_.y *= slip_gain;
			}
		} else {
///			if(files_->get_select_out()) {
			if(get_select_out()) {
				speed_ = files_->get_param().speed_;
			}
			position_ += speed_;
			speed_ *= speed_gain;
			position_.x *= gain;
//			if(-0.5f < position_.x && position_.x < 0.5f) {
//				position_.x = 0.0f;
//				speed_.x = 0;
//			}

			// y 軸クリップ
			{
				short pos = files_->get_rect().size.y;
				if(d < 0) {
					if(position_.y < d) {
						position_.y -= d;
						position_.y *= gain;
						position_.y += d;
						speed_.y = 0.0f;
						if(position_.y > (d - 0.5f)) {
							position_.y = d;
						}
					} else if(position_.y > 0.0f) {
						position_.y *= gain;
						speed_.y = 0.0f;
						if(position_.y < 0.5f) {
							position_.y = 0.0f;
						}
					} else {
						const vtx::spos& scr = wd_.get_scroll();
						if(files_->get_focus() && scr.y != 0) {
							position_.y += scr.y * scroll_gain;
							if(position_.y < d) {
								position_.y = d;
							} else if(position_.y > 0.0f) {
								position_.y = 0.0f;
							}
						}
					}
				} else {
					position_.y *= gain;
					if(-0.5f < position_.y && position_.y < 0.5f) {
						position_.y = 0.0f;
						speed_.y = 0.0f;
					}
				}
			}
		}
		files_->at_rect().org = position_;

		// パスに紐づいた位置の記録
		{
			file_map_it it = file_map_.find(param_.path_);
			if(it == file_map_.end()) {
				file_t t;
				it = file_map_.insert(file_map_pair(param_.path_, t)).first;
			}
			file_t& f = it->second;
			f.position_ = position_;
			f.select_pos_ = select_pos_;
		}

		// path 文字列を設定
		{
			if(files_->get_rect().org.x > (get_rect().size.x / 2)) {
				std::string np;
				if(utils::previous_path(param_.path_, np)) {
					path_->at_local_param().text_param_.text_ = np;
				}
			} else {
				path_->at_local_param().text_param_.text_ = param_.path_;
			}
		}

		// 選択されたファイルをハイライトする位置の検出
		if(files_->get_select()) {
			widget_files_cit cit = scan_select_file_(center_);
			if(cit != center_.end()) {
				select_pos_ = cit - center_.begin();
			}
		} else if(!center_.empty() && select_pos_ < center_.size()) {
			widget_file& wf = center_[select_pos_];
			wf.name->set_action(widget::action::SELECT_HIGHLIGHT);
			wf.name->set_state(widget::state::IS_SELECT);
			wf.info->set_action(widget::action::SELECT_HIGHLIGHT);
			wf.info->set_state(widget::state::IS_SELECT);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void widget_filer::render()
	{
		if(objh_ == 0) return;

		wd_.at_mobj().resize(objh_, get_param().rect_.size);
		glEnable(GL_TEXTURE_2D);
		wd_.at_mobj().draw(objh_, gl::glmobj::normal, 0, 0);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void widget_filer::service()
	{
		if(!get_state(widget::state::ENABLE)) return;

		// ファイル情報の取得と反映（ファイル情報収集はスレッドで動作）
		if(fsc_.probe()) {
			if(center_.empty()) {
				create_files_(center_, 0);
				update_files_info_(center_);

				std::string s;
				if(utils::previous_path(param_.path_, s)) {
//					fsc_.set_path(s);
				}
			} else if(left_.empty()) {
//				create_files_(left_, -files_->get_rect().size.x);
			}
		}

		// 選択の確認と動作
		bool selected = false;
///		if(files_->get_select_out()) {
		if(files_->get_selected()) {
			widget_files_cit cit = scan_selected_file_(center_);
			if(cit != center_.end()) {
				const widget_file& wf = *cit;
				if(wf.name) {
					const std::string& n = wf.name->get_local_param().text_param_.text_;
					if(n == "..") {
						std::string np;
						utils::previous_path(param_.path_, np);
						param_.path_ = np;
						fsc_.set_path(param_.path_, param_.filter_);
						destroy_files_(center_);
					} else if(wf.dir) {
						std::string np;
						utils::append_path(param_.path_, n, np);
						utils::strip_last_of_delimita_path(np, param_.path_);
						fsc_.set_path(param_.path_, param_.filter_);
						destroy_files_(center_);
					} else {
						utils::append_path(param_.path_, n, file_);
						selected = true;
					}
				}
			} else {
				if(files_->get_rect().org.x > (get_rect().size.x / 2)) {
					std::string np;
					utils::previous_path(param_.path_, np);
					param_.path_ = np;
					fsc_.set_path(param_.path_);
					destroy_files_(center_);
				}
			}
		}
		if(selected) {
			enable(false);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルを選択
		@param[in]	path	選択するファイルパス
		@return 該当するファイルが無い場合「false」
	*/
	//-----------------------------------------------------------------//
	bool widget_filer::select_file(const std::string& path)
	{
#if 0
		file_map_it it = file_map_.find(path);
		if(it != file_map_.end()) {
			select_pos_ = it->second.select_pos_;
		}
#endif
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のセーブ
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_filer::save(sys::preference& pre)
	{
		int err = 0;
		if(!pre.put_text(key_path_, param_.path_)) ++err;
		if(!pre.put_position(key_locate_, vtx::ipos(get_rect().org))) ++err;
		if(!pre.put_position(key_size_, vtx::ipos(get_rect().size))) ++err;
///		if(!pre.put_integer(key_info_, info_state_) ++err;
		return err == 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	状態のロード
		@param[in]	pre	プリファレンス参照
		@return エラーが無い場合「true」
	*/
	//-----------------------------------------------------------------//
	bool widget_filer::load(sys::preference& pre)
	{
		int err = 0;
		std::string path;
		if(pre.get_text(key_path_, path)) {
			param_.path_ = path;
			fsc_.set_path(param_.path_, param_.filter_);
			destroy_files_(center_);
		} else {
			++err;
		}

		vtx::ipos p;
		if(pre.get_position(key_locate_, p)) {
			at_rect().org = p;
		} else {
			++err;
		}
		if(pre.get_position(key_size_, p)) {
			at_rect().size = p;
		} else {
			++err;
		}
#if 0
		int i;
		if(pre.get_integer(key_info_, i)) {
			info_state_ = static_cast<info_state::type>(i);
		} else {
			++err;
		}
#endif
		return err == 0;
	}
}
