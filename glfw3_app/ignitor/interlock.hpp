#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・インターロック・クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <vector>
#include "widgets/widget_check.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  インターロック・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class interlock {
	public:
		//=================================================================//
		/*!
			@brief  スイッチの種別
		*/
		//=================================================================//
		enum class swtype {
			N,
			S01, S02, S03, S04, S05,
			S06, S07, S08, S09, S10,
			S11, S12, S13, S14, S15,
			S16, S17, S18, S19, S20,
			S21, S22, S23, S24, S25,
			S26, S27, S28, S29, S30,
			S31, S32, S33, S34, S35,
			S36, S37, S38, S39, S40,
			S41, S42, S43, S44, S45,
			S46, S47, S48, S49,
		};


		// バスの型
		enum class bustype {
			N,
			T1,
			T2,
			T3,
			T4,
			T5,
			T6,
			T7,
			RP,
			RN,
			RP_RN,
			T2_T7,
			T3_T7,
			VP,
			VN,
			VP_VN
		};

	private:
		struct interlock_t {
			swtype	swtype_;
			bustype	bustype_;
			gui::widget_check*	check_;
			bool	before_;
			interlock_t() : swtype_(swtype::N), bustype_(bustype::N),
				check_(nullptr), before_(false) { }
		};

		std::vector<interlock_t>	ilocks_;

		bool	enable_;

		bustype get_bustype_(swtype swt) {
			static const bustype bustbl[] = {
				bustype::N,
				// CRM: S01 to S14
				bustype::T1, bustype::T2,
				bustype::T3, bustype::T4, bustype::T5, bustype::T6,
				bustype::T7, bustype::T1, bustype::T2, bustype::T3,
				bustype::T4, bustype::T5, bustype::T6, bustype::T7,
				// DC2: S15 to S28
				bustype::T1, bustype::T2,
				bustype::T3, bustype::T4, bustype::T5, bustype::T6,
				bustype::T7, bustype::T1, bustype::T2, bustype::T3,
				bustype::T4, bustype::T5, bustype::T6, bustype::T7,
				// WDM: S29 to S32
				bustype::RP_RN, bustype::T2_T7, bustype::T3_T7, bustype::VP_VN,
				// 欠番 S33
				bustype::N,
				// ICM: S34 to S39
				bustype::T1,
				bustype::RP,
				bustype::RP,
				bustype::RP,
				bustype::VP,
				bustype::VN,
				// DC1: S40 to S43
				bustype::T1,
				bustype::T7, bustype::T7, bustype::RP,
				// WGM: S44 to S48
				bustype::T3,
				bustype::T3, bustype::T3, bustype::T3, bustype::T7,
				// DC1: S49
				bustype::RP,
			};
			return bustbl[static_cast<uint32_t>(swt)];
		}

		void lock_bus_(bustype bt, uint32_t inidx)
		{
//			std::cout << "Lock Bus: " << static_cast<int>(bt) << std::endl;

			uint32_t idx = 0;
			for(const auto& t : ilocks_) {
				if(inidx != idx) {

					bool e = false;
					if(bt == t.bustype_) {
						e = true;
					}
					if(e) {
						t.check_->set_check(false);
						t.check_->set_stall();
					}
				}
				++idx;
			}
		}


		void free_bus_(bustype bt, uint32_t inidx)
		{
//			std::cout << "OFF Bus: " << static_cast<int>(t.bustype_) << std::endl;

			uint32_t idx = 0;
			for(auto& t : ilocks_) {
				if(inidx != idx) {
					if(t.bustype_ == bt) {
						t.check_->set_stall(false);
					}
				}
				++idx;
			}
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		interlock() : ilocks_(), enable_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief  モジュールのスイッチ登録
			@param[in]	swtype	スイッチ種別
			@param[in]	check	チェック GUI
			@return 登録できたら「true」
		*/
		//-----------------------------------------------------------------//
		bool install(swtype swt, gui::widget_check* check)
		{
			interlock_t t;
			t.swtype_ = swt;
			t.bustype_ = get_bustype_(swt);
			t.check_ = check;
			t.before_ = check->get_check();
			ilocks_.push_back(t);
/// std::cout << "SW: " << static_cast<int>(swt) << ", BUS: " << static_cast<int>(t.bustype_) << std::endl;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
			@param[in]	ena	インターロック機構を有効な場合「true」
		*/
		//-----------------------------------------------------------------//
		void update(bool ena)
		{
			bool enable = enable_;
			enable_ = ena;
			// interlock が無効になった場合全て、「STALL」を無効にする。
			if(enable && !ena) {
				for(auto& t : ilocks_) {
					t.check_->set_stall(false);
				}
				return;
			}
			if(!ena) return;

			uint32_t idx = 0;
			for(auto& t : ilocks_) {
				bool before = t.before_;
				t.before_ = t.check_->get_check();
				if(!before && t.check_->get_check()) {  // チェック ON
					lock_bus_(t.bustype_, idx);
					break;
				} else if(before && !t.check_->get_check()) {  // チェック OFF
					free_bus_(t.bustype_, idx);
					break;
				}
				++idx;
			}
		}
	};
}
