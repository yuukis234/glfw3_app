#pragma once
//=====================================================================//
/*!	@file
	@brief	シーン・インターフェース
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//

namespace utils {

	class i_scene {
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	仮想デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~i_scene() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		 */
		//-----------------------------------------------------------------//
		virtual void initialize() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		 */
		//-----------------------------------------------------------------//
		virtual void update() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		 */
		//-----------------------------------------------------------------//
		virtual void render() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄
		 */
		//-----------------------------------------------------------------//
		virtual void destroy() = 0;
	};
}
