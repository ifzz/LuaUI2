
package.cpath = "E:\\myworks\\LuaUI2\\Debug\\?.dll;E:\\Study\\LuaUI2\\Debug\\?.dll"

print (package.cpath)

require("luaui2")
--local drivers = WinApi.GetLogicalDrivers()
--for key, val in ipairs(drivers) do
--  print(val)
--end xx
print("--- require luaui2 ---")

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

local Rect = {}
luaui2.Rect = Rect;

function Rect:new(x, y, w, h)
	local self = {}
	self.__index = Rect;
	setmetatable(self,self);
	self.x = x;
	self.y = y;
	self.w = w;
	self.h = h;
	return self;
end

function Rect:PtInRect(x, y)
	return x > self.x and x < self.x + self.w and y > self.y and y < self.y + self.h;
end

function Rect:Unpack()
	return self.x, self.y, self.w, self.h;
end

function SListInsert(root, node)
	if root then
		node.next = root;
	end
	return node;
end
local slist = {data = 1}
slist = SListInsert(slist, {data = 2})
slist = SListInsert(slist, {data = 3})

function LuaInherit(derived, base)
	for k, v in pairs(base) do
		if type(key) == 'string' then
			derived[k] = v;
		end
	end
end

local IControl = {}
luaui2.IControl = IControl;

function IControl:SetRect(x, y, w, h)
	assert(false, "pure virtual function_ call");
end

function IControl:GetRect(x, y, w, h)
	assert(false, "pure virtual function_ call");
end

function IControl:GetRootSprite()
	assert(false, "pure virtual function_ call");
end

local Button = {}
luaui2.Button = Button

function Button:new()
	local self = {}
	self.__index = Button;
	setmetatable(self, self);
	self._root = luaui2.CreateRectangleSprite();
	self._root:SetColor(245, 245, 240);
	self._root:SetBorderColor(150, 150, 150);
	self._text = luaui2.CreateTextSprite();
	self._text:SetColor(55, 55, 55);
	self._text:SetFont("微软雅黑", 12, 0);
	self._root:AddChild(self._text);
	self._root:SetEventHandler("OnLButtonDwon", function(x, y, flag)
			self:OnLButtonDwon(x, y);
		end);
	return self;
end

function Button:OnLButtonDwon(x, y)
	self._old_pos = {} -- 这样甚至 GetTable() 都是不需要的 lua太强大了 upvalue带self
end

function Button:SetRect(x, y, w, h)
	self._root:SetRect(x, y, w, h)
	self._text:SetRect(0, 0, w, h);
end

function Button:GetRect(x, y, w, h)
	return self._root:GetRect();
end

function Button:GetRootSprite()
	return self._root; -- TODO 怎么通过userdata得到Button 是不是需要一个SetTable ... OMG SetUserData 名字很重要 想想
	-- SetTable 的时候要发一个消息 能让lua绑定的 里面打一个StackTrace 这样好找在哪里修改的 OnTableChange(old, new)
	-- 或者也可以在c++里打印 (感觉是方便点 但可能会导致log爆炸)
end

function InitUI()
	g_hostWnd = luaui2.CreateHostWindow(); -- 这尼玛不搞个全局引用 这个函数一返回 就可能会被辣鸡回收 这个最好在c++层面做一下
	local mt = getmetatable(g_hostWnd);
	--mt.__gc = nil; -- 这尼玛真危险 太口怕了 lua这么坑人 还得在c++里防御一下 用 __metatable
	local spRoot = luaui2.CreateSprite();
	g_hostWnd:AttachSprite(spRoot);
	spRoot:SetRect(0, 0 , 100, 100);
	local spRect = luaui2.CreateRectangleSprite();
	spRect:SetRect(20, 20 , 80, 30);
	spRect:SetColor(245, 245, 240);
	spRect:SetBorderColor(150, 150, 150);
	local spText = luaui2.CreateTextSprite();
	local _, _, w, h = spRect:GetRect()
	spText:SetRect(0, 0, w, h);
	spText:SetText("Hello");
	spText:SetColor(55, 55, 55);
	spText:SetFont("微软雅黑", 12, 0);
	spRect:AddChild(spText);
	local spBackground = luaui2.CreateRectangleSprite();
	spBackground:SetRect(0, 0, 1000, 1000);
	spBackground:SetColor(255, 255, 255);
	spBackground:SetBorderColor(0, 0, 0);

	spRoot:AddChild(spBackground);
	spRoot:AddChild(spRect);


	g_hostWnd:Create();

	local rcButton = Rect:new(spRect:GetRect());
	g_hostWnd:SetEventHandler("OnDestroy", function()
			print("OnDestroy")
			luaui2.PostQuitMessage(0);
		end)
	g_hostWnd:SetEventHandler("OnSize", function(cx, cy, flag)
			print('OnSize', cx, cy, flag);
			spBackground:SetRect(0, 0, cx - 1, cy - 1);
			local _, _, w, h = spRect:GetRect()
			local margin = 10;
			spRect:SetRect(cx - w - margin, cy - h -margin, w, h)
		end)
	spRect:SetEventHandler("OnLButtonDwon", function(x, y, flag)
			-- print('text', spText:GetAbsRect());
			local x, y, w, h = spRect:GetRect()
			spRect:GetTable()._old_pos = {x, y, w, h};
			spRect:GetTable()._pressed = true;
			y = y + 2;
			spRect:SetRect(x, y, w, h);
		end)
	spRect:SetEventHandler("OnLButtonUp", function(x, y, flag)
			print('mouse', x, y, flag);
			local pos = spRect:GetTable()._old_pos
			if pos then
				spRect:SetRect(table.unpack(pos))
				spRect:GetTable()._old_pos = nil;
			end
		end)
	spRect:SetEventHandler("OnMouseEnter", function()
			spRect:SetColor(235, 235, 230)
		end)
	spRect:SetEventHandler("OnMouseLeave", function()
			spRect:SetColor(245, 245, 240)
			local pos = spRect:GetTable()._old_pos
			if pos then
				spRect:SetRect(table.unpack(pos))
				spRect:GetTable()._old_pos = nil;
			end
		end)
end

InitUI();
luaui2.RunMessageLoop();
collectgarbage();
print('----');