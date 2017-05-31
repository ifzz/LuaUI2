local LineEdit = {}
luaui2.LineEdit = LineEdit;
luaui2.LuaInherit(LineEdit, luaui2.Control)

function LineEdit:new()
	local self = luaui2.CreateInstance(LineEdit);
	self._root = luaui2.CreateSprite();
	self._root_cb = {
		OnDraw = function(...)
			self:OnDraw(...);
		end,
		OnSize = function(...)
			self:OnSize(...)
		end,
	}
	self._root:SetCallbacks(self._root_cb);
	self._edit = luaui2.CreateEditSprite();
	self._root:AddChild(self._edit);
	return self;
end

function LineEdit:GetSprite()
	return self._root;
end

function LineEdit:OnDraw(cavans)
	cavans:SetColor(0, 255, 255);
	local _, _, w, h = self._root:GetRect();
	cavans:DrawLine(0, 0, w, 0, 1);
	cavans:DrawLine(w, 0, w, h, 1);
	cavans:DrawLine(w, h, 0, h, 1);
	cavans:DrawLine(0, h, 0, 0, 1);
end

function LineEdit:OnSize(cx, cy)
	local margin = 5
	local line_h = 25;
	self._edit:SetRect{
		x = margin,
		y = (cy - line_h) / 2, 
		w = cx - margin * 2, 
		h = line_h}
end