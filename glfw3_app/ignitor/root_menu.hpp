#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・ルート・メニュー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "utils/preference.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "projector.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ルート・メニュー・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class root_menu {

		static const int ofs_x_ = 50;
		static const int ofs_y_ = 50;
		static const int btn_w_ = 240;
		static const int btn_h_ = 80;

		sys::preference&		pre_;
		gui::widget_director&	wd_;

		gui::widget_button*		new_project_;
		gui::widget_label*		proj_title_;

		gui::widget_button*		load_project_;
		gui::widget_label*		proj_path_;

		gui::widget_button*		edit_project_;
		gui::widget_button*		save_project_;
		gui::widget_button*		settings_;

		gui::widget_button*		run_;

		gui::widget_dialog*		proj_name_dialog_;
		gui::widget_label*		proj_name_label_;

		gui::widget_dialog*		proj_dialog_;



		gui::widget_dialog*		setting_dialog_;
		gui::widget_label*		setting_ip_[4];

		gui::widget_filer*		proj_load_filer_;
		gui::widget_filer*		proj_save_filer_;

		ign::projector			projector_;

		std::string				root_name_;

		std::string				ip_str_[4];
		int						ip_[4];


		void stall_button_(bool f)
		{
			new_project_->set_stall(f);
			proj_title_->set_stall(f);
			load_project_->set_stall(f);
			edit_project_->set_stall(f);
			save_project_->set_stall(f);
			settings_->set_stall(f);
			run_->set_stall(f);
		}


		void save_setting_value_()
		{
			for(int i = 0; i < 4; ++i) {
				ip_str_[i] = setting_ip_[i]->get_text();
			}
		}


		void load_setting_value_()
		{
			for(int i = 0; i < 4; ++i) {
				setting_ip_[i]->set_text(ip_str_[i]);
			}
		}


		void update_project_()
		{
			if(projector_.status()) {
				edit_project_->set_stall(false);
				save_project_->set_stall(false);
				run_->set_stall(false);
			} else {
				edit_project_->set_stall();
				save_project_->set_stall();
				run_->set_stall();
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		root_menu(sys::preference& pre, gui::widget_director& wd) : pre_(pre), wd_(wd),
			new_project_(nullptr),  proj_title_(nullptr),
			load_project_(nullptr), proj_path_(nullptr),
			edit_project_(nullptr),
			save_project_(nullptr),
			settings_(nullptr),
			run_(nullptr),
			proj_name_dialog_(nullptr), proj_name_label_(nullptr),
			proj_dialog_(nullptr),
			setting_dialog_(nullptr), setting_ip_{ nullptr },
			proj_load_filer_(nullptr), proj_save_filer_(nullptr),
			projector_(),
			ip_{ 0 }
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		~root_menu()
		{
			destroy();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			auto& core = gl::core::get_instance();
			const auto& scs = core.get_rect().size;

			using namespace gui;

			int sph = btn_h_ + 50;
			int scw = scs.x;
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 0, btn_w_, btn_h_));
				widget_button::param wp_("新規プロジェクト");
				new_project_ = wd_.add_widget<widget_button>(wp, wp_);
				new_project_->at_local_param().select_func_ = [this](bool f) {
					proj_name_dialog_->enable();
					root_name_ = proj_title_->get_text();
				};
				{
					widget::param wp(vtx::irect(ofs_x_ + btn_w_ + 50,
						ofs_y_ + sph * 0 + ((btn_h_ - 40) / 2), 200, 40));
					widget_label::param wp_("");
					proj_title_ = wd_.add_widget<widget_label>(wp, wp_);
				}
			}

			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 1, btn_w_, btn_h_));
				widget_button::param wp_("プロジェクト・ロード");
				load_project_ = wd_.add_widget<widget_button>(wp, wp_);
				load_project_->at_local_param().select_func_ = [this](bool f) {
					stall_button_(true);
					proj_load_filer_->enable();
				};
				{
					widget::param wp(vtx::irect(ofs_x_ + btn_w_ + 50,
						ofs_y_ + sph * 1 + ((btn_h_ - 40) / 2), 600, 40));
					widget_label::param wp_("");
					proj_path_ = wd_.add_widget<widget_label>(wp, wp_);
				}
			}
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 2, btn_w_, btn_h_));
				widget_button::param wp_("プロジェクト編集");
				edit_project_ = wd_.add_widget<widget_button>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 3, btn_w_, btn_h_));
				widget_button::param wp_("プロジェクト・セーブ");
				save_project_ = wd_.add_widget<widget_button>(wp, wp_);
				save_project_->at_local_param().select_func_ = [this](bool f) {
					stall_button_(true);
					proj_save_filer_->enable();
				};
			}
			{
				widget::param wp(vtx::irect(ofs_x_, ofs_y_ + sph * 4, btn_w_, btn_h_));
				widget_button::param wp_("コントローラー設定");
				settings_ = wd_.add_widget<widget_button>(wp, wp_);
				settings_->at_local_param().select_func_ = [this](bool f) {
					save_setting_value_();
					setting_dialog_->enable();
				};
			}
			{
				widget::param wp(vtx::irect(scw - btn_w_ - ofs_x_, ofs_y_ + sph * 0, btn_w_, btn_h_));
				widget_button::param wp_("検査開始");
				run_ = wd_.add_widget<widget_button>(wp, wp_);
				run_->at_local_param().select_func_ = [this](bool f) {
				};
			}

			{  // プロジェクト名入力ダイアログ
				int w = 300;
				int h = 200;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				proj_name_dialog_ = wd_.add_widget<widget_dialog>(wp, wp_);
				proj_name_dialog_->enable(false);
				proj_name_dialog_->at_local_param().select_func_ = [this](bool ok) {
					if(!ok) {
						proj_title_->set_text(root_name_);
						return;
					}
					projector_.start(proj_name_label_->get_text());
					proj_title_->set_text(proj_name_label_->get_text());
					update_project_();
				};
				{
					widget::param wp(vtx::irect(10, 20, w - 10 * 2, 40), proj_name_dialog_);
					widget_text::param wp_("プロジェクト名：");
					wd_.add_widget<widget_text>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10, 70, w - 10 * 2, 40), proj_name_dialog_);
					widget_label::param wp_("", false);
					proj_name_label_ = wd_.add_widget<widget_label>(wp, wp_);
				}
			}

			{  // 設定ダイアログ
				int w = 330;
				int h = 200;
				widget::param wp(vtx::irect(100, 100, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::CANCEL_OK;
				setting_dialog_ = wd_.add_widget<widget_dialog>(wp, wp_);
				setting_dialog_->enable(false);
				setting_dialog_->at_local_param().select_func_ = [this](bool ok) {
					if(!ok) {
						load_setting_value_();
						return;
					}
					for(int i = 0; i < 4; ++i) {
						const std::string& ip = setting_ip_[i]->get_text();
						int v = 0;
						if((utils::input("%d", ip.c_str()) % v).status()) {
							ip_[i] = v;
						}
					}
					utils::format("IP: %d, %d, %d, %d\n") % ip_[0] % ip_[1] % ip_[2] % ip_[3];
				};
				{
					widget::param wp(vtx::irect(10, 20, w - 10 * 2, 40), setting_dialog_);
					widget_text::param wp_("コントローラーＩＰ：");
					wd_.add_widget<widget_text>(wp, wp_);
				}
				int ipw = 60;  // IP 設定幅
				int ips = 20;  // IP 設定隙間
				{
					widget::param wp(vtx::irect(10 + (ipw + ips) * 0, 70, 60, 40), setting_dialog_);
					widget_label::param wp_("192", false);
					setting_ip_[0] = wd_.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10 + (ipw + ips) * 1, 70, 60, 40), setting_dialog_);
					widget_label::param wp_("168", false);
					setting_ip_[1] = wd_.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10 + (ipw + ips) * 2, 70, 60, 40), setting_dialog_);
					widget_label::param wp_("1", false);
					setting_ip_[2] = wd_.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(10 + (ipw + ips) * 3, 70, 60, 40), setting_dialog_);
					widget_label::param wp_("1", false);
					setting_ip_[3] = wd_.add_widget<widget_label>(wp, wp_);
				}
			}

			{  // プロジェクト・ファイラー
				gl::core& core = gl::core::get_instance();
				{
					widget::param wp(vtx::irect(30, 30, 500, 400));
					widget_filer::param wp_(core.get_current_path(), "", false);
					proj_load_filer_ = wd_.add_widget<widget_filer>(wp, wp_);
					proj_load_filer_->enable(false);
					proj_load_filer_->at_local_param().select_file_func_ = [this](const std::string& path) {
						if(projector_.load(path)) {
							proj_title_->set_text(projector_.get_title());
							proj_path_->set_text(path);
							update_project_();
						}
						stall_button_(false);
					};
					proj_load_filer_->at_local_param().cancel_file_func_ = [this](void) {
						stall_button_(false);
					};
				}
				{
					widget::param wp(vtx::irect(30, 30, 500, 400));
					widget_filer::param wp_(core.get_current_path(), "", true);
					proj_save_filer_ = wd_.add_widget<widget_filer>(wp, wp_);
					proj_save_filer_->enable(false);
					proj_save_filer_->at_local_param().select_file_func_ = [this](const std::string& path) {
						projector_.save(path);
						stall_button_(false);
					};
					proj_save_filer_->at_local_param().cancel_file_func_ = [this](void) {
						stall_button_(false);
					};
				}
			}

			// プリファレンスのロード
			setting_dialog_->load(pre_);
			setting_ip_[0]->load(pre_);
			setting_ip_[1]->load(pre_);
			setting_ip_[2]->load(pre_);
			setting_ip_[3]->load(pre_);

			proj_name_dialog_->load(pre_);

			proj_load_filer_->load(pre_);
			proj_save_filer_->load(pre_);

			update_project_();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			auto& core = gl::core::get_instance();
			const auto& scs = core.get_rect().size;
			run_->at_rect().org.x = scs.x - btn_w_ - ofs_x_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy()
		{
			setting_dialog_->save(pre_);
			setting_ip_[0]->save(pre_);
			setting_ip_[1]->save(pre_);
			setting_ip_[2]->save(pre_);
			setting_ip_[3]->save(pre_);

			proj_name_dialog_->save(pre_);

			proj_load_filer_->save(pre_);
			proj_save_filer_->save(pre_);

			wd_.del_widget(proj_save_filer_);
			proj_save_filer_ = nullptr;
			wd_.del_widget(proj_load_filer_);
			proj_load_filer_ = nullptr;

			wd_.del_widget(setting_dialog_);
			setting_dialog_ = nullptr;
			wd_.del_widget(proj_name_dialog_);
			proj_name_dialog_ = nullptr;

			wd_.del_widget(settings_);
			settings_ = nullptr;
			wd_.del_widget(save_project_);
			save_project_ = nullptr;
			wd_.del_widget(edit_project_);
			edit_project_ = nullptr;

			wd_.del_widget(load_project_);
			load_project_ = nullptr;
			wd_.del_widget(proj_path_);
			proj_path_ = nullptr;

			wd_.del_widget(proj_title_);
			proj_title_ = nullptr;
			wd_.del_widget(new_project_);
			new_project_ = nullptr;
		}
	};
}

