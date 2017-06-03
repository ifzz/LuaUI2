if jit then
	jit.off(true, true)
    jit.flush();
end

-- 这里用一个相对路径 使得ZeroBraneStudio可以双击跳转到代码
package.cpath = "Debug\\?.dll"
package.path = "script\\src\\?.lua"

--[[
local curl = require("curl");
local c1 = curl.easy()
c1:setopt(curl.OPT_URL, "http://lua.org")
c1:setopt_writefunction(function(data)
		print(data);
	end)
c1:perform();
]]



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
require("BoxLayout")
require("ScrollBar")
require("LineEdit")
require("Constant")
ProFi = require("ProFi")
--require("Entry")

--print = winapi.Trace;

local old_print = print;
print = function(...)
	old_print(...);
	local info = debug.getinfo(2, "Sl");
	local path;
	if info.source:byte(1) == 64 then
		path = info.source:sub(2);
	else
		path = info.source;
	end
	old_print("\t" .. path .. ":" .. info.currentline .. ":");
end

table.unpack = unpack;

local function TravsereTree(sp, level)
	local child = sp:GetFirstSubSprite();
	while child do
		local rc = child:GetRectT();
		local name = ""
		local tbl = child:GetCallbacks()
		if tbl and type(tbl.GetName) == "function" then
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

	local splitter = luaui2.Splitter:new("horizontal");
	spBackground:AddChild(splitter:GetSprite());
	
	local box1 = luaui2.BoxLayout:new();
	box1:SetVertical();
	local btn1 = luaui2.ImgButton:new();
	btn1:SetText("打开");
    local _rich;
	btn1:SetDelegate({
      OnLButtonUp = function()
        local path = winapi.GetOpenFileName({"txt files", "*.txt", "all files", "*.*"}, g_hostWnd:GetHWND());
        print("GetOpenFileName:", path);
		if not path then
			return
		end
        local file = io.open(path, "rb");
        if file then
            local text = file:read("*all");
            file:close();
            _rich:SetText(text);
        else
            print("Load File Failed:", path);
        end
      end});
	btn1:SetPreferedHeight(25);
	box1:Append(btn1);
	local btn2 = luaui2.ImgButton:new();
	btn2:SetPreferedHeight(25);
	box1:Append(btn2);
	local btn3 = luaui2.ImgButton:new();
	btn3:SetPreferedHeight(25);
	box1:Append(btn3);
	
	local edit = luaui2.LineEdit:new();
	box1:Append(edit);
	edit:SetPreferedHeight(65);
-- dit:SetRect(0, 200, 140, 40);
  
	-- local btn4 = luaui2.ImgButton:new();
--	box1:AppendSpace();
--	local btn5 = luaui2.ImgButton:new();
--	box1:Append(btn5);
	
	-- local panel1 = NaviPanel:new();
	splitter:SetFirstChild(box1);
	local sb = luaui2.ScrollBar:new();

--	local v_splitter = luaui2.Splitter:new("vertical");
--	splitter:SetSecondChild(v_splitter);
	
	_rich = luaui2.TextView:new()
	local path = "Res\\test.txt";
	local file = io.open(path, "rb");
--	local file = io.open("d:\\baidu.htm", "rb");
	if file then
		local text = file:read("*all");
		file:close();
		_rich:SetText(text);
	else
		print("TextView load path:", path);
	end

  local box2 = luaui2.BoxLayout:new();
  box2:SetHorizontal();
  box2:Append(_rich);
  box2:Append(sb);
  splitter:SetSecondChild(box2);

	-- TODO 这里要不要实现一个 AsyncCall呢? 还是逻辑有问题?
	-- luaui2.TimerManager:SetOnceTimer(100, function()
		splitter:SetSize(150);
--		v_splitter:SetSize(200);
	-- end);
	
	local const = luaui2.const;
	local style = const.WS_OVERLAPPED;
	style = bit.bor(style, const.WS_CAPTION);
	style = bit.bor(style, const.WS_SYSMENU);
	style = bit.bor(style, const.WS_THICKFRAME);
	style = bit.bor(style, const.WS_MINIMIZEBOX);
	style = bit.bor(style, const.WS_MAXIMIZEBOX);
	style = bit.bor(style, const.WS_VISIBLE);
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
	
--	luaui2.TimerManager:SetOnceTimer(500, function()
--		DebugSpriteTree(spBackground);
--	end);
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

local function MemoryTest()
	print("MemoryTest begin");
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
	print("MemoryTest end");
end

print ("Path:GetAppData()", luaui2.Path:GetAppData())

MemoryTest();
-- ProFi:start();
InitMainUI();
--InitLoginUI();
--HttpTest();
--TimerTest();

luaui2.RunMessageLoop();
-- ProFi:stop();
-- ProFi:writeReport("ProFi.txt");
collectgarbage();
print('----');