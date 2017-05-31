local BoxLayout = {}
luaui2.LuaInherit(BoxLayout, luaui2.Control);
luaui2.BoxLayout = BoxLayout

function BoxLayout:new()
	local self = luaui2.CreateInstance(BoxLayout);
	self._children = {};
	self._mode = 'horizontal';
	self._root = luaui2.CreateSprite();
	self._events = {
		OnSize = function(cx, cy)
			self:DoLayout();
		end
	}
	self._root:SetCallbacks(self._events);
	self._spacing = 5;
	self._margin = 5;
	return self;
end

function BoxLayout:GetSprite()
	return self._root;
end

function BoxLayout:Append(sp)
	self._root:AddChild(sp:GetSprite());
	table.insert(self._children, sp);
	-- self:DoLayout();
end

function BoxLayout:SetHorizontal()
	self._mode = 'horizontal';
end

function BoxLayout:SetVertical()
	self._mode = 'vertical';
end

function BoxLayout:SetSpacing(i)
	self._spacing = i;
end

function BoxLayout:SetMargin(i)
	self._margin = i;
end

local dummy = {
	GetPreferedWidth = function()
		return nil
	end,
	GetPreferedHeight = function()
		return nil
	end,
	SetRect = function()
		return nil
	end
}

function BoxLayout:AppendSpace()
	table.insert(self._children, dummy);
end

function BoxLayout:DoLayout()
	-- print("BEGIN", self._mode);
	local pos = self._margin;
	local _, _, w, h = self._root:GetRect(); 
	
	local prefer_func;
	if self._mode == 'horizontal' then
		prefer_func = "GetPreferedWidth";
	else
		prefer_func = "GetPreferedHeight";
	end
	-- print ("h:", h);
	local all_prefered = 0;
	for idx, sp in ipairs(self._children) do
		if sp[prefer_func](sp) then
			all_prefered = all_prefered + sp[prefer_func](sp);
		end
	end
	-- print('all_prefered', all_prefered);
	local divide = 0;
	for idx, sp in ipairs(self._children) do
		if sp[prefer_func](sp) == nil then
			divide = divide + 1;
		end
	end
	-- print("divide", divide);
	-- if divide == 0 then
		-- divide = 1
	-- end
	local all_space = (#self._children - 1) * self._spacing + self._margin * 2;
	-- print("all_space", all_space);

	local remained;
	if self._mode == 'horizontal' then
		remained = w - all_prefered - all_space;
	else
		remained = h - all_prefered - all_space;
	end
	-- print("remained: ", remained);
	
	for idx, sp in ipairs(self._children) do
	
		local size;
		if sp[prefer_func](sp) then
			size = sp[prefer_func](sp);
		else
			-- print('remained:', remained);
			size = (remained) / divide;
			-- print ("size:", size);
			if size < 10 then
				size = 10;
			end
		end
		if self._mode == 'horizontal' then
			sp:SetRect({ x = pos, y = self._margin, w = size, h = h - self._margin * 2});
		else
			sp:SetRect({ x = self._margin, y = pos, w = w - self._margin * 2, h = size});
		end
		pos = pos + size + self._spacing;
	end
	-- print("END");
end
