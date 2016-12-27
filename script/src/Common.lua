 
function EnableProperty(obj)
	if obj.__prop or obj.__prop_getter or obj.__prop_setter then
		error("EnableProperty twice");
	end
	obj.__prop = {}
	obj.__prop_getter = {}
	obj.__prop_setter = {}
	
	local data_obj_mt = {}
	data_obj_mt.__index = function(t, k)
		local getter = t.__prop_getter[k];
		if getter then
			return getter(t, k);
		else
			return t.__prop[k];
		end
	end
	data_obj_mt.__newindex = function(t, k, v)
		t.__prop[k] = v;
		local setter = t.__prop_setter[k];
		if setter then
			setter(t, k, v);
		end
	end
	setmetatable(obj, data_obj_mt);
end

function SetPropertySetter(obj, k, fn)
	obj.__prop_setter[k] = fn;
end

--[[
local test = {}

EnableProperty(test);
SetPropertySetter(test, "def", function(t, k, v)
		print("Setter:", t, k, v);
	end)
test.abc = 1;
test.def = 2
]]


local CallbackManager = {}
luaui2.CallbackManager = CallbackManager

function CallbackManager:SetCallback(name, callback)
	if type(callback) ~= 'function' then
		error('callback type error');
	end
	if not self._callbacks then
		self._callbacks = {}
	end
	name = tostring(name);
--	if self._callbacks[name] then
--		error("Callback Exists");
--	end
	self._callbacks[name] = callback;
end

function CallbackManager:InvokeCallback(name, ...)
	if not self._callbacks then
		return;
	end
	local callback = self._callbacks[name];
	return callback(...);
end

function CallbackManager:GetCallback(name)
	if not self._callbacks then
		return nil
	end
	return self._callbacks[name]
end

-- 机智的多继承实现
local function LuaInherit(derived, base)
	for k, v in pairs(base) do
		if type(k) == 'string' then
			derived[k] = v;
		end
	end
end
luaui2.LuaInherit = LuaInherit;

local function CreateInstance(class)
	local self = {}
	self.__index = class
	setmetatable(self, self);
	return self;
end
luaui2.CreateInstance = CreateInstance

local Control = {}
luaui2.Control = Control;

function Control:SetRect(x, y, w, h)
	self:GetSprite():SetRect(x, y, w, h)
end

function Control:GetRect()
	return self:GetSprite():GetRect()
end

function Control:GetSprite()
	assert(false, "pure virtual function_ call");
end

local function PointInRect(x, y, rc)
	return x >= rc.x and x < rc.x + rc.w and y >= rc.y and y < rc.y + rc.h;
end

luaui2.PointInRect = PointInRect;

local function DrawTextureNineInOne(canvas, bmp, offset, width, nine_in_one_info, rc_dst)
	assert(#nine_in_one_info == 4);
	local u1, u2, v1, v2 = unpack(nine_in_one_info);
	u2 = width - u2 - 1;
	local _, height = bmp:GetSize();
	v2 = height - v2 - 1;
	
	-- 左上
	canvas:DrawBitmap(bmp, 
		{x = offset, y = 0, w = u1, h = v1}, -- src
		{x = rc_dst.x, y = rc_dst.y, w = u1, h = v1}) -- dst
	
	-- 中上
	canvas:DrawBitmap(bmp, 
		{x = offset + u1 + 1, y = 0, w = width - u1 - u2 - 2, h = v1}, -- src
		{x = rc_dst.x + u1, y = rc_dst.y, w = rc_dst.w - u1 - u2 + 1, h = v1}) -- dst
	
	-- 右上
	canvas:DrawBitmap(bmp, 
		{x = offset + (width - u2), y = 0, w = u2, h = v1}, -- src
		{x = rc_dst.x + (rc_dst.w - u2), y = rc_dst.y, w = u2, h = v1}) -- dst
	
	-- 左中
	canvas:DrawBitmap(bmp, 
		{x = offset, y = v1 + 1, w = u1, h = height - v1 - v2 - 2}, -- src
		{x = rc_dst.x, y = rc_dst.y + v1, w = u1, h = rc_dst.h - v1 - v2 + 2}) -- dst
	
	-- 中中
	canvas:DrawBitmap(bmp, 
		{x = offset + u1 + 1, y = v1 + 1, w = width - u1 - u2 - 2, h = height - v1 - v2 - 2}, -- src
		{x = rc_dst.x + u1, y = rc_dst.y + v1, w = rc_dst.w - u1 - u2, h = rc_dst.h - v1 - v2}) -- dst

	-- 右中
	canvas:DrawBitmap(bmp, 
		{x = offset + (width - u2), y = v1 + 1, w = u2, h = height - v1 - v2 - 2}, -- src
		{x = rc_dst.x + (rc_dst.w - u2), y = rc_dst.y + v1, w = u2, h = rc_dst.h - v1 - v2}) -- dst
	
	-- 左下
	canvas:DrawBitmap(bmp, 
		{x = offset, y = height - v2 + 1, w = u1, h = v2 - 1}, -- src
		{x = rc_dst.x, y = rc_dst.y + (rc_dst.h - v2) + 1, w = u1, h = v2 - 1}) -- dst
	
	-- 中下
	canvas:DrawBitmap(bmp, 
		{x = offset + u1 + 1, y = height - v2, w = width - u1 - u2 - 2, h = v2}, -- src
		{x = rc_dst.x + u1, y = rc_dst.y + (rc_dst.h - v2), w = rc_dst.w - u1 - u2, h = v2}) -- dst
	
	-- 右下
	canvas:DrawBitmap(bmp, 
		{x = offset + (width - u2), y = height - v2, w = u2, h = v2}, -- src
		{x = rc_dst.x + (rc_dst.w - u2), y = rc_dst.y + (rc_dst.h - v2), w = u2, h = v2}) -- dst
end

luaui2.DrawTextureNineInOne = DrawTextureNineInOne