//=====================================================================//
/*!	@file
	@brief	PMX ファイルを扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "pmx_io.hpp"
#include "img_io/img_files.hpp"

namespace mdf {

	void pmx_io::initialize_()
	{
	}

	void pmx_io::destroy_()
	{
		if(vtx_id_) {
			glDeleteBuffers(1, &vtx_id_);
			vtx_id_ = 0;
		}

		if(!idx_id_.empty()) {
			glDeleteBuffers(idx_id_.size(), &idx_id_[0]);
			idx_id_.clear();
		}

		if(tex_id_.empty()) {
			glDeleteTextures(tex_id_.size(), &tex_id_[0]);
			tex_id_.clear();
		}
	}


	static bool probe_(utils::file_io& fio)
	{
		std::string s;
		if(fio.get(s, 4) != 4) {
			return false;
		}
		if(s == "PMX ") {
			return true;
		}
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイルが有効か検査
		@return 有効なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmx_io::probe(utils::file_io& fio)
	{
		size_t pos = fio.tell();
		bool f = probe_(fio);
		fio.seek(pos, utils::file_io::seek::set);
		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ロード
		@param[in]	fio	ファイル入出力クラス
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmx_io::load(utils::file_io& fio)
	{
		initialize_();

		current_path_.clear();
		utils::get_file_path(fio.get_path(), current_path_);

		destroy_();

		if(!probe_(fio)) {
			return false;
		}

		if(!fio.get(version_)) {
			return false;
		}

		if(!reading_info_.get(fio)) {
			return false;
		}
///		std::cout << "Appendix UV: " << static_cast<int>(reading_info_.appendix_uv) << std::endl;
///		std::cout << "Text Encodng: " << static_cast<int>(reading_info_.text_encode_type) << std::endl;
///		std::cout << "Bone Index Sizeof: " << static_cast<int>(reading_info_.bone_index_sizeof) << std::endl;

		bool utf16 = (reading_info_.text_encode_type == 0 ? true : false);
		if(!model_info_.get(fio, utf16)) {
			return false;
		}
//		std::cout << model_info_.name << std::endl;
//		std::cout << model_info_.comment << std::endl;

		{  // 頂点データの読み込み
			uint32_t num;
			if(!fio.get(num)) return false;
///			std::cout << "Vertex: " << num << std::endl;
			vertexes_.resize(num);
			for(uint32_t i = 0; i < num; ++i) {
				if(!vertexes_[i].get(fio, reading_info_)) return false;
			}
		}

		{  // 面データの読み込み
			uint32_t num;
			if(!fio.get(num)) return false;
///			std::cout << "Face: " << num << std::endl;
			faces_.resize(num);
			for(uint32_t i = 0; i < num; ++i) {
				if(!get_(fio, reading_info_.vertex_index_sizeof, faces_[i])) return false;
			}
		}

		{  // テクスチャ
			uint32_t num;
			if(!fio.get(num)) return false;
			textures_.resize(num);
///			std::cout << "Texture: " << num << std::endl;
			for(uint32_t i = 0; i < num; ++i) {
				if(!get_text_(fio, textures_[i], reading_info_.text_encode_type)) return false;
			}
		}
		{  // 材質
			uint32_t num;
			if(!fio.get(num)) return false;
///			std::cout << "Material: " << num << std::endl;
			materials_.resize(num);
			for(uint32_t i = 0; i < num; ++i) {
				if(!materials_[i].get(fio, reading_info_)) return false;
			}
		}
		if(0) {  // ボーン
			uint32_t num;
			if(!fio.get(num)) return false;
			std::cout << "Bone: " << num << std::endl;
			bones_.resize(num);
			for(uint32_t i = 0; i < num; ++i) {
				if(!bones_[i].get(fio, reading_info_)) return false;
			}
		}



		return true;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	セーブ
		@param[in]	fio	ファイル入出力クラス
		@return 成功なら「true」
	*/
	//-----------------------------------------------------------------//
	bool pmx_io::save(utils::file_io fio)
	{
		return false;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング・セットアップ
	*/
	//-----------------------------------------------------------------//
	void pmx_io::render_setup()
	{
		if(vertexes_.empty()) return;
		if(faces_.empty()) return;

		{	// 頂点バッファの作成
			std::vector<vbo_t> vbos;
			vbos.resize(vertexes_.size());
			uint32_t i = 0;
			BOOST_FOREACH(pmx_vertex& v, vertexes_) {
				vbo_t& vbo = vbos[i];
				vbo.uv = v.uv_;
				vbo.n = v.normal_;			
				vbo.v = v.position_;
				++i;
			}

			glGenBuffers(1, &vtx_id_);
			glBindBuffer(GL_ARRAY_BUFFER, vtx_id_);
			glBufferData(GL_ARRAY_BUFFER, vbos.size() * sizeof(vbo_t), &vbos[0],
				GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		{ // インデックス・バッファの作成（マテリアル別に作成）
			std::vector<uint32_t> idxes;
			idxes.resize(faces_.size());
			for(uint32_t i = 0; i < (faces_.size() / 3); ++i) {
				idxes[i * 3 + 0] = faces_[i * 3 + 0];
				idxes[i * 3 + 1] = faces_[i * 3 + 2];
				idxes[i * 3 + 2] = faces_[i * 3 + 1];
			}

			idx_id_.resize(materials_.size());
			glGenBuffers(materials_.size(), &idx_id_[0]);

			uint32_t n = 0;
			uint32_t in = 0;
			BOOST_FOREACH(const pmx_material& m, materials_) {
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_id_[n]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.face_num_ * sizeof(uint32_t),
					&idxes[in], GL_STATIC_DRAW);
				in += m.face_num_;
				++n;
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		// テクスチャーの作成と登録
		{
			img::img_files imf;
			imf.initialize();
			tex_id_.resize(textures_.size());
			glGenTextures(tex_id_.size(), &tex_id_[0]);
			uint32_t i = 0;
			BOOST_FOREACH(const std::string& s, textures_) {
				if(imf.load(current_path_ + '/' + s)) {
					const img::i_img* img = imf.get_image_if();
					if(img == 0) continue;

					glBindTexture(GL_TEXTURE_2D, tex_id_[i]);
					int level = 0;
					int border = 0;
					int w = img->get_size().x;
					int h = img->get_size().y;
					glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, w, h, border,
						GL_RGBA, GL_UNSIGNED_BYTE, img->get_image());
				}
				++i;
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	レンダリング
	*/
	//-----------------------------------------------------------------//
	void pmx_io::render_surface()
	{
		if(vertexes_.empty()) return;
		if(faces_.empty()) return;

		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		glScalef(-1.0f, 1.0f, 1.0f);

		glPushMatrix();

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		// 頂点情報をバインド
		glBindBuffer(GL_ARRAY_BUFFER, vtx_id_);
		glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);

		uint32_t n = 0;
		BOOST_FOREACH(const pmx_material& m, materials_) {
			glColor4f(m.diffuse_.r, m.diffuse_.g, m.diffuse_.b, m.diffuse_.a);

			if(m.normal_texture_index_ > 0) {
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, tex_id_[m.normal_texture_index_]);
				GLenum edge;
///				if(m.edge_flag) {
					edge = GL_CLAMP_TO_EDGE;
///				} else {
///					edge = GL_REPEAT;
///				}
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, edge);
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, edge);
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			} else {
				glDisable(GL_TEXTURE_2D);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_id_[n]);
			glDrawElements(GL_TRIANGLES, m.face_num_, GL_UNSIGNED_INT, 0);
			++n;
		}
		glDisable(GL_TEXTURE_2D);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glDisable(GL_CULL_FACE);

		glPopMatrix();
	}

}

