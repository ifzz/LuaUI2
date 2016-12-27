
local threshold = 10 * 1024
local path = [[F:\TMP2\当年明月《明朝那些事儿》精校版.txt]]
local out_dir = [[F:\TMP3\]]
local out_name = "明朝那些事儿";
local out_ext = ".txt";

function get_out_path(num)
	local path = string.format("%s%s%03d%s", out_dir, out_name, num, out_ext);
	print (path);
	return path;
end

function main()
	local get_next_line = io.lines(path);

	local line = get_next_line();
	local current_size = 0;
	local current_num = 0;
	local current_file = io.open(get_out_path(current_num), "w+");
	while line do
		current_file:write(line, "\r\n");
		current_size = current_size + #line;
		if current_size > threshold then
			current_file:close();
			current_num = current_num + 1;
			current_file = io.open(get_out_path(current_num), "w+");
			current_size = 0;
		end
		line = get_next_line();
	end
	current_file:close();
end

main();