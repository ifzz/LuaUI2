local ResourceManager = {}
luaui2.ResourceManager = ResourceManager

function ResourceManager:Init()
--	local self = luaui2.CreateInstance(ResourceManager);
	if self._bmp_map ~= nil then
		print("init more than once");
		return;
	end
	self._bmp_map = {}
	local mt = {}
	mt.__mode = 'kv';
	setmetatable(self._bmp_map, mt);
--	return self;
end

function ResourceManager:GetBitmap(path)
	local bmp = self._bmp_map[path]
	if bmp ~= nil then
		return bmp;
	end
	bmp = luaui2.CreateBitmap();
	if bmp:LoadFromFile(path) then
		self._bmp_map[path] = bmp;
		return bmp;
	else
		return nil;
	end
end

ResourceManager:Init();