#pragma once
//=====================================================================//
/*!	@file
  @brief	FIFO (first in first out)
  @author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <cstdint>

namespace utils {

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    /*!
        @brief  fifo クラス
		@param[in]	size	バッファサイズ
	*/
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	template <class T, uint32_t size_ = 256>
	class fifo {

		volatile uint32_t	get_;
		volatile uint32_t	put_;

		T	buff_[size_];

	public:
        //-----------------------------------------------------------------//
        /*!
            @brief  コンストラクター
		*/
        //-----------------------------------------------------------------//
		fifo() : get_(0), put_(0) { }


        //-----------------------------------------------------------------//
        /*!
            @brief  クリア
		*/
        //-----------------------------------------------------------------//
		void clear() { get_ = put_ = 0; }


        //-----------------------------------------------------------------//
        /*!
            @brief  値の格納
			@param[in]	v	値
		*/
        //-----------------------------------------------------------------//
		void put(const T& v) {
			buff_[put_] = v;
			++put_;
			if(put_ >= size_) {
				put_ = 0;
			}
		}


        //-----------------------------------------------------------------//
        /*!
            @brief  先頭の値取得
			@return	値
		*/
        //-----------------------------------------------------------------//
		const T& top() const { return buff_[get_]; }


        //-----------------------------------------------------------------//
        /*!
            @brief  値の取得
			@return	値
		*/
        //-----------------------------------------------------------------//
		const T& get() {
			uint32_t pos = get_;
			++get_;
			if(get_ >= size_) {
				get_ = 0;
			}
			return buff_[pos];
		}


        //-----------------------------------------------------------------//
        /*!
            @brief  長さを返す
			@return	長さ
		*/
        //-----------------------------------------------------------------//
		uint32_t length() const {
			if(put_ >= get_) return (put_ - get_);
			else return (size_ + put_ - get_);
		}


        //-----------------------------------------------------------------//
        /*!
            @brief  get 位置を返す
			@return	位置
		*/
        //-----------------------------------------------------------------//
		uint32_t pos_get() const { return get_; }


        //-----------------------------------------------------------------//
        /*!
            @brief  put 位置を返す
			@return	位置
		*/
        //-----------------------------------------------------------------//
		uint32_t pos_put() const { return put_; }


        //-----------------------------------------------------------------//
        /*!
            @brief  バッファのサイズを返す
			@return	バッファのサイズ
		*/
        //-----------------------------------------------------------------//
		uint32_t size() const { return size_; }

	};

}
