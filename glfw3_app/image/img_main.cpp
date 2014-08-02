//=====================================================================//
/*! @file
	@brief  img メイン関係
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include <tuple>
#include "img_main.hpp"
#include "core/glcore.hpp"
#include "widgets/widget_utils.hpp"
#include <boost/lexical_cast.hpp>

namespace app {

	typedef std::tuple<const std::string, const img::shared_img> save_t;
	bool save_task_(save_t t)
	{
		
		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  初期化
	*/
	//-----------------------------------------------------------------//
	void img_main::initialize()
	{
		gl::core& core = gl::core::get_instance();

		using namespace gui;
		widget_director& wd = director_.at().widget_director_;

		{ // 画像ファイル表示用フレーム
			widget::param wp(vtx::srect(30, 30, 256, 256));
			widget_frame::param wp_;
			wp_.plate_param_.set_caption(30);
			frame_ = wd.add_widget<widget_frame>(wp, wp_);
		}
		{ // 画像ファイル表示イメージ
			widget::param wp(vtx::srect(0, 0, 256, 256), frame_);
			widget_image::param wp_;
			image_ = wd.add_widget<widget_image>(wp, wp_);
			image_->set_state(widget::state::CLIP_PARENTS);
			image_->set_state(widget::state::RESIZE_ROOT);
			image_->set_state(widget::state::MOVE_ROOT, false);
		}

		{ // 機能ツールパレット
			widget::param wp(vtx::srect(10, 10, 130, 300));
			widget_frame::param wp_;
			tools_ = wd.add_widget<widget_frame>(wp, wp_);
			tools_->set_state(widget::state::SIZE_LOCK);
		}
		{ // ロード起動ボタン
			widget::param wp(vtx::srect(10, 10+50*0, 100, 40), tools_);
			widget_button::param wp_("load");
			load_ = wd.add_widget<widget_button>(wp, wp_);
		}
		{ // セーブ起動ボタン
			widget::param wp(vtx::srect(10, 10+50*1, 100, 40), tools_);
			widget_button::param wp_("save");
			save_ = wd.add_widget<widget_button>(wp, wp_);
		}
		short ofs = 110;
		{ // スケール FIT
			widget::param wp(vtx::srect(10, ofs+30*0, 90, 30), tools_);
			widget_radio::param wp_("fit");
			wp_.check_ = true;
			scale_fit_ = wd.add_widget<widget_radio>(wp, wp_);	
		}
		{ // スケール 1X
			widget::param wp(vtx::srect(10, ofs+30*1, 90, 30), tools_);
			widget_radio::param wp_("1x");
			scale_1x_ = wd.add_widget<widget_radio>(wp, wp_);	
		}
		{ // スケール 2X
			widget::param wp(vtx::srect(10, ofs+30*2, 90, 30), tools_);
			widget_radio::param wp_("2x");
			scale_2x_ = wd.add_widget<widget_radio>(wp, wp_);	
		}
		{ // スケール 3X
			widget::param wp(vtx::srect(10, ofs+30*3, 90, 30), tools_);
			widget_radio::param wp_("3x");
			scale_3x_ = wd.add_widget<widget_radio>(wp, wp_);	
		}
		{ // スケール 4X
			widget::param wp(vtx::srect(10, ofs+30*4, 90, 30), tools_);
			widget_radio::param wp_("4x");
			scale_4x_ = wd.add_widget<widget_radio>(wp, wp_);	
		}
		{ // ターミナル
			{
				widget::param wp(vtx::srect(10, 320, 9*14-8, 18*16+28));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(20);
				info_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::srect(0), info_);
				widget_terminal::param wp_;
				wp_.echo_ = false;
				term_ = wd.add_widget<widget_terminal>(wp, wp_);
			}
		}

		{ // load ファイラー本体
			widget::param wp(vtx::srect(10, 30, 300, 200));
			widget_filer::param wp_(core.get_current_path());
			load_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
			load_ctx_->enable(false);
		}
		{ // save ファイラー本体
			widget::param wp(vtx::srect(10, 30, 300, 200));
			widget_filer::param wp_(core.get_current_path());
			wp_.new_file_ = true;
			save_ctx_ = wd.add_widget<widget_filer>(wp, wp_);
			save_ctx_->enable(false);
		}
		{ // ダイアログ
			widget::param wp(vtx::srect(10, 30, 450, 200));
			widget_dialog::param wp_;
			dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
			dialog_->enable(false);
		}

		mobj_.initialize();

		// プリファレンスの取得
		sys::preference& pre = director_.at().preference_;
		if(load_ctx_) load_ctx_->load(pre);
		if(save_ctx_) save_ctx_->load(pre);
		if(frame_) frame_->load(pre);
		if(tools_) tools_->load(pre);
		if(scale_fit_) scale_fit_->load(pre);
		if(scale_1x_) scale_1x_->load(pre);
		if(scale_2x_) scale_2x_->load(pre);
		if(scale_3x_) scale_3x_->load(pre);
		if(scale_4x_) scale_4x_->load(pre);
		if(info_) info_->load(pre);
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  アップデート
	*/
	//-----------------------------------------------------------------//
	void img_main::update()
	{
		gl::core& core = gl::core::get_instance();

		gui::widget_director& wd = director_.at().widget_director_;

		if(load_) {
			if(load_->get_selected()) {
				if(load_ctx_) {
					bool f = load_ctx_->get_state(gui::widget::state::ENABLE);
					load_ctx_->enable(!f);
				}
			}
		}

		if(save_) {
			if(save_->get_selected()) {
				if(save_ctx_) {
					bool f = save_ctx_->get_state(gui::widget::state::ENABLE);
					save_ctx_->enable(!f);
				}
			}
		}

		std::string imfn;
		int id = core.get_recv_file_id();
		if(dd_id_ != id) {
			dd_id_ = id;
			const utils::strings& ss = core.get_recv_file_path();
			if(!ss.empty()) {
				imfn = ss.back();
			}
		}

		bool load_stall = false;
		bool save_stall = false;

		if(load_ctx_) {
			if(load_ctx_->get_state(gui::widget::state::ENABLE)) {
				save_stall = true;
				wd.top_widget(load_ctx_);
			}
			if(load_id_ != load_ctx_->get_select_file_id()) {
				load_id_ = load_ctx_->get_select_file_id();
				imfn = load_ctx_->get_file();
			}
		}

		if(!imfn.empty()) {
			img::img_files& imf = wd.at_img_files();
			if(!imf.load(imfn)) {
				dialog_->set_text("Can't decode image file:\n '"
					+ load_ctx_->get_file() + "'");
				dialog_->enable();
			} else {
				if(term_) {
					std::string s;
					s = "W: " + boost::lexical_cast<std::string>(imf.get_image()->get_size().x) + '\r';
					term_->output(s);
					s = "H: " + boost::lexical_cast<std::string>(imf.get_image()->get_size().y) + '\r';
					term_->output(s);
					img::IMG::type t = imf.get_image()->get_type();
					if(t == img::IMG::INDEXED8) {
						term_->output("INDEXED8\r");
					} else if(t == img::IMG::FULL8) {
						term_->output("FULL8\r");
					}
					if(imf.get_image()->test_alpha()) {
						term_->output("Alpha\r");
					}
					s = "C: " + boost::lexical_cast<std::string>(imf.get_image()->count_color()) + '\r';
					term_->output(s);
					term_->output('\r');
				}
				image_offset_.set(0.0f);
				frame_->at_local_param().text_param_.text_ = imfn;
				mobj_.destroy();
				mobj_.initialize();
				img_handle_ = mobj_.install(imf.get_image().get());
				image_->at_local_param().mobj_ = mobj_;
				image_->at_local_param().mobj_handle_ = img_handle_;

				
			}
		}

		// frame 内 image のサイズを設定
		if(frame_ && image_) {
			if(!image_->get_local_param().mobj_handle_) {
				save_stall = true;
			}

			float s = 1.0f;
			if(scale_fit_->get_check()) {
				vtx::fpos is = mobj_.get_size(img_handle_);
				vtx::fpos ss = image_->at_rect().size;
				vtx::fpos sc = ss / is;
				if(sc.x < sc.y) s = sc.x; else s = sc.y;
				image_->at_local_param().offset_ = 0.0f;
			} else {
				if(scale_1x_->get_check()) s = 1.0f;
 				else if(scale_2x_->get_check()) s = 2.0f;
 				else if(scale_3x_->get_check()) s = 3.0f;
 				else if(scale_4x_->get_check()) s = 4.0f;

				if(image_->get_select_in()) {
					image_offset_ = image_->get_local_param().offset_;
				}
				if(image_->get_select()) {
					vtx::spos d = image_->get_param().move_pos_ - image_->get_param().move_org_;
					image_->at_local_param().offset_ = image_offset_ + d / s;
				}
			}
			image_->at_local_param().scale_ = s;
		}

		if(save_ctx_) {
			if(save_ctx_->get_state(gui::widget::state::ENABLE)) {
				load_stall = true;
				wd.top_widget(save_ctx_);
			}
			if(save_id_ != save_ctx_->get_select_file_id()) {
				save_id_ = save_ctx_->get_select_file_id();
				save_file_name_ = save_ctx_->get_file();
			}
		}

		load_->set_state(gui::widget::state::STALL, load_stall);
		save_->set_state(gui::widget::state::STALL, save_stall);

		wd.update();

		// 画像ファイルのセーブタスク起動
		if(!save_file_name_.empty()) {
			if(image_saver_.valid()) {
				if(image_saver_.get()) {
					// turn on error dialog
				}
				save_file_name_.clear();
			} else {
				img::shared_img img;
				save_t t = std::make_tuple(save_file_name_, img);
				image_saver_ = std::async(std::launch::async, save_task_, t);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  レンダリング
	*/
	//-----------------------------------------------------------------//
	void img_main::render()
	{
		director_.at().widget_director_.service();
		director_.at().widget_director_.render();
	}


	//-----------------------------------------------------------------//
	/*!
		@brief  廃棄
	*/
	//-----------------------------------------------------------------//
	void img_main::destroy()
	{
		sys::preference& pre = director_.at().preference_;
		if(load_ctx_) load_ctx_->save(pre);
		if(save_ctx_) save_ctx_->save(pre);
		if(frame_) frame_->save(pre);
		if(tools_) tools_->save(pre);
		if(scale_fit_) scale_fit_->save(pre);
		if(scale_1x_) scale_1x_->save(pre);
		if(scale_2x_) scale_2x_->save(pre);
		if(scale_3x_) scale_3x_->save(pre);
		if(scale_4x_) scale_4x_->save(pre);
		if(info_) info_->save(pre);
	}
}
