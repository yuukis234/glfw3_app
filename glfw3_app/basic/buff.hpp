#pragma once
//=====================================================================//
/*!	@file
	@brief	バッファ・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>
#include <cstring>

namespace utils {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	標準的バッファ操作
		@param[in]	SIZE	バッファ・サイズ
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <uint16_t SIZE>
	class buff {
	public:
		typedef uint16_t	index_type;

	private:
		uint8_t		buff_[SIZE];

		uint16_t	front_;
		uint16_t	back_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		buff() : front_(0), back_(SIZE) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリア・フロント
		*/
		//-----------------------------------------------------------------//
		void clear_front() { front_ = 0; }


		//-----------------------------------------------------------------//
		/*!
			@brief	クリア・バック
		*/
		//-----------------------------------------------------------------//
		void clear_back() { back_ = SIZE; }


		//-----------------------------------------------------------------//
		/*!
			@brief	全クリア
		*/
		//-----------------------------------------------------------------//
		void clear() {
			clear_front();
			clear_back();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フリー・サイズを返す
			@return フリー・サイズ
		*/
		//-----------------------------------------------------------------//
		uint16_t get_free() const { return back_ - front_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フロント・サイズを返す
			@return フロント・サイズ
		*/
		//-----------------------------------------------------------------//
		uint16_t get_front_size() const { return front_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	バック・サイズを返す
			@return バック・サイズ
		*/
		//-----------------------------------------------------------------//
		uint16_t get_back_size() const { return SIZE - back_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フロントへ追加
			@param[in]	src	ブロックの先頭
			@param[in]	len	長さ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool add_front(const void* src, uint16_t len) {
			if((front_ + len) > back_) {
				return false;
			}
			std::memcpy(&buff_[front_], src, len);
			front_ += len;
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	バックへ追加
			@param[in]	src	ブロックの先頭
			@param[in]	len	長さ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool add_back(const void* src, uint16_t len) {
			if((back_ - len) < front_) {
				return false;
			}
			std::memcpy(&buff_[back_ - len], src, len);
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	データの移動
			@param[in]	src	データの移動元
			@param[in]	dst	データの移動先
			@param[in]	len	長さ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool move(uint16_t src, uint16_t dst, uint16_t len) {
			if(src < front_ && (src + len) < back_) {
				std::memmove(&buff_[dst], &buff_[src], len);
				return true;
			} else if(back_ < src && front_ <= (back_ - len)) {
				std::memmove(&buff_[dst], &buff_[src], len);
				return true;
			} else {
				return false;
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ポインターを取得
			@param[in]	pos	バッファの位置
			@return ポインターを取得
		*/
		//-----------------------------------------------------------------//
		const void* get(uint16_t pos) const {
			return &buff_[pos];
		} 


		//-----------------------------------------------------------------//
		/*!
			@brief	8 bits 値を取得
			@param[in]	pos	バッファの位置
			@return 値
		*/
		//-----------------------------------------------------------------//
		uint8_t get8(uint16_t pos) const {
			return buff_[pos];
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	16 bits 値を取得
			@param[in]	pos	バッファの位置
			@return 値
		*/
		//-----------------------------------------------------------------//
		uint16_t get16(uint16_t pos) const {
			uint16_t v = buff_[pos];
			v |= static_cast<uint16_t>(buff_[pos + 1]) << 8;
			return v;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	32 bits 値を取得
			@param[in]	pos	バッファの位置
			@return 値
		*/
		//-----------------------------------------------------------------//
		uint32_t get32(uint16_t pos) const {
			uint32_t v = buff_[pos];
			v |= static_cast<uint32_t>(buff_[pos + 1]) << 8;
			v |= static_cast<uint32_t>(buff_[pos + 2]) << 16;
			v |= static_cast<uint32_t>(buff_[pos + 3]) << 24;
			return v;
		}
	};
}
