#pragma once
//=====================================================================//
/*!	@file
	@brief	漢字フォントイメージのインターフェースクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <cmath>
#include "img_io/img_gray8.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	テキストの幅を計算する
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class font_metrics {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの測定基準構造体@n
					・水平基準は、フォントのベースライン@n
					・垂直基準は、フォントの中心
		 */
		//-----------------------------------------------------------------//
		struct metrics {
			float	bitmap_w;	///< フォント・ビットマップの横幅
			float	bitmap_h;	///< フォント・ビットマップの高さ
			float	width;		///< フォントの幅
			float	height;		///< フォントの高さ
			float	hori_x;		///< 水平基準 X 軸オフセット
			float	hori_y;		///< 水平基準 Y 軸オフセット
			float	vert_x;		///< 垂直基準 X 軸オフセット
			float	vert_y;		///< 垂直基準 Y 軸オフセット
		};

	private:
		metrics	m_metrics;

	public:
		font_metrics() { }
		~font_metrics() { }

	};	// font_metrics


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	漢字フォントイメージ(Bitmap)クラス@n
				※FreeType2 ライブラリーを使う。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class Ikfimg {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	仮想デストラクター
		 */
		//-----------------------------------------------------------------//
		virtual ~Ikfimg() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		 */
		//-----------------------------------------------------------------//
		virtual void initialize() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントをインストール
			@param[in]	fontfile	フォント・ファイル名
			@param[in]	alias		フォントの別名@n
						省略した場合、フォント・ファイル名と同一となる。
			@return 成功した場合は「true」
		 */
		//-----------------------------------------------------------------//
		virtual bool install_font_type(const std::string& fontfile, const std::string& alias = "") = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを設定
			@param[in]	alias	フォントの別名
			@return 正常なら「true」
		 */
		//-----------------------------------------------------------------//
		virtual bool set_font(const std::string& alias) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントのフォントを返す
			@return フォントの別名
		 */
		//-----------------------------------------------------------------//
		virtual const std::string& get_font() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの有無を検査
			@param[in]	alias	フォントの別名
			@return フォントがインストール済みの場合は「true」
		 */
		//-----------------------------------------------------------------//
		virtual bool find_font(const std::string& alias) const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントを破棄する
			@param[in]	alias	フォントの別名
		 */
		//-----------------------------------------------------------------//
		virtual bool delete_font(const std::string& alias) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	生成するビットマップのアンチエリアス設定をする
			@param[in]	value	「true」（無し） の場合は、アンチエリアス有効、@n
								「false」 を指定すると、アンチエリアスし無効
		 */
		//-----------------------------------------------------------------//
		virtual void set_antialias(bool value) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	unicode に対応するビットマップを生成する。
			@param[in]	size	生成するビットマップのサイズ
			@param[in]	unicode	生成するビットマップの UNICODE
		 */
		//-----------------------------------------------------------------//
		virtual void create_bitmap(int size, uint32_t unicode) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントのビットマップイメージを得る。
			@return	ビットマップイメージの参照
		 */
		//-----------------------------------------------------------------//
		virtual const img_gray8& get_img() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォントの測定基準（メトリックス）を得る。
			@return	metrics 構造体
		 */
		//-----------------------------------------------------------------//
		virtual const font_metrics::metrics& get_metrics() const = 0;

	};

	void create_kfimg();

	Ikfimg* get_kfimg();

	void destroy_kfimg();

}	// namespace Ikfimg

