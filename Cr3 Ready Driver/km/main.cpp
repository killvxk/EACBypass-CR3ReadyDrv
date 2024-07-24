#include "utils.hpp"
#define nt_qword_sig "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x38\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x38\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x28\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x06\xFF\x15\x00\x00\x00\x00\x48\x83\xC4\x28\xC3\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x48\x83\xEC\x38"
#define nt_qword_mask "xxx????xxxxxxx????xxxxxxxxxxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxxxxxxxxxxxx????xxxxxxx????xxxxxxxxxxxxxxxxxx"

__int64 request_handler(uintptr_t request)
{
	PDRIVER_REQUEST in = reinterpret_cast<PDRIVER_REQUEST>(request);
	switch (in->type)
	{
		case BASE:
		{
			in->base = utils::get_base_address(in);
			break;
		}
		case WRITE:
		{
			utils::write_process_memory(in);
			break;
		}
		case READ:
		{
			utils::read_process_memory(in);
			break;
		}
	}
	return 0;
}

NTSTATUS driver_entry()
{
	const uintptr_t win32k = utils::get_kernel_module("win32k.sys");
	if (!win32k) return STATUS_FAILED_dexzsdriver_ENTRY;
	uintptr_t nt_qword = utils::pattern_scan(win32k, nt_qword_sig, nt_qword_mask);
	if (!nt_qword) return STATUS_FAILED_dexzsdriver_ENTRY;
	uintptr_t nt_qword_deref = nt_qword + *(int*)((PUCHAR)nt_qword + 3) + 7;
	*reinterpret_cast<uintptr_t*>(nt_qword_deref) = reinterpret_cast<uintptr_t>(&request_handler);
	return STATUS_SUCCESS;
}