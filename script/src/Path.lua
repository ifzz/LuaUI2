local Path = {}
luaui2.Path = Path;

function Path:GetExeDir()
	local path = winapi.GetModuleFileName();
	return self:RemoveSlash(string.match(path, ".*\\"));
end

-- TODO 这个没测过
function Path:GetFileName(path)
	return string.match(path, ".*/(.*)");
end

function Path:UpOneLevel(path)
	path = self:RemoveSlash(path);
	-- print("UpOneLevel RemoveSlash", path)
	path = string.match(path, ".*\\");
	-- print("UpOneLevel match", path)
	return self:RemoveSlash(path);
end

function Path:RemoveSlash(path)
	if path:byte(#path) == 92 then -- \
		-- print("RemoveSlash ok")
		return path:sub(1, -2);
	else
		return path;
	end
end

function Path:GetAppData()
	local path = winapi.SHGetFolderPath()
	return self:RemoveSlash(path);
end