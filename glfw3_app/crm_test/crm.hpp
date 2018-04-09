#pragma once
//=====================================================================//
/*! @file
    @brief  CRM 直接、クラス @n
		１、DC成分の除去 @n
			FPGAからレポートされる抵抗成分データCRRD、容量成分データCRCD、@n
			直流成分データCRDDを使用して、まずDC成分の除去を実施してください。@n
			SRP = CRRD - CRDD  //電圧オフセット分を取り除いた実数部データです。@n
			SIP = CRCD - CRDD  //電圧オフセット分を取り除いた虚数部データです。@n
		２、位相オフセット成分の除去 @n
			回路等に含まれる位相オフセット分を除去するため、SRPおよびSIPを次の様に@n
			修正願います。@n
			CRP = SRP + A_CAL * SIP  //A_CALの値は、純抵抗を測定した時（位相校正操作時）の @n
        	CIP = SIP - A_CAL * SRP  //(SIP/SRP)の値（位相補正係数）を使用します。（後述）@n
		３、抵抗値および容量値の算出 @n
			CRPおよびCIPの値から、次の式を用いて抵抗値と容量値を求めます。@n
        	R [ohm]= COR * (CRP^2 + CIP^2) / (_i * CRP) @n
        	C [F] = (COC * _i * CIP) / {(2 * π * _f) * (CRP^2 + CIP^2) } @n
			ここで：@n
			CORは抵抗値計算用の定数係数 = 4.6588E-8 @n
			COCは容量値計算用の定数係数 = 21454732.4 @n
			_iはユーザーが選択した測定電流値 [Ap-p] @n
			_fはユーザーが選択した測定周波数 [Hz] @n
		４、位相補正係数A_CALの決定 @n
			位相オフセットは、電圧オフセットのように半自動的に除去する事ができないので、@n
			ユーザー操作によりシステムの構成を行う必要があります。並列容量が存在しない筈 @n
			の純抵抗素子；@n
			・0.2mAp-p時には3.3KΩ程度@n
			・2mAp-p時には330Ω程度@n
			・20mAp-p時には33Ω程度@n
			・200mAp-p時には3.3Ω程度@n
			（いずれも、0.778Vp-pを越えない範囲でなるべく大きな測定電圧が得られる値です。@n
			正確である必要はありません。）@n
			の抵抗を測定させ、その時のSIP/SRPの計算結果をA_CALの値として使用します。@n
			（この値には測定回路系に含まれる位相誤差の合計値[rad]がほぼそのまま反映されます。）
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <algorithm>
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
#include "widgets/widget_chip.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"

#include "utils/serial_win32.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  CRM クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class crm
	{
		static const int32_t SAMPLE_NUM = 50;

		utils::director<core>&	director_;
		device::serial_win32&	serial_;

		gui::widget_check*		sw_[14];	///< CRM 接続スイッチ
		gui::widget_check*		ena_;		///< CRM 有効、無効
		gui::widget_list*		amps_;		///< CRM 電流レンジ切り替え
		gui::widget_list*		freq_;		///< CRM 周波数（100Hz, 1KHz, 10KHz）
		gui::widget_list*		mode_;		///< CRM 抵抗測定、容量測定
		gui::widget_label*		ans_;		///< CRM 測定結果 
		gui::widget_button*		exec_;		///< CRM 設定転送
		gui::widget_list*		carib_type_;
		gui::widget_button*		carib_;

		gui::widget_label*		carib_tbls_[4];

		gui::widget_label*		net_regs_;

		gui::widget_terminal*	termcore_;

		std::string				crm_line_;
		uint32_t				crrd_id_;
		uint32_t				crcd_id_;
		uint32_t				crdd_id_;
		uint32_t				crrd_value_;
		uint32_t				crcd_value_;
		uint32_t				crdd_value_;

		enum class carib_task {
			idle,
			C0,   ///< 0.2mA / 3.3K
			C1,   ///< 2mA   / 330
			C2,   ///< 20mA  / 33
			C3,   ///< 200mA / 3.3
		};
		carib_task				carib_task_;

		struct crm_t {
			uint16_t	sw;		///< 14 bits
			bool		ena;	///< 0, 1
			uint16_t	amps;	///< 0, 1, 2, 3
			uint16_t	freq;	///< 0, 1, 2
			uint16_t	mode;	///< 0, 1

			crm_t() : sw(0), ena(0), amps(0), freq(0), mode(0) { }

			std::string build() const
			{
				std::string s;
				s = (boost::format("CRSW%04X \n") % sw).str();
				if(ena) {
					s += (boost::format("CRIS%d    \n") % (amps % 10)).str();
					static const char* frqtbl[3] = { "001", "010", "100" };
					s += (boost::format("CRFQ%s  \n") % frqtbl[freq % 3]).str();
				}
				s += (boost::format("CROE%d    \n") % ena).str();
				if(ena) {
					s += (boost::format("CRD?1    \n")).str();
					s += (boost::format("CRR?1    \n")).str();
					s += (boost::format("CRC?1    \n")).str();
				}
				return s;
			}
		};


		std::string make_crm_param_()
		{
			crm_t t;
			uint16_t sw = 0;
			for(int i = 0; i < 14; ++i) {
				sw <<= 1;
				if(sw_[i]->get_check()) sw |= 1;
			}
			t.sw = sw;
			t.ena = ena_->get_check();
			t.amps = amps_->get_select_pos();
			t.freq = freq_->get_select_pos();
			t.mode = mode_->get_select_pos();
			return t.build();
		}


		static void init_sw_(gui::widget_director& wd, gui::widget* root,
			int ofsx, int ofsy, gui::widget_check* out[], int num, int swn)
		{
///			auto md = get_module(swn);
			for(int i = 0; i < num; ++i) {
				gui::widget::param wp(vtx::irect(ofsx, ofsy, 60, 40), root);
				gui::widget_check::param wp_((boost::format("%d") % swn).str());
				out[i] = wd.add_widget<gui::widget_check>(wp, wp_);
				ofsx += 60;
///				ilc.install(md, static_cast<interlock::swtype>(swn), out[i]);
				++swn;
			}
		}


		static uint32_t get32_(const void* src)
		{
			uint32_t v;
			const uint8_t* ptr = static_cast<const uint8_t*>(src);
			v  = static_cast<uint32_t>(ptr[0]) << 24;
			v |= static_cast<uint32_t>(ptr[1]) << 16;
			v |= static_cast<uint32_t>(ptr[2]) << 8;
			v |= static_cast<uint32_t>(ptr[3]);
			return v;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		crm(utils::director<core>& d, device::serial_win32& serial) :
			director_(d), serial_(serial),
			sw_{ nullptr },
			ena_(nullptr), amps_(nullptr), freq_(nullptr), mode_(nullptr),
			ans_(nullptr), exec_(nullptr),
			carib_type_(nullptr), carib_(nullptr),
			carib_tbls_{ nullptr },
			net_regs_(nullptr),
			termcore_(nullptr),
			crm_line_(),
			crrd_id_(0), crcd_id_(0), crdd_id_(0),
			crrd_value_(0), crcd_value_(0), crdd_value_(0),
			carib_task_(carib_task::idle)
		{ }


		void set_termcore(gui::widget_terminal* tc) { termcore_ = tc; }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
			@param[in]	root	ルート
			@param[in]	d_w		横幅最大
			@param[in]	ofsx	オフセット X
			@param[in]	ofsy	オフセット Y
		*/
		//-----------------------------------------------------------------//
		void init(gui::widget* root, int d_w, int ofsx, int ofsy)
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			{
				widget::param wp(vtx::irect(15, ofsy, 60, 40), root);
				widget_text::param wp_("CRM:");
				wp_.text_param_.placement_ = vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
				wd.add_widget<widget_text>(wp, wp_);
			}

			init_sw_(wd, root, ofsx, ofsy, sw_, 14, 1);
			ofsy += 50;
			// ＣＲメジャー・モジュール
			{
				widget::param wp(vtx::irect(ofsx, ofsy, 90, 40), root);
				widget_check::param wp_("有効");
				ena_ = wd.add_widget<widget_check>(wp, wp_);
			}
			{  // 電流レンジ切り替え（2mA, 20mA, 200mA）
				widget::param wp(vtx::irect(ofsx + 90, ofsy, 110, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("0.2 mA");
				wp_.init_list_.push_back("  2 mA");
				wp_.init_list_.push_back(" 20 mA");
				wp_.init_list_.push_back("200 mA");
				amps_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 周波数設定 (100、1K, 10K)
				widget::param wp(vtx::irect(ofsx + 220, ofsy, 110, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("100 Hz");
				wp_.init_list_.push_back(" 1 KHz");
				wp_.init_list_.push_back("10 KHz");
				freq_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 抵抗値、容量値選択
				widget::param wp(vtx::irect(ofsx + 350, ofsy, 120, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("抵抗測定");
				wp_.init_list_.push_back("容量測定");
				wp_.init_list_.push_back("合成測定");
				mode_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // 答え
				widget::param wp(vtx::irect(ofsx + 500, ofsy, 200, 40), root);
				widget_label::param wp_("");
				ans_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // exec
				widget::param wp(vtx::irect(d_w - 55, ofsy, 30, 30), root);
				widget_button::param wp_(">");
				exec_ = wd.add_widget<widget_button>(wp, wp_);
				exec_->at_local_param().select_func_ = [=](int n) {
					auto s = make_crm_param_();
					serial_.write(s.c_str(), s.size());
					if(termcore_ != nullptr) {
						termcore_->output(s);
					}
					ans_->set_text("");
				};
			}
			{  // 合成回路、抵抗設定
				widget::param wp(vtx::irect(ofsx + 330, ofsy + 60, 110, 40), root);
				widget_label::param wp_("390.0", false);
				net_regs_ = wd.add_widget<widget_label>(wp, wp_);
			}
			{  // キャリブレーション・リスト
				widget::param wp(vtx::irect(ofsx, ofsy + 60, 170, 40), root);
				widget_list::param wp_;
				wp_.init_list_.push_back("0.2mA/3.3K");
				wp_.init_list_.push_back("2mA/330");
				wp_.init_list_.push_back("20mA/33");
				wp_.init_list_.push_back("200mA/3.3");
				carib_type_ = wd.add_widget<widget_list>(wp, wp_); 
			}
			{  // キャリブレーション・ボタン
				widget::param wp(vtx::irect(ofsx + 190, ofsy + 60, 90, 40), root);
				widget_button::param wp_("carib");
				carib_ = wd.add_widget<widget_button>(wp, wp_);
				carib_->at_local_param().select_func_ = [=](int n) {
					carib_task_ = static_cast<carib_task>(carib_type_->get_select_pos() + 1);
				};
			}
			for(int i = 0; i < 4; ++i) {  // キャリブレーション・データ
				widget::param wp(vtx::irect(ofsx + 180 * i, ofsy + 120, 170, 40), root);
				widget_label::param wp_("");
				carib_tbls_[i] = wd.add_widget<widget_label>(wp, wp_);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			{
				char tmp[256];
				auto len = serial_.read(tmp, sizeof(tmp));
				for(uint32_t i = 0; i < len; ++i) {
					crm_line_ += tmp[i];
				}
#if 0
if(!crm_line_.empty()) {
	std::cout << crm_line_.size() << std::endl;
	std::cout << crm_line_ << std::endl;
	crm_line_.clear();
}
#endif
			}

			// 受信した文字列をデコード
			bool crcd = false;
			bool crrd = false;
			bool crdd = false;
			if(crm_line_.size() >= 9 && crm_line_[8] == '\n') {
				std::string key;
				key += crm_line_[0];
				key += crm_line_[1];
				key += crm_line_[2];
				key += crm_line_[3];
				if(key == "CRCD") {
					crcd_value_ = get32_(&crm_line_[4]);
					++crcd_id_;
					crcd = true;
				} else if(key == "CRRD") {
					crrd_value_ = get32_(&crm_line_[4]);
					++crrd_id_;
					crrd = true;
				} else if(key == "CRDD") {
					crdd_value_ = get32_(&crm_line_[4]);
					++crdd_id_;
					crdd = true;
				}
				crm_line_ = crm_line_.substr(9);
			}

			if(crdd) {
				termcore_->output((boost::format("CRDD: %08X\n") % crdd_value_).str());
			}

			if(carib_task_ != carib_task::idle) {
				if(crcd) {
					float r = static_cast<int32_t>(crrd_value_ - crdd_value_);
					float c = static_cast<int32_t>(crcd_value_ - crdd_value_);
					std::string s = (boost::format("%5.4f") % (c / r)).str();
					carib_tbls_[carib_type_->get_select_pos()]->set_text(s);
				}
				return;
			}

			// 抵抗測定の場合
			if(mode_->get_select_pos() == 0 && crrd) {  // CRRD
				termcore_->output((boost::format("CRRD: %08X\n") % crrd_value_).str());
				int32_t v = static_cast<int32_t>(crrd_value_);
				int32_t ofs = static_cast<int32_t>(crdd_value_);
				v -= ofs;
				double lim = static_cast<double>(ofs) / 1.570798233;
				if(v >= lim) {
					ans_->set_text("OVF");
					return;
				}
				double a = static_cast<double>(v) / static_cast<double>(ofs) * 1.570798233;
				a *= 778.2;  // 778.2 mV P-P
				static const double itbl[4] = {  // 電流テーブル
					0.2, 2.0, 20.0, 200.0
				};
				a /= itbl[amps_->get_select_pos()];
				ans_->set_text((boost::format("%6.5f Ω") % a).str());

			} else if(mode_->get_select_pos() != 0 && crcd) {  // 容量、合成容量の場合
				termcore_->output((boost::format("CRCD: %08X\n") % crcd_value_).str());
				// 基準値 (ofs)
				int32_t ofs = static_cast<int32_t>(crdd_value_);

				// 抵抗値 (r)
				int32_t r = static_cast<int32_t>(crrd_value_);
				r -= ofs;

				// 容量値 (c)
				int32_t c = static_cast<int32_t>(crcd_value_);
				c -= ofs;
				double lim = static_cast<double>(ofs) / 1.570798233;
				if(c >= lim) {
					ans_->set_text("OVF");
					return;
				}
				if(r >= lim) {
					ans_->set_text("OVF");
					return;
				}

				static const double itbl[4] = {  // 電流テーブル
					0.2, 2.0, 20.0, 200.0
				};
				double ib = itbl[amps_->get_select_pos()];
				c -= 447158 * ib;
				c += 23989;

				double fc = static_cast<double>(c) / static_cast<double>(ofs) * 1.570798233;
				fc *= 778.2;  // 778.2 mV P-P

				static const double ftbl[4] = {  // 周波数テーブル
					100.0, 1000.0, 10000.0
				};
				double fq = ftbl[freq_->get_select_pos()];
				if(mode_->get_select_pos() == 1) {  // 容量
					fc = ib / (2.0 * 3.141592654 * fq * fc);
				} else {  // 合成
					float b = 0.0;
					if((utils::input("%f", net_regs_->get_text().c_str()) % b).status()) {
						b *= ib;
					}
					fc = (ib * fc) / (2.0 * 3.141592654 * fq * (fc * fc + b * b));
				}
				fc *= 1e6;
				ans_->set_text((boost::format("%6.5f uF") % fc).str());
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void save(sys::preference& pre)
		{
//			tools::save_sw(pre, sw_, 14);
			ena_->save(pre);
			amps_->save(pre);
			freq_->save(pre);
			mode_->save(pre);
			carib_tbls_[0]->save(pre);
			carib_tbls_[1]->save(pre);
			carib_tbls_[2]->save(pre);
			carib_tbls_[3]->save(pre);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  ロード
			@param[in]	pre	プリファレンス
		*/
		//-----------------------------------------------------------------//
		void load(sys::preference& pre)
		{
//			tools::load_sw(pre, sw_, 14);
			ena_->load(pre);
			amps_->load(pre);
			freq_->load(pre);
			mode_->load(pre);
			carib_tbls_[0]->load(pre);
			carib_tbls_[1]->load(pre);
			carib_tbls_[2]->load(pre);
			carib_tbls_[3]->load(pre);
		}
	};
}
