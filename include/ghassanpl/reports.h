#pragma once

#include "detail/common.h"

#include <system_error>

namespace ghassanpl::err
{
	/// To consider: https://docs.microsoft.com/en-us/windows/win32/wer/about-wer

	using urgency_t = int;

	enum class notification_medium
	{
		audio,
		visual, /// on windows: FLASHW_ALL or FLASHW_TIMERNOFG depending on 'requires_interactive_dismissal'
		message_box,
		toast_notification,
		log_entry,
		system_wide_alert,
	};

	struct notification_properties
	{
		urgency_t urgency{};
		enum_flags<notification_medium> media{};
		bool requires_interactive_dismissal = false; ///< whether or not user interaction is required to dismiss this notification
		std::string_view message;
	};

	bool notify_user(notification_properties const& properties, uri_view target_user_address = {});

	struct system_provided_notification
	{
		std::string_view name;
		notification_properties properties{};
		uintptr_t system_handle{};
	};

	bool notify_user(system_provided_notification const& type, uri_view target_user_address = {});

	namespace operating_system
	{
		enum class common_notification_types
		{
			action_finished_successfully,
			action_finished_unsuccessfully,
			attention_needed,
			confirmation_needed,
			data_modification_warning,
			dangerous_action_warning,
			critical_error,
		};

		std::span<system_provided_notification const> available_notification_types();
		system_provided_notification const& query_notification_type(std::string_view name);
		system_provided_notification const& query_notification_type(common_notification_types common);

		urgency_t query_error_urgency(std::error_code ec);
		urgency_t query_error_urgency(std::error_condition ec);
	}
}