local TabCtrl = {}
luaui2.TabCtrl = TabCtrl;
luaui2.LuaInherit(TabCtrl, luaui2.Control)
luaui2.LuaInherit(TabCtrl, luaui2.CallbackManager);

function TabCtrl:new()
	local self = luaui2.CreateInstance(TabCtrl);
	
	self._root = luaui2.CreateRectangleSprite();
	self._root:SetColor(255, 255, 255);
	self._root:SetBorderColor(200, 200, 200);
	self._root:SetClipChildren(true);
	self._root_event = {
		OnSize = function(...)
			self:OnSize(...);	
		end,
		OnLButtonUp = function(...)
			self:OnRootLButtonUp(...)
		end,
		OnMouseMove = function(...)
			self:OnRootMouseMove(...)
		end,
	}
	self._root:SetCallbacks(self._root_event);
	self._tabs = {}
	
	return self;
end

function TabCtrl:OnRootLButtonUp(x, y, flag)
	print("TabCtrl:OnRootLButtonUp", self._capture, x, y, flag);
	if self._capture then
		self._capture = false;
		self._root:ReleaseCapture();
		local _, _, w3, h3 = self._root:GetRect();
		self:RefreshUI(w3, h3);
		local ui_obj = self._moving_tab._ui_obj;
		ui_obj:OnLButtonUp();
	end
end

function TabCtrl:OnRootMouseMove(x, y, flag)
	y = y
	flag = flag 
	if self._capture then
		local ui_obj = self._moving_tab._ui_obj;
		local x2, y2, w2, h2 = ui_obj:GetRect();
		local x3, _, w3, h3 = self._root:GetRect();
		x2 = x - self._move_delta;
		if x2 < 0 then
			x2 = 0;
		end
		if x2 + w2 > x3 + w3 then
			x2 = x3 + w3 - w2;
		end
		
		self:UpdateTabIndex();
		local tab_data = self:TabFromPoint(x, h3 / 2);
		if tab_data then
			self._tabs[tab_data._idx] = self._moving_tab;
			self._tabs[self._moving_tab._idx] = tab_data;
			self:UpdateTabIndex();
			self:RefreshUI(w3, h3);
		end
		ui_obj:SetRect(x2, y2, w2, h2);
	end
end

function TabCtrl:UpdateTabIndex()
	for	idx, tab_data in ipairs(self._tabs) do
		tab_data._idx = idx;
	end
end

function TabCtrl:TabFromPoint(x, y)
	for _, tab_data in ipairs(self._tabs) do
		if tab_data ~= self._moving_tab then
			local rc_tab = tab_data._ui_obj:GetSprite():GetRectT();
			if luaui2.PointInRect(x, y, rc_tab) then
				return tab_data;
			end
		end
	end
end

function TabCtrl:GetSprite()
	return self._root;
end

function TabCtrl:AddTab(text, userdata)
	return self:InsertTab(#self._tabs + 1, text, userdata);
end

function TabCtrl:InsertTab(pos, text, userdata)
	local tab_data = {}
	tab_data._text = text;
	tab_data._userdata = userdata;
	
	tab_data._ui_obj = luaui2.ImgButton:new();
	tab_data._ui_obj:SetText(text);
	-- tab_data._ui_obj:SetColor(255, 255, 255);
	-- tab_data._ui_obj:SetBorderColor(200, 200, 200);
	
	tab_data._ui_event = {
		OnMouseWheel = function(...)
			print("OnMouseWheel", ...);
		end,
		OnLButtonDown = function(x)
			print("tab OnLButtonDown", pos);
			self._move_delta = x;
			self._capture = true;
			self._moving_tab = tab_data;
			tab_data._ui_obj:GetSprite():BringToFront();
			self._root:SetCapture();
			self:OnTabClick(pos);
		end,
	}
	tab_data._ui_obj:SetDelegate(tab_data._ui_event);
	
	self._root:AddChild(tab_data._ui_obj:GetSprite());
	
	-- tab_data._text = luaui2.CreateTextSprite()
	-- tab_data._text:SetColor(55, 55, 55);
	-- tab_data._text:SetFont("微软雅黑", 12, 0);
	
	-- tab_data._ui_obj:AddChild(tab_data._text);
		
	table.insert(self._tabs, pos, tab_data);
end

function TabCtrl:RefreshUI(cx, cy)
	local _, _, rw, rh = 0, 0, cx, cy; -- self._root:GetRect();
	local iw, ih = rw / #self._tabs - 3 , rh - 4;
	if iw < 50 then
		iw = 50
	end
	if iw > 150 then
		iw = 150
	end
	-- print("tab size", iw);
	-- iw = math.floor(iw); -- 这个掩盖了问题 究竟要不要整个系统都是float?
	local x, y = 0, 0;
	for _, tab in ipairs(self._tabs) do
		tab._ui_obj:SetRect(x + 2, y + 2, iw, ih);
		x = x + iw + 2;
	end
end

function TabCtrl:OnSize(cx, cy)
	self:RefreshUI(cx, cy)
end

function TabCtrl:OnTabClick(pos)
	self:InvokeCallback("OnTabChanged", self._old_tab, pos);
end