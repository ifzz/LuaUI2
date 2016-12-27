package.cpath = "E:\\myworks\\LuaUI2\\Debug\\?.dll"
require("luaui2")
--[[
g_hostWnd = luaui2.CreateHostWindow();
local spRoot = luaui2.CreateRectangleSprite();
spRoot:SetColor(255, 255, 255);
spRoot:SetBorderColor(0, 0, 0);
g_hostWnd:AttachSprite(spRoot);
g_hostWnd:SetRect(100, 100, 600, 400);
g_hostWnd:Create();
host_wnd_event = {
	OnDestroy = function()
		print("OnDestroy")
		luaui2.PostQuitMessage(0);
	end,
	OnSize = function(cx, cy, _)
--		spRoot:SetRect(0, 0 , cx, cy);
	end
}
g_hostWnd:SetCallbacks(host_wnd_event);
luaui2.RunMessageLoop();
]]
g_sprite = luaui2.CreateRectangleSprite();