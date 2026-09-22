"""CPython/Windows accounting for a completed Popen whose handle is still open.

These counters describe only the requested process, excluding LLVM children.
Keep that scope distinct from POSIX wait4 accounting in reports.
API contracts: https://learn.microsoft.com/windows/win32/api/processthreadsapi/nf-processthreadsapi-getprocesstimes
https://learn.microsoft.com/windows/win32/api/psapi/nf-psapi-getprocessmemoryinfo
"""
import ctypes
from ctypes import wintypes


def completed_process_usage(process):
    class MemoryCounters(ctypes.Structure):
        _fields_ = [('cb', wintypes.DWORD), ('PageFaultCount', wintypes.DWORD)] + [
            (name, ctypes.c_size_t) for name in (
                'PeakWorkingSetSize', 'WorkingSetSize', 'QuotaPeakPagedPoolUsage',
                'QuotaPagedPoolUsage', 'QuotaPeakNonPagedPoolUsage',
                'QuotaNonPagedPoolUsage', 'PagefileUsage', 'PeakPagefileUsage')]

    kernel = ctypes.WinDLL('kernel32', use_last_error=True)
    kernel.GetProcessTimes.argtypes = [wintypes.HANDLE] + [ctypes.POINTER(wintypes.FILETIME)] * 4
    kernel.GetProcessTimes.restype = wintypes.BOOL
    kernel.K32GetProcessMemoryInfo.argtypes = [wintypes.HANDLE, ctypes.POINTER(MemoryCounters), wintypes.DWORD]
    kernel.K32GetProcessMemoryInfo.restype = wintypes.BOOL
    # Popen owns this handle; do not close it here. Querying by PID after wait
    # could instead attach to an unrelated process if Windows recycled the PID.
    handle = wintypes.HANDLE(int(process._handle))
    creation, exit_time, system, user = (wintypes.FILETIME() for _ in range(4))
    if not kernel.GetProcessTimes(handle, ctypes.byref(creation), ctypes.byref(exit_time),
                                  ctypes.byref(system), ctypes.byref(user)):
        raise ctypes.WinError(ctypes.get_last_error())
    memory = MemoryCounters()
    memory.cb = ctypes.sizeof(memory)
    if not kernel.K32GetProcessMemoryInfo(handle, ctypes.byref(memory), memory.cb):
        raise ctypes.WinError(ctypes.get_last_error())
    if not memory.PeakWorkingSetSize:
        raise RuntimeError('Windows returned no peak working set; accounting is unavailable')

    def seconds(value):
        return ((value.dwHighDateTime << 32) | value.dwLowDateTime) / 10_000_000

    return {'user_seconds': seconds(user), 'system_seconds': seconds(system),
            'max_process_rss_bytes': memory.PeakWorkingSetSize,
            'accounting_scope': 'compiler-process-only'}
