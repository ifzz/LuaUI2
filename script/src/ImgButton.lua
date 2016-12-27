require("Path")

local ImgButton = {}
luaui2.LuaInherit(ImgButton, luaui2.Control);
luaui2.ImgButton = ImgButton

function ImgButton:new()
	local self = luaui2.CreateInstance(ImgButton);
	self._canvas = luaui2.CreateCanvasSprite();
--	self._img_normal = luaui2.CreateBitmapFromFile("d:\\test1.png");
	local Path = luaui2.Path;
	local path = Path:GetExeDir()
	-- print ("ImgButton GetExeDir", path)
	path = Path:UpOneLevel(path);
	print('ImgButton path:', path);
	self._img_4state = luaui2.CreateBitmapFromFile(path .."\\res\\lua_btn2.png");
	local s1, s2, s3 = self._img_4state:GetFourStateInfo();
	print("GetFourStateInfo", s1, s2, s3)
	local u1, u2, v1, v2 = self._img_4state:GetNineInOneInfo();
	print("GetNineInOneInfo", u1, u2, v1, v2)
	local nine_in_one_info = {u1, u2, v1, v2};
--	local iw, ih = self._img_normal:GetSize();
	self._state = 'normal'
	self._text = "ImgButton";
	self._delegate = {}
	self._canvas_event = {
		OnDraw = function()
			--[[
			self._canvas:DrawBitmap(self._img_normal,
				{x = 0, y = 0, w = iw, h = ih}, 
				{x = 0, y = 0, w = iw, h = ih});
			]]
--			self._canvas:DrawBitmap(self._img_normal, 0, 0);
			local rc_dst = self._canvas:GetRectT();
			rc_dst.x = 0;
			rc_dst.y = 0;
			local offset = 0;
			local width = s1;
			
			if self._state == 'normal' then
				
			elseif self._state == 'hover' then
				offset = s1 + 1;
				width = s2 - s1 - 1;
			elseif self._state == 'down' then
				offset = s2 + 1;
				width = s3 - s2 - 1;
			end
			luaui2.DrawTextureNineInOne(self._canvas, self._img_4state, offset, width, nine_in_one_info, rc_dst);
			self._canvas:SetColor(0, 0, 0);
			self._canvas:DrawString(self._text, rc_dst, 1, 1);
		end,
		OnMouseEnter = function (...)
			if self._delegate.OnMouseEnter then
				self._delegate.OnMouseEnter(...)
			end
			self._state = 'hover';
			self._canvas:Invalidate();
			self._canvas:TrackMouseLeave();
		end,
		OnMouseLeave = function (...)
			if self._delegate.OnMouseLeave then
				self._delegate.OnMouseLeave(...)
			end
			self._state = 'normal'
			self._canvas:Invalidate();
		end,
		OnLButtonDown = function (...)
			if self._delegate.OnLButtonDown then
				self._delegate.OnLButtonDown(...)
			end
			self._state = 'down'
			self._canvas:Invalidate();
		end,
		OnLButtonUp = function (...)
			if self._delegate.OnLButtonUp then
				self._delegate.OnLButtonUp(...)
			end
			self._state = 'hover'
			self._canvas:Invalidate();
		end,
	}
	self._canvas:SetCallbacks(self._canvas_event);
	return self;
end

function ImgButton:GetSprite()
	return self._canvas;
end

function ImgButton:SetText(t)
	self._text = t;
	self._canvas:Invalidate();
end

function ImgButton:SetDelegate(d)
	self._delegate = d;
end

function ImgButton:OnLButtonUp()
	self._state = 'hover'
	self._canvas:Invalidate();
end