local Button = {}
luaui2.LuaInherit(Button, luaui2.Control);
luaui2.Button = Button

function Button:new()
	local self = luaui2.CreateInstance(Button);
	self._root = luaui2.CreateRectangleSprite();
	self._root:SetColor(245, 245, 240);
	self._root:SetBorderColor(150, 150, 150);
	self._text = luaui2.CreateTextSprite();
	self._text:SetColor(55, 55, 55);
	self._text:SetFont("微软雅黑", 12, 0);
	self._root:AddChild(self._text);
	self._root:EnableFocus();
	self._root_event = {
		OnLButtonDown = function(...)
			self:OnLButtonDown(...);
		end,
		OnSize = function(...)
			self:OnSize(...);
		end,
		OnLButtonUp = function(...)
			self:OnLButtonUp(...);
		end,
		OnMouseEnter = function()
			self:OnMouseEnter();
		end,
		OnMouseLeave = function()
			self:OnMouseLeave();
		end,
		OnKeyDown = function(...)
			self:OnKeyDown(...)
		end,
		OnMouseWheel = function(...)
			print("OnMouseWheel", ...);
		end,
	}
	self._root:SetCallbacks(self._root_event);
	return self;
end

function Button:OnLButtonDown()
	-- 这样甚至 GetTable() 都是不需要的 lua太强大了 upvalue带self
	print("OnLButtonDown");
	local x, y, w, h = self._root:GetRect()
	self._old_pos = {x, y, w, h};
	self._pressed = true;
	y = y + 2;
	self._root:SetRect(x, y, w, h);
end

function Button:OnLButtonUp(x, y)
	print('OnLButtonUp', x, y);
	local pos = self._old_pos
	if pos then
		self._root:SetRect(unpack(pos))
		self._root:GetTable()._old_pos = nil;
	end
end

function Button:OnMouseEnter()
	self._root:SetColor(235, 235, 230);
end

function Button:OnMouseLeave()
	self._root:SetColor(245, 245, 240)
	local pos = self._old_pos
	if pos then
		self._root:SetRect(unpack(pos))
		self._old_pos = nil;
	end
end

function Button:OnKeyDown(key)
	print(self._name, "OnKeyDown", key);
end

function Button:OnSize(cx, cy)
	self._text:SetRect(0, 0, cx, cy);
end

function Button:SetText(str)
	self._text:SetText(str);
end

function Button:GetSprite()
	return self._root;
end

function Button:SetName(str)
	self._name = str;
end