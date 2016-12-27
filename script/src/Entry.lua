local Entry = {}
luaui2.Entry = Entry;
luaui2.LuaInherit(Entry, luaui2.Control)

function Entry:new()
	local self = luaui2.CreateInstance(Entry);
	self._root = luaui2.CreateSprite();
	self._root_event = {
		OnSize = function(cx, cy)
			self:OnSize(cx, cy);
		end
	}
	self._root:SetCallbacks(self._root_event);
	
	self._canvas = luaui2.CreateCanvasSprite();
	math.randomseed(os.time());
	self._text = "编辑框演示 可以输入中文 逻辑由lua编写"
	self._utf16 = luaui2.GetUtf16Table(self._text);
	self._rc_caret = {x = 0, y = 0, w = 2, h = 10};
	self._canvas_event = {
		OnDraw = function()
			local rand = math.random;
--			local _, _, cx, cy = self._canvas:GetRect();
			
--			for i = 1, 1000 do
				--[[
				self._canvas:DrawLine(
					rand() * cx, rand() * cy,  -- point 1
					rand() * cx, rand() * cy,  -- point 2
					1)
				]]
				--[[
				self._canvas:DrawChar(
					0x554A,
					rand() * cx, rand() * cy,
				) ]]
--			end 
			local x, y = 0, 0;
			local w, h;
			for _, ch in ipairs(self._utf16) do
--				self._canvas:SetColor(rand() * 256, rand() * 256, rand() * 256);
				w, h = self._canvas:MeasureChar(ch)
				h = h
				self._canvas:DrawChar(ch, x, y);
				x = x + w;
			end
			local arc = self._canvas:GetAbsRectT();
			self._rc_caret.x = x + arc.x + 5;
			self._rc_caret.y = y + arc.y;
			if h then
				self._rc_caret.h = h;
			end
			self._canvas:ShowCaret(self._rc_caret);
			local i;
			i = 1;
		end,
		OnChar = function(key_code, flag)
			if key_code == 0x08 then
				self._utf16[#self._utf16] = nil;
			else
				table.insert(self._utf16, key_code);
			end
			self._canvas:Invalidate();
		end,
		OnKeyDown = function(key_code, flag)
			
		end,
		OnLButtonDown = function()
			self._canvas:Invalidate();
		end,
		OnImeInput = function(text)
			print("OnImeInput", text);
			local tmp16 = luaui2.GetUtf16Table(text);
			for _, code in ipairs(tmp16) do
				table.insert(self._utf16, code);
			end
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
	}
	self._canvas:SetCallbacks(self._canvas_event);
	self._canvas:EnableFocus(true);
	self._root:AddChild(self._canvas);
	
	return self;
end

function Entry:OnSize(cx, cy)
	self._canvas:SetRect(5, 5, cx - 5 * 2, cy - 5 * 2);
end

function Entry:GetSprite()
	return self._root;
end