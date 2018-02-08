#pragma once
//=====================================================================//
/*! @file
    @brief  波形キャプチャー・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "widgets/widget.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_null.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_filer.hpp"
#include "widgets/widget_terminal.hpp"
#include "widgets/widget_list.hpp"
#include "widgets/widget_view.hpp"
#include "widgets/widget_radio.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_utils.hpp"

#include "gl_fw/render_waves.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  wave_cap クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class wave_cap {

		utils::director<core>&	director_;

		typedef view::render_waves<uint16_t, 4096, 4> WAVES;
		WAVES					waves_;

		gui::widget_frame*		frame_;
		gui::widget_view*		core_;

		gui::widget_frame*		tools_;

		gui::widget_list*		time_div_;
		gui::widget_list*		trg_ch_;
		gui::widget_list*		trg_slope_;
		gui::widget_spinbox*	trg_window_;
		gui::widget_label*		trg_level_;
		gui::widget_list*		ch_gain_[4];

		struct mesure_t {
			gui::widget_check*		ena_;
			gui::widget_spinbox*   	org_;
			gui::widget_spinbox*   	len_;
			gui::widget_label*		in_;
			mesure_t() : ena_(nullptr), org_(nullptr), len_(nullptr), in_(nullptr)
			{ }

			void load(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->load(pre);
				}
			}

			void save(sys::preference& pre)
			{
				if(ena_ != nullptr) {
					ena_->save(pre);
				}
			}
		};
		mesure_t				time_;
		mesure_t				volt_;



		vtx::ipos				size_;

		static constexpr double div_tbls_[] = {
			1000e-3,
			500e-3,
			250e-3,
			100e-3,
			50e-3,
			10e-3,
			5e-3,
			1e-3,
			500e-6,
			100e-6,
			50e-6,
			25e-6,
			10e-6,
			5e-6,
			1e-6,
		};

		// 波形描画
		void update_view_()
		{
		}


		void render_view_(const vtx::irect& clip)
		{
//			gui::widget_director& wd = director_.at().widget_director_;
			glDisable(GL_TEXTURE_2D);
//			auto pos = div_->get_menu()->get_select_pos();
			waves_.render(clip.size, 65536);
			glEnable(GL_TEXTURE_2D);
			size_ = clip.size;
		}


		void service_view_()
		{
		}


		void init_mesure_(int loc, const std::string& text, mesure_t& t)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int y = loc * 50;
			{  // メジャー有効、無効
				widget::param wp(vtx::irect(10, 22 + 5 + y, 130, 40), tools_);
				widget_check::param wp_(text);
				t.ena_ = wd.add_widget<widget_check>(wp, wp_);
				t.ena_->at_local_param().select_func_ = [=](bool f) {
					waves_.at_info().time_enable_ = f;
				};
			}
			y += 50;
			{
				widget::param wp(vtx::irect(10, 22 + 5 + y, 130, 40), tools_);
				widget_spinbox::param wp_(0, 0, 100);
				t.org_ = wd.add_widget<widget_spinbox>(wp, wp_);
				t.org_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			y += 50;
			{
				widget::param wp(vtx::irect(10, 22 + 5 + y, 130, 40), tools_);
				widget_spinbox::param wp_(0, 0, 100);
				t.len_ = wd.add_widget<widget_spinbox>(wp, wp_);
				t.len_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
		}


		std::string limiti_(const std::string& str, int min, int max, const char* form)
		{
			std::string newtext;
			int v;
			if((utils::input("%d", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		wave_cap(utils::director<core>& d) : director_(d),
			waves_(), frame_(nullptr), core_(nullptr),
			tools_(nullptr),
			time_div_(nullptr), trg_ch_(nullptr), trg_slope_(nullptr),
			trg_window_(nullptr), trg_level_(nullptr), ch_gain_{ nullptr },

			time_(), volt_()
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{	// 波形フレーム
				widget::param wp(vtx::irect(40, 150, 400, 400));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				frame_ = wd.add_widget<widget_frame>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(0), frame_);
				widget_view::param wp_;
				wp_.update_func_ = [=]() {
					update_view_();
				};
				wp_.render_func_ = [=](const vtx::irect& clip) {
					render_view_(clip);
				};
				wp_.service_func_ = [=]() {
					service_view_();
				};
				core_ = wd.add_widget<widget_view>(wp, wp_);
			}

			int menu_width  = 230;
			int menu_height = 540;
			{	// 波形ツールフレーム
				widget::param wp(vtx::irect(100, 100, menu_width, menu_height));
				widget_frame::param wp_;
				wp_.plate_param_.set_caption(12);
				tools_ = wd.add_widget<widget_frame>(wp, wp_);
				tools_->set_state(gui::widget::state::SIZE_LOCK);
			}
			{  // 時間軸リスト 10K、20K、50K、100K、200K、500K、1M、2M、5M、10M、20M、50M、100M
				widget::param wp(vtx::irect(20, 22 + 10, 110, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100us");
				wp_.init_list_.push_back(" 50us");
				wp_.init_list_.push_back(" 20us");
				wp_.init_list_.push_back(" 10us");
				wp_.init_list_.push_back("  5us");
				wp_.init_list_.push_back("  2us");
				wp_.init_list_.push_back("  1us");
				wp_.init_list_.push_back("500ns");
				wp_.init_list_.push_back("200ns");
				wp_.init_list_.push_back("100ns");
				wp_.init_list_.push_back(" 50ns");
				wp_.init_list_.push_back(" 20ns");
				wp_.init_list_.push_back(" 10ns");
				time_div_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・チャネル選択
				widget::param wp(vtx::irect(20 + 120, 22 + 10, 80, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("---");
				wp_.init_list_.push_back("CH0");
				wp_.init_list_.push_back("CH1");
				wp_.init_list_.push_back("CH2");
				wp_.init_list_.push_back("CH3");
				trg_ch_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・スロープ選択
				widget::param wp(vtx::irect(20, 22 + 60, 80, 40), tools_);
				widget_list::param wp_;
				wp_.init_list_.push_back("Fall");
				wp_.init_list_.push_back("Rise");
				trg_slope_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // トリガー・ウィンドウ（１～１５）
				widget::param wp(vtx::irect(20 + 90, 22 + 60, 90, 40), tools_);
				widget_spinbox::param wp_(1, 1, 15);
				trg_window_ = wd.add_widget<widget_spinbox>(wp, wp_);
				trg_window_->at_local_param().select_func_
					= [=](widget_spinbox::state st, int before, int newpos) {
					return (boost::format("%d") % newpos).str();
				};
			}
			{  // トリガーレベル設定
				widget::param wp(vtx::irect(20, 22 + 110, 80, 40), tools_);
				widget_label::param wp_("1", false);
				trg_level_ = wd.add_widget<widget_label>(wp, wp_);
				trg_level_->at_local_param().select_func_ = [=](const std::string& str) {
					trg_level_->set_text(limiti_(str, 1, 65534, "%d"));
				};
			}
			for(int i = 0; i < 4; ++i) {
					widget::param wp(vtx::irect(20, 22 + 10, 110, 40), tools_);
					widget_list::param wp_;
					wp_.init_list_.push_back("100us");
					wp_.init_list_.push_back(" 50us");
					wp_.init_list_.push_back(" 20us");
					wp_.init_list_.push_back(" 10us");

			}


			init_mesure_(5, "Time", time_);
			init_mesure_(7, "Volt", volt_);


			sys::preference& pre = director_.at().preference_;
			if(frame_ != nullptr) {
				frame_->load(pre);
			}
			if(tools_ != nullptr) {
				tools_->load(pre);
			}

			// テスト波形生成
///			waves_.create_buffer(0.5, 10e-6);

//			waves_.at_param(0).gain_ = 0.025f;
//			waves_.at_param(1).gain_ = 0.025f;
//			waves_.at_param(0).color_ = img::rgba8(255, 128, 255, 255);
//			waves_.at_param(1).color_ = img::rgba8(128, 255, 255, 255);
//			waves_.at_param(0).offset_ = 0;
//			waves_.at_param(1).offset_ = 200;

			waves_.at_info().volt_org_ = 80;
			waves_.at_info().volt_len_ = 130;
///			waves_.build_sin(10e3);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(frame_ == nullptr) return;
			if(time_.org_ == nullptr) return;
			if(time_.len_ == nullptr) return;
			time_.org_->at_local_param().max_pos_ = size_.x;
			time_.len_->at_local_param().max_pos_ = size_.x;
			waves_.at_info().time_org_ = time_.org_->get_select_pos();
			waves_.at_info().time_len_ = time_.len_->get_select_pos();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
		*/
		//-----------------------------------------------------------------//
		void load()
		{
			sys::preference& pre = director_.at().preference_;

			if(frame_ != nullptr) {
				frame_->load(pre);
			}
			if(tools_ != nullptr) {
				tools_->load(pre);
			}
			time_.load(pre);
			volt_.load(pre);
#if 0
					waves_.at_info().time_enable_ = time_->get_check();
				}
				if(volt_ != nullptr) {
					volt_->load(pre);
					waves_.at_info().volt_enable_ = volt_->get_check();
#endif
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
		*/
		//-----------------------------------------------------------------//
		void save()
		{
			sys::preference& pre = director_.at().preference_;

			if(frame_ != nullptr) {
				frame_->save(pre);
			}
			if(tools_ != nullptr) {
				tools_->save(pre);
			}
			time_.save(pre);
			volt_.save(pre);
		}
	};
}


