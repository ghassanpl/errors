#pragma once

#include "detail/common.h"

#include <filesystem>

namespace ghassanpl::err
{
	/// https://docs.microsoft.com/en-us/windows/win32/api/minidumpapiset/nf-minidumpapiset-minidumpwritedump
	/// https://docs.microsoft.com/en-us/previous-versions/windows/desktop/proc_snap/export-a-process-snapshot-to-a-file

	std::filesystem::path perform_simple_process_dump(std::filesystem::path requested_dump_directory);

	/// Returns an address for the symbol information and other debug data for the current module.
	/// This will usually be the module itself or the path to a PDB file.
	uri symbol_information_for_module();

	/// https://docs.microsoft.com/en-us/windows/win32/api/processsnapshot/nf-processsnapshot-psscapturesnapshot
	info_source& process_snapshot_source();
}