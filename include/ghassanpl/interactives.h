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
		struct debugger_connection_t;

		using debugger_connection = debugger_connection_t*;

		bool valid_connection(debugger_connection connection);
		bool fully_connected(debugger_connection connection);
		bool disconnect(debugger_connection connection);

		debugger_connection request_system_debugger();

		debugger_connection connect_to_debugger(uri_view address);

		debugger_connection under_debugger();

		void request_break(debugger_connection connection = {});

		void output_debug_mesage(std::string_view message, debugger_connection connection = {});

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