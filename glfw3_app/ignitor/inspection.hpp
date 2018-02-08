#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター単体検査クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "utils/select_file.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

#include "relay_map.hpp"
#include "ign_client.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  単体検査クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class inspection {

		static constexpr const char* UNIT_EXT_ = "unt";  ///< 単体検査ファイル、拡張子

		utils::director<core>&	director_;

		net::ign_client&		client_;

		gui::widget_dialog*		dialog_;
		gui::widget_label*		unit_name_;				///< 単体試験名
		gui::widget_button*		load_file_;				///< load file
		gui::widget_button*		save_file_;				///< save file
		utils::select_file		unit_load_filer_;
		utils::select_file		unit_save_filer_;

//		gui::widget_list*		inspection_standards_;	///< 検査規格 (Inspection standards)

		// DC1 設定
		gui::widget_check*		dc1_sw_[5];		///< DC1 接続スイッチ
		gui::widget_check*		dc1_ena_;		///< DC1 有効、無効
		gui::widget_list*		dc1_mode_;		///< DC1 電流、電圧モード
		gui::widget_label*		dc1_voltage_;	///< DC1（電圧）
		gui::widget_label*		dc1_current_;	///< DC1（電流）
		gui::widget_button*		dc1_exec_;		///< DC1 設定転送

		// DC2 設定
		gui::widget_check*		dc2_sw_[14];	///< DC2 接続スイッチ
		gui::widget_check*		dc2_ena_;		///< DC2 有効、無効
		gui::widget_list*		dc2_mode_;		///< DC2 電流、電圧モード
		gui::widget_label*		dc2_voltage_;	///< DC2（電圧）
		gui::widget_label*		dc2_current_;	///< DC2（電流）
		gui::widget_button*		dc2_exec_;		///< DC2 設定転送

		// WGM 設定
		gui::widget_check*		gen_sw_[5];	///< ジェネレータ接続スイッチ
		gui::widget_check*		gen_ena_;	///< ジェネレータ有効、無効
		gui::widget_list*		gen_mode_;	///< ジェネレータモード（矩形波、三角波、直流）
		gui::widget_label*		gen_freq_;	///< ジェネレータ設定・周波数（1Hz to 100Hz / 1Hz)
		gui::widget_label*		gen_volt_;	///< ジェネレータ設定・電圧（0 to 14V / 0.1V）
		gui::widget_label*		gen_duty_;	///< ジェネレーター設定・Duty（0.1% to 100% / 0.1%）
		gui::widget_button*		gen_exec_;	///< ジェネレーター設定転送

		// CRM 設定
		gui::widget_check*		crm_sw_[14];	///< CRM 接続スイッチ
		gui::widget_check*		crm_ena_;		///< CRM 有効、無効
		gui::widget_list*		crm_freq_;		///< CRM 周波数（100Hz, 1KHz, 10KHz）
		gui::widget_list*		crm_mode_;		///< CRM 抵抗測定、容量測定
		gui::widget_label*		crm_ans_;		///< CRM 測定結果 
		gui::widget_text*		crm_anst_;		///< CRM 測定結果単位 
		gui::widget_button*		crm_exec_;		///< CRM 設定転送

		// ICM 設定
		gui::widget_check*		icm_sw_[6];		///< ICM 接続スイッチ
		gui::widget_button*		icm_exec_;		///< ICM 設定転送

		gui::widget_label*		wait_time_;		///< Wait時間設定

		gui::widget_text*		help_;			///< HELP


		struct dc1_t {
			uint16_t	sw;		///< 5 bits
			bool		ena;	///< 0, 1
			bool		mode;	///< 0, 1
			uint32_t	volt;	///< 20 bits
			uint32_t	curr;	///< 20 bits

			dc1_t() : sw(0), ena(0), mode(0), volt(0), curr(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("dc1 D1SW%04X\n") % sw).str();
				s += (boost::format("dc1 D1MD%d\n") % mode).str();
				s += (boost::format("dc1 D1VS%05X\n") % (volt & 0xfffff)).str();
				s += (boost::format("dc1 D1IS%05X\n") % (curr & 0xfffff)).str();
				s += (boost::format("dc1 D1OE%d\n") % ena).str();
				return s;
			}
		};


		struct dc2_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			bool		mode;	///< 0, 1
			uint32_t	volt;	///< 20 bits
			uint32_t	curr;	///< 20 bits

			dc2_t() : sw(0), ena(0), mode(0), volt(0), curr(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("dc2 D2SW%04X\n") % sw).str();
				s += (boost::format("dc2 D2MD%d\n") % mode).str();
				s += (boost::format("dc2 D2VS%05X\n") % (volt & 0xfffff)).str();
				s += (boost::format("dc2 D2IS%05X\n") % (curr & 0xfffff)).str();
				s += (boost::format("dc2 D2OE%d\n") % ena).str();
				return s;
			}
		};


		struct wgm_t {
			uint16_t	sw;		///< 5 bits
			bool		ena;	///< 0, 1
			bool		type;	///< 0, 1
			uint16_t   	frq;	///< 7 bits
			uint16_t	duty;	///< 10 bits
			uint16_t	volt;	///< 10 bits

			wgm_t() : ena(0), type(0), frq(0), duty(0), volt(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("wgm WGSW%02X\n") % sw).str();
				s += (boost::format("wgm WGSP%d\n") % type).str();
				s += (boost::format("wgm WGFQ%02X\n") % (frq & 0x7f)).str();
				s += (boost::format("wgm WGPW%03X\n") % (duty & 0x3ff)).str();
				s += (boost::format("wgm WGPV%03X\n") % (volt & 0x3ff)).str();
				s += (boost::format("wgm WGOE%d\n") % ena).str();
				return s;
			}
		};


		struct crm_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			uint16_t	freq;	///< 0, 1, 2
			uint16_t	mode;	///< 0, 1

			crm_t() : sw(0), ena(0), freq(0), mode(0) { }

			std::string build() const
			{
				std::string s;
				s += (boost::format("crm CRSW%04X\n") % sw).str();
				static const char* frqtbl[3] = { "001", "010", "100" };
				s += (boost::format("crm CRFQ%s\n") % frqtbl[freq % 3]).str();
				s += (boost::format("crm CROE%d\n") % ena).str();
				if(mode) {
					s += (boost::format("crm CRC?1\n")).str();
				} else {
					s += (boost::format("crm CRR?1\n")).str();
				}
				return s;
			}
		};


		struct icm_t {
			uint16_t	sw;		///< 14 bits

			icm_t() : sw(0) { }

			std::string build() const
			{
				std::string s;
				s = (boost::format("icm CSW%02X\n") % sw).str();
				return s;
			}
		};


		struct vc_t {
			float		volt_max_;	/// 0.1V step
			float		volt_;		/// 0.1V step
			float		curt_max_;	/// 0.1A/0.01mA step
			float		curt_;		/// 0.1A/0.01mA step
		};

		std::string limitf_(const std::string& str, float min, float max, const char* form)
		{
			std::string newtext;
			float v;
			if((utils::input("%f", str.c_str()) % v).status()) {
				if(v < min) v = min;
				else if(v > max) v = max;
				char tmp[256];
				utils::format(form, tmp, sizeof(tmp)) % v;
				newtext = tmp;
			}
			return newtext;
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


		void init_sw_(int ofsx, int h, int loc, gui::widget_check* out[], int num, int swn)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;			
			for(int i = 0; i < num; ++i) {
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 60, 40), dialog_);
				widget_check::param wp_((boost::format("%d") % swn).str());
				++swn;
				out[i] = wd.add_widget<widget_check>(wp, wp_);
				ofsx += 60;
			}
		}


		void init_dc1_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, dc1_sw_, 5, 43);
			++loc;
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				dc1_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("定電圧");
				wp_.init_list_.push_back("定電流");
				dc1_mode_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 60V/0.1V, 30A/10mA
				widget::param wp(vtx::irect(ofsx + 230, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc1_voltage_ = wd.add_widget<widget_label>(wp, wp_);
				dc1_voltage_->at_local_param().select_func_ = [=](const std::string& str) {
					dc1_voltage_->set_text(limitf_(str, 0.0f, 60.0f, "%3.1f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 330, 20 + h * loc, 40, 40), dialog_);
				widget_text::param wp_("V");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // Max: 30A / step: 10mA
				widget::param wp(vtx::irect(ofsx + 370, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc1_current_ = wd.add_widget<widget_label>(wp, wp_);
				dc1_current_->at_local_param().select_func_ = [=](const std::string& str) {
					dc1_current_->set_text(limitf_(str, 0.0f, 30.0f, "%4.2f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 470, 20 + h * loc, 50, 40), dialog_);
				widget_text::param wp_("A");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				dc1_exec_ = wd.add_widget<widget_button>(wp, wp_);
				dc1_exec_->at_local_param().select_func_ = [=](int n) {
					dc1_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 5; ++i) {
						sw <<= 1;
						if(dc1_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = dc1_ena_->get_check();
					t.mode = dc1_mode_->get_select_pos() & 1;
					float v;
					if((utils::input("%f", dc1_voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 62.5e-6;
					}
					if((utils::input("%f", dc1_current_->get_text().c_str()) % v).status()) {
						t.curr = v / 31.25e-6;
					}
					client_.send(t.build());
				};
			}
		}


		void init_dc2_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, dc2_sw_, 14, 15);
			++loc;
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				dc2_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("定電圧");
				wp_.init_list_.push_back("定電流");
				dc2_mode_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 300V/0.1V, 100mA/0.01mA
				widget::param wp(vtx::irect(ofsx + 230, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc2_voltage_ = wd.add_widget<widget_label>(wp, wp_);
				dc2_voltage_->at_local_param().select_func_ = [=](const std::string& str) {
					dc2_voltage_->set_text(limitf_(str, 0.0f, 300.0f, "%2.1f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 330, 20 + h * loc, 40, 40), dialog_);
				widget_text::param wp_("V");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{  // Max: 100mA / step: 0.01mA
				widget::param wp(vtx::irect(ofsx + 370, 20 + h * loc, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				dc2_current_ = wd.add_widget<widget_label>(wp, wp_);
				dc2_current_->at_local_param().select_func_ = [=](const std::string& str) {
					dc2_current_->set_text(limitf_(str, 0.0f, 100.0f, "%3.2f"));
				};
			}
			{
				widget::param wp(vtx::irect(ofsx + 470, 20 + h * loc, 50, 40), dialog_);
				widget_text::param wp_("mA");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				dc2_exec_ = wd.add_widget<widget_button>(wp, wp_);
				dc2_exec_->at_local_param().select_func_ = [=](int n) {
					dc2_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 14; ++i) {
						sw <<= 1;
						if(dc2_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = dc2_ena_->get_check();
					t.mode = dc2_mode_->get_select_pos() & 1;
					float v;
					if((utils::input("%f", dc2_voltage_->get_text().c_str()) % v).status()) {
						t.volt = v / 312.5e-6;
					}
					if((utils::input("%f", dc2_current_->get_text().c_str()) % v).status()) {
						t.curr = v / 100e-6;
					}

					client_.send(t.build());
				};
			}
		}


		void init_gen_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, gen_sw_, 5, 44);
			++loc;
			// ジェネレーター・モジュール
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 100, 40), dialog_);
				widget_check::param wp_("有効");
				gen_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // (6) ジェネレータ設定： 出力モード選択（矩形波/三角波/直流）
			   // タイプ（連続、単発）、
			   // 出力電圧、周波数、ON時間（レンジ：0.01ms）
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("矩形波");
				wp_.init_list_.push_back("三角波");
				gen_mode_ = wd.add_widget<widget_list>(wp, wp_);
				gen_mode_->at_local_param().select_func_ = [=](const std::string& str,
															   uint32_t pos) {
					if(pos == 0) {
						gen_duty_->set_stall(false);
					} else {
						gen_duty_->set_stall();
					}
				};
			}
			{  // ジェネレータ設定、周波数（1Hz to 100Hz, 1Hz/step)
				widget::param wp(vtx::irect(ofsx + 230 + 130 * 0, 20 + h * loc, 70, 40), dialog_);
				widget_label::param wp_("1", false);
				gen_freq_ = wd.add_widget<widget_label>(wp, wp_);
				gen_freq_->at_local_param().select_func_ = [=](const std::string& str) {
					gen_freq_->set_text(limitf_(str, 1.0f, 100.0f, "%1.0f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 0 + 80, 20 + h * loc, 30, 40),
						dialog_);
					widget_text::param wp_("Hz");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、電圧（0V to 14V, 0.1V/step)
				widget::param wp(vtx::irect(ofsx + 230 + 120 * 1, 20 + h * loc, 70, 40), dialog_);
				widget_label::param wp_("0", false);
				gen_volt_ = wd.add_widget<widget_label>(wp, wp_);
				gen_volt_->at_local_param().select_func_ = [=](const std::string& str) {
					gen_volt_->set_text(limitf_(str, 0.0f, 14.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 1 + 80, 20 + h * loc, 30, 40),
									 dialog_);
					widget_text::param wp_("V");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{  // ジェネレータ設定、DUTY（0.1% to 100%, 0.1%/step）
				widget::param wp(vtx::irect(ofsx + 230 + 120 * 2, 20 + h * loc, 70, 40), dialog_);
				widget_label::param wp_("0.1", false);
				gen_duty_ = wd.add_widget<widget_label>(wp, wp_);
				gen_duty_->at_local_param().select_func_ = [=](const std::string& str) {
					gen_duty_->set_text(limitf_(str, 0.1f, 100.0f, "%2.1f"));
				};
				{
					widget::param wp(vtx::irect(ofsx + 230 + 120 * 2 + 80, 20 + h * loc, 30, 40),
									 dialog_);
					widget_text::param wp_("%");
					wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
												 vtx::placement::vertical::CENTER);
					wd.add_widget<widget_text>(wp, wp_);
				}
			}
			{
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				gen_exec_ = wd.add_widget<widget_button>(wp, wp_);
				gen_exec_->at_local_param().select_func_ = [=](int n) {
					wgm_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 5; ++i) {
						sw <<= 1;
						if(gen_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = gen_ena_->get_check();
					t.type = gen_mode_->get_select_pos() & 1;
					float v;
					if((utils::input("%f", gen_freq_->get_text().c_str()) % v).status()) {
						t.frq = v;
					}
					if((utils::input("%f", gen_duty_->get_text().c_str()) % v).status()) {
						t.duty = v * 10.0f;
					}
					if((utils::input("%f", gen_volt_->get_text().c_str()) % v).status()) {
						t.volt = v / 0.02f;
					}

					client_.send(t.build());
				};
			}
		}


		void init_crm_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, crm_sw_, 14, 1);
			++loc;
			// ＣＲメジャー・モジュール
			{
				widget::param wp(vtx::irect(ofsx, 20 + h * loc, 90, 40), dialog_);
				widget_check::param wp_("有効");
				crm_ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 周波数設定 (100、1K, 10K)
				widget::param wp(vtx::irect(ofsx + 90, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("100 Hz");
				wp_.init_list_.push_back("1  KHz");
				wp_.init_list_.push_back("10 KHz");
				crm_freq_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 抵抗値、容量値選択
				widget::param wp(vtx::irect(ofsx + 220, 20 + h * loc, 110, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("抵抗値");
				wp_.init_list_.push_back("容量値");
				crm_mode_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 350, 20 + h * loc, 140, 40), dialog_);
				widget_label::param wp_("");
				crm_ans_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{
				widget::param wp(vtx::irect(ofsx + 500, 20 + h * loc, 50, 40), dialog_);
				widget_text::param wp_("-");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
					  						 vtx::placement::vertical::CENTER);
				crm_anst_ = wd.add_widget<widget_text>(wp, wp_);
			}
			{  // exec
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				crm_exec_ = wd.add_widget<widget_button>(wp, wp_);
				crm_exec_->at_local_param().select_func_ = [=](int n) {
					crm_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 14; ++i) {
						sw <<= 1;
						if(crm_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;
					t.ena = crm_ena_->get_check();
					t.freq = crm_freq_->get_select_pos();
					t.mode = crm_mode_->get_select_pos();

					client_.send(t.build());
				};
			}
		}


		void init_icm_(int d_w, int ofsx, int h, int loc)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			init_sw_(ofsx, h, loc, icm_sw_, 6, 34);
			{  // exec
				widget::param wp(vtx::irect(d_w - 50, 20 + h * loc, 30, 30), dialog_);
				widget_button::param wp_(">");
				icm_exec_ = wd.add_widget<widget_button>(wp, wp_);
				icm_exec_->at_local_param().select_func_ = [=](int n) {
					icm_t t;
					uint16_t sw = 0;
					for(int i = 0; i < 6; ++i) {
						sw <<= 1;
						if(icm_sw_[i]->get_check()) sw |= 1;
					}
					t.sw = sw;

					client_.send(t.build());
				};
			}
		}


		void load_sw_(sys::preference& pre, gui::widget_check* sw[], uint32_t n)
		{
			for(uint32_t i = 0; i < n; ++i) {
				sw[i]->load(pre);
			}
		}


		void save_sw_(sys::preference& pre, gui::widget_check* sw[], uint32_t n)
		{
			for(uint32_t i = 0; i < n; ++i) {
				sw[i]->save(pre);
			}
		}


	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		inspection(utils::director<core>& d, net::ign_client& client) : director_(d), client_(client),
			dialog_(nullptr),
			unit_name_(nullptr), load_file_(nullptr), save_file_(nullptr),

			dc1_sw_{ nullptr },
			dc1_ena_(nullptr),
			dc1_exec_(nullptr),

			dc2_sw_{ nullptr },
			dc2_ena_(nullptr), dc2_mode_(nullptr), dc2_voltage_(nullptr), dc2_current_(nullptr),
			dc2_exec_(nullptr),

			gen_ena_(nullptr), gen_mode_(nullptr), gen_freq_(nullptr),
			gen_volt_(nullptr), gen_duty_(nullptr), gen_exec_(nullptr),

			crm_sw_{ nullptr },
			crm_ena_(nullptr), crm_freq_(nullptr), crm_mode_(nullptr),
			crm_ans_(nullptr), crm_exec_(nullptr),

			icm_sw_{ nullptr }, icm_exec_(nullptr),

			wait_time_(nullptr),
			help_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログの取得
			@return ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_dialog() { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
//			auto& core = gl::core::get_instance();
//			const auto& scs = core.get_rect().size;

			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int d_w = 1000;
			int d_h = 700;
			{
				widget::param wp(vtx::irect(100, 100, d_w, d_h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				dialog_->at_local_param().select_func_ = [=](bool ok) {
				};
			}
			int w = 130;
			int h = 45;
			static const char* tbls[] = {
				"ファイル名：",
				"ＤＣ１：", nullptr,
				"ＤＣ２：", nullptr,
				"発信機：", nullptr,
				"ＣＲ測定：", nullptr,
				"負荷切替："
			};
			for(int i = 0; i < sizeof(tbls) / sizeof(const char*); ++i) {
				widget::param wp(vtx::irect(20, 20 + h * i, w, h), dialog_);
				if(tbls[i] == nullptr) continue;
				widget_text::param wp_(tbls[i]);
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}
			int ofsx = w + 10;
			{  // 単体試験名
				widget::param wp(vtx::irect(ofsx, 20 + h * 0, 300, 40), dialog_);
				widget_label::param wp_("", false);
				unit_name_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // ロード・ファイル
				widget::param wp(vtx::irect(ofsx + 320, 20 + h * 0, 100, 40), dialog_);
				widget_button::param wp_("ロード");
				load_file_ = wd.add_widget<widget_button>(wp, wp_);
				load_file_->at_local_param().select_func_ = [=](bool f) {
					std::string filter = "単体テスト(*.";
					filter += UNIT_EXT_;
					filter += ")\t*.";
					filter += UNIT_EXT_;
					filter += "\t";
					unit_load_filer_.open(filter);
				};
			}
			{  // ロード・ファイル
				widget::param wp(vtx::irect(ofsx + 440, 20 + h * 0, 100, 40), dialog_);
				widget_button::param wp_("セーブ");
				save_file_ = wd.add_widget<widget_button>(wp, wp_);
				save_file_->at_local_param().select_func_ = [=](bool f) {
					std::string filter = "単体テスト(*.";
					filter += UNIT_EXT_;
					filter += ")\t*.";
					filter += UNIT_EXT_;
					filter += "\t";
					unit_save_filer_.open(filter, true);
				};
			}
#if 0
			{  // 検査規格
				widget::param wp(vtx::irect(ofsx, 20 + h * 1, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("MAX 値");
				wp_.init_list_.push_back("MIN 値");
				inspection_standards_ = wd.add_widget<widget_list>(wp, wp_);
			}
			{  // 検査方法
				widget::param wp(vtx::irect(ofsx, 20 + h * 2, 150, 40), dialog_);
				widget_list::param wp_;
				wp_.init_list_.push_back("静特性検査");
				wp_.init_list_.push_back("動特性検査");
				wp_.init_list_.push_back("LCR検査");
				inspection_method_ = wd.add_widget<widget_list>(wp, wp_);
				inspection_method_->at_local_param().select_func_
					= [=](const std::string& str, uint32_t pos) {
					widget_director& wd = director_.at().widget_director_;
					if(pos == 0) {
						voltage_[0]->set_stall(false);
						current_[0]->set_stall(false);

						wd.enable(voltage_[0]);
						wd.enable(current_[0]);
						wd.enable(voltage_[1], false);
						wd.enable(current_[1], false);
						current_text_->set_text("mA");
					} else if(pos == 1) {
						voltage_[1]->set_stall(false);
						current_[1]->set_stall(false);

						wd.enable(voltage_[0], false);
						wd.enable(current_[0], false);
						wd.enable(voltage_[1]);
						wd.enable(current_[1]);
						current_text_->set_text("A");
					} else if(pos == 2) {  // LCR 検査
						voltage_[0]->set_stall();
						current_[0]->set_stall();
						voltage_[1]->set_stall();
						current_[1]->set_stall();
					}
				};
#endif
				init_dc1_(d_w, ofsx, h, 1);
				init_dc2_(d_w, ofsx, h, 3);
				init_gen_(d_w, ofsx, h, 5);
				init_crm_(d_w, ofsx, h, 7);
				init_icm_(d_w, ofsx, h, 9);

			{  // (10) Wait時間設定： ０～１．０ｓ（レンジ：０．０１ｓ）
				widget::param wp(vtx::irect(ofsx, 20 + h * 12, 90, 40), dialog_);
				widget_label::param wp_("0", false);
				wait_time_ = wd.add_widget<widget_label>(wp, wp_);
				wait_time_->at_local_param().select_func_ = [=](const std::string& str) {
					wait_time_->set_text(limitf_(str, 0.0f, 1.0f, "%3.2f"));
				};
			}

			{  // help message
				widget::param wp(vtx::irect(20, d_h - 100, d_w, 40), dialog_);
				widget_text::param wp_;
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				help_ = wd.add_widget<widget_text>(wp, wp_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(!dialog_->get_state(gui::widget::state::ENABLE)) return;

			// モジュールから受け取ったパラメーターをＧＵＩに反映
			if(client_.recv_probe()) {
				auto s = client_.recv();
				if(s.find("CRCD") == 0) {
					auto t = s.substr(4, 4);
					int v = 0;
					utils::input("%x", t.c_str()) % v;
					if(v >= 1000) {
						float a = static_cast<float>(v) / 10000;
						crm_ans_->set_text((boost::format("%6.5f") % a).str());
						crm_anst_->set_text("uF");
					} else {
						float a = static_cast<float>(v) / 10;
						crm_ans_->set_text((boost::format("%5.4f") % a).str());
						crm_anst_->set_text("pF");
					}
				} else if(s.find("CRRD") == 0) {
					auto t = s.substr(4, 4);
					int v = 0;
					utils::input("%x", t.c_str()) % v;
					float a = static_cast<float>(v) / 1000;
					crm_ans_->set_text((boost::format("%5.4f") % a).str());
					crm_anst_->set_text("mΩ");
				}
			}


			if(unit_load_filer_.state()) {
				auto path = unit_load_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += UNIT_EXT_;
					}
					load(ph);
				}
			}
			if(unit_save_filer_.state()) {
				auto path = unit_save_filer_.get();
				if(!path.empty()) {
					auto ph = path;
					if(utils::get_file_ext(path).empty()) {
						ph += UNIT_EXT_;
					}
					save(ph);
				}
			}

			// ヘルプ機能
			if(dc1_current_->get_focus()) {
				help_->set_text("0.0 to 30.0 [A], 0.1 [A] / step");
			} else if(dc1_voltage_->get_focus()) {
				help_->set_text("0.0 to 60.0 [V], 0.1 [V] / step");
			} else if(dc2_voltage_->get_focus()) {
				help_->set_text("0.0 to 300.0 [V], 0.1 [V] / step");
			} else if(dc2_current_->get_focus()) {
				help_->set_text("0.0 to 100.0 [mA], 0.01 [mA] / step");
			} else if(gen_freq_->get_focus()) {
				help_->set_text("1 to 100 [Hz], 1 [Hz] / step");
			} else if(gen_volt_->get_focus()) {
				help_->set_text("0.0 to 14.0 [V], 0.1 [V] / step");
			} else if(gen_duty_->get_focus()) {
				help_->set_text("0.1 to 100.0 [%], 0.1 [%] / step");
			} else if(wait_time_->get_focus()) {
				help_->set_text("0.0 to 1.0 [秒], 0.01 [秒] / step");
			} else {
				help_->set_text("");
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(const std::string& path)
		{
			sys::preference pre;
			unit_name_->save(pre);

			save_sw_(pre, dc1_sw_, 5);
			dc1_ena_->save(pre);
			dc1_mode_->save(pre);
			dc1_voltage_->save(pre);
			dc1_current_->save(pre);

			save_sw_(pre, dc2_sw_, 14);
			dc2_ena_->save(pre);
			dc2_mode_->save(pre);
			dc2_voltage_->save(pre);
			dc2_current_->save(pre);

			save_sw_(pre, gen_sw_, 5);
			gen_ena_->save(pre);
			gen_mode_->save(pre);
			gen_freq_->save(pre);
			gen_volt_->save(pre);
			gen_duty_->save(pre);

			save_sw_(pre, crm_sw_, 14);
			crm_ena_->save(pre);
			crm_freq_->save(pre);
			crm_mode_->save(pre);

			save_sw_(pre, icm_sw_, 6);

			wait_time_->save(pre);

			return pre.save(path);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	path	ファイルパス
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const std::string& path)
		{
			sys::preference pre;
			auto ret = pre.load(path);
			if(ret) {
				unit_name_->load(pre);

				load_sw_(pre, dc1_sw_, 5);
				dc1_ena_->load(pre);
				dc1_mode_->load(pre);
				dc1_voltage_->load(pre);
				dc1_current_->load(pre);

				load_sw_(pre, dc2_sw_, 14);
				dc2_ena_->load(pre);
				dc2_mode_->load(pre);
				dc2_voltage_->load(pre);
				dc2_current_->load(pre);

				load_sw_(pre, gen_sw_, 5);
				gen_ena_->load(pre);
				gen_mode_->load(pre);
				gen_freq_->load(pre);
				gen_volt_->load(pre);
				gen_duty_->load(pre);

				load_sw_(pre, crm_sw_, 14);
				crm_ena_->load(pre);
				crm_freq_->load(pre);
				crm_mode_->load(pre);

				load_sw_(pre, icm_sw_, 6);

				wait_time_->load(pre);
			}
			return ret; 
		}
	};
}
