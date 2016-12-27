local EventSource = {}
luaui2.EventSource = EventSource

function EventSource:new()
	local self = {}
	self.__index = EventSource;
	setmetatable(self, self);
	return self;
end

function EventSource:RegisterEvent(name)
	assert(type(name) == 'string');
	if type(self._mapEvents[name]) == 'table' then
		return;
	else
		self._mapEvents[name] = {}
	end
end

function EventSource:AttachListener(name, callback)
	assert(type(name) == 'string');
	assert(type(callback) == 'function');
	if type(self._mapEvents[name]) ~= 'table' then
		error('Unregisted event name:'..name);
	else
		local list = self._mapEvents[name];
		table.insert(list, callback);
	end
end

function EventSource:FireEvent(name, ...)
	assert(type(name) == 'string');
	if type(self._mapEvents[name]) ~= 'table' then
		error('Unregisted event name:'..name);
	else
		local list = self._mapEvents[name];
		local ret;
		for _, callback in ipairs(list) do
			ret = table.pack(callback(...)); -- 结果为最后一次的结果
			-- 可不可以不关心结果?
		end
		if ret then
			return table.unpack(ret);
		end
	end
end