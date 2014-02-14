//=====================================================================//
/*! @file
	@brief  メイン
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"
#include "spinv.hpp"
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "gl_fw/glutils.hpp"

int main(int argc, char** argv);

int main(int argc, char** argv)
{
	gl::create_glcore();

	gl::IGLcore* igl = gl::get_glcore();

	// カレントパスを生成
	std::string tmp;
	utils::convert_delimiter(argv[0], '\\', '/', tmp);
	std::string pref;
	utils::get_file_base(tmp, pref);
	pref += ".pre";
	std::string path;
	utils::get_file_path(tmp, path);

	if(!igl->initialize(path)) {
		return -1;
	}

	{
		utils::director<app::core> director;

		director.at().preference_.load(pref);

		vtx::ipos locate(10, 50);
		director.at().preference_.get_position("/window/locate", locate);
//		vtx::ipos size(800, 600);
//		vtx::spos lsz = size;
//		director.at().preference_.get_position("player/window/size", size);

		if(!igl->setup(vtx::srect(locate, vtx::spos(512)), "Space Invader", false)) {
			return -1;
		}
		igl->set_limit_size(vtx::spos(512, 512));

		director.at().sound_.initialize(16);

		director.at().widget_director_.initialize();

		director.install_scene<app::spinv>();

		while(!igl->get_exit_signal()) {
			igl->service();

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			gl::glColor(img::rgbaf(1.0f));

//	gl::IGLcore* igl = gl::get_glcore();
//	const vtx::spos& size = igl->get_size();
//	core.mobj_bg_.setup_matrix(-size.x / 2, -size.y / 2, size.x / 2, size.y / 2, -1.0f, 1.0f);

			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glEnable(GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			director.render();

			igl->flip_frame();

			director.at().sound_.service();
		}
		// プログラム終了の廃棄
		director.erase_scene();
		director.render();

//		vtx::ipos p = igl->get_size();
//		director.at().preference_.put_position("player/window/size", p);
		const vtx::ipos& p = igl->get_locate();
		director.at().preference_.put_position("/window/locate", p);

		director.at().preference_.save(pref);
	}

	igl->destroy();

	gl::destroy_glcore();

	return 0;
}
