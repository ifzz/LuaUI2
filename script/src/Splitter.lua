local Splitter = {}
luaui2.Splitter = Splitter;
luaui2.LuaInherit(Splitter, luaui2.Control)
luaui2.LuaInherit(Splitter, luaui2.CallbackManager);

local IDC_SIZEWE = 32644
local IDC_SIZENS = 32645

function Splitter:new(mode)
	local self = luaui2.CreateInstance(Splitter);
	self._root = luaui2.CreateRectangleSprite();
	self._root:SetColor(255, 255, 255);
	self._root:SetBorderColor(200, 200, 200);
	
	self._root_event = {
		GetName = function()
			return "Splitter";
		end,
		OnSize = function(...)
			self:OnSize(...);	
		end,
		OnLButtonDown = function(...)
			self:OnRootLBtnDown(...);
		end,
		OnMouseMove = function(...)
			self:OnRootMouseMove(...)
		end,
		OnLButtonUp = function(...)
			self:OnRootLBtnUp(...);
		end,
	}
	self._root:SetCallbacks(self._root_event);
	
	self._splitter = luaui2.CreateRectangleSprite();
	self._splitter:SetColor(255, 255, 255);
	self._splitter:SetBorderColor(200, 200, 200);
	self._root:AddChild(self._splitter);
	self._splitter_event = {
		OnLButtonDown = function(...)
			self:OnSplitterLBtnDown(...);
		end,
		OnMouseMove = function(...)
			self:OnSplitterMouseMove(...);
		end
	}
	self._splitter:SetCallbacks(self._splitter_event);
	
	self._size = 100;
	self._splitter_size = 5;
	self._mode = mode or "horizontal"; -- vertical 
	self._mini_size = 30;
	
	return self;
end

function Splitter:SetMinimalSize(i)
	assert(type(i) == 'number');
	self._mini_size = i;
	self:Update();
end

function Splitter:LimitSize()
	local _, _, cx, cy = self._root:GetRect();
	if self._mode == "horizontal" then
		local size = math.min(self._size, cx - self._splitter_size - self._mini_size);
		return math.max(size, self._mini_size);
	else
		local size = math.min(self._size, cy - self._splitter_size - self._mini_size);
		return math.max(size, self._mini_size);
	end
end

function Splitter:Update()
	-- print("Splitter:Update _mode", self._mode);
	local _, _, cx, cy = self._root:GetRect();
	-- print("Splitter:Update cx", cx, "cy", cy);
	
	local size = self:LimitSize();
	if self._mode == "horizontal" then
		self._splitter:SetRect(size, 0, self._splitter_size, cy);
	else
		self._splitter:SetRect(0, size, cx, self._splitter_size);
	end

	if self._first then
		if self._mode == "horizontal" then
			self._first:GetSprite():SetRect(0, 0, size, cy);
		else
			self._first:GetSprite():SetRect(0, 0, cx, size);
		end
	end
	if self._second then
		if self._mode == "horizontal" then
			self._second:GetSprite():SetRect(size + self._splitter_size, 0, 
				cx - size - self._splitter_size, cy);
			-- print("Splitter:Update _second", self._second:GetRect());
		else
			self._second:GetSprite():SetRect(0, size + self._splitter_size,
				cx, cy - size - self._splitter_size);
		end
	end
end

function Splitter:OnSize(cx, cy)
	-- print("Splitter:OnSize _mode", self._mode);
	self:Update();
end

function Splitter:GetSprite()
	return self._root;
end

function Splitter:OnSplitterLBtnDown(x, y)
	-- print("OnSplitterLBtnDown", x, y);
	self:SetResizeCursor();
	
	if self._mode == "horizontal" then
		self._move_delta = x;
	else
		self._move_delta = y;
	end
	self._capture = true;
	self._root:SetCapture();
end

function Splitter:OnRootLBtnDown(x, y)
	-- print("OnRootLBtnDown", x, y);
end

function Splitter:OnRootLBtnUp()
	self._move_delta = nil;
	self._capture = false;
	self._root:ReleaseCapture();
end

function Splitter:OnRootMouseMove(x, y)
	if self._capture then
		self:SetResizeCursor();
		-- local sp_rc = self._splitter:GetRectT();
		-- sp_rc.x = x - self._move_delta;
		
		if self._mode == "horizontal" then
			self._size = x - self._move_delta;
		else
			self._size = y - self._move_delta;
		end
		self:Update();
		-- self._splitter:SetRect(sp_rc);
	end
end

function Splitter:OnSplitterMouseMove(x, y)
	-- print("OnSplitterMouseMove", x, y);
	self:SetResizeCursor();
end

function Splitter:SetFirstChild(control)
	if self._first then
		-- TODO remove from _root
	end
	self._first = control;
	self._root:AddChild(control:GetSprite());
	self:Update();
end

-- TODO FIXME 这里这样没有引用控件的表... 会导致回收 然后子视图无法收到事件
function Splitter:SetSecondChild(control)
	if self._second then
		-- TODO remove from _root
	end
	self._second = control;
	self._root:AddChild(control:GetSprite());
	self:Update();
end

function Splitter:SetResizeCursor()
	if self._mode == "horizontal" then
		winapi.SetCursor(IDC_SIZEWE);
	else
		winapi.SetCursor(IDC_SIZENS);
	end
end

function Splitter:SetSize(size)
	self._size = size;
	self:Update();
end