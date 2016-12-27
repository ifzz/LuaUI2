-- 另一个文字控件 和RichEdit区别是 这个的buffer是线性的 而RichEdit的buffer是一行一个

local CharObject = {}
luaui2.CharObject = CharObject

function CharObject:FromChar(ch)
	local self = luaui2.CreateInstance(CharObject);
	self.ch = ch;
	return self;
end

function CharObject:FromImage(img)
	local self = luaui2.CreateInstance(CharObject);
	self.ch = ch;
	return self;
end

function CharObject:GetSize(canvas)
	
end

function CharObject:Draw(canvas)

end


local TextView = {}
luaui2.TextView = TextView;
luaui2.LuaInherit(TextView, luaui2.Control)

function TextView:new()
	local self = luaui2.CreateInstance(TextView);
	
	self._canvas_event = {
		OnDraw = function(...)
			self:OnDraw(...);
		end,
		OnLButtonDown = function(...)
			self:OnLButtonDown(...);
		end,
		OnLButtonUp = function(...)
			self:OnLButtonUp(...);
		end,
		OnMouseMove = function(...)
			self:OnMouseMove(...);
		end,
		OnSetFocus = function(...)
			self._canvas:ShowCaret();
		end,
		OnKillFocus = function(...)
			self._canvas:HideCaret();
		end,
		OnMouseWheel = function(...)
			self:OnMouseWheel(...);
		end,
		OnChar = function(...)
			self:OnChar(...)
		end,
		OnKeyDown = function(...)
			self:OnKeyDown(...)
		end,
	}
	self._canvas = luaui2.CreateCanvasSprite();
	self._canvas:SetCallbacks(self._canvas_event);
	self._canvas:EnableFocus(true);
	self._canvas:SetClipChildren(true);
	self._canvas:SetFont("微软雅黑", 14, 0);
	
	self.DEFAULT_H = 12;
	self._scroll_y = 0;
	
	self._test_img = luaui2.CreateBitmapFromFile(luaui2.Path:GetExeDir() .. "\\..\\res\\test1.png");
	self._dirty = true;
	
	return self;
end

function TextView:GetSprite()
	return self._canvas;
end

function TextView:SetText(utf8)
	self._buffer = {}
	local utf16 = winapi.GetUtf16Table(utf8);
	for _, ch_ in ipairs(utf16) do
		if ch_ == 0x0D then  -- \r
			-- do nothing
--		elseif ch_ == 0x0A then -- \n
		else
			table.insert(self._buffer, { ch = ch_ });
		end
	end
	self._caret = 1;
	self._canvas:Invalidate();
end

-- deprecated
function TextView:MeasureText()
	for _, ch_data in ipairs(self._buffer) do
		if not ch_data.w or not ch_data.h then
			ch_data.w, ch_data.h = self._canvas:MeasureChar(ch_data.ch)
		end
	end
end

function TextView:IsSelected(i)
	if not self._selection then
		return false;
	end
	local _begin, _end;
	if self._selection > self._caret then
		_begin = self._caret;
		_end = self._selection;
	else
		_begin = self._selection;
		_end = self._caret;
	end
	return i >= _begin and i < _end;
end

function TextView:OnDraw(dirty)
	local _, _, _, canvas_h = self._canvas:GetRect();
	--[[
	if self._selection then
		self._canvas:SetColor(0, 0, 255);
		self:WrapText(function (state, ch_data ,x, y, i)
			if state == 'DrawChar' then
				if y + self._scroll_y + ch_data.h >= 0 and y + self._scroll_y < canvas_h then
					if self:IsSelected(i) then
						self._canvas:DrawRect{ x = x, y = y + self._scroll_y, w = ch_data.w, h = ch_data.h}
					end
				end
			end
		end)
	end
	self._canvas:SetColor(0, 0, 0);
	self:WrapText(function (state, ch_data ,x, y, i)
		if state == 'DrawChar' then
			if y + self._scroll_y + ch_data.h >= 0 and y + self._scroll_y < canvas_h then
				if self:IsSelected(i) then
					self._canvas:SetColor(255, 255, 255);
				else
					self._canvas:SetColor(0, 0, 0);
				end
				if ch_data.img then
					self._canvas:DrawBitmap(ch_data.img, x, y + self._scroll_y)
				else
					self._canvas:DrawChar(ch_data.ch, x, y + self._scroll_y);
				end
			end
		end
	end)
	]]
	if self._dirty then
		self:GenMatrix();
	end
	for _, line in ipairs(self._matrix) do
		for _, ch_data in ipairs(line) do
			local y = ch_data.y;
			if y + self._scroll_y + ch_data.h >= 0 and y + self._scroll_y < canvas_h then
				if self:IsSelected(ch_data.pos) then
					self._canvas:SetColor(255, 0, 0);
				else
					self._canvas:SetColor(0, 0, 0);
				end
				self._canvas:DrawChar(ch_data.ch, ch_data.x, y + self._scroll_y);
			end
		end
	end
	self:MoveCaret();
end

function TextView:OnMouseWheel(x, y, flag, delta)
	self._scroll_y = self._scroll_y + delta * 40;
	if self._scroll_y > 0 then
		self._scroll_y = 0
	end
	self._canvas:Invalidate();
end

function TextView:CaretFromPos(x, y)
	y = y - self._scroll_y;
	-- local caret = -1;
	local _, _, _, canvas_h = self._canvas:GetRect();
	-- TODO 
	for _, line in ipairs(self._matrix) do
		-- 每一行的 最大高度
		if y > line.y and y < line.y + line.h then
			for _, ch_data in ipairs(line) do
				if x > ch_data.x and x < ch_data.x + ch_data.w then
					return ch_data.pos;
				end
			end
		end
	end
	return -1;
end

function TextView:OnLButtonDown(x, y)
	self._capture = true;
	if self._selection ~= nil then
		self._selection = nil;
		self._canvas:Invalidate();
	end
	local caret = self:CaretFromPos(x, y);
	if caret ~= -1 then
		self._caret = caret;
		self:MoveCaret();
	end
	print("Found Caret:", caret);
end

function TextView:OnMouseMove(x, y)
	if not self._capture then
		return
	end
	local selection = self:CaretFromPos(x, y);
	if selection ~= -1 then
		if self._selection ~= selection then
			self._selection = selection;
			self._canvas:Invalidate();
			print("_selection:", self._selection);
		end
	-- else
		-- if self._selection ~= nil then
			-- self._selection = nil;
			-- self._canvas:Invalidate();
			-- print("_selection:", self._selection);
		-- end
	end
end

function TextView:OnLButtonUp()
	self._capture = false;
end

function TextView:MoveCaret()
	local ch_data = self._buffer[self._caret];
	if ch_data then
		local rc = {x = ch_data.x, y = ch_data.y, w = 1, h = ch_data.h}
		self._canvas:SetCaretPos(rc);
	end
end

function TextView:DeleteSelected()
	local _begin = math.min(self._caret, self._selection);
	local _end = math.max(self._caret, self._selection);
	local delta = _end - _begin;
	for i = _begin, #self._buffer - delta do
		self._buffer[i] = self._buffer[i + delta];
	end
	for i = #self._buffer, #self._buffer - delta + 1, -1 do
		self._buffer[i] = nil -- 从后向前删除 防止产生空洞
	end
end

function TextView:OnChar(key_code)
	if  key_code == 0x08 then -- backspace
		if self._selection then
			self:DeleteSelected();
			self._selection = nil;
		else
			table.remove(self._buffer, self._caret - 1);
			self._caret = self._caret - 1;
			self:MoveCaret();
		end
	else
		if key_code == 0x0D then -- enter \r
			key_code = 0x0A; -- \n
		end
		if self._selection then
			self:DeleteSelected();
			self._selection = nil;
		end
		table.insert(self._buffer, self._caret, { ch = key_code });
		self._caret = self._caret + 1;
		self:MoveCaret();
	end
	self._canvas:Invalidate();
end

function TextView:OnKeyDown(key_code)
	if key_code == 0x25 then -- VK_LEFT
	elseif key_code == 0x27 then -- VK_RIGHT
	elseif key_code == 0x2E then -- VK_DELETE
		if self._selection then
			self:DeleteSelected();
			self._selection = nil;
		else
			table.remove(self._buffer, self._caret);
		end
		self._canvas:Invalidate();
	elseif key_code == 0x78 then -- VK_F9
		local ch_data = {}
		ch_data.ch = 0;
		ch_data.img = self._test_img;
		ch_data.w, ch_data.h = self._test_img:GetSize();
		table.insert(self._buffer, self._caret, ch_data);
		self._canvas:Invalidate();
	end
end

function TextView:GenMatrix()
	for _, ch_data in ipairs(self._buffer) do
		if not ch_data.w or not ch_data.h then
			ch_data.w, ch_data.h = self._canvas:MeasureChar(ch_data.ch)
		end
	end
	local _, _, canvas_w = self._canvas:GetRect()
	if canvas_w < 200 then
		canvas_w = 200 -- 防止当宽度太小时 导致死循环 
	end
	local x, y = 0, 0;
	local max_h = self.DEFAULT_H;
	local i = 1;
	local ch_data = self._buffer[i];
	self._matrix = {};
	local line = {}
	
	function InsertLine()
			line.y = y;
			line.h = max_h;
			table.insert(self._matrix, line);
			line = {};
			y = y + max_h;
			x = 0;

			max_h = self.DEFAULT_H;
	end
	
	while ch_data do
		if max_h < ch_data.h then
			print("max_h update:", max_h, )
			max_h = ch_data.h;
		end
		if ch_data.ch == 0x0A then
			InsertLine()
			-- callback('NewLine', ch_data, x, y, i);
			max_h = self.DEFAULT_H;
			i = i + 1;
			ch_data = self._buffer[i];
		elseif x + ch_data.w > canvas_w then
			InsertLine();
			y = y + max_h;
			x = 0;
			max_h = self.DEFAULT_H;
		else
			-- callback('DrawChar', ch_data, x, y, i);
			ch_data.x = x;
			ch_data.y = y;
			ch_data.pos = i;
			table.insert(line, ch_data);
			local next_ch = self._buffer[i + 1];
			if next_ch and next_ch.ch == 0x0A then
				-- InsertLine();
			end
			x = x + ch_data.w
			i = i + 1;
			ch_data = self._buffer[i];
		end
	end
	self._dirty = false;
end