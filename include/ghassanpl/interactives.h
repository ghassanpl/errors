#pragma once

#include "detail/common.h"

namespace ghassanpl::err
{

	struct interactive_session
	{
		/// a bidirectional command-and-control pipe over which the user can send commands and receive responses
		/// can be out ouf band or in-band
	};

	namespace debugger
	{
		struct debugger_connection
		{
			bool valid() const;
			bool ready() const;

			bool disconnect();

			uri address() const;

			std::string debugger_name();

			void request_break();
			void output_debug_mesage(std::string_view message);
			void request_command(std::string_view command);
		};

		debugger_connection under_debugger();

		debugger_connection request_system_debugger();

		/// https://github.com/CarloWood/libcwd/blob/8d866ee338fd398bf917e12730fa87f1ae184f32/utils/attach_gdb.cc
		debugger_connection connect_to_debugger(uri_view address);

		struct debugger_session_listener
		{
			debugger_session_listener(uri_view listen_address);
			
			bool is_started() const noexcept;
			void stop();

			debugger_connection debugger() const;

			~debugger_session_listener();
		};
	}

}