#pragma once
//=====================================================================//
/*! @file
    @brief  波形描画テンプレート・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include "gl_fw/glutils.hpp"

namespace view {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  render_waves template class
		@param[in]	UNIT	波形の型
		@param[in]	LIMIT	最大波形数
		@param[in]	CHN		チャネル数
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <typename UNIT, uint32_t LIMIT, uint32_t CHN>
	class render_waves {
	public:
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  チャネル描画パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct chr_param {
			img::rgba8	color_;		///< 描画色
			vtx::ipos	offset_;	///< オフセット
			float		gain_;		///< 垂直ゲイン（電圧）

			bool		update_;	///< 描画の更新「true」
			bool		render_;	///< 描画しない場合「false」
			bool		ground_;	///< GND 電位表示

			chr_param() : color_(img::rgba8(255, 255)),
				offset_(0), gain_(1.0f), update_(false), render_(true),
				ground_(false)
			{ }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief  インフォメーション描画パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		class info_param {
		public:
			img::rgba8	grid_color_;	///< グリッド・カラー
			int32_t		grid_step_;		///< グリッド・ステップ
			uint16_t	grid_stipple_;	///< グリッド・破線パターン
			bool		grid_enable_;	///< グリッド有効

			img::rgba8	time_color_;	///< 時間軸カラー
			int32_t		time_org_;		///< 時間軸開始
			int32_t		time_len_;		///< 時間軸長さ
			uint16_t	time_stipple_;	///< 時間軸破線パターン
			bool		time_enable_;	///< 時間軸有効
//			bool		time_anime_;	///< 時間軸アニメーション

			img::rgba8	volt_color_;	///< 電圧軸カラー
			int32_t		volt_org_;		///< 電圧軸開始
			int32_t		volt_len_;		///< 電圧軸長さ
			uint16_t	volt_stipple_;	///< 電圧軸破線パターン
			bool		volt_enable_;	///< 電圧軸有効
//			bool		volt_anime_;	///< 電圧軸アニメーション

			img::rgba8	trig_color_;	///< トリガー軸カラー
			int32_t		trig_pos_;		///< トリガー軸位置（時間軸）
			bool		trig_enable_;	///< トリガー軸有効

		private:
			vtx::sposs	grid_;
			vtx::sposs	time_;
			vtx::sposs	volt_;
			vtx::ipos	size_;
			vtx::sposs	trig_;

			uint32_t	count_;

			static void rotate_(uint16_t& bits) {
				auto mod = bits & 0x8000;
				bits <<= 1;
				if(mod) bits |= 1;
			}


		public:
			info_param() : grid_color_(img::rgba8(255, 255, 255, 96)), grid_step_(30),
				grid_stipple_(0b1111000011110000), grid_enable_(true),

				time_color_(img::rgba8(255, 255, 128, 192)),
				time_org_(0), time_len_(0),
				time_stipple_(0b1110110011101100), time_enable_(true),

				volt_color_(img::rgba8(128, 255, 128, 192)),
				volt_org_(0), volt_len_(0),
				volt_stipple_(0b1110110011101100), volt_enable_(true),

				trig_color_(img::rgba8(190, 255, 140, 192)),
				trig_pos_(0), trig_enable_(true),

				grid_(), time_(), volt_(), size_(), count_(0)
			{ }


			void build(const vtx::ipos& size)
			{
				if(size == size_) return;
				size_ = size;

				grid_.clear();
				for(int h = 0; h < size.x; h += grid_step_) {  // |||
					grid_.push_back(vtx::spos(h, 0));
					grid_.push_back(vtx::spos(h, size.y));
				}
				// ---
				int org = (size.y / 2) / grid_step_;
				org *= grid_step_;
				if(org < (size.y / 2)) org += grid_step_;
				for(int v = -org; v < (size.y / 2); v += grid_step_) {
					grid_.push_back(vtx::spos(0, v + size.y / 2));
					grid_.push_back(vtx::spos(size.x, v + size.y / 2));
				}

				time_.clear();
				time_.push_back(vtx::spos(0, 0));
				time_.push_back(vtx::spos(0, size.y));

				volt_.clear();
				volt_.push_back(vtx::spos(0, 0));
				volt_.push_back(vtx::spos(size.x, 0));

				trig_.clear();
				trig_.push_back(vtx::spos(0, 0));
				trig_.push_back(vtx::spos(0, size.y));
			}


			void render()
			{
				glEnable(GL_LINE_STIPPLE);
				if(!grid_.empty() && grid_enable_) {
					glLineStipple(1, grid_stipple_);
					gl::glColor(grid_color_);
					gl::draw_lines(grid_);
				}
				if(time_enable_) {
					glLineStipple(1, time_stipple_);
					glPushMatrix();
					gl::glTranslate(time_org_, 0);
					gl::glColor(time_color_);
					gl::draw_lines(time_);
					gl::glTranslate(time_len_, 0);
					gl::draw_lines(time_);
					glPopMatrix();
				}
				if(volt_enable_) {
					glLineStipple(1, volt_stipple_);
					glPushMatrix();
					gl::glTranslate(0, volt_org_);
					gl::glColor(volt_color_);
					gl::draw_lines(volt_);
					gl::glTranslate(0, volt_len_);
					gl::draw_lines(volt_);
					glPopMatrix();
				}
				if(trig_enable_) {
					glLineStipple(1, time_stipple_);
					glPushMatrix();
					gl::glTranslate(trig_pos_, 0);
					gl::glColor(trig_color_);
					gl::draw_lines(trig_);
					glPopMatrix();
				}
				if(count_ > 0) {
					--count_;
				} else {
					rotate_(time_stipple_);
					rotate_(volt_stipple_);
					count_ = 3;
				}
				glDisable(GL_LINE_STIPPLE);
			}
		};

	private:
		typedef std::vector<UNIT> UNITS;

		struct ch_t {
			chr_param	param_;
			uint32_t	tstep_;
			UNITS		units_;
			vtx::sposs	lines_;

			ch_t() : param_(), tstep_(0), units_(), lines_()
			{ }
		};

		info_param	info_;

		ch_t		ch_[CHN];
		double		div_;

		float		gain_[CHN];
		vtx::ipos	offset_[CHN];

		vtx::ipos	win_size_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		render_waves() : ch_{ }, div_(0.0), gain_{ 1.0f }, win_size_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  波形の最大数を取得
			@return 波形の最大数
		*/
		//-----------------------------------------------------------------//
		uint32_t size() const { return LIMIT; }


		//-----------------------------------------------------------------//
		/*!
			@brief  パラメーターを取得
			@param[in]	ch	チャネル
			@return パラメーター
		*/
		//-----------------------------------------------------------------//
		const chr_param& get_param(uint32_t ch) const
		{
			if(ch >= CHN) {
				static chr_param p;
				return p;
			}
			return ch_[ch].param_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  パラメーターを参照
			@param[in]	ch	チャネル
			@return パラメーター
		*/
		//-----------------------------------------------------------------//
		chr_param& at_param(uint32_t ch)
		{
			if(ch >= CHN) {
				static chr_param p;
				return p;
			}
			return ch_[ch].param_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  情報パラメーターを取得
			@return 情報パラメーター
		*/
		//-----------------------------------------------------------------//
		const info_param& get_info() const {
			return info_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  情報パラメーターを参照
			@return 情報パラメーター
		*/
		//-----------------------------------------------------------------//
		info_param& at_info() {
			return info_;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形生成
		*/
		//-----------------------------------------------------------------//
		void create_buffer()
		{
			for(int i = 0; i < CHN; ++i) {
				ch_[i].units_.resize(size());
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  テスト波形生成
			@param[in]	ch		チャネル
			@param[in]	frq		周波数 [Hz]
			@param[in]	gain	ゲイン（振幅）
		*/
		//-----------------------------------------------------------------//
		void build_sin(uint32_t ch, double frq, double gain)
		{
			if(ch >= CHN) return;

			for(uint32_t i = 0; i < ch_[ch].units_.size(); ++i) {
				double t = frq / static_cast<double>(LIMIT);
				ch_[ch].units_[i] = 32768 - static_cast<UNIT>(sin(2.0 * vtx::get_pi<double>()
					* t * i) * gain * 32767.0);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  波形コピー
			@param[in]	ch		チャネル
			@param[in]	src		波形ソース
			@param[in]	len		波形数
		*/
		//-----------------------------------------------------------------//
		void copy(uint32_t ch, const UNIT* src, uint32_t len)
		{
			if(len > ch_[ch].units_.size()) len = ch_[ch].units_.size();
			for(uint32_t i = 0; i < len; ++i) {
				uint16_t w = *src++;
				ch_[ch].units_[i] = w;
			}
			ch_[ch].param_.update_ = true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
   			@param[in]	size	描画サイズ（ピクセル）
			@param[in]	step	時間軸ステップ（65536を1.0）
		*/
		//-----------------------------------------------------------------//
		void render(const vtx::ipos& size, uint32_t tstep)
		{
			bool update_win = win_size_ != size;
			win_size_ = size;
			for(uint32_t n = 0; n < CHN; ++n) {
				ch_t& t = ch_[n];
				if(!t.param_.render_) continue;

				bool update = update_win;
				if(t.units_.empty()) update = false;
				else {
					if(t.lines_.size() != size.x) {
						t.lines_.resize(size.x);
					}
					if(t.tstep_ != tstep) {
						t.tstep_ = tstep;
						update = true;
					}
					if(t.param_.gain_ != gain_[n]) {
						gain_[n] = t.param_.gain_;
						update = true;
					}
					if(t.param_.offset_ != offset_[n]) {
						offset_[n] = t.param_.offset_;
						update = true;
					}
				}
				int mod_x = 0;
				if(update || t.param_.update_) {
					float gain = t.param_.gain_;
					uint32_t tsc = t.param_.offset_.x * tstep;
					for(uint32_t i = 0; i < size.x; ++i) {
						uint32_t idx = (tsc >> 16) % t.units_.size();
						float v = static_cast<float>(t.units_[idx]);
						v -= 32768.0f;
						t.lines_[i] = vtx::spos(i, v * -gain);
						tsc += tstep;
					}
					t.param_.update_ = false;
				}
				if(!t.lines_.empty()) {
					glPushMatrix();
					gl::glTranslate(mod_x, t.param_.offset_.y);
					gl::glColor(t.param_.color_);
					gl::draw_line_strip(t.lines_);
					glPopMatrix();
				}
			}

			info_.build(size);

			info_.render();
		}
	};
}
