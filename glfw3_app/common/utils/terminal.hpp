#pragma once
//=====================================================================//
/*!	@file
	@brief	ターミナル・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <vector>
#include <deque>
#include "img_io/img.hpp"
#include "utils/vtx.hpp"
#include "utils/string_utils.hpp"

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ターミナル・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct terminal {

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	文字トランク
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct cha_t {
			uint32_t	cha_;
			img::rgba8	fc_;
			img::rgba8	bc_;

			cha_t(uint32_t cha = 0,
				  const img::rgba8& fc = img::rgba8(255, 255, 255, 255),
				  const img::rgba8& bc = img::rgba8(  0,   0,   0, 255)) :
				cha_(cha), fc_(fc), bc_(bc) { }
		};

		typedef std::vector<cha_t>	line;
		typedef std::deque<line>	lines;

	private:
		cha_t		cha_;

		lines		lines_;
		uint32_t	max_;

		vtx::ipos	pos_;

		cha_t		tmp_;

		bool		cre_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
			@param[in]	max	最大ライン数
		*/
		//-----------------------------------------------------------------//
		terminal(uint32_t max = 150) : cha_(), lines_(), max_(max), pos_(0), tmp_(' '), cre_(true) {
			line l;
			lines_.push_back(l);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~terminal() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	消去
		*/
		//-----------------------------------------------------------------//
		void clear() {
			lines_.clear();
			line l;
			lines_.push_back(l);
			pos_.set(0);
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	cha	文字
		*/
		//-----------------------------------------------------------------//
		void output(uint32_t cha) { 
			if(cha == '\n' || (cre_ && cha == '\r')) {
				if(lines_.size() >= max_) {
					lines_.pop_front();
				}
				line l;
				lines_.push_back(l);
				pos_.x = 0;
			} else {
				line& l = lines_.back();
				cha_.cha_ = cha;
				l.push_back(cha_);
				pos_.x = l.size();
			}
			pos_.y = lines_.size() - 1;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	文字出力
			@param[in]	str	文字列
		*/
		//-----------------------------------------------------------------//
		void output(const std::string& str) {
			auto ls = utils::utf8_to_utf32(str);
			for(auto lch : ls) {
				output(lch);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル位置を取得
			@return カーソル位置
		*/
		//-----------------------------------------------------------------//
		const vtx::ipos& get_cursor() const { return pos_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ライン数を取得
			@return ライン数
		*/
		//-----------------------------------------------------------------//
		uint32_t get_line_num() const { return lines_.size(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ラインを取得
			@param[in]	pos	ライン位置
			@return ライン
		*/
		//-----------------------------------------------------------------//
		const line& get_line(uint32_t pos) const { return lines_[pos]; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キャラクター・コンテナを取得
			@param[in]	pos	キャラクター位置
			@return キャラクター
		*/
		//-----------------------------------------------------------------//
		const cha_t& get_char(const vtx::ipos& pos) const {
			if(pos.y >= lines_.size()) return tmp_;
			const auto& l = lines_[pos.y];
			if(pos.x >= l.size()) return tmp_;
			return l[pos.x]; 
		}		
	};
}
