local ScrollBar = {}
luaui2.LuaInherit(ScrollBar, luaui2.Control);
luaui2.ScrollBar = ScrollBar

function ScrollBar:new()
	local self = luaui2.CreateInstance(ScrollBar);
	self._root = luaui2.CreateSprite();
	self._root_event = {
		OnSize = function(cx, cy)
			self:OnSize(cx, cy);
		end,
		OnLButtonUp = function(x, y)
			self:EndDrag(x, y);
		end,
		OnMouseMove = function(x, y)
			self:OnMouseMove(x, y);
		end
	}
	self._root:SetCallbacks(self._root_event);
	
	self._prev_btn = luaui2.ImgButton:new();
	self._prev_btn:SetText("▲");
	self._prev_btn:SetTextColor(127, 127, 127)
	self._root:AddChild(self._prev_btn:GetSprite());
	
	self._next_btn = luaui2.ImgButton:new();
	self._next_btn:SetText("▼");
	self._next_btn:SetTextColor(127, 127, 127)
	self._root:AddChild(self._next_btn:GetSprite());
	
	self._slider_btn = luaui2.ImgButton:new();
	self._slider_btn:SetText("");
	self._root:AddChild(self._slider_btn:GetSprite());
	self._slider_event = {
		OnLButtonDown = function(x, y)
			self:BeginDrag(x, y);
		end
	}
	self._slider_btn:SetDelegate(self._slider_event);
	
	self._btn_size = 20;
	self._position = 0;
	self._total = 1000;
	return self;
end

function ScrollBar:GetSprite()
	return self._root;
end

function ScrollBar:GetPreferedWidth()
	return self._btn_size;
end

function ScrollBar:OnSize(cx, cy)
	self._prev_btn:SetRect({x = 0, y = 0, w = cx, h = self._btn_size});
	local pos = cy - self._btn_size;
	if pos < self._btn_size then
		pos = self._btn_size;
	end
	self._next_btn:SetRect({x = 0, y = pos, w = cx, h = self._btn_size});
	self:Update();
end

function ScrollBar:Update()
	local _, _, w, h = self._root:GetRect();
	if h == 0 then
		print ("h == 0")
		return;
	end
	local h2 = h - self._btn_size * 2;
	if self._total <= h then
		self._slider_btn:SetVisible(false);
		return
	else
		self._slider_btn:SetVisible(true);
	end
	local size = h / (self._total) * h2;
	if size < self._btn_size then
		size = self._btn_size;
	end
	if size > h2 then
		size = h2;
	end
	self._slider_size = size;
	if self._position + h > self._total then
		self._position = self._total - h;
	end
	local pos = self._position / (self._total - h) * (h2 - size);
	self._slider_btn:SetRect({x = 0, y = self._btn_size + pos, w = w, h = size});
--	print(h, h2, size, self._total - h, pos, self._position);
end

function ScrollBar:GetSliderSize()
	
end

function ScrollBar:BeginDrag(x, y)
	self._drag_delta = y;
	self._capture = true;
	self._root:SetCapture();
end

function ScrollBar:EndDrag(x, y)
	if self._capture then
		self._capture = false;
		self._root:ReleaseCapture();
		self._slider_btn:OnLButtonUp();
		
		local _, _, w, h = self._root:GetRect();
		local h2 = h - self._btn_size * 2;
		local pos = y - self._drag_delta - self._btn_size;
		if pos < 0 then
			pos = 0;
		end
		if pos > h2 then
			pos = h2
		end
		self._position = pos / h2 * self._total;
		print(self._position, pos, self._drag_delta, h2);
	end
end

function ScrollBar:OnMouseMove(x, y)
	if self._capture then
		local pos = y - self._drag_delta
		if pos < self._btn_size then
			pos = self._btn_size;
		end
		local _, _, w, h = self._root:GetRect();
		local max = h - self._btn_size - self._slider_size
		if pos > max then
			pos = max;
		end
		self._slider_btn:SetRect({x = 0, y = pos, w = self._btn_size, h = self._slider_size});
	end
end

function ScrollBar:SetTotalSize(t)
  self._total = t;
  self:Invalidate();
end