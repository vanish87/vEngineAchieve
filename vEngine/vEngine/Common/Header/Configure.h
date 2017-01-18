#ifndef CONFIGURE_H_
#define CONFIGURE_H_

#pragma once

#include <string>

namespace vEngine
{
	class Configure
	{
	public:
		struct RenderSetting
		{
			bool	full_screen;
			int		left;
			int		top;
			int		width;
			int		height;

			bool	msaa4x;
			bool	gamma;

			bool	deferred_rendering;
			int		gbuffer_size;

		};

		struct ContextSetting
		{
			std::string render_factory_name;
			RenderSetting render_setting;
		};

	public:
		Configure();
		~Configure();

		void LoadConfig(std::string const & cfg_file);
		ContextSetting& GetContextSetting();

	private:
		ContextSetting context_setting_;
	};
}

#endif