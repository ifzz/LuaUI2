local RichEdit = {}
luaui2.RichEdit = RichEdit;
luaui2.LuaInherit(RichEdit, luaui2.Control)

-- self._lines = { { ch = 'a', w = 12, h = 10}, ... }

function RichEdit:new()
	local self = luaui2.CreateInstance(RichEdit);
	self._root = luaui2.CreateSprite();
	self._root_event = {
		OnSize = function(cx, cy)
			self._canvas:SetRect(5, 5, cx - 5 * 2, cy - 5 * 2);
		end
	}
	self._root:SetCallbacks(self._root_event);
	
	self._lines = {}
	self._lines[1] = {h = 10}
	self._rc_caret = {x = 0, y = 0, w = 2, h = 10};
	self._caret = {col = 1, row = 1}
	self._scroll_y = 0;
	
	self._canvas_event = {
		OnDraw = function() 
--			print("OnDraw", math.random());
			if self._resized then
				--print("OnDraw", "_resized", math.random());
				self:UnWrapText();
				for lnum, line in ipairs(self._lines) do
					self:MeasureLine(lnum, line);
				end
				self:WrapText();
				self:RestoreCaret();
				self._resized = false;
			end
			self:DrawSelection();
			self:DrawText();
			self:UpdateCaret();
		end,
		OnChar = function(key_code, _)
			local _caret = self._caret;
			local line = self._lines[_caret.row];
			if  key_code == 0x08 then -- backspace
				self:OnBacksapce();
--			elseif key_code == 0x
			elseif key_code == 0x0D then -- enter
				-- 光标在中间时 切成2行
				local new_line = {}
				for i = _caret.col, #line do
					table.insert(new_line, line[i]);
				end
				for i = #line, _caret.col, -1 do
					line[i] = nil; -- 要从后向前删除 避免空洞
				end
				if line.eol then
					new_line.eol = true;
				else
					line.eol = true;
				end
				table.insert(self._lines, _caret.row + 1, new_line);
				_caret.row = _caret.row + 1;
				_caret.col = 1;
			else
				if _caret.col > #line then
					table.insert(line, { ch = key_code, _caret = true });
--					_caret.col = _caret.col + 1
					print("insert at end")
				else
					table.insert(line, _caret.col, { ch = key_code, _caret = true });
--					_caret.col = _caret.col + 1
				end
				self._selection = nil;
			end
			self._resized = true; -- wrap
			self._canvas:Invalidate();
		end,
		OnKeyDown = function(key_code)
			local _caret = self._caret;
			local line = self._lines[_caret.row];
			if key_code == 0x25 then -- VK_LEFT
				if _caret.col > 1 then
					_caret.col = _caret.col - 1;
					self:LimitCaret(); -- TODO 这里实际上应该折到上一行
					self:UpdateCaret();
				end
			elseif key_code == 0x27 then -- VK_RIGHT
				if _caret.col <= #line then
					_caret.col = _caret.col + 1;
					self:LimitCaret(); -- TODO 这里实际上应该折到下一行
					self:UpdateCaret();
				end
			elseif key_code == 0x26 then -- VK_UP
				if _caret.row > 1 then
					_caret.row = _caret.row - 1;
					self:LimitCaret();
					self:UpdateCaret();
				end
			elseif key_code == 0x28 then -- VK_DOWN
				if _caret.row < #self._lines then
					_caret.row = _caret.row + 1;
					self:LimitCaret();
					self:UpdateCaret();
				end
			elseif key_code == 0x24 then -- VK_HOME
				_caret.col = 1;
				self:UpdateCaret();
			elseif key_code == 0x23 then -- VK_END
				_caret.col = 999999;
				self:LimitCaret();
				self:UpdateCaret();
			elseif key_code == 0x2E then -- VK_DELETE
				self:OnDeleteKeyDown();
			end
		end,
		OnLButtonDown = function(x, y)
			self._drag = true;
			self._caret = self:CaretFromPos(x, y);
			self._selection = {}
			self._selection.row = self._caret.row;
			self._selection.col = self._caret.col;
			self:UpdateCaret();
			self._canvas:Invalidate();
		end,
		OnLButtonUp = function()
			self._drag = false;
			self._old_caret = nil;
		end,
		OnMouseMove = function(x, y)
			if self._drag then
				local _caret = self:CaretFromPos(x, y);
				self._caret = _caret;
				-- 这里仅当变化时刷新
				if not self._old_caret or self._old_caret.row ~= _caret.row or self._old_caret.col ~= _caret.col then
--					print("UpdateCaret", _caret.row, _caret.col);
					self:UpdateCaret();
					self._canvas:Invalidate();
					self._old_caret = {row = _caret.row, col = _caret.col};
				end
			end
		end,
		OnImeInput = function(text)
			print("OnImeInput", text);
			local tmp16 = winapi.GetUtf16Table(text);
			local line = self._lines[self._caret.row];
			for _, code in ipairs(tmp16) do
				table.insert(line, self._caret.col, { ch = code }); -- FIXME 这里复杂度太高
				self._caret.col = self._caret.col + 1
			end
			line[self._caret.col - 1]._caret = true;
			self._selection = nil;
			self._resized = true; -- wrap
			self._canvas:Invalidate();
		end,
		OnSetFocus = function()
			print("OnSetFocus");
			self._canvas:ShowCaret(self._rc_caret);
		end,
		OnKillFocus = function()
			print("OnKillFocus");
			self._canvas:HideCaret();
		end,
		OnMouseWheel = function(x, y, flag, delta)
			x = x;
			y = y;
			flag = flag;
			self._scroll_y = self._scroll_y + delta * 40;
			if self._scroll_y > 0 then
				self._scroll_y = 0
			end
			self._canvas:Invalidate();
		end,
		OnSize = function()
			self._selection = nil;
			self._resized = true;
		end,
	}
	self._canvas = luaui2.CreateSprite();
	self._canvas:SetCallbacks(self._canvas_event);
	self._canvas:EnableFocus(true);
	self._canvas:SetClipChildren(true);
	self._canvas:SetFont("微软雅黑", 12, 0);
	self._root:AddChild(self._canvas);
	
	return self;
end

function RichEdit:OnBacksapce()
	local _caret = self._caret;
	local line = self._lines[_caret.row];
	if _caret.col > 1 then
		table.remove(line, _caret.col - 1);
		_caret.col = _caret.col - 1;
		print("backspace", "normal", _caret.col)
	else
		assert(_caret.col == 1);
		if _caret.row > 1 then
			print("backspace", "join_line", "begin");
			local prev_line = self._lines[_caret.row - 1];
			local bMoved = false;
			if prev_line.eol then
				prev_line.eol = false;
				if not prev_line[1] then -- 上一行只有一个回车 要删掉这一行
					table.remove(self._lines, _caret.row - 1);
					print("backspace", "join_line", "remove_row", _caret.row - 1);
					_caret.row = _caret.row - 1;
					_caret.col = 1;
					bMoved = true;
				end
			end
			if not bMoved then
				_caret.row = _caret.row - 1;
				_caret.col = #self._lines[_caret.row] + 1 -- FIXME 删到第一行会挂掉
			end
			print("backspace", "join_line", "end", _caret.row, _caret.col);
		end
	end
	self._selection = nil;
end

function RichEdit:OnDeleteKeyDown()
	local _caret = self._caret;
	local line = self._lines[_caret.row];
	if _caret.col < #line + 1 then
		table.remove(line, _caret.col);
		-- _caret.col = _caret.col - 1;
		print("backspace", "normal", _caret.col)
	else
		assert(_caret.col == #line + 1);
		-- if _caret.row > 1 then -- TODO 限制最后一行不能再删了
			print("backspace", "join_line", "begin");
			-- local prev_line = self._lines[_caret.row - 1];
			local bMoved = false;
			if line.eol then
				line.eol = false;
				if not line[1] then -- 这一行只有一个回车 要删掉这一行
					table.remove(self._lines, _caret.row);
					print("backspace", "join_line", "remove_row", _caret.row);
					-- _caret.row = _caret.row - 1;
					_caret.col = 1;
					bMoved = true;
				end
			end
			if not bMoved then
				-- _caret.row = _caret.row - 1;
				-- _caret.col = #self._lines[_caret.row] + 1 -- FIXME 删到第一行会挂掉
			end
			print("backspace", "join_line", "end", _caret.row, _caret.col);
		-- end
	end
	self._resized = true; -- wrap
	self._selection = nil;
	self._canvas:Invalidate();
end

function RichEdit:LimitCaret()
	local _caret = self._caret;
	local line = self._lines[_caret.row];
	if _caret.col > #line + 1 then
		_caret.col = #line + 1;
	end
end

function RichEdit:MeasureLine(lnum, line)
	-- 优化方案: 在用户输入时测量单个字符的大小 删除不需要更新
	lnum = lnum;
	local max_h = 12
	for _, ch_data in ipairs(line) do
		if not ch_data.w or not ch_data.h then
				ch_data.w, ch_data.h = self._canvas:MeasureChar(ch_data.ch)
		end
		if ch_data.h > max_h then
			max_h = ch_data.h;
		end
	end
	line.h = max_h;
end

function RichEdit:DrawText()
	-- 优化方案1: 局部刷新
	-- 优化方案2: 2分法找到屏幕第一行 然后再绘制 这样可以接近O(logN)复杂度
	-- 因为用户的屏幕大小是有限的 所以复杂度的计算应该在查找第一行上
	local _, _, _, canvas_h = self._canvas:GetRect();
	local _draw_y = self._scroll_y;
	local selected = false;
--	self._canvas:SetColor(0,0,0);
	for row, line in ipairs(self._lines) do
		local x = 0;
		if _draw_y + line.h >= 0 and _draw_y < canvas_h then
			for col, ch_data in ipairs(line) do
		--		ch_data.w, ch_data.h = self._canvas:MeasureChar(ch_data.ch)
				selected = self:IsSelected(row, col)
				if selected then
					--self._canvas:SetColor(0, 0, 255);
					--self._canvas:DrawRect{ x = x, y = _draw_y, w = ch_data.w, h = ch_data.h}
					self._canvas:SetColor(255,255,255);
				else
					self._canvas:SetColor(0,0,0);
				end
				self._canvas:DrawChar(ch_data.ch, x, _draw_y);
				x = x + ch_data.w;
			end
		end
		_draw_y = _draw_y + line.h;
	end
end

function RichEdit:DrawSelection()
	if self:IsEmptySelection() then
		return
	end
	local _, _, _, canvas_h = self._canvas:GetRect();
	local _draw_y = self._scroll_y;
	local selected = false;
	for row, line in ipairs(self._lines) do
		local x = 0;
		if _draw_y + line.h >= 0 and _draw_y < canvas_h then
			for col, ch_data in ipairs(line) do
		--		ch_data.w, ch_data.h = self._canvas:MeasureChar(ch_data.ch)
				selected = self:IsSelected(row, col)
				if selected then
					self._canvas:SetColor(0, 0, 255);
					self._canvas:DrawRect{ x = x, y = _draw_y, w = ch_data.w, h = ch_data.h}
				end
				x = x + ch_data.w;
			end
		end
		_draw_y = _draw_y + line.h;
	end
end

function RichEdit:SwapCaretSelection()
	local begin = self._caret;
	local _end = self._selection;
	local tmp;

	-- 交换位置 
	if begin.row > _end.row then
		tmp = begin;
		begin = _end;
		_end = tmp;
	elseif begin.row == _end.row then
		if begin.col > _end.col then
			tmp = begin;
			begin = _end;
			_end = tmp;
		end
	end
	return begin, _end;
end

function RichEdit:IsEmptySelection()
	if not self._selection then
		return true;
	end
	if self._selection.row == self._caret.row and self._selection.col == self._caret.col then
		return true;
	end
end

function RichEdit:IsSelected(row, col)
	if not self._selection then
		return false;
	end
	local begin, _end = self:SwapCaretSelection();
	if begin.row == _end.row then
		if row == begin.row and col >= begin.col and col < _end.col then
			return true;
		end
	elseif row == begin.row and col > begin.col then
		return true;
	elseif row == _end.row and col < _end.col then
		return true;
	elseif row > begin.row and row < _end.row then
		return true;
	end
	return false;
end

function RichEdit:UpdateCaret()
	-- 最坏情况 O(n)复杂度
	local _rc_caret = self._rc_caret;
	local _caret = self._caret;
	_rc_caret.y = 0;
	for i = 1, _caret.row - 1 do
		local line = self._lines[i]
		_rc_caret.y = _rc_caret.y + line.h;
	end
	local line = self._lines[_caret.row]
	_rc_caret.x = 0;
	for j = 1, _caret.col - 1 do
		if line and line[j] then
			_rc_caret.x = _rc_caret.x + line[j].w;
		else
--			print('RichEdit:UpdateCaret() line nil');
		end
	end
	if _caret.col > 1 then
		_rc_caret.x = _rc_caret.x + 2;
	end
	_rc_caret.h = self._lines[_caret.row].h;
	_rc_caret.y = _rc_caret.y + self._scroll_y;
	self._canvas:SetCaretPos(_rc_caret);
end

function RichEdit:CaretFromPos(x, y)
	y = y - self._scroll_y; -- 处理滚动
	local cy = 0;
	local foundRow = false
	local _caret = {}
	for row, line in ipairs(self._lines) do
		cy = cy + line.h
		if y < cy then
			_caret.row = row;
			foundRow = true;
			break;
		end
	end
	if not foundRow then -- 处理点空白情况
		_caret.row = #self._lines;
	end
	local line = self._lines[_caret.row]
	local cx = 0;
	local foundCol = false;
	for col, ch_data in ipairs(line) do
		cx = cx + ch_data.w;
		if x < cx then
			_caret.col = col;
			foundCol = true;
			break;
		end
	end
	if not foundCol then -- 处理点空白情况
		_caret.col = #line + 1;
	end
	return _caret;
end

-- 从ch_data上面找到正确的光标位置(在折行之后)
function RichEdit:RestoreCaret()
	local bFound = false;
	for row, line in ipairs(self._lines) do
		for col, ch_data in ipairs(line) do
			if ch_data._caret then
				print("RestoreCaret", row, col);
				self._caret.row = row;
				self._caret.col = col + 1;
				assert(not bFound); -- 找出有多个ch_data被设置的bug
				bFound = true;
				ch_data._caret = nil;
			end
		end
	end
	if not bFound then
		print('RestoreCaret', 'no found');
	end
end

function RichEdit:SplitLine(line, canvas_w)
	local lines = {}
	local new_line = {h = line.h}
	if #line == 0 then
		new_line.eol = true;
		table.insert(lines, new_line);
		return lines;
	end
	local x = 0;
	for _, ch_data in ipairs(line) do
		if x + ch_data.w > canvas_w then
			table.insert(lines, new_line);
			new_line = {h = line.h}
			x = 0;
		end
		table.insert(new_line, ch_data);
		x = x + ch_data.w;
	end
	if #new_line > 0 then
		table.insert(lines, new_line);
	end
	if #lines > 0 then
		lines[#lines].eol = true;
	end
	return lines;
end

function RichEdit:WrapText()
	local _, _, canvas_w = self._canvas:GetRect()
	if canvas_w < 100 then
		return -- 防止当宽度太小时 导致死循环 
	end
	local lines = {}
	-- TODO 这里应该像UnWrapText一样用个临时表来插入所有行 否则在中间插入非常费时 并且不是o(n)复杂度 可能会到o(n^2)
	for _, line in ipairs(self._lines) do	
		local wraped = self:SplitLine(line, canvas_w);
		for _, wraped_line in ipairs(wraped) do
			table.insert(lines, wraped_line);
		end
--		if not lines[#lines] then
--			print("break");
--		end
	end
	self._lines = lines;
end

-- 改变大小之后 先要恢复本来的文字(不换行) 然后再已新大小重新折行
function RichEdit:UnWrapText()
	local lines = {}
	local new_line = {};
	for _, line in ipairs(self._lines) do
		for _, ch_data in ipairs(line) do
			table.insert(new_line, ch_data);
		end
		-- 放一个ch_data.ch = '\n' 这个作为决定行尾的标志
--		if not line[#line] then
--			print("break");
--		end
		assert(#line > 0 or line.eol);

		if line.eol then
			new_line.eol = true;
			table.insert(lines, new_line);
			new_line = {}
		end
	end
--	if #new_line > 0 then
		new_line.eol = true;
		table.insert(lines, new_line);
--	end
	self._lines = lines;
end

function RichEdit:SetText(utf8)
	local utf16 = winapi.GetUtf16Table(utf8);
	local lines = {}
	local line = {}
	
	for _, ch_ in ipairs(utf16) do
		if ch_ == 0x0D then  -- \r
			-- do nothing
		elseif ch_ == 0x0A then -- \n
--			table.insert(line, { ch = ch_ });
			line.eol = true;
			table.insert(lines, line);
			line = {};
		else
			table.insert(line, { ch = ch_ });
		end
	end
	if #line > 0 then
		table.insert(lines, line); -- 最后一行究竟没有有换行 如果搞不清楚可能SetText完了在来个GetText尾部的换行就诡异了
	end
	self._lines = lines;
	self._caret = {row = 1, col = 1};
	self._canvas:Invalidate();
end

function RichEdit:GetSprite()
	return self._root;
end