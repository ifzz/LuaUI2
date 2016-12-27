-- FFI bounder check 

local ffi = require("ffi")

function CreateDwordArray(size)
	local m_array = ffi.new("unsigned long[?]", size)
	local obj = {}
	obj.GetAt = function(idx)
		assert(idx >= 0 and idx < size, "out_of_range")
		return m_array[idx];
	end
	obj.SetAt = function(idx, n)
		assert(type(n) == 'number', 'type_error')
		assert(idx >= 0 and idx < size, "out_of_range")
		m_array[idx] = n;
	end
	return obj;
end
	
--[[
local array = CreateDwordArray(1024 * 1024);
for i = 0, 1024 * 1024 - 1 do
	array.SetAt(i, i);
end

print(array.GetAt(1024 * 1024 - 1));

function CreatePtrArray(type_, size)
	local array_t = ffi.cdef("$ **", type_);
	
end
]]

local kernel32 = ffi.load("kernel32");

ffi.cdef[[
typedef uint32_t UINT;
typedef uint32_t DWORD;
typedef uint16_t WCHAR;
typedef void * HANDLE;
typedef const char * LPCSTR;
typedef uint16_t * LPWSTR;
typedef char * LPSTR;
typedef const uint16_t * LPCWSTR;
typedef uint32_t BOOL;
typedef uint32_t * LPBOOL;

int MultiByteToWideChar(UINT CodePage,
    DWORD    dwFlags,
    LPCSTR   lpMultiByteStr, int cbMultiByte,
    LPWSTR  lpWideCharStr, int cchWideChar);

int WideCharToMultiByte(UINT CodePage,
    DWORD    dwFlags,
	LPCWSTR  lpWideCharStr, int cchWideChar,
    LPSTR   lpMultiByteStr, int cbMultiByte,
    LPCSTR   lpDefaultChar,
    LPBOOL  lpUsedDefaultChar);

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
	
typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    WCHAR  cFileName[ 260 ];
    WCHAR  cAlternateFileName[ 14 ];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

HANDLE
FindFirstFileW(
    LPCWSTR lpFileName,
    LPWIN32_FIND_DATAW lpFindFileData
    );
	
BOOL FindNextFileW(
    HANDLE hFindFile,
    LPWIN32_FIND_DATAW lpFindFileData
);

BOOL FindClose(
    HANDLE hFindFile
);

]]

local C = ffi.C;

function CreateApiBind()
	function MultiByteToWideChar(mbcs, codepage)
		assert(type(mbcs)=='string');
		assert(type(codepage)=='number');
		local size_buffer = C.MultiByteToWideChar(codepage, 0, mbcs, #mbcs, nil, 0);
		-- print('size_buffer:', size_buffer, '#mbcs:', #mbcs);
		if size_buffer <= 0 then -- ? 空字符串怎么处理
			return nil
		end
		local buffer = ffi.new("uint16_t[?]", size_buffer + 1); -- 结尾问题还是要好好看MSDN
		local size_written = C.MultiByteToWideChar(codepage, 0, mbcs, #mbcs, buffer, size_buffer);
		-- print('size_written:', size_written);
		return ffi.cast("void *", buffer), size_written; -- 这样就无法下标访问了 比较安全一点 
		-- return buffer, size_written;
	end

	function WideCharToMultiByte(unicode, codepage)
		assert(type(unicode)=='cdata');
		assert(type(codepage)=='number');
		local size_buffer = C.WideCharToMultiByte(codepage, 0, unicode, -1, nil, 0, nil, nil);
		-- print('size_buffer:', size_buffer);
		if size_buffer <= 0 then -- ? 空字符串怎么处理
			return nil
		end
		local buffer = ffi.new("uint8_t[?]", size_buffer + 1); -- 结尾问题还是要好好看MSDN
		local size_written = C.WideCharToMultiByte(codepage, 0, unicode, -1, buffer, size_buffer, nil, nil);
		-- print('size_written:', size_written);
		return ffi.string(buffer, size_written);
	end

	CP_ACP = 0;
	CP_UTF8 = 65001;
	INVALID_HANDLE_VALUE = ffi.cast("intptr_t", -1)

	local str_utf8 = "大家好我是luajit我喜欢用ffi"
	-- print('str_utf8:', str_utf8);
	local utf16, length = MultiByteToWideChar(str_utf8, CP_UTF8);
	-- print("utf16 begin")
	-- for i = 0, 9999 do
		-- print(utf16[i]);
		-- utf16[i] = 0; -- 必崩溃啊 亲 lua写出能崩溃的代码 要命啊
	-- end
	-- print("utf16 end")
	local str_gbk = WideCharToMultiByte(utf16, 936);
	print('str_gbk:', str_gbk, '#str_gbk:', #str_gbk);

	function FindDataToTable(wfd)
		local data = {}
		data.path = WideCharToMultiByte(wfd.cFileName, 936);
		if bit.band(wfd.dwFileAttributes, 0x10) ~= 0 then
			data.is_dir = true;
		end
		return data;
	end

	function ListDirFiles(dir_path)
		local wfd = ffi.new("WIN32_FIND_DATAW");
		local list = {};
		local hFind = C.FindFirstFileW(MultiByteToWideChar(dir_path, CP_UTF8), wfd);
		if hFind == INVALID_HANDLE_VALUE then
			return nil;
		end
		local data = FindDataToTable(wfd);
		table.insert(list, data);
		while(C.FindNextFileW(hFind, wfd) ~= 0) do
			data = FindDataToTable(wfd);
			table.insert(list, data);
		end
		C.FindClose(hFind);
		return list;
	end
	
	local ApiBind = {}
	ApiBind.ListDirFiles = ListDirFiles;
	return ApiBind;
end

--[[
local api = CreateApiBind()

local list = api.ListDirFiles("D:\\*");
for _, entry in ipairs(list) do
	print(entry.is_dir and '<DIR>' or '', entry.path);
end
]]

local LuaCmd = ffi.load([[E:\myworks\LuaCmd\Debug\LuaCmd.dll]]);

ffi.cdef([[
struct LuaCmdInputParam
{
	const char *title;
	const char *prompt;
	const char *input;
};
int LuaCmdInputBox(struct LuaCmdInputParam *param);

void LuaCmdFree(void *p);

]])

local param = ffi.new("struct LuaCmdInputParam");
param.title = "你好啊";
param.prompt = "这是一个输入框";
local ret = LuaCmd.LuaCmdInputBox(param);
print('LuaCmdInputBox', ret, ffi.string(param.input));
LuaCmd.LuaCmdFree(ffi.cast("void *", param.input));