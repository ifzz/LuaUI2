-- if jit then
	-- jit.off(true, true)
-- end

-- TODO 这里怎么弄一个相对路径
package.cpath = "D:\\chenshi\\LuaUI2\\Debug\\?.dll"
-- package.cpath = "E:\\myworks\\LuaUI2\\Release\\?.dll"
package.path = "D:\\chenshi\\LuaUI2\\script\\src\\?.lua"

--[[
local curl = require("curl");
local c1 = curl.easy()
c1:setopt(curl.OPT_URL, "http://lua.org")
c1:setopt_writefunction(function(data)
		print(data);
	end)
c1:perform();
]]


-- Window Styles

WS_OVERLAPPED       = 0x00000000
WS_POPUP            = 0x80000000
WS_CHILD            = 0x40000000
WS_MINIMIZE         = 0x20000000
WS_VISIBLE          = 0x10000000
WS_DISABLED         = 0x08000000
WS_CLIPSIBLINGS     = 0x04000000
WS_CLIPCHILDREN     = 0x02000000
WS_MAXIMIZE         = 0x01000000
WS_CAPTION          = 0x00C00000
WS_BORDER           = 0x00800000
WS_DLGFRAME         = 0x00400000
WS_VSCROLL          = 0x00200000
WS_HSCROLL          = 0x00100000
WS_SYSMENU          = 0x00080000
WS_THICKFRAME       = 0x00040000
WS_GROUP            = 0x00020000
WS_TABSTOP          = 0x00010000
WS_MINIMIZEBOX      = 0x00020000
WS_MAXIMIZEBOX      = 0x00010000


-- Extended Window Styles

WS_EX_DLGMODALFRAME     = 0x00000001
WS_EX_NOPARENTNOTIFY    = 0x00000004
WS_EX_TOPMOST           = 0x00000008
WS_EX_ACCEPTFILES       = 0x00000010
WS_EX_TRANSPARENT       = 0x00000020
WS_EX_MDICHILD          = 0x00000040
WS_EX_TOOLWINDOW        = 0x00000080
WS_EX_WINDOWEDGE        = 0x00000100
WS_EX_CLIENTEDGE        = 0x00000200
WS_EX_CONTEXTHELP       = 0x00000400
WS_EX_RIGHT             = 0x00001000
WS_EX_LEFT              = 0x00000000
WS_EX_RTLREADING        = 0x00002000
WS_EX_LTRREADING        = 0x00000000
WS_EX_LEFTSCROLLBAR     = 0x00004000
WS_EX_RIGHTSCROLLBAR    = 0x00000000
WS_EX_CONTROLPARENT     = 0x00010000
WS_EX_STATICEDGE        = 0x00020000
WS_EX_APPWINDOW         = 0x00040000

print ("package.cpath", package.cpath)

require("luaui2")
require("Common")
require("Button")
require("TabCtrl")
require("RichEdit")
require("TextView")
require("ImgButton")
require("Splitter")
require("Path")
--require("Entry")

--print = winapi.Trace;

local NaviPanel = {}
luaui2.LuaInherit(NaviPanel, luaui2.Control);

function NaviPanel:new()
	local self = luaui2.CreateInstance(NaviPanel);
	self._root = luaui2.CreateRectangleSprite();
	self._root:SetColor(255, 255, 255);
	self._root:SetBorderColor(200, 200, 200);
	self._btn1 = luaui2.ImgButton:new();
	self._root:AddChild(self._btn1:GetSprite());
	self._btn2 = luaui2.ImgButton:new();
	self._root:AddChild(self._btn2:GetSprite());
--	self._edit = luaui2.CreateEditSprite();
--	self._root:AddChild(self._edit);
	self._btn3 = luaui2.ImgButton:new();
	self._btn3:SetText("大家好");
	self._root:AddChild(self._btn3:GetSprite());
	self._root_event = {
		GetName = function()
			return "NaviPanel";
		end,
		OnSize = function(cx, cy)
			self:OnSize(cx, cy);
		end
	}
	self._root:SetCallbacks(self._root_event);
	return self;
end

function NaviPanel:GetSprite()
	return self._root;
end

function NaviPanel:OnSize(cx, cy)
	cy = cy;
	local padding = 5;
	local h = 40;
	local w = cx - padding * 2;
	self._btn1:SetRect(padding, padding, w, h);
	self._btn2:SetRect(padding, padding * 2 + h, w, h);
	self._btn3:SetRect(padding, padding * 3 + h * 2, w, h);
end

local MainPanel = {}
luaui2.LuaInherit(MainPanel, luaui2.Control);

function MainPanel:new()
	local self = luaui2.CreateInstance(MainPanel);
	self._root = luaui2.CreateSprite();
	self._tab_ctrl = luaui2.TabCtrl:new();
	self._root:AddChild(self._tab_ctrl:GetSprite());
	self._root_event = {
		GetName = function()
			return "MainPanel";
		end,
		OnSize = function(cx, cy)
			self:OnSize(cx, cy);
		end
	}
	self._root:SetCallbacks(self._root_event);
	
	for i = 1, 10 do 
		self._tab_ctrl:AddTab("Tab_"..i, nil);
	end
	
	self._rich = luaui2.TextView:new()
	local path = luaui2.Path:GetExeDir() .. "\\..\\Res\\test.txt";
	local file = io.open(path, "rb");
--	local file = io.open("d:\\baidu.htm", "rb");
	if file then
		local text = file:read("*all");
		file:close();
		self._rich:SetText(text);
	else
		print("TextView load path:", path);
	end
	self._root:AddChild(self._rich:GetSprite());
	
	-- 这里测试 循环引用 内存泄露的问题
	for _ = 1, 100 do
		local sp = luaui2.CreateSprite();
		local event = {
			OnSize = function()
				sp:SetRect(0, 0, 100, 100);
			end,
		}
		sp:SetCallbacks(event);
	end
	collectgarbage("collect");
	return self;
end

function MainPanel:GetSprite()
	return self._root;
end

function MainPanel:OnSize(cx, cy)
	self._tab_ctrl:SetRect(1, 1, cx, 40);
	self._rich:SetRect(5, 45, cx - 10, cy - 50);
end

local function TravsereTree(sp, level)
	local child = sp:GetFirstSubSprite();
	while child do
		local rc = child:GetRectT();
		local name = ""
		local tbl = child:GetCallbacks()
		if type(tbl.GetName) == "function" then
			name = tbl.GetName(); -- 名称 调试用 以后可能会有 GetSpriteByName() 
		end
		print(string.format("%s%s%s", string.rep("  ", level), "{"..rc.x..","..rc.y..","..rc.w..","..rc.h.."} ", name));
		TravsereTree(child, level + 1);
		child = child:GetNextSprite();
	end
end

local function DebugSpriteTree(root)
	TravsereTree(root, 0);
end
luaui2.DebugSpriteTree = DebugSpriteTree;

local function InitMainUI()
	g_hostWnd = luaui2.CreateHostWindow(); -- 这尼玛不搞个全局引用 这个函数一返回 就可能会被辣鸡回收 这个最好在c++层面做一下
	-- local mt = getmetatable(g_hostWnd);
	-- mt.__gc = nil; -- 这尼玛真危险 太口怕了 lua这么坑人 还得在c++里防御一下 用 __metatable
	
	-- 纯色背景 没这个会是黑的(双缓冲导致)
	local spBackground = luaui2.CreateRectangleSprite();
--	spBackground:SetRect(0, 0, 1000, 1000);
	spBackground:SetColor(255, 255, 255);
	spBackground:SetBorderColor(0, 0, 0);
	g_hostWnd:AttachSprite(spBackground);

	--[[

	local panel2 = MainPanel:new();
	spBackground:AddChild(panel2:GetSprite());
	]]
	local splitter = luaui2.Splitter:new("horizontal");
	spBackground:AddChild(splitter:GetSprite());
	
	local panel1 = NaviPanel:new();
	splitter:SetFirstChild(panel1);
	
	local v_splitter = luaui2.Splitter:new("vertical");
	splitter:SetSecondChild(v_splitter);
	
	local _rich = luaui2.TextView:new()
	local path = luaui2.Path:GetExeDir() .. "\\..\\Res\\test.txt";
	local file = io.open(path, "rb");
--	local file = io.open("d:\\baidu.htm", "rb");
	if file then
		local text = file:read("*all");
		file:close();
		_rich:SetText(text);
	else
		print("TextView load path:", path);
	end
	v_splitter:SetFirstChild(_rich);
	-- TODO 这里要不要实现一个 AsyncCall呢? 还是逻辑有问题?
	-- luaui2.TimerManager:SetOnceTimer(100, function()
		splitter:SetSize(150);
		v_splitter:SetSize(200);
	-- end);
	
	local style = WS_OVERLAPPED;
	style = bit.bor(style, WS_CAPTION);
	style = bit.bor(style, WS_SYSMENU);
	style = bit.bor(style, WS_THICKFRAME);
	style = bit.bor(style, WS_MINIMIZEBOX);
	style = bit.bor(style, WS_MAXIMIZEBOX);
	style = bit.bor(style, WS_VISIBLE);
	print("style", style)
	g_hostWnd:Create(0, style, {x = 100, y = 100, w = 600, h = 400});

	host_wnd_event = {
		OnDestroy = function()
			print("OnDestroy")
			luaui2.PostQuitMessage(0);
		end,
		OnSize = function(cx, cy, _)
			spBackground:SetRect(1, 1, cx - 1, cy - 1); -- FIXME 诡异的1个像素问题 为什么这里不是从0,0开始?
			splitter:SetRect(2, 2, cx - 5, cy - 5);
			--[[
			local padding = 5;
			local w = 200 --(cx - padding * 3) / 2
			panel1:SetRect(padding, padding, w, cy - padding * 2);
			panel2:SetRect(
				padding * 2 + w, 
				padding,
				cx - w - padding * 3,
				cy - padding * 2);
			]]
		end,
	}
	g_hostWnd:SetCallbacks(host_wnd_event);
	
	luaui2.TimerManager:SetOnceTimer(500, function()
		DebugSpriteTree(spBackground);
	end);
end

local function InitLoginUI()
	
end

local function HttpTest()
	if true then
		luaui2.Http:Get("http://127.0.0.1/msg.php", 
			function(data, curl_code, http_code)
				if curl_code ~= 0 then
					print('http get error', curl_code);
					return;
				end
				if data then
					print(data);
					
					local cjson = require("cjson.safe");
					local json = cjson.decode(data);
					if not json then
						print(data);
						return;
					end
					
				end
			end)
	end
	if false then
		local path = "d:\\qq.exe"
		os.remove(path);
		luaui2.Http:DownloadToFile("http://dldir1.qq.com/qqfile/qq/QQ8.5/18600/QQ8.5.exe", path,
			function(code, winCode, http_code)
				if code ~= 0 or winCode ~= 0 then
					print("DownloadToFile error", code, winCode);
					return;
				end
			end,
			function(total, current, speed)
				local info = string.format("%d%% %dkb/s", current / total * 100, speed / 1000);
				print("Http:DownloadToFile", info)
			end)
	end
	if false then
		luaui2.Http:Post("http://127.0.0.1/post.php", "name=chenshi&passwd=fuck",
			function(data, curl_code, http_code)
				if curl_code ~= 0 then
					print('http post error', curl_code);
					return;
				end
			end);
	end
end

local function TimerTest()
	luaui2.TimerManager:SetOnceTimer(5000, function()
		print('SetOnceTimer callback');
	end)
	local i = 0;
	local timer = luaui2.TimerManager:SetTimer(500, function(id)
		i = i + 1;
		print('SetTimer callback', i);
		if i == 10 then
			luaui2.TimerManager:KillTimer(id);
		end
	end)
	print("timer id:", timer)
end

print ("Path:GetAppData()", luaui2.Path:GetAppData())

InitMainUI();
--InitLoginUI();
--HttpTest();
--TimerTest();

luaui2.RunMessageLoop();
collectgarbage();
print('----');