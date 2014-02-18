//=====================================================================//
/*!	@file
	@brief	キーボード入力を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/keyboard.hpp"
#include "gl_fw/IGLcore.hpp"
#include <boost/foreach.hpp>

namespace sys {

	struct key_t {
		gl::device::key::type	key_type;
		char	normal_code;
		char	shift_code;
	};

	static const key_t us_key_type_tbls_[] = {
		{ gl::device::key::ESCAPE,        0x1b, 0x1b },
		{ gl::device::key::GRAVE_ACCENT,  '`', '~' },
		{ gl::device::key::_1,            '1', '!' },
		{ gl::device::key::_2,            '2', '@' },
		{ gl::device::key::_3,            '3', '#' },
		{ gl::device::key::_4,            '4', '$' },
		{ gl::device::key::_5,            '5', '%' },
		{ gl::device::key::_6,            '6', '^' },
		{ gl::device::key::_7,            '7', '&' },
		{ gl::device::key::_8,            '8', '*' },
		{ gl::device::key::_9,            '9', '(' },
		{ gl::device::key::_0,            '0', ')' },
		{ gl::device::key::MINUS,         '-', '_' },
		{ gl::device::key::EQUAL,         '=', '+' },
		{ gl::device::key::BACKSPACE,     0x08, 0x08 },
		{ gl::device::key::DELETE,        0x3f, 0x3f },
		{ gl::device::key::TAB,           0x09, 0x09 },
		{ gl::device::key::Q,             'Q', 'q' },
		{ gl::device::key::W,             'W', 'w' },
		{ gl::device::key::E,             'E', 'e' },
		{ gl::device::key::R,             'R', 'r' },
		{ gl::device::key::T,             'T', 't' },
		{ gl::device::key::Y,             'Y', 'y' },
		{ gl::device::key::U,             'U', 'u' },
		{ gl::device::key::I,             'I', 'i' },
		{ gl::device::key::O,             'O', 'o' },
		{ gl::device::key::P,             'P', 'p' },
		{ gl::device::key::LEFT_BRACKET,  '[', '{' },
		{ gl::device::key::RIGHT_BRACKET, ']', '}' },
		{ gl::device::key::BACKSLASH,     '\\', '|' },

		{ gl::device::key::A,             'A', 'a' },
		{ gl::device::key::S,             'S', 's' },
		{ gl::device::key::D,             'D', 'd' },
		{ gl::device::key::F,             'F', 'f' },
		{ gl::device::key::G,             'G', 'g' },
		{ gl::device::key::H,             'H', 'h' },
		{ gl::device::key::J,             'J', 'j' },
		{ gl::device::key::K,             'K', 'k' },
		{ gl::device::key::L,             'L', 'l' },
		{ gl::device::key::SEMICOLON,     ';', ':' },
		{ gl::device::key::APOSTROPHE,    '\'', '"' },
		{ gl::device::key::ENTER,         0x0d, 0x0d },

		{ gl::device::key::Z,             'Z', 'z' },
		{ gl::device::key::X,             'X', 'x' },
		{ gl::device::key::C,             'C', 'c' },
		{ gl::device::key::V,             'V', 'v' },
		{ gl::device::key::B,             'B', 'b' },
		{ gl::device::key::N,             'N', 'n' },
		{ gl::device::key::M,             'M', 'm' },
		{ gl::device::key::COMMA,         ',', '<' },
		{ gl::device::key::PERIOD,        '.', '>' },
		{ gl::device::key::SLASH,         '/', '?' },
		{ gl::device::key::SPACE,         ' ', ' ' }
	};

	static const key_t jp_key_type_tbls_[] = {
		{ gl::device::key::ESCAPE,        0x1b, 0x1b },
//		{ gl::device::key::WORLD_1,       'a', 'a' },
		{ gl::device::key::_1,            '1', '!' },
		{ gl::device::key::_2,            '2', '"' },
		{ gl::device::key::_3,            '3', '#' },
		{ gl::device::key::_4,            '4', '$' },
		{ gl::device::key::_5,            '5', '%' },
		{ gl::device::key::_6,            '6', '&' },
		{ gl::device::key::_7,            '7', '\'' },
		{ gl::device::key::_8,            '8', '(' },
		{ gl::device::key::_9,            '9', ')' },
		{ gl::device::key::_0,            '0', '0' },
		{ gl::device::key::MINUS,         '-', '=' },
		{ gl::device::key::EQUAL,         ';', '+' },
		{ gl::device::key::BACKSPACE,     0x08, 0x08 },
		{ gl::device::key::DELETE,        0x3f, 0x3f },

		{ gl::device::key::TAB,           0x09, 0x09 },
		{ gl::device::key::Q,             'Q', 'q' },
		{ gl::device::key::W,             'W', 'w' },
		{ gl::device::key::E,             'E', 'e' },
		{ gl::device::key::R,             'R', 'r' },
		{ gl::device::key::T,             'T', 't' },
		{ gl::device::key::Y,             'Y', 'y' },
		{ gl::device::key::U,             'U', 'u' },
		{ gl::device::key::I,             'I', 'i' },
		{ gl::device::key::O,             'O', 'o' },
		{ gl::device::key::P,             'P', 'p' },
		{ gl::device::key::GRAVE_ACCENT,  '@', '`' },
		{ gl::device::key::LEFT_BRACKET,  '[', '{' },
		{ gl::device::key::RIGHT_BRACKET, ']', '}' },
		{ gl::device::key::BACKSLASH,     '\\', '|' },

		{ gl::device::key::A,             'A', 'a' },
		{ gl::device::key::S,             'S', 's' },
		{ gl::device::key::D,             'D', 'd' },
		{ gl::device::key::F,             'F', 'f' },
		{ gl::device::key::G,             'G', 'g' },
		{ gl::device::key::H,             'H', 'h' },
		{ gl::device::key::J,             'J', 'j' },
		{ gl::device::key::K,             'K', 'k' },
		{ gl::device::key::L,             'L', 'l' },
		{ gl::device::key::SEMICOLON,     ':', '*' },
		{ gl::device::key::APOSTROPHE,    '^', '~' },
		{ gl::device::key::ENTER,         0x0d, 0x0d },

		{ gl::device::key::Z,             'Z', 'z' },
		{ gl::device::key::X,             'X', 'x' },
		{ gl::device::key::C,             'C', 'c' },
		{ gl::device::key::V,             'V', 'v' },
		{ gl::device::key::B,             'B', 'b' },
		{ gl::device::key::N,             'N', 'n' },
		{ gl::device::key::M,             'M', 'm' },
		{ gl::device::key::COMMA,         ',', '<' },
		{ gl::device::key::PERIOD,        '.', '>' },
		{ gl::device::key::SLASH,         '/', '?' },
		{ gl::device::key::WORLD_2,       '\\', '_' },
		{ gl::device::key::SPACE,         ' ', ' ' }
	};

#if 0
				INSERT    = 260,
				RIGHT     = 262,
				LEFT      = 263,
				DOWN      = 264,
				UP        = 265,
				PAGE_UP   = 266,
				PAGE_DOWN = 267,
				HOME      = 268,
				END       = 269,

				CAPS_LOCK    = 280,
				SCROLL_LOCK  = 281,
				NUM_LOCK     = 282,
				PRINT_SCREEN = 283,
				PAUSE        = 284,

				LEFT_SHIFT    = 340,
				LEFT_CONTROL,
				LEFT_ALT,
				LEFT_SUPER,
				RIGHT_SHIFT,
				RIGHT_CONTROL,
				RIGHT_ALT,
				RIGHT_SUPER,
				SHIFT,
				CONTROL,
				ALT,
				SUPER,
				MENU,
#endif


	//-----------------------------------------------------------------//
	/*!
		@brief	サービス
	*/
	//-----------------------------------------------------------------//
	void keyboard::service()
	{
		using namespace gl;

		IGLcore* igl = get_glcore();
		const device& dev = igl->get_device();

		input_.clear();
		bool caps = dev.get_level(device::key::STATE_CAPS_LOCK);
		const key_t* tbl;
		int n;
		if(igl->keyboard_japan()) {
			tbl = jp_key_type_tbls_;
			n = sizeof(jp_key_type_tbls_) / sizeof(key_t);
		} else {
			tbl = us_key_type_tbls_;
			n = sizeof(us_key_type_tbls_) / sizeof(key_t);
		}
		for(int i = 0; i < n; ++i) {
			const key_t& t = tbl[i];
			if(dev.get_positive(t.key_type)) {
				char nc = t.normal_code;
				bool f = dev.get_level(device::key::SHIFT);
				if(nc >= 'A' && nc <= 'Z') {
					if(caps) f = !f;
					if(f) input_ += nc;
					else input_ += t.shift_code;
				} else {
					if(!f) input_ += nc;
					else input_ += t.shift_code;
				}
			}
		}

	}

}
