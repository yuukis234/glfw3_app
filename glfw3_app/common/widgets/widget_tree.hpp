#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_tree クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_check.hpp"
#include "utils/tree_unit.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_tree クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_tree : public widget {

		typedef widget_tree value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_tree パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			color_param	color_param_;	///< カラー・パラメーター

			int			height_;		///< ユニットの高さ

			bool		single_;		///< シングル選択の場合「true」

			param() :
				color_param_(widget_director::default_tree_color_),
				height_(28),
				single_(true)
			{ }
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	ツリー・データベースのユニット
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct value {
			widget_check*	path_;
			std::string		data_;
			bool			open_;

			value() : path_(0), data_(), open_(false)
			{ }
			void list_all() const {
				std::cout << "(" << open_ << "): '" << data_ << "'";
			}
		};
		typedef utils::tree_unit<value>	tree_unit;

	private:
		widget_director&	wd_;

		param				param_;

		tree_unit			tree_unit_;
		uint32_t			serial_id_;
		uint32_t			unit_num_;

		vtx::fpos			position_;

		uint32_t			select_id_;
		tree_unit::unit_map_it	select_it_;

		void create_();
		void destroy_();

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_tree(widget_director& wd, const widget::param& bp, const param& p) :
			wd_(wd), widget(bp), param_(p),
			tree_unit_(), serial_id_(0), unit_num_(0),
			position_(0.0f), select_id_(0), select_it_()
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_tree() { destroy_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const { return "tree"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const { return true; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(ro)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	有効・無効の設定
			@param[in]	f	無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) { wd_.enable(this, f, true); }


		//-----------------------------------------------------------------//
		/*!
			@brief	ツリー構造の参照
			@return ツリー構造
		*/
		//-----------------------------------------------------------------//
		tree_unit& at_tree_unit() { return tree_unit_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択 ID を取得（メニューが選択されたら、ID が進む）
			@return 選択 ID
		*/
		//-----------------------------------------------------------------//
		uint32_t get_select_id() const { return select_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	選択メニューを取得
			@return 選択メニュー
		*/
		//-----------------------------------------------------------------//
		tree_unit::unit_map_it get_select_it() const { return select_it_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre);


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre);
	};

}
