//=====================================================================//
/*! @file
	@brief  Space Invader クラス
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "utils/i_scene.hpp"
#include "utils/director.hpp"
#include "side/arcade.h"
#include "gl_fw/gltexfb.hpp"

namespace app {

	class spinv : public utils::i_scene {

		utils::director<core>&	director_;

		int					ships_;
		int					n_easy_;

		InvadersMachine		spinv_;

		std::vector<char>	rom_;
		std::vector<uint32_t>	scan_line_color_;
		std::vector<uint32_t>	fb_;

		gl::texfb			texfb_;

		uint32_t			se_id_[9];

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		spinv(utils::director<core>& d) : director_(d),
			ships_(3), n_easy_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~spinv() { }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief  アップデート
		*/
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief  レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief  廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};

}
